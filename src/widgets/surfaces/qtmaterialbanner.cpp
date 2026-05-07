#include "qtmaterial/widgets/surfaces/qtmaterialbanner.h"

#include <QAbstractButton>
#include <QEvent>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QStyle>
#include <QStyleOption>
#include <QToolButton>

#include "qtmaterial/specs/qtmaterialbannerspec.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"

namespace {
constexpr int kDefaultMinWidth = 280;
constexpr int kDefaultMinHeight = 64;
constexpr int kOuterMargin = 0;
constexpr int kHorizontalPadding = 16;
constexpr int kVerticalPadding = 12;
constexpr int kIconSize = 24;
constexpr int kIconSpacing = 16;
constexpr int kActionSpacing = 8;
constexpr int kCornerRadius = 12;
}

QtMaterialBanner::QtMaterialBanner(QWidget* parent)
    : QtMaterialSurface(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setObjectName(QStringLiteral("qtmaterial_banner"));
    syncDismissButton();
    syncActionButtons();
    syncAccessibility();
}

QtMaterialBanner::QtMaterialBanner(const QString& titleText,
                                   const QString& bodyText,
                                   QWidget* parent)
    : QtMaterialBanner(parent)
{
    m_titleText = titleText;
    m_bodyText = bodyText;
    syncAccessibility();
}

QtMaterialBanner::~QtMaterialBanner() = default;

QString QtMaterialBanner::titleText() const { return m_titleText; }

void QtMaterialBanner::setTitleText(const QString& text)
{
    if (m_titleText == text) {
        return;
    }
    m_titleText = text;
    invalidateLayoutCache();
    syncAccessibility();
    updateGeometry();
    update();
}

QString QtMaterialBanner::bodyText() const { return m_bodyText; }

void QtMaterialBanner::setBodyText(const QString& text)
{
    if (m_bodyText == text) {
        return;
    }
    m_bodyText = text;
    invalidateLayoutCache();
    syncAccessibility();
    updateGeometry();
    update();
}

QIcon QtMaterialBanner::leadingIcon() const { return m_leadingIcon; }

void QtMaterialBanner::setLeadingIcon(const QIcon& icon)
{
    m_leadingIcon = icon;
    invalidateLayoutCache();
    updateGeometry();
    update();
}

bool QtMaterialBanner::isDismissible() const { return m_dismissible; }

void QtMaterialBanner::setDismissible(bool dismissible)
{
    if (m_dismissible == dismissible) {
        return;
    }
    m_dismissible = dismissible;
    syncDismissButton();
    invalidateLayoutCache();
    syncAccessibility();
    updateGeometry();
    update();
}

bool QtMaterialBanner::dismissOnEscape() const noexcept { return m_dismissOnEscape; }

void QtMaterialBanner::setDismissOnEscape(bool dismissOnEscape)
{
    if (m_dismissOnEscape == dismissOnEscape) {
        return;
    }
    m_dismissOnEscape = dismissOnEscape;
    syncAccessibility();
}

QString QtMaterialBanner::primaryActionText() const { return m_primaryActionText; }

void QtMaterialBanner::setPrimaryActionText(const QString& text)
{
    if (m_primaryActionText == text) {
        return;
    }
    m_primaryActionText = text;
    syncActionButtons();
    invalidateLayoutCache();
    syncAccessibility();
    updateGeometry();
    update();
}

QString QtMaterialBanner::secondaryActionText() const { return m_secondaryActionText; }

void QtMaterialBanner::setSecondaryActionText(const QString& text)
{
    if (m_secondaryActionText == text) {
        return;
    }
    m_secondaryActionText = text;
    syncActionButtons();
    invalidateLayoutCache();
    syncAccessibility();
    updateGeometry();
    update();
}

QString QtMaterialBanner::dismissAccessibleName() const
{
    return m_dismissAccessibleName.isEmpty() ? QStringLiteral("Dismiss banner") : m_dismissAccessibleName;
}

