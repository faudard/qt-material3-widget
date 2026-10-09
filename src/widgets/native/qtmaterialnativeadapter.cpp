#include "qtmaterial/widgets/native/qtmaterialnativeadapter.h"

#include <QApplication>
#include <QCheckBox>
#include <QDynamicPropertyChangeEvent>
#include <QEvent>
#include <QHash>
#include <QPointer>
#include <QSet>
#include <QStyle>
#include <QTimer>
#include <QVector>
#include <QComboBox>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QToolButton>
#include <QVariant>
#include <QWidget>

#include <algorithm>

#include "qtmaterial/widgets/native/qtmaterialbuttonadapter.h"
#include "qtmaterial/widgets/native/qtmaterialcomboboxadapter.h"
#include "qtmaterial/widgets/native/qtmateriallineeditadapter.h"
#include "qtmaterial/widgets/native/qtmaterialprogressbaradapter.h"
#include "qtmaterial/widgets/native/qtmaterialselectionadapter.h"
#include "qtmaterial/widgets/native/qtmaterialslideradapter.h"
#include "qtmaterial/widgets/native/qtmaterialtoolbuttonadapter.h"

namespace QtMaterial {
namespace {

constexpr char kAdaptProperty[] = "qtm3MaterialAdapt";
constexpr char kVariantProperty[] = "qtm3MaterialVariant";
constexpr char kDensityProperty[] = "qtm3MaterialDensity";
constexpr char kTextFieldVariantProperty[] = "qtm3MaterialTextFieldVariant";

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

Density densityFromDeclared(
    const QVariant& value,
    Density fallback)
{
    if (!value.isValid()) {
        return fallback;
    }

    const QString name =
        value.toString().trimmed().toLower();
    if (name == QStringLiteral("compact")) {
        return Density::Compact;
    }
    if (name == QStringLiteral("comfortable")) {
        return Density::Comfortable;
    }
    if (name == QStringLiteral("default")) {
        return Density::Default;
    }
    return fallback;
}

ButtonVariant buttonVariantFromDeclared(
    const QVariant& value,
    ButtonVariant fallback)
{
    if (!value.isValid()) {
        return fallback;
    }

    const QString name =
        value.toString().trimmed().toLower();
    if (name == QStringLiteral("text")) {
        return ButtonVariant::Text;
    }
    if (name == QStringLiteral("filled")) {
        return ButtonVariant::Filled;
    }
    if (name == QStringLiteral("filled-tonal")
        || name == QStringLiteral("filledtonal")
        || name == QStringLiteral("tonal")) {
        return ButtonVariant::FilledTonal;
    }
    if (name == QStringLiteral("outlined")) {
        return ButtonVariant::Outlined;
    }
    if (name == QStringLiteral("elevated")) {
        return ButtonVariant::Elevated;
    }
    return fallback;
}

QtMaterialNativeAdapter::TextFieldVariant
textFieldVariantFromDeclared(
    const QVariant& value,
    QtMaterialNativeAdapter::TextFieldVariant fallback)
{
    if (!value.isValid()) {
        return fallback;
    }

    const QString name =
        value.toString().trimmed().toLower();
    if (name == QStringLiteral("filled")) {
        return QtMaterialNativeAdapter::TextFieldVariant::Filled;
    }
    if (name == QStringLiteral("outlined")) {
        return QtMaterialNativeAdapter::TextFieldVariant::Outlined;
    }
    return fallback;
}

QtMaterialNativeAdapter::Options declaredOptions(
    const QWidget* widget,
    const QtMaterialNativeAdapter::Options& fallback)
{
    if (!widget) {
        return fallback;
    }

    return QtMaterialNativeAdapter::Options(
        densityFromDeclared(
            widget->property(kDensityProperty),
            fallback.density),
        buttonVariantFromDeclared(
            widget->property(kVariantProperty),
            fallback.buttonVariant),
        textFieldVariantFromDeclared(
            widget->property(kTextFieldVariantProperty),
            fallback.textFieldVariant));
}


bool sameOptions(
    const QtMaterialNativeAdapter::Options& left,
    const QtMaterialNativeAdapter::Options& right)
{
    return left.density == right.density
        && left.buttonVariant == right.buttonVariant
        && left.textFieldVariant == right.textFieldVariant;
}

// A watcher is not allowed to take ownership of implementation details
// exposed as QWidget children of a native control.
bool hasNativeControlAncestor(const QWidget* widget)
{
    for (const QWidget* parent = widget ? widget->parentWidget() : nullptr;
         parent;
         parent = parent->parentWidget()) {
        if (QtMaterialNativeAdapter::isSupported(parent)
            || hasMaterialClassName(parent)) {
            return true;
        }
    }
    return false;
}

class NativeRuntimeWatcher final : public QObject
{
public:
    explicit NativeRuntimeWatcher(QObject* parent)
        : QObject(parent)
    {
    }

