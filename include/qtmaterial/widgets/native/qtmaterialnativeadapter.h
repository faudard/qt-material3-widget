#pragma once

#include "qtmaterial/foundation/qtmaterialdensity.h"
#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/specs/qtmaterialbuttonspec.h"

class QWidget;

namespace QtMaterial {

/**
 * Unified entry point for opt-in adaptation of supported native Qt widgets.
 *
 * The facade dispatches to the specialized adapters and deliberately treats
 * each supported native control as a traversal barrier so internal Qt
 * implementation children are not adapted accidentally.
 */
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialNativeAdapter final
{
public:
    enum class WidgetKind
    {
        Unsupported,
        PushButton,
        ToolButton,
        CheckBox,
        RadioButton,
        Slider,
        ComboBox,
        LineEdit,
        ProgressBar
    };

    enum class TextFieldVariant
    {
        Outlined,
        Filled
    };

    struct Options
    {
        Options(
            Density densityValue = Density::Default,
            ButtonVariant buttonVariantValue = ButtonVariant::Text,
            TextFieldVariant textFieldVariantValue = TextFieldVariant::Outlined) noexcept
            : density(densityValue)
            , buttonVariant(buttonVariantValue)
            , textFieldVariant(textFieldVariantValue)
        {
        }

        Density density;
        ButtonVariant buttonVariant;
        TextFieldVariant textFieldVariant;
    };

    static WidgetKind kind(const QWidget* widget);
    static bool isSupported(const QWidget* widget);

    static bool apply(
        QWidget* widget,
        const Options& options = Options());

    static bool applyDeclared(
        QWidget* widget,
        const Options& fallback = Options());

    static bool isDeclared(const QWidget* widget);

    static bool remove(QWidget* widget);
    static bool isApplied(const QWidget* widget);

    static void setOptOut(QWidget* widget, bool excluded);
    static bool isOptedOut(const QWidget* widget);

    /**
     * Adapts supported widgets below root using direct-child traversal.
     *
     * A supported widget is a traversal barrier: once adapted (or opted out),
     * its implementation children are not visited. First-class QtMaterial
     * widgets and their implementation children are always skipped.
     */
    static int applyToDescendants(
        QWidget* root,
        const Options& options = Options());

    static int applyDeclaredToDescendants(
        QWidget* root,
        const Options& fallback = Options());

    /**
     * Removes facade-managed specialized adapters below root.
     *
     * The same traversal barriers as applyToDescendants() are used.
     */
    static int removeFromDescendants(QWidget* root);

    static const char* adaptPropertyName() noexcept;
};

} // namespace QtMaterial