void QtMaterialBanner::setDismissAccessibleName(const QString& name)
{
    if (m_dismissAccessibleName == name) {
        return;
    }
    m_dismissAccessibleName = name;
    syncDismissButton();
    syncAccessibility();
}

QString QtMaterialBanner::accessibilitySummary() const
{
    QStringList parts;
    if (!m_titleText.isEmpty()) {
        parts << m_titleText;
    }
    if (!m_bodyText.isEmpty()) {
        parts << m_bodyText;
    }
    if (!m_primaryActionText.isEmpty()) {
        parts << tr("Primary action: %1").arg(m_primaryActionText);
    }
    if (!m_secondaryActionText.isEmpty()) {
        parts << tr("Secondary action: %1").arg(m_secondaryActionText);
    }
    if (m_dismissible) {
        parts << dismissAccessibleName();
    }
    return parts.join(QStringLiteral(". "));
}

QSize QtMaterialBanner::sizeHint() const
{
    ensureLayoutResolved();
    return QSize(qMax(kDefaultMinWidth, m_visualRect.width()), qMax(kDefaultMinHeight, m_visualRect.height()));
}

QSize QtMaterialBanner::minimumSizeHint() const
{
    return QSize(kDefaultMinWidth, kDefaultMinHeight);
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

    QStyleOption opt;
    opt.initFrom(this);
    const QColor baseColor = opt.palette.color(QPalette::Window);
    const QColor textColor = opt.palette.color(QPalette::WindowText);
    const QColor outlineColor = opt.palette.color(QPalette::Mid);

    painter.setPen(QPen(outlineColor, 1));
    painter.setBrush(baseColor);
    painter.drawPath(m_containerPath);

    if (!m_leadingIcon.isNull() && m_iconRect.isValid()) {
        const QPixmap pix = m_leadingIcon.pixmap(m_iconRect.size());
        painter.drawPixmap(m_iconRect, pix);
    }

    painter.setPen(textColor);
    QFont titleFont = font();
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.drawText(m_titleRect, Qt::AlignLeft | Qt::AlignVCenter, m_elidedTitle);

    if (!m_bodyText.isEmpty()) {
        painter.setFont(font());
        painter.drawText(m_bodyRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, m_elidedBody);
    }

    if (hasFocus()) {
        QPen focusPen(opt.palette.color(QPalette::Highlight), 2);
        focusPen.setStyle(Qt::DashLine);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(m_containerPath);
    }
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
    case QEvent::LanguageChange:
        invalidateLayoutCache();
        syncAccessibility();
        updateGeometry();
        update();
        break;
    default:
        break;
    }
}

