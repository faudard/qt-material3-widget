#include "progressindicatorspage.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>

#include "qtmaterial/specs/qtmaterialprogressindicatorspec.h"
#include "qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h"
#include "qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h"

namespace QtMaterialGallery {
namespace {
QLabel* sectionTitle(const QString& text, QWidget* parent)
{
    auto* label = new QLabel(text, parent);
    QFont font = label->font();
    font.setPointSize(font.pointSize() + 3);
    font.setBold(true);
    label->setFont(font);
    return label;
}

QFrame* card(QWidget* parent)
{
    auto* frame = new QFrame(parent);
    frame->setFrameShape(QFrame::StyledPanel);
    auto* layout = new QVBoxLayout(frame);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(16);
    return frame;
}

void showProgressDialog(QWidget* parent, const QtMaterial::ProgressIndicatorSpec& spec)
{
    using QtMaterial::QtMaterialCircularProgressIndicator;
    using QtMaterial::QtMaterialLinearProgressIndicator;

    auto* dialog = new QDialog(parent);
    dialog->setWindowTitle(QObject::tr("Progress task"));
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    auto* root = new QVBoxLayout(dialog);
    root->setContentsMargins(24, 24, 24, 24);
    root->setSpacing(16);

    auto* title = sectionTitle(QObject::tr("Importing assets"), dialog);
    root->addWidget(title);

    auto* body = new QLabel(QObject::tr("This dialog demonstrates progress inside a blocking task flow."), dialog);
    body->setWordWrap(true);
    root->addWidget(body);

    auto* linear = new QtMaterialLinearProgressIndicator(spec, dialog);
    linear->setValue(0.0);
    linear->setAccessibleName(QObject::tr("Import progress"));
    root->addWidget(linear);

    auto* row = new QHBoxLayout;
    auto* circular = new QtMaterialCircularProgressIndicator(spec, dialog);
    circular->setMode(QtMaterialCircularProgressIndicator::Mode::Indeterminate);
    circular->setAccessibleName(QObject::tr("Import activity"));
    auto* status = new QLabel(QObject::tr("Preparing…"), dialog);
    row->addWidget(circular);
    row->addWidget(status, 1);
    root->addItem(row);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Cancel, dialog);
    root->addWidget(buttons);
    QObject::connect(buttons, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    auto* timer = new QTimer(dialog);
    timer->setInterval(250);
    QObject::connect(timer, &QTimer::timeout, dialog, [dialog, linear, circular, status, timer]() {
        const qreal next = qMin<qreal>(1.0, linear->value() + 0.04);
        linear->setValue(next);
        status->setText(QObject::tr("Imported %1%").arg(qRound(next * 100.0)));
        if (next >= 1.0) {
            timer->stop();
            circular->setMode(QtMaterialCircularProgressIndicator::Mode::Determinate);
            circular->setValue(1.0);
            status->setText(QObject::tr("Done"));
            QTimer::singleShot(500, dialog, &QDialog::accept);
        }
    });
    QObject::connect(dialog, &QDialog::finished, timer, &QTimer::stop);
    timer->start();

    dialog->resize(420, dialog->sizeHint().height());
    dialog->open();
}
} // namespace

ProgressIndicatorsPage::ProgressIndicatorsPage(QWidget* parent)
    : QWidget(parent)
{
    using QtMaterial::ProgressIndicatorSpec;
    using QtMaterial::QtMaterialCircularProgressIndicator;
    using QtMaterial::QtMaterialLinearProgressIndicator;

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 32, 32, 32);
    root->setSpacing(24);

    root->addWidget(sectionTitle(tr("Progress indicators"), this));

    auto* intro = new QLabel(tr("Progress indicators express an ongoing process with determinate, indeterminate, disabled and dialog usage variants."), this);
    intro->setWordWrap(true);
    root->addWidget(intro);

    ProgressIndicatorSpec spec;
    spec.linearHeight = 4;
    spec.circularSize = QSize(48, 48);
    spec.circularStrokeWidth = 4;
    spec.trackGap = 4;
    spec.stopIndicatorSize = 4;

