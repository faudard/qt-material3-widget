#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

#include <QEvent>
#include <QFont>
#include <QFontMetrics>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QResizeEvent>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/effects/qtmaterialstatelayerpainter.h"
#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/theme/qtmaterialtypographyscale.h"

namespace QtMaterial {
namespace {
qreal lerp(qreal a, qreal b, qreal t)
{
    return a + ((b - a) * t);
}

QFont interpolatedFont(const QFont& from, const QFont& to, qreal t)
{
    QFont f = from;
    const qreal fromSize = from.pointSizeF() > 0.0 ? from.pointSizeF() : static_cast<qreal>(from.pointSize());
    const qreal toSize = to.pointSizeF() > 0.0 ? to.pointSizeF() : static_cast<qreal>(to.pointSize());
    if (fromSize > 0.0 && toSize > 0.0) {
        f.setPointSizeF(lerp(fromSize, toSize, t));
    }
    f.setWeight(t >= 0.5 ? to.weight() : from.weight());
    return f;
}
}

QtMaterialOutlinedTextField::QtMaterialOutlinedTextField(QWidget* parent)
    : QtMaterialInputControl(parent)
    , m_lineEdit(new QLineEdit(this))
    , m_transition(new QtMaterialTransitionController(this))
{
    setFocusPolicy(Qt::StrongFocus);
    setMinimumHeight(64);

    if (m_lineEdit) {
        m_lineEdit->setFrame(false);
        m_lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
        m_lineEdit->setPlaceholderText(QString());
        m_lineEdit->installEventFilter(this);
        setFocusProxy(m_lineEdit);

        connect(m_lineEdit, &QLineEdit::textChanged, this, [this](const QString&) {
            updateTransitionTarget();
            update();
        });
    }

    if (m_transition) {
        connect(m_transition, &QtMaterialTransitionController::progressChanged, this, [this](qreal) {
            update();
        });
    }

    updateTransitionTarget();
}

QtMaterialOutlinedTextField::~QtMaterialOutlinedTextField() = default;

QString QtMaterialOutlinedTextField::text() const
{
    return m_lineEdit ? m_lineEdit->text() : QString();
}

void QtMaterialOutlinedTextField::setText(const QString& text)
{
    if (m_lineEdit) {
        m_lineEdit->setText(text);
    }
    updateTransitionTarget();
    update();
}

QString QtMaterialOutlinedTextField::placeholderText() const
{
    return m_lineEdit ? m_lineEdit->placeholderText() : QString();
}

void QtMaterialOutlinedTextField::setPlaceholderText(const QString& text)
{
    if (m_lineEdit) {
        m_lineEdit->setPlaceholderText(text);
    }
}

QLineEdit* QtMaterialOutlinedTextField::lineEdit() const
{
    return m_lineEdit;
}

QSize QtMaterialOutlinedTextField::sizeHint() const
{
    resolveSpecIfNeeded();

    const TypographyStyle inputStyle = theme().typography().style(m_spec.inputTypeRole);
    const QFont inputFont = inputStyle.font.family().isEmpty() ? font() : inputStyle.font;
    const QFontMetrics fm(inputFont);

    const int textWidth = qMax(160, fm.horizontalAdvance(text().isEmpty() ? QStringLiteral("Outlined text field") : text()));
    const int widthHint = (m_spec.horizontalPadding * 2) + textWidth;
    const int heightHint = m_spec.minHeight + m_spec.supportingTopSpacing + m_spec.supportingHeight;
    return QSize(widthHint, heightHint);
}

QSize QtMaterialOutlinedTextField::minimumSizeHint() const
{
    resolveSpecIfNeeded();
    return QSize(120, m_spec.minHeight + m_spec.supportingTopSpacing + m_spec.supportingHeight);
}

bool QtMaterialOutlinedTextField::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_lineEdit) {
        switch (event->type()) {
        case QEvent::FocusIn:
            interactionState().setFocused(true);
            updateTransitionTarget();
            stateChangedEvent();
            break;
        case QEvent::FocusOut:
            interactionState().setFocused(false);
            interactionState().setPressed(false);
            updateTransitionTarget();
            stateChangedEvent();
            break;
        case QEvent::MouseButtonPress:
            interactionState().setPressed(true);
            stateChangedEvent();
            break;
        case QEvent::MouseButtonRelease:
            interactionState().setPressed(false);
            stateChangedEvent();
            break;
        default:
            break;
        }
    }
    return QtMaterialInputControl::eventFilter(watched, event);
}

