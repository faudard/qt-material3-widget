#pragma once
#include <QWidget>
#include "qtmaterial/qtmaterialglobal.h"
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialGridList : public QWidget {
    Q_OBJECT
public:
    explicit QtMaterialGridList(QWidget* parent = nullptr);
    ~QtMaterialGridList() override;
};
