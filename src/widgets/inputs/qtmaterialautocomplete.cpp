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

} // namespace QtMaterial