void QtMaterialOutlinedTextField::themeChangedEvent(const Theme& theme)
{
    QtMaterialInputControl::themeChangedEvent(theme);
    m_specDirty = true;
    updateLineEditStyle();
}

void QtMaterialOutlinedTextField::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialOutlinedTextField::stateChangedEvent()
{
    updateTransitionTarget();
    updateLineEditStyle();
    QtMaterialInputControl::stateChangedEvent();
}

void QtMaterialOutlinedTextField::resolveSpecIfNeeded() const
{
    if (!m_specDirty) {
        return;
    }

    SpecFactory factory;
    m_spec = factory.outlinedTextFieldSpec(theme(), density());
    m_specDirty = false;
}

void QtMaterialOutlinedTextField::updateLineEditStyle()
{
    if (!m_lineEdit) {
        return;
    }

    resolveSpecIfNeeded();

    const TypographyStyle inputStyle = theme().typography().style(m_spec.inputTypeRole);
    if (!inputStyle.font.family().isEmpty()) {
        m_lineEdit->setFont(inputStyle.font);
    }

    QPalette pal = m_lineEdit->palette();
    pal.setColor(QPalette::Base, Qt::transparent);
    pal.setColor(QPalette::Text, isEnabled() ? m_spec.inputTextColor : m_spec.disabledInputTextColor);
    pal.setColor(QPalette::PlaceholderText, m_spec.labelColor);
    pal.setColor(QPalette::Highlight, theme().colorScheme().color(ColorRole::PrimaryContainer));
    pal.setColor(QPalette::HighlightedText, theme().colorScheme().color(ColorRole::OnPrimaryContainer));
    m_lineEdit->setPalette(pal);
    m_lineEdit->setStyleSheet(QStringLiteral("QLineEdit { border: none; background: transparent; }") );
}

void QtMaterialOutlinedTextField::updateTransitionTarget()
{
    if (!m_transition) {
        return;
    }

    const bool shouldFloat = interactionState().isFocused() || !text().isEmpty();
    m_transition->setProgress(shouldFloat ? 1.0 : 0.0);
}

QRectF QtMaterialOutlinedTextField::containerRect() const
{
    resolveSpecIfNeeded();
    const int supportingZone = m_spec.supportingTopSpacing + m_spec.supportingHeight;
    return QRectF(0.5, 0.5, width() - 1.0, qMax(0, height() - supportingZone) - 1.0);
}

QRect QtMaterialOutlinedTextField::editorRect() const
{
    resolveSpecIfNeeded();
    const QRectF container = containerRect();
    const int left = static_cast<int>(container.left()) + m_spec.horizontalPadding;
    const int top = static_cast<int>(container.top()) + m_spec.editorTopPadding;
    const int right = static_cast<int>(container.right()) - m_spec.horizontalPadding;
    const int bottom = static_cast<int>(container.bottom()) - m_spec.editorBottomPadding;
    return QRect(QPoint(left, top), QPoint(right, qMax(top, bottom)));
}

QRect QtMaterialOutlinedTextField::supportingTextRectForSpec() const
{
    resolveSpecIfNeeded();
    const QRectF container = containerRect();
    return QRect(
        static_cast<int>(container.left()) + m_spec.horizontalPadding,
        static_cast<int>(container.bottom()) + m_spec.supportingTopSpacing,
        width() - (m_spec.horizontalPadding * 2),
        m_spec.supportingHeight);
}

QRectF QtMaterialOutlinedTextField::labelRectForProgress(qreal progress) const
{
    resolveSpecIfNeeded();
    const QRectF container = containerRect();
    const qreal y = lerp(m_spec.labelRestingTop, m_spec.labelFloatingTop, progress);
    return QRectF(
        container.left() + m_spec.horizontalPadding,
        container.top() + y,
        container.width() - (m_spec.horizontalPadding * 2),
        20.0);
}

