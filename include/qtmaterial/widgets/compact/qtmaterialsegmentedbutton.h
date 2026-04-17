#pragma once
#include <QWidget>
#include "qtmaterial/qtmaterialglobal.h"
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSegmentedButton : public QWidget {
    Q_OBJECT
public:
    explicit QtMaterialSegmentedButton(QWidget* parent = nullptr);
    ~QtMaterialSegmentedButton() override;
};
