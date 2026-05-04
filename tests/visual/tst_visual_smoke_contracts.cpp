#include <QtTest/QtTest>

#include <QApplication>
#include <QBoxLayout>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QScopedPointer>
#include <QSize>
#include <QWidget>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"
#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"
#include "qtmaterial/widgets/progress/qtmaterialcircularprogressindicator.h"
#include "qtmaterial/widgets/progress/qtmateriallinearprogressindicator.h"
#include "qtmaterial/widgets/selection/qtmaterialcheckbox.h"
#include "qtmaterial/widgets/selection/qtmaterialradiobutton.h"
#include "qtmaterial/widgets/selection/qtmaterialswitch.h"

namespace {

QSize physicalSizeFor(const QSize& logicalSize, qreal dpr)
{
    return QSize(qMax(1, qRound(logicalSize.width() * dpr)),
                 qMax(1, qRound(logicalSize.height() * dpr)));
}

bool hasPaintedPixels(const QImage& image)
{
    for (int y = 0; y < image.height(); ++y) {
        const QRgb* row = reinterpret_cast<const QRgb*>(image.constScanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            if (qAlpha(row[x]) != 0) {
                return true;
            }
        }
    }
    return false;
}

QImage renderWidget(QWidget& widget, qreal dpr = 1.0)
{
    widget.ensurePolished();

    QSize logicalSize = widget.size();
    if (logicalSize.isEmpty()) {
        logicalSize = widget.sizeHint();
    }
    if (logicalSize.isEmpty()) {
        logicalSize = QSize(320, 160);
    }

    logicalSize = logicalSize.expandedTo(QSize(160, 80));
    widget.resize(logicalSize);
    if (widget.layout()) {
        widget.layout()->activate();
    }

    QImage image(physicalSizeFor(logicalSize, dpr), QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(dpr);
    image.fill(Qt::transparent);

    QPainter painter(&image);
    widget.render(&painter);
    painter.end();

    return image;
}

void verifyRenderable(QWidget& widget, const char* context, qreal dpr = 1.0)
{
    const QImage image = renderWidget(widget, dpr);

    QVERIFY2(!image.isNull(), context);
    QVERIFY2(image.width() > 0, context);
    QVERIFY2(image.height() > 0, context);
    QVERIFY2(hasPaintedPixels(image), context);
}

QWidget* buildCoreControlsPage(Qt::LayoutDirection direction)
{
    auto* page = new QWidget;
    page->setLayoutDirection(direction);

    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    auto* title = new QLabel(QStringLiteral("Visual smoke: core controls"), page);
    layout->addWidget(title);

    auto* filled = new QtMaterial::QtMaterialFilledButton(page);
    filled->setText(QStringLiteral("Filled action"));
    filled->setProperty("materialTestId", QStringLiteral("visual-filled-button"));
    layout->addWidget(filled);

    auto* outlined = new QtMaterial::QtMaterialOutlinedButton(page);
    outlined->setText(QStringLiteral("Outlined action"));
    layout->addWidget(outlined);

    auto* text = new QtMaterial::QtMaterialTextButton(page);
    text->setText(QStringLiteral("Text action"));
    layout->addWidget(text);

    auto* field = new QtMaterial::QtMaterialOutlinedTextField(page);
    field->setLabelText(QStringLiteral("Name"));
    field->setPlaceholderText(QStringLiteral("Ada Lovelace"));
    field->setSupportingText(QStringLiteral("Required for the release smoke test"));
    field->setText(QStringLiteral("Ada Lovelace"));
    layout->addWidget(field);

    auto* checkbox = new QtMaterial::QtMaterialCheckbox(page);
    checkbox->setText(QStringLiteral("Checkbox"));
    checkbox->setChecked(true);
    layout->addWidget(checkbox);

    auto* radio = new QtMaterial::QtMaterialRadioButton(page);
    radio->setText(QStringLiteral("Radio option"));
    radio->setChecked(true);
    layout->addWidget(radio);

    auto* sw = new QtMaterial::QtMaterialSwitch(page);
    sw->setText(QStringLiteral("Switch"));
    sw->setChecked(true);
    layout->addWidget(sw);

    auto* linear = new QtMaterial::QtMaterialLinearProgressIndicator(page);
    linear->setMode(QtMaterial::QtMaterialLinearProgressIndicator::Mode::Determinate);
    linear->setValue(0.5);
    linear->setStatusText(QStringLiteral("Half complete"));
    layout->addWidget(linear);

    auto* circular = new QtMaterial::QtMaterialCircularProgressIndicator(page);
    circular->setIndeterminate(true);
    circular->setBusy(true);
    layout->addWidget(circular);

    return page;
}

QWidget* buildNavigationPage(Qt::LayoutDirection direction)
{
    auto* page = new QWidget;
    page->setLayoutDirection(direction);

    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);

    auto* tabs = new QtMaterial::QtMaterialTabs(page);
    tabs->addTab(new QLabel(QStringLiteral("Home content"), tabs), QStringLiteral("Home"));
    tabs->addTab(new QLabel(QStringLiteral("Settings content"), tabs), QStringLiteral("Settings"));
    tabs->addTab(new QLabel(QStringLiteral("About content"), tabs), QStringLiteral("About"));
    tabs->setCurrentIndex(1);

    layout->addWidget(tabs);
    return page;
}

} // namespace

class tst_VisualSmokeContracts : public QObject
{
    Q_OBJECT

private slots:
    void coreControlsRenderLtr();
    void coreControlsRenderRtl();
    void navigationRendersLtr();
    void navigationRendersRtl();
    void coreControlsRenderHighDpi();
};

void tst_VisualSmokeContracts::coreControlsRenderLtr()
{
    QScopedPointer<QWidget> page(buildCoreControlsPage(Qt::LeftToRight));
    verifyRenderable(*page, "core controls LTR should render");
}

void tst_VisualSmokeContracts::coreControlsRenderRtl()
{
    QScopedPointer<QWidget> page(buildCoreControlsPage(Qt::RightToLeft));
    verifyRenderable(*page, "core controls RTL should render");
}

void tst_VisualSmokeContracts::navigationRendersLtr()
{
    QScopedPointer<QWidget> page(buildNavigationPage(Qt::LeftToRight));
    verifyRenderable(*page, "navigation LTR should render");
}

void tst_VisualSmokeContracts::navigationRendersRtl()
{
    QScopedPointer<QWidget> page(buildNavigationPage(Qt::RightToLeft));
    verifyRenderable(*page, "navigation RTL should render");
}

void tst_VisualSmokeContracts::coreControlsRenderHighDpi()
{
    QScopedPointer<QWidget> page(buildCoreControlsPage(Qt::LeftToRight));
    verifyRenderable(*page, "core controls high-DPI should render", 2.0);
}

QTEST_MAIN(tst_VisualSmokeContracts)
#include "tst_visual_smoke_contracts.moc"
