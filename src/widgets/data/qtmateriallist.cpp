#include "qtmaterial/widgets/data/qtmateriallist.h"

#include "qtmaterial/widgets/data/qtmateriallistitem.h"

#include <QEvent>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <memory>

namespace QtMaterial {

struct QtMaterialListPrivate
{
    QVector<QtMaterialListItem*> m_items;
    QVBoxLayout* m_layout = nullptr;
    QtMaterialList::SelectionMode m_selectionMode = QtMaterialList::SelectionMode::SingleSelection;
    int m_currentIndex = -1;
    QString m_lastAccessibilitySummary;
};



namespace {
QString defaultListAccessibleName()
{
    return QStringLiteral("List");
}
} // namespace

QtMaterialList::QtMaterialList(QWidget* parent)
    : QWidget(parent)
    , d_ptr(std::make_unique<QtMaterialListPrivate>())
{
    d_ptr->m_layout = new QVBoxLayout(this);

    setFocusPolicy(Qt::StrongFocus);
    setAccessibleName(defaultListAccessibleName());

    d_ptr->m_layout->setContentsMargins(0, 0, 0, 0);
    d_ptr->m_layout->setSpacing(0);

    syncAccessibility();
}

QtMaterialList::~QtMaterialList() = default;

int QtMaterialList::count() const noexcept
{
    return d_ptr->m_items.size();
}

bool QtMaterialList::isEmpty() const noexcept
{
    return d_ptr->m_items.isEmpty();
}

QtMaterialListItem* QtMaterialList::itemAt(int index) const
{
    return isValidIndex(index) ? d_ptr->m_items.at(index) : nullptr;
}

int QtMaterialList::indexOf(const QtMaterialListItem* item) const noexcept
{
    for (int i = 0; i < d_ptr->m_items.size(); ++i) {
        if (d_ptr->m_items.at(i) == item) {
            return i;
        }
    }
    return -1;
}

void QtMaterialList::addItem(QtMaterialListItem* item)
{
    insertItem(d_ptr->m_items.size(), item);
}

QtMaterialListItem* QtMaterialList::addItem(const QString& headline)
{
    auto* item = new QtMaterialListItem(headline, this);
    addItem(item);
    return item;
}

void QtMaterialList::insertItem(int index, QtMaterialListItem* item)
{
    if (!item || d_ptr->m_items.contains(item)) {
        return;
    }

    index = qBound(0, index, d_ptr->m_items.size());
    item->setParent(this);
    initialiseItem(item);
    d_ptr->m_items.insert(index, item);
    d_ptr->m_layout->insertWidget(index, item);

    if (d_ptr->m_currentIndex < 0 && item->isEnabled()) {
        d_ptr->m_currentIndex = index;
        emit currentIndexChanged(d_ptr->m_currentIndex);
    } else if (d_ptr->m_currentIndex >= index) {
        ++d_ptr->m_currentIndex;
        emit currentIndexChanged(d_ptr->m_currentIndex);
    }

    syncItemSelection();
    syncAccessibility();
}

QtMaterialListItem* QtMaterialList::takeItem(int index)
{
    if (!isValidIndex(index)) {
        return nullptr;
    }

    auto* item = d_ptr->m_items.takeAt(index);
    d_ptr->m_layout->removeWidget(item);
    item->removeEventFilter(this);
    item->disconnect(this);
    item->setParent(nullptr);

    if (d_ptr->m_items.isEmpty()) {
        if (d_ptr->m_currentIndex != -1) {
            d_ptr->m_currentIndex = -1;
            emit currentIndexChanged(d_ptr->m_currentIndex);
        }
    } else if (d_ptr->m_currentIndex == index) {
        const int next = nextEnabledIndex(qMin(index, d_ptr->m_items.size() - 1), 1);
        const int fallback = next >= 0 ? next : lastEnabledIndex();
        if (d_ptr->m_currentIndex != fallback) {
            d_ptr->m_currentIndex = fallback;
            emit currentIndexChanged(d_ptr->m_currentIndex);
        }
    } else if (d_ptr->m_currentIndex > index) {
        --d_ptr->m_currentIndex;
        emit currentIndexChanged(d_ptr->m_currentIndex);
    }

    syncItemSelection();
    syncAccessibility();
    return item;
}

void QtMaterialList::removeItem(int index)
{
    if (auto* item = takeItem(index)) {
        item->deleteLater();
    }
}

void QtMaterialList::clear()
{
    const bool hadItems = !d_ptr->m_items.isEmpty();
    while (!d_ptr->m_items.isEmpty()) {
        removeItem(d_ptr->m_items.size() - 1);
    }

    if (hadItems) {
        emit selectionChanged();
        syncAccessibility();
    }
}

int QtMaterialList::currentIndex() const noexcept
{
    return d_ptr->m_currentIndex;
}

void QtMaterialList::setCurrentIndex(int index)
{
    if (index < 0) {
        if (d_ptr->m_currentIndex == -1) {
            return;
        }
        d_ptr->m_currentIndex = -1;
        syncItemSelection();
        emit currentIndexChanged(d_ptr->m_currentIndex);
        syncAccessibility();
        return;
    }

    if (!isValidIndex(index) || !d_ptr->m_items.at(index)->isEnabled()) {
        return;
    }

    if (d_ptr->m_currentIndex == index) {
        return;
    }

    d_ptr->m_currentIndex = index;
    syncItemSelection();
    emit currentIndexChanged(d_ptr->m_currentIndex);
    syncAccessibility();
}

QtMaterialList::SelectionMode QtMaterialList::selectionMode() const noexcept
{
    return d_ptr->m_selectionMode;
}

void QtMaterialList::setSelectionMode(SelectionMode mode)
{
    if (d_ptr->m_selectionMode == mode) {
        return;
    }

    d_ptr->m_selectionMode = mode;
    syncItemSelection();
    emit selectionModeChanged(d_ptr->m_selectionMode);
    emit selectionChanged();
    syncAccessibility();
}

QList<int> QtMaterialList::selectedIndexes() const
{
    QList<int> indexes;
    for (int i = 0; i < d_ptr->m_items.size(); ++i) {
        if (d_ptr->m_items.at(i)->isSelected()) {
            indexes.append(i);
        }
    }
    return indexes;
}

void QtMaterialList::clearSelection()
{
    bool changed = false;
    for (auto* item : d_ptr->m_items) {
        if (item->isSelected()) {
            item->setSelected(false);
            changed = true;
        }
    }

    if (changed) {
        emit selectionChanged();
        syncAccessibility();
    }
}

bool QtMaterialList::isItemEnabled(int index) const
{
    return isValidIndex(index) && d_ptr->m_items.at(index)->isEnabled();
}

void QtMaterialList::setItemEnabled(int index, bool enabled)
{
    if (!isValidIndex(index) || d_ptr->m_items.at(index)->isEnabled() == enabled) {
        return;
    }

    d_ptr->m_items.at(index)->setEnabled(enabled);

    if (!enabled && d_ptr->m_currentIndex == index) {
        const int next = nextEnabledIndex(index + 1, 1);
        const int previous = next >= 0 ? next : nextEnabledIndex(index - 1, -1);
        setCurrentIndex(previous);
    }

    syncAccessibility();
}

QString QtMaterialList::itemAccessibleText(int index) const
{
    const auto* item = itemAt(index);
    if (!item) {
        return QString();
    }

    QStringList parts;
    const QString headline = item->headlineText().trimmed();
    const QString supporting = item->supportingText().trimmed();
    parts << (headline.isEmpty() ? QStringLiteral("List item %1").arg(index + 1) : headline);

    if (!supporting.isEmpty()) {
        parts << supporting;
    }
    if (item->isSelected()) {
        parts << QStringLiteral("selected");
    }
    if (!item->isEnabled()) {
        parts << QStringLiteral("disabled");
    }

    return parts.join(QStringLiteral(", "));
}

QString QtMaterialList::accessibilitySummary() const
{
    const int total = d_ptr->m_items.size();
    const int selected = selectedIndexes().size();

    if (total == 0) {
        return QStringLiteral("Empty list");
    }

    QString summary = QStringLiteral("%1 item%2")
                          .arg(total)
                          .arg(total == 1 ? QString() : QStringLiteral("s"));

    if (d_ptr->m_currentIndex >= 0) {
        summary += QStringLiteral(", current %1 of %2").arg(d_ptr->m_currentIndex + 1).arg(total);
    }
    if (selected > 0) {
        summary += QStringLiteral(", %1 selected").arg(selected);
    }

    return summary;
}

bool QtMaterialList::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::KeyPress && indexOf(qobject_cast<QtMaterialListItem*>(watched)) >= 0) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        switch (keyEvent->key()) {
        case Qt::Key_Up:
        case Qt::Key_Left:
            keyEvent->accept();
            return moveCurrent(-1);
        case Qt::Key_Down:
        case Qt::Key_Right:
            keyEvent->accept();
            return moveCurrent(1);
        case Qt::Key_Home:
            keyEvent->accept();
            return moveToBoundary(true);
        case Qt::Key_End:
            keyEvent->accept();
            return moveToBoundary(false);
        default:
            break;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void QtMaterialList::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_Left:
        if (moveCurrent(-1)) {
            event->accept();
            return;
        }
        break;
    case Qt::Key_Down:
    case Qt::Key_Right:
        if (moveCurrent(1)) {
            event->accept();
            return;
        }
        break;
    case Qt::Key_Home:
        if (moveToBoundary(true)) {
            event->accept();
            return;
        }
        break;
    case Qt::Key_End:
        if (moveToBoundary(false)) {
            event->accept();
            return;
        }
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        if (activateIndex(d_ptr->m_currentIndex)) {
            event->accept();
            return;
        }
        break;
    default:
        break;
    }

    QWidget::keyPressEvent(event);
}

