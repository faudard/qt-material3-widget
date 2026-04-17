#pragma once
#include <QWidget>
#include "qtmaterial/qtmaterialglobal.h"
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFilterChip : public QWidget {
    Q_OBJECT
public:
    explicit QtMaterialFilterChip(QWidget* parent = nullptr);
    ~QtMaterialFilterChip() override;
};
