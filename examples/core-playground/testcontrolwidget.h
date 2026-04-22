#pragma once

#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"

namespace QtMaterial {
class QtMaterialBottomSheet;
}

class QLabel;
class QProgressBar;
class QPushButton;

namespace QtMaterial {
class QtMaterialBottomSheet;
}

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
    void buildUi();

private:
    QtMaterial::ButtonSpec m_spec;

    QLabel* m_progressLabel = nullptr;
    QLabel* m_stateLabel = nullptr;
    QProgressBar* m_progressBar = nullptr;
    QPushButton* m_openButton = nullptr;
    QPushButton* m_closeButton = nullptr;

    QtMaterial::QtMaterialBottomSheet* m_sheet = nullptr;
};