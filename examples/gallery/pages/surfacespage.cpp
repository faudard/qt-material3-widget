#include "surfacespage.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "qtmaterial/widgets/surfaces/qtmaterialdialog.h"
SurfacesPage::SurfacesPage(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    auto* button = new QPushButton(QStringLiteral("Open dialog"), this);
    auto* dialog = new QtMaterial::QtMaterialDialog(this);
    dialog->resize(360, 220);
    dialog->setBodyWidget(new QLabel(QStringLiteral("Phase 3 dialog scaffold"), dialog));
    QObject::connect(button, &QPushButton::clicked, dialog, &QtMaterial::QtMaterialDialog::open);
    layout->addWidget(button);
    layout->addStretch(1);
}
