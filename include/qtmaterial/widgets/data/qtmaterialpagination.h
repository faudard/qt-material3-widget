#pragma once

#include <memory>

#include <QList>
#include <QWidget>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"
#include "qtmaterial/theme/qtmaterialthemecontexthost.h"

namespace QtMaterial {

class ThemeContext;
class QtMaterialPaginationPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialPagination
    : public QWidget
    , public ThemeContextHost
{
    Q_OBJECT
    Q_INTERFACES(QtMaterial::ThemeContextHost)
    Q_PROPERTY(QtMaterial::ThemeContext* themeContext READ themeContext WRITE setThemeContext NOTIFY themeContextChanged)
    Q_PROPERTY(int page READ page WRITE setPage NOTIFY pageChanged)
    Q_PROPERTY(int pageSize READ pageSize WRITE setPageSize NOTIFY pageSizeChanged)
    Q_PROPERTY(int totalCount READ totalCount WRITE setTotalCount NOTIFY totalCountChanged)

public:
    explicit QtMaterialPagination(QWidget* parent = nullptr);
    ~QtMaterialPagination() override;

    void setThemeContext(ThemeContext* context);
    ThemeContext* themeContext() const noexcept override;
    ThemeContext* effectiveThemeContext() const noexcept override;

    PaginationSpec spec() const;
    void setSpec(const PaginationSpec& spec);
    void resetSpec();
    bool hasExplicitSpec() const noexcept;

    int page() const noexcept;
    int pageSize() const noexcept;
    int totalCount() const noexcept;
    int pageCount() const noexcept;

    void setPage(int page);
    void setPageSize(int pageSize);
    void setTotalCount(int totalCount);

    QList<int> pageSizeOptions() const;
    void setPageSizeOptions(const QList<int>& options);

    QString rangeText() const;

Q_SIGNALS:
    void themeContextChanged(QtMaterial::ThemeContext* context);
    void effectiveThemeContextChanged(QtMaterial::ThemeContext* context);
    void pageChanged(int page);
    void pageSizeChanged(int pageSize);
    void totalCountChanged(int totalCount);

protected:
    void changeEvent(QEvent* event) override;

private:
    const PaginationSpec& resolvedSpec() const;
    void ensureSpecResolved() const;
    void applyResolvedSpec();
    void updateUi();

    std::unique_ptr<QtMaterialPaginationPrivate> d_ptr;
};

} // namespace QtMaterial
