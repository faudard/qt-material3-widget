#include "dashboardherobanners.h"

#include <QAbstractButton>
#include <QBoxLayout>
#include <QFont>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QResizeEvent>
#include <QStackedWidget>
#include <QStringList>
#include <QToolButton>
#include <QVBoxLayout>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

namespace {

QColor materialColor(QtMaterial::ColorRole role)
{
    return QtMaterial::ThemeManager::instance().theme().colorScheme().color(role);
}

QColor withAlpha(QColor color, int alpha)
{
    color.setAlpha(alpha);
    return color;
}

void setLabelColor(QLabel* label, const QColor& color)
{
    if (!label) {
        return;
    }

    QPalette palette = label->palette();
    palette.setColor(QPalette::WindowText, color);
    label->setPalette(palette);
}

class WelcomeArtwork final : public QWidget
{
public:
    explicit WelcomeArtwork(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_StyledBackground, true);
        setMinimumHeight(278);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const QRectF bounds = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
        if (bounds.isEmpty()) {
            return;
        }

        const QColor inverseSurface = materialColor(QtMaterial::ColorRole::InverseSurface);
        const QColor primary = materialColor(QtMaterial::ColorRole::Primary);
        const QColor tertiary = materialColor(QtMaterial::ColorRole::Tertiary);

        QPainterPath clip;
        clip.addRoundedRect(bounds, 20.0, 20.0);
        painter.setClipPath(clip);

        QLinearGradient background(bounds.topLeft(), bounds.bottomRight());
        background.setColorAt(0.0, inverseSurface.darker(126));
        background.setColorAt(1.0, inverseSurface.darker(154));
        painter.fillRect(bounds, background);

        painter.setPen(Qt::NoPen);
        painter.setBrush(withAlpha(primary, 32));
        painter.drawEllipse(
            QPointF(bounds.left() + bounds.width() * 0.16, bounds.top() + 52.0),
            88.0,
            88.0);
        painter.setBrush(withAlpha(tertiary, 28));
        painter.drawEllipse(
            QPointF(bounds.left() + bounds.width() * 0.33, bounds.bottom() - 44.0),
            116.0,
            116.0);

        const qreal artWidth = qMin<qreal>(330.0, bounds.width() * 0.34);
        const QRectF card(
            bounds.right() - artWidth - 42.0,
            bounds.top() + 66.0,
            artWidth,
            qMin<qreal>(142.0, bounds.height() - 104.0));

        painter.setBrush(QColor(250, 252, 255, 246));
        painter.drawRoundedRect(card, 16.0, 16.0);

        const QRectF accent(
            card.left() - 26.0,
            card.top() + 18.0,
            74.0,
            card.height() - 36.0);
        painter.setBrush(primary.lighter(118));
        painter.drawRoundedRect(accent, 13.0, 13.0);

        painter.setBrush(QColor(255, 255, 255, 92));
        painter.drawEllipse(
            QPointF(accent.center().x(), accent.top() + 25.0),
            10.0,
            10.0);
        painter.drawEllipse(
            QPointF(accent.center().x(), accent.center().y()),
            10.0,
            10.0);
        painter.drawEllipse(
            QPointF(accent.center().x(), accent.bottom() - 25.0),
            10.0,
            10.0);

        painter.setBrush(QColor(225, 229, 236));
        painter.drawRoundedRect(
            QRectF(card.left() + 72.0, card.top() + 34.0, card.width() * 0.28, 22.0),
            5.0,
            5.0);
        painter.drawRoundedRect(
            QRectF(card.left() + 72.0, card.top() + 69.0, card.width() * 0.36, 32.0),
            5.0,
            5.0);

        const qreal chartLeft = card.right() - 68.0;
        const qreal chartBottom = card.bottom() - 34.0;
        const qreal heights[] = {26.0, 48.0, 34.0};
        for (int i = 0; i < 3; ++i) {
            painter.setBrush(i == 1 ? primary : withAlpha(primary, 118));
            painter.drawRoundedRect(
                QRectF(
                    chartLeft + i * 17.0,
                    chartBottom - heights[i],
                    10.0,
                    heights[i]),
                3.0,
                3.0);
        }