void QtMaterialList::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    syncAccessibility();
}

void QtMaterialList::initialiseItem(QtMaterialListItem* item)
{
    item->installEventFilter(this);

    connect(item, &QtMaterialListItem::clicked, this, [this, item]() {
        const int index = indexOf(item);
        if (index < 0 || !item->isEnabled()) {
            return;
        }
        emit itemClicked(index);
        activateIndex(index);
    });

    connect(item, &QtMaterialListItem::activated, this, [this, item]() {
        const int index = indexOf(item);
        if (index >= 0 && item->isEnabled()) {
            activateIndex(index);
        }
    });

    connect(item, &QtMaterialListItem::selectionChanged, this, [this]() {
        emit selectionChanged();
        syncAccessibility();
    });
}

void QtMaterialList::syncItemSelection()
{
    if (d_ptr->m_selectionMode == SelectionMode::NoSelection) {
        for (auto* item : d_ptr->m_items) {
            item->setSelected(false);
        }
        return;
    }

    if (d_ptr->m_selectionMode == SelectionMode::SingleSelection) {
        for (int i = 0; i < d_ptr->m_items.size(); ++i) {
            d_ptr->m_items.at(i)->setSelected(i == d_ptr->m_currentIndex);
        }
    }
}

void QtMaterialList::syncAccessibility()
{
    if (accessibleName().isEmpty()) {
        setAccessibleName(defaultListAccessibleName());
    }
    setAccessibleDescription(accessibilitySummary());
    emitAccessibilityIfChanged();
}

