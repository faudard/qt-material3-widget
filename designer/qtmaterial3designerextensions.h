#pragma once

#include <QList>
#include <QObject>
#include <QStringList>
#include <Qt>

#include <QtDesigner/QDesignerContainerExtension>

class QDesignerFormEditorInterface;
class QWidget;

namespace QtMaterial {
class QtMaterialAdaptiveShell;
class QtMaterialTabs;
}

namespace QtMaterial3Designer {

enum class PreviewMode
{
    Light,
    Dark,
    Expressive
};

enum class PreviewWidth
{
    Current,
    Compact,
    Medium,
    Expanded
};

QStringList editablePropertyNames(const QWidget* widget);
bool propertyCanReset(const QWidget* widget, const QString& propertyName);
bool resetPropertyToDefault(QWidget* widget, const QString& propertyName);

QStringList designerThemePresetIds();
bool applyThemePreset(const QString& presetId);
int previewLogicalWidth(PreviewWidth width);

void applyPreviewMode(PreviewMode mode);
void restorePreviewMode();

void registerExtensions(QDesignerFormEditorInterface* core);

class TabsContainerExtension final
    : public QObject
    , public QDesignerContainerExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerContainerExtension)

public:
    explicit TabsContainerExtension(
        QtMaterial::QtMaterialTabs* tabs,
        QObject* parent = nullptr);

    int count() const override;
    QWidget* widget(int index) const override;
    int currentIndex() const override;
    void setCurrentIndex(int index) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool canAddWidget() const override;
    bool canRemove(int index) const override;
#endif
    void addWidget(QWidget* widget) override;
    void insertWidget(int index, QWidget* widget) override;
    void remove(int index) override;

private:
    QtMaterial::QtMaterialTabs* tabs_ = nullptr;
};

class AdaptiveShellContainerExtension final
    : public QObject
    , public QDesignerContainerExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerContainerExtension)

public:
    explicit AdaptiveShellContainerExtension(
        QtMaterial::QtMaterialAdaptiveShell* shell,
        QObject* parent = nullptr);

    int count() const override;
    QWidget* widget(int index) const override;
    int currentIndex() const override;
    void setCurrentIndex(int index) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    bool canAddWidget() const override;
    bool canRemove(int index) const override;
#endif
    void addWidget(QWidget* widget) override;
    void insertWidget(int index, QWidget* widget) override;
    void remove(int index) override;

private:
    QList<QWidget*> pages() const;

    QtMaterial::QtMaterialAdaptiveShell* shell_ = nullptr;
    int currentIndex_ = 0;
};

} // namespace QtMaterial3Designer
