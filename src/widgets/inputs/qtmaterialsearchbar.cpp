#include "qtmaterial/widgets/inputs/qtmaterialsearchbar.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>

namespace QtMaterial {

QtMaterialSearchBar::QtMaterialSearchBar(QWidget* parent)
    : QWidget(parent)
    , m_lineEdit(new QLineEdit(this))
    , m_clearButton(new QToolButton(this))
{
    setObjectName(QStringLiteral("qtmaterial_search_bar"));
    setFocusProxy(m_lineEdit);
    setAccessibleName(tr("Search"));

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 4, 8, 4);
    layout->setSpacing(4);

    m_lineEdit->setFrame(false);
    m_lineEdit->setClearButtonEnabled(false);
    m_lineEdit->setAccessibleName(tr("Search query"));

    m_clearButton->setText(QString::fromUtf8("\xC3\x97"));
    m_clearButton->setAutoRaise(true);
    m_clearButton->setFocusPolicy(Qt::StrongFocus);
    m_clearButton->setAccessibleName(tr("Clear search"));

    layout->addWidget(m_lineEdit, 1);
    layout->addWidget(m_clearButton);

    connect(m_lineEdit, &QLineEdit::textChanged, this, [this](const QString& value) {
        syncClearButton();
        emit textChanged(value);
    });
    connect(m_lineEdit, &QLineEdit::returnPressed, this, [this]() {
        emit searchRequested(m_lineEdit->text());
    });
    connect(m_clearButton, &QToolButton::clicked, this, [this]() {
        if (!m_lineEdit->text().isEmpty()) {
            m_lineEdit->clear();
        }
        m_lineEdit->setFocus(Qt::ShortcutFocusReason);
        emit cleared();
    });

    syncClearButton();
}

QtMaterialSearchBar::~QtMaterialSearchBar() = default;

QString QtMaterialSearchBar::text() const { return m_lineEdit->text(); }
void QtMaterialSearchBar::setText(const QString& text) { m_lineEdit->setText(text); }
QString QtMaterialSearchBar::placeholderText() const { return m_lineEdit->placeholderText(); }
void QtMaterialSearchBar::setPlaceholderText(const QString& text) { m_lineEdit->setPlaceholderText(text); }
bool QtMaterialSearchBar::isClearButtonVisible() const noexcept { return m_clearButtonVisible; }

void QtMaterialSearchBar::setClearButtonVisible(bool visible)
{
    if (m_clearButtonVisible == visible) {
        return;
    }
    m_clearButtonVisible = visible;
    syncClearButton();
}

QLineEdit* QtMaterialSearchBar::lineEdit() const noexcept { return m_lineEdit; }

QSize QtMaterialSearchBar::sizeHint() const
{
    const QSize editHint = m_lineEdit->sizeHint();
    return QSize(qMax(240, editHint.width() + 56), qMax(48, editHint.height() + 8));
}

QSize QtMaterialSearchBar::minimumSizeHint() const
{
    return QSize(120, 48);
}

void QtMaterialSearchBar::syncClearButton()
{
    m_clearButton->setVisible(m_clearButtonVisible && !m_lineEdit->text().isEmpty());
}

} // namespace QtMaterial