        QPainterPath trend;
        trend.moveTo(card.left() + 24.0, card.bottom() - 22.0);
        trend.cubicTo(
            card.left() + 92.0,
            card.bottom() - 65.0,
            card.left() + 152.0,
            card.bottom() - 18.0,
            card.right() - 18.0,
            card.top() + 34.0);
        QPen trendPen(tertiary.lighter(136), 2.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(trendPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(trend);

        painter.setClipping(false);
        painter.setPen(withAlpha(materialColor(QtMaterial::ColorRole::Outline), 100));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(bounds, 20.0, 20.0);
    }
};

class CarouselSlide final : public QWidget
{
public:
    CarouselSlide(
        const QString& eyebrow,
        const QString& title,
        const QString& description,
        QtMaterial::ColorRole accentRole,
        int artwork,
        QWidget* parent = nullptr)
        : QWidget(parent)
        , m_accentRole(accentRole)
        , m_artwork(artwork)
    {
        setMinimumHeight(278);

        auto* layout = new QVBoxLayout(this);
        layout->setContentsMargins(24, 62, 24, 22);
        layout->setSpacing(6);
        layout->addStretch(1);

        m_eyebrow = new QLabel(eyebrow, this);
        QFont eyebrowFont = m_eyebrow->font();
        eyebrowFont.setBold(true);
        eyebrowFont.setPointSizeF(qMax<qreal>(8.0, eyebrowFont.pointSizeF() - 1.0));
        m_eyebrow->setFont(eyebrowFont);

        m_title = new QLabel(title, this);
        QFont titleFont = m_title->font();
        titleFont.setBold(true);
        titleFont.setPointSizeF(titleFont.pointSizeF() + 4.0);
        m_title->setFont(titleFont);
        m_title->setWordWrap(true);

        m_description = new QLabel(description, this);
        QFont bodyFont = m_description->font();
        bodyFont.setPointSizeF(qMax<qreal>(8.0, bodyFont.pointSizeF() - 1.0));
        m_description->setFont(bodyFont);
        m_description->setWordWrap(true);

        layout->addWidget(m_eyebrow);
        layout->addWidget(m_title);
        layout->addWidget(m_description);

        refreshTheme();
    }

