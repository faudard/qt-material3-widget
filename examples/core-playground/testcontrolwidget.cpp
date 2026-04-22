#include "testcontrolwidget.h"

#include <QLabel>
#include <QPainter>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"

TestControlWidget::TestControlWidget(QWidget* parent)
    : QtMaterial::QtMaterialControl(parent)
    , m_spec()
{
    invalidateResolvedSpec();
    setMinimumSize(900, 640);
    buildUi();
}

TestControlWidget::~TestControlWidget() = default;

void TestControlWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), QColor(248, 245, 250));

    painter.setBrush(m_spec.containerColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(QRect(24, 24, width() - 48, 120), 24, 24);

    painter.setPen(m_spec.labelColor);
    painter.drawText(QRect(40, 40, width() - 80, 80),
                     Qt::AlignLeft | Qt::AlignVCenter,
                     QStringLiteral("Bottom sheet debug harness"));
}

void TestControlWidget::invalidateResolvedSpec()
{
    QtMaterial::SpecFactory factory;
    m_spec = factory.filledButtonSpec(theme(), density());
}

void TestControlWidget::buildUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(24, 160, 24, 24);
    root->setSpacing(12);

    auto* row = new QHBoxLayout;
    row->setSpacing(12);

    m_openButton = new QPushButton(QStringLiteral("Open"), this);
    m_closeButton = new QPushButton(QStringLiteral("Close"), this);
    m_progressLabel = new QLabel(QStringLiteral("progress: 0.000"), this);
    m_stateLabel = new QLabel(QStringLiteral("state: Closed"), this);
    m_progressBar = new QProgressBar(this);

    m_progressBar->setRange(0, 1000);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);

    row->addWidget(m_openButton);
    row->addWidget(m_closeButton);
    row->addSpacing(12);
    row->addWidget(m_progressLabel);
    row->addSpacing(12);
    row->addWidget(m_stateLabel);
    row->addStretch(1);

    root->addLayout(row);
    root->addWidget(m_progressBar);

    m_sheet = new QtMaterial::QtMaterialBottomSheet(this);
    m_sheet->setModal(false);
    m_sheet->setExpandedHeight(320);

    QWidget* sheetContent = m_sheet->contentWidget();

    auto* sheetLayout = new QVBoxLayout(sheetContent);
    sheetLayout->setContentsMargins(24, 24, 24, 24);
    sheetLayout->setSpacing(12);

    auto* title = new QLabel(QStringLiteral("Debug bottom sheet"), sheetContent);
    auto* body = new QLabel(
        QStringLiteral("This content should stay inside the bottom panel, "
                       "not at the top of the fullscreen overlay."),
        sheetContent);
    body->setWordWrap(true);

    auto* closeInside = new QPushButton(QStringLiteral("Dismiss"), sheetContent);

    sheetLayout->addWidget(title);
    sheetLayout->addWidget(body);
    sheetLayout->addStretch(1);
    sheetLayout->addWidget(closeInside);

    connect(m_openButton, &QPushButton::clicked, m_sheet, &QtMaterial::QtMaterialBottomSheet::open);
    connect(m_closeButton, &QPushButton::clicked, m_sheet, &QtMaterial::QtMaterialBottomSheet::close);
    connect(closeInside, &QPushButton::clicked, m_sheet, &QtMaterial::QtMaterialBottomSheet::close);

    connect(m_sheet, &QtMaterial::QtMaterialBottomSheet::progressChanged,
            this, [this](qreal value) {
                m_progressLabel->setText(QStringLiteral("progress: %1")
                                             .arg(value, 0, 'f', 3));
                m_progressBar->setValue(qRound(value * 1000.0));
            });

    connect(m_sheet, &QtMaterial::QtMaterialBottomSheet::stateChanged,
            this, [this](QtMaterial::QtMaterialBottomSheet::SheetState state) {
                QString text = QStringLiteral("Closed");
                switch (state) {
                case QtMaterial::QtMaterialBottomSheet::SheetState::Closed:
                    text = QStringLiteral("Closed");
                    break;
                case QtMaterial::QtMaterialBottomSheet::SheetState::Opening:
                    text = QStringLiteral("Opening");
                    break;
                case QtMaterial::QtMaterialBottomSheet::SheetState::Open:
                    text = QStringLiteral("Open");
                    break;
                case QtMaterial::QtMaterialBottomSheet::SheetState::Closing:
                    text = QStringLiteral("Closing");
                    break;
                }
                m_stateLabel->setText(QStringLiteral("state: %1").arg(text));
            });
}