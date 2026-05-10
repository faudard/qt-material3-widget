#include <QtTest/QtTest>

#include <memory>

#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

namespace {

struct ButtonCase
{
    QString name;
    std::function<std::unique_ptr<QtMaterial::QtMaterialTextButton>()> create;
};

QVector<ButtonCase> textButtonCases()
{
    return {
        {QStringLiteral("text"), [] { return std::make_unique<QtMaterial::QtMaterialTextButton>(); }},
        {QStringLiteral("filled"), [] { return std::make_unique<QtMaterial::QtMaterialFilledButton>(); }},
        {QStringLiteral("filled-tonal"), [] { return std::make_unique<QtMaterial::QtMaterialFilledTonalButton>(); }},
        {QStringLiteral("outlined"), [] { return std::make_unique<QtMaterial::QtMaterialOutlinedButton>(); }},
        {QStringLiteral("elevated"), [] { return std::make_unique<QtMaterial::QtMaterialElevatedButton>(); }},
    };
}

} // namespace

class tst_ButtonTextAccessibility : public QObject
{
    Q_OBJECT

private slots:
    void visibleTextProvidesAccessibleName();
    void visibleTextChangeUpdatesAutoAccessibleName();
    void explicitAccessibleNameIsPreserved();
    void mnemonicMarkersAreRemovedFromAutoAccessibleName();
    void tooltipProvidesAutoDescription();
    void explicitAccessibleDescriptionIsPreserved();
    void extendedFabVisibleTextProvidesAccessibleName();
};

void tst_ButtonTextAccessibility::visibleTextProvidesAccessibleName()
{
    for (const ButtonCase& testCase : textButtonCases()) {
        auto button = testCase.create();
        button->setText(QStringLiteral("Save"));
        QCOMPARE(button->accessibleName(), QStringLiteral("Save"));
    }
}

void tst_ButtonTextAccessibility::visibleTextChangeUpdatesAutoAccessibleName()
{
    for (const ButtonCase& testCase : textButtonCases()) {
        auto button = testCase.create();
        button->setText(QStringLiteral("Save"));
        QCOMPARE(button->accessibleName(), QStringLiteral("Save"));

        button->setText(QStringLiteral("Send"));
        QCOMPARE(button->accessibleName(), QStringLiteral("Send"));
    }
}

void tst_ButtonTextAccessibility::explicitAccessibleNameIsPreserved()
{
    for (const ButtonCase& testCase : textButtonCases()) {
        auto button = testCase.create();
        button->setText(QStringLiteral("Save"));
        button->setAccessibleName(QStringLiteral("Commit changes"));
        button->setText(QStringLiteral("Send"));
        QCOMPARE(button->accessibleName(), QStringLiteral("Commit changes"));
    }
}

void tst_ButtonTextAccessibility::mnemonicMarkersAreRemovedFromAutoAccessibleName()
{
    for (const ButtonCase& testCase : textButtonCases()) {
        auto button = testCase.create();
        button->setText(QStringLiteral("&Save"));
        QCOMPARE(button->accessibleName(), QStringLiteral("Save"));
    }
}

void tst_ButtonTextAccessibility::tooltipProvidesAutoDescription()
{
    for (const ButtonCase& testCase : textButtonCases()) {
        auto button = testCase.create();
        button->setText(QStringLiteral("Save"));
        button->setToolTip(QStringLiteral("Save the current document"));
        QCoreApplication::sendPostedEvents(button.get(), QEvent::DeferredDelete);
        QCOMPARE(button->accessibleDescription(), QStringLiteral("Save the current document"));

        button->setToolTip(QStringLiteral("Save all changes"));
        QCOMPARE(button->accessibleDescription(), QStringLiteral("Save all changes"));
    }
}

void tst_ButtonTextAccessibility::explicitAccessibleDescriptionIsPreserved()
{
    for (const ButtonCase& testCase : textButtonCases()) {
        auto button = testCase.create();
        button->setText(QStringLiteral("Save"));
        button->setToolTip(QStringLiteral("Save the current document"));
        button->setAccessibleDescription(QStringLiteral("Explicit description"));
        button->setToolTip(QStringLiteral("Save all changes"));
        QCOMPARE(button->accessibleDescription(), QStringLiteral("Explicit description"));
    }
}

void tst_ButtonTextAccessibility::extendedFabVisibleTextProvidesAccessibleName()
{
    QtMaterial::QtMaterialExtendedFab button(QStringLiteral("Create"));
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Create"));
    QVERIFY(button.hasUsableAccessibleName());

    button.setAccessibleName(QStringLiteral("Create project"));
    button.setText(QStringLiteral("Add"));
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Create project"));
}

QTEST_MAIN(tst_ButtonTextAccessibility)
#include "tst_button_text_accessibility.moc"
