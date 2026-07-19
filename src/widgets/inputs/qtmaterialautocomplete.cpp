#include "qtmaterial/widgets/inputs/qtmaterialautocomplete.h"

#include <QAbstractItemModel>
#include <QEvent>
#include <QKeyEvent>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>
#include <QStringListModel>
#include <QVBoxLayout>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/widgets/inputs/qtmaterialautocompletepopup.h"
#include <memory>
#include "qtmaterial/specs/qtmaterialautocompletespecresolver.h"

namespace QtMaterial {
struct QtMaterialAutocompletePrivate {

    explicit QtMaterialAutocompletePrivate(QtMaterialAutocomplete* q)
        : m_lineEdit(new QLineEdit(q))
        , m_popup(new ::QtMaterialAutocompletePopup(q))
        , m_ownedStringModel(new QStringListModel(q))
    {}

    QLineEdit* m_lineEdit = nullptr;
    ::QtMaterialAutocompletePopup* m_popup = nullptr;
    QStringListModel* m_ownedStringModel = nullptr;
    int m_maxVisibleCompletions = 8;
    bool m_completesOnReturn = true;
    bool m_opensOnFocus = true;
    bool m_hidePopupOnEscape = true;
    QString m_lastAccessibilitySummary;
    mutable bool m_specDirty = true;
    mutable AutocompleteSpec m_spec;
};

namespace {

// Tranche 31: local helpers formerly declared as private QtMaterialAutocomplete methods.
// Keeping them local removes implementation-only declarations from the public header
// without changing the public/protected API surface of QtMaterialAutocomplete.
void invalidateResolvedSpec(QtMaterialAutocompletePrivate* d)
{
    d->m_specDirty = true;
}

void ensureSpecResolved(QtMaterialAutocompletePrivate* d, const Theme& theme)
{
    if (!d->m_specDirty) {
        return;
    }

    d->m_spec = AutocompleteSpecResolver().autocompleteSpec(theme);
    d->m_specDirty = false;
}

void updateChildGeometry(QtMaterialAutocomplete* q,
                         QtMaterialAutocompletePrivate* d,
                         const Theme& theme)
{
    ensureSpecResolved(d, theme);
    d->m_lineEdit->setGeometry(
        q->rect().adjusted(
            d->m_spec.horizontalPadding,
            d->m_spec.verticalInset,
            -d->m_spec.horizontalPadding,
            -d->m_spec.verticalInset));
}

void updatePaletteFromSpec(QtMaterialAutocompletePrivate* d, const Theme& theme)
{
    ensureSpecResolved(d, theme);

    QPalette palette = d->m_lineEdit->palette();
    palette.setColor(QPalette::Base, Qt::transparent);
    palette.setColor(QPalette::Text, d->m_spec.inputTextColor);
    palette.setColor(QPalette::PlaceholderText, d->m_spec.placeholderColor);
    d->m_lineEdit->setPalette(palette);
    if (d->m_spec.hasResolvedInputFont) {
        d->m_lineEdit->setFont(d->m_spec.inputFont);
    }
}

void updateFilterText(QtMaterialAutocomplete* q, QtMaterialAutocompletePrivate* d)
{
    d->m_popup->setFilterText(d->m_lineEdit->text());
    q->setPopupVisible(!d->m_lineEdit->text().isEmpty());
}

void updateAccessibilityState(QtMaterialAutocomplete* q, QtMaterialAutocompletePrivate* d)
{
    if (q->accessibleName().isEmpty()) {
        const QString fallback = q->placeholderText().isEmpty()
            ? QtMaterialAutocomplete::tr("Autocomplete")
            : q->placeholderText();
        q->setAccessibleName(fallback);
    }

    const QString summary = q->accessibilitySummary();
    q->setAccessibleDescription(summary);

    if (d->m_lineEdit) {
        d->m_lineEdit->setAccessibleName(q->accessibleName());
        d->m_lineEdit->setAccessibleDescription(summary);
    }

    if (summary != d->m_lastAccessibilitySummary) {
        d->m_lastAccessibilitySummary = summary;
        emit q->accessibilitySummaryChanged(summary);
    }
}

// End tranche 31 local helpers.

} // namespace

QtMaterialAutocomplete::QtMaterialAutocomplete(QWidget* parent)
    : QtMaterialWidget(parent)
    , d_ptr(std::make_unique<QtMaterialAutocompletePrivate>(this))
{
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    d_ptr->m_lineEdit->setFrame(false);
    d_ptr->m_lineEdit->installEventFilter(this);
    d_ptr->m_popup->setAnchorLineEdit(d_ptr->m_lineEdit);
    d_ptr->m_popup->setModel(d_ptr->m_ownedStringModel);
    d_ptr->m_popup->hide();

    connect(d_ptr->m_lineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        updateFilterText(this, d_ptr.get());
        emit textChanged(text);
    });
    connect(d_ptr->m_popup, &::QtMaterialAutocompletePopup::completionActivated, this, [this](const QString& completion) {
        d_ptr->m_lineEdit->setText(completion);
        d_ptr->m_lineEdit->setCursorPosition(completion.size());
        emit completionActivated(completion);
    });
    connect(d_ptr->m_popup, &::QtMaterialAutocompletePopup::popupVisibilityChanged,
            this, &QtMaterialAutocomplete::popupVisibilityChanged);
}

