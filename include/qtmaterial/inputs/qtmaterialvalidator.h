#pragma once

#include <memory>
#include <QString>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/inputs/qtmaterialvalidationresult.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT Validator
{
public:
    Validator() = default;
    virtual ~Validator() = default;

    virtual ValidationResult validate(const QString& text) const = 0;
};

class QTMATERIAL3_WIDGETS_EXPORT RegexValidator final : public Validator
{
public:
    explicit RegexValidator(const QString& pattern,
                            ValidationSeverity failureSeverity = ValidationSeverity::Error,
                            QString failureMessage = QString());

    ValidationResult validate(const QString& text) const override;

private:
    QString m_pattern;
    ValidationSeverity m_failureSeverity = ValidationSeverity::Error;
    QString m_failureMessage;
};

class QTMATERIAL3_WIDGETS_EXPORT MinLengthValidator final : public Validator
{
public:
    explicit MinLengthValidator(int minLength,
                                ValidationSeverity failureSeverity = ValidationSeverity::Error,
                                QString failureMessage = QString());

    ValidationResult validate(const QString& text) const override;

private:
    int m_minLength = 0;
    ValidationSeverity m_failureSeverity = ValidationSeverity::Error;
    QString m_failureMessage;
};

class QTMATERIAL3_WIDGETS_EXPORT CompositeValidator final : public Validator
{
public:
    CompositeValidator() = default;
    ~CompositeValidator() override = default;

    void add(std::shared_ptr<Validator> validator);
    ValidationResult validate(const QString& text) const override;

private:
    QList<std::shared_ptr<Validator>> m_validators;
};

} // namespace QtMaterial
