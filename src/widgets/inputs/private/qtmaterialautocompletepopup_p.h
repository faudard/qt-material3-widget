#pragma once

#include <memory>

#include <QPointer>
#include <QStringList>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/theme/qtmaterialthemecontexthost.h"

class QAbstractItemModel;
class QEvent;
class QKeyEvent;
class QLineEdit;
class QPaintEvent;
class QResizeEvent;

namespace QtMaterial {
struct AutocompletePopupSpec;
class Theme;
class ThemeContext;
} // namespace QtMaterial

class QtMaterialAutocompletePopupPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialAutocompletePopup
    : public QWidget
    , public QtMaterial::ThemeContextHost
{
    Q_OBJECT
    Q_INTERFACES(QtMaterial::ThemeContextHost)
    Q_PROPERTY(
        QtMaterial::ThemeContext* themeContext
        READ themeContext
        WRITE setThemeContext
        NOTIFY themeContextChanged)

public:
    explicit QtMaterialAutocompletePopup(QWidget* parent = nullptr);
    ~QtMaterialAutocompletePopup() override;

    void setThemeContext(QtMaterial::ThemeContext* context);
    QtMaterial::ThemeContext* themeContext() const noexcept override;
    QtMaterial::ThemeContext* effectiveThemeContext() const noexcept override;

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

Q_SIGNALS:
    void themeContextChanged(QtMaterial::ThemeContext* context);
    void effectiveThemeContextChanged(QtMaterial::ThemeContext* context);
    void completionActivated(const QString& text);
    void popupVisibilityChanged(bool visible);

protected:
    bool event(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    const QtMaterial::AutocompletePopupSpec& resolvedSpec() const;
    void handleThemeChanged(const QtMaterial::Theme& theme);
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
