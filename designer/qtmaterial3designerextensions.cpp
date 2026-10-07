#include "qtmaterial3designerextensions.h"

#include <algorithm>
#include <optional>

#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMetaEnum>
#include <QMetaProperty>
#include <QPlainTextEdit>
#include <QSet>
#include <QSpinBox>
#include <QTabWidget>
#include <QVariant>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

#include <QtDesigner/QDesignerFormEditorInterface>
#include <QtDesigner/QDesignerFormWindowCursorInterface>
#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerTaskMenuExtension>
#include <QtDesigner/QExtensionFactory>
#include <QtDesigner/QExtensionManager>

#include "qtmaterial/theme/qtmaterialthememanager.h"
#include "qtmaterial/theme/qtmaterialthemeoptions.h"
#include "qtmaterial/widgets/layouts/qtmaterialadaptiveshell.h"
#include "qtmaterial/widgets/navigation/qtmaterialtabs.h"

namespace QtMaterial3Designer {
namespace {

constexpr const char* kRegisteredProperty = "_qtm3_designer_extensions_registered";

const QSet<QString>& curatedProperties()
{
    static const QSet<QString> properties = {
        QStringLiteral("text"),
        QStringLiteral("title"),
        QStringLiteral("titleText"),
        QStringLiteral("bodyText"),
        QStringLiteral("label"),
        QStringLiteral("labelText"),
        QStringLiteral("placeholderText"),
        QStringLiteral("helperText"),
        QStringLiteral("errorText"),
        QStringLiteral("prefixText"),
        QStringLiteral("suffixText"),
        QStringLiteral("required"),
        QStringLiteral("clearable"),
        QStringLiteral("clearButtonVisible"),
        QStringLiteral("clearButtonEnabled"),
        QStringLiteral("readOnly"),
        QStringLiteral("maxLength"),
        QStringLiteral("characterCounterEnabled"),
        QStringLiteral("minimum"),
        QStringLiteral("maximum"),
        QStringLiteral("value"),
        QStringLiteral("lowerValue"),
        QStringLiteral("upperValue"),
        QStringLiteral("page"),
        QStringLiteral("pageSize"),
        QStringLiteral("totalCount"),
        QStringLiteral("orientation"),
        QStringLiteral("variant"),
        QStringLiteral("density"),
        QStringLiteral("alignment"),
        QStringLiteral("scrollable"),
        QStringLiteral("indicatorHeight"),
        QStringLiteral("overflowMode"),
        QStringLiteral("wrapNavigation"),
        QStringLiteral("lazyLoading"),
        QStringLiteral("interactive"),
        QStringLiteral("decorative"),
        QStringLiteral("thickness"),
        QStringLiteral("active"),
        QStringLiteral("indicatorSize"),
        QStringLiteral("expressive"),
        QStringLiteral("expressiveSize"),
        QStringLiteral("exclusive"),
        QStringLiteral("currentIndex"),
        QStringLiteral("spacing"),
        QStringLiteral("expanded"),
        QStringLiteral("labelsVisible"),
        QStringLiteral("buttonLabels"),
        QStringLiteral("destinationLabels"),
        QStringLiteral("itemLabels")
    };
    return properties;
}

int propertyTypeId(const QMetaProperty& property)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return property.metaType().id();
#else
    return property.userType();
#endif
}

bool isQtMaterialWidget(const QWidget* widget)
{
    if (!widget) {
        return false;
    }

    for (const QMetaObject* meta = widget->metaObject(); meta; meta = meta->superClass()) {
        const QString name = QString::fromLatin1(meta->className());
        if (name.startsWith(QStringLiteral("QtMaterial"))
            || name.startsWith(QStringLiteral("QtMaterial::"))) {
            return true;
        }
    }
    return false;
}

struct EditorBinding
{
    QByteArray name;
    QMetaProperty property;
    QWidget* editor = nullptr;
    QVariant original;
};

QVariant editorValue(const EditorBinding& binding)
{
    if (auto* check = qobject_cast<QCheckBox*>(binding.editor)) {
        return check->isChecked();
    }
    if (auto* spin = qobject_cast<QSpinBox*>(binding.editor)) {
        return spin->value();
    }
    if (auto* spin = qobject_cast<QDoubleSpinBox*>(binding.editor)) {
        return spin->value();
    }
    if (auto* line = qobject_cast<QLineEdit*>(binding.editor)) {
        return line->text();
    }
    if (auto* text = qobject_cast<QPlainTextEdit*>(binding.editor)) {
        return text->toPlainText()
            .split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    }
    if (auto* combo = qobject_cast<QComboBox*>(binding.editor)) {
        return combo->currentData();
    }
    return {};
}

QWidget* makeEditor(
    const QMetaProperty& property,
    const QVariant& value,
    QWidget* parent)
{
    if (property.isEnumType()) {
        auto* combo = new QComboBox(parent);
        const QMetaEnum metaEnum = property.enumerator();
        for (int index = 0; index < metaEnum.keyCount(); ++index) {
            combo->addItem(
                QString::fromLatin1(metaEnum.key(index)),
                metaEnum.value(index));
        }
        const int current = combo->findData(value.toInt());
        if (current >= 0) {
            combo->setCurrentIndex(current);
        }
        return combo;
    }

    switch (propertyTypeId(property)) {
    case QMetaType::Bool: {
        auto* check = new QCheckBox(parent);
        check->setChecked(value.toBool());
        return check;
    }
    case QMetaType::Int: {
        auto* spin = new QSpinBox(parent);
        spin->setRange(-1000000, 1000000);
        spin->setValue(value.toInt());
        return spin;
    }
    case QMetaType::Double: {
        auto* spin = new QDoubleSpinBox(parent);
        spin->setRange(-1000000.0, 1000000.0);
        spin->setDecimals(4);
        spin->setValue(value.toDouble());
        return spin;
    }
    case QMetaType::QString: {
        auto* line = new QLineEdit(value.toString(), parent);
        return line;
    }
    case QMetaType::QStringList: {
        auto* text = new QPlainTextEdit(parent);
        text->setPlainText(value.toStringList().join(QLatin1Char('\n')));
        text->setPlaceholderText(
            QObject::tr("One item per line; saved as a QStringList in the .ui file."));
        text->setMinimumHeight(96);
        return text;
    }
    default:
        return nullptr;
    }
}

class MaterialPropertyDialog final : public QDialog
{
public:
    MaterialPropertyDialog(
        QWidget* target,
        QDesignerFormEditorInterface* core,
        QWidget* parent = nullptr)
        : QDialog(parent)
        , target_(target)
        , core_(core)
    {
        setWindowTitle(tr("Edit Material 3 Properties"));
        resize(460, 360);

        auto* layout = new QVBoxLayout(this);
        auto* form = new QFormLayout;
        layout->addLayout(form);

        const QStringList names = editablePropertyNames(target_);
        for (const QString& name : names) {
            const int index = target_->metaObject()->indexOfProperty(name.toLatin1().constData());
            if (index < 0) {
                continue;
            }

            const QMetaProperty property = target_->metaObject()->property(index);
            const QVariant value = property.read(target_);
            QWidget* editor = makeEditor(property, value, this);
            if (!editor) {
                continue;
            }

            form->addRow(name, editor);
            bindings_.append(
                {name.toLatin1(), property, editor, value});
        }

        auto* buttons = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
            this);
        layout->addWidget(buttons);
        connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
            applyChanges();
            accept();
        });
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    }

