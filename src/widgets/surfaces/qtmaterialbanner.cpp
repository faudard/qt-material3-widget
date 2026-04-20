#include "qtmaterial/widgets/surfaces/qtmaterialbanner.h"

#include <QAbstractButton>
#include <QEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QStyle>
#include <QToolButton>

#include "qtmaterial/effects/qtmaterialfocusindicator.h"
#include "qtmaterial/specs/qtmaterialbannerspec.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
// #include "qtmaterial/widgets/surfaces/private/qtmaterialsurfacerenderhelper_p.h"

QtMaterialBanner::QtMaterialBanner(QWidget* parent)
    : QtMaterialSurface(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    syncDismissButton();
}

QtMaterialBanner::QtMaterialBanner(const QString& titleText,
                                   const QString& bodyText,
                                   QWidget* parent)
    : QtMaterialBanner(parent)
{
    m_titleText = titleText;
    m_bodyText = bodyText;
}

QtMaterialBanner::~QtMaterialBanner() = default;

QString QtMaterialBanner::titleText() const
{
    return m_titleText;
}

void QtMaterialBanner::setTitleText(const QString& text)
{
    if (m_titleText == text) {
        return;
    }
    m_titleText = text;
    invalidateLayoutCache();
    updateGeometry();
    update();
}

QString QtMaterialBanner::bodyText() const
{
    return m_bodyText;
}

void QtMaterialBanner::setBodyText(const QString& text)
{
    if (m_bodyText == text) {
        return;
    }
    m_bodyText = text;
    invalidateLayoutCache();
    updateGeometry();
    update();
}

QIcon QtMaterialBanner::leadingIcon() const
{
    return m_leadingIcon;
}

void QtMaterialBanner::setLeadingIcon(const QIcon& icon)
{
    m_leadingIcon = icon;
    invalidateLayoutCache();
    update();
}

bool QtMaterialBanner::isDismissible() const
{
    return m_dismissible;
}

void QtMaterialBanner::setDismissible(bool dismissible)
{
    if (m_dismissible == dismissible) {
        return;
    }
    m_dismissible = dismissible;
    syncDismissButton();
    invalidateLayoutCache();
    updateGeometry();
    update();
}

QSize QtMaterialBanner::sizeHint() const
{
    ensureSpecResolved();
    ensureLayoutResolved();
    return QSize();
    // return QSize(qMax(m_spec.minWidth, 280), qMax(m_spec.minHeight, m_visualRect.height()));
}

QSize QtMaterialBanner::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize();
    // return QSize(m_spec.minWidth, m_spec.minHeight);
}

void QtMaterialBanner::dismiss()
{
    hide();
    emit dismissed();
}

void QtMaterialBanner::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();
    ensureLayoutResolved();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // QtMaterialSurfaceRenderHelper::paintContainer(
    //     &painter,
    //     m_visualRect,
    //     m_containerPath,
    //     m_spec.containerColor,
    //     m_spec.outlineColor,
    //     m_spec.outlineWidth);

    // painter.setPen(m_spec.titleColor);
    // painter.setFont(m_spec.titleFont);
    // painter.drawText(m_titleRect, Qt::AlignLeft | Qt::AlignVCenter, m_elidedTitle);

    // if (!m_bodyText.isEmpty()) {
    //     painter.setPen(m_spec.bodyColor);
    //     painter.setFont(m_spec.bodyFont);
    //     painter.drawText(m_bodyRect,
    //                      Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap,
    //                      m_elidedBody);
    // }

    // if (!m_leadingIcon.isNull()) {
    //     const QPixmap pix = m_leadingIcon.pixmap(m_spec.iconSize, m_spec.iconSize);
    //     painter.drawPixmap(m_iconRect, pix);
    // }

    // if (hasFocus()) {
    //     QtMaterialFocusIndicator::paintPathFocusRing(
    //         &painter,
    //         m_containerPath,
    //         m_spec.focusRingColor,
    //         m_spec.focusRingWidth);
    // }
}

void QtMaterialBanner::resizeEvent(QResizeEvent* event)
{
    QtMaterialSurface::resizeEvent(event);
    invalidateLayoutCache();
}

void QtMaterialBanner::changeEvent(QEvent* event)
{
    QtMaterialSurface::changeEvent(event);
    switch (event->type()) {
    case QEvent::FontChange:
    case QEvent::StyleChange:
    case QEvent::LayoutDirectionChange:
    case QEvent::EnabledChange:
        invalidateLayoutCache();
        update();
        break;
    default:
        break;
    }
}

