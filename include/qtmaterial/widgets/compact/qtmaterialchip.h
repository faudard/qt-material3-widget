#pragma once
#include <QWidget>
#include "qtmaterial/qtmaterialglobal.h"
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialChip : public QWidget {
    Q_OBJECT
public:
    explicit QtMaterialChip(QWidget* parent = nullptr);
    ~QtMaterialChip() override;
};
