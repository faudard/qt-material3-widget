#include "qtmaterial/widgets/buttons/qtmaterialbuttongroup.h"

#include <QBoxLayout>
#include <QEvent>
#include <QKeyEvent>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

namespace QtMaterial {

QtMaterialButtonGroup::QtMaterialButtonGroup(QWidget* parent)
    : QtMaterialWidget(parent)
    , m_layout(new QBoxLayout(QBoxLayout::LeftToRight, this))
{
    setMaterialComponent(QStringLiteral("ButtonGroup"));
    setFocusPolicy(Qt::NoFocus);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(2);
}

QtMaterialButtonGroup::~QtMaterialButtonGroup()
{
    // QWidget destroys child buttons after derived members have been torn down.
    // Disconnect callbacks now so QObject::destroyed cannot access m_buttons
    // once its storage has already been released.
    for (QtMaterialTextButton* button : m_buttons) {
        if (!button) {
            continue;
        }
        button->removeEventFilter(this);
        QObject::disconnect(button, nullptr, this, nullptr);
    }
    m_buttons.clear();
}

int QtMaterialButtonGroup::count() const noexcept
{
    return m_buttons.size();
}

QtMaterialTextButton* QtMaterialButtonGroup::buttonAt(int index) const
{
    return index >= 0 && index < m_buttons.size()
        ? m_buttons.at(index)
        : nullptr;
}

QtMaterialFilledButton* QtMaterialButtonGroup::addButton(const QString& text)
{
    auto* button = new QtMaterialFilledButton(text, this);
    addButton(button);
    return button;
}

void QtMaterialButtonGroup::addButton(QtMaterialTextButton* button)
{
    if (!button || m_buttons.contains(button)) {
        return;
    }

    button->setParent(this);
    button->installEventFilter(this);
    m_layout->addWidget(button);
    m_buttons.append(button);

    connect(button, &QAbstractButton::clicked, this, [this, button]() {
        const int index = m_buttons.indexOf(button);
        if (index < 0) {
            return;
        }
        if (m_exclusive) {
            setCurrentIndex(index);
        }
        Q_EMIT buttonTriggered(index);
    });

    connect(button, &QObject::destroyed, this, [this, button]() {
        const int index = m_buttons.indexOf(button);
        if (index < 0) {
            return;
        }
        m_buttons.remove(index);
        if (m_currentIndex == index) {
            m_currentIndex = -1;
            Q_EMIT currentIndexChanged(-1);
        } else if (m_currentIndex > index) {
            --m_currentIndex;
            Q_EMIT currentIndexChanged(m_currentIndex);
        }
        syncButtons();
    });

    if (m_currentIndex < 0 && button->isEnabled()) {
        m_currentIndex = m_buttons.size() - 1;
    }

    syncButtons();
    updateGeometry();
}

void QtMaterialButtonGroup::removeButton(QtMaterialTextButton* button)
{
    const int index = m_buttons.indexOf(button);
    if (index < 0) {
        return;
    }

    m_buttons.remove(index);
    m_layout->removeWidget(button);
    button->removeEventFilter(this);
    QObject::disconnect(button, nullptr, this, nullptr);

    if (m_currentIndex == index) {
        m_currentIndex = -1;
        const int replacement = nextEnabledIndex(index - 1, 1);
        if (replacement >= 0) {
            m_currentIndex = replacement;
        }
        Q_EMIT currentIndexChanged(m_currentIndex);
    } else if (m_currentIndex > index) {
        --m_currentIndex;
        Q_EMIT currentIndexChanged(m_currentIndex);
    }

    syncButtons();
    updateGeometry();
}

void QtMaterialButtonGroup::clear()
{
    while (!m_buttons.isEmpty()) {
        QtMaterialTextButton* button = m_buttons.takeLast();
        if (button) {
            button->removeEventFilter(this);
            m_layout->removeWidget(button);
            button->setParent(nullptr);
        }
    }

    if (m_currentIndex != -1) {
        m_currentIndex = -1;
        Q_EMIT currentIndexChanged(-1);
    }

    updateGeometry();
}

bool QtMaterialButtonGroup::isExclusive() const noexcept
{
    return m_exclusive;
}

void QtMaterialButtonGroup::setExclusive(bool exclusive)
{
    if (m_exclusive == exclusive) {
        return;
    }

    m_exclusive = exclusive;
    syncButtons();
    Q_EMIT exclusiveChanged(exclusive);
}

int QtMaterialButtonGroup::currentIndex() const noexcept
{
    return m_currentIndex;
}

void QtMaterialButtonGroup::setCurrentIndex(int index)
{
    if (index < -1 || index >= m_buttons.size()) {
        index = -1;
    }
    if (index >= 0 && !m_buttons.at(index)->isEnabled()) {
        return;
    }
    if (m_currentIndex == index) {
        return;
    }

    m_currentIndex = index;
    syncButtons();
    Q_EMIT currentIndexChanged(index);
}

int QtMaterialButtonGroup::spacing() const noexcept
{
    return m_layout->spacing();
}

void QtMaterialButtonGroup::setSpacing(int spacing)
{
    spacing = qMax(0, spacing);
    if (m_layout->spacing() == spacing) {
        return;
    }
    m_layout->setSpacing(spacing);
    Q_EMIT spacingChanged(spacing);
}

bool QtMaterialButtonGroup::expressive() const noexcept
{
    return m_expressive;
}

void QtMaterialButtonGroup::setExpressive(bool expressive)
{
    if (m_expressive == expressive) {
        return;
    }

    m_expressive = expressive;
    syncButtons();
    Q_EMIT expressiveChanged(expressive);
}

bool QtMaterialButtonGroup::eventFilter(QObject* watched, QEvent* event)
{
    if (!event || event->type() != QEvent::KeyPress) {
        return QtMaterialWidget::eventFilter(watched, event);
    }

    const int current = indexOf(watched);
    if (current < 0) {
        return QtMaterialWidget::eventFilter(watched, event);
    }

    auto* keyEvent = static_cast<QKeyEvent*>(event);
    int step = 0;
    switch (keyEvent->key()) {
    case Qt::Key_Left:
        step = layoutDirection() == Qt::RightToLeft ? 1 : -1;
        break;
    case Qt::Key_Right:
        step = layoutDirection() == Qt::RightToLeft ? -1 : 1;
        break;
    case Qt::Key_Home:
        focusIndex(nextEnabledIndex(-1, 1));
        keyEvent->accept();
        return true;
    case Qt::Key_End:
        focusIndex(nextEnabledIndex(m_buttons.size(), -1));
        keyEvent->accept();
        return true;
    default:
        return QtMaterialWidget::eventFilter(watched, event);
    }

    focusIndex(nextEnabledIndex(current, step));
    keyEvent->accept();
    return true;
}

int QtMaterialButtonGroup::indexOf(const QObject* object) const noexcept
{
    for (int index = 0; index < m_buttons.size(); ++index) {
        if (m_buttons.at(index) == object) {
            return index;
        }
    }
    return -1;
}

int QtMaterialButtonGroup::nextEnabledIndex(int start, int step) const noexcept
{
    if (m_buttons.isEmpty() || step == 0) {
        return -1;
    }

    int index = start;
    for (int scanned = 0; scanned < m_buttons.size(); ++scanned) {
        index += step;
        if (index < 0) {
            index = m_buttons.size() - 1;
        } else if (index >= m_buttons.size()) {
            index = 0;
        }
        QtMaterialTextButton* button = m_buttons.at(index);
        if (button && button->isEnabled()) {
            return index;
        }
    }
    return -1;
}

void QtMaterialButtonGroup::syncButtons()
{
    for (int index = 0; index < m_buttons.size(); ++index) {
        QtMaterialTextButton* button = m_buttons.at(index);
        if (!button) {
            continue;
        }
        button->setExpressive(m_expressive);
        if (m_expressive) {
            button->setExpressiveSize(QtMaterialButtonSize::Small);
            button->setExpressiveShape(
                index == 0 || index == m_buttons.size() - 1
                    ? QtMaterialButtonShape::Round
                    : QtMaterialButtonShape::Square);
        }

        button->setCheckable(m_exclusive);
        if (m_exclusive) {
            button->setChecked(index == m_currentIndex);
        }
    }

    const QString summary = tr("%1 button group, %2 items")
        .arg(m_exclusive ? tr("Single-choice") : tr("Action"))
        .arg(m_buttons.size());
    setAccessibleName(tr("Button group"));
    setAccessibleDescription(summary);
}

void QtMaterialButtonGroup::focusIndex(int index)
{
    QtMaterialTextButton* button = buttonAt(index);
    if (button) {
        button->setFocus(Qt::TabFocusReason);
    }
}

} // namespace QtMaterial