qreal QtMaterialOutlinedTextField::stateLayerOpacity() const
{
    if (interactionState().isPressed()) {
        return theme().stateLayer().pressOpacity;
    }
    if (interactionState().isFocused()) {
        return theme().stateLayer().focusOpacity;
    }
    if (interactionState().isHovered()) {
        return theme().stateLayer().hoverOpacity;
    }
    return 0.0;
}

void QtMaterialOutlinedTextField::resizeEvent(QResizeEvent* event)
{
    QtMaterialInputControl::resizeEvent(event);
    if (m_lineEdit) {
        m_lineEdit->setGeometry(editorRect());
    }
}

void QtMaterialOutlinedTextField::changeEvent(QEvent* event)
{
    QtMaterialInputControl::changeEvent(event);
    if (event->type() == QEvent::EnabledChange) {
        updateLineEditStyle();
    }
}

void QtMaterialOutlinedTextField::paintEvent(QPaintEvent*)
{
    resolveSpecIfNeeded();
    updateLineEditStyle();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF box = containerRect();
    const qreal progress = m_transition ? m_transition->progress() : ((interactionState().isFocused() || !text().isEmpty()) ? 1.0 : 0.0);

    QPainterPath boxPath;
    boxPath.addRoundedRect(box, m_spec.cornerRadius, m_spec.cornerRadius);

    if (m_spec.containerColor.alpha() > 0) {
        painter.fillPath(boxPath, m_spec.containerColor);
    }

    const qreal overlayOpacity = stateLayerOpacity();
    if (overlayOpacity > 0.0) {
        QtMaterialStateLayerPainter::paintPath(&painter, boxPath, m_spec.stateLayerColor, overlayOpacity);
    }

    QColor outlineColor = m_spec.outlineColor;
    if (!isEnabled()) {
        outlineColor = m_spec.disabledOutlineColor;
    } else if (hasErrorState()) {
        outlineColor = m_spec.errorColor;
    } else if (interactionState().isFocused()) {
        outlineColor = m_spec.focusedOutlineColor;
    }

    painter.setPen(QPen(outlineColor, interactionState().isFocused() ? 2.0 : 1.0));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(boxPath);

    QColor labelColor = m_spec.labelColor;
    if (!isEnabled()) {
        labelColor = m_spec.disabledLabelColor;
    } else if (hasErrorState()) {
        labelColor = m_spec.errorColor;
    } else if (interactionState().isFocused()) {
        labelColor = m_spec.focusedLabelColor;
    }

    const TypographyStyle inputStyle = theme().typography().style(m_spec.inputTypeRole);
    const TypographyStyle labelStyle = theme().typography().style(m_spec.labelTypeRole);
    const QFont labelFont = labelStyle.font.family().isEmpty() ? font() : labelStyle.font;
    const QFont restFont = inputStyle.font.family().isEmpty() ? font() : inputStyle.font;
    painter.setFont(interpolatedFont(restFont, labelFont, progress));
    painter.setPen(labelColor);
    painter.drawText(labelRectForProgress(progress), Qt::AlignLeft | Qt::AlignVCenter,
                     QFontMetrics(painter.font()).elidedText(labelText(), Qt::ElideRight,
                                                            static_cast<int>(box.width()) - (m_spec.horizontalPadding * 2)));

    const QString helperText = hasErrorState() && !errorText().isEmpty() ? errorText() : supportingText();
    if (!helperText.isEmpty()) {
        const TypographyStyle supportingStyle = theme().typography().style(m_spec.supportingTextRole);
        if (!supportingStyle.font.family().isEmpty()) {
            painter.setFont(supportingStyle.font);
        }
        painter.setPen(hasErrorState() ? m_spec.errorColor : m_spec.supportingTextColor);
        painter.drawText(supportingTextRectForSpec(), Qt::AlignLeft | Qt::AlignVCenter,
                         QFontMetrics(painter.font()).elidedText(helperText, Qt::ElideRight,
                                                                supportingTextRectForSpec().width()));
    }

    if (interactionState().isFocused()) {
        QtMaterialFocusIndicator::paintRectFocusRing(
            &painter,
            box.adjusted(-2.0, -2.0, 2.0, 2.0),
            hasErrorState() ? m_spec.errorColor : m_spec.focusedOutlineColor,
            m_spec.cornerRadius + 2.0,
            1.5);
    }
}

} // namespace QtMaterial
