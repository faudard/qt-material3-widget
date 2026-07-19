#include <QtTest/QtTest>

#include "qtmaterial/widgets/navigation/model/qtmaterialnavigationmodel.h"

class tst_NavigationModel : public QObject {
    Q_OBJECT
private slots:
    void selectionIsExclusive()
    {
        QtMaterial::QtMaterialNavigationModel model;
        model.setItems({
            {QStringLiteral("home"), QStringLiteral("/"), QStringLiteral("Home")},
            {QStringLiteral("settings"), QStringLiteral("/settings"), QStringLiteral("Settings")}
        });

        QVERIFY(model.setSelectedId(QStringLiteral("settings")));
        QCOMPARE(model.selectedId(), QStringLiteral("settings"));
        QCOMPARE(model.selectedRoute(), QStringLiteral("/settings"));
        QCOMPARE(model.data(model.index(1, 0), QtMaterial::QtMaterialNavigationModel::SelectedRole).toBool(), true);
        QCOMPARE(model.data(model.index(0, 0), QtMaterial::QtMaterialNavigationModel::SelectedRole).toBool(), false);
    }

    void disabledItemCannotBeSelected()
    {
        QtMaterial::QtMaterialNavigationModel model;
        QtMaterial::QtMaterialNavigationItem disabled;
        disabled.id = QStringLiteral("disabled");
        disabled.route = QStringLiteral("/disabled");
        disabled.label = QStringLiteral("Disabled");
        disabled.enabled = false;
        model.setItems({disabled});
        QVERIFY(!model.setSelectedId(QStringLiteral("disabled")));
        QVERIFY(model.selectedId().isEmpty());
    }
};

QTEST_MAIN(tst_NavigationModel)
#include "tst_navigationmodel.moc"
