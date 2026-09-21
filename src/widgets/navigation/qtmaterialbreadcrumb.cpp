#include "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QToolButton>

namespace QtMaterial {

QtMaterialBreadcrumb::QtMaterialBreadcrumb(QWidget* parent)
    : QWidget(parent)
    , m_layout(new QHBoxLayout(this))
{
    setObjectName(QStringLiteral("QtMaterialBreadcrumb"));
    setAccessibleName(tr("Breadcrumb"));
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(4);
}

QStringList QtMaterialBreadcrumb::items() const
{
    return m_items;
}

void QtMaterialBreadcrumb::setItems(const QStringList& items)
{
    if (m_items == items) {
        return;
    }

    m_items = items;
    const int nextIndex = m_items.isEmpty() ? -1 : m_items.size() - 1;
    const bool indexChanged = m_currentIndex != nextIndex;
    m_currentIndex = nextIndex;
    rebuild();
    if (indexChanged) {
        Q_EMIT currentIndexChanged(m_currentIndex);
    }
}

void QtMaterialBreadcrumb::addItem(const QString& text)
{
    m_items.push_back(text);
    m_currentIndex = m_items.size() - 1;
    rebuild();
    Q_EMIT currentIndexChanged(m_currentIndex);
}

void QtMaterialBreadcrumb::clear()
{
    setItems({});
}

int QtMaterialBreadcrumb::currentIndex() const noexcept
{
    return m_currentIndex;
}

void QtMaterialBreadcrumb::setCurrentIndex(int index)
{
    if (m_items.isEmpty()) {
        index = -1;
    } else {
        index = qBound(0, index, m_items.size() - 1);
    }

    if (m_currentIndex == index) {
        return;
    }
    m_currentIndex = index;
    rebuild();
    Q_EMIT currentIndexChanged(m_currentIndex);
}

void QtMaterialBreadcrumb::rebuild()
{
    while (QLayoutItem* item = m_layout->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    for (int index = 0; index < m_items.size(); ++index) {
        if (index > 0) {
            auto* separator = new QLabel(QStringLiteral("›"), this);
            separator->setAccessibleName(tr("Separator"));
            m_layout->addWidget(separator);
        }

        auto* button = new QToolButton(this);
        button->setText(m_items.at(index));
        button->setAutoRaise(true);
        button->setEnabled(index != m_currentIndex);
        button->setAccessibleName(m_items.at(index));
        connect(button, &QToolButton::clicked, this, [this, index]() {
            setCurrentIndex(index);
            Q_EMIT activated(index, m_items.value(index));
        });
        m_layout->addWidget(button);
    }

    m_layout->addStretch(1);
}

} // namespace QtMaterial
