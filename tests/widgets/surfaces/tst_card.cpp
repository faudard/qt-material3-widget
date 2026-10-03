#include <QtTest/QtTest>

#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"

class tst_QtMaterialCard : public QObject {
    Q_OBJECT

private slots:
    void construction();
    void titleBodySettersEmitSignals();
    void sizeHintUsesMinimums();
    void variantsRoundTrip();
    void interactiveControlsFocusPolicy();
    void keyboardActivationEmitsClicked();
    void mouseActivationEmitsClicked();
    void accessibilitySummaryTracksContent();
    void rtlAndDesktopScaleRendering_data();
    void rtlAndDesktopScaleRendering();
};

void tst_QtMaterialCard::construction()
{
    QtMaterial::QtMaterialCard card;
    QVERIFY(card.sizeHint().isValid());
    QVERIFY(card.minimumSizeHint().isValid());
    QVERIFY(card.sizeHint().width() >= card.minimumSizeHint().width());
    QVERIFY(card.sizeHint().height() >= card.minimumSizeHint().height());
}

void tst_QtMaterialCard::titleBodySettersEmitSignals()
{
    QtMaterial::QtMaterialCard card;

    QSignalSpy titleSpy(&card, &QtMaterial::QtMaterialCard::titleTextChanged);
    QSignalSpy bodySpy(&card, &QtMaterial::QtMaterialCard::bodyTextChanged);

    card.setTitleText(QStringLiteral("Card title"));
    card.setBodyText(QStringLiteral("Card body"));

    QCOMPARE(card.titleText(), QStringLiteral("Card title"));
    QCOMPARE(card.bodyText(), QStringLiteral("Card body"));
    QCOMPARE(titleSpy.count(), 1);
    QCOMPARE(bodySpy.count(), 1);

    card.setTitleText(QStringLiteral("Card title"));
    card.setBodyText(QStringLiteral("Card body"));
    QCOMPARE(titleSpy.count(), 1);
    QCOMPARE(bodySpy.count(), 1);
}

void tst_QtMaterialCard::sizeHintUsesMinimums()
{
    QtMaterial::QtMaterialCard card;
    card.setTitleText(QStringLiteral("A title"));
    card.setBodyText(QStringLiteral("A longer body text that should contribute to the preferred height."));

    const QSize size = card.sizeHint();

    QVERIFY(size.width() >= card.minimumSizeHint().width());
    QVERIFY(size.height() >= card.minimumSizeHint().height());
}

void tst_QtMaterialCard::variantsRoundTrip()
{
    QtMaterial::QtMaterialCard card;
    QSignalSpy spy(&card, &QtMaterial::QtMaterialCard::variantChanged);

    QCOMPARE(card.variant(), QtMaterial::QtMaterialCard::Variant::Elevated);

    card.setVariant(QtMaterial::QtMaterialCard::Variant::Outlined);
    QCOMPARE(card.variant(), QtMaterial::QtMaterialCard::Variant::Outlined);
    QCOMPARE(spy.count(), 1);

    card.setVariant(QtMaterial::QtMaterialCard::Variant::Outlined);
    QCOMPARE(spy.count(), 1);

    card.setVariant(QtMaterial::QtMaterialCard::Variant::Filled);
    QCOMPARE(card.variant(), QtMaterial::QtMaterialCard::Variant::Filled);
    QCOMPARE(spy.count(), 2);
}

void tst_QtMaterialCard::interactiveControlsFocusPolicy()
{
    QtMaterial::QtMaterialCard card;

    QVERIFY(!card.isInteractive());
    QCOMPARE(card.focusPolicy(), Qt::NoFocus);

    QSignalSpy spy(&card, &QtMaterial::QtMaterialCard::interactiveChanged);

    card.setInteractive(true);
    QVERIFY(card.isInteractive());
    QCOMPARE(card.focusPolicy(), Qt::StrongFocus);
    QCOMPARE(spy.count(), 1);

    card.setInteractive(false);
    QVERIFY(!card.isInteractive());
    QCOMPARE(card.focusPolicy(), Qt::NoFocus);
    QCOMPARE(spy.count(), 2);
}

