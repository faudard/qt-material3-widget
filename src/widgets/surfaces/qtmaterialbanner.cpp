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

#include <memory>
#include <QPainterPath>
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

struct QtMaterialBannerPrivate
{
    QString m_titleText;
    QString m_bodyText;
    QIcon m_leadingIcon;
    bool m_dismissible = true;
    bool m_dismissOnEscape = true;
    QString m_primaryActionText;
    QString m_secondaryActionText;
    QString m_dismissAccessibleName;
    QString m_lastAccessibilitySummary;

    mutable bool m_specDirty = true;
    mutable bool m_layoutDirty = true;
    mutable QtMaterial::BannerSpec m_spec;
    mutable QRect m_visualRect;
    mutable QRect m_contentRect;
    mutable QRect m_titleRect;
    mutable QRect m_bodyRect;
    mutable QRect m_iconRect;
    mutable QPainterPath m_containerPath;
    mutable QString m_elidedTitle;
    mutable QString m_elidedBody;

    QPointer<QToolButton> m_dismissButton;
    QPointer<QToolButton> m_primaryActionButton;
    QPointer<QToolButton> m_secondaryActionButton;
};

QtMaterialBanner::QtMaterialBanner(QWidget* parent)
    : QtMaterialSurface(parent)
    , d(std::make_unique<QtMaterialBannerPrivate>())
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
    d->m_titleText = titleText;
    d->m_bodyText = bodyText;
    syncAccessibility();
}

QtMaterialBanner::~QtMaterialBanner() = default;

QString QtMaterialBanner::titleText() const { return d->m_titleText; }

void QtMaterialBanner::setTitleText(const QString& text)
{
    if (d->m_titleText == text) {
        return;
    }
    d->m_titleText = text;
    invalidateLayoutCache();
    syncAccessibility();
    updateGeometry();
    update();
}

QString QtMaterialBanner::bodyText() const { return d->m_bodyText; }

void QtMaterialBanner::setBodyText(const QString& text)
{
    if (d->m_bodyText == text) {
        return;
    }
    d->m_bodyText = text;
    invalidateLayoutCache();
    syncAccessibility();
    updateGeometry();
    update();
}

QIcon QtMaterialBanner::leadingIcon() const { return d->m_leadingIcon; }

void QtMaterialBanner::setLeadingIcon(const QIcon& icon)
{
    d->m_leadingIcon = icon;
    invalidateLayoutCache();
    updateGeometry();
    update();
}

bool QtMaterialBanner::isDismissible() const { return d->m_dismissible; }

void QtMaterialBanner::setDismissible(bool dismissible)
{
    if (d->m_dismissible == dismissible) {
        return;
    }
    d->m_dismissible = dismissible;
    syncDismissButton();
    invalidateLayoutCache();
    syncAccessibility();
    updateGeometry();
    update();
}

bool QtMaterialBanner::dismissOnEscape() const noexcept { return d->m_dismissOnEscape; }

void QtMaterialBanner::setDismissOnEscape(bool dismissOnEscape)
{
    if (d->m_dismissOnEscape == dismissOnEscape) {
        return;
    }
    d->m_dismissOnEscape = dismissOnEscape;
    syncAccessibility();
}

QString QtMaterialBanner::primaryActionText() const { return d->m_primaryActionText; }

void QtMaterialBanner::setPrimaryActionText(const QString& text)
{
    if (d->m_primaryActionText == text) {
        return;
    }
    d->m_primaryActionText = text;
    syncActionButtons();
    invalidateLayoutCache();
    syncAccessibility();
    updateGeometry();
    update();
}

QString QtMaterialBanner::secondaryActionText() const { return d->m_secondaryActionText; }

void QtMaterialBanner::setSecondaryActionText(const QString& text)
{
    if (d->m_secondaryActionText == text) {
        return;
    }
    d->m_secondaryActionText = text;
    syncActionButtons();
    invalidateLayoutCache();
    syncAccessibility();
    updateGeometry();
    update();
}

