#include "qtmaterial/widgets/inputs/qtmaterialtimepicker.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

#include "qtmaterial/widgets/inputs/qtmaterialtimefield.h"

namespace QtMaterial {

QtMaterialTimePicker::QtMaterialTimePicker(QWidget* parent)
    : QDialog(parent)
    , m_timeField(new QtMaterialTimeField(this))
{
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
    layout->addWidget(m_timeField);
    layout->addWidget(buttons);

    connect(m_timeField, &QTimeEdit::timeChanged, this, &QtMaterialTimePicker::selectedTimeChanged);
    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        emit timeAccepted(selectedTime());
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QtMaterialTimePicker::~QtMaterialTimePicker() = default;

QTime QtMaterialTimePicker::selectedTime() const { return m_timeField->time(); }

void QtMaterialTimePicker::setSelectedTime(const QTime& time)
{
    if (time.isValid()) {
        m_timeField->setTime(time);
    }
}

QtMaterialTimeField* QtMaterialTimePicker::timeField() const noexcept { return m_timeField; }

} // namespace QtMaterial
