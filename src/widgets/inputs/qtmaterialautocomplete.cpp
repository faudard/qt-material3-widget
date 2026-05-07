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

namespace QtMaterial {

QtMaterialAutocomplete::QtMaterialAutocomplete(QWidget* parent)
    : QtMaterialWidget(parent)
    , m_lineEdit(new QLineEdit(this))
    , m_popup(new ::QtMaterialAutocompletePopup(this))
    , m_ownedStringModel(new QStringListModel(this))
{
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_Hover, true);
    m_lineEdit->setFrame(false);
    m_lineEdit->installEventFilter(this);
    m_popup->setAnchorLineEdit(m_lineEdit);
    m_popup->setModel(m_ownedStringModel);
    m_popup->hide();

    connect(m_lineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        updateFilterText();
        emit textChanged(text);
    });
    connect(m_popup, &::QtMaterialAutocompletePopup::completionActivated, this, [this](const QString& completion) {
        m_lineEdit->setText(completion);
        m_lineEdit->setCursorPosition(completion.size());
        emit completionActivated(completion);
    });
    connect(m_popup, &::QtMaterialAutocompletePopup::popupVisibilityChanged,
            this, &QtMaterialAutocomplete::popupVisibilityChanged);
}

QtMaterialAutocomplete::~QtMaterialAutocomplete() = default;

QLineEdit* QtMaterialAutocomplete::lineEdit() const noexcept { return m_lineEdit; }

QString QtMaterialAutocomplete::text() const { return m_lineEdit->text(); }

void QtMaterialAutocomplete::setText(const QString& text) { m_lineEdit->setText(text); }

QString QtMaterialAutocomplete::placeholderText() const { return m_lineEdit->placeholderText(); }

void QtMaterialAutocomplete::setPlaceholderText(const QString& text) { m_lineEdit->setPlaceholderText(text); }

void QtMaterialAutocomplete::setSuggestions(const QStringList& suggestions)
{
    m_ownedStringModel->setStringList(suggestions);
    m_popup->setSuggestions(suggestions);
    updateFilterText();
}

void QtMaterialAutocomplete::setModel(QAbstractItemModel* model)
{
    m_popup->setModel(model ? model : m_ownedStringModel);
    updateFilterText();
}

QAbstractItemModel* QtMaterialAutocomplete::model() const noexcept
{
    return m_popup->model();
}

bool QtMaterialAutocomplete::isPopupVisible() const noexcept
{
    return m_popup->isPopupVisible();
}

void QtMaterialAutocomplete::setPopupVisible(bool visible)
{
    m_popup->setPopupVisible(visible);
}

void QtMaterialAutocomplete::themeChangedEvent(const Theme& theme)
{
    QtMaterialWidget::themeChangedEvent(theme);
    invalidateResolvedSpec();
    updatePaletteFromSpec();
}

void QtMaterialAutocomplete::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialAutocomplete::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }
    SpecFactory factory;
    m_spec = factory.autocompleteSpec(theme());
    m_specDirty = false;
}

void QtMaterialAutocomplete::updateChildGeometry()
{
    ensureSpecResolved();
    const int margin = 16;
    m_lineEdit->setGeometry(rect().adjusted(margin, 4, -margin, -4));
}

void QtMaterialAutocomplete::updatePaletteFromSpec()
{
    ensureSpecResolved();
    QPalette palette = m_lineEdit->palette();
    palette.setColor(QPalette::Base, Qt::transparent);
    palette.setColor(QPalette::Text, m_spec.inputTextColor);
    palette.setColor(QPalette::PlaceholderText, m_spec.placeholderColor);
    m_lineEdit->setPalette(palette);
}

void QtMaterialAutocomplete::updateFilterText()
{
    m_popup->setFilterText(m_lineEdit->text());
    setPopupVisible(!m_lineEdit->text().isEmpty());
}

QSize QtMaterialAutocomplete::sizeHint() const
{
    ensureSpecResolved();
    return QSize(280, m_spec.inputMinHeight);
}

QSize QtMaterialAutocomplete::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(160, m_spec.inputMinHeight);
}

bool QtMaterialAutocomplete::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_lineEdit) {
        if (event->type() == QEvent::KeyPress) {
            auto* key = static_cast<QKeyEvent*>(event);
            if (key->key() == Qt::Key_Down && isPopupVisible()) {
                m_popup->selectNext();
                key->accept();
                return true;
            }
            if (key->key() == Qt::Key_Up && isPopupVisible()) {
                m_popup->selectPrevious();
                key->accept();
                return true;
            }
            if ((key->key() == Qt::Key_Return || key->key() == Qt::Key_Enter) && isPopupVisible()) {
                m_popup->acceptCurrent();
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
            if (!m_popup->isActiveWindow()) {
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

    const qreal radius = theme().shapes().contains(m_spec.inputShapeRole)
        ? theme().shapes().radius(m_spec.inputShapeRole)
        : 4.0;
    QPainterPath path;
    path.addRoundedRect(QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);
    painter.fillPath(path, m_spec.inputContainerColor);
    painter.setPen(QPen(m_lineEdit->hasFocus() ? m_spec.focusedOutlineColor : m_spec.outlineColor, m_lineEdit->hasFocus() ? 2 : 1));
    painter.drawPath(path);

    if (m_lineEdit->hasFocus()) {
        QtMaterialFocusIndicator::paintPathFocusRing(&painter, path, m_spec.focusRingColor, 2.0);
    }
}


int QtMaterialAutocomplete::maxVisibleCompletions() const noexcept {
    return m_maxVisibleCompletions;
}

void QtMaterialAutocomplete::setMaxVisibleCompletions(int count) {
    const int sanitized = qMax(1, count);
    if (m_maxVisibleCompletions == sanitized) {
        return;
    }
    m_maxVisibleCompletions = sanitized;
    if (m_popup) {
        m_popup->setMaximumHeight(sizeHint().height() * m_maxVisibleCompletions);
    }
    updateAccessibilityState();
}

bool QtMaterialAutocomplete::completesOnReturn() const noexcept {
    return m_completesOnReturn;
}

void QtMaterialAutocomplete::setCompletesOnReturn(bool enabled) {
    m_completesOnReturn = enabled;
}

bool QtMaterialAutocomplete::opensOnFocus() const noexcept {
    return m_opensOnFocus;
}

void QtMaterialAutocomplete::setOpensOnFocus(bool enabled) {
    if (m_opensOnFocus == enabled) {
        return;
    }
    m_opensOnFocus = enabled;
    if (!m_opensOnFocus && !m_lineEdit->hasFocus()) {
        setPopupVisible(false);
    }
}

bool QtMaterialAutocomplete::hidePopupOnEscape() const noexcept {
    return m_hidePopupOnEscape;
}

void QtMaterialAutocomplete::setHidePopupOnEscape(bool enabled) {
    m_hidePopupOnEscape = enabled;
}

QString QtMaterialAutocomplete::currentCompletion() const {
    return m_popup ? m_popup->currentCompletion() : QString();
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
    if (m_lineEdit) {
        m_lineEdit->setAccessibleName(accessibleName());
        m_lineEdit->setAccessibleDescription(summary);
    }
    if (summary != m_lastAccessibilitySummary) {
        m_lastAccessibilitySummary = summary;
        emit accessibilitySummaryChanged(summary);
    }
}

} // namespace QtMaterial
