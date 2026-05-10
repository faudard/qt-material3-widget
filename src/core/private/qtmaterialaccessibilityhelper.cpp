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


void qtm3SyncAutoAccessibleName(QWidget* widget, const QString& candidate)
{
    if (!widget) {
        return;
    }

    const QString trimmedCandidate = candidate.trimmed();
    const QString previousAuto =
        widget->property(kAutoAccessibleNameProperty).toString();
    const QString current = widget->accessibleName();

    if (trimmedCandidate.isEmpty()) {
        if (!previousAuto.isEmpty() && current == previousAuto) {
            widget->setAccessibleName(QString());
        }
        widget->setProperty(kAutoAccessibleNameProperty, QString());
        return;
    }

    if (current.isEmpty() || current == previousAuto) {
        widget->setAccessibleName(trimmedCandidate);
        widget->setProperty(kAutoAccessibleNameProperty, trimmedCandidate);
    }
}

void qtm3SyncAutoAccessibleDescription(QWidget* widget, const QString& candidate)
{
    if (!widget) {
        return;
    }

    const QString trimmedCandidate = candidate.trimmed();
    const QString previousAuto =
        widget->property(kAutoAccessibleDescriptionProperty).toString();
    const QString current = widget->accessibleDescription();

    if (trimmedCandidate.isEmpty()) {
        if (!previousAuto.isEmpty() && current == previousAuto) {
            widget->setAccessibleDescription(QString());
        }
        widget->setProperty(kAutoAccessibleDescriptionProperty, QString());
        return;
    }

    if (current.isEmpty() || current == previousAuto) {
        widget->setAccessibleDescription(trimmedCandidate);
        widget->setProperty(kAutoAccessibleDescriptionProperty, trimmedCandidate);
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
    const QString shellDescription =
        effectiveDescription(supportingText, errorText, hasErrorState);
    const QString compactEditorDescription =
        hasErrorState && !errorText.isEmpty() ? errorText : supportingText;
    const QString editorDescription =
        shell && shell->isVisible() ? shellDescription : compactEditorDescription;

    if (shell) {
        if (shell->focusPolicy() == Qt::NoFocus) {
            shell->setFocusPolicy(Qt::StrongFocus);
        }

        qtm3SyncAutoAccessibleName(shell, label);
        qtm3SyncAutoAccessibleDescription(shell, shellDescription);
    }

    if (editor) {
        qtm3SyncAutoAccessibleName(editor, label);
        qtm3SyncAutoAccessibleDescription(editor, editorDescription);
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
