#pragma once

#include <QString>
#include <QStringList>

namespace QtMaterial {

enum class ValidationSeverity {
    None,
    Info,
    Success,
    Warning,
    Error
};

enum class ValidationTrigger {
    Manual,
    OnBlur,
    OnSubmit,
    OnInputDebounced
};

struct ValidationResult
{
    ValidationSeverity severity = ValidationSeverity::None;
    QString message;
    QString errorCode;
    QStringList details;

    bool isValid() const noexcept
    {
        return severity != ValidationSeverity::Error;
    }

    bool hasMessage() const noexcept
    {
        return !message.isEmpty();
    }
};

} // namespace QtMaterial
