#pragma once
#include <QObject>
#include "qtmaterial/qtmaterialglobal.h"
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialMenuItem : public QObject {
    Q_OBJECT
public:
    explicit QtMaterialMenuItem(QObject* parent = nullptr);
    ~QtMaterialMenuItem() override;
};
