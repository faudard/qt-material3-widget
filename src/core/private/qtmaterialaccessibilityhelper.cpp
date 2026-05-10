#include "qtmaterialaccessibilityhelper_p.h"
#include <QString>
#include <QVariant>

#include <QAbstractButton>
#include <QLineEdit>
#include <QWidget>

namespace {

constexpr const char* kAutoAccessibleNameProperty = "_qtm3_autoAccessibleName";
constexpr const char* kAutoAccessibleDescriptionProperty = "_qtm3_autoAccessibleDescription";

QString qtm3StripMnemonicMarkers(const QString& text)
{
    QString result;
    result.reserve(text.size());

    for (int i = 0; i < text.size(); ++i) {
        const QChar ch = text.at(i);
        if (ch != QLatin1Char('&')) {
            result.append(ch);
            continue;
        }

        if (i + 1 < text.size() && text.at(i + 1) == QLatin1Char('&')) {
            result.append(QLatin1Char('&'));
            ++i;
        }
    }

    return result.trimmed();
}

void qtm3SyncAutoAccessibleName(QAbstractButton* button)
{
    const QString candidate = qtm3StripMnemonicMarkers(button->text());
    const QString previousAuto =
        button->property(kAutoAccessibleNameProperty).toString();
    const QString current = button->accessibleName();

    if (candidate.isEmpty()) {
        if (!previousAuto.isEmpty() && current == previousAuto) {
            button->setAccessibleName(QString());
        }
        button->setProperty(kAutoAccessibleNameProperty, QString());
        return;
    }

    if (current.isEmpty() || current == previousAuto) {
        button->setAccessibleName(candidate);
        button->setProperty(kAutoAccessibleNameProperty, candidate);
    }
}

void qtm3SyncAutoAccessibleDescription(QAbstractButton* button)
{
    const QString candidate = button->toolTip().trimmed();
    const QString previousAuto =
        button->property(kAutoAccessibleDescriptionProperty).toString();
    const QString current = button->accessibleDescription();

    if (candidate.isEmpty()) {
        if (!previousAuto.isEmpty() && current == previousAuto) {
            button->setAccessibleDescription(QString());
        }
        button->setProperty(kAutoAccessibleDescriptionProperty, QString());
        return;
    }

    if (current.isEmpty() || current == previousAuto) {
        button->setAccessibleDescription(candidate);
        button->setProperty(kAutoAccessibleDescriptionProperty, candidate);
    }
}

} // namespace

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

    qtm3SyncAutoAccessibleName(button);
    qtm3SyncAutoAccessibleDescription(button);
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