    void refreshTheme()
    {
        const QColor text(255, 255, 255);
        setLabelColor(m_eyebrow, materialColor(QtMaterial::ColorRole::PrimaryContainer).lighter(118));
        setLabelColor(m_title, text);
        setLabelColor(m_description, withAlpha(text, 214));
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        const QRectF bounds = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
        if (bounds.isEmpty()) {
            return;
        }

        QColor accent = materialColor(m_accentRole);
        QColor dark = accent.darker(accent.lightness() > 150 ? 265 : 180);
        QColor deep = dark.darker(142);

        QPainterPath clip;
        clip.addRoundedRect(bounds, 20.0, 20.0);
        painter.setClipPath(clip);

        QLinearGradient background(bounds.topLeft(), bounds.bottomRight());
        background.setColorAt(0.0, dark);
        background.setColorAt(1.0, deep);
        painter.fillRect(bounds, background);

        painter.setPen(Qt::NoPen);
        if (m_artwork == 0) {
            painter.setBrush(withAlpha(accent.lighter(150), 112));
            painter.drawEllipse(
                QPointF(bounds.right() - 76.0, bounds.top() + 74.0),
                112.0,
                86.0);
            painter.setBrush(withAlpha(QColor(255, 255, 255), 34));
            painter.drawEllipse(
                QPointF(bounds.right() - 145.0, bounds.top() + 46.0),
                74.0,
                74.0);

            QPainterPath wave;
            wave.moveTo(bounds.left() + 8.0, bounds.top() + 84.0);
            wave.cubicTo(
                bounds.left() + 74.0, bounds.top() + 14.0,
                bounds.center().x(), bounds.top() + 132.0,
                bounds.right() - 8.0, bounds.top() + 42.0);
            QPen pen(withAlpha(QColor(255, 255, 255), 118), 4.0, Qt::SolidLine, Qt::RoundCap);
            painter.setPen(pen);
            painter.drawPath(wave);
        } else if (m_artwork == 1) {
            const QPointF center(bounds.right() - 94.0, bounds.top() + 86.0);
            for (int i = 0; i < 5; ++i) {
                painter.setPen(QPen(
                    withAlpha(QColor(255, 255, 255), 42 + i * 18),
                    3.0));
                painter.setBrush(Qt::NoBrush);
                painter.drawEllipse(center, 24.0 + i * 17.0, 24.0 + i * 17.0);
            }
            painter.setPen(Qt::NoPen);
            painter.setBrush(accent.lighter(148));
            painter.drawEllipse(center, 19.0, 19.0);
        } else {
            const qreal left = bounds.right() - 206.0;
            const qreal bottom = bounds.top() + 152.0;
            const qreal values[] = {48.0, 83.0, 62.0, 110.0};
            for (int i = 0; i < 4; ++i) {
                painter.setBrush(
                    i == 3
                        ? accent.lighter(148)
                        : withAlpha(QColor(255, 255, 255), 74));
                painter.drawRoundedRect(
                    QRectF(
                        left + i * 40.0,
                        bottom - values[i],
                        24.0,
                        values[i]),
                    6.0,
                    6.0);
            }
            QPainterPath line;
            line.moveTo(left - 4.0, bottom - 38.0);
            line.lineTo(left + 47.0, bottom - 74.0);
            line.lineTo(left + 88.0, bottom - 57.0);
            line.lineTo(left + 129.0, bottom - 102.0);
            line.lineTo(left + 174.0, bottom - 128.0);
            painter.setPen(QPen(
                withAlpha(QColor(255, 255, 255), 190),
                2.5,
                Qt::SolidLine,
                Qt::RoundCap,
                Qt::RoundJoin));
            painter.setBrush(Qt::NoBrush);
            painter.drawPath(line);
        }

        painter.setClipping(false);
        painter.setPen(withAlpha(materialColor(QtMaterial::ColorRole::Outline), 92));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(bounds, 20.0, 20.0);
    }

private:
    QLabel* m_eyebrow = nullptr;
    QLabel* m_title = nullptr;
    QLabel* m_description = nullptr;
    QtMaterial::ColorRole m_accentRole;
    int m_artwork = 0;
};

class CarouselPanel final : public QWidget
{
public:
    explicit CarouselPanel(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setMinimumHeight(278);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setFocusPolicy(Qt::StrongFocus);
        setAccessibleName(QStringLiteral("Featured dashboard carousel"));

        m_stack = new QStackedWidget(this);

        m_slides.append(new CarouselSlide(
            QStringLiteral("FEATURED APP"),
            QStringLiteral("Build richer desktop experiences"),
            QStringLiteral("Explore a polished Material 3 application shell made with Qt Widgets."),
            QtMaterial::ColorRole::Primary,
            0,
            m_stack));
        m_slides.append(new CarouselSlide(
            QStringLiteral("DESIGN SYSTEM"),
            QStringLiteral("Tokens that follow your theme"),
            QStringLiteral("Switch appearance, contrast and palettes without leaving the showcase."),
            QtMaterial::ColorRole::Tertiary,
            1,
            m_stack));
        m_slides.append(new CarouselSlide(
            QStringLiteral("PRODUCTIVITY"),
            QStringLiteral("Data views ready for real apps"),
            QStringLiteral("Tables, charts, filters and responsive layouts share the same Material language."),
            QtMaterial::ColorRole::Secondary,
            2,
            m_stack));

        for (CarouselSlide* slide : m_slides) {
            m_stack->addWidget(slide);
        }

        m_previous = new QToolButton(this);
        m_next = new QToolButton(this);
        configureArrowButton(m_previous, false);
        configureArrowButton(m_next, true);

        connect(m_previous, &QAbstractButton::clicked, this, [this]() {
            showIndex(m_stack->currentIndex() - 1);
        });
        connect(m_next, &QAbstractButton::clicked, this, [this]() {
            showIndex(m_stack->currentIndex() + 1);
        });

        for (int i = 0; i < m_slides.size(); ++i) {
            auto* indicator = new QToolButton(this);
            indicator->setCursor(Qt::PointingHandCursor);
            indicator->setAutoRaise(true);
            indicator->setAccessibleName(
                QStringLiteral("Show carousel slide %1").arg(i + 1));
            indicator->setToolTip(
                QStringLiteral("Slide %1 of %2").arg(i + 1).arg(m_slides.size()));
            connect(indicator, &QAbstractButton::clicked, this, [this, i]() {
                showIndex(i);
            });
            m_indicators.append(indicator);
        }

        showIndex(0);
    }

