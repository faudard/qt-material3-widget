#include "validationformpage.h"

#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

ValidationFormPage::ValidationFormPage(QWidget *parent)
    : QWidget(parent)
{
    auto *root = new QVBoxLayout(this);
    auto *title = new QLabel(QStringLiteral("Validation Form"), this);
    auto *description = new QLabel(
        QStringLiteral("This page is intended to demonstrate validation patterns: helper text, reserved supporting lines, warning/success/error states, and submit-triggered validation."),
        this);
    description->setWordWrap(true);

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("Username"), new QLabel(QStringLiteral("[Outlined field with helper text and min-length validator]"), this));
    form->addRow(QStringLiteral("Password"), new QLabel(QStringLiteral("[Filled field with error message path]"), this));
    form->addRow(QStringLiteral("Promo code"), new QLabel(QStringLiteral("[Outlined field with success/info state]"), this));

    root->addWidget(title);
    root->addWidget(description);
    root->addLayout(form);
    root->addStretch(1);
}

ValidationFormPage::~ValidationFormPage() = default;
