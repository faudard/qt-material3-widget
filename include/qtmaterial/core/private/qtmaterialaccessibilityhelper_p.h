#pragma once

#include <QString>
#include "qtmaterial/qtmaterialglobal.h"

class QAbstractButton;
class QLineEdit;
class QWidget;

namespace QtMaterial {

class QTMATERIAL3_CORE_EXPORT AccessibilityHelper
{
public:
    static void applyButtonAccessibility(QAbstractButton* button);
    static void applyInputAccessibility(
        QWidget* shell,
        QLineEdit* editor,
        const QString& label,
        const QString& supportingText,
        const QString& errorText,
        bool hasErrorState);
    static void applyDialogAccessibility(QWidget* dialog, QWidget* bodyWidget = nullptr);
    static QString effectiveDescription(const QString& supportingText, const QString& errorText, bool hasErrorState);
};

} // namespace QtMaterial