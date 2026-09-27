#include "qtmaterial/widgets/inputs/qtmaterialtimepicker.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

#include "qtmaterial/widgets/inputs/qtmaterialtimefield.h"

namespace QtMaterial {

class QtMaterialTimePickerPrivate final
{
public:
    QtMaterialTimeField* timeField = nullptr;
};

QtMaterialTimePicker::QtMaterialTimePicker(QWidget* parent)
    : QDialog(parent)
    , d_ptr(std::make_unique<QtMaterialTimePickerPrivate>())
{
    d_ptr->timeField = new QtMaterialTimeField(this);
    setObjectName(QStringLiteral("qtmaterial_time_picker"));
    setWindowTitle(tr("Choose time"));
    setModal(true);
    setAccessibleName(tr("Time picker"));

    auto* title = new QLabel(tr("Choose time"), this);
    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal,
        this);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(title);
    layout->addWidget(d_ptr->timeField);
    layout->addWidget(buttons);

    connect(d_ptr->timeField, &QTimeEdit::timeChanged, this, &QtMaterialTimePicker::selectedTimeChanged);
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        emit timeAccepted(selectedTime());
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QtMaterialTimePicker::~QtMaterialTimePicker() = default;

QTime QtMaterialTimePicker::selectedTime() const { return d_ptr->timeField->time(); }

void QtMaterialTimePicker::setSelectedTime(const QTime& time)
{
    if (time.isValid()) {
        d_ptr->timeField->setTime(time);
    }
}

QtMaterialTimeField* QtMaterialTimePicker::timeField() const noexcept { return d_ptr->timeField; }

} // namespace QtMaterial
