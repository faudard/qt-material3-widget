#pragma once

#include <QString>
#include <QVector>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"
#include "qtmaterial/specs/qtmaterialdialogspec.h"
#include "qtmaterial/specs/qtmaterialiconbuttonspec.h"
#include "qtmaterial/specs/qtmaterialsnackbarspec.h"
#include "qtmaterial/specs/qtmaterialtextfieldspec.h"

namespace QtMaterial {

enum class SpecValidationSeverity {
    Info,
    Warning,
    Error
};

struct QTMATERIAL3_SPECS_EXPORT SpecValidationIssue
{
    SpecValidationSeverity severity = SpecValidationSeverity::Error;
    QString code;
    QString message;
};

class QTMATERIAL3_SPECS_EXPORT SpecValidationResult
{
public:
    void add(SpecValidationSeverity severity, const QString& code, const QString& message);
    bool isValid() const;
    bool hasErrors() const;
    QVector<SpecValidationIssue> issues() const;
    QStringList messages() const;

private:
    QVector<SpecValidationIssue> m_issues;
};

QTMATERIAL3_SPECS_EXPORT SpecValidationResult validateButtonSpec(const ButtonSpec& spec);
QTMATERIAL3_SPECS_EXPORT SpecValidationResult validateIconButtonSpec(const IconButtonSpec& spec);
QTMATERIAL3_SPECS_EXPORT SpecValidationResult validateTextFieldSpec(const TextFieldSpec& spec);
QTMATERIAL3_SPECS_EXPORT SpecValidationResult validateDialogSpec(const DialogSpec& spec);
QTMATERIAL3_SPECS_EXPORT SpecValidationResult validateSnackbarSpec(const SnackbarSpec& spec);

} // namespace QtMaterial
