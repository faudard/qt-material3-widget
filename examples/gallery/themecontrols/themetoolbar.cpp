#include "themetoolbar.h"

#include "qtmaterial/theme/qtmaterialthememanager.h"

#include <QApplication>
#include <QColorDialog>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMetaEnum>
#include <QMetaProperty>
#include <QPushButton>
#include <QWidget>

namespace {

void applyDensityKey(const QByteArray& requested)
{
    const auto widgets = QApplication::allWidgets();
    for (QWidget* widget : widgets) {
        const QMetaObject* meta = widget->metaObject();
        const int propertyIndex = meta->indexOfProperty("density");
        if (propertyIndex < 0) continue;

        const QMetaProperty property = meta->property(propertyIndex);
        if (!property.isWritable() || !property.isEnumType()) continue;

        const QMetaEnum values = property.enumerator();
        bool ok = false;
        int value = values.keyToValue(requested.constData(), &ok);
        if (!ok && requested == "Default") value = values.keyToValue("Regular", &ok);
        if (!ok && requested == "Compact") value = values.keyToValue("Dense", &ok);
        if (!ok) continue;

        property.write(widget, value);
        widget->updateGeometry();
        widget->update();
    }
    qApp->setProperty("qtmaterial3GalleryDensity", QString::fromLatin1(requested));
}

QComboBox* labeledCombo(
    const QString& label,
    const QStringList& values,
    QHBoxLayout* layout,
    QWidget* parent)
{
    auto* text = new QLabel(label, parent);
    auto* combo = new QComboBox(parent);
    combo->addItems(values);
    layout->addWidget(text);
    layout->addWidget(combo);
    return combo;
}

} // namespace

ThemeToolbar::ThemeToolbar(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto* mode = labeledCombo(
        QStringLiteral("Theme"),
        {QStringLiteral("Light"), QStringLiteral("Dark")},
        layout,
        this);
    auto* contrast = labeledCombo(
        QStringLiteral("Contrast"),
        {QStringLiteral("Standard"), QStringLiteral("Medium"), QStringLiteral("High")},
        layout,
        this);
    auto* variant = labeledCombo(
        QStringLiteral("Style"),
        {QStringLiteral("Standard"), QStringLiteral("Expressive")},
        layout,
        this);
    auto* direction = labeledCombo(
        QStringLiteral("Direction"),
        {QStringLiteral("LTR"), QStringLiteral("RTL")},
        layout,
        this);
    auto* density = labeledCombo(
        QStringLiteral("Density"),
        {QStringLiteral("Default"), QStringLiteral("Compact"), QStringLiteral("Comfortable")},
        layout,
        this);

    auto* seed = new QPushButton(QStringLiteral("Seed color"), this);
    layout->addWidget(seed);
    layout->addStretch(1);

    QObject::connect(
        mode,
        static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        [](int index) {
            auto options = QtMaterial::ThemeManager::instance().options();
            options.mode = index == 0
                ? QtMaterial::ThemeMode::Light
                : QtMaterial::ThemeMode::Dark;
            options.preference = index == 0
                ? QtMaterial::ThemePreference::Light
                : QtMaterial::ThemePreference::Dark;
            QtMaterial::ThemeManager::instance().setThemeOptions(options);
        });

    QObject::connect(
        contrast,
        static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        [](int index) {
            auto options = QtMaterial::ThemeManager::instance().options();
            options.contrast = index == 2
                ? QtMaterial::ContrastMode::High
                : (index == 1
                    ? QtMaterial::ContrastMode::Medium
                    : QtMaterial::ContrastMode::Standard);
            QtMaterial::ThemeManager::instance().setThemeOptions(options);
        });

    QObject::connect(
        variant,
        static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        [](int index) {
            auto options = QtMaterial::ThemeManager::instance().options();
            const bool expressive = index == 1;
            options.variant = expressive
                ? QtMaterial::ThemeVariant::Expressive
                : QtMaterial::ThemeVariant::TonalSpot;
            options.motionScheme = expressive
                ? QtMaterial::MotionScheme::Expressive
                : QtMaterial::MotionScheme::Standard;
            QtMaterial::ThemeManager::instance().setThemeOptions(options);
        });

    QObject::connect(
        direction,
        static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        [](int index) {
            QApplication::setLayoutDirection(
                index == 0 ? Qt::LeftToRight : Qt::RightToLeft);
        });

    QObject::connect(
        density,
        &QComboBox::currentTextChanged,
        this,
        [](const QString& value) {
            applyDensityKey(value.toLatin1());
        });

    QObject::connect(seed, &QPushButton::clicked, this, []() {
        const QColor color = QColorDialog::getColor(
            QtMaterial::ThemeManager::instance().options().sourceColor);
        if (!color.isValid()) return;
        auto options = QtMaterial::ThemeManager::instance().options();
        options.sourceColor = color;
        QtMaterial::ThemeManager::instance().setThemeOptions(options);
    });
}
