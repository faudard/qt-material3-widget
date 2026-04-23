#include "testcontrolwidget.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>
#include <QGroupBox>

#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "qtmaterial/widgets/surfaces/qtmaterialbottomsheet.h"
#include "qtmaterial/widgets/surfaces/qtmaterialsnackbar.h"
#include "qtmaterial/widgets/surfaces/qtmaterialsnackbarhost.h"

TestControlWidget::TestControlWidget(QWidget* parent)
    : QtMaterial::QtMaterialControl(parent)
    , m_spec()
{
    invalidateResolvedSpec();
    setMinimumSize(980, 760);
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
                     QStringLiteral("Transient surfaces debug harness"));
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
    root->setSpacing(16);

    // ----- BottomSheet group -----
    auto* bottomSheetGroup = new QGroupBox(QStringLiteral("BottomSheet"), this);
    auto* bsRoot = new QVBoxLayout(bottomSheetGroup);
    bsRoot->setSpacing(12);

    auto* bsRow = new QHBoxLayout;
    bsRow->setSpacing(12);

    m_openButton = new QPushButton(QStringLiteral("Open"), bottomSheetGroup);
    m_closeButton = new QPushButton(QStringLiteral("Close"), bottomSheetGroup);
    m_progressLabel = new QLabel(QStringLiteral("progress: 0.000"), bottomSheetGroup);
    m_stateLabel = new QLabel(QStringLiteral("state: Closed"), bottomSheetGroup);
    m_progressBar = new QProgressBar(bottomSheetGroup);

    m_progressBar->setRange(0, 1000);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);

    bsRow->addWidget(m_openButton);
    bsRow->addWidget(m_closeButton);
    bsRow->addSpacing(12);
    bsRow->addWidget(m_progressLabel);
    bsRow->addSpacing(12);
    bsRow->addWidget(m_stateLabel);
    bsRow->addStretch(1);

    bsRoot->addLayout(bsRow);
    bsRoot->addWidget(m_progressBar);

    root->addWidget(bottomSheetGroup);

    // ----- Snackbar group -----
    auto* snackbarGroup = new QGroupBox(QStringLiteral("Snackbar"), this);
    auto* sbRoot = new QVBoxLayout(snackbarGroup);
    sbRoot->setSpacing(12);

    auto* sbFormRow1 = new QHBoxLayout;
    sbFormRow1->setSpacing(12);

    m_snackbarTextEdit = new QLineEdit(snackbarGroup);
    m_snackbarActionEdit = new QLineEdit(snackbarGroup);
    m_snackbarDismissCheck = new QCheckBox(QStringLiteral("Dismiss button"), snackbarGroup);
    m_snackbarRepeatSpin = new QSpinBox(snackbarGroup);

    m_snackbarTextEdit->setPlaceholderText(QStringLiteral("Snackbar text"));
    m_snackbarTextEdit->setText(QStringLiteral("Draft saved"));
    m_snackbarActionEdit->setPlaceholderText(QStringLiteral("Action text"));
    m_snackbarActionEdit->setText(QStringLiteral("Undo"));
    m_snackbarRepeatSpin->setRange(1, 10);
    m_snackbarRepeatSpin->setValue(3);

    sbFormRow1->addWidget(new QLabel(QStringLiteral("Text:"), snackbarGroup));
    sbFormRow1->addWidget(m_snackbarTextEdit, 1);
    sbFormRow1->addWidget(new QLabel(QStringLiteral("Action:"), snackbarGroup));
    sbFormRow1->addWidget(m_snackbarActionEdit, 1);
    sbFormRow1->addWidget(m_snackbarDismissCheck);
    sbFormRow1->addWidget(new QLabel(QStringLiteral("Burst:"), snackbarGroup));
    sbFormRow1->addWidget(m_snackbarRepeatSpin);

    auto* sbButtonsRow = new QHBoxLayout;
    sbButtonsRow->setSpacing(12);

    m_showSnackbarButton = new QPushButton(QStringLiteral("Show"), snackbarGroup);
    m_showActionSnackbarButton = new QPushButton(QStringLiteral("Show with action"), snackbarGroup);
    m_replaceSnackbarButton = new QPushButton(QStringLiteral("Replace current"), snackbarGroup);
    m_dismissSnackbarButton = new QPushButton(QStringLiteral("Dismiss current"), snackbarGroup);
    m_snackbarStatusLabel = new QLabel(QStringLiteral("status: idle"), snackbarGroup);

    sbButtonsRow->addWidget(m_showSnackbarButton);
    sbButtonsRow->addWidget(m_showActionSnackbarButton);
    sbButtonsRow->addWidget(m_replaceSnackbarButton);
    sbButtonsRow->addWidget(m_dismissSnackbarButton);
    sbButtonsRow->addSpacing(12);
    sbButtonsRow->addWidget(m_snackbarStatusLabel);
    sbButtonsRow->addStretch(1);

    sbRoot->addLayout(sbFormRow1);
    sbRoot->addLayout(sbButtonsRow);

    root->addWidget(snackbarGroup);
    root->addStretch(1);

    // ----- BottomSheet instance -----
    m_sheet = new QtMaterial::QtMaterialBottomSheet(this);
    m_sheet->setModal(false);
    m_sheet->setExpandedHeight(320);

    QWidget* sheetContent = m_sheet->contentWidget();
    auto* sheetLayout = new QVBoxLayout(sheetContent);
    sheetLayout->setContentsMargins(24, 24, 24, 24);
    sheetLayout->setSpacing(12);

    auto* title = new QLabel(QStringLiteral("Debug bottom sheet"), sheetContent);
    auto* body = new QLabel(
        QStringLiteral("This content should stay inside the bottom panel and follow "
                       "the animation, masking and parent resize behavior."),
        sheetContent);
    body->setWordWrap(true);

    auto* closeInside = new QPushButton(QStringLiteral("Dismiss"), sheetContent);

    sheetLayout->addWidget(title);
    sheetLayout->addWidget(body);
    sheetLayout->addStretch(1);
    sheetLayout->addWidget(closeInside);

    // ----- Snackbar host -----
    m_snackbarHost = new QtMaterial::QtMaterialSnackbarHost(this, this);

    // BottomSheet wiring
    connect(m_openButton, &QPushButton::clicked,
            m_sheet, &QtMaterial::QtMaterialBottomSheet::open);
    connect(m_closeButton, &QPushButton::clicked,
            m_sheet, &QtMaterial::QtMaterialBottomSheet::close);
    connect(closeInside, &QPushButton::clicked,
            m_sheet, &QtMaterial::QtMaterialBottomSheet::close);

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

    // Snackbar wiring
    connect(m_showSnackbarButton, &QPushButton::clicked, this, [this]() {
        QtMaterial::SnackbarRequest req;
        req.text = m_snackbarTextEdit->text().trimmed().isEmpty()
                       ? QStringLiteral("Draft saved")
                       : m_snackbarTextEdit->text();
        req.actionText.clear();
        req.duration = QtMaterial::SnackbarDuration::Short;
        req.showDismissButton = m_snackbarDismissCheck->isChecked();

        m_snackbarHost->showMessage(req);
        m_snackbarStatusLabel->setText(QStringLiteral("status: show"));
    });

    connect(m_showActionSnackbarButton, &QPushButton::clicked, this, [this]() {
        const int burst = m_snackbarRepeatSpin->value();

        for (int i = 0; i < burst; ++i) {
            QtMaterial::SnackbarRequest req;
            req.text = QStringLiteral("%1 #%2")
                           .arg(m_snackbarTextEdit->text().trimmed().isEmpty()
                                    ? QStringLiteral("Draft saved")
                                    : m_snackbarTextEdit->text())
                           .arg(i + 1);
            req.actionText = m_snackbarActionEdit->text().trimmed().isEmpty()
                                 ? QStringLiteral("Undo")
                                 : m_snackbarActionEdit->text();
            req.duration = QtMaterial::SnackbarDuration::Short;
            req.showDismissButton = m_snackbarDismissCheck->isChecked();

            m_snackbarHost->showMessage(req);
        }

        m_snackbarStatusLabel->setText(QStringLiteral("status: queued burst"));
    });

    connect(m_replaceSnackbarButton, &QPushButton::clicked, this, [this]() {
        QtMaterial::SnackbarRequest req;
        req.text = QStringLiteral("Replaced: %1")
                       .arg(m_snackbarTextEdit->text().trimmed().isEmpty()
                                ? QStringLiteral("Draft saved")
                                : m_snackbarTextEdit->text());
        req.actionText = m_snackbarActionEdit->text().trimmed();
        req.duration = QtMaterial::SnackbarDuration::Long;
        req.showDismissButton = true;

        m_snackbarHost->showMessage(req, true);
        m_snackbarStatusLabel->setText(QStringLiteral("status: replace requested"));
    });

    connect(m_dismissSnackbarButton, &QPushButton::clicked, this, [this]() {
        m_snackbarHost->dismissCurrent();
        m_snackbarStatusLabel->setText(QStringLiteral("status: dismiss current"));
    });

    if (m_snackbarHost->snackbar()) {
        connect(m_snackbarHost->snackbar(), &QtMaterial::QtMaterialSnackbar::shown,
                this, [this]() {
                    m_snackbarStatusLabel->setText(QStringLiteral("status: shown"));
                });

        connect(m_snackbarHost->snackbar(), &QtMaterial::QtMaterialSnackbar::actionTriggered,
                this, [this]() {
                    m_snackbarStatusLabel->setText(QStringLiteral("status: action triggered"));
                });

        connect(m_snackbarHost->snackbar(), &QtMaterial::QtMaterialSnackbar::dismissed,
                this, [this](QtMaterial::SnackbarDismissReason reason) {
                    QString text = QStringLiteral("dismissed");
                    switch (reason) {
                    case QtMaterial::SnackbarDismissReason::Timeout:
                        text = QStringLiteral("dismissed: timeout");
                        break;
                    case QtMaterial::SnackbarDismissReason::Action:
                        text = QStringLiteral("dismissed: action");
                        break;
                    case QtMaterial::SnackbarDismissReason::Manual:
                        text = QStringLiteral("dismissed: manual");
                        break;
                    case QtMaterial::SnackbarDismissReason::Consecutive:
                        text = QStringLiteral("dismissed: consecutive");
                        break;
                    }
                    m_snackbarStatusLabel->setText(QStringLiteral("status: %1").arg(text));
                });
    }
}