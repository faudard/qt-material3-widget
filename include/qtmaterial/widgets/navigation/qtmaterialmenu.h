#pragma once
#include <QWidget>
#include "qtmaterial/qtmaterialglobal.h"
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialMenu : public QWidget {
    Q_OBJECT
public:
    explicit QtMaterialMenu(QWidget* parent = nullptr);
    ~QtMaterialMenu() override;
};