private:
    void applyChanges()
    {
        QDesignerFormWindowInterface* formWindow =
            QDesignerFormWindowInterface::findFormWindow(target_);
        QDesignerFormWindowCursorInterface* cursor =
            formWindow ? formWindow->cursor() : nullptr;

        for (const EditorBinding& binding : bindings_) {
            const QVariant value = editorValue(binding);
            if (!value.isValid() || value == binding.original) {
                continue;
            }

            const QString name = QString::fromLatin1(binding.name);
            if (cursor) {
                cursor->setWidgetProperty(target_, name, value);
            } else {
                target_->setProperty(binding.name.constData(), value);
                if (formWindow) {
                    formWindow->setDirty(true);
                }
            }
        }

        target_->updateGeometry();
        target_->update();
    }

    QWidget* target_ = nullptr;
    QDesignerFormEditorInterface* core_ = nullptr;
    QVector<EditorBinding> bindings_;
};

class MaterialTaskMenu final
    : public QObject
    , public QDesignerTaskMenuExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerTaskMenuExtension)

public:
    MaterialTaskMenu(
        QWidget* widget,
        QDesignerFormEditorInterface* core,
        QObject* parent)
        : QObject(parent)
        , widget_(widget)
        , core_(core)
    {
        editAction_ = new QAction(tr("Edit Material 3 properties..."), this);
        connect(editAction_, &QAction::triggered, this, [this]() {
            MaterialPropertyDialog dialog(widget_, core_, widget_);
            dialog.exec();
        });

        lightAction_ = new QAction(tr("Preview Light"), this);
        darkAction_ = new QAction(tr("Preview Dark"), this);
        expressiveAction_ = new QAction(tr("Preview Expressive"), this);
        restoreAction_ = new QAction(tr("Restore theme preview"), this);

        connect(lightAction_, &QAction::triggered, this, []() {
            applyPreviewMode(PreviewMode::Light);
        });
        connect(darkAction_, &QAction::triggered, this, []() {
            applyPreviewMode(PreviewMode::Dark);
        });
        connect(expressiveAction_, &QAction::triggered, this, []() {
            applyPreviewMode(PreviewMode::Expressive);
        });
        connect(restoreAction_, &QAction::triggered, this, []() {
            restorePreviewMode();
        });
    }

    QList<QAction*> taskActions() const override
    {
        return {
            editAction_,
            lightAction_,
            darkAction_,
            expressiveAction_,
            restoreAction_
        };
    }

    QAction* preferredEditAction() const override
    {
        return editAction_;
    }

