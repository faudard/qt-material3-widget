#pragma once

#include <QList>
#include <QObject>
#include <QStringList>

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

QStringList editablePropertyNames(const QWidget* widget);

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
    void addWidget(QWidget* widget) override;
    void insertWidget(int index, QWidget* widget) override;
    void remove(int index) override;

private:
    QList<QWidget*> pages() const;

    QtMaterial::QtMaterialAdaptiveShell* shell_ = nullptr;
    int currentIndex_ = 0;
};

} // namespace QtMaterial3Designer
