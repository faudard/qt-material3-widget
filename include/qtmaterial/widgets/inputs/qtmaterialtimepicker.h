#pragma once

#include <memory>

#include <QDialog>
#include <QTime>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QtMaterialTimeField;
class QtMaterialTimePickerPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTimePicker : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(QTime selectedTime READ selectedTime WRITE setSelectedTime NOTIFY selectedTimeChanged)

public:
    explicit QtMaterialTimePicker(QWidget* parent = nullptr);
    ~QtMaterialTimePicker() override;

    QTime selectedTime() const;
    void setSelectedTime(const QTime& time);

    QtMaterialTimeField* timeField() const noexcept;

signals:
    void selectedTimeChanged(const QTime& time);
    void timeAccepted(const QTime& time);

private:
    std::unique_ptr<QtMaterialTimePickerPrivate> d_ptr;
};

} // namespace QtMaterial
