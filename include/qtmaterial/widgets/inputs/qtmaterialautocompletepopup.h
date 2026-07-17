#pragma once
#include <memory>

#include <QPointer>
#include <QStringList>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"

class QListView;
class QStringListModel;
class QSortFilterProxyModel;
class QModelIndex;
class QAbstractItemModel;
class QLineEdit;
class QKeyEvent;
class QPaintEvent;
class QResizeEvent;

namespace QtMaterial {

struct AutocompletePopupSpec;
class Theme;
class ThemeContext;

} // namespace QtMaterial


class QtMaterialAutocompletePopupPrivate;
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialAutocompletePopup : public QWidget {
    Q_PROPERTY(QtMaterial::ThemeContext* themeContext READ themeContext WRITE setThemeContext NOTIFY themeContextChanged)
    Q_OBJECT
public:
    explicit QtMaterialAutocompletePopup(QWidget* parent = nullptr);
    ~QtMaterialAutocompletePopup() override;

    void setThemeContext(
        QtMaterial::ThemeContext* context);
    QtMaterial::ThemeContext*
    themeContext() const noexcept;
    QtMaterial::ThemeContext*
    effectiveThemeContext() const noexcept;

    const QtMaterial::AutocompletePopupSpec&
    resolvedSpec() const;

    void setAnchorLineEdit(QLineEdit* lineEdit);
    QLineEdit* anchorLineEdit() const noexcept;

    void setModel(QAbstractItemModel* model);
    QAbstractItemModel* model() const noexcept;

    void setSuggestions(const QStringList& suggestions);
    QString currentCompletion() const;

    void setPopupVisible(bool visible);
    bool isPopupVisible() const noexcept;

    void setFilterText(const QString& text);
    QString filterText() const;

    void clearSelection();
    void selectNext();
    void selectPrevious();
    void acceptCurrent();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void themeContextChanged(
        QtMaterial::ThemeContext* context);
    void effectiveThemeContextChanged(
        QtMaterial::ThemeContext* context);
    void completionActivated(const QString& text);
    void popupVisibilityChanged(bool visible);

protected:
    bool event(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    bool refreshThemeContextConnection();
    void handleThemeChanged(
        const QtMaterial::Theme& theme);
    void handleInheritedThemeContextChanged(
        QtMaterial::ThemeContext* context);
    void handleThemeContextDestroyed(
        bool explicitContext);
    void refreshPopupVisibility();
    void setEffectivePopupVisible(bool visible);
    void ensureSpecResolved() const;
    void invalidatePopupLayout();
    void updatePopupGeometry();
    void updatePopupPalette();
    void syncToAnchorGeometry();
    void syncSelectionFromCurrentIndex();
    std::unique_ptr<QtMaterialAutocompletePopupPrivate> d_ptr;
};
