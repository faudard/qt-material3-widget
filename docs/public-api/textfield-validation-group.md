# TextField validation group

`QtMaterialTextFieldValidationGroup` is a small coordinator for form-like screens that
contain multiple `QtMaterialOutlinedTextField` or `QtMaterialFilledTextField`
instances.

The group does not own fields. It observes their acceptable-input state and forwards
validation/reset requests.

```cpp
auto* group = new QtMaterialTextFieldValidationGroup(this);

group->addField(emailField);
group->addField(passwordField);

connect(group, &QtMaterialTextFieldValidationGroup::acceptableChanged,
        submitButton, &QWidget::setEnabled);

connect(submitButton, &QPushButton::clicked, this, [group]() {
    if (!group->validateAll()) {
        group->focusFirstInvalidField();
        return;
    }

    // Submit form.
});
```

Use this with `ErrorDisplayMode::AfterCommit` when errors should stay hidden until
the user submits the form for the first time.
