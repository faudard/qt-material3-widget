#include <memory>

#pragma once

#include <QModelIndex>
class QTableView;
#include <QString>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialdatacomponentspecs.h"

namespace QtMaterial {

class QtMaterialTablePrivate;

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
    void setModel(QAbstractItemModel* model) override;

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
    void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;

private:
    std::unique_ptr<QtMaterialTablePrivate> d_ptr;
    void applySpec();
    void syncAccessibility();
    QString headerText(int column) const;
    QString cellText(const QModelIndex& index) const;

};

} // namespace QtMaterial