void tst_QtMaterialCard::keyboardActivationEmitsClicked()
{
    QWidget window;
    QVBoxLayout layout(&window);

    QtMaterial::QtMaterialCard card;
    card.resize(240, 96);
    card.setInteractive(true);
    layout.addWidget(&card);

    window.show();
    QVERIFY(QTest::qWaitForWindowExposed(&window));

    QSignalSpy clickedSpy(&card, &QtMaterial::QtMaterialCard::clicked);
    QSignalSpy pressedSpy(&card, &QtMaterial::QtMaterialCard::pressed);
    QSignalSpy releasedSpy(&card, &QtMaterial::QtMaterialCard::released);

    window.activateWindow();
    card.setFocus(Qt::OtherFocusReason);
    QTRY_VERIFY(card.hasFocus());

    QTest::keyClick(&card, Qt::Key_Return);

    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(pressedSpy.count(), 1);
    QCOMPARE(releasedSpy.count(), 1);
    QVERIFY(!card.isPressed());
}

void tst_QtMaterialCard::mouseActivationEmitsClicked()
{
    QtMaterial::QtMaterialCard card;
    card.resize(240, 96);
    card.setInteractive(true);
    card.show();
    QVERIFY(QTest::qWaitForWindowExposed(&card));

    QSignalSpy clickedSpy(&card, &QtMaterial::QtMaterialCard::clicked);

    QTest::mouseClick(&card, Qt::LeftButton, Qt::NoModifier, card.rect().center());

    QCOMPARE(clickedSpy.count(), 1);
    QVERIFY(!card.isPressed());
}

void tst_QtMaterialCard::accessibilitySummaryTracksContent()
{
    QtMaterial::QtMaterialCard card;
    QSignalSpy spy(&card, &QtMaterial::QtMaterialCard::accessibilitySummaryChanged);

    card.setTitleText(QStringLiteral("Project status"));
    card.setBodyText(QStringLiteral("Build is green"));

    const QString summary = card.accessibilitySummary();

    QVERIFY(summary.contains(QStringLiteral("Project status")));
    QVERIFY(summary.contains(QStringLiteral("Build is green")));
    QCOMPARE(card.accessibleName(), QStringLiteral("Project status"));
    QCOMPARE(card.accessibleDescription(), summary);
    QVERIFY(spy.count() >= 2);
}


void tst_QtMaterialCard::rtlAndDesktopScaleRendering_data()
{
    QTest::addColumn<qreal>("dpr");
    QTest::newRow("100-percent") << qreal(1.00);
    QTest::newRow("125-percent") << qreal(1.25);
    QTest::newRow("150-percent") << qreal(1.50);
    QTest::newRow("175-percent") << qreal(1.75);
    QTest::newRow("200-percent") << qreal(2.00);
}

void tst_QtMaterialCard::rtlAndDesktopScaleRendering()
{
    QFETCH(qreal, dpr);
    QtMaterial::QtMaterialCard card;
    card.setTitleText(QStringLiteral("Project status"));
    card.setBodyText(QStringLiteral("Build is green"));
    card.setVariant(QtMaterial::QtMaterialCard::Variant::Outlined);
    card.setLayoutDirection(Qt::RightToLeft);
    card.resize(320, 140);
    QCOMPARE(card.layoutDirection(), Qt::RightToLeft);

    QPixmap pixmap(qMax(1, qRound(card.width() * dpr)),
                   qMax(1, qRound(card.height() * dpr)));
    pixmap.setDevicePixelRatio(dpr);
    pixmap.fill(Qt::transparent);
    card.render(&pixmap);
    QVERIFY(!pixmap.isNull());
    QCOMPARE(pixmap.devicePixelRatio(), dpr);
}

QTEST_MAIN(tst_QtMaterialCard)

#include "tst_card.moc"
