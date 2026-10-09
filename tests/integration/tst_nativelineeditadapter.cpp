#include <QtTest/QtTest>

#include <QCompleter>
#include <QIntValidator>
#include <QLineEdit>
#include <QSignalSpy>
#include <QStringListModel>
#include <QStyle>
#include <QStyleOptionFrame>
#include <QVBoxLayout>
#include <QWidget>

#include "qtmaterial/widgets/native/qtmateriallineeditadapter.h"

using namespace QtMaterial;

class tst_NativeLineEditAdapter : public QObject
{
    Q_OBJECT

private slots:
    void preservesNativeContracts();
    void variantsAndDensityAreLive();
    void contentGeometryUsesMaterialInsets();
    void adaptsTreeAndHonorsOptOut();
    void restoresStyleAndPalette();
    void restoresExplicitPaletteAndFont();
};

void tst_NativeLineEditAdapter::preservesNativeContracts()
{
    QLineEdit edit;
    QIntValidator validator(0, 999, &edit);
    QStringListModel completionModel(
        {QStringLiteral("100"), QStringLiteral("200")},
        &edit);
    QCompleter completer(&completionModel, &edit);

    edit.setValidator(&validator);
    edit.setCompleter(&completer);
    edit.setInputMask(QStringLiteral("000;_"));
    edit.setEchoMode(QLineEdit::Password);
    edit.setClearButtonEnabled(true);
    edit.setTextMargins(3, 4, 5, 6);

    const QMargins margins = edit.textMargins();

    QSignalSpy changed(
        &edit,
        &QLineEdit::textChanged);

    QtMaterialLineEditAdapter::apply(
        &edit,
        QtMaterialLineEditAdapter::Variant::Outlined,
        Density::Default);

    QVERIFY(
        QtMaterialLineEditAdapter::isApplied(&edit));
    QCOMPARE(edit.validator(), &validator);
    QCOMPARE(edit.completer(), &completer);
    QCOMPARE(edit.inputMask(), QStringLiteral("000;_"));
    QCOMPARE(edit.echoMode(), QLineEdit::Password);
    QVERIFY(edit.isClearButtonEnabled());
    QCOMPARE(edit.textMargins(), margins);

    edit.setText(QStringLiteral("123"));
    QCOMPARE(edit.text(), QStringLiteral("123"));
    QVERIFY(changed.count() >= 1);
}

void tst_NativeLineEditAdapter::variantsAndDensityAreLive()
{
    QLineEdit edit;
    QtMaterialLineEditAdapter::apply(&edit);

    QCOMPARE(
        int(QtMaterialLineEditAdapter::variant(&edit)),
        int(QtMaterialLineEditAdapter::Variant::Outlined));

    QtMaterialLineEditAdapter::setVariant(
        &edit,
        QtMaterialLineEditAdapter::Variant::Filled);
    QCOMPARE(
        int(QtMaterialLineEditAdapter::variant(&edit)),
        int(QtMaterialLineEditAdapter::Variant::Filled));

    edit.setProperty(
        QtMaterialLineEditAdapter::densityPropertyName(),
        QStringLiteral("compact"));
    QCOMPARE(
        int(QtMaterialLineEditAdapter::density(&edit)),
        int(Density::Compact));

    QtMaterialLineEditAdapter::setDensity(
        &edit,
        Density::Comfortable);
    QCOMPARE(
        int(QtMaterialLineEditAdapter::density(&edit)),
        int(Density::Comfortable));
}

void tst_NativeLineEditAdapter::
    contentGeometryUsesMaterialInsets()
{
    QLineEdit edit;
    edit.resize(260, 56);
    QtMaterialLineEditAdapter::apply(
        &edit,
        QtMaterialLineEditAdapter::Variant::Outlined,
        Density::Default);

    QStyleOptionFrame option;
    option.initFrom(&edit);
    option.rect = edit.rect();

    const QRect contents =
        edit.style()->subElementRect(
            QStyle::SE_LineEditContents,
            &option,
            &edit);

    QVERIFY(contents.left() > edit.rect().left());
    QVERIFY(contents.right() < edit.rect().right());
    QVERIFY(contents.height() <= edit.rect().height());
    QVERIFY(edit.sizeHint().height() >= 56);
}