private:
    QWidget* widget_ = nullptr;
    QDesignerFormEditorInterface* core_ = nullptr;
    QAction* editAction_ = nullptr;
    QAction* lightAction_ = nullptr;
    QAction* darkAction_ = nullptr;
    QAction* expressiveAction_ = nullptr;
    QAction* restoreAction_ = nullptr;
};

class TaskMenuFactory final : public QExtensionFactory
{
    Q_OBJECT

public:
    TaskMenuFactory(
        QDesignerFormEditorInterface* core,
        QExtensionManager* parent)
        : QExtensionFactory(parent)
        , core_(core)
    {
    }

protected:
    QObject* createExtension(
        QObject* object,
        const QString& iid,
        QObject* parent) const override
    {
        if (iid != Q_TYPEID(QDesignerTaskMenuExtension)) {
            return nullptr;
        }
        auto* widget = qobject_cast<QWidget*>(object);
        if (!isQtMaterialWidget(widget)) {
            return nullptr;
        }
        return new MaterialTaskMenu(widget, core_, parent);
    }

private:
    QDesignerFormEditorInterface* core_ = nullptr;
};

class ContainerFactory final : public QExtensionFactory
{
    Q_OBJECT

public:
    explicit ContainerFactory(QExtensionManager* parent)
        : QExtensionFactory(parent)
    {
    }

protected:
    QObject* createExtension(
        QObject* object,
        const QString& iid,
        QObject* parent) const override
    {
        if (iid != Q_TYPEID(QDesignerContainerExtension)) {
            return nullptr;
        }
        if (auto* tabs = qobject_cast<QtMaterial::QtMaterialTabs*>(object)) {
            return new TabsContainerExtension(tabs, parent);
        }
        if (auto* shell = qobject_cast<QtMaterial::QtMaterialAdaptiveShell*>(object)) {
            return new AdaptiveShellContainerExtension(shell, parent);
        }
        return nullptr;
    }
};

std::optional<QtMaterial::ThemeOptions>& savedPreviewOptions()
{
    static std::optional<QtMaterial::ThemeOptions> options;
    return options;
}

} // namespace

QStringList editablePropertyNames(const QWidget* widget)
{
    QStringList result;
    if (!widget) {
        return result;
    }

    const QMetaObject* meta = widget->metaObject();
    for (int index = 0; index < meta->propertyCount(); ++index) {
        const QMetaProperty property = meta->property(index);
        const QString name = QString::fromLatin1(property.name());
        bool designable = false;
        bool stored = false;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        designable = property.isDesignable();
        stored = property.isStored();
#else
        designable = property.isDesignable(widget);
        stored = property.isStored(widget);
#endif
        if (!curatedProperties().contains(name)
            || !property.isReadable()
            || !property.isWritable()
            || !designable
            || !stored) {
            continue;
        }

        if (property.isEnumType()) {
            result.append(name);
            continue;
        }

        const int type = propertyTypeId(property);
        if (type == QMetaType::Bool
            || type == QMetaType::Int
            || type == QMetaType::Double
            || type == QMetaType::QString
            || type == QMetaType::QStringList) {
            result.append(name);
        }
    }
    return result;
}

void applyPreviewMode(PreviewMode mode)
{
    auto& manager = QtMaterial::ThemeManager::instance();
    auto& saved = savedPreviewOptions();
    if (!saved.has_value()) {
        saved = manager.options();
    }

    QtMaterial::ThemeOptions options = manager.options();
    switch (mode) {
    case PreviewMode::Light:
        options.mode = QtMaterial::ThemeMode::Light;
        options.preference = QtMaterial::ThemePreference::Light;
        options.variant = QtMaterial::ThemeVariant::TonalSpot;
        break;
    case PreviewMode::Dark:
        options.mode = QtMaterial::ThemeMode::Dark;
        options.preference = QtMaterial::ThemePreference::Dark;
        options.variant = QtMaterial::ThemeVariant::TonalSpot;
        break;
    case PreviewMode::Expressive:
        options.mode = QtMaterial::ThemeMode::Light;
        options.preference = QtMaterial::ThemePreference::Light;
        options.variant = QtMaterial::ThemeVariant::Expressive;
        break;
    }
    manager.setThemeOptions(options);
}

void restorePreviewMode()
{
    auto& saved = savedPreviewOptions();
    if (!saved.has_value()) {
        return;
    }
    QtMaterial::ThemeManager::instance().setThemeOptions(*saved);
    saved.reset();
}

