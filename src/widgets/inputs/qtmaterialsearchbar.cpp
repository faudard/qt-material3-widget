#include "qtmaterial/widgets/inputs/qtmaterialsearchbar.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QToolButton>

namespace QtMaterial {

class QtMaterialSearchBarPrivate final
{
public:
    QLineEdit* lineEdit = nullptr;
    QToolButton* clearButton = nullptr;
    bool clearButtonVisible = true;
};

QtMaterialSearchBar::QtMaterialSearchBar(QWidget* parent)
    : QWidget(parent)
    , d_ptr(std::make_unique<QtMaterialSearchBarPrivate>())
{
    d_ptr->lineEdit = new QLineEdit(this);
    d_ptr->clearButton = new QToolButton(this);
    setObjectName(QStringLiteral("qtmaterial_search_bar"));
    setFocusProxy(d_ptr->lineEdit);
    setAccessibleName(tr("Search"));

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 4, 8, 4);
    layout->setSpacing(4);

    d_ptr->lineEdit->setFrame(false);
    d_ptr->lineEdit->setClearButtonEnabled(false);
    d_ptr->lineEdit->setAccessibleName(tr("Search query"));

    d_ptr->clearButton->setText(QString::fromUtf8("\xC3\x97"));
    d_ptr->clearButton->setAutoRaise(true);
    d_ptr->clearButton->setFocusPolicy(Qt::StrongFocus);
    d_ptr->clearButton->setAccessibleName(tr("Clear search"));

    layout->addWidget(d_ptr->lineEdit, 1);
    layout->addWidget(d_ptr->clearButton);

    connect(d_ptr->lineEdit, &QLineEdit::textChanged, this, [this](const QString& value) {
        syncClearButton();
        emit textChanged(value);
    });
    connect(d_ptr->lineEdit, &QLineEdit::returnPressed, this, [this]() {
        emit searchRequested(d_ptr->lineEdit->text());
    });
    connect(d_ptr->clearButton, &QToolButton::clicked, this, [this]() {
        if (!d_ptr->lineEdit->text().isEmpty()) {
            d_ptr->lineEdit->clear();
        }
        d_ptr->lineEdit->setFocus(Qt::ShortcutFocusReason);
        emit cleared();
    });

    syncClearButton();
}

QtMaterialSearchBar::~QtMaterialSearchBar() = default;

QString QtMaterialSearchBar::text() const { return d_ptr->lineEdit->text(); }
void QtMaterialSearchBar::setText(const QString& text) { d_ptr->lineEdit->setText(text); }
QString QtMaterialSearchBar::placeholderText() const { return d_ptr->lineEdit->placeholderText(); }
void QtMaterialSearchBar::setPlaceholderText(const QString& text) { d_ptr->lineEdit->setPlaceholderText(text); }
bool QtMaterialSearchBar::isClearButtonVisible() const noexcept { return d_ptr->clearButtonVisible; }

void QtMaterialSearchBar::setClearButtonVisible(bool visible)
{
    if (d_ptr->clearButtonVisible == visible) {
        return;
    }
    d_ptr->clearButtonVisible = visible;
    syncClearButton();
}

QLineEdit* QtMaterialSearchBar::lineEdit() const noexcept { return d_ptr->lineEdit; }

QSize QtMaterialSearchBar::sizeHint() const
{
    const QSize editHint = d_ptr->lineEdit->sizeHint();
    return QSize(qMax(240, editHint.width() + 56), qMax(48, editHint.height() + 8));
}

QSize QtMaterialSearchBar::minimumSizeHint() const
{
    return QSize(120, 48);
}

void QtMaterialSearchBar::syncClearButton()
{
    d_ptr->clearButton->setVisible(d_ptr->clearButtonVisible && !d_ptr->lineEdit->text().isEmpty());
}

} // namespace QtMaterial
