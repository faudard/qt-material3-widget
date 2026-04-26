#pragma once

#include <QColor>
#include <QPointer>
#include <QWidget>

#include "qtmaterial/theme/qtmaterialcolortoken.h"
#include "qtmaterial/theme/qtmaterialsystemtheme.h"

class QButtonGroup;
class QCheckBox;
class QComboBox;
class QFrame;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QRadioButton;
class QScrollArea;
class QTabWidget;
class QVBoxLayout;

class ThemePreviewWindow : public QWidget {
    Q_OBJECT
public:
    explicit ThemePreviewWindow(QWidget* parent = nullptr);

private slots:
    void chooseSeedColor();
    void updateModePreferenceFromUi();
    void updateContrastFromUi();
    void exportThemeJson();
    void importThemeJson();
    void copyThemeJson();
    void copyCppSnippet();
    void copyTokenReport();
    void exportScreenshot();
    void refreshPreview();

private:
    void buildControls(QVBoxLayout* root);
    void buildPreviewTabs(QVBoxLayout* root);
    QWidget* buildComponentPreview();
    QWidget* buildTypographyPreview();
    QWidget* buildShapePreview();
    QWidget* buildElevationPreview();
    QWidget* buildStateLayerPreview();
    QWidget* panel(const QString& title, QWidget* body) const;

    QString cppSnippet() const;
    QString tokenReport() const;
    QString modeLabelText() const;
    QString contrastLabelText() const;
    QString colorName(QtMaterial::ColorRole role) const;
    void setColorChip(QFrame* chip, const QColor& fill, const QColor& border);
    QLabel* roleChip(const QString& name, const QColor& fill, const QColor& text, QWidget* parent) const;
    void applyPlaygroundStyle();

    QLabel* m_titleLabel = nullptr;
    QLabel* m_statusLabel = nullptr;
    QPushButton* m_seedButton = nullptr;
    QPushButton* m_exportButton = nullptr;
    QPushButton* m_importButton = nullptr;
    QPushButton* m_copyJsonButton = nullptr;
    QPushButton* m_copyCppButton = nullptr;
    QPushButton* m_copyTokensButton = nullptr;
    QPushButton* m_screenshotButton = nullptr;
    QRadioButton* m_lightRadio = nullptr;
    QRadioButton* m_darkRadio = nullptr;
    QRadioButton* m_followSystemRadio = nullptr;
    QComboBox* m_contrastCombo = nullptr;
    QCheckBox* m_platformFontCheck = nullptr;
    QTabWidget* m_tabs = nullptr;
    QPlainTextEdit* m_jsonPreview = nullptr;

    QFrame* m_primaryChip = nullptr;
    QFrame* m_secondaryChip = nullptr;
    QFrame* m_tertiaryChip = nullptr;
    QFrame* m_errorChip = nullptr;
    QFrame* m_surfaceChip = nullptr;
};
