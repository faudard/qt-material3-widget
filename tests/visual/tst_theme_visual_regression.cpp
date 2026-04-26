#include <QtTest/QtTest>

#include <memory>

#include <QApplication>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QPalette>
#include <QSlider>
#include <QWidget>

#include "qtmaterial/theme/qtmaterialthemebuilder.h"
#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"
#include "qtmaterial/widgets/selection/qtmaterialswitch.h"
#include "qtmaterial/widgets/surfaces/qtmaterialcard.h"

#include "qtmaterialvisualtesthelpers.h"

using namespace QtMaterial;

namespace {

Theme makeTheme(ThemeMode mode, ContrastMode contrast, const QColor& seed = QColor(QStringLiteral("#6750A4")))
{
    ThemeOptions options;
    options.sourceColor = seed;
    options.mode = mode;
    options.contrast = contrast;
    options.expressive = false;
    options.useMaterialColorUtilities = false;
    return ThemeBuilder().build(options);
}

QWidget* buildComponentGrid(const Theme& theme)
{
    ThemeManager::instance().setTheme(theme);

    auto* root = new QWidget;
    root->setObjectName(QStringLiteral("visualComponentGrid"));
    root->setAutoFillBackground(true);

    QPalette palette = root->palette();
    palette.setColor(QPalette::Window, theme.colorScheme().color(ColorRole::Surface));
    palette.setColor(QPalette::WindowText, theme.colorScheme().color(ColorRole::OnSurface));
    root->setPalette(palette);

    auto* layout = new QGridLayout(root);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setHorizontalSpacing(16);
    layout->setVerticalSpacing(16);

    auto* title = new QLabel(QStringLiteral("Qt Material 3 component grid"), root);
    QFont titleFont = title->font();
    titleFont.setPixelSize(22);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAccessibleName(QStringLiteral("Component grid title"));
    layout->addWidget(title, 0, 0, 1, 4);

    auto* filled = new QtMaterialFilledButton(root);
    filled->setText(QStringLiteral("Filled"));
    filled->setAccessibleName(QStringLiteral("Filled button visual sample"));
    layout->addWidget(filled, 1, 0);

    auto* outlined = new QtMaterialOutlinedButton(root);
    outlined->setText(QStringLiteral("Outlined"));
    outlined->setAccessibleName(QStringLiteral("Outlined button visual sample"));
    layout->addWidget(outlined, 1, 1);

    auto* elevated = new QtMaterialElevatedButton(root);
    elevated->setText(QStringLiteral("Elevated"));
    elevated->setAccessibleName(QStringLiteral("Elevated button visual sample"));
    layout->addWidget(elevated, 1, 2);

    auto* text = new QtMaterialTextButton(root);
    text->setText(QStringLiteral("Text"));
    text->setAccessibleName(QStringLiteral("Text button visual sample"));
    layout->addWidget(text, 1, 3);

    auto* checkbox = new QtMaterialCheckbox(root);
    checkbox->setText(QStringLiteral("Checkbox"));
    checkbox->setChecked(true);
    checkbox->setAccessibleName(QStringLiteral("Checkbox visual sample"));
    layout->addWidget(checkbox, 2, 0);

    auto* radio = new QtMaterialRadioButton(root);
    radio->setText(QStringLiteral("Radio"));
    radio->setChecked(true);
    radio->setAccessibleName(QStringLiteral("Radio visual sample"));
    layout->addWidget(radio, 2, 1);

    auto* sw = new QtMaterialSwitch(QStringLiteral("Switch"), root);
    sw->setChecked(true);
    sw->setAccessibleName(QStringLiteral("Switch visual sample"));
    layout->addWidget(sw, 2, 2);

    auto* slider = new QSlider(Qt::Horizontal, root);
    slider->setRange(0, 100);
    slider->setValue(64);
    slider->setAccessibleName(QStringLiteral("Slider visual sample"));
    layout->addWidget(slider, 2, 3);

    auto* field = new QtMaterialOutlinedTextField(root);
    field->setText(QStringLiteral("Outlined text field"));
    field->setAccessibleName(QStringLiteral("Text field visual sample"));
    layout->addWidget(field, 3, 0, 1, 2);

    auto* combo = new QComboBox(root);
    combo->addItems({QStringLiteral("Default density"), QStringLiteral("Compact"), QStringLiteral("Comfortable")});
    combo->setAccessibleName(QStringLiteral("Combo visual sample"));
    layout->addWidget(combo, 3, 2, 1, 2);

    auto* card = new QtMaterialCard(root);
    card->setTitleText(QStringLiteral("Card surface"));
    card->setBodyText(QStringLiteral("Surface, shape, elevation, text, and outline are captured together."));
    card->setAccessibleName(QStringLiteral("Card visual sample"));
    layout->addWidget(card, 4, 0, 1, 4);

    return root;
}

} // namespace

