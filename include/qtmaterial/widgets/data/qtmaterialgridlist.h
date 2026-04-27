#pragma once

#include <QListView>

#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"

namespace QtMaterial {

class QtMaterialGridList : public QListView {
    Q_OBJECT

public:
    explicit QtMaterialGridList(QWidget* parent = nullptr);
    ~QtMaterialGridList() override;

    GridListSpec spec() const;
    void setSpec(const GridListSpec& spec);

    int columns() const;
    void setColumns(int columns);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    void applySpec();
    void updateGridSize();

    GridListSpec m_spec;
    int m_columns = 0;
};

} // namespace QtMaterial