QtMaterialAutocomplete::~QtMaterialAutocomplete() = default;

QLineEdit* QtMaterialAutocomplete::lineEdit() const noexcept { return d_ptr->m_lineEdit; }

QString QtMaterialAutocomplete::text() const { return d_ptr->m_lineEdit->text(); }

void QtMaterialAutocomplete::setText(const QString& text) { d_ptr->m_lineEdit->setText(text); }

QString QtMaterialAutocomplete::placeholderText() const { return d_ptr->m_lineEdit->placeholderText(); }

void QtMaterialAutocomplete::setPlaceholderText(const QString& text) { d_ptr->m_lineEdit->setPlaceholderText(text); }

void QtMaterialAutocomplete::setSuggestions(const QStringList& suggestions)
{
    d_ptr->m_ownedStringModel->setStringList(suggestions);
    d_ptr->m_popup->setSuggestions(suggestions);
    updateFilterText(this, d_ptr.get());
}

void QtMaterialAutocomplete::setModel(QAbstractItemModel* model)
{
    d_ptr->m_popup->setModel(model ? model : d_ptr->m_ownedStringModel);
    updateFilterText(this, d_ptr.get());
}

QAbstractItemModel* QtMaterialAutocomplete::model() const noexcept
{
    return d_ptr->m_popup->model();
}

bool QtMaterialAutocomplete::isPopupVisible() const noexcept
{
    return d_ptr->m_popup->isPopupVisible();
}

void QtMaterialAutocomplete::setPopupVisible(bool visible)
{
    d_ptr->m_popup->setPopupVisible(visible);
}

void QtMaterialAutocomplete::themeChangedEvent(const Theme& theme)
{
    QtMaterialWidget::themeChangedEvent(theme);
    invalidateResolvedSpec(d_ptr.get());
    updatePaletteFromSpec(d_ptr.get(), theme);
}

const AutocompleteSpec& QtMaterialAutocomplete::resolvedSpec() const
{
    ensureSpecResolved(d_ptr.get(), theme());
    return d_ptr->m_spec;
}

QSize QtMaterialAutocomplete::sizeHint() const
{
    ensureSpecResolved(d_ptr.get(), theme());
    return QSize(280, d_ptr->m_spec.inputMinHeight);
}

QSize QtMaterialAutocomplete::minimumSizeHint() const
{
    ensureSpecResolved(d_ptr.get(), theme());
    return QSize(160, d_ptr->m_spec.inputMinHeight);
}

bool QtMaterialAutocomplete::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == d_ptr->m_lineEdit) {
        if (event->type() == QEvent::KeyPress) {
            auto* key = static_cast<QKeyEvent*>(event);
            if (key->key() == Qt::Key_Down && isPopupVisible()) {
                d_ptr->m_popup->selectNext();
                key->accept();
                return true;
            }
            if (key->key() == Qt::Key_Up && isPopupVisible()) {
                d_ptr->m_popup->selectPrevious();
                key->accept();
                return true;
            }
            if ((key->key() == Qt::Key_Return || key->key() == Qt::Key_Enter) && isPopupVisible()) {
                d_ptr->m_popup->acceptCurrent();
                key->accept();
                return true;
            }
            if (key->key() == Qt::Key_Escape && isPopupVisible()) {
                setPopupVisible(false);
                key->accept();
                return true;
            }
        }
        if (event->type() == QEvent::FocusIn) {
            updateFilterText(this, d_ptr.get());
        }
        if (event->type() == QEvent::FocusOut) {
            // Popup owns its own focus while visible. Hide only when the popup is not active.
            if (!d_ptr->m_popup->isActiveWindow()) {
                setPopupVisible(false);
            }
        }
    }
    return QtMaterialWidget::eventFilter(watched, event);
}