void registerExtensions(QDesignerFormEditorInterface* core)
{
    if (!core || !core->extensionManager()) {
        return;
    }

    QExtensionManager* manager = core->extensionManager();
    if (manager->property(kRegisteredProperty).toBool()) {
        return;
    }

    manager->registerExtensions(
        new TaskMenuFactory(core, manager),
        Q_TYPEID(QDesignerTaskMenuExtension));
    manager->registerExtensions(
        new ContainerFactory(manager),
        Q_TYPEID(QDesignerContainerExtension));
    manager->setProperty(kRegisteredProperty, true);
}

TabsContainerExtension::TabsContainerExtension(
    QtMaterial::QtMaterialTabs* tabs,
    QObject* parent)
    : QObject(parent)
    , tabs_(tabs)
{
}

int TabsContainerExtension::count() const
{
    return tabs_ ? tabs_->count() : 0;
}

QWidget* TabsContainerExtension::widget(int index) const
{
    return tabs_ ? tabs_->widget(index) : nullptr;
}

int TabsContainerExtension::currentIndex() const
{
    return tabs_ ? tabs_->currentIndex() : -1;
}

void TabsContainerExtension::setCurrentIndex(int index)
{
    if (tabs_ && index >= 0 && index < tabs_->count()) {
        tabs_->setCurrentIndex(index);
    }
}

void TabsContainerExtension::addWidget(QWidget* widget)
{
    insertWidget(count(), widget);
}

void TabsContainerExtension::insertWidget(int index, QWidget* widget)
{
    if (!tabs_ || !widget) {
        return;
    }
    const int bounded = qBound(0, index, tabs_->count());
    const QString label = widget->objectName().isEmpty()
        ? tr("Page %1").arg(bounded + 1)
        : widget->objectName();
    tabs_->insertTab(bounded, widget, label);
    tabs_->setCurrentIndex(bounded);
}

void TabsContainerExtension::remove(int index)
{
    if (!tabs_ || index < 0 || index >= tabs_->count()) {
        return;
    }
    tabs_->removeTab(index);
}

AdaptiveShellContainerExtension::AdaptiveShellContainerExtension(
    QtMaterial::QtMaterialAdaptiveShell* shell,
    QObject* parent)
    : QObject(parent)
    , shell_(shell)
{
}

QList<QWidget*> AdaptiveShellContainerExtension::pages() const
{
    QList<QWidget*> result;
    if (!shell_) {
        return result;
    }
    if (shell_->contentWidget()) {
        result.append(shell_->contentWidget());
    }
    if (shell_->supportingWidget()) {
        result.append(shell_->supportingWidget());
    }
    return result;
}

int AdaptiveShellContainerExtension::count() const
{
    return pages().size();
}

QWidget* AdaptiveShellContainerExtension::widget(int index) const
{
    const auto currentPages = pages();
    return index >= 0 && index < currentPages.size()
        ? currentPages.at(index)
        : nullptr;
}

int AdaptiveShellContainerExtension::currentIndex() const
{
    return count() == 0
        ? -1
        : qBound(0, currentIndex_, count() - 1);
}

void AdaptiveShellContainerExtension::setCurrentIndex(int index)
{
    if (index >= 0 && index < count()) {
        currentIndex_ = index;
    }
}

void AdaptiveShellContainerExtension::addWidget(QWidget* widget)
{
    insertWidget(count(), widget);
}

void AdaptiveShellContainerExtension::insertWidget(int index, QWidget* widget)
{
    if (!shell_ || !widget) {
        return;
    }

    if (!shell_->contentWidget()) {
        shell_->setContentWidget(widget);
        currentIndex_ = 0;
        return;
    }
    if (!shell_->supportingWidget()) {
        if (index <= 0) {
            QWidget* previousContent = shell_->contentWidget();
            shell_->setContentWidget(nullptr);
            shell_->setSupportingWidget(previousContent);
            shell_->setContentWidget(widget);
            currentIndex_ = 0;
        } else {
            shell_->setSupportingWidget(widget);
            currentIndex_ = 1;
        }
    }
}

void AdaptiveShellContainerExtension::remove(int index)
{
    if (!shell_) {
        return;
    }

    const auto currentPages = pages();
    if (index < 0 || index >= currentPages.size()) {
        return;
    }

    QWidget* page = currentPages.at(index);
    if (page == shell_->contentWidget()) {
        shell_->setContentWidget(nullptr);
    } else if (page == shell_->supportingWidget()) {
        shell_->setSupportingWidget(nullptr);
    }
    currentIndex_ = qMax(0, qMin(currentIndex_, count() - 1));
}

} // namespace QtMaterial3Designer

#include "qtmaterial3designerextensions.moc"
