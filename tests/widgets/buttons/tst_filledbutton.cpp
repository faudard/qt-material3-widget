#include <QtTest/QtTest>

#include <QSignalSpy>
#include <QWidget>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

class tst_Filledbutton : public QObject
{
    Q_OBJECT

private slots:
    void basicConstruction();
    void sizeHintGrowsWithTextAndIcon();
    void reactsToThemeChange();
};

void tst_Filledbutton::basicConstruction()
{
    QtMaterial::QtMaterialFilledButton widget;
    widget.setText(QStringLiteral("Filled"));

    QVERIFY(widget.sizeHint().height() >= 40);
    QVERIFY(widget.minimumSizeHint().height() >= 40);
}

void tst_Filledbutton::sizeHintGrowsWithTextAndIcon()
{
    QtMaterial::QtMaterialFilledButton shortButton;
    shortButton.setText(QStringLiteral("Go"));
    const int shortWidth = shortButton.sizeHint().width();

    QtMaterial::QtMaterialFilledButton longButton;
    longButton.setText(QStringLiteral("A much longer button label"));
    const int longWidth = longButton.sizeHint().width();

    QVERIFY(longWidth > shortWidth);

    QtMaterial::QtMaterialFilledButton iconButton;
    iconButton.setText(QStringLiteral("Icon"));
    iconButton.setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    QVERIFY(iconButton.sizeHint().width() >= shortWidth);
}

void tst_Filledbutton::reactsToThemeChange()
{
    QtMaterial::ThemeBuilder builder;
    QtMaterial::ThemeManager& manager = QtMaterial::ThemeManager::instance();
    QSignalSpy spy(&manager, &QtMaterial::ThemeManager::themeChanged);
    QVERIFY(spy.isValid());

    QtMaterial::QtMaterialFilledButton widget;
    widget.setText(QStringLiteral("Filled"));
    const QSize before = widget.sizeHint();

    manager.setTheme(builder.buildDarkFromSeed(QColor(QStringLiteral("#00639B"))));
    const QSize after = widget.sizeHint();

    QVERIFY(spy.count() >= 1);
    QVERIFY(after.height() == before.height());
}

QTEST_MAIN(tst_Filledbutton)
#include "tst_filledbutton.moc"
