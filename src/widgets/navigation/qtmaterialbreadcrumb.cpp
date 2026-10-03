#include "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QList>
#include <QToolButton>

namespace QtMaterial {

class QtMaterialBreadcrumbPrivate final
{
public:
    QStringList items;
    int currentIndex = -1;
    QHBoxLayout* layout = nullptr;
    QList<QToolButton*> buttons;
    QList<QLabel*> separators;
};

QtMaterialBreadcrumb::QtMaterialBreadcrumb(QWidget* parent)
    : QWidget(parent)
    , d_ptr(std::make_unique<QtMaterialBreadcrumbPrivate>())
{
    setObjectName(QStringLiteral("QtMaterialBreadcrumb"));
    setAccessibleName(tr("Breadcrumb"));
    d_ptr->layout = new QHBoxLayout(this);
    d_ptr->layout->setContentsMargins(0, 0, 0, 0);
    d_ptr->layout->setSpacing(4);
}

QtMaterialBreadcrumb::~QtMaterialBreadcrumb() = default;

QStringList QtMaterialBreadcrumb::items() const
{
    return d_ptr->items;
}

void QtMaterialBreadcrumb::setItems(const QStringList& items)
{
    if (d_ptr->items == items) {
        return;
    }

    d_ptr->items = items;
    const int nextIndex = d_ptr->items.isEmpty() ? -1 : d_ptr->items.size() - 1;
    const bool indexChanged = d_ptr->currentIndex != nextIndex;
    d_ptr->currentIndex = nextIndex;
    rebuild();
    if (indexChanged) {
        Q_EMIT currentIndexChanged(d_ptr->currentIndex);
    }
}

void QtMaterialBreadcrumb::addItem(const QString& text)
{
    d_ptr->items.push_back(text);
    d_ptr->currentIndex = d_ptr->items.size() - 1;
    rebuild();
    Q_EMIT currentIndexChanged(d_ptr->currentIndex);
}

void QtMaterialBreadcrumb::clear()
{
    setItems({});
}

int QtMaterialBreadcrumb::currentIndex() const noexcept
{
    return d_ptr->currentIndex;
}

void QtMaterialBreadcrumb::setCurrentIndex(int index)
{
    if (d_ptr->items.isEmpty()) {
        index = -1;
    } else {
        index = qBound(0, index, d_ptr->items.size() - 1);
    }

    if (d_ptr->currentIndex == index) {
        return;
    }

    d_ptr->currentIndex = index;
    refreshCurrentSegment();
    Q_EMIT currentIndexChanged(d_ptr->currentIndex);
}

void QtMaterialBreadcrumb::rebuild()
{
    d_ptr->buttons.clear();
    d_ptr->separators.clear();

    while (QLayoutItem* item = d_ptr->layout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    for (int index = 0; index < d_ptr->items.size(); ++index) {
        if (index > 0) {
            auto* separator = new QLabel(this);
            separator->setAccessibleName(tr("Breadcrumb separator"));
            separator->setFocusPolicy(Qt::NoFocus);
            d_ptr->separators.push_back(separator);
            d_ptr->layout->addWidget(separator);
        }

        const QString text = d_ptr->items.at(index);
        auto* button = new QToolButton(this);
        button->setText(text);
        button->setAutoRaise(true);
        button->setAccessibleName(text);
        connect(button, &QToolButton::clicked, this, [this, index, text]() {
            setCurrentIndex(index);
            Q_EMIT activated(index, text);
        });
        d_ptr->buttons.push_back(button);
        d_ptr->layout->addWidget(button);
    }

    refreshDirection();
    refreshCurrentSegment();
    d_ptr->layout->addStretch(1);
}

void QtMaterialBreadcrumb::refreshCurrentSegment()
{
    QStringList pathParts;
    for (int index = 0; index < d_ptr->buttons.size(); ++index) {
        QToolButton* button = d_ptr->buttons.at(index);
        const bool current = index == d_ptr->currentIndex;
        button->setEnabled(!current);
        button->setAccessibleDescription(
            current
                ? tr("%1 of %2, current location")
                      .arg(index + 1)
                      .arg(d_ptr->buttons.size())
                : tr("%1 of %2")
                      .arg(index + 1)
                      .arg(d_ptr->buttons.size()));
        pathParts.push_back(d_ptr->items.at(index));
    }

    setAccessibleDescription(
        pathParts.isEmpty()
            ? tr("Empty breadcrumb")
            : tr("Path: %1").arg(
                  pathParts.join(QStringLiteral(" / "))));
}

void QtMaterialBreadcrumb::refreshDirection()
{
    const QString separatorText =
        layoutDirection() == Qt::RightToLeft
            ? QStringLiteral("‹")
            : QStringLiteral("›");
    for (QLabel* separator : d_ptr->separators) {
        separator->setText(separatorText);
    }
}

void QtMaterialBreadcrumb::changeEvent(QEvent* event)
{
    QWidget::changeEvent(event);
    if (event
        && event->type() == QEvent::LayoutDirectionChange) {
        refreshDirection();
    }
}

} // namespace QtMaterial
