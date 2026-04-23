#pragma once

#include "qtmaterial/core/qtmaterialcontrol.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"

class QLabel;
class QProgressBar;
class QPushButton;
class QLineEdit;
class QCheckBox;
class QSpinBox;

namespace QtMaterial {
class QtMaterialBottomSheet;
class QtMaterialSnackbarHost;
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

    // BottomSheet debug
    QLabel* m_progressLabel = nullptr;
    QLabel* m_stateLabel = nullptr;
    QProgressBar* m_progressBar = nullptr;
    QPushButton* m_openButton = nullptr;
    QPushButton* m_closeButton = nullptr;
    QtMaterial::QtMaterialBottomSheet* m_sheet = nullptr;

    // Snackbar debug
    QLineEdit* m_snackbarTextEdit = nullptr;
    QLineEdit* m_snackbarActionEdit = nullptr;
    QCheckBox* m_snackbarDismissCheck = nullptr;
    QSpinBox* m_snackbarRepeatSpin = nullptr;
    QPushButton* m_showSnackbarButton = nullptr;
    QPushButton* m_showActionSnackbarButton = nullptr;
    QPushButton* m_replaceSnackbarButton = nullptr;
    QPushButton* m_dismissSnackbarButton = nullptr;
    QLabel* m_snackbarStatusLabel = nullptr;

    QtMaterial::QtMaterialSnackbarHost* m_snackbarHost = nullptr;
};