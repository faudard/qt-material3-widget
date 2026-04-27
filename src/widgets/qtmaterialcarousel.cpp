#include "qtmaterial/widgets/qtmaterialcarousel.h"

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

namespace QtMaterial {

QtMaterialCarousel::QtMaterialCarousel(QWidget* parent)
    : QWidget(parent), m_spec(defaultCarouselSpec())
{
    setObjectName(QStringLiteral("QtMaterialCarousel"));
    setFocusPolicy(Qt::StrongFocus);
    buildUi();
    applySpec();
    updateControls();
}

QtMaterialCarousel::~QtMaterialCarousel() = default;

int QtMaterialCarousel::count() const
{
    return m_stack->count();
}

int QtMaterialCarousel::currentIndex() const
{
    return m_stack->currentIndex();
}

bool QtMaterialCarousel::wrap() const
{
    return m_wrap;
}

void QtMaterialCarousel::setWrap(bool wrap)
{
    if (m_wrap == wrap) {
        return;
    }
    m_wrap = wrap;
    updateControls();
}

void QtMaterialCarousel::addPage(QWidget* page, const QString& label)
{
    if (!page) {
        return;
    }
    m_stack->addWidget(page);
    m_labels.append(label.isEmpty() ? tr("Page %1").arg(m_stack->count()) : label);

    auto* dot = new QPushButton(m_indicatorHost);
    dot->setCheckable(true);
    dot->setFixedSize(qMax(16, m_spec.indicatorSize * 2), qMax(16, m_spec.indicatorSize * 2));
    dot->setAccessibleName(tr("Show %1").arg(m_labels.last()));
    const int index = m_stack->count() - 1;
    connect(dot, &QPushButton::clicked, this, [this, index]() { setCurrentIndex(index); });
    m_indicatorLayout->addWidget(dot);

    updateControls();
}

QWidget* QtMaterialCarousel::pageAt(int index) const
{
    return m_stack->widget(index);
}

QString QtMaterialCarousel::pageLabel(int index) const
{
    return index >= 0 && index < m_labels.size() ? m_labels.at(index) : QString();
}

CarouselSpec QtMaterialCarousel::spec() const
{
    return m_spec;
}

void QtMaterialCarousel::setSpec(const CarouselSpec& spec)
{
    m_spec = spec;
    applySpec();
    update();
}

void QtMaterialCarousel::setCurrentIndex(int index)
{
    if (count() == 0) {
        return;
    }
    if (m_wrap) {
        if (index < 0) {
            index = count() - 1;
        } else if (index >= count()) {
            index = 0;
        }
    }
    index = qBound(0, index, count() - 1);
    if (index == currentIndex()) {
        return;
    }
    m_stack->setCurrentIndex(index);
    updateControls();
    emit currentIndexChanged(index);
    emit pageActivated(index);
}

void QtMaterialCarousel::next()
{
    setCurrentIndex(currentIndex() + 1);
}

void QtMaterialCarousel::previous()
{
    setCurrentIndex(currentIndex() - 1);
}

void QtMaterialCarousel::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_Left:
    case Qt::Key_Up:
        previous();
        event->accept();
        return;
    case Qt::Key_Right:
    case Qt::Key_Down:
        next();
        event->accept();
        return;
    case Qt::Key_Home:
        setCurrentIndex(0);
        event->accept();
        return;
    case Qt::Key_End:
        setCurrentIndex(count() - 1);
        event->accept();
        return;
    default:
        break;
    }
    QWidget::keyPressEvent(event);
}

void QtMaterialCarousel::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_spec.backgroundColor);
    painter.drawRoundedRect(rect().adjusted(0, 0, -1, -1), m_spec.cornerRadius, m_spec.cornerRadius);

    if (hasFocus()) {
        painter.setPen(QPen(m_spec.focusRingColor, m_spec.focusRingWidth));
        painter.setBrush(Qt::NoBrush);
        painter.drawRoundedRect(rect().adjusted(2, 2, -3, -3), m_spec.cornerRadius, m_spec.cornerRadius);
    }
}

void QtMaterialCarousel::buildUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(m_spec.pageSpacing);

    auto* body = new QHBoxLayout();
    body->setContentsMargins(0, 0, 0, 0);
    body->setSpacing(8);

    m_previousButton = new QPushButton(QStringLiteral("‹"), this);
    m_previousButton->setAccessibleName(tr("Previous page"));
    connect(m_previousButton, &QPushButton::clicked, this, &QtMaterialCarousel::previous);

    m_stack = new QStackedWidget(this);
    m_stack->setMinimumSize(m_spec.preferredPageSize);

    m_nextButton = new QPushButton(QStringLiteral("›"), this);
    m_nextButton->setAccessibleName(tr("Next page"));
    connect(m_nextButton, &QPushButton::clicked, this, &QtMaterialCarousel::next);

    body->addWidget(m_previousButton);
    body->addWidget(m_stack, 1);
    body->addWidget(m_nextButton);
    root->addLayout(body, 1);

    m_indicatorHost = new QWidget(this);
    m_indicatorLayout = new QHBoxLayout(m_indicatorHost);
    m_indicatorLayout->setContentsMargins(0, 0, 0, 0);
    m_indicatorLayout->setSpacing(6);
    m_indicatorLayout->addStretch(1);
    root->addWidget(m_indicatorHost, 0, Qt::AlignHCenter);
}

void QtMaterialCarousel::applySpec()
{
    const QString navStyle = QStringLiteral(
        "QPushButton { min-width: 40px; min-height: 40px; border:0; border-radius:20px; background:%1; color:%2; font-weight:600; }"
        "QPushButton:disabled { background:%3; color:%4; }")
        .arg(m_spec.navigationButtonColor.name(),
             m_spec.navigationButtonTextColor.name(),
             m_spec.pageIndicatorColor.name(),
             m_spec.foregroundColor.name());
    m_previousButton->setStyleSheet(navStyle);
    m_nextButton->setStyleSheet(navStyle);
    m_previousButton->setFont(m_spec.labelFont);
    m_nextButton->setFont(m_spec.labelFont);

    for (int i = 0; i < m_indicatorLayout->count(); ++i) {
        if (auto* button = qobject_cast<QPushButton*>(m_indicatorLayout->itemAt(i)->widget())) {
            button->setStyleSheet(QStringLiteral(
                "QPushButton { border:0; border-radius:%1px; background:%2; }"
                "QPushButton:checked { background:%3; }")
                .arg(m_spec.indicatorSize / 2)
                .arg(m_spec.pageIndicatorColor.name(), m_spec.activePageIndicatorColor.name()));
        }
    }
    updateControls();
}

void QtMaterialCarousel::updateControls()
{
    const bool hasPages = count() > 0;
    const int index = currentIndex();
    m_previousButton->setEnabled(hasPages && (m_wrap || index > 0));
    m_nextButton->setEnabled(hasPages && (m_wrap || index < count() - 1));

    int dotIndex = 0;
    for (int i = 0; i < m_indicatorLayout->count(); ++i) {
        if (auto* button = qobject_cast<QPushButton*>(m_indicatorLayout->itemAt(i)->widget())) {
            button->setChecked(dotIndex == index);
            ++dotIndex;
        }
    }

    setAccessibleName(hasPages ? tr("Carousel, %1 of %2, %3").arg(index + 1).arg(count()).arg(pageLabel(index)) : tr("Carousel"));
}

} // namespace QtMaterial
