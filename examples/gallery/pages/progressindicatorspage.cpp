#include "progressindicatorspage.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
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

    auto* intro = new QLabel(tr("Progress indicators express an ongoing process with determinate and indeterminate variants."), this);
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

    root->addStretch();
}

} // namespace QtMaterialGallery
