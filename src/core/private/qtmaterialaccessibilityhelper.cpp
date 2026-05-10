#include "qtmaterialaccessibilityhelper_p.h"
#include <QString>
#include <QVariant>

#include <QAbstractButton>
#include <QLineEdit>
#include <QWidget>

namespace QtMaterial {

QString AccessibilityHelper::effectiveDescription(const QString& supportingText, const QString& errorText, const bool hasErrorState)
{
    if (hasErrorState && !errorText.isEmpty()) {
        if (!supportingText.isEmpty()) {
            return supportingText + QStringLiteral("\\n") + errorText;
        }
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

    constexpr const char* autoNameProperty = "_qtm3_auto_accessible_name";
    constexpr const char* autoDescriptionProperty = "_qtm3_auto_accessible_description";

    QString textName = button->text().trimmed();
    textName.remove(QLatin1Char('&'));

    const QString currentName = button->accessibleName().trimmed();
    const QString previousAutoName = button->property(autoNameProperty).toString().trimmed();
    const bool currentNameIsAuto = !previousAutoName.isEmpty() && currentName == previousAutoName;
    const bool currentNameIsExplicit = !currentName.isEmpty() && !currentNameIsAuto;

    if (!currentNameIsExplicit && !textName.isEmpty()) {
        button->setAccessibleName(textName);
        button->setProperty(autoNameProperty, textName);
    } else if (!currentNameIsExplicit && textName.isEmpty() && currentNameIsAuto) {
        button->setAccessibleName(QString());
        button->setProperty(autoNameProperty, QString());
    }

    const QString tooltipDescription = button->toolTip().trimmed();
    const QString currentDescription = button->accessibleDescription().trimmed();
    const QString previousAutoDescription = button->property(autoDescriptionProperty).toString().trimmed();
    const bool currentDescriptionIsAuto = !previousAutoDescription.isEmpty() && currentDescription == previousAutoDescription;
    const bool currentDescriptionIsExplicit = !currentDescription.isEmpty() && !currentDescriptionIsAuto;

    if (!currentDescriptionIsExplicit && !tooltipDescription.isEmpty()) {
        button->setAccessibleDescription(tooltipDescription);
        button->setProperty(autoDescriptionProperty, tooltipDescription);
    } else if (!currentDescriptionIsExplicit && tooltipDescription.isEmpty() && currentDescriptionIsAuto) {
        button->setAccessibleDescription(QString());
        button->setProperty(autoDescriptionProperty, QString());
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
