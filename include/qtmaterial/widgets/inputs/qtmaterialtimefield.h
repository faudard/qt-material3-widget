#pragma once

#include <QTimeEdit>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTimeField : public QTimeEdit
{
    Q_OBJECT

public:
    explicit QtMaterialTimeField(QWidget* parent = nullptr);
    ~QtMaterialTimeField() override;
};

} // namespace QtMaterial
