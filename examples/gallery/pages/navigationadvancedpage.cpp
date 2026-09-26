#include "navigationadvancedpage.h"

#include <QLabel>
#include <QPushButton>
#include <QStringListModel>
#include <QVBoxLayout>

#include "qtmaterial/widgets/navigation/qtmaterialbreadcrumb.h"
#include "qtmaterial/widgets/navigation/qtmaterialcommandpalette.h"

NavigationAdvancedPage::NavigationAdvancedPage(QWidget* parent)
    : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(12);

    layout->addWidget(new QLabel(tr("0.9 — Desktop navigation"), this));

    auto* breadcrumb = new QtMaterial::QtMaterialBreadcrumb(this);
    breadcrumb->setItems({
        tr("Workspace"),
        tr("Requirements"),
        tr("REQ-42")
    });
    layout->addWidget(breadcrumb);

    auto* model = new QStringListModel({
        tr("Open workspace"),
        tr("Search requirements"),
        tr("Toggle dense mode"),
        tr("Run validation")
    }, this);

    auto* palette = new QtMaterial::QtMaterialCommandPalette(this);
    palette->setSourceModel(model);

    auto* openPalette = new QPushButton(tr("Open command palette"), this);
    connect(openPalette, &QPushButton::clicked, palette, [palette]() {
        palette->setQuery(QString());
        palette->show();
        palette->raise();
        palette->activateWindow();
    });
    layout->addWidget(openPalette);
    layout->addStretch(1);
}
