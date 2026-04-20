#include "qtmaterialaccessibilityhelper_p.h"

#include <QAbstractButton>
#include <QLineEdit>
#include <QWidget>

namespace QtMaterial {

QString AccessibilityHelper::effectiveDescription(const QString& supportingText, const QString& errorText, const bool hasErrorState)
{
    if (hasErrorState && !errorText.isEmpty()) {
        return errorText;
    }
    return supportingText;
}

void AccessibilityHelper::applyButtonAccessibility(QAbstractButton* button)
{
    if (!button) {
        return;
    }

    if (button->focusPolicy() == Qt::NoFocus) {
        button->setFocusPolicy(Qt::StrongFocus);
    }

    if (button->accessibleName().isEmpty() && !button->text().isEmpty()) {
        button->setAccessibleName(button->text());
    }

    if (button->accessibleDescription().isEmpty() && !button->toolTip().isEmpty()) {
        button->setAccessibleDescription(button->toolTip());
    }
}

void AccessibilityHelper::applyInputAccessibility(
    QWidget* shell,
    QLineEdit* editor,
    const QString& label,
    const QString& supportingText,
    const QString& errorText,
    const bool hasErrorState)
{
    const QString description = effectiveDescription(supportingText, errorText, hasErrorState);

    if (shell) {
        if (shell->focusPolicy() == Qt::NoFocus) {
            shell->setFocusPolicy(Qt::StrongFocus);
        }
        if (shell->accessibleName().isEmpty() && !label.isEmpty()) {
            shell->setAccessibleName(label);
        }
        shell->setAccessibleDescription(description);
    }

    if (editor) {
        if (editor->accessibleName().isEmpty() && !label.isEmpty()) {
            editor->setAccessibleName(label);
        }
        editor->setAccessibleDescription(description);
    }
}

void AccessibilityHelper::applyDialogAccessibility(QWidget* dialog, QWidget* bodyWidget)
{
    if (!dialog) {
        return;
    }

    if (dialog->focusPolicy() == Qt::NoFocus) {
        dialog->setFocusPolicy(Qt::StrongFocus);
    }

    if (dialog->accessibleName().isEmpty()) {
        if (!dialog->windowTitle().isEmpty()) {
            dialog->setAccessibleName(dialog->windowTitle());
        } else if (bodyWidget && !bodyWidget->accessibleName().isEmpty()) {
            dialog->setAccessibleName(bodyWidget->accessibleName());
        }
    }
}

} // namespace QtMaterial
