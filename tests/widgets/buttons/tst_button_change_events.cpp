#include <QtTest/QtTest>

#include <QEvent>
#include <QFont>
#include <QIcon>
#include <QPainter>
#include <QPixmap>
#include <QSignalSpy>
#include <memory>

#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

namespace {

QIcon contractIcon()
{
    QPixmap pixmap(24, 24);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawEllipse(QRectF(4, 4, 16, 16));
    painter.end();

    return QIcon(pixmap);
}

struct TextButtonCase
{
    QString name;
    std::function<std::unique_ptr<QtMaterial::QtMaterialTextButton>()> create;
};

QVector<TextButtonCase> textButtonCases()
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

class tst_ButtonChangeEvents : public QObject
{
    Q_OBJECT

private slots:
    void fontChangeUpdatesSizeHint();
    void layoutDirectionChangeKeepsSizeHintStable();
    void enabledChangeUpdatesMaterialState();
    void tooltipChangeRefreshesIconButtonAccessibility();
    void paletteChangeKeepsRenderValid();
};

void tst_ButtonChangeEvents::fontChangeUpdatesSizeHint()
{
    for (const TextButtonCase& testCase : textButtonCases()) {
        auto button = testCase.create();
        button->setText(QStringLiteral("Create project"));
        button->setIcon(contractIcon());

        const QSize before = button->sizeHint();

        QFont larger = button->font();
        larger.setPointSize(larger.pointSize() + 8);
        button->setFont(larger);

        const QSize after = button->sizeHint();
        QVERIFY2(after.width() >= before.width(), qPrintable(testCase.name));
        QVERIFY2(after.height() >= before.height(), qPrintable(testCase.name));
    }
}

void tst_ButtonChangeEvents::layoutDirectionChangeKeepsSizeHintStable()
{
    for (const TextButtonCase& testCase : textButtonCases()) {
        auto button = testCase.create();
        button->setText(QStringLiteral("Create"));
        button->setIcon(contractIcon());

        button->setLayoutDirection(Qt::LeftToRight);
        const QSize ltr = button->sizeHint();

        button->setLayoutDirection(Qt::RightToLeft);
        const QSize rtl = button->sizeHint();

        QCOMPARE(ltr, rtl);
    }
}

void tst_ButtonChangeEvents::enabledChangeUpdatesMaterialState()
{
    QtMaterial::QtMaterialFilledButton button;
    button.setText(QStringLiteral("Save"));

    QVERIFY(button.isEnabled());
    button.setEnabled(false);
    QVERIFY(!button.isEnabled());
    QVERIFY(button.materialState().contains(QStringLiteral("disabled")) || !button.materialState().contains(QStringLiteral("enabled")));

    button.setEnabled(true);
    QVERIFY(button.isEnabled());
    QVERIFY(button.materialState().contains(QStringLiteral("enabled")) || !button.materialState().contains(QStringLiteral("disabled")));
}

void tst_ButtonChangeEvents::tooltipChangeRefreshesIconButtonAccessibility()
{
    QtMaterial::QtMaterialIconButton button(contractIcon());
    QVERIFY(button.requiresAccessibleName());
    QVERIFY(!button.hasUsableAccessibleName());

    button.setToolTip(QStringLiteral("Search"));
    QVERIFY(button.hasUsableAccessibleName());
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Search"));

    button.setAccessibleName(QStringLiteral("Explicit Search"));
    QVERIFY(button.hasUsableAccessibleName());
    QCOMPARE(button.effectiveAccessibleName(), QStringLiteral("Explicit Search"));
}

void tst_ButtonChangeEvents::paletteChangeKeepsRenderValid()
{
    for (const TextButtonCase& testCase : textButtonCases()) {
        auto button = testCase.create();
        button->setText(QStringLiteral("Create"));
        button->setIcon(contractIcon());
        button->resize(button->sizeHint().expandedTo(QSize(128, 56)));
        button->show();
        QTest::qWaitForWindowExposed(button.get());

        QEvent paletteChange(QEvent::PaletteChange);
        QCoreApplication::sendEvent(button.get(), &paletteChange);

        QPixmap pixmap(button->size() * 2);
        pixmap.setDevicePixelRatio(2.0);
        pixmap.fill(Qt::transparent);
        button->render(&pixmap);

        QVERIFY2(!pixmap.isNull(), qPrintable(testCase.name));
    }
}

QTEST_MAIN(tst_ButtonChangeEvents)
#include "tst_button_change_events.moc"