void QtMaterialList::emitAccessibilityIfChanged()
{
    const QString summary = accessibilitySummary();
    if (d_ptr->m_lastAccessibilitySummary == summary) {
        return;
    }
    d_ptr->m_lastAccessibilitySummary = summary;
    emit accessibilitySummaryChanged(summary);
}

bool QtMaterialList::activateIndex(int index)
{
    if (!isValidIndex(index) || !d_ptr->m_items.at(index)->isEnabled()) {
        return false;
    }

    setCurrentIndex(index);

    if (d_ptr->m_selectionMode == SelectionMode::MultiSelection) {
        d_ptr->m_items.at(index)->setSelected(!d_ptr->m_items.at(index)->isSelected());
    } else {
        syncItemSelection();
        emit selectionChanged();
    }

    emit itemActivated(index);
    syncAccessibility();
    return true;
}

bool QtMaterialList::moveCurrent(int delta)
{
    if (d_ptr->m_items.isEmpty()) {
        return false;
    }

    const int start = d_ptr->m_currentIndex < 0
        ? (delta >= 0 ? 0 : d_ptr->m_items.size() - 1)
        : d_ptr->m_currentIndex + delta;
    const int next = nextEnabledIndex(start, delta);

    if (next < 0 || next == d_ptr->m_currentIndex) {
        return false;
    }

    setCurrentIndex(next);
    if (auto* item = itemAt(next)) {
        item->setFocus(Qt::OtherFocusReason);
    }
    return true;
}

bool QtMaterialList::moveToBoundary(bool first)
{
    const int index = first ? firstEnabledIndex() : lastEnabledIndex();
    if (index < 0 || index == d_ptr->m_currentIndex) {
        return false;
    }

    setCurrentIndex(index);
    if (auto* item = itemAt(index)) {
        item->setFocus(Qt::OtherFocusReason);
    }
    return true;
}

int QtMaterialList::nextEnabledIndex(int start, int delta) const
{
    if (delta == 0 || d_ptr->m_items.isEmpty()) {
        return -1;
    }

    for (int i = start; i >= 0 && i < d_ptr->m_items.size(); i += delta) {
        if (d_ptr->m_items.at(i)->isEnabled()) {
            return i;
        }
    }
    return -1;
}

int QtMaterialList::firstEnabledIndex() const
{
    return nextEnabledIndex(0, 1);
}

int QtMaterialList::lastEnabledIndex() const
{
    return nextEnabledIndex(d_ptr->m_items.size() - 1, -1);
}

bool QtMaterialList::isValidIndex(int index) const noexcept
{
    return index >= 0 && index < d_ptr->m_items.size();
}

} // namespace QtMaterial