QString QtMaterialBanner::dismissAccessibleName() const
{
    return d->m_dismissAccessibleName.isEmpty() ? QStringLiteral("Dismiss banner") : d->m_dismissAccessibleName;
}

void QtMaterialBanner::setDismissAccessibleName(const QString& name)
{
    if (d->m_dismissAccessibleName == name) {
        return;
    }
    d->m_dismissAccessibleName = name;
    syncDismissButton();
    syncAccessibility();
}

QString QtMaterialBanner::accessibilitySummary() const
{
    QStringList parts;
    if (!d->m_titleText.isEmpty()) {
        parts << d->m_titleText;
    }
    if (!d->m_bodyText.isEmpty()) {
        parts << d->m_bodyText;
    }
    if (!d->m_primaryActionText.isEmpty()) {
        parts << tr("Primary action: %1").arg(d->m_primaryActionText);
    }
    if (!d->m_secondaryActionText.isEmpty()) {
        parts << tr("Secondary action: %1").arg(d->m_secondaryActionText);
    }
    if (d->m_dismissible) {
        parts << dismissAccessibleName();
    }
    return parts.join(QStringLiteral(". "));
}

QSize QtMaterialBanner::sizeHint() const
{
    ensureLayoutResolved();
    return QSize(qMax(kDefaultMinWidth, d->m_visualRect.width()), qMax(kDefaultMinHeight, d->m_visualRect.height()));
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
    painter.drawPath(d->m_containerPath);

    if (!d->m_leadingIcon.isNull() && d->m_iconRect.isValid()) {
        const QPixmap pix = d->m_leadingIcon.pixmap(d->m_iconRect.size());
        painter.drawPixmap(d->m_iconRect, pix);
    }

    painter.setPen(textColor);
    QFont titleFont = font();
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.drawText(d->m_titleRect, Qt::AlignLeft | Qt::AlignVCenter, d->m_elidedTitle);

    if (!d->m_bodyText.isEmpty()) {
        painter.setFont(font());
        painter.drawText(d->m_bodyRect, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, d->m_elidedBody);
    }

    if (hasFocus()) {
        QPen focusPen(opt.palette.color(QPalette::Highlight), 2);
        focusPen.setStyle(Qt::DashLine);
        painter.setPen(focusPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(d->m_containerPath);
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
    if (event->key() == Qt::Key_Escape && d->m_dismissible && d->m_dismissOnEscape) {
        dismiss();
        event->accept();
        return;
    }
    QtMaterialSurface::keyPressEvent(event);
}

void QtMaterialBanner::themeChangedEvent(const QtMaterial::Theme& theme)
{
    QtMaterialSurface::themeChangedEvent(theme);
    d->m_specDirty = true;
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
    if ((watched == d->m_dismissButton || watched == d->m_primaryActionButton || watched == d->m_secondaryActionButton)
        && event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape && d->m_dismissible && d->m_dismissOnEscape) {
            dismiss();
            return true;
        }
    }
    return QtMaterialSurface::eventFilter(watched, event);
}

void QtMaterialBanner::ensureSpecResolved() const
{
    if (!d->m_specDirty) {
        return;
    }
    QtMaterial::SpecFactory factory;
    Q_UNUSED(factory);
    d->m_specDirty = false;
}

void QtMaterialBanner::ensureLayoutResolved() const
{
    if (!d->m_layoutDirty) {
        return;
    }

    const QRect bounds = rect().isValid()
        ? rect().adjusted(kOuterMargin, kOuterMargin, -kOuterMargin, -kOuterMargin)
        : QRect(0, 0, kDefaultMinWidth, kDefaultMinHeight);

    d->m_visualRect = bounds;
    d->m_contentRect = bounds.adjusted(kHorizontalPadding, kVerticalPadding, -kHorizontalPadding, -kVerticalPadding);

    int leading = d->m_contentRect.left();
    int trailing = d->m_contentRect.right();

    if (!d->m_leadingIcon.isNull()) {
        d->m_iconRect = QRect(leading, d->m_contentRect.top(), kIconSize, kIconSize);
        leading = d->m_iconRect.right() + 1 + kIconSpacing;
    } else {
        d->m_iconRect = QRect();
    }

    auto reserveButton = [&](QToolButton* button) {
        if (!button || !button->isVisible()) {
            return;
        }
        const QSize hint = button->sizeHint().expandedTo(QSize(40, 32));
        const QRect buttonRect(trailing - hint.width() + 1,
                               d->m_contentRect.top(),
                               hint.width(),
                               hint.height());
        button->setGeometry(buttonRect);
        trailing = buttonRect.left() - kActionSpacing;
    };

    reserveButton(d->m_dismissButton);
    reserveButton(d->m_primaryActionButton);
    reserveButton(d->m_secondaryActionButton);

    const int textWidth = qMax(0, trailing - leading + 1);
    QFont titleFont = font();
    titleFont.setBold(true);
    const QFontMetrics titleFm(titleFont);
    const QFontMetrics bodyFm(font());

    d->m_titleRect = QRect(leading, d->m_contentRect.top(), textWidth, titleFm.height());
    d->m_elidedTitle = titleFm.elidedText(d->m_titleText, Qt::ElideRight, textWidth);

    if (!d->m_bodyText.isEmpty()) {
        const int bodyTop = d->m_titleRect.bottom() + 1 + 4;
        d->m_bodyRect = QRect(leading, bodyTop, textWidth, qMax(bodyFm.height(), d->m_contentRect.bottom() - bodyTop + 1));
        d->m_elidedBody = bodyFm.elidedText(d->m_bodyText, Qt::ElideRight, textWidth * 2);
    } else {
        d->m_bodyRect = QRect();
        d->m_elidedBody.clear();
    }

    d->m_containerPath = QPainterPath();
    d->m_containerPath.addRoundedRect(QRectF(d->m_visualRect), kCornerRadius, kCornerRadius);
    d->m_layoutDirty = false;
}

void QtMaterialBanner::invalidateLayoutCache()
{
    d->m_layoutDirty = true;
}

void QtMaterialBanner::syncDismissButton()
{
    if (!d->m_dismissible) {
        if (d->m_dismissButton) {
            d->m_dismissButton->hide();
            d->m_dismissButton->removeEventFilter(this);
            d->m_dismissButton->deleteLater();
            d->m_dismissButton = nullptr;
        }
        return;
    }

    if (!d->m_dismissButton) {
        auto* button = new QToolButton(this);
        button->setObjectName(QStringLiteral("qtmaterial_banner_dismissButton"));
        button->setAutoRaise(true);
        button->setToolButtonStyle(Qt::ToolButtonIconOnly);
        button->setFocusPolicy(Qt::StrongFocus);
        button->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));
        connect(button, &QAbstractButton::clicked, this, &QtMaterialBanner::dismiss);
        button->installEventFilter(this);
        d->m_dismissButton = button;
    }

    if (d->m_dismissButton) {
        d->m_dismissButton->setAccessibleName(dismissAccessibleName());
        d->m_dismissButton->setToolTip(dismissAccessibleName());
        d->m_dismissButton->setVisible(true);
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

    syncButton(d->m_primaryActionButton,
               d->m_primaryActionText,
               "qtmaterial_banner_primaryActionButton",
               &QtMaterialBanner::primaryActionTriggered);
    syncButton(d->m_secondaryActionButton,
               d->m_secondaryActionText,
               "qtmaterial_banner_secondaryActionButton",
               &QtMaterialBanner::secondaryActionTriggered);
}

void QtMaterialBanner::syncAccessibility()
{
    setAccessibleName(d->m_titleText.isEmpty() ? tr("Banner") : d->m_titleText);
    setAccessibleDescription(accessibilitySummary());
    syncDismissButton();
    emitAccessibilitySummaryIfChanged();
}

void QtMaterialBanner::emitAccessibilitySummaryIfChanged()
{
    const QString summary = accessibilitySummary();
    if (d->m_lastAccessibilitySummary == summary) {
        return;
    }
    d->m_lastAccessibilitySummary = summary;
    emit accessibilitySummaryChanged(summary);
}
