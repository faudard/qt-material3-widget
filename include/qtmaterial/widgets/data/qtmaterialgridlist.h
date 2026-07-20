#pragma once

#include <memory>

#include <QIcon>
#include <QList>
#include <QListWidget>
#include <QSize>
#include <QString>
#include <QStringList>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"
#include "qtmaterial/theme/qtmaterialthemecontexthost.h"

class QEvent;
class QKeyEvent;
class QPaintEvent;
class QResizeEvent;

namespace QtMaterial {

class Theme;
class ThemeContext;
class QtMaterialGridListPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialGridList
    : public QListWidget
    , public ThemeContextHost
{
    Q_OBJECT
    Q_INTERFACES(QtMaterial::ThemeContextHost)
    Q_PROPERTY(
        QtMaterial::ThemeContext* themeContext
        READ themeContext
        WRITE setThemeContext
        NOTIFY themeContextChanged)
    Q_PROPERTY(
        int columns
        READ columns
        WRITE setColumns
        NOTIFY columnsChanged)
    Q_PROPERTY(
        QSize cellExtent
        READ cellExtent
        WRITE setCellExtent
        NOTIFY cellExtentChanged)
    Q_PROPERTY(
        QString accessibilitySummary
        READ accessibilitySummary
        NOTIFY accessibilitySummaryChanged)

public:
    enum class SelectionMode {
        NoSelection,
        SingleSelection,
        MultiSelection,
    };
    Q_ENUM(SelectionMode)

    explicit QtMaterialGridList(
        QWidget* parent = nullptr);
    ~QtMaterialGridList() override;

    void setThemeContext(
        ThemeContext* context);
    ThemeContext* themeContext() const noexcept override;
    ThemeContext* effectiveThemeContext() const noexcept override;

    const GridListSpec& resolvedSpec() const;
    GridListSpec spec() const;
    void setSpec(const GridListSpec& spec);
    bool hasExplicitSpec() const noexcept;
    void resetSpec();

    int addGridItem(
        const QString& title,
        const QString& supportingText = QString(),
        const QIcon& icon = QIcon());
    void insertGridItem(
        int index,
        const QString& title,
        const QString& supportingText = QString(),
        const QIcon& icon = QIcon());

    QString itemTitle(int index) const;
    void setItemTitle(
        int index,
        const QString& title);

    QString itemSupportingText(int index) const;
    void setItemSupportingText(
        int index,
        const QString& supportingText);

    QIcon itemIcon(int index) const;
    void setItemIcon(
        int index,
        const QIcon& icon);

    bool isItemEnabled(int index) const;
    void setItemEnabled(
        int index,
        bool enabled);

    int currentIndex() const;
    void setCurrentIndex(int index);

    SelectionMode gridSelectionMode() const;
    void setGridSelectionMode(
        SelectionMode mode);

    QStringList selectedIndexesAsStrings() const;
    QList<int> selectedIndexesList() const;

    void setItemSelected(
        int index,
        bool selected);
    void clearGridSelection();

    int columns() const;
    void setColumns(int columns);

    QSize cellExtent() const;
    void setCellExtent(const QSize& size);
    bool hasCellExtentOverride() const noexcept;
    void resetCellExtent();

    QString itemAccessibleText(int index) const;
    QString currentItemAccessibleText() const;
    QString accessibilitySummary() const;

    QSize sizeHint() const override;

Q_SIGNALS:
    void themeContextChanged(
        QtMaterial::ThemeContext* context);
    void effectiveThemeContextChanged(
        QtMaterial::ThemeContext* context);

    void currentIndexChanged(int index);
    void gridItemClicked(int index);
    void gridItemActivated(int index);
    void gridSelectionChanged();

    void columnsChanged(int columns);
    void cellExtentChanged(const QSize& size);

    void accessibilitySummaryChanged(
        const QString& summary);

protected:
    bool event(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:

    void ensureSpecResolved() const;
    void applyResolvedSpec();
    void updateGridSize();
    void updateAllItemMetrics();
    void updateAccessibilitySummary();

    std::unique_ptr<QtMaterialGridListPrivate> d_ptr;
};

} // namespace QtMaterial
