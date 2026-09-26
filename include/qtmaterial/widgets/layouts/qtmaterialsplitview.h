#pragma once

#include <QHash>
#include <QSplitter>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialSplitView : public QSplitter
{
    Q_OBJECT

public:
    explicit QtMaterialSplitView(QWidget* parent = nullptr);
    explicit QtMaterialSplitView(Qt::Orientation orientation, QWidget* parent = nullptr);

    void setPaneCollapsible(int index, bool collapsible);
    bool paneCollapsible(int index) const;

    void setPaneCollapsed(int index, bool collapsed);
    bool paneCollapsed(int index) const;

Q_SIGNALS:
    void paneCollapsedChanged(int index, bool collapsed);

private:
    QHash<QWidget*, int> m_lastExpandedSize;
    QHash<QWidget*, bool> m_preCollapseCollapsible;
};

} // namespace QtMaterial
