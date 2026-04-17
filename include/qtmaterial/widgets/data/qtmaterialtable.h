#pragma once
#include <QWidget>
#include "qtmaterial/qtmaterialglobal.h"
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTable : public QWidget {
    Q_OBJECT
public:
    explicit QtMaterialTable(QWidget* parent = nullptr);
    ~QtMaterialTable() override;
};
