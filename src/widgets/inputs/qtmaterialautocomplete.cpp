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
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/widgets/inputs/qtmaterialautocompletepopup.h"
#include <memory>

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
        updateFilterText();
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
    updateFilterText();
}

void QtMaterialAutocomplete::setModel(QAbstractItemModel* model)
{
    d_ptr->m_popup->setModel(model ? model : d_ptr->m_ownedStringModel);
    updateFilterText();
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
    invalidateResolvedSpec();
    updatePaletteFromSpec();
}

void QtMaterialAutocomplete::invalidateResolvedSpec()
{
    d_ptr->m_specDirty = true;
}

void QtMaterialAutocomplete::ensureSpecResolved() const
{
    if (!d_ptr->m_specDirty) {
        return;
    }
    SpecFactory factory;
    d_ptr->m_spec = factory.autocompleteSpec(theme());
    d_ptr->m_specDirty = false;
}

void QtMaterialAutocomplete::updateChildGeometry()
{
    ensureSpecResolved();
    const int margin = 16;
    d_ptr->m_lineEdit->setGeometry(rect().adjusted(margin, 4, -margin, -4));
}

void QtMaterialAutocomplete::updatePaletteFromSpec()
{
    ensureSpecResolved();
    QPalette palette = d_ptr->m_lineEdit->palette();
    palette.setColor(QPalette::Base, Qt::transparent);
    palette.setColor(QPalette::Text, d_ptr->m_spec.inputTextColor);
    palette.setColor(QPalette::PlaceholderText, d_ptr->m_spec.placeholderColor);
    d_ptr->m_lineEdit->setPalette(palette);
}

void QtMaterialAutocomplete::updateFilterText()
{
    d_ptr->m_popup->setFilterText(d_ptr->m_lineEdit->text());
    setPopupVisible(!d_ptr->m_lineEdit->text().isEmpty());
}

QSize QtMaterialAutocomplete::sizeHint() const
{
    ensureSpecResolved();
    return QSize(280, d_ptr->m_spec.inputMinHeight);
}

QSize QtMaterialAutocomplete::minimumSizeHint() const
{
    ensureSpecResolved();
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
            updateFilterText();
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
    updateChildGeometry();
}

void QtMaterialAutocomplete::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const qreal radius = theme().shapes().contains(d_ptr->m_spec.inputShapeRole)
        ? theme().shapes().radius(d_ptr->m_spec.inputShapeRole)
        : 4.0;
    QPainterPath path;
    path.addRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    painter.fillPath(path, d_ptr->m_spec.inputContainerColor);
    painter.setPen(QPen(d_ptr->m_lineEdit->hasFocus() ? d_ptr->m_spec.focusedOutlineColor : d_ptr->m_spec.outlineColor, d_ptr->m_lineEdit->hasFocus() ? 2 : 1));
    painter.drawPath(path);

    if (d_ptr->m_lineEdit->hasFocus()) {
        QtMaterialFocusIndicator::paintPathFocusRing(&painter, path, d_ptr->m_spec.focusRingColor, 2.0);
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
    updateAccessibilityState();
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

void QtMaterialAutocomplete::updateAccessibilityState() {
    if (accessibleName().isEmpty()) {
        const QString fallback = placeholderText().isEmpty() ? tr("Autocomplete") : placeholderText();
        setAccessibleName(fallback);
    }
    const QString summary = accessibilitySummary();
    setAccessibleDescription(summary);
    if (d_ptr->m_lineEdit) {
        d_ptr->m_lineEdit->setAccessibleName(accessibleName());
        d_ptr->m_lineEdit->setAccessibleDescription(summary);
    }
    if (summary != d_ptr->m_lastAccessibilitySummary) {
        d_ptr->m_lastAccessibilitySummary = summary;
        emit accessibilitySummaryChanged(summary);
    }
}

} // namespace QtMaterial
