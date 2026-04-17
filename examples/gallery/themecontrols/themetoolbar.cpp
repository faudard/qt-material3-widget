#include "themetoolbar.h"
#include <QColorDialog>
#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include "qtmaterial/theme/qtmaterialthememanager.h"
ThemeToolbar::ThemeToolbar(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QHBoxLayout(this);
    auto* mode = new QComboBox(this);
    mode->addItem(QStringLiteral("Light"));
    mode->addItem(QStringLiteral("Dark"));
    auto* seed = new QPushButton(QStringLiteral("Seed color"), this);
    layout->addWidget(mode);
    layout->addWidget(seed);
    layout->addStretch(1);
    QObject::connect(mode, &QComboBox::currentIndexChanged, this, [mode]() {
        auto options = QtMaterial::ThemeManager::instance().options();
        options.mode = mode->currentIndex() == 0 ? QtMaterial::ThemeMode::Light : QtMaterial::ThemeMode::Dark;
        QtMaterial::ThemeManager::instance().setThemeOptions(options);
    });
    QObject::connect(seed, &QPushButton::clicked, this, []() {
        QColor color = QColorDialog::getColor(QtMaterial::ThemeManager::instance().options().sourceColor);
        if (!color.isValid()) return;
        auto options = QtMaterial::ThemeManager::instance().options();
        options.sourceColor = color;
        QtMaterial::ThemeManager::instance().setThemeOptions(options);
    });
}