    void refreshTheme()
    {
        for (CarouselSlide* slide : m_slides) {
            slide->refreshTheme();
        }
        update();
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QWidget::resizeEvent(event);
        m_stack->setGeometry(rect());
        positionControls();
    }

    void keyPressEvent(QKeyEvent* event) override
    {
        switch (event->key()) {
        case Qt::Key_Left:
            showIndex(m_stack->currentIndex() - 1);
            event->accept();
            return;
        case Qt::Key_Right:
            showIndex(m_stack->currentIndex() + 1);
            event->accept();
            return;
        case Qt::Key_Home:
            showIndex(0);
            event->accept();
            return;
        case Qt::Key_End:
            showIndex(m_slides.size() - 1);
            event->accept();
            return;
        default:
            break;
        }

        QWidget::keyPressEvent(event);
    }

private:
    void configureArrowButton(QToolButton* button, bool next)
    {
        button->setText(QString(QChar(next ? 0x203A : 0x2039)));
        button->setCursor(Qt::PointingHandCursor);
        button->setAutoRaise(true);
        button->setAccessibleName(
            next
                ? QStringLiteral("Next carousel slide")
                : QStringLiteral("Previous carousel slide"));
        button->setToolTip(button->accessibleName());
        button->setStyleSheet(QStringLiteral(
            "QToolButton {"
            " color:#ffffff;"
            " background-color:rgba(8, 15, 28, 150);"
            " border:1px solid rgba(255,255,255,60);"
            " border-radius:17px;"
            " font-size:22px;"
            " font-weight:700;"
            " padding:0px;"
            " }"
            " QToolButton:hover {"
            " background-color:rgba(8, 15, 28, 205);"
            " }"
            " QToolButton:focus {"
            " border:2px solid rgba(255,255,255,210);"
            " }"));
    }

    void showIndex(int index)
    {
        const int count = m_stack->count();
        if (count <= 0) {
            return;
        }

        int normalized = index % count;
        if (normalized < 0) {
            normalized += count;
        }

        m_stack->setCurrentIndex(normalized);
        updateIndicators();
        positionControls();
    }

    void updateIndicators()
    {
        const int current = m_stack->currentIndex();
        for (int i = 0; i < m_indicators.size(); ++i) {
            QToolButton* indicator = m_indicators[i];
            const bool active = i == current;
            indicator->setFixedSize(active ? QSize(18, 7) : QSize(7, 7));
            indicator->setStyleSheet(QStringLiteral(
                "QToolButton {"
                " background-color:%1;"
                " border:0px;"
                " border-radius:3px;"
                " padding:0px;"
                " }"
                " QToolButton:hover { background-color:rgba(255,255,255,235); }")
                .arg(active
                         ? QStringLiteral("rgba(255,255,255,245)")
                         : QStringLiteral("rgba(255,255,255,115)")));
        }
    }

    void positionControls()
    {
        if (!m_previous || !m_next) {
            return;
        }

        const int margin = 16;
        const int arrowSize = 34;
        const int gap = 8;

        m_next->setGeometry(
            width() - margin - arrowSize,
            margin,
            arrowSize,
            arrowSize);
        m_previous->setGeometry(
            width() - margin - arrowSize * 2 - gap,
            margin,
            arrowSize,
            arrowSize);

        int x = 20;
        const int y = 28;
        for (QToolButton* indicator : m_indicators) {
            const QSize size = indicator->size();
            indicator->move(x, y - size.height() / 2);
            x += size.width() + 8;
        }

        m_previous->raise();
        m_next->raise();
        for (QToolButton* indicator : m_indicators) {
            indicator->raise();
        }
    }

    QStackedWidget* m_stack = nullptr;
    QVector<CarouselSlide*> m_slides;
    QVector<QToolButton*> m_indicators;
    QToolButton* m_previous = nullptr;
    QToolButton* m_next = nullptr;
};

} // namespace

DashboardHeroBanners::DashboardHeroBanners(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("dashboardHeroBanners"));

