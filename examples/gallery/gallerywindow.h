#pragma once

#include <QMainWindow>
#include <QPointer>
#include <QString>

class QComboBox;
class QLineEdit;
class QPlainTextEdit;
class QTabWidget;
class QTableWidget;
class QTreeWidget;
class QWidget;
struct GalleryComponentEntry;

class GalleryWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit GalleryWindow(QWidget* parent = nullptr);
    ~GalleryWindow() override;

    bool navigateToRoute(const QString& route);

private:
    void rebuildNavigation(const QString& filter = QString());
    void navigateToComponent(const GalleryComponentEntry& entry);
    QWidget* findComponentWidget(const QString& widgetType, int* tabIndex = nullptr) const;
    void updateInspector();
    void applyStatePreview(const QString& state);
    void writeEditedProperty(int row, int column);

    QTabWidget* m_tabs = nullptr;
    QLineEdit* m_search = nullptr;
    QTreeWidget* m_navigation = nullptr;
    QLineEdit* m_route = nullptr;
    QComboBox* m_state = nullptr;
    QTableWidget* m_properties = nullptr;
    QPlainTextEdit* m_cppSnippet = nullptr;
    QPlainTextEdit* m_uiSnippet = nullptr;
    QPointer<QWidget> m_target;
    QString m_currentComponentId;
};
