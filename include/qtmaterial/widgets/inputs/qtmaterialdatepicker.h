#pragma once
#include <QWidget>
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial { class DatePickerSpec; }
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialDatePicker : public QWidget {
    Q_OBJECT
public:
    explicit QtMaterialDatePicker(QWidget* parent = nullptr);
    ~QtMaterialDatePicker() override;
};
