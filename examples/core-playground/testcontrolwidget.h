#pragma once
#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
class TestControlWidget : public QtMaterial::QtMaterialControl
{
    Q_OBJECT
public:
    explicit TestControlWidget(QWidget* parent = nullptr);
    ~TestControlWidget() override;
protected:
    void paintEvent(QPaintEvent* event) override;
    void invalidateResolvedSpec() override;
private:
    QtMaterial::ButtonSpec m_spec;
};
