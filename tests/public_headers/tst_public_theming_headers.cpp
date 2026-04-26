#include <QtTest/QtTest>

#if __has_include(<qtmaterial/qtmaterialglobal.h>)
#  include <qtmaterial/qtmaterialglobal.h>
#endif

#if __has_include(<qtmaterial/theme/qtmaterialcolortoken.h>)
#  include <qtmaterial/theme/qtmaterialcolortoken.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialcolorscheme.h>)
#  include <qtmaterial/theme/qtmaterialcolorscheme.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialthemeoptions.h>)
#  include <qtmaterial/theme/qtmaterialthemeoptions.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialtheme.h>)
#  include <qtmaterial/theme/qtmaterialtheme.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialthemebuilder.h>)
#  include <qtmaterial/theme/qtmaterialthemebuilder.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialthemeserializer.h>)
#  include <qtmaterial/theme/qtmaterialthemeserializer.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialthememanager.h>)
#  include <qtmaterial/theme/qtmaterialthememanager.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialthemeobserver.h>)
#  include <qtmaterial/theme/qtmaterialthemeobserver.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialtypographyscale.h>)
#  include <qtmaterial/theme/qtmaterialtypographyscale.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialshapescale.h>)
#  include <qtmaterial/theme/qtmaterialshapescale.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialelevationscale.h>)
#  include <qtmaterial/theme/qtmaterialelevationscale.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialmotiontokens.h>)
#  include <qtmaterial/theme/qtmaterialmotiontokens.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialstatelayer.h>)
#  include <qtmaterial/theme/qtmaterialstatelayer.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialdensitytokens.h>)
#  include <qtmaterial/theme/qtmaterialdensitytokens.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialiconsizetokens.h>)
#  include <qtmaterial/theme/qtmaterialiconsizetokens.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialcomponentoverrides.h>)
#  include <qtmaterial/theme/qtmaterialcomponentoverrides.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialsystemtheme.h>)
#  include <qtmaterial/theme/qtmaterialsystemtheme.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialaccessibilitytokens.h>)
#  include <qtmaterial/theme/qtmaterialaccessibilitytokens.h>
#endif
#if __has_include(<qtmaterial/theme/qtmaterialxmlthemeadapter.h>)
#  include <qtmaterial/theme/qtmaterialxmlthemeadapter.h>
#endif

class PublicThemingHeadersCompileTest : public QObject {
    Q_OBJECT

private slots:
    void publicHeadersAreSelfContained()
    {
        QVERIFY2(true, "Public theming headers compiled as standalone includes.");
    }
};

QTEST_MAIN(PublicThemingHeadersCompileTest)
#include "tst_public_theming_headers.moc"