void tst_NativeLineEditAdapter::
    adaptsTreeAndHonorsOptOut()
{
    QWidget root;
    auto* layout = new QVBoxLayout(&root);
    auto* first = new QLineEdit(&root);
    auto* second = new QLineEdit(&root);
    layout->addWidget(first);
    layout->addWidget(second);

    QtMaterialLineEditAdapter::setOptOut(
        second,
        true);

    QCOMPARE(
        QtMaterialLineEditAdapter::applyToDescendants(
            &root,
            QtMaterialLineEditAdapter::Variant::Filled,
            Density::Compact),
        1);

    QVERIFY(
        QtMaterialLineEditAdapter::isApplied(first));
    QVERIFY(
        !QtMaterialLineEditAdapter::isApplied(second));
    QCOMPARE(
        int(QtMaterialLineEditAdapter::variant(first)),
        int(QtMaterialLineEditAdapter::Variant::Filled));
}

void tst_NativeLineEditAdapter::
    restoresStyleAndPalette()
{
    QLineEdit edit;
    QStyle* originalStyle = edit.style();
    const QPalette originalPalette = edit.palette();
    const QFont originalFont = edit.font();
    const bool hadExplicitPalette =
        edit.testAttribute(Qt::WA_SetPalette);
    const bool hadExplicitFont =
        edit.testAttribute(Qt::WA_SetFont);

    QtMaterialLineEditAdapter::apply(
        &edit,
        QtMaterialLineEditAdapter::Variant::Filled);

    QVERIFY(edit.style() != originalStyle);

    QtMaterialLineEditAdapter::remove(&edit);

    QVERIFY(
        !QtMaterialLineEditAdapter::isApplied(&edit));
    QCOMPARE(edit.style(), originalStyle);
    QCOMPARE(
        edit.testAttribute(Qt::WA_SetPalette),
        hadExplicitPalette);
    QCOMPARE(
        edit.testAttribute(Qt::WA_SetFont),
        hadExplicitFont);

    // Inherited palette/font values may be recomputed by the platform style
    // during repolish (notably Cocoa). Exact value equality is only a valid
    // round-trip contract when the widget owned explicit presentation state.
    if (hadExplicitPalette) {
        QCOMPARE(edit.palette(), originalPalette);
    }
    if (hadExplicitFont) {
        QCOMPARE(edit.font(), originalFont);
    }
}

void tst_NativeLineEditAdapter::
    restoresExplicitPaletteAndFont()
{
    QLineEdit edit;
    QStyle* originalStyle = edit.style();

    QPalette explicitPalette = edit.palette();
    explicitPalette.setColor(
        QPalette::Base,
        QColor(QStringLiteral("#123456")));
    explicitPalette.setColor(
        QPalette::Text,
        QColor(QStringLiteral("#fedcba")));
    edit.setPalette(explicitPalette);

    QFont explicitFont = edit.font();
    explicitFont.setItalic(true);
    explicitFont.setPointSize(
        qMax(8, explicitFont.pointSize() + 2));
    edit.setFont(explicitFont);

    QVERIFY(edit.testAttribute(Qt::WA_SetPalette));
    QVERIFY(edit.testAttribute(Qt::WA_SetFont));

    QtMaterialLineEditAdapter::apply(
        &edit,
        QtMaterialLineEditAdapter::Variant::Outlined,
        Density::Compact);

    QtMaterialLineEditAdapter::remove(&edit);

    QCOMPARE(edit.style(), originalStyle);
    QVERIFY(edit.testAttribute(Qt::WA_SetPalette));
    QVERIFY(edit.testAttribute(Qt::WA_SetFont));
    QCOMPARE(edit.palette(), explicitPalette);
    QCOMPARE(edit.font(), explicitFont);
}

QTEST_MAIN(tst_NativeLineEditAdapter)
#include "tst_nativelineeditadapter.moc"