class tst_ThemeVisualRegression : public QObject {
    Q_OBJECT

private slots:
    void tokenBoardGoldens_data();
    void tokenBoardGoldens();
    void componentGridSmoke_data();
    void componentGridSmoke();
    void componentGridStrictGoldens_data();
    void componentGridStrictGoldens();
};

void tst_ThemeVisualRegression::tokenBoardGoldens_data()
{
    QTest::addColumn<QString>("caseName");
    QTest::addColumn<ThemeMode>("mode");
    QTest::addColumn<ContrastMode>("contrast");
    QTest::addColumn<QColor>("seed");

    QTest::newRow("seed_6750A4_light_standard") << QStringLiteral("token_seed_6750A4_light_standard") << ThemeMode::Light << ContrastMode::Standard << QColor(QStringLiteral("#6750A4"));
    QTest::newRow("seed_6750A4_dark_standard") << QStringLiteral("token_seed_6750A4_dark_standard") << ThemeMode::Dark << ContrastMode::Standard << QColor(QStringLiteral("#6750A4"));
    QTest::newRow("seed_00639B_light_high") << QStringLiteral("token_seed_00639B_light_high") << ThemeMode::Light << ContrastMode::High << QColor(QStringLiteral("#00639B"));
    QTest::newRow("seed_FF0000_dark_high") << QStringLiteral("token_seed_FF0000_dark_high") << ThemeMode::Dark << ContrastMode::High << QColor(QStringLiteral("#FF0000"));
}

void tst_ThemeVisualRegression::tokenBoardGoldens()
{
    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);
    QFETCH(QColor, seed);

    const Theme theme = makeTheme(mode, contrast, seed);
    const QImage image = QtMaterialVisualTest::renderTokenBoard(theme);
    QCOMPARE(image.size(), QSize(720, 420));
    QtMaterialVisualTest::verifyOrUpdateGolden(caseName, image);
}

void tst_ThemeVisualRegression::componentGridSmoke_data()
{
    QTest::addColumn<QString>("caseName");
    QTest::addColumn<ThemeMode>("mode");
    QTest::addColumn<ContrastMode>("contrast");

    QTest::newRow("light_standard") << QStringLiteral("component_grid_light_standard") << ThemeMode::Light << ContrastMode::Standard;
    QTest::newRow("dark_standard") << QStringLiteral("component_grid_dark_standard") << ThemeMode::Dark << ContrastMode::Standard;
    QTest::newRow("light_high") << QStringLiteral("component_grid_light_high") << ThemeMode::Light << ContrastMode::High;
}

void tst_ThemeVisualRegression::componentGridSmoke()
{
    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    Theme theme = makeTheme(mode, contrast);
    std::unique_ptr<QWidget> grid(buildComponentGrid(theme));
    const QImage image = QtMaterialVisualTest::renderWidget(grid.get());
    QVERIFY(!image.isNull());
    QVERIFY(image.width() >= 360);
    QVERIFY(image.height() >= 220);

    const QString artifactPath = QDir(QtMaterialVisualTest::artifactsDir()).absoluteFilePath(caseName + QStringLiteral(".actual.png"));
    QString error;
    QVERIFY2(QtMaterialVisualTest::savePng(artifactPath, image, &error), qPrintable(error));
    QtMaterialVisualTest::writeManifestEntry(caseName, image, QStringLiteral("widget-smoke"));
}

void tst_ThemeVisualRegression::componentGridStrictGoldens_data()
{
    componentGridSmoke_data();
}

void tst_ThemeVisualRegression::componentGridStrictGoldens()
{
    if (!QtMaterialVisualTest::strictGoldens() && !QtMaterialVisualTest::updateGoldens()) {
        QSKIP("Component-widget pixel goldens are opt-in. Set QTMATERIAL3_VISUAL_STRICT=1 to compare or QTMATERIAL3_UPDATE_VISUAL_GOLDENS=1 to create.");
    }

    QFETCH(QString, caseName);
    QFETCH(ThemeMode, mode);
    QFETCH(ContrastMode, contrast);

    Theme theme = makeTheme(mode, contrast);
    std::unique_ptr<QWidget> grid(buildComponentGrid(theme));
    const QImage image = QtMaterialVisualTest::renderWidget(grid.get());
    QtMaterialVisualTest::verifyOrUpdateGolden(caseName, image);
}

QTEST_MAIN(tst_ThemeVisualRegression)
#include "tst_theme_visual_regression.moc"
