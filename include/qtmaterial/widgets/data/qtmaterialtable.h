#pragma once

#include <memory>

#include <QModelIndex>
#include <QString>
#include <QTableView>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"

class QAbstractItemModel;
class QEvent;
class QFocusEvent;
class QKeyEvent;
class QPaintEvent;

namespace QtMaterial {

class Theme;
class ThemeContext;
class QtMaterialTablePrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTable
    : public QTableView
{
    Q_OBJECT
    Q_PROPERTY(
        QtMaterial::ThemeContext* themeContext
        READ themeContext
        WRITE setThemeContext
        NOTIFY themeContextChanged)
    Q_PROPERTY(
        bool dense
        READ dense
        WRITE setDense
        NOTIFY denseChanged)
    Q_PROPERTY(
        QString accessibilitySummary
        READ accessibilitySummary
        NOTIFY accessibilitySummaryChanged)

public:
    explicit QtMaterialTable(
        QWidget* parent = nullptr);
    ~QtMaterialTable() override;

    void setThemeContext(
        ThemeContext* context);
    ThemeContext* themeContext() const noexcept;
    ThemeContext* effectiveThemeContext() const noexcept;

    TableSpec spec() const;
    const TableSpec& resolvedSpec() const;

    /**
     * Installs an explicit, already resolved spec.
     *
     * Theme changes no longer replace the explicit values until resetSpec()
     * is called.
     */
    void setSpec(const TableSpec& spec);
    void resetSpec();
    bool hasExplicitSpec() const noexcept;

    bool dense() const;
    void setDense(bool dense);

    QString accessibilitySummary() const;
    QString currentCellAccessibleText() const;
    QString rowAccessibleText(int row) const;

    void setModel(
        QAbstractItemModel* model) override;

public Q_SLOTS:
    void activateCurrentRow();

Q_SIGNALS:
    void themeContextChanged(
        QtMaterial::ThemeContext* context);
    void effectiveThemeContextChanged(
        QtMaterial::ThemeContext* context);

    void denseChanged(bool dense);
    void accessibilitySummaryChanged(
        const QString& summary);
    void rowActivated(int row);

protected:
    bool event(QEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void currentChanged(
        const QModelIndex& current,
        const QModelIndex& previous) override;

private:
    bool refreshThemeContextConnection();

    void handleThemeChanged(
        const Theme& theme);
    void handleInheritedThemeContextChanged(
        ThemeContext* context);
    void handleThemeContextDestroyed(
        bool explicitContext);

    void ensureSpecResolved() const;
    void applyResolvedSpec();
    void syncAccessibility();

    std::unique_ptr<QtMaterialTablePrivate> d_ptr;
};

} // namespace QtMaterial
