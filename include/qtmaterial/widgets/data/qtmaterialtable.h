#pragma once

#include <QTableView>

#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTable : public QTableView {
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
