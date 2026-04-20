#include "basicformpage.h"

#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

// Replace these includes with the real integrated public headers.
// #include <qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h>
// #include <qtmaterial/widgets/inputs/qtmaterialfilledtextfield.h>
// #include <qtmaterial/widgets/buttons/qtmaterialfilledbutton.h>
// #include <qtmaterial/widgets/buttons/qtmaterialoutlinedbutton.h>

BasicFormPage::BasicFormPage(QWidget *parent)
    : QWidget(parent)
{
    auto *root = new QVBoxLayout(this);
    auto *title = new QLabel(QStringLiteral("Basic Form"), this);
    title->setObjectName(QStringLiteral("formTitle"));

    auto *description = new QLabel(
        QStringLiteral("This page is intended to demonstrate a realistic basic form using outlined and filled text fields, primary and secondary actions, supporting text, and consistent vertical rhythm."),
        this);
    description->setWordWrap(true);

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("First name"), new QLabel(QStringLiteral("[QtMaterialOutlinedTextField placeholder]"), this));
    form->addRow(QStringLiteral("Last name"), new QLabel(QStringLiteral("[QtMaterialOutlinedTextField placeholder]"), this));
    form->addRow(QStringLiteral("Email"), new QLabel(QStringLiteral("[QtMaterialFilledTextField placeholder]"), this));
    form->addRow(QStringLiteral("Company"), new QLabel(QStringLiteral("[QtMaterialOutlinedTextField placeholder]"), this));

    root->addWidget(title);
    root->addWidget(description);
    root->addLayout(form);
    root->addStretch(1);
}

BasicFormPage::~BasicFormPage() = default;
