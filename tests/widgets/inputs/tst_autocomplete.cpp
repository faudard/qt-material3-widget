#include <QtTest/QtTest>

#include <QLineEdit>
#include <QSignalSpy>
#include <QStringListModel>

#include "qtmaterial/widgets/inputs/qtmaterialautocomplete.h"

using QtMaterial::QtMaterialAutocomplete;

class tst_Autocomplete : public QObject {
 Q_OBJECT

private slots:
 void placeholderAndTextRoundTrip();
 void suggestionsOpenPopupAndExposeCurrentCompletion();
 void escapeHidesPopupByDefault();
 void returnAcceptsCurrentCompletionByDefault();
 void canDisableCompletionOnReturn();
 void canDisableOpenOnFocus();
 void accessibilitySummaryTracksPopupState();
};

void tst_Autocomplete::placeholderAndTextRoundTrip() {
 QtMaterialAutocomplete field;
 field.setPlaceholderText(QStringLiteral("Search"));
 field.setText(QStringLiteral("Qt"));

 QCOMPARE(field.placeholderText(), QStringLiteral("Search"));
 QCOMPARE(field.text(), QStringLiteral("Qt"));
 QCOMPARE(field.lineEdit()->text(), QStringLiteral("Qt"));
 QVERIFY(field.accessibilitySummary().contains(QStringLiteral("Qt")));
}

void tst_Autocomplete::suggestionsOpenPopupAndExposeCurrentCompletion() {
 QtMaterialAutocomplete field;
 field.setSuggestions({QStringLiteral("Alpha"), QStringLiteral("Beta"), QStringLiteral("Gamma")});
 field.show();
 QVERIFY(QTest::qWaitForWindowExposed(&field));

 field.lineEdit()->setFocus();
 QTest::keyClicks(field.lineEdit(), "Al");

 QVERIFY(field.isPopupVisible());
 QCOMPARE(field.currentCompletion(), QStringLiteral("Alpha"));
 QVERIFY(field.accessibilitySummary().contains(QStringLiteral("Alpha")));
}

void tst_Autocomplete::escapeHidesPopupByDefault() {
 QtMaterialAutocomplete field;
 field.setSuggestions({QStringLiteral("Alpha"), QStringLiteral("Beta")});
 field.show();
 QVERIFY(QTest::qWaitForWindowExposed(&field));

 field.lineEdit()->setFocus();
 QTest::keyClicks(field.lineEdit(), "A");
 QVERIFY(field.isPopupVisible());

 QTest::keyClick(field.lineEdit(), Qt::Key_Escape);
 QVERIFY(!field.isPopupVisible());
}

void tst_Autocomplete::returnAcceptsCurrentCompletionByDefault() {
 QtMaterialAutocomplete field;
 field.setSuggestions({QStringLiteral("Alpha"), QStringLiteral("Beta")});
 field.show();
 QVERIFY(QTest::qWaitForWindowExposed(&field));

 QSignalSpy activated(&field, &QtMaterialAutocomplete::completionActivated);
 field.lineEdit()->setFocus();
 QTest::keyClicks(field.lineEdit(), "A");
 QVERIFY(field.isPopupVisible());

 QTest::keyClick(field.lineEdit(), Qt::Key_Return);
 QCOMPARE(field.text(), QStringLiteral("Alpha"));
 QCOMPARE(activated.count(), 1);
}

void tst_Autocomplete::canDisableCompletionOnReturn() {
 QtMaterialAutocomplete field;
 field.setCompletesOnReturn(false);
 field.setSuggestions({QStringLiteral("Alpha"), QStringLiteral("Beta")});
 field.show();
 QVERIFY(QTest::qWaitForWindowExposed(&field));

 QSignalSpy activated(&field, &QtMaterialAutocomplete::completionActivated);
 field.lineEdit()->setFocus();
 QTest::keyClicks(field.lineEdit(), "A");
 QVERIFY(field.isPopupVisible());

 QTest::keyClick(field.lineEdit(), Qt::Key_Return);
 QCOMPARE(field.text(), QStringLiteral("A"));
 QCOMPARE(activated.count(), 0);
}

void tst_Autocomplete::canDisableOpenOnFocus() {
 QtMaterialAutocomplete field;
 field.setOpensOnFocus(false);
 field.setSuggestions({QStringLiteral("Alpha"), QStringLiteral("Beta")});
 field.show();
 QVERIFY(QTest::qWaitForWindowExposed(&field));

 field.lineEdit()->setFocus();
 QTest::keyClicks(field.lineEdit(), "A");
 QVERIFY(!field.isPopupVisible());
}

void tst_Autocomplete::accessibilitySummaryTracksPopupState() {
 QtMaterialAutocomplete field;
 field.setAccessibleName(QStringLiteral("Project search"));
 field.setSuggestions({QStringLiteral("Apollo")});

 QSignalSpy summaryChanged(&field, &QtMaterialAutocomplete::accessibilitySummaryChanged);
 field.setText(QStringLiteral("Ap"));

 QVERIFY(field.accessibilitySummary().contains(QStringLiteral("Project search")));
 QVERIFY(field.accessibilitySummary().contains(QStringLiteral("Ap")));
 QVERIFY(summaryChanged.count() >= 1);
}

QTEST_MAIN(tst_Autocomplete)
#include "tst_autocomplete.moc"