#include "qtmaterial/widgets/inputs/qtmaterialcombobox.h"

#include <QAbstractItemView>
#include <QBitmap>
#include <QBrush>
#include <QEvent>
#include <QFontMetrics>
#include <QFrame>
#include <QIcon>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QPointer>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QStyleOptionViewItem>

#include "qtmaterial/core/private/qtmaterialthemecontextbinding_p.h"
#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "../resolution/qtmaterialinputspecresolution_p.h"

namespace QtMaterial {
namespace {

QColor stateLayerColor(
    const QColor& color,
    qreal opacity)
{
    QColor result = color;
    result.setAlphaF(
        qBound<qreal>(
            0.0,
            color.alphaF() * opacity,
            1.0));
    return result;
}

int comboMinimumHeight(
    const AutocompleteSpec& spec)
{
    return qMax(
        40,
        spec.inputMinHeight - 8);
}

int comboItemMinimumHeight(
    const AutocompleteSpec& spec)
{
    return qMax(
        36,
        spec.inputMinHeight - 12);
}

AutocompleteSpec normalizedComboSpec(
    AutocompleteSpec spec)
{
    spec.inputMinHeight =
        qMax(40, spec.inputMinHeight);
    spec.horizontalPadding =
        qMax(8, spec.horizontalPadding);
    spec.verticalInset =
        qMax(0, spec.verticalInset);
    spec.popupVisibleItemCount =
        qMax(1, spec.popupVisibleItemCount);
    spec.inputCornerRadius =
        qMax<qreal>(0.0, spec.inputCornerRadius);
    spec.popupCornerRadius =
        qMax<qreal>(0.0, spec.popupCornerRadius);
    spec.outlineWidth =
        qMax<qreal>(0.0, spec.outlineWidth);
    spec.focusedOutlineWidth =
        qMax<qreal>(0.0, spec.focusedOutlineWidth);
    spec.focusRingWidth =
        qMax<qreal>(0.0, spec.focusRingWidth);
    spec.hoverStateLayerOpacity =
        qBound<qreal>(
            0.0,
            spec.hoverStateLayerOpacity,
            1.0);
    spec.pressStateLayerOpacity =
        qBound<qreal>(
            0.0,
            spec.pressStateLayerOpacity,
            1.0);
    return spec;
}

class MaterialComboItemDelegate final
    : public QStyledItemDelegate
{
public:
    explicit MaterialComboItemDelegate(
        QtMaterialComboBox* combo)
        : QStyledItemDelegate(combo)
        , m_combo(combo)
    {
    }

    void setSpec(
        const AutocompleteSpec& spec)
    {
        m_spec = spec;
    }

    QSize sizeHint(
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override
    {
        QSize result =
            QStyledItemDelegate::sizeHint(
                option,
                index);

        if (m_spec.hasResolvedSuggestionFont) {
            const QFontMetrics metrics(
                m_spec.suggestionFont);
            result.setHeight(
                qMax(
                    result.height(),
                    metrics.height() + 16));
        }

        result.setHeight(
            qMax(
                result.height(),
                comboItemMinimumHeight(m_spec)));
        result.rwidth() += 24;
        return result;
    }

    void paint(
        QPainter* painter,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const override
    {
        if (!painter || !index.isValid()) {
            return;
        }

        QStyleOptionViewItem resolved(option);
        initStyleOption(
            &resolved,
            index);

        const bool enabled =
            resolved.state.testFlag(
                QStyle::State_Enabled);
        const bool selected =
            m_combo
            && index.row()
                == m_combo->currentIndex();
        const bool hovered =
            resolved.state.testFlag(
                QStyle::State_MouseOver);

        QRectF stateRect(
            resolved.rect.adjusted(
                4,
                2,
                -4,
                -2));

        const qreal radius =
            qMin<qreal>(
                m_spec.popupCornerRadius,
                stateRect.height() / 2.0);

        painter->save();
        painter->setRenderHint(
            QPainter::Antialiasing,
            true);
        painter->setPen(Qt::NoPen);

        if (selected) {
            painter->setBrush(
                m_spec.selectedSuggestionContainerColor);
            painter->drawRoundedRect(
                stateRect,
                radius,
                radius);

            if (hovered) {
                painter->setBrush(
                    stateLayerColor(
                        m_spec.stateLayerColor,
                        m_spec.hoverStateLayerOpacity
                            * 0.5));
                painter->drawRoundedRect(
                    stateRect,
                    radius,
                    radius);
            }
        } else if (hovered && enabled) {
            painter->setBrush(
                stateLayerColor(
                    m_spec.stateLayerColor,
                    m_spec.hoverStateLayerOpacity));
            painter->drawRoundedRect(
                stateRect,
                radius,
                radius);
        }

        resolved.state &=
            ~QStyle::State_Selected;
        resolved.state &=
            ~QStyle::State_MouseOver;
        resolved.state &=
            ~QStyle::State_HasFocus;
        resolved.backgroundBrush =
            QBrush();

        const QColor textColor =
            !enabled
                ? m_spec.disabledTextColor
                : selected
                    ? m_spec.selectedSuggestionTextColor
                    : m_spec.suggestionTextColor;

        resolved.palette.setColor(
            QPalette::Text,
            textColor);
        resolved.palette.setColor(
            QPalette::WindowText,
            textColor);
        resolved.palette.setColor(
            QPalette::ButtonText,
            textColor);
        resolved.palette.setColor(
            QPalette::Disabled,
            QPalette::Text,
            m_spec.disabledTextColor);

        if (m_spec.hasResolvedSuggestionFont) {
            resolved.font =
                m_spec.suggestionFont;
        }
        if (selected) {
            resolved.font.setWeight(
                QFont::DemiBold);
        }

        resolved.rect =
            resolved.rect.adjusted(
                12,
                0,
                -12,
                0);

        QStyledItemDelegate::paint(
            painter,
            resolved,
            index);
        painter->restore();
    }

private:
    QPointer<QtMaterialComboBox> m_combo;
    AutocompleteSpec m_spec;
};

} // namespace

class QtMaterialComboBoxPrivate final
{
public:
    QString labelText;

    mutable AutocompleteSpec spec;
    mutable bool specDirty = true;
    bool popupOpen = false;

    QtMaterialThemeContextBinding* themeBinding = nullptr;
    MaterialComboItemDelegate* delegate = nullptr;
    QPointer<QWidget> popupWindow;
    QPointer<QLineEdit> editableLineEdit;
};

QtMaterialComboBox::QtMaterialComboBox(
    QWidget* parent)
    : QComboBox(parent)
    , d_ptr(
        std::make_unique<
            QtMaterialComboBoxPrivate>())
{
    d_ptr->themeBinding =
        new QtMaterialThemeContextBinding(
            this,
            this);

    connect(
        d_ptr->themeBinding,
        &QtMaterialThemeContextBinding::
            effectiveThemeContextChanged,
        this,
        &QtMaterialComboBox::
            effectiveThemeContextChanged);
    connect(
        d_ptr->themeBinding,
        &QtMaterialThemeContextBinding::
            themeChanged,
        this,
        [this](const Theme&) {
            d_ptr->specDirty = true;
            ensureSpecResolved();
            applyResolvedSpec();
        });

    setObjectName(
        QStringLiteral(
            "qtmaterial_combo_box"));
    setFocusPolicy(
        Qt::StrongFocus);
    setAttribute(
        Qt::WA_Hover,
        true);
    setSizeAdjustPolicy(
        QComboBox::
            AdjustToContentsOnFirstShow);

    d_ptr->delegate =
        new MaterialComboItemDelegate(
            this);
    setItemDelegate(
        d_ptr->delegate);

    ensureSpecResolved();
    setMinimumHeight(
        comboMinimumHeight(
            d_ptr->spec));
    applyResolvedSpec();

    connect(
        this,
        QOverload<int>::of(
            &QComboBox::
                currentIndexChanged),
        this,
        [this](int) {
            update();
        });
}

QtMaterialComboBox::~QtMaterialComboBox()
{
    if (d_ptr->popupWindow) {
        d_ptr->popupWindow
            ->removeEventFilter(this);
    }
    if (d_ptr->editableLineEdit) {
        d_ptr->editableLineEdit
            ->removeEventFilter(this);
    }
}

QString
QtMaterialComboBox::labelText() const
{
    return d_ptr->labelText;
}

void QtMaterialComboBox::setLabelText(
    const QString& text)
{
    if (d_ptr->labelText == text) {
        return;
    }

    d_ptr->labelText = text;

    if (!text.isEmpty()) {
        setAccessibleName(text);
    }

    emit labelTextChanged(text);
}

void QtMaterialComboBox::setThemeContext(
    ThemeContext* context)
{
    if (
        d_ptr->themeBinding
            ->themeContext()
        == context) {
        return;
    }

    d_ptr->themeBinding
        ->setThemeContext(context);
    emit themeContextChanged(context);
}

ThemeContext*
QtMaterialComboBox::themeContext()
    const noexcept
{
    return d_ptr->themeBinding
        ->themeContext();
}

ThemeContext*
QtMaterialComboBox::
effectiveThemeContext()
    const noexcept
{
    return d_ptr->themeBinding
        ->effectiveThemeContext();
}

QSize QtMaterialComboBox::sizeHint() const
{
    ensureSpecResolved();

    QSize result =
        QComboBox::sizeHint();
    result.setHeight(
        qMax(
            result.height(),
            comboMinimumHeight(
                d_ptr->spec)));
    result.rwidth() += 8;
    return result;
}

QSize
QtMaterialComboBox::
minimumSizeHint() const
{
    ensureSpecResolved();

    QSize result =
        QComboBox::minimumSizeHint();
    result.setHeight(
        qMax(
            result.height(),
            comboMinimumHeight(
                d_ptr->spec)));
    return result;
}

void QtMaterialComboBox::showPopup()
{
    ensureSpecResolved();
    preparePopup();

    QComboBox::showPopup();

    preparePopup();
    d_ptr->popupOpen =
        d_ptr->popupWindow
        && d_ptr->popupWindow->isVisible();
    updatePopupMask();
    update();
}

void QtMaterialComboBox::hidePopup()
{
    QComboBox::hidePopup();

    if (!d_ptr->popupOpen) {
        return;
    }

    d_ptr->popupOpen = false;
    update();
}

bool QtMaterialComboBox::event(
    QEvent* event)
{
    const bool handled =
        QComboBox::event(event);

    if (!event) {
        return handled;
    }

    switch (event->type()) {
    case QEvent::Enter:
    case QEvent::Leave:
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::HoverMove:
    case QEvent::EnabledChange:
        syncEditableLineEdit();
        update();
        break;

    case QEvent::FontChange:
    case QEvent::StyleChange:
        update();
        break;

    case QEvent::ChildAdded:
        if (d_ptr->themeBinding) {
            syncEditableLineEdit();
        }
        break;

    default:
        break;
    }

    return handled;
}

bool QtMaterialComboBox::eventFilter(
    QObject* watched,
    QEvent* event)
{
    if (!event) {
        return QComboBox::eventFilter(
            watched,
            event);
    }

    if (
        watched
        == d_ptr->popupWindow) {
        switch (event->type()) {
        case QEvent::Show:
            d_ptr->popupOpen = true;
            updatePopupMask();
            update();
            break;

        case QEvent::Hide:
            d_ptr->popupOpen = false;
            update();
            break;

        case QEvent::Resize:
            updatePopupMask();
            break;

        default:
            break;
        }
    }

    if (
        watched
        == d_ptr->editableLineEdit) {
        switch (event->type()) {
        case QEvent::FocusIn:
        case QEvent::FocusOut:
        case QEvent::Enter:
        case QEvent::Leave:
            update();
            break;

        default:
            break;
        }
    }

    return QComboBox::eventFilter(
        watched,
        event);
}

void QtMaterialComboBox::paintEvent(
    QPaintEvent*)
{
    ensureSpecResolved();

    const AutocompleteSpec& resolved =
        d_ptr->spec;

    const QRectF fieldRect =
        QRectF(rect()).adjusted(
            0.5,
            0.5,
            -0.5,
            -0.5);

    if (!fieldRect.isValid()) {
        return;
    }

    const qreal radius =
        qMin<qreal>(
            resolved.inputCornerRadius,
            fieldRect.height() / 2.0);

    QPainterPath fieldPath;
    fieldPath.addRoundedRect(
        fieldRect,
        radius,
        radius);

    QPainter painter(this);
    painter.setRenderHint(
        QPainter::Antialiasing,
        true);

    painter.fillPath(
        fieldPath,
        resolved.inputContainerColor);

    if (isEnabled()) {
        if (d_ptr->popupOpen) {
            painter.fillPath(
                fieldPath,
                stateLayerColor(
                    resolved.stateLayerColor,
                    resolved.pressStateLayerOpacity));
        } else if (underMouse()) {
            painter.fillPath(
                fieldPath,
                stateLayerColor(
                    resolved.stateLayerColor,
                    resolved.hoverStateLayerOpacity));
        }
    }

    const bool focused =
        hasFocus()
        || (
            lineEdit()
            && lineEdit()->hasFocus());

    QColor outlineColor =
        focused
            ? resolved.focusedOutlineColor
            : resolved.outlineColor;

    if (!isEnabled()) {
        outlineColor.setAlphaF(
            outlineColor.alphaF()
            * 0.45);
    }

    painter.setBrush(Qt::NoBrush);
    painter.setPen(
        QPen(
            outlineColor,
            focused
                ? resolved.focusedOutlineWidth
                : resolved.outlineWidth));
    painter.drawPath(fieldPath);

    if (
        focused
        && resolved.focusRingWidth
            > 0.0) {
        QtMaterialFocusIndicator::
            paintPathFocusRing(
                &painter,
                fieldPath,
                resolved.focusRingColor,
                resolved.focusRingWidth);
    }

    const int horizontalPadding =
        qMax(
            10,
            resolved.horizontalPadding);
    const int arrowAreaWidth = 28;
    const bool rtl =
        layoutDirection()
        == Qt::RightToLeft;

    QRect contentRect = rect();
    QRect arrowRect = rect();

    if (rtl) {
        arrowRect.setRight(
            horizontalPadding
            + arrowAreaWidth);
        arrowRect.setLeft(
            horizontalPadding);
        contentRect.adjust(
            horizontalPadding
                + arrowAreaWidth,
            0,
            -horizontalPadding,
            0);
    } else {
        arrowRect.setLeft(
            width()
            - horizontalPadding
            - arrowAreaWidth);
        arrowRect.setRight(
            width()
            - horizontalPadding);
        contentRect.adjust(
            horizontalPadding,
            0,
            -(
                horizontalPadding
                + arrowAreaWidth),
            0);
    }

    if (!isEditable()) {
        QColor textColor =
            isEnabled()
                ? resolved.inputTextColor
                : resolved.disabledTextColor;

        painter.setPen(textColor);

        if (resolved.hasResolvedInputFont) {
            painter.setFont(
                resolved.inputFont);
        }

        const QIcon currentItemIcon =
            currentIndex() >= 0
                ? itemIcon(currentIndex())
                : QIcon();

        if (!currentItemIcon.isNull()) {
            const QSize iconExtent =
                iconSize()
                    .boundedTo(
                        QSize(
                            contentRect.height()
                                - 12,
                            contentRect.height()
                                - 12));

            QRect iconRect(
                QPoint(),
                iconExtent);
            iconRect.moveCenter(
                QPoint(
                    rtl
                        ? contentRect.right()
                            - iconExtent.width()
                                / 2
                        : contentRect.left()
                            + iconExtent.width()
                                / 2,
                    contentRect.center().y()));

            currentItemIcon.paint(
                &painter,
                iconRect,
                Qt::AlignCenter,
                isEnabled()
                    ? QIcon::Normal
                    : QIcon::Disabled);

            const int gap = 8;
            if (rtl) {
                contentRect.setRight(
                    iconRect.left()
                    - gap);
            } else {
                contentRect.setLeft(
                    iconRect.right()
                    + gap);
            }
        }

        const QFontMetrics metrics(
            painter.font());
        const QString elided =
            metrics.elidedText(
                currentText(),
                Qt::ElideRight,
                qMax(
                    0,
                    contentRect.width()));

        painter.drawText(
            contentRect,
            Qt::AlignVCenter
                | (
                    rtl
                        ? Qt::AlignRight
                        : Qt::AlignLeft),
            elided);
    }

    QColor arrowColor =
        isEnabled()
            ? resolved.inputTextColor
            : resolved.disabledTextColor;

    painter.setPen(
        QPen(
            arrowColor,
            1.8,
            Qt::SolidLine,
            Qt::RoundCap,
            Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    const QPointF center =
        QRectF(arrowRect).center();
    const qreal dx = 4.0;
    const qreal dy = 2.5;

    QPainterPath arrowPath;
    if (d_ptr->popupOpen) {
        arrowPath.moveTo(
            center.x() - dx,
            center.y() + dy);
        arrowPath.lineTo(
            center.x(),
            center.y() - dy);
        arrowPath.lineTo(
            center.x() + dx,
            center.y() + dy);
    } else {
        arrowPath.moveTo(
            center.x() - dx,
            center.y() - dy);
        arrowPath.lineTo(
            center.x(),
            center.y() + dy);
        arrowPath.lineTo(
            center.x() + dx,
            center.y() - dy);
    }
    painter.drawPath(arrowPath);
}

const AutocompleteSpec&
QtMaterialComboBox::
resolvedSpec() const
{
    ensureSpecResolved();
    return d_ptr->spec;
}

void QtMaterialComboBox::
ensureSpecResolved() const
{
    if (!d_ptr->specDirty) {
        return;
    }

    d_ptr->spec =
        normalizedComboSpec(
            InputSpecResolution::autocompleteSpec(
                d_ptr->themeBinding));

    d_ptr->specDirty = false;
}

void QtMaterialComboBox::
applyResolvedSpec()
{
    ensureSpecResolved();

    const AutocompleteSpec& resolved =
        d_ptr->spec;

    const int targetHeight =
        comboMinimumHeight(resolved);
    if (minimumHeight() < targetHeight) {
        setMinimumHeight(targetHeight);
    }

    if (resolved.hasResolvedInputFont) {
        setFont(resolved.inputFont);
    }

    QPalette comboPalette =
        palette();
    comboPalette.setColor(
        QPalette::Base,
        resolved.inputContainerColor);
    comboPalette.setColor(
        QPalette::Button,
        resolved.inputContainerColor);
    comboPalette.setColor(
        QPalette::Text,
        resolved.inputTextColor);
    comboPalette.setColor(
        QPalette::ButtonText,
        resolved.inputTextColor);
    comboPalette.setColor(
        QPalette::Highlight,
        resolved.selectedSuggestionContainerColor);
    comboPalette.setColor(
        QPalette::HighlightedText,
        resolved.selectedSuggestionTextColor);
    setPalette(comboPalette);

    if (d_ptr->delegate) {
        d_ptr->delegate
            ->setSpec(resolved);
    }

    if (QAbstractItemView* popupView =
            view()) {
        popupView->setMouseTracking(true);
        if (popupView->viewport()) {
            popupView->viewport()
                ->setMouseTracking(true);
        }
        popupView->setFrameShape(
            QFrame::NoFrame);

        QPalette popupPalette =
            popupView->palette();
        popupPalette.setColor(
            QPalette::Base,
            resolved.popupContainerColor);
        popupPalette.setColor(
            QPalette::Window,
            resolved.popupContainerColor);
        popupPalette.setColor(
            QPalette::Text,
            resolved.suggestionTextColor);
        popupPalette.setColor(
            QPalette::Highlight,
            resolved.selectedSuggestionContainerColor);
        popupPalette.setColor(
            QPalette::HighlightedText,
            resolved.selectedSuggestionTextColor);
        popupView->setPalette(
            popupPalette);

        if (resolved.hasResolvedSuggestionFont) {
            popupView->setFont(
                resolved.suggestionFont);
        }

        popupView->viewport()->update();
    }

    syncEditableLineEdit();
    preparePopup();
    updateGeometry();
    update();
}

void QtMaterialComboBox::preparePopup()
{
    QAbstractItemView* popupView =
        view();
    if (!popupView) {
        return;
    }

    QWidget* popup =
        popupView->window();
    if (
        !popup
        || popup == this
        || !popup
                ->windowFlags()
                .testFlag(Qt::Popup)) {
        return;
    }

    if (
        d_ptr->popupWindow
        && d_ptr->popupWindow
            != popup) {
        d_ptr->popupWindow
            ->removeEventFilter(this);
    }

    d_ptr->popupWindow = popup;
    popup->installEventFilter(this);
    popup->setObjectName(
        QStringLiteral(
            "qtmaterial_combo_popup"));
    popup->setAttribute(
        Qt::WA_StyledBackground,
        true);

    const AutocompleteSpec& resolved =
        resolvedSpec();

    QPalette popupPalette =
        popup->palette();
    popupPalette.setColor(
        QPalette::Window,
        resolved.popupContainerColor);
    popupPalette.setColor(
        QPalette::Base,
        resolved.popupContainerColor);
    popup->setPalette(
        popupPalette);
    popup->setAutoFillBackground(true);

    popup->setStyleSheet(
        QStringLiteral(
            "QWidget#qtmaterial_combo_popup {"
            " background:%1;"
            " border:1px solid %2;"
            " border-radius:%3px;"
            " }")
            .arg(
                resolved.popupContainerColor
                    .name(QColor::HexRgb))
            .arg(
                resolved.outlineColor
                    .name(QColor::HexRgb))
            .arg(
                QString::number(
                    resolved.popupCornerRadius)));

    popupView->setContentsMargins(
        4,
        4,
        4,
        4);

    updatePopupMask();
}

void QtMaterialComboBox::
updatePopupMask()
{
    if (!d_ptr->popupWindow) {
        return;
    }

    const AutocompleteSpec& resolved =
        resolvedSpec();
    QWidget* popup =
        d_ptr->popupWindow.data();

    if (
        resolved.popupCornerRadius
            <= 0.0
        || popup->width() <= 0
        || popup->height() <= 0) {
        popup->clearMask();
        return;
    }

    QBitmap mask(
        popup->size());
    mask.fill(Qt::color0);

    QPainter painter(&mask);
    painter.setRenderHint(
        QPainter::Antialiasing,
        true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::color1);
    painter.drawRoundedRect(
        popup->rect().adjusted(
            0,
            0,
            -1,
            -1),
        resolved.popupCornerRadius,
        resolved.popupCornerRadius);

    popup->setMask(mask);
}

void QtMaterialComboBox::
syncEditableLineEdit()
{
    QLineEdit* edit =
        lineEdit();

    if (
        d_ptr->editableLineEdit
        && d_ptr->editableLineEdit
            != edit) {
        d_ptr->editableLineEdit
            ->removeEventFilter(this);
        d_ptr->editableLineEdit = nullptr;
    }

    if (!edit) {
        return;
    }

    if (
        d_ptr->editableLineEdit
        != edit) {
        d_ptr->editableLineEdit = edit;
        edit->installEventFilter(this);
    }

    const AutocompleteSpec& resolved =
        resolvedSpec();

    edit->setFrame(false);
    edit->setAttribute(
        Qt::WA_TranslucentBackground,
        true);
    edit->setStyleSheet(
        QStringLiteral(
            "background:transparent;"
            "border:0;"));

    QPalette editPalette =
        edit->palette();
    editPalette.setColor(
        QPalette::Base,
        Qt::transparent);
    editPalette.setColor(
        QPalette::Text,
        isEnabled()
            ? resolved.inputTextColor
            : resolved.disabledTextColor);
    editPalette.setColor(
        QPalette::PlaceholderText,
        resolved.placeholderColor);
    edit->setPalette(
        editPalette);

    if (resolved.hasResolvedInputFont) {
        edit->setFont(
            resolved.inputFont);
    }

    const int margin =
        qMax(
            0,
            resolved.horizontalPadding
                - 6);
    edit->setTextMargins(
        margin,
        0,
        margin,
        0);
}

} // namespace QtMaterial
