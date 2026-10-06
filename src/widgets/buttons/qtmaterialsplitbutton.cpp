#include "qtmaterial/widgets/buttons/qtmaterialsplitbutton.h"

#include <QHBoxLayout>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

namespace QtMaterial {

QtMaterialSplitButton::QtMaterialSplitButton(QWidget* parent)
    : QtMaterialWidget(parent)
    , m_primary(new QtMaterialFilledButton(this))
    , m_trailing(new QtMaterialFilledButton(QStringLiteral("\u25BE"), this))
{
    setMaterialComponent(QStringLiteral("SplitButton"));
    setFocusPolicy(Qt::NoFocus);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    layout->addWidget(m_primary);
    layout->addWidget(m_trailing);

    m_trailing->setAccessibleName(tr("More actions"));
    m_trailing->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    connect(m_primary, &QAbstractButton::clicked,
            this, &QtMaterialSplitButton::primaryTriggered);
    connect(m_trailing, &QAbstractButton::clicked,
            this, &QtMaterialSplitButton::secondaryTriggered);

    syncButtons();
}

QtMaterialSplitButton::QtMaterialSplitButton(
    const QString& text,
    QWidget* parent)
    : QtMaterialSplitButton(parent)
{
    setText(text);
}

QtMaterialSplitButton::~QtMaterialSplitButton() = default;

QString QtMaterialSplitButton::text() const
{
    return m_primary->text();
}

void QtMaterialSplitButton::setText(const QString& text)
{
    if (m_primary->text() == text) {
        return;
    }
    m_primary->setText(text);
    setAccessibleName(text.isEmpty() ? tr("Split button") : text);
    Q_EMIT textChanged(text);
}

QIcon QtMaterialSplitButton::icon() const
{
    return m_primary->icon();
}

void QtMaterialSplitButton::setIcon(const QIcon& icon)
{
    m_primary->setIcon(icon);
}

bool QtMaterialSplitButton::expressive() const noexcept
{
    return m_expressive;
}

void QtMaterialSplitButton::setExpressive(bool expressive)
{
    if (m_expressive == expressive) {
        return;
    }
    m_expressive = expressive;
    syncButtons();
    Q_EMIT expressiveChanged(expressive);
}

QtMaterialButtonSize QtMaterialSplitButton::expressiveSize() const noexcept
{
    return m_size;
}

void QtMaterialSplitButton::setExpressiveSize(QtMaterialButtonSize size)
{
    if (m_size == size) {
        return;
    }
    m_size = size;
    syncButtons();
    Q_EMIT expressiveSizeChanged(size);
}

QtMaterialFilledButton* QtMaterialSplitButton::primaryButton() const noexcept
{
    return m_primary;
}

QtMaterialFilledButton* QtMaterialSplitButton::trailingButton() const noexcept
{
    return m_trailing;
}

void QtMaterialSplitButton::syncButtons()
{
    m_primary->setExpressive(m_expressive);
    m_trailing->setExpressive(m_expressive);
    if (m_expressive) {
        m_primary->setExpressiveSize(m_size);
        m_trailing->setExpressiveSize(m_size);
        m_primary->setExpressiveShape(QtMaterialButtonShape::Round);
        m_trailing->setExpressiveShape(QtMaterialButtonShape::Round);
    }

    const int trailingWidth = qMax(48, m_trailing->sizeHint().height());
    m_trailing->setFixedWidth(trailingWidth);
    setAccessibleDescription(
        tr("Primary action with a separate more-actions button"));
}

} // namespace QtMaterial
