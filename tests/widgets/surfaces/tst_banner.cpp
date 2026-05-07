#include <QtTest/QtTest>

#include <QSignalSpy>
#include <QToolButton>

#include "qtmaterial/widgets/surfaces/qtmaterialbanner.h"

class tst_Banner : public QObject
{
    Q_OBJECT

private slots:
    void textAndActionAccessors();
    void actionButtonsEmitSignals();
    void escapeDismissesOnlyWhenAllowed();
    void accessibilitySummaryIncludesContentActionsAndDismiss();
    void sizeHintsAreNonEmpty();
};

void tst_Banner::textAndActionAccessors()
{
    QtMaterialBanner banner;

    banner.setTitleText(QStringLiteral("Network offline"));
    banner.setBodyText(QStringLiteral("Changes will be saved when connectivity returns."));
    banner.setPrimaryActionText(QStringLiteral("Retry"));
    banner.setSecondaryActionText(QStringLiteral("Settings"));
    banner.setDismissAccessibleName(QStringLiteral("Close network banner"));

    QCOMPARE(banner.titleText(), QStringLiteral("Network offline"));
    QCOMPARE(banner.bodyText(), QStringLiteral("Changes will be saved when connectivity returns."));
    QCOMPARE(banner.primaryActionText(), QStringLiteral("Retry"));
    QCOMPARE(banner.secondaryActionText(), QStringLiteral("Settings"));
    QCOMPARE(banner.dismissAccessibleName(), QStringLiteral("Close network banner"));
}

void tst_Banner::actionButtonsEmitSignals()
{
    QtMaterialBanner banner;
    banner.setPrimaryActionText(QStringLiteral("Retry"));
    banner.setSecondaryActionText(QStringLiteral("Settings"));
    banner.resize(480, 96);
    banner.show();
    QVERIFY(QTest::qWaitForWindowExposed(&banner));

    auto* primary = banner.findChild<QToolButton*>(QStringLiteral("qtmaterial_banner_primaryActionButton"));
    auto* secondary = banner.findChild<QToolButton*>(QStringLiteral("qtmaterial_banner_secondaryActionButton"));
    QVERIFY(primary);
    QVERIFY(secondary);

    QSignalSpy primarySpy(&banner, &QtMaterialBanner::primaryActionTriggered);
    QSignalSpy secondarySpy(&banner, &QtMaterialBanner::secondaryActionTriggered);

    QTest::mouseClick(primary, Qt::LeftButton);
    QTest::mouseClick(secondary, Qt::LeftButton);

    QCOMPARE(primarySpy.count(), 1);
    QCOMPARE(secondarySpy.count(), 1);
}

void tst_Banner::escapeDismissesOnlyWhenAllowed()
{
    QtMaterialBanner banner;
    banner.setDismissible(true);
    banner.setDismissOnEscape(true);
    banner.show();
    QVERIFY(QTest::qWaitForWindowExposed(&banner));

    QSignalSpy dismissedSpy(&banner, &QtMaterialBanner::dismissed);
    QTest::keyClick(&banner, Qt::Key_Escape);
    QCOMPARE(dismissedSpy.count(), 1);
    QVERIFY(!banner.isVisible());

    banner.show();
    QVERIFY(QTest::qWaitForWindowExposed(&banner));
    banner.setDismissOnEscape(false);
    QTest::keyClick(&banner, Qt::Key_Escape);
    QCOMPARE(dismissedSpy.count(), 1);
    QVERIFY(banner.isVisible());
}

void tst_Banner::accessibilitySummaryIncludesContentActionsAndDismiss()
{
    QtMaterialBanner banner(QStringLiteral("Sync paused"), QStringLiteral("Your files are safe."));
    banner.setPrimaryActionText(QStringLiteral("Resume"));
    banner.setDismissAccessibleName(QStringLiteral("Dismiss sync banner"));

    const QString summary = banner.accessibilitySummary();
    QVERIFY(summary.contains(QStringLiteral("Sync paused")));
    QVERIFY(summary.contains(QStringLiteral("Your files are safe")));
    QVERIFY(summary.contains(QStringLiteral("Resume")));
    QVERIFY(summary.contains(QStringLiteral("Dismiss sync banner")));
    QCOMPARE(banner.accessibleName(), QStringLiteral("Sync paused"));
    QVERIFY(banner.accessibleDescription().contains(QStringLiteral("Resume")));
}

void tst_Banner::sizeHintsAreNonEmpty()
{
    QtMaterialBanner banner(QStringLiteral("Title"), QStringLiteral("Body"));
    QVERIFY(banner.sizeHint().width() >= banner.minimumSizeHint().width());
    QVERIFY(banner.sizeHint().height() >= banner.minimumSizeHint().height());
    QVERIFY(banner.minimumSizeHint().width() > 0);
    QVERIFY(banner.minimumSizeHint().height() > 0);
}

QTEST_MAIN(tst_Banner)
#include "tst_banner.moc"
