# TextField validation summary

`QtMaterialTextFieldValidationGroup` can build a stable validation summary for
form screens. This keeps the submit handler simple and gives applications one
place to show form-level errors.

```cpp
auto* group = new QtMaterialTextFieldValidationGroup(this);

group->addField(emailField);
group->setFieldLabel(emailField, QStringLiteral("Email"));
group->setFieldErrorMessage(emailField, QStringLiteral("Enter a valid email address"));

group->addField(passwordField);
group->setFieldLabel(passwordField, QStringLiteral("Password"));
group->setFieldErrorMessage(passwordField, QStringLiteral("Password is required"));

connect(group, &QtMaterialTextFieldValidationGroup::validationSummaryChanged,
        this, [summaryLabel](const QStringList& summary) {
            summaryLabel->setText(summary.join(QStringLiteral("\n")));
            summaryLabel->setVisible(!summary.isEmpty());
        });

connect(submitButton, &QPushButton::clicked, this, [group]() {
    if (!group->validateAll()) {
        group->focusFirstInvalidField();
        return;
    }

    // Submit form.
});
```

Summary item resolution order:

1. explicit label/message set on the group;
2. the field accessible name/description;
3. object name or `Field N` fallback for labels;
4. `Invalid value` fallback for messages.

The group does not own fields. Removing a field also removes its explicit summary
label/message metadata.
