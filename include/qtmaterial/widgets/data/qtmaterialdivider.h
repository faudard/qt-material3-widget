#pragma once
#include <QWidget>
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialDivider : public QWidget
{
    Q_OBJECT
public:
    explicit QtMaterialDivider(QWidget* parent = nullptr);
    ~QtMaterialDivider() override;
protected:
    void paintEvent(QPaintEvent* event) override;
};
} // namespace QtMaterial