    auto* determinateCard = card(this);
    root->addWidget(determinateCard);
    determinateCard->layout()->addWidget(sectionTitle(tr("Determinate"), determinateCard));

    auto* linear = new QtMaterialLinearProgressIndicator(spec, determinateCard);
    linear->setValue(0.62);
    linear->setAccessibleName(tr("Gallery determinate linear progress"));
    determinateCard->layout()->addWidget(linear);

    auto* circularRow = new QHBoxLayout;
    auto* circular25 = new QtMaterialCircularProgressIndicator(spec, determinateCard);
    circular25->setValue(0.25);
    auto* circular50 = new QtMaterialCircularProgressIndicator(spec, determinateCard);
    circular50->setValue(0.50);
    auto* circular80 = new QtMaterialCircularProgressIndicator(spec, determinateCard);
    circular80->setValue(0.80);
    circularRow->addWidget(circular25);
    circularRow->addWidget(circular50);
    circularRow->addWidget(circular80);
    circularRow->addStretch();
    determinateCard->layout()->addItem(circularRow);

    auto* slider = new QSlider(Qt::Horizontal, determinateCard);
    slider->setRange(0, 100);
    slider->setValue(62);
    connect(slider, &QSlider::valueChanged, this, [linear, circular25, circular50, circular80](int value) {
        const qreal normalized = static_cast<qreal>(value) / 100.0;
        linear->setValue(normalized);
        circular25->setValue(normalized);
        circular50->setValue(normalized);
        circular80->setValue(normalized);
    });
    determinateCard->layout()->addWidget(slider);

    auto* indeterminateCard = card(this);
    root->addWidget(indeterminateCard);
    indeterminateCard->layout()->addWidget(sectionTitle(tr("Indeterminate"), indeterminateCard));

    auto* indeterminateLinear = new QtMaterialLinearProgressIndicator(spec, indeterminateCard);
    indeterminateLinear->setMode(QtMaterialLinearProgressIndicator::Mode::Indeterminate);
    indeterminateCard->layout()->addWidget(indeterminateLinear);

    auto* indeterminateCircular = new QtMaterialCircularProgressIndicator(spec, indeterminateCard);
    indeterminateCircular->setMode(QtMaterialCircularProgressIndicator::Mode::Indeterminate);
    auto* indeterminateCircularRow = new QHBoxLayout;
    indeterminateCircularRow->addWidget(indeterminateCircular);
    indeterminateCircularRow->addStretch();
    indeterminateCard->layout()->addItem(indeterminateCircularRow);

    auto* disabledCard = card(this);
    root->addWidget(disabledCard);
    disabledCard->layout()->addWidget(sectionTitle(tr("Disabled"), disabledCard));
    auto* disabledLinear = new QtMaterialLinearProgressIndicator(spec, disabledCard);
    disabledLinear->setValue(0.42);
    disabledLinear->setEnabled(false);
    disabledCard->layout()->addWidget(disabledLinear);
    auto* disabledCircular = new QtMaterialCircularProgressIndicator(spec, disabledCard);
    disabledCircular->setValue(0.42);
    disabledCircular->setEnabled(false);
    auto* disabledCircularRow = new QHBoxLayout;
    disabledCircularRow->addWidget(disabledCircular);
    disabledCircularRow->addStretch();
    disabledCard->layout()->addItem(disabledCircularRow);

    auto* dialogCard = card(this);
    root->addWidget(dialogCard);
    dialogCard->layout()->addWidget(sectionTitle(tr("Dialog usage"), dialogCard));
    auto* dialogDescription = new QLabel(tr("Use progress indicators in dialogs for task-oriented flows, with cancel support and accessible labels."), dialogCard);
    dialogDescription->setWordWrap(true);
    dialogCard->layout()->addWidget(dialogDescription);
    auto* openDialog = new QPushButton(tr("Show progress dialog"), dialogCard);
    connect(openDialog, &QPushButton::clicked, this, [this, spec]() { showProgressDialog(this, spec); });
    dialogCard->layout()->addWidget(openDialog);

    root->addStretch();
}

} // namespace QtMaterialGallery
