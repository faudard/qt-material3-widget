
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTest>

#include "qtmaterial/widgets/data/qtmaterialtable.h"

using QtMaterial::QtMaterialTable;

class tst_Table : public QObject
{
    Q_OBJECT

private slots:
    void constructsWithMaterialDefaults();
    void denseChangesRowHeightAndEmitsSignal();
    void summarizesModelForAccessibility();
    void reportsCurrentCellAccessibleText();
    void reportsRowAccessibleTextWithHeaders();
    void keyboardActivationEmitsRowActivated();
    void modelChangesUpdateAccessibilitySummary();
};

static QStandardItemModel* makeModel(QObject* parent = nullptr)
{
    auto* model = new QStandardItemModel(2, 3, parent);
    model->setHorizontalHeaderLabels({QStringLiteral("Name"), QStringLiteral("Role"), QStringLiteral("Status")});
    model->setData(model->index(0, 0), QStringLiteral("Ada"));
    model->setData(model->index(0, 1), QStringLiteral("Engineer"));
    model->setData(model->index(0, 2), QStringLiteral("Active"));
    model->setData(model->index(1, 0), QStringLiteral("Linus"));
    model->setData(model->index(1, 1), QStringLiteral("Maintainer"));
    model->setData(model->index(1, 2), QStringLiteral("Away"));
    return model;
}

void tst_Table::constructsWithMaterialDefaults()
{
    QtMaterialTable table;
    QCOMPARE(table.objectName(), QStringLiteral("QtMaterialTable"));
    QCOMPARE(table.selectionBehavior(), QAbstractItemView::SelectRows);
    QCOMPARE(table.selectionMode(), QAbstractItemView::SingleSelection);
    QVERIFY(table.sortingEnabled());
    QVERIFY(table.focusPolicy() & Qt::StrongFocus);
    QVERIFY(!table.dense());
    QCOMPARE(table.accessibleName(), QStringLiteral("Table"));
}

void tst_Table::denseChangesRowHeightAndEmitsSignal()
{
    QtMaterialTable table;
    const int regularHeight = table.verticalHeader()->defaultSectionSize();
    QSignalSpy spy(&table, &QtMaterialTable::denseChanged);

    table.setDense(true);

    QCOMPARE(spy.count(), 1);
    QVERIFY(table.dense());
    QVERIFY(table.verticalHeader()->defaultSectionSize() < regularHeight);

    table.setDense(true);
    QCOMPARE(spy.count(), 1);
}

void tst_Table::summarizesModelForAccessibility()
{
    QtMaterialTable table;
    auto model = std::unique_ptr<QStandardItemModel>(makeModel());
    QSignalSpy spy(&table, &QtMaterialTable::accessibilitySummaryChanged);

    table.setModel(model.get());

    QVERIFY(table.accessibilitySummary().contains(QStringLiteral("2 rows")));
    QVERIFY(table.accessibilitySummary().contains(QStringLiteral("3 columns")));
    QCOMPARE(table.accessibleDescription(), table.accessibilitySummary());
    QVERIFY(spy.count() >= 1);
}

void tst_Table::reportsCurrentCellAccessibleText()
{
    QtMaterialTable table;
    auto model = std::unique_ptr<QStandardItemModel>(makeModel());
    table.setModel(model.get());
    table.setCurrentIndex(model->index(1, 2));

    const QString text = table.currentCellAccessibleText();
    QVERIFY(text.contains(QStringLiteral("row 2")));
    QVERIFY(text.contains(QStringLiteral("column 3")));
    QVERIFY(text.contains(QStringLiteral("Status")));
    QVERIFY(text.contains(QStringLiteral("Away")));
}

void tst_Table::reportsRowAccessibleTextWithHeaders()
{
    QtMaterialTable table;
    auto model = std::unique_ptr<QStandardItemModel>(makeModel());
    table.setModel(model.get());

    const QString text = table.rowAccessibleText(0);
    QVERIFY(text.contains(QStringLiteral("Row 1")));
    QVERIFY(text.contains(QStringLiteral("Name: Ada")));
    QVERIFY(text.contains(QStringLiteral("Role: Engineer")));
    QVERIFY(text.contains(QStringLiteral("Status: Active")));
    QVERIFY(table.rowAccessibleText(-1).isEmpty());
    QVERIFY(table.rowAccessibleText(99).isEmpty());
}

void tst_Table::keyboardActivationEmitsRowActivated()
{
    QtMaterialTable table;
    auto model = std::unique_ptr<QStandardItemModel>(makeModel());
    table.setModel(model.get());
    table.setCurrentIndex(model->index(1, 0));

    QSignalSpy rowSpy(&table, &QtMaterialTable::rowActivated);
    QSignalSpy activatedSpy(&table, &QTableView::activated);

    QTest::keyClick(&table, Qt::Key_Return);

    QCOMPARE(rowSpy.count(), 1);
    QCOMPARE(rowSpy.takeFirst().at(0).toInt(), 1);
    QCOMPARE(activatedSpy.count(), 1);
    QVERIFY(table.selectionModel()->isRowSelected(1, QModelIndex()));
}

void tst_Table::modelChangesUpdateAccessibilitySummary()
{
    QtMaterialTable table;
    auto model = std::unique_ptr<QStandardItemModel>(makeModel());
    table.setModel(model.get());

    QSignalSpy spy(&table, &QtMaterialTable::accessibilitySummaryChanged);
    model->insertRow(2);

    QVERIFY(table.accessibilitySummary().contains(QStringLiteral("3 rows")));
    QVERIFY(spy.count() >= 1);
}

QTEST_MAIN(tst_Table)
#include "tst_table.moc"
