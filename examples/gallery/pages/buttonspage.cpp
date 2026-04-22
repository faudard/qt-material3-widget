#include "buttonspage.h"

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QStyle>
#include <QVBoxLayout>

#include "qtmaterial/widgets/buttons/qtmaterialelevatedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialextendedfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialfab.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfilledtonalbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialiconbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

namespace {

QLabel* makeTitle(const QString& text, QWidget* parent)
{
    auto* label = new QLabel(text, parent);
    QFont f = label->font();
    f.setBold(true);
    f.setPointSizeF(f.pointSizeF() + 1.5);
    label->setFont(f);
    return label;
}

QLabel* makeRowLabel(const QString& text, QWidget* parent)
{
    auto* label = new QLabel(text, parent);
    label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    return label;
}

QIcon demoIcon()
{
    if (qApp && qApp->style()) {
        return qApp->style()->standardIcon(QStyle::SP_DialogApplyButton);
    }
    return QIcon();
}

template <typename T>
T* makeButton(QWidget* parent)
{
    auto* b = new T(parent);
    b->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
    return b;
}

} // namespace

ButtonsPage::ButtonsPage(QWidget* parent)
    : QWidget(parent)
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);

    root->addWidget(makeTitle(QStringLiteral("Buttons"), this));

    auto* grid = new QGridLayout();
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(10);
    grid->setVerticalSpacing(8);

    int row = 0;

    auto* text = makeButton<QtMaterial::QtMaterialTextButton>(this);
    text->setText(QStringLiteral("Text"));
    grid->addWidget(makeRowLabel(QStringLiteral("Text"), this), row, 0);
    grid->addWidget(text, row++, 1, Qt::AlignLeft);

    auto* filled = makeButton<QtMaterial::QtMaterialFilledButton>(this);
    filled->setText(QStringLiteral("Filled"));
    grid->addWidget(makeRowLabel(QStringLiteral("Filled"), this), row, 0);
    grid->addWidget(filled, row++, 1, Qt::AlignLeft);

    auto* tonal = makeButton<QtMaterial::QtMaterialFilledTonalButton>(this);
    tonal->setText(QStringLiteral("Tonal"));
    grid->addWidget(makeRowLabel(QStringLiteral("Tonal"), this), row, 0);
    grid->addWidget(tonal, row++, 1, Qt::AlignLeft);

    auto* outlined = makeButton<QtMaterial::QtMaterialOutlinedButton>(this);
    outlined->setText(QStringLiteral("Outlined"));
    grid->addWidget(makeRowLabel(QStringLiteral("Outlined"), this), row, 0);
    grid->addWidget(outlined, row++, 1, Qt::AlignLeft);

    auto* elevated = makeButton<QtMaterial::QtMaterialElevatedButton>(this);
    elevated->setText(QStringLiteral("Elevated"));
    grid->addWidget(makeRowLabel(QStringLiteral("Elevated"), this), row, 0);
    grid->addWidget(elevated, row++, 1, Qt::AlignLeft);

    auto* icon = makeButton<QtMaterial::QtMaterialIconButton>(this);
    icon->setIcon(demoIcon());
    grid->addWidget(makeRowLabel(QStringLiteral("Icon"), this), row, 0);
    grid->addWidget(icon, row++, 1, Qt::AlignLeft);

    auto* fab = makeButton<QtMaterial::QtMaterialFab>(this);
    fab->setIcon(demoIcon());
    grid->addWidget(makeRowLabel(QStringLiteral("FAB"), this), row, 0);
    grid->addWidget(fab, row++, 1, Qt::AlignLeft);

    auto* extFab = makeButton<QtMaterial::QtMaterialExtendedFab>(this);
    extFab->setIcon(demoIcon());
    extFab->setText(QStringLiteral("Extended FAB"));
    grid->addWidget(makeRowLabel(QStringLiteral("Extended FAB"), this), row, 0);
    grid->addWidget(extFab, row++, 1, Qt::AlignLeft);

    grid->setColumnStretch(2, 1);

    root->addLayout(grid);
    root->addStretch(1);
}