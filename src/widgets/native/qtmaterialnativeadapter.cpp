#include "qtmaterial/widgets/native/qtmaterialnativeadapter.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QToolButton>
#include <QWidget>

#include "qtmaterial/widgets/native/qtmaterialbuttonadapter.h"
#include "qtmaterial/widgets/native/qtmaterialcomboboxadapter.h"
#include "qtmaterial/widgets/native/qtmateriallineeditadapter.h"
#include "qtmaterial/widgets/native/qtmaterialprogressbaradapter.h"
#include "qtmaterial/widgets/native/qtmaterialselectionadapter.h"
#include "qtmaterial/widgets/native/qtmaterialslideradapter.h"
#include "qtmaterial/widgets/native/qtmaterialtoolbuttonadapter.h"

namespace QtMaterial {
namespace {

bool hasMaterialClassName(const QWidget* widget)
{
    if (!widget || !widget->metaObject()) {
        return false;
    }

    QByteArray className(widget->metaObject()->className());
    const int scope = className.lastIndexOf("::");
    if (scope >= 0) {
        className = className.mid(scope + 2);
    }
    return className.startsWith("QtMaterial");
}

bool isInsideFirstClassMaterialWidget(const QWidget* widget)
{
    for (const QWidget* current = widget;
         current;
         current = current->parentWidget()) {
        if (hasMaterialClassName(current)) {
            return true;
        }
    }
    return false;
}

QtMaterialLineEditAdapter::Variant lineEditVariant(
    QtMaterialNativeAdapter::TextFieldVariant variant)
{
    return variant
            == QtMaterialNativeAdapter::TextFieldVariant::Filled
        ? QtMaterialLineEditAdapter::Variant::Filled
        : QtMaterialLineEditAdapter::Variant::Outlined;
}

QList<QWidget*> directChildWidgets(QWidget* widget)
{
    if (!widget) {
        return {};
    }
    return widget->findChildren<QWidget*>(
        QString(),
        Qt::FindDirectChildrenOnly);
}

} // namespace

QtMaterialNativeAdapter::WidgetKind
QtMaterialNativeAdapter::kind(const QWidget* widget)
{
    if (!widget
        || isInsideFirstClassMaterialWidget(widget)) {
        return WidgetKind::Unsupported;
    }

    if (qobject_cast<const QPushButton*>(widget)) {
        return WidgetKind::PushButton;
    }
    if (qobject_cast<const QToolButton*>(widget)) {
        return WidgetKind::ToolButton;
    }
    if (qobject_cast<const QCheckBox*>(widget)) {
        return WidgetKind::CheckBox;
    }
    if (qobject_cast<const QRadioButton*>(widget)) {
        return WidgetKind::RadioButton;
    }
    if (qobject_cast<const QSlider*>(widget)) {
        return WidgetKind::Slider;
    }
    if (qobject_cast<const QComboBox*>(widget)) {
        return WidgetKind::ComboBox;
    }
    if (qobject_cast<const QLineEdit*>(widget)) {
        return WidgetKind::LineEdit;
    }
    if (qobject_cast<const QProgressBar*>(widget)) {
        return WidgetKind::ProgressBar;
    }

    return WidgetKind::Unsupported;
}

bool QtMaterialNativeAdapter::isSupported(
    const QWidget* widget)
{
    return kind(widget) != WidgetKind::Unsupported;
}

bool QtMaterialNativeAdapter::apply(
    QWidget* widget,
    const Options& options)
{
    if (!widget || isOptedOut(widget)) {
        return false;
    }

    switch (kind(widget)) {
    case WidgetKind::PushButton:
        QtMaterialButtonAdapter::apply(
            static_cast<QPushButton*>(widget),
            options.buttonVariant,
            options.density);
        return true;

    case WidgetKind::ToolButton:
        QtMaterialToolButtonAdapter::apply(
            static_cast<QToolButton*>(widget),
            options.buttonVariant,
            options.density);
        return true;

    case WidgetKind::CheckBox:
        QtMaterialSelectionAdapter::apply(
            static_cast<QCheckBox*>(widget),
            options.density);
        return true;

    case WidgetKind::RadioButton:
        QtMaterialSelectionAdapter::apply(
            static_cast<QRadioButton*>(widget),
            options.density);
        return true;

    case WidgetKind::Slider:
        QtMaterialSliderAdapter::apply(
            static_cast<QSlider*>(widget),
            options.density);
        return true;

    case WidgetKind::ComboBox:
        QtMaterialComboBoxAdapter::apply(
            static_cast<QComboBox*>(widget),
            options.density);
        return true;

    case WidgetKind::LineEdit:
        QtMaterialLineEditAdapter::apply(
            static_cast<QLineEdit*>(widget),
            lineEditVariant(options.textFieldVariant),
            options.density);
        return true;

    case WidgetKind::ProgressBar:
        QtMaterialProgressBarAdapter::apply(
            static_cast<QProgressBar*>(widget));
        return true;

    case WidgetKind::Unsupported:
    default:
        return false;
    }
}

bool QtMaterialNativeAdapter::remove(QWidget* widget)
{
    if (!widget) {
        return false;
    }

    switch (kind(widget)) {
    case WidgetKind::PushButton: {
        auto* button = static_cast<QPushButton*>(widget);
        if (!QtMaterialButtonAdapter::isApplied(button)) {
            return false;
        }
        QtMaterialButtonAdapter::remove(button);
        return true;
    }

    case WidgetKind::ToolButton: {
        auto* button = static_cast<QToolButton*>(widget);
        if (!QtMaterialToolButtonAdapter::isApplied(button)) {
            return false;
        }
        QtMaterialToolButtonAdapter::remove(button);
        return true;
    }

    case WidgetKind::CheckBox: {
        auto* checkbox = static_cast<QCheckBox*>(widget);
        if (!QtMaterialSelectionAdapter::isApplied(checkbox)) {
            return false;
        }
        QtMaterialSelectionAdapter::remove(checkbox);
        return true;
    }

    case WidgetKind::RadioButton: {
        auto* radio = static_cast<QRadioButton*>(widget);
        if (!QtMaterialSelectionAdapter::isApplied(radio)) {
            return false;
        }
        QtMaterialSelectionAdapter::remove(radio);
        return true;
    }

    case WidgetKind::Slider: {
        auto* slider = static_cast<QSlider*>(widget);
        if (!QtMaterialSliderAdapter::isApplied(slider)) {
            return false;
        }
        QtMaterialSliderAdapter::remove(slider);
        return true;
    }

    case WidgetKind::ComboBox: {
        auto* combo = static_cast<QComboBox*>(widget);
        if (!QtMaterialComboBoxAdapter::isApplied(combo)) {
            return false;
        }
        QtMaterialComboBoxAdapter::remove(combo);
        return true;
    }

    case WidgetKind::LineEdit: {
        auto* lineEdit = static_cast<QLineEdit*>(widget);
        if (!QtMaterialLineEditAdapter::isApplied(lineEdit)) {
            return false;
        }
        QtMaterialLineEditAdapter::remove(lineEdit);
        return true;
    }

    case WidgetKind::ProgressBar: {
        auto* progress = static_cast<QProgressBar*>(widget);
        if (!QtMaterialProgressBarAdapter::isApplied(progress)) {
            return false;
        }
        QtMaterialProgressBarAdapter::remove(progress);
        return true;
    }

    case WidgetKind::Unsupported:
    default:
        return false;
    }
}

bool QtMaterialNativeAdapter::isApplied(
    const QWidget* widget)
{
    if (!widget) {
        return false;
    }

    switch (kind(widget)) {
    case WidgetKind::PushButton:
        return QtMaterialButtonAdapter::isApplied(
            static_cast<const QPushButton*>(widget));
    case WidgetKind::ToolButton:
        return QtMaterialToolButtonAdapter::isApplied(
            static_cast<const QToolButton*>(widget));
    case WidgetKind::CheckBox:
        return QtMaterialSelectionAdapter::isApplied(
            static_cast<const QCheckBox*>(widget));
    case WidgetKind::RadioButton:
        return QtMaterialSelectionAdapter::isApplied(
            static_cast<const QRadioButton*>(widget));
    case WidgetKind::Slider:
        return QtMaterialSliderAdapter::isApplied(
            static_cast<const QSlider*>(widget));
    case WidgetKind::ComboBox:
        return QtMaterialComboBoxAdapter::isApplied(
            static_cast<const QComboBox*>(widget));
    case WidgetKind::LineEdit:
        return QtMaterialLineEditAdapter::isApplied(
            static_cast<const QLineEdit*>(widget));
    case WidgetKind::ProgressBar:
        return QtMaterialProgressBarAdapter::isApplied(
            static_cast<const QProgressBar*>(widget));
    case WidgetKind::Unsupported:
    default:
        return false;
    }
}

void QtMaterialNativeAdapter::setOptOut(
    QWidget* widget,
    bool excluded)
{
    if (!widget) {
        return;
    }

    switch (kind(widget)) {
    case WidgetKind::PushButton:
        QtMaterialButtonAdapter::setOptOut(
            static_cast<QPushButton*>(widget),
            excluded);
        return;
    case WidgetKind::ToolButton:
        QtMaterialToolButtonAdapter::setOptOut(
            static_cast<QToolButton*>(widget),
            excluded);
        return;
    case WidgetKind::CheckBox:
        QtMaterialSelectionAdapter::setOptOut(
            static_cast<QCheckBox*>(widget),
            excluded);
        return;
    case WidgetKind::RadioButton:
        QtMaterialSelectionAdapter::setOptOut(
            static_cast<QRadioButton*>(widget),
            excluded);
        return;
    case WidgetKind::Slider:
        QtMaterialSliderAdapter::setOptOut(
            static_cast<QSlider*>(widget),
            excluded);
        return;
    case WidgetKind::ComboBox:
        QtMaterialComboBoxAdapter::setOptOut(
            static_cast<QComboBox*>(widget),
            excluded);
        return;
    case WidgetKind::LineEdit:
        QtMaterialLineEditAdapter::setOptOut(
            static_cast<QLineEdit*>(widget),
            excluded);
        return;
    case WidgetKind::ProgressBar:
        QtMaterialProgressBarAdapter::setOptOut(
            static_cast<QProgressBar*>(widget),
            excluded);
        return;
    case WidgetKind::Unsupported:
    default:
        return;
    }
}

bool QtMaterialNativeAdapter::isOptedOut(
    const QWidget* widget)
{
    if (!widget) {
        return false;
    }

    switch (kind(widget)) {
    case WidgetKind::PushButton:
        return QtMaterialButtonAdapter::isOptedOut(
            static_cast<const QPushButton*>(widget));
    case WidgetKind::ToolButton:
        return QtMaterialToolButtonAdapter::isOptedOut(
            static_cast<const QToolButton*>(widget));
    case WidgetKind::CheckBox:
        return QtMaterialSelectionAdapter::isOptedOut(
            static_cast<const QCheckBox*>(widget));
    case WidgetKind::RadioButton:
        return QtMaterialSelectionAdapter::isOptedOut(
            static_cast<const QRadioButton*>(widget));
    case WidgetKind::Slider:
        return QtMaterialSliderAdapter::isOptedOut(
            static_cast<const QSlider*>(widget));
    case WidgetKind::ComboBox:
        return QtMaterialComboBoxAdapter::isOptedOut(
            static_cast<const QComboBox*>(widget));
    case WidgetKind::LineEdit:
        return QtMaterialLineEditAdapter::isOptedOut(
            static_cast<const QLineEdit*>(widget));
    case WidgetKind::ProgressBar:
        return QtMaterialProgressBarAdapter::isOptedOut(
            static_cast<const QProgressBar*>(widget));
    case WidgetKind::Unsupported:
    default:
        return false;
    }
}

int QtMaterialNativeAdapter::applyToDescendants(
    QWidget* root,
    const Options& options)
{
    if (!root || isInsideFirstClassMaterialWidget(root)) {
        return 0;
    }

    if (kind(root) != WidgetKind::Unsupported) {
        // Supported controls are intentional traversal barriers, including
        // opt-out controls. Their implementation children stay untouched.
        return apply(root, options) ? 1 : 0;
    }

    int count = 0;
    const auto children = directChildWidgets(root);
    for (QWidget* child : children) {
        count += applyToDescendants(child, options);
    }
    return count;
}

int QtMaterialNativeAdapter::removeFromDescendants(
    QWidget* root)
{
    if (!root || isInsideFirstClassMaterialWidget(root)) {
        return 0;
    }

    if (kind(root) != WidgetKind::Unsupported) {
        return remove(root) ? 1 : 0;
    }

    int count = 0;
    const auto children = directChildWidgets(root);
    for (QWidget* child : children) {
        count += removeFromDescendants(child);
    }
    return count;
}

} // namespace QtMaterial