    bool watch(
        QWidget* root,
        QtMaterialNativeAdapter::WatchPolicy policy,
        const QtMaterialNativeAdapter::Options& options)
    {
        if (!root || isInsideFirstClassMaterialWidget(root)
            || hasNativeControlAncestor(root)) {
            return false;
        }

        auto it = m_roots.find(root);
        if (it != m_roots.end()
            && it->policy == policy
            && sameOptions(it->options, options)) {
            return true;
        }

        m_roots.insert(root, RootSpec{policy, options});
        reconcile();
        return true;
    }

    bool unwatch(QWidget* root)
    {
        if (!root || !m_roots.remove(root)) {
            return false;
        }
        reconcile();
        return true;
    }

    bool isWatched(const QWidget* root) const
    {
        return root && m_roots.contains(const_cast<QWidget*>(root));
    }

protected:
    bool eventFilter(QObject* object, QEvent* event) override
    {
        switch (event->type()) {
        case QEvent::ChildAdded:
        case QEvent::ChildRemoved:
        case QEvent::ParentChange:
        case QEvent::StyleChange:
            scheduleReconcile();
            break;
        case QEvent::DynamicPropertyChange: {
            const QByteArray propertyName =
                static_cast<QDynamicPropertyChangeEvent*>(event)
                    ->propertyName();
            if (propertyName == kAdaptProperty
                || propertyName == "qtm3MaterialOptOut"
                || propertyName == kVariantProperty
                || propertyName == kDensityProperty
                || propertyName == kTextFieldVariantProperty) {
                scheduleReconcile();
            }
            break;
        }
        default:
            break;
        }
        // Never consume Qt's events: normal signals, focus, input and style
        // notification behavior must be preserved.
        return QObject::eventFilter(object, event);
    }

private:
    struct RootSpec
    {
        QtMaterialNativeAdapter::WatchPolicy policy;
        QtMaterialNativeAdapter::Options options;
    };

    struct Desired
    {
        QtMaterialNativeAdapter::Options options;
    };

    struct Managed
    {
        QtMaterialNativeAdapter::Options options;
        QPointer<QStyle> installedStyle;
        QVariant originalVariant;
        QVariant originalDensity;
        QVariant originalTextFieldVariant;
    };

    void scheduleReconcile()
    {
        if (m_reconciling || m_pending) {
            return;
        }
        m_pending = true;
        // ChildAdded is emitted before the derived QWidget constructor has
        // completed. Defer discovery rather than inspecting a half-built
        // control; multiple events in one turn are coalesced.
        QTimer::singleShot(0, this, [this]() {
            m_pending = false;
            reconcile();
        });
    }

    static int depth(const QWidget* widget)
    {
        int result = 0;
        for (auto* current = widget; current;
             current = current->parentWidget()) {
            ++result;
        }
        return result;
    }

    void collect(
        QWidget* widget,
        const RootSpec& spec,
        QSet<QWidget*>& observed,
        QHash<QWidget*, Desired>& desired)
    {
        if (!widget || isInsideFirstClassMaterialWidget(widget)) {
            return;
        }

        observed.insert(widget);
        if (QtMaterialNativeAdapter::isSupported(widget)) {
            const bool eligible =
                !QtMaterialNativeAdapter::isOptedOut(widget)
                && (spec.policy
                        == QtMaterialNativeAdapter::WatchPolicy::AllSupported
                    || QtMaterialNativeAdapter::isDeclared(widget));
            if (eligible) {
                const auto options =
                    spec.policy
                        == QtMaterialNativeAdapter::WatchPolicy::DeclaredOnly
                    ? declaredOptions(widget, spec.options)
                    : spec.options;
                desired.insert(widget, Desired{options});
            } else {
                // A deeper watched root can override a shallower one.
                desired.remove(widget);
            }
            // Native implementations (combo line edits, clear buttons, popup
            // widgets) are never descended into, including on opt-out.
            return;
        }

        const auto children = directChildWidgets(widget);
        for (QWidget* child : children) {
            collect(child, spec, observed, desired);
        }
    }

    void observe(QWidget* widget)
    {
        if (m_observed.contains(widget)) {
            return;
        }
        m_observed.insert(widget);
        widget->installEventFilter(this);
        m_destroyConnections.insert(
            widget,
            QObject::connect(
                widget, &QObject::destroyed, this,
                [this, widget]() {
                    m_observed.remove(widget);
                    m_destroyConnections.remove(widget);
                    m_managed.remove(widget);
                    m_roots.remove(widget);
                    scheduleReconcile();
                }));
    }

    static void restoreProperties(QWidget* widget, const Managed& record)
    {
        widget->setProperty(kVariantProperty, record.originalVariant);
        widget->setProperty(kDensityProperty, record.originalDensity);
        widget->setProperty(
            kTextFieldVariantProperty, record.originalTextFieldVariant);
    }

    void release(QWidget* widget, const Managed& record)
    {
        if (!widget) {
            return;
        }
        // Do not overwrite an application-installed replacement QStyle.
        // The specialized adapter already checks its own style ownership.
        if (QtMaterialNativeAdapter::isApplied(widget)) {
            QtMaterialNativeAdapter::remove(widget);
        }
        restoreProperties(widget, record);
    }