void QtMaterialBanner::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape && m_dismissible) {
        dismiss();
        event->accept();
        return;
    }
    QtMaterialSurface::keyPressEvent(event);
}

void QtMaterialBanner::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialSurface::themeChangedEvent(theme);
    m_specDirty = true;
    invalidateLayoutCache();
    updateGeometry();
    update();
}

void QtMaterialBanner::stateChangedEvent()
{
    QtMaterialSurface::stateChangedEvent();
    update();
}

bool QtMaterialBanner::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_dismissButton && event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            dismiss();
            return true;
        }
    }
    return QtMaterialSurface::eventFilter(watched, event);
}

void QtMaterialBanner::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }

    QtMaterial::SpecFactory factory;
    // m_spec = factory.bannerSpec(theme(), density());
    m_specDirty = false;
}

void QtMaterialBanner::ensureLayoutResolved() const
{
    if (!m_layoutDirty) {
        return;
    }
    ensureSpecResolved();

    // const QRect bounds = rect().adjusted(m_spec.outerMargin,
    //                                      m_spec.outerMargin,
    //                                      -m_spec.outerMargin,
    //                                      -m_spec.outerMargin);
    // m_visualRect = bounds;
    // m_contentRect = bounds.adjusted(m_spec.horizontalPadding,
    //                                 m_spec.verticalPadding,
    //                                 -m_spec.horizontalPadding,
    //                                 -m_spec.verticalPadding);

    // int x = m_contentRect.left();
    // const int top = m_contentRect.top();

    // if (!m_leadingIcon.isNull()) {
    //     m_iconRect = QRect(x, top + m_spec.iconTopOffset, m_spec.iconSize, m_spec.iconSize);
    //     x = m_iconRect.right() + 1 + m_spec.iconSpacing;
    // } else {
    //     m_iconRect = QRect();
    // }

    // int right = m_contentRect.right();
    // if (m_dismissible) {
    //     right -= (m_spec.dismissButtonSize + m_spec.dismissSpacing);
    //     if (m_dismissButton) {
    //         m_dismissButton->setGeometry(QRect(right + m_spec.dismissSpacing,
    //                                            top,
    //                                            m_spec.dismissButtonSize,
    //                                            m_spec.dismissButtonSize));
    //     }
    // }

    // const int textWidth = qMax(0, right - x + 1);
    // const QFontMetrics titleFm(m_spec.titleFont);
    // const QFontMetrics bodyFm(m_spec.bodyFont);

    // m_titleRect = QRect(x, top, textWidth, titleFm.height());
    // m_elidedTitle = titleFm.elidedText(m_titleText, Qt::ElideRight, textWidth);

    // if (!m_bodyText.isEmpty()) {
    //     const int bodyTop = m_titleRect.bottom() + 1 + m_spec.bodyTopSpacing;
    //     const int bodyHeight = qMax(bodyFm.height(), m_contentRect.bottom() - bodyTop + 1);
    //     m_bodyRect = QRect(x, bodyTop, textWidth, bodyHeight);
    //     m_elidedBody = bodyFm.elidedText(m_bodyText, Qt::ElideRight, textWidth * 2);
    // } else {
    //     m_bodyRect = QRect();
    //     m_elidedBody.clear();
    // }

    // m_containerPath = QPainterPath();
    // m_containerPath.addRoundedRect(QRectF(m_visualRect), m_spec.cornerRadius, m_spec.cornerRadius);

    m_layoutDirty = false;
}

void QtMaterialBanner::invalidateLayoutCache()
{
    m_layoutDirty = true;
}

void QtMaterialBanner::syncDismissButton()
{
    if (!m_dismissible) {
        if (m_dismissButton) {
            m_dismissButton->hide();
            m_dismissButton->removeEventFilter(this);
            m_dismissButton->deleteLater();
            m_dismissButton = nullptr;
        }
        return;
    }

    if (!m_dismissButton) {
        auto* button = new QToolButton(this);
        button->setAutoRaise(true);
        button->setToolButtonStyle(Qt::ToolButtonIconOnly);
        button->setFocusPolicy(Qt::StrongFocus);
        button->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
        connect(button, &QAbstractButton::clicked, this, &QtMaterialBanner::dismiss);
        button->installEventFilter(this);
        m_dismissButton = button;
    }

    if (m_dismissButton) {
        m_dismissButton->setVisible(true);
    }
}