    m_layout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(18);

    auto* welcome = new WelcomeArtwork(this);
    m_welcomeCard = welcome;

    auto* heroLayout = new QHBoxLayout(welcome);
    heroLayout->setContentsMargins(34, 28, 30, 28);
    heroLayout->setSpacing(0);

    auto* copy = new QWidget(welcome);
    copy->setMaximumWidth(520);
    copy->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    auto* copyLayout = new QVBoxLayout(copy);
    copyLayout->setContentsMargins(0, 0, 0, 0);
    copyLayout->setSpacing(8);
    copyLayout->addStretch(1);

    auto* greeting = new QLabel(QStringLiteral("Welcome back"), copy);
    QFont greetingFont = greeting->font();
    greetingFont.setPointSizeF(greetingFont.pointSizeF() + 4.0);
    greetingFont.setBold(true);
    greeting->setFont(greetingFont);

    auto* title = new QLabel(QStringLiteral("QtMaterial Demo"), copy);
    QFont titleFont = title->font();
    titleFont.setPointSizeF(titleFont.pointSizeF() + 6.0);
    titleFont.setBold(true);
    title->setFont(titleFont);

    auto* body = new QLabel(
        QStringLiteral(
            "Build polished desktop experiences with responsive Material 3 components, "
            "real application states and live theming."),
        copy);
    body->setWordWrap(true);
    QFont bodyFont = body->font();
    bodyFont.setPointSizeF(qMax<qreal>(8.0, bodyFont.pointSizeF() - 1.0));
    body->setFont(bodyFont);

    auto* goNow = new QtMaterial::QtMaterialFilledButton(
        QStringLiteral("Go now"),
        copy);
    goNow->setAccessibleName(QStringLiteral("Open dashboard projects"));
    goNow->setToolTip(QStringLiteral("Open projects"));
    goNow->setMinimumWidth(94);
    goNow->setMaximumWidth(126);

    copyLayout->addWidget(greeting);
    copyLayout->addWidget(title);
    copyLayout->addWidget(body);
    copyLayout->addSpacing(8);
    copyLayout->addWidget(goNow, 0, Qt::AlignLeft);
    copyLayout->addStretch(1);

    heroLayout->addWidget(copy, 3);
    heroLayout->addStretch(2);

    const QColor heroText = materialColor(QtMaterial::ColorRole::InverseOnSurface);
    setLabelColor(greeting, heroText);
    setLabelColor(title, heroText);
    setLabelColor(body, withAlpha(heroText, 210));

    connect(goNow, &QAbstractButton::clicked, this, [this]() {
        if (m_goNowHandler) {
            m_goNowHandler();
        }
    });

    auto* carousel = new CarouselPanel(this);
    m_carouselCard = carousel;

    m_layout->addWidget(m_welcomeCard);
    m_layout->addWidget(m_carouselCard);
    m_layout->setStretch(0, 2);
    m_layout->setStretch(1, 1);

    connect(
        &QtMaterial::ThemeManager::instance(),
        &QtMaterial::ThemeManager::themeChanged,
        this,
        [this, welcome, greeting, title, body, carousel](const QtMaterial::Theme&) {
            const QColor text = materialColor(QtMaterial::ColorRole::InverseOnSurface);
            setLabelColor(greeting, text);
            setLabelColor(title, text);
            setLabelColor(body, withAlpha(text, 210));
            welcome->update();
            carousel->refreshTheme();
        });

    updateLayoutMode();
}

void DashboardHeroBanners::setGoNowHandler(const std::function<void()>& handler)
{
    m_goNowHandler = handler;
}

void DashboardHeroBanners::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateLayoutMode();
}

void DashboardHeroBanners::updateLayoutMode()
{
    if (!m_layout) {
        return;
    }

    const bool stacked = width() > 0 && width() < 900;
    const QBoxLayout::Direction direction =
        stacked
            ? QBoxLayout::TopToBottom
            : QBoxLayout::LeftToRight;

    if (m_layout->direction() != direction) {
        m_layout->setDirection(direction);
    }

    m_layout->setStretch(0, stacked ? 1 : 2);
    m_layout->setStretch(1, 1);
}
