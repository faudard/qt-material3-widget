#pragma once

#include <QTableView>

#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"

namespace QtMaterial {

class QtMaterialTable : public QTableView {
    Q_OBJECT

public:
    explicit QtMaterialTable(QWidget* parent = nullptr);
    ~QtMaterialTable() override;

    TableSpec spec() const;
    void setSpec(const TableSpec& spec);

    bool dense() const;
    void setDense(bool dense);

protected:
    void paintEvent(QPaintEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    void applySpec();

    TableSpec m_spec;
    bool m_dense = false;
};

} // namespace QtMaterial
