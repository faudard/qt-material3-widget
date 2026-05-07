
#pragma once

#include <QModelIndex>
#include <QTableView>
#include <QString>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialTable : public QTableView
{
    Q_OBJECT
    Q_PROPERTY(bool dense READ dense WRITE setDense NOTIFY denseChanged)
    Q_PROPERTY(QString accessibilitySummary READ accessibilitySummary NOTIFY accessibilitySummaryChanged)

public:
    explicit QtMaterialTable(QWidget* parent = nullptr);
    ~QtMaterialTable() override;

    TableSpec spec() const;
    void setSpec(const TableSpec& spec);

    bool dense() const;
    void setDense(bool dense);

    QString accessibilitySummary() const;
    QString currentCellAccessibleText() const;
    QString rowAccessibleText(int row) const;

public Q_SLOTS:
    void activateCurrentRow();

Q_SIGNALS:
    void denseChanged(bool dense);
    void accessibilitySummaryChanged(const QString& summary);
    void rowActivated(int row);

protected:
    void paintEvent(QPaintEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void setModel(QAbstractItemModel* model) override;
    void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;

private:
    void applySpec();
    void syncAccessibility();
    QString headerText(int column) const;
    QString cellText(const QModelIndex& index) const;

    TableSpec m_spec;
    bool m_dense = false;
    QString m_accessibilitySummary;
};

} // namespace QtMaterial
