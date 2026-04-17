#pragma once
#include <QWidget>
#include "qtmaterial/qtmaterialglobal.h"
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCarousel : public QWidget {
    Q_OBJECT
public:
    explicit QtMaterialCarousel(QWidget* parent = nullptr);
    ~QtMaterialCarousel() override;
};
