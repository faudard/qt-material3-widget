#pragma once

#include <QIcon>
#include <QString>
#include <QVector>
#include <QWidget>

#include <memory>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialmenuspec.h"
#include "qtmaterial/theme/qtmaterialthemecontexthost.h"

namespace QtMaterial {
class QtMaterialThemeContextBinding;
class ThemeContext;
}

class QtMaterialMenuPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialMenu
    : public QWidget
    , public QtMaterial::ThemeContextHost
{
    Q_OBJECT
    Q_INTERFACES(QtMaterial::ThemeContextHost)

    Q_PROPERTY(
        int currentIndex
        READ currentIndex
        WRITE setCurrentIndex
        NOTIFY currentIndexChanged)
    Q_PROPERTY(
        QString accessibilitySummary
        READ accessibilitySummary
        NOTIFY accessibilitySummaryChanged)

public:
    enum class ItemRole {
        Action,
        Separator,
    };
    Q_ENUM(ItemRole)

    struct Item {
        QString text;
        QString shortcutText;
        QIcon icon;
        ItemRole role = ItemRole::Action;
        bool enabled = true;
        bool checkable = false;
        bool checked = false;
    };

    explicit QtMaterialMenu(QWidget* parent = nullptr);
    ~QtMaterialMenu() override;

    void setThemeContext(QtMaterial::ThemeContext* context);
    QtMaterial::ThemeContext* themeContext() const noexcept override;
    QtMaterial::ThemeContext*
    effectiveThemeContext() const noexcept override;

    QtMaterial::MenuSpec spec() const;
    const QtMaterial::MenuSpec& resolvedSpec() const noexcept;
    void setSpec(const QtMaterial::MenuSpec& spec);
    bool hasExplicitSpec() const noexcept;
    void resetSpec();

    int addItem(
        const QString& text,
        const QIcon& icon = QIcon());
    int addSeparator();
    void clear();

    int count() const noexcept;
    bool isEmpty() const noexcept;

    QString itemText(int index) const;
    void setItemText(int index, const QString& text);

    QString itemShortcutText(int index) const;
    void setItemShortcutText(
        int index,
        const QString& shortcutText);

    QIcon itemIcon(int index) const;
    void setItemIcon(int index, const QIcon& icon);

    bool isSeparator(int index) const;
    bool isItemEnabled(int index) const;
    void setItemEnabled(int index, bool enabled);
    bool isItemCheckable(int index) const;
    void setItemCheckable(int index, bool checkable);
    bool isItemChecked(int index) const;
    void setItemChecked(int index, bool checked);

    int currentIndex() const noexcept;
    void setCurrentIndex(int index);

    int itemAt(const QPoint& position) const;
    QRect itemRect(int index) const;
    QString itemAccessibleText(int index) const;
    QString accessibilitySummary() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

Q_SIGNALS:
    void effectiveThemeContextChanged(
        QtMaterial::ThemeContext* context);
    void activated(int index);
    void currentIndexChanged(int index);
    void accessibilitySummaryChanged(const QString& summary);
    void dismissed();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void changeEvent(QEvent* event) override;

private:
    void resolveThemeSpec();
    void applySpec();
    void ensureCurrentIndex();
    void activateIndex(int index);
    void updateAccessibility();

    std::unique_ptr<QtMaterialMenuPrivate> d_ptr;
};
