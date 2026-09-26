#pragma once

#include <memory>

#include <QSplitter>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QtMaterialSplitViewPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSplitView : public QSplitter
{
    Q_OBJECT

public:
    explicit QtMaterialSplitView(QWidget* parent = nullptr);
    explicit QtMaterialSplitView(Qt::Orientation orientation, QWidget* parent = nullptr);
    ~QtMaterialSplitView() override;

    void setPaneCollapsible(int index, bool collapsible);
    bool paneCollapsible(int index) const;

    void setPaneCollapsed(int index, bool collapsed);
    bool paneCollapsed(int index) const;

Q_SIGNALS:
    void paneCollapsedChanged(int index, bool collapsed);

private:
    std::unique_ptr<QtMaterialSplitViewPrivate> d_ptr;
};

} // namespace QtMaterial
