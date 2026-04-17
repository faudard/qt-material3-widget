#include <QtTest/QtTest>

#include <QIcon>
#include <QSignalSpy>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"

class tst_OutlinedButton : public QObject
{
    Q_OBJECT

private slots:
    void basicConstruction();
    void sizeHintGrowsWithTextAndIcon();
    void reactsToThemeChange();
};

void tst_OutlinedButton::basicConstruction()
{
    QtMaterial::QtMaterialOutlinedButton widget;
    widget.setText(QStringLiteral("Outlined"));

    QVERIFY(widget.sizeHint().height() >= 40);
    QVERIFY(widget.minimumSizeHint().height() >= 40);
}

void tst_OutlinedButton::sizeHintGrowsWithTextAndIcon()
{
    QtMaterial::QtMaterialOutlinedButton shortButton;
    shortButton.setText(QStringLiteral("Go"));
    const int shortWidth = shortButton.sizeHint().width();

    QtMaterial::QtMaterialOutlinedButton longButton;
    longButton.setText(QStringLiteral("A much longer outlined button label"));
    const int longWidth = longButton.sizeHint().width();

    QVERIFY(longWidth > shortWidth);

    QtMaterial::QtMaterialOutlinedButton iconButton;
    iconButton.setText(QStringLiteral("Icon"));
    iconButton.setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    QVERIFY(iconButton.sizeHint().width() >= shortWidth);
}

void tst_OutlinedButton::reactsToThemeChange()
{
    QtMaterial::ThemeBuilder builder;
    QtMaterial::ThemeManager& manager = QtMaterial::ThemeManager::instance();
    QSignalSpy spy(&manager, &QtMaterial::ThemeManager::themeChanged);
    QVERIFY(spy.isValid());

    QtMaterial::QtMaterialOutlinedButton widget;
    widget.setText(QStringLiteral("Outlined"));
    const QSize before = widget.sizeHint();

    manager.setTheme(builder.buildDarkFromSeed(QColor(QStringLiteral("#00639B"))));
    const QSize after = widget.sizeHint();

    QVERIFY(spy.count() >= 1);
    QCOMPARE(after.height(), before.height());
}

QTEST_MAIN(tst_OutlinedButton)
#include "tst_outlinedbutton.moc"
