#include <QtTest/QtTest>

#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"

class tst_QtMaterialCardRender : public QObject
{
    Q_OBJECT

private slots:
    void render_default();
};

void tst_QtMaterialCardRender::render_default()
{
    QtMaterial::QtMaterialCard card;
    card.setTitleText(QStringLiteral("Card title"));
    card.setBodyText(QStringLiteral("Body copy for baseline."));
    card.resize(320, 160);
    card.show();
    QVERIFY(QTest::qWaitForWindowExposed(&card));
    QVERIFY(card.grab().size().isValid());
}

QTEST_MAIN(tst_QtMaterialCardRender)
#include "tst_card_render.moc"
