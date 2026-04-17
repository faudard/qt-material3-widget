#pragma once
#include <QWidget>
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialList : public QWidget
{
    Q_OBJECT
public:
    explicit QtMaterialList(QWidget* parent = nullptr);
    ~QtMaterialList() override;
};
} // namespace QtMaterial
