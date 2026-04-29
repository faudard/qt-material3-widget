#include <QtTest/QtTest>

#include "qtmaterial/core/qtmaterialpaletteadapter.h"
#include "qtmaterial/theme/qtmaterialtheme.h"

class tst_PaletteAdapter : public QObject {
    Q_OBJECT
private slots:
    void keepsValidFallbackForEmptyTheme()
    {
        QtMaterial::Theme theme;
        const QPalette base;
        const QPalette palette = QtMaterial::QtMaterialPaletteAdapter::toPalette(theme, base);
        QVERIFY(palette.color(QPalette::Window).isValid());
        QVERIFY(palette.color(QPalette::WindowText).isValid());
    }
};

QTEST_MAIN(tst_PaletteAdapter)
#include "tst_paletteadapter.moc"
