#pragma once
#include <QWidget>
#include "qtmaterial/qtmaterialglobal.h"
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialInputChip : public QWidget {
    Q_OBJECT
public:
    explicit QtMaterialInputChip(QWidget* parent = nullptr);
    ~QtMaterialInputChip() override;
};