void QtMaterialAutocomplete::resizeEvent(QResizeEvent* event)
{
    QtMaterialWidget::resizeEvent(event);
    updateChildGeometry(this, d_ptr.get(), theme());
}

void QtMaterialAutocomplete::paintEvent(QPaintEvent*)
{
    ensureSpecResolved(d_ptr.get(), theme());
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const qreal radius = d_ptr->m_spec.inputCornerRadius;
    QPainterPath path;
    path.addRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    painter.fillPath(path, d_ptr->m_spec.inputContainerColor);
    painter.setPen(QPen(
        d_ptr->m_lineEdit->hasFocus()
            ? d_ptr->m_spec.focusedOutlineColor
            : d_ptr->m_spec.outlineColor,
        d_ptr->m_lineEdit->hasFocus()
            ? d_ptr->m_spec.focusedOutlineWidth
            : d_ptr->m_spec.outlineWidth));
    painter.drawPath(path);

    if (d_ptr->m_lineEdit->hasFocus()) {
        QtMaterialFocusIndicator::paintPathFocusRing(&painter, path, d_ptr->m_spec.focusRingColor,
            d_ptr->m_spec.focusRingWidth);
    }
}

int QtMaterialAutocomplete::maxVisibleCompletions() const noexcept {
    return d_ptr->m_maxVisibleCompletions;
}

void QtMaterialAutocomplete::setMaxVisibleCompletions(int count) {
    const int sanitized = qMax(1, count);
    if (d_ptr->m_maxVisibleCompletions == sanitized) {
        return;
    }
    d_ptr->m_maxVisibleCompletions = sanitized;
    if (d_ptr->m_popup) {
        d_ptr->m_popup->setMaximumHeight(sizeHint().height() * d_ptr->m_maxVisibleCompletions);
    }
    updateAccessibilityState(this, d_ptr.get());
}

bool QtMaterialAutocomplete::completesOnReturn() const noexcept {
    return d_ptr->m_completesOnReturn;
}

void QtMaterialAutocomplete::setCompletesOnReturn(bool enabled) {
    d_ptr->m_completesOnReturn = enabled;
}

bool QtMaterialAutocomplete::opensOnFocus() const noexcept {
    return d_ptr->m_opensOnFocus;
}

void QtMaterialAutocomplete::setOpensOnFocus(bool enabled) {
    if (d_ptr->m_opensOnFocus == enabled) {
        return;
    }
    d_ptr->m_opensOnFocus = enabled;
    if (!d_ptr->m_opensOnFocus && !d_ptr->m_lineEdit->hasFocus()) {
        setPopupVisible(false);
    }
}

bool QtMaterialAutocomplete::hidePopupOnEscape() const noexcept {
    return d_ptr->m_hidePopupOnEscape;
}

void QtMaterialAutocomplete::setHidePopupOnEscape(bool enabled) {
    d_ptr->m_hidePopupOnEscape = enabled;
}

QString QtMaterialAutocomplete::currentCompletion() const {
    return d_ptr->m_popup ? d_ptr->m_popup->currentCompletion() : QString();
}

QString QtMaterialAutocomplete::accessibilitySummary() const {
    QStringList parts;
    const QString name = accessibleName().trimmed();
    if (!name.isEmpty()) {
        parts << name;
    } else if (!placeholderText().trimmed().isEmpty()) {
        parts << placeholderText().trimmed();
    } else {
        parts << QStringLiteral("Autocomplete");
    }
    if (!text().isEmpty()) {
        parts << tr("Current text: %1").arg(text());
    }
    if (isPopupVisible()) {
        const QString completion = currentCompletion();
        if (!completion.isEmpty()) {
            parts << tr("Suggestion: %1").arg(completion);
        } else {
            parts << tr("Suggestions available");
        }
    }
    return parts.join(QStringLiteral(". "));
}

} // namespace QtMaterial
