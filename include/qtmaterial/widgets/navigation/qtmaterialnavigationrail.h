#pragma once
#include <QWidget>
#include "qtmaterial/qtmaterialglobal.h"
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialNavigationRail : public QWidget {
    Q_OBJECT
public:
    explicit QtMaterialNavigationRail(QWidget* parent = nullptr);
    ~QtMaterialNavigationRail() override;
};