void QtMaterialBanner::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape && m_dismissible && m_dismissOnEscape) {
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
    if ((watched == m_dismissButton || watched == m_primaryActionButton || watched == m_secondaryActionButton)
        && event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape && m_dismissible && m_dismissOnEscape) {
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
    Q_UNUSED(factory);
    m_specDirty = false;
}

void QtMaterialBanner::ensureLayoutResolved() const
{
    if (!m_layoutDirty) {
        return;
    }

    const QRect bounds = rect().isValid()
        ? rect().adjusted(kOuterMargin, kOuterMargin, -kOuterMargin, -kOuterMargin)
        : QRect(0, 0, kDefaultMinWidth, kDefaultMinHeight);

    m_visualRect = bounds;
    m_contentRect = bounds.adjusted(kHorizontalPadding, kVerticalPadding, -kHorizontalPadding, -kVerticalPadding);

    int leading = m_contentRect.left();
    int trailing = m_contentRect.right();

    if (!m_leadingIcon.isNull()) {
        m_iconRect = QRect(leading, m_contentRect.top(), kIconSize, kIconSize);
        leading = m_iconRect.right() + 1 + kIconSpacing;
    } else {
        m_iconRect = QRect();
    }

    auto reserveButton = [&](QToolButton* button) {
        if (!button || !button->isVisible()) {
            return;
        }
        const QSize hint = button->sizeHint().expandedTo(QSize(40, 32));
        const QRect buttonRect(trailing - hint.width() + 1,
                               m_contentRect.top(),
                               hint.width(),
                               hint.height());
        button->setGeometry(buttonRect);
        trailing = buttonRect.left() - kActionSpacing;
    };

    reserveButton(m_dismissButton);
    reserveButton(m_primaryActionButton);
    reserveButton(m_secondaryActionButton);

    const int textWidth = qMax(0, trailing - leading + 1);
    QFont titleFont = font();
    titleFont.setBold(true);
    const QFontMetrics titleFm(titleFont);
    const QFontMetrics bodyFm(font());

    m_titleRect = QRect(leading, m_contentRect.top(), textWidth, titleFm.height());
    m_elidedTitle = titleFm.elidedText(m_titleText, Qt::ElideRight, textWidth);

    if (!m_bodyText.isEmpty()) {
        const int bodyTop = m_titleRect.bottom() + 1 + 4;
        m_bodyRect = QRect(leading, bodyTop, textWidth, qMax(bodyFm.height(), m_contentRect.bottom() - bodyTop + 1));
        m_elidedBody = bodyFm.elidedText(m_bodyText, Qt::ElideRight, textWidth * 2);
    } else {
        m_bodyRect = QRect();
        m_elidedBody.clear();
    }

    m_containerPath = QPainterPath();
    m_containerPath.addRoundedRect(QRectF(m_visualRect), kCornerRadius, kCornerRadius);
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
        button->setObjectName(QStringLiteral("qtmaterial_banner_dismissButton"));
        button->setAutoRaise(true);
        button->setToolButtonStyle(Qt::ToolButtonIconOnly);
        button->setFocusPolicy(Qt::StrongFocus);
        button->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
        connect(button, &QAbstractButton::clicked, this, &QtMaterialBanner::dismiss);
        button->installEventFilter(this);
        m_dismissButton = button;
    }

    if (m_dismissButton) {
        m_dismissButton->setAccessibleName(dismissAccessibleName());
        m_dismissButton->setToolTip(dismissAccessibleName());
        m_dismissButton->setVisible(true);
    }
}

void QtMaterialBanner::syncActionButtons()
{
    auto syncButton = [this](QPointer<QToolButton>& target,
                            const QString& text,
                            const char* objectName,
                            auto signal) {
        if (text.isEmpty()) {
            if (target) {
                target->hide();
                target->removeEventFilter(this);
                target->deleteLater();
                target = nullptr;
            }
            return;
        }

        if (!target) {
            auto* button = new QToolButton(this);
            button->setObjectName(QString::fromLatin1(objectName));
            button->setAutoRaise(true);
            button->setToolButtonStyle(Qt::ToolButtonTextOnly);
            button->setFocusPolicy(Qt::StrongFocus);
            button->installEventFilter(this);
            connect(button, &QAbstractButton::clicked, this, signal);
            target = button;
        }

        target->setText(text);
        target->setAccessibleName(text);
        target->setToolTip(text);
        target->setVisible(true);
    };

    syncButton(m_primaryActionButton,
               m_primaryActionText,
               "qtmaterial_banner_primaryActionButton",
               &QtMaterialBanner::primaryActionTriggered);
    syncButton(m_secondaryActionButton,
               m_secondaryActionText,
               "qtmaterial_banner_secondaryActionButton",
               &QtMaterialBanner::secondaryActionTriggered);
}

void QtMaterialBanner::syncAccessibility()
{
    setAccessibleName(m_titleText.isEmpty() ? tr("Banner") : m_titleText);
    setAccessibleDescription(accessibilitySummary());
    syncDismissButton();
    emitAccessibilitySummaryIfChanged();
}

void QtMaterialBanner::emitAccessibilitySummaryIfChanged()
{
    const QString summary = accessibilitySummary();
    if (m_lastAccessibilitySummary == summary) {
        return;
    }
    m_lastAccessibilitySummary = summary;
    emit accessibilitySummaryChanged(summary);
}
