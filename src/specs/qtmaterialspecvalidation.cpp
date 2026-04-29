#include "qtmaterial/specs/qtmaterialspecvalidation.h"

#include <QColor>
#include <QStringList>

namespace QtMaterial {
namespace {
void requireColor(SpecValidationResult& result, const QColor& color, const QString& code, const QString& name)
{
    if (!color.isValid()) {
        result.add(SpecValidationSeverity::Error, code,
                   QStringLiteral("%1 must be a valid QColor").arg(name));
    }
}

void requirePositive(SpecValidationResult& result, int value, const QString& code, const QString& name)
{
    if (value <= 0) {
        result.add(SpecValidationSeverity::Error, code,
                   QStringLiteral("%1 must be greater than zero").arg(name));
    }
}

void requireNonNegative(SpecValidationResult& result, int value, const QString& code, const QString& name)
{
    if (value < 0) {
        result.add(SpecValidationSeverity::Error, code,
                   QStringLiteral("%1 must be non-negative").arg(name));
    }
}

void requireTouchTarget(SpecValidationResult& result, const QSize& size, const QString& code)
{
    if (size.width() < 48 || size.height() < 48) {
        result.add(SpecValidationSeverity::Warning, code,
                   QStringLiteral("touch target should be at least 48x48 for accessible pointer input"));
    }
}
} // namespace

void SpecValidationResult::add(SpecValidationSeverity severity,
                               const QString& code,
                               const QString& message)
{
    m_issues.push_back({severity, code, message});
}

bool SpecValidationResult::isValid() const
{
    return !hasErrors();
}

bool SpecValidationResult::hasErrors() const
{
    for (const auto& issue : m_issues) {
        if (issue.severity == SpecValidationSeverity::Error) {
            return true;
        }
    }
    return false;
}

QVector<SpecValidationIssue> SpecValidationResult::issues() const
{
    return m_issues;
}

QStringList SpecValidationResult::messages() const
{
    QStringList out;
    for (const auto& issue : m_issues) {
        out << QStringLiteral("%1: %2").arg(issue.code, issue.message);
    }
    return out;
}

SpecValidationResult validateButtonSpec(const ButtonSpec& spec)
{
    SpecValidationResult result;
    requireColor(result, spec.labelColor, QStringLiteral("button.labelColor.invalid"), QStringLiteral("ButtonSpec::labelColor"));
    requireColor(result, spec.iconColor, QStringLiteral("button.iconColor.invalid"), QStringLiteral("ButtonSpec::iconColor"));
    requireColor(result, spec.stateLayerColor, QStringLiteral("button.stateLayerColor.invalid"), QStringLiteral("ButtonSpec::stateLayerColor"));
    requirePositive(result, spec.containerHeight, QStringLiteral("button.containerHeight.invalid"), QStringLiteral("ButtonSpec::containerHeight"));
    requirePositive(result, spec.horizontalPadding, QStringLiteral("button.horizontalPadding.invalid"), QStringLiteral("ButtonSpec::horizontalPadding"));
    requirePositive(result, spec.iconSize, QStringLiteral("button.iconSize.invalid"), QStringLiteral("ButtonSpec::iconSize"));
    requireNonNegative(result, spec.iconSpacing, QStringLiteral("button.iconSpacing.invalid"), QStringLiteral("ButtonSpec::iconSpacing"));
    requireTouchTarget(result, spec.touchTarget, QStringLiteral("button.touchTarget.small"));
    return result;
}

SpecValidationResult validateIconButtonSpec(const IconButtonSpec& spec)
{
    SpecValidationResult result;
    requireColor(result, spec.iconColor, QStringLiteral("iconButton.iconColor.invalid"), QStringLiteral("IconButtonSpec::iconColor"));
    requireColor(result, spec.stateLayerColor, QStringLiteral("iconButton.stateLayerColor.invalid"), QStringLiteral("IconButtonSpec::stateLayerColor"));
    requirePositive(result, spec.containerSize, QStringLiteral("iconButton.containerSize.invalid"), QStringLiteral("IconButtonSpec::containerSize"));
    requirePositive(result, spec.iconSize, QStringLiteral("iconButton.iconSize.invalid"), QStringLiteral("IconButtonSpec::iconSize"));
    requireTouchTarget(result, spec.touchTarget, QStringLiteral("iconButton.touchTarget.small"));
    if (spec.iconSize > spec.containerSize) {
        result.add(SpecValidationSeverity::Error, QStringLiteral("iconButton.iconSize.tooLarge"),
                   QStringLiteral("IconButtonSpec::iconSize must not exceed containerSize"));
    }
    return result;
}

SpecValidationResult validateTextFieldSpec(const TextFieldSpec& spec)
{
    SpecValidationResult result;
    requireColor(result, spec.inputTextColor, QStringLiteral("textField.inputTextColor.invalid"), QStringLiteral("TextFieldSpec::inputTextColor"));
    requireColor(result, spec.cursorColor, QStringLiteral("textField.cursorColor.invalid"), QStringLiteral("TextFieldSpec::cursorColor"));
    requireColor(result, spec.errorColor, QStringLiteral("textField.errorColor.invalid"), QStringLiteral("TextFieldSpec::errorColor"));
    requirePositive(result, spec.minHeight, QStringLiteral("textField.minHeight.invalid"), QStringLiteral("TextFieldSpec::minHeight"));
    requirePositive(result, spec.horizontalPadding, QStringLiteral("textField.horizontalPadding.invalid"), QStringLiteral("TextFieldSpec::horizontalPadding"));
    requireNonNegative(result, spec.verticalPadding, QStringLiteral("textField.verticalPadding.invalid"), QStringLiteral("TextFieldSpec::verticalPadding"));
    requirePositive(result, spec.focusedOutlineWidth, QStringLiteral("textField.focusedOutlineWidth.invalid"), QStringLiteral("TextFieldSpec::focusedOutlineWidth"));
    if (spec.focusedOutlineWidth < spec.outlineWidth) {
        result.add(SpecValidationSeverity::Warning, QStringLiteral("textField.focusedOutlineWidth.tooSmall"),
                   QStringLiteral("focused outline should be at least as visible as the default outline"));
    }
    return result;
}

SpecValidationResult validateDialogSpec(const DialogSpec& spec)
{
    SpecValidationResult result;
    requireColor(result, spec.containerColor, QStringLiteral("dialog.containerColor.invalid"), QStringLiteral("DialogSpec::containerColor"));
    requireColor(result, spec.scrimColor, QStringLiteral("dialog.scrimColor.invalid"), QStringLiteral("DialogSpec::scrimColor"));
    requirePositive(result, spec.maxWidth, QStringLiteral("dialog.maxWidth.invalid"), QStringLiteral("DialogSpec::maxWidth"));
    requireNonNegative(result, spec.padding, QStringLiteral("dialog.padding.invalid"), QStringLiteral("DialogSpec::padding"));
    return result;
}

SpecValidationResult validateSnackbarSpec(const SnackbarSpec& spec)
{
    SpecValidationResult result;
    requireColor(result, spec.containerColor, QStringLiteral("snackbar.containerColor.invalid"), QStringLiteral("SnackbarSpec::containerColor"));
    requireColor(result, spec.textColor, QStringLiteral("snackbar.textColor.invalid"), QStringLiteral("SnackbarSpec::textColor"));
    requirePositive(result, spec.minHeight, QStringLiteral("snackbar.minHeight.invalid"), QStringLiteral("SnackbarSpec::minHeight"));
    requirePositive(result, spec.maxWidth, QStringLiteral("snackbar.maxWidth.invalid"), QStringLiteral("SnackbarSpec::maxWidth"));
    requirePositive(result, spec.dismissButtonSize, QStringLiteral("snackbar.dismissButtonSize.invalid"), QStringLiteral("SnackbarSpec::dismissButtonSize"));
    return result;
}

} // namespace QtMaterial
