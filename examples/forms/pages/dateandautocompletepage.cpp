#include "dateandautocompletepage.h"

#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

DateAndAutocompletePage::DateAndAutocompletePage(QWidget *parent)
    : QWidget(parent)
{
    auto *root = new QVBoxLayout(this);
    auto *title = new QLabel(QStringLiteral("Date & Autocomplete"), this);
    auto *description = new QLabel(
        QStringLiteral("This page is intended to demonstrate composite form flows with date fields, autocomplete popup integration, prefix/suffix content, and keyboard-friendly navigation."),
        this);
    description->setWordWrap(true);

    auto *form = new QFormLayout;
    form->addRow(QStringLiteral("Location"), new QLabel(QStringLiteral("[Outlined field + QtMaterialAutocompletePopup placeholder]"), this));
    form->addRow(QStringLiteral("Travel date"), new QLabel(QStringLiteral("[QtMaterialDateField placeholder]"), this));
    form->addRow(QStringLiteral("Notes"), new QLabel(QStringLiteral("[Filled field with prefix/suffix support placeholder]"), this));

    root->addWidget(title);
    root->addWidget(description);
    root->addLayout(form);
    root->addStretch(1);
}

DateAndAutocompletePage::~DateAndAutocompletePage() = default;