    void reconcile()
    {
        if (m_reconciling) {
            return;
        }
        m_reconciling = true;

        QSet<QWidget*> observed;
        QHash<QWidget*, Desired> desired;

        QVector<QWidget*> roots = m_roots.keys().toVector();
        // Nearest watch root wins when watches overlap.
        std::sort(
            roots.begin(), roots.end(),
            [](const QWidget* left, const QWidget* right) {
                return depth(left) < depth(right);
            });
        for (QWidget* root : roots) {
            collect(root, m_roots.value(root), observed, desired);
        }

        // First release no-longer-eligible or externally restyled widgets.
        for (auto it = m_managed.begin(); it != m_managed.end();) {
            QWidget* widget = it.key();
            const auto wanted = desired.constFind(widget);
            const bool noLongerManaged = wanted == desired.cend();
            const bool styleReplaced =
                !noLongerManaged
                && widget->style() != it->installedStyle.data();
            const bool optionsChanged =
                !noLongerManaged
                && !sameOptions(wanted->options, it->options);
            const bool externallyRemoved =
                !noLongerManaged
                && !QtMaterialNativeAdapter::isApplied(widget);

            if (noLongerManaged || styleReplaced || optionsChanged
                || externallyRemoved) {
                const Managed previous = it.value();
                it = m_managed.erase(it);
                // Only undo changes made by this controller.
                release(widget, previous);
            } else {
                ++it;
            }
        }

        // The specialized adapters may create QObject implementation
        // children and emit StyleChange. Do not react recursively.
        for (auto it = desired.cbegin(); it != desired.cend(); ++it) {
            QWidget* widget = it.key();
            if (m_managed.contains(widget)
                || QtMaterialNativeAdapter::isApplied(widget)) {
                // Pre-existing manual adaptations are never owned/removed.
                continue;
            }

            Managed record{
                it->options,
                QPointer<QStyle>(),
                widget->property(kVariantProperty),
                widget->property(kDensityProperty),
                widget->property(kTextFieldVariantProperty)};
            if (QtMaterialNativeAdapter::apply(widget, it->options)) {
                record.installedStyle = widget->style();
                m_managed.insert(widget, record);
            }
        }

        for (QWidget* widget : observed) {
            observe(widget);
        }
        const auto previous = m_observed;
        for (QWidget* widget : previous) {
            if (!observed.contains(widget)) {
                widget->removeEventFilter(this);
                QObject::disconnect(m_destroyConnections.take(widget));
                m_observed.remove(widget);
            }
        }

        m_reconciling = false;
    }

    QHash<QWidget*, RootSpec> m_roots;
    QSet<QWidget*> m_observed;
    QHash<QWidget*, QMetaObject::Connection> m_destroyConnections;
    QHash<QWidget*, Managed> m_managed;
    bool m_reconciling = false;
    bool m_pending = false;
};

NativeRuntimeWatcher* nativeRuntimeWatcher(bool create)
{
    static QPointer<NativeRuntimeWatcher> watcher;
    if (!watcher && create && qApp) {
        watcher = new NativeRuntimeWatcher(qApp);
    }
    return watcher.data();
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

bool QtMaterialNativeAdapter::applyDeclared(
    QWidget* widget,
    const Options& fallback)
{
    if (!isDeclared(widget)) {
        return false;
    }

    return apply(
        widget,
        declaredOptions(widget, fallback));
}

bool QtMaterialNativeAdapter::isDeclared(
    const QWidget* widget)
{
    return widget
        && isSupported(widget)
        && widget->property(kAdaptProperty).toBool();
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

int QtMaterialNativeAdapter::applyDeclaredToDescendants(
    QWidget* root,
    const Options& fallback)
{
    if (!root || isInsideFirstClassMaterialWidget(root)) {
        return 0;
    }

    if (kind(root) != WidgetKind::Unsupported) {
        // A supported control is always a traversal barrier, even when it is
        // not declared. Native implementation children remain untouched.
        return applyDeclared(root, fallback) ? 1 : 0;
    }

    int count = 0;
    const auto children = directChildWidgets(root);
    for (QWidget* child : children) {
        count += applyDeclaredToDescendants(
            child,
            fallback);
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

bool QtMaterialNativeAdapter::watch(
    QWidget* root,
    WatchPolicy policy,
    const Options& options)
{
    NativeRuntimeWatcher* watcher = nativeRuntimeWatcher(true);
    return watcher && watcher->watch(root, policy, options);
}

bool QtMaterialNativeAdapter::unwatch(QWidget* root)
{
    NativeRuntimeWatcher* watcher = nativeRuntimeWatcher(false);
    return watcher && watcher->unwatch(root);
}

bool QtMaterialNativeAdapter::isWatched(const QWidget* root)
{
    NativeRuntimeWatcher* watcher = nativeRuntimeWatcher(false);
    return watcher && watcher->isWatched(root);
}

const char*
QtMaterialNativeAdapter::adaptPropertyName() noexcept
{
    return kAdaptProperty;
}

} // namespace QtMaterial
