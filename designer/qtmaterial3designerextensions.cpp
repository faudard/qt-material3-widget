#include "qtmaterial3designerextensions.h"

#include <algorithm>
#include <optional>

#include <QAction>
#include <QCheckBox>
#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMetaEnum>
#include <QMetaProperty>
#include <QMetaType>
#include <QPixmap>
#include <QPointer>
#include <QPushButton>
#include <QScrollArea>
#include <QSet>
#include <QSizePolicy>
#include <QSpinBox>
#include <QTimeEdit>
#include <QToolButton>
#include <QVariant>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

#include <QtDesigner/QDesignerDynamicPropertySheetExtension>
#include <QtDesigner/QDesignerPropertySheetExtension>
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
#include "qtmaterial/widgets/native/qtmaterialnativeadapter.h"

namespace QtMaterial3Designer {
namespace {

constexpr const char* kRegisteredProperty = "_qtm3_designer_extensions_registered";
constexpr const char* kResetRequestedProperty = "_qtm3_designer_reset_requested";

struct ThemePresetDescriptor
{
    QString id;
    QString displayName;
    QColor sourceColor;
    QtMaterial::ThemeMode mode = QtMaterial::ThemeMode::Light;
    QtMaterial::ContrastMode contrast = QtMaterial::ContrastMode::Standard;
    QtMaterial::ThemeVariant variant = QtMaterial::ThemeVariant::TonalSpot;
};

QVector<ThemePresetDescriptor> themePresets()
{
    using namespace QtMaterial;
    return {
        {QStringLiteral("material-default-light"),
         QObject::tr("Material Default Light"),
         QColor(QStringLiteral("#6750A4")),
         ThemeMode::Light,
         ContrastMode::Standard,
         ThemeVariant::TonalSpot},
        {QStringLiteral("material-default-dark"),
         QObject::tr("Material Default Dark"),
         QColor(QStringLiteral("#6750A4")),
         ThemeMode::Dark,
         ContrastMode::Standard,
         ThemeVariant::TonalSpot},
        {QStringLiteral("blue-light"),
         QObject::tr("Blue Light"),
         QColor(QStringLiteral("#0B57D0")),
         ThemeMode::Light,
         ContrastMode::Standard,
         ThemeVariant::TonalSpot},
        {QStringLiteral("green-light"),
         QObject::tr("Green Light"),
         QColor(QStringLiteral("#146C2E")),
         ThemeMode::Light,
         ContrastMode::Standard,
         ThemeVariant::TonalSpot},
        {QStringLiteral("amber-dark"),
         QObject::tr("Amber Dark"),
         QColor(QStringLiteral("#B06000")),
         ThemeMode::Dark,
         ContrastMode::Standard,
         ThemeVariant::TonalSpot},
        {QStringLiteral("rose-expressive"),
         QObject::tr("Rose Expressive"),
         QColor(QStringLiteral("#A73E8C")),
         ThemeMode::Light,
         ContrastMode::Medium,
         ThemeVariant::Expressive}
    };
}

std::optional<QtMaterial::ThemeOptions>& savedPreviewOptions()
{
    static std::optional<QtMaterial::ThemeOptions> options;
    return options;
}

bool optionsForPreset(
    const QString& presetId,
    const QtMaterial::ThemeOptions& base,
    QtMaterial::ThemeOptions* outOptions)
{
    if (!outOptions) {
        return false;
    }

    for (const ThemePresetDescriptor& preset : themePresets()) {
        if (preset.id != presetId) {
            continue;
        }

        QtMaterial::ThemeOptions options = base;
        options.sourceColor = preset.sourceColor;
        options.mode = preset.mode;
        options.preference = preset.mode == QtMaterial::ThemeMode::Dark
            ? QtMaterial::ThemePreference::Dark
            : QtMaterial::ThemePreference::Light;
        options.contrast = preset.contrast;
        options.variant = preset.variant;
        *outOptions = options;
        return true;
    }
    return false;
}

int propertyTypeId(const QMetaProperty& property)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return property.metaType().id();
#else
    return property.userType();
#endif
}

bool isQtMaterialMetaObject(const QMetaObject* meta)
{
    if (!meta) {
        return false;
    }
    const QString name = QString::fromLatin1(meta->className());
    return name.startsWith(QStringLiteral("QtMaterial"));
}

bool isQtMaterialWidget(const QWidget* widget)
{
    if (!widget) {
        return false;
    }

    for (const QMetaObject* meta = widget->metaObject(); meta; meta = meta->superClass()) {
        if (isQtMaterialMetaObject(meta)) {
            return true;
        }
    }
    return false;
}

bool supportedPropertyType(const QMetaProperty& property)
{
    if (property.isEnumType()) {
        return true;
    }

    switch (propertyTypeId(property)) {
    case QMetaType::Bool:
    case QMetaType::Int:
    case QMetaType::UInt:
    case QMetaType::Double:
    case QMetaType::Float:
    case QMetaType::QString:
    case QMetaType::QStringList:
    case QMetaType::QColor:
    case QMetaType::QDate:
    case QMetaType::QTime:
    case QMetaType::QDateTime:
        return true;
    default:
        return false;
    }
}

QString propertyDisplayName(const QString& name)
{
    if (name == QStringLiteral("destinationLabels")) {
        return QObject::tr("Navigation destinations");
    }
    if (name == QStringLiteral("buttonLabels")) {
        return QObject::tr("Button group items");
    }
    if (name == QStringLiteral("itemLabels")) {
        return QObject::tr("Segmented list items");
    }
    if (name == QStringLiteral("activeColor")) {
        return QObject::tr("Active color / token");
    }
    if (name == QStringLiteral("trackColor")) {
        return QObject::tr("Track color / token");
    }
    if (name == QStringLiteral("color")) {
        return QObject::tr("Color / token");
    }
    return name;
}

class ColorEditor final : public QWidget
{
public:
    ColorEditor(const QColor& color, QWidget* parent = nullptr)
        : QWidget(parent)
        , color_(color)
    {
        auto* layout = new QHBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        valueLabel_ = new QLabel(this);
        chooseButton_ = new QPushButton(tr("Choose..."), this);
        layout->addWidget(valueLabel_, 1);
        layout->addWidget(chooseButton_);
        updateLabel();

        connect(chooseButton_, &QPushButton::clicked, this, [this]() {
            const QColor initial = color_.isValid() ? color_ : QColor(Qt::white);
            const QColor selected = QColorDialog::getColor(
                initial,
                this,
                tr("Choose Material color override"),
                QColorDialog::ShowAlphaChannel);
            if (!selected.isValid()) {
                return;
            }
            color_ = selected;
            updateLabel();
        });
    }

    QColor color() const
    {
        return color_;
    }

private:
    void updateLabel()
    {
        valueLabel_->setText(
            color_.isValid()
                ? color_.name(QColor::HexArgb)
                : tr("Theme token / default"));
    }

    QColor color_;
    QLabel* valueLabel_ = nullptr;
    QPushButton* chooseButton_ = nullptr;
};

class CollectionEditor final : public QWidget
{
public:
    CollectionEditor(
        const QStringList& values,
        const QString& itemNoun,
        QWidget* parent = nullptr)
        : QWidget(parent)
        , itemNoun_(itemNoun)
    {
        auto* outer = new QVBoxLayout(this);
        outer->setContentsMargins(0, 0, 0, 0);

        list_ = new QListWidget(this);
        list_->addItems(values);
        list_->setEditTriggers(
            QAbstractItemView::DoubleClicked
            | QAbstractItemView::EditKeyPressed
            | QAbstractItemView::SelectedClicked);
        list_->setMinimumHeight(120);
        outer->addWidget(list_);

        auto* actions = new QHBoxLayout;
        auto* add = new QPushButton(tr("Add"), this);
        auto* remove = new QPushButton(tr("Remove"), this);
        auto* up = new QToolButton(this);
        auto* down = new QToolButton(this);
        up->setText(tr("Up"));
        down->setText(tr("Down"));
        actions->addWidget(add);
        actions->addWidget(remove);
        actions->addStretch(1);
        actions->addWidget(up);
        actions->addWidget(down);
        outer->addLayout(actions);

        connect(add, &QPushButton::clicked, this, [this]() {
            bool ok = false;
            const QString value = QInputDialog::getText(
                this,
                tr("Add %1").arg(itemNoun_),
                tr("Label"),
                QLineEdit::Normal,
                QString(),
                &ok);
            if (ok && !value.trimmed().isEmpty()) {
                list_->addItem(value.trimmed());
                list_->setCurrentRow(list_->count() - 1);
            }
        });
        connect(remove, &QPushButton::clicked, this, [this]() {
            delete list_->takeItem(list_->currentRow());
        });
        connect(up, &QToolButton::clicked, this, [this]() {
            moveCurrent(-1);
        });
        connect(down, &QToolButton::clicked, this, [this]() {
            moveCurrent(1);
        });
    }

    QStringList values() const
    {
        QStringList result;
        result.reserve(list_->count());
        for (int row = 0; row < list_->count(); ++row) {
            result.append(list_->item(row)->text());
        }
        return result;
    }

private:
    void moveCurrent(int delta)
    {
        const int row = list_->currentRow();
        const int next = row + delta;
        if (row < 0 || next < 0 || next >= list_->count()) {
            return;
        }

        QListWidgetItem* item = list_->takeItem(row);
        list_->insertItem(next, item);
        list_->setCurrentRow(next);
    }

    QString itemNoun_;
    QListWidget* list_ = nullptr;
};

struct EditorBinding
{
    QByteArray name;
    QMetaProperty property;
    QWidget* editor = nullptr;
    QVariant original;
};

QVariant editorValue(const EditorBinding& binding)
{
    if (auto* color = dynamic_cast<ColorEditor*>(binding.editor)) {
        return color->color();
    }
    if (auto* collection = dynamic_cast<CollectionEditor*>(binding.editor)) {
        return collection->values();
    }

    switch (propertyTypeId(binding.property)) {
    case QMetaType::QDate:
        return qobject_cast<QDateEdit*>(binding.editor)->date();
    case QMetaType::QTime:
        return qobject_cast<QTimeEdit*>(binding.editor)->time();
    case QMetaType::QDateTime:
        return qobject_cast<QDateTimeEdit*>(binding.editor)->dateTime();
    default:
        break;
    }

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
    if (auto* combo = qobject_cast<QComboBox*>(binding.editor)) {
        return combo->currentData();
    }
    return {};
}

QWidget* makeEditor(
    const QString& propertyName,
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
    case QMetaType::Int:
    case QMetaType::UInt: {
        auto* spin = new QSpinBox(parent);
        spin->setRange(propertyTypeId(property) == QMetaType::UInt ? 0 : -1000000, 1000000);
        spin->setValue(value.toInt());
        return spin;
    }
    case QMetaType::Double:
    case QMetaType::Float: {
        auto* spin = new QDoubleSpinBox(parent);
        spin->setRange(-1000000.0, 1000000.0);
        spin->setDecimals(4);
        spin->setValue(value.toDouble());
        return spin;
    }
    case QMetaType::QString:
        return new QLineEdit(value.toString(), parent);
    case QMetaType::QStringList: {
        QString noun = QObject::tr("item");
        if (propertyName == QStringLiteral("destinationLabels")) {
            noun = QObject::tr("destination");
        } else if (propertyName == QStringLiteral("buttonLabels")) {
            noun = QObject::tr("button");
        } else if (propertyName == QStringLiteral("itemLabels")) {
            noun = QObject::tr("segment");
        }
        return new CollectionEditor(value.toStringList(), noun, parent);
    }
    case QMetaType::QColor:
        return new ColorEditor(value.value<QColor>(), parent);
    case QMetaType::QDate: {
        auto* edit = new QDateEdit(value.toDate(), parent);
        edit->setCalendarPopup(true);
        return edit;
    }
    case QMetaType::QTime:
        return new QTimeEdit(value.toTime(), parent);
    case QMetaType::QDateTime: {
        auto* edit = new QDateTimeEdit(value.toDateTime(), parent);
        edit->setCalendarPopup(true);
        return edit;
    }
    default:
        return nullptr;
    }
}

QWidget* makeEditorRow(
    const QMetaProperty& property,
    QWidget* editor,
    QWidget* parent)
{
    if (!property.isResettable()) {
        return editor;
    }

    auto* row = new QWidget(parent);
    auto* layout = new QHBoxLayout(row);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(editor, 1);

    auto* reset = new QToolButton(row);
    reset->setText(QObject::tr("Token/default"));
    reset->setCheckable(true);
    reset->setToolTip(QObject::tr(
        "Reset this property through Qt Designer so the widget resolves its Material token/default."));
    layout->addWidget(reset);

    QObject::connect(reset, &QToolButton::toggled, editor, [editor, reset](bool checked) {
        editor->setProperty(kResetRequestedProperty, checked);
        editor->setEnabled(!checked);
        reset->setText(checked ? QObject::tr("Keep reset") : QObject::tr("Token/default"));
    });
    return row;
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
        Q_UNUSED(core_);
        setWindowTitle(tr("Edit Material 3 Properties"));
        resize(640, 560);

        auto* layout = new QVBoxLayout(this);
        auto* hint = new QLabel(
            tr("Designer 3.0 edits persistence-safe Material properties. "
               "Collections, enums, dates and color/token overrides use specialized editors."),
            this);
        hint->setWordWrap(true);
        layout->addWidget(hint);

        auto* scroll = new QScrollArea(this);
        scroll->setWidgetResizable(true);
        auto* body = new QWidget(scroll);
        auto* form = new QFormLayout(body);
        scroll->setWidget(body);
        layout->addWidget(scroll, 1);

        const QStringList names = editablePropertyNames(target_);
        for (const QString& name : names) {
            const int index = target_->metaObject()->indexOfProperty(name.toLatin1().constData());
            if (index < 0) {
                continue;
            }

            const QMetaProperty property = target_->metaObject()->property(index);
            const QVariant value = property.read(target_);
            QWidget* editor = makeEditor(name, property, value, body);
            if (!editor) {
                continue;
            }

            QWidget* row = makeEditorRow(property, editor, body);
            form->addRow(propertyDisplayName(name), row);
            bindings_.append({name.toLatin1(), property, editor, value});
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
            const QString name = QString::fromLatin1(binding.name);
            if (binding.editor->property(kResetRequestedProperty).toBool()) {
                if (cursor) {
                    cursor->resetWidgetProperty(target_, name);
                } else {
                    resetPropertyToDefault(target_, name);
                    if (formWindow) {
                        formWindow->setDirty(true);
                    }
                }
                continue;
            }

            const QVariant value = editorValue(binding);
            if (!value.isValid() || value == binding.original) {
                continue;
            }

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

struct PreviewCapture
{
    QPixmap pixmap;
    QString detail;
};

PreviewCapture capturePreview(
    QWidget* target,
    PreviewWidth widthMode,
    qreal dpr,
    Qt::LayoutDirection direction)
{
    if (!target) {
        return {};
    }

    const QSize originalSize = target->size();
    const Qt::LayoutDirection originalDirection = target->layoutDirection();

    const int requestedWidth = previewLogicalWidth(widthMode);
    const int logicalWidth = requestedWidth > 0
        ? requestedWidth
        : qMax(240, qMax(target->width(), target->sizeHint().width()));
    const int logicalHeight = qBound(
        120,
        qMax(target->height(), target->sizeHint().height()),
        760);
    const QSize logicalSize(logicalWidth, logicalHeight);

    target->setLayoutDirection(direction);
    target->resize(logicalSize);
    target->ensurePolished();

    const QSize pixelSize(
        qMax(1, qRound(logicalSize.width() * dpr)),
        qMax(1, qRound(logicalSize.height() * dpr)));
    QPixmap pixmap(pixelSize);
    pixmap.setDevicePixelRatio(dpr);
    pixmap.fill(Qt::transparent);
    target->render(&pixmap);

    QString detail = QObject::tr("%1 x %2 logical px, DPR %3, %4")
        .arg(logicalSize.width())
        .arg(logicalSize.height())
        .arg(dpr, 0, 'f', 0)
        .arg(direction == Qt::RightToLeft ? QObject::tr("RTL") : QObject::tr("LTR"));
    if (auto* shell = qobject_cast<QtMaterial::QtMaterialAdaptiveShell*>(target)) {
        detail += QStringLiteral(" — ") + shell->accessibilitySummary();
    }

    target->resize(originalSize);
    target->setLayoutDirection(originalDirection);
    target->updateGeometry();
    target->update();

    return {pixmap, detail};
}

class MaterialPreviewDialog final : public QDialog
{
public:
    explicit MaterialPreviewDialog(QWidget* target, QWidget* parent = nullptr)
        : QDialog(parent)
        , target_(target)
    {
        setWindowTitle(tr("Material 3 Designer Preview"));
        resize(900, 680);

        auto* root = new QVBoxLayout(this);
        auto* controls = new QHBoxLayout;

        preset_ = new QComboBox(this);
        preset_->addItem(tr("Current theme"), QString());
        for (const ThemePresetDescriptor& preset : themePresets()) {
            preset_->addItem(preset.displayName, preset.id);
        }

        width_ = new QComboBox(this);
        width_->addItem(tr("Current width"), static_cast<int>(PreviewWidth::Current));
        width_->addItem(tr("Compact (480)"), static_cast<int>(PreviewWidth::Compact));
        width_->addItem(tr("Medium (720)"), static_cast<int>(PreviewWidth::Medium));
        width_->addItem(tr("Expanded (1024)"), static_cast<int>(PreviewWidth::Expanded));

        dpr_ = new QComboBox(this);
        dpr_->addItem(tr("DPR 1x"), 1.0);
        dpr_->addItem(tr("DPR 2x"), 2.0);

        direction_ = new QComboBox(this);
        direction_->addItem(tr("LTR"), static_cast<int>(Qt::LeftToRight));
        direction_->addItem(tr("RTL"), static_cast<int>(Qt::RightToLeft));

        auto* refresh = new QPushButton(tr("Refresh"), this);
        controls->addWidget(preset_);
        controls->addWidget(width_);
        controls->addWidget(dpr_);
        controls->addWidget(direction_);
        controls->addWidget(refresh);
        root->addLayout(controls);

        detail_ = new QLabel(this);
        detail_->setWordWrap(true);
        root->addWidget(detail_);

        auto* scroll = new QScrollArea(this);
        scroll->setWidgetResizable(false);
        preview_ = new QLabel(scroll);
        preview_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        preview_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        scroll->setWidget(preview_);
        root->addWidget(scroll, 1);

        auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, this);
        root->addWidget(buttons);
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
        connect(refresh, &QPushButton::clicked, this, [this]() {
            refreshPreview();
        });

        const auto refreshOnChange = [this](int) { refreshPreview(); };
        connect(preset_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, refreshOnChange);
        connect(width_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, refreshOnChange);
        connect(dpr_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, refreshOnChange);
        connect(direction_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, refreshOnChange);

        refreshPreview();
    }

private:
    void refreshPreview()
    {
        if (!target_) {
            detail_->setText(tr("The preview target no longer exists."));
            preview_->clear();
            return;
        }

        auto& manager = QtMaterial::ThemeManager::instance();
        const QtMaterial::ThemeOptions originalOptions = manager.options();
        const QString presetId = preset_->currentData().toString();

        if (!presetId.isEmpty()) {
            QtMaterial::ThemeOptions options;
            if (optionsForPreset(presetId, originalOptions, &options)) {
                manager.setThemeOptions(options);
            }
        }

        const PreviewWidth widthMode =
            static_cast<PreviewWidth>(width_->currentData().toInt());
        const qreal dpr = dpr_->currentData().toDouble();
        const Qt::LayoutDirection direction =
            static_cast<Qt::LayoutDirection>(direction_->currentData().toInt());

        const PreviewCapture capture =
            capturePreview(target_, widthMode, dpr, direction);

        if (!presetId.isEmpty()) {
            manager.setThemeOptions(originalOptions);
        }

        preview_->setPixmap(capture.pixmap);
        const qreal ratio = qMax<qreal>(1.0, capture.pixmap.devicePixelRatio());
        preview_->resize(
            qRound(capture.pixmap.width() / ratio),
            qRound(capture.pixmap.height() / ratio));
        detail_->setText(capture.detail);
    }

    QPointer<QWidget> target_;
    QComboBox* preset_ = nullptr;
    QComboBox* width_ = nullptr;
    QComboBox* dpr_ = nullptr;
    QComboBox* direction_ = nullptr;
    QLabel* detail_ = nullptr;
    QLabel* preview_ = nullptr;
};


class NativeMaterialPolicyDialog final : public QDialog
{
public:
    NativeMaterialPolicyDialog(
        QWidget* target,
        QDesignerFormEditorInterface* core,
        QWidget* parent = nullptr)
        : QDialog(parent)
        , target_(target)
        , core_(core)
    {
        setWindowTitle(tr("Native Qt Material adaptation"));
        auto* outer = new QVBoxLayout(this);
        auto* help = new QLabel(
            tr("Keep the original Qt widget class. These opt-in dynamic "
               "properties are saved in the .ui file; call "
               "QtMaterialNativeAdapter::applyDeclaredToDescendants() "
               "after setupUi() in the application."),
            this);
        help->setWordWrap(true);
        outer->addWidget(help);
        auto* form = new QFormLayout;
        outer->addLayout(form);

        enabled_ = new QCheckBox(tr("Enable Material adaptation"), this);
        enabled_->setChecked(target_->property("qtm3MaterialAdapt").toBool());
        form->addRow(tr("Material"), enabled_);
        optOut_ = new QCheckBox(tr("Opt out of Material adaptation"), this);
        optOut_->setToolTip(tr("An opt-out takes precedence over an enabled declaration."));
        optOut_->setChecked(target_->property("qtm3MaterialOptOut").toBool());
        form->addRow(tr("Exception"), optOut_);

        const QStringList names = nativeEditablePropertyNames(target_);
        if (names.contains(QStringLiteral("qtm3MaterialVariant"))) {
            variant_ = new QComboBox(this);
            variant_->addItem(tr("Text"), QStringLiteral("text"));
            variant_->addItem(tr("Filled"), QStringLiteral("filled"));
            variant_->addItem(tr("Filled tonal"), QStringLiteral("filled-tonal"));
            variant_->addItem(tr("Outlined"), QStringLiteral("outlined"));
            variant_->addItem(tr("Elevated"), QStringLiteral("elevated"));
            form->addRow(tr("Button variant"), variant_);
            selectValue(variant_, QStringLiteral("qtm3MaterialVariant"));
        }
        if (names.contains(QStringLiteral("qtm3MaterialDensity"))) {
            density_ = new QComboBox(this);
            density_->addItem(tr("Default"), QStringLiteral("default"));
            density_->addItem(tr("Compact"), QStringLiteral("compact"));
            density_->addItem(tr("Comfortable"), QStringLiteral("comfortable"));
            form->addRow(tr("Density"), density_);
            selectValue(density_, QStringLiteral("qtm3MaterialDensity"));
        }
        if (names.contains(QStringLiteral("qtm3MaterialTextFieldVariant"))) {
            textFieldVariant_ = new QComboBox(this);
            textFieldVariant_->addItem(tr("Outlined"), QStringLiteral("outlined"));
            textFieldVariant_->addItem(tr("Filled"), QStringLiteral("filled"));
            form->addRow(tr("Text field variant"), textFieldVariant_);
            selectValue(textFieldVariant_, QStringLiteral("qtm3MaterialTextFieldVariant"));
        }

        auto* buttons = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Reset,
            this);
        outer->addWidget(buttons);
        connect(buttons->button(QDialogButtonBox::Reset),
                &QPushButton::clicked, this, [this]() {
            enabled_->setChecked(false);
            optOut_->setChecked(false);
            if (variant_) variant_->setCurrentIndex(0);
            if (density_) density_->setCurrentIndex(0);
            if (textFieldVariant_) textFieldVariant_->setCurrentIndex(0);
        });
        connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
            if (applyChanges()) {
                accept();
            }
        });
        connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    }

private:
    void selectValue(QComboBox* combo, const QString& name)
    {
        const QVariant stored = target_->property(name.toLatin1().constData());
        const QString value = stored.isValid()
            ? stored.toString().trimmed().toLower()
            : nativeDesignerDefault(name).toString();
        const int index = combo->findData(value);
        if (index >= 0) {
            combo->setCurrentIndex(index);
        }
    }

    bool applyChanges()
    {
        if (!target_) {
            return false;
        }
        const QStringList names = nativeEditablePropertyNames(target_);
        if (names.isEmpty()) {
            return false;
        }
        const QDesignerFormWindowInterface* form =
            QDesignerFormWindowInterface::findFormWindow(target_);
        // beginCommand/endCommand group Designer's undoable cursor operations.
        QDesignerFormWindowInterface* writableForm =
            const_cast<QDesignerFormWindowInterface*>(form);
        if (writableForm) {
            writableForm->beginCommand(tr("Configure native Material properties"));
        }
        bool ok = true;
        const auto set = [this, &ok](const QString& key, const QVariant& value) {
            if (!ok) {
                return;
            }
            const QVariant previous = target_->property(key.toLatin1().constData());
            if (previous.isValid() && previous == value) {
                return;
            }
            // Do not add no-op default declarations to an unconfigured control.
            if (!previous.isValid() && value == nativeDesignerDefault(key)) {
                return;
            }
            ok = setNativeDesignerProperty(target_, key, value, core_);
        };
        set(QStringLiteral("qtm3MaterialAdapt"), enabled_->isChecked());
        set(QStringLiteral("qtm3MaterialOptOut"), optOut_->isChecked());
        if (variant_) set(QStringLiteral("qtm3MaterialVariant"), variant_->currentData());
        if (density_) set(QStringLiteral("qtm3MaterialDensity"), density_->currentData());
        if (textFieldVariant_) {
            set(QStringLiteral("qtm3MaterialTextFieldVariant"), textFieldVariant_->currentData());
        }
        if (writableForm) {
            writableForm->endCommand();
        }
        return ok;
    }

    QPointer<QWidget> target_;
    QDesignerFormEditorInterface* core_ = nullptr;
    QCheckBox* enabled_ = nullptr;
    QCheckBox* optOut_ = nullptr;
    QComboBox* variant_ = nullptr;
    QComboBox* density_ = nullptr;
    QComboBox* textFieldVariant_ = nullptr;
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
        if (!isQtMaterialWidget(widget_)) {
            editAction_ = new QAction(tr("Configure native Material 3..."), this);
            connect(editAction_, &QAction::triggered, this, [this]() {
                if (!widget_) return;
                NativeMaterialPolicyDialog dialog(widget_, core_, widget_);
                dialog.exec();
            });
            return;
        }
        editAction_ = new QAction(tr("Edit Material 3 properties..."), this);
        connect(editAction_, &QAction::triggered, this, [this]() {
            MaterialPropertyDialog dialog(widget_, core_, widget_);
            dialog.exec();
        });

        previewAction_ = new QAction(tr("Designer 3.0 preview..."), this);
        connect(previewAction_, &QAction::triggered, this, [this]() {
            MaterialPreviewDialog dialog(widget_, widget_);
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
        if (!previewAction_) {
            return {editAction_};
        }
        return {
            editAction_,
            previewAction_,
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
    QAction* previewAction_ = nullptr;
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
        if (!isQtMaterialWidget(widget)
            && nativeEditablePropertyNames(widget).isEmpty()) {
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

} // namespace


QStringList nativeEditablePropertyNames(const QWidget* widget)
{
    using Kind = QtMaterial::QtMaterialNativeAdapter::WidgetKind;
    const Kind kind = QtMaterial::QtMaterialNativeAdapter::kind(widget);
    if (kind == Kind::Unsupported) {
        return {};
    }

    QStringList properties = {
        QStringLiteral("qtm3MaterialAdapt"),
        QStringLiteral("qtm3MaterialOptOut")
    };
    if (kind != Kind::ProgressBar) {
        properties.append(QStringLiteral("qtm3MaterialDensity"));
    }
    if (kind == Kind::PushButton || kind == Kind::ToolButton) {
        properties.append(QStringLiteral("qtm3MaterialVariant"));
    }
    if (kind == Kind::LineEdit) {
        properties.append(QStringLiteral("qtm3MaterialTextFieldVariant"));
    }
    return properties;
}

QVariant nativeDesignerDefault(const QString& propertyName)
{
    if (propertyName == QStringLiteral("qtm3MaterialAdapt")
        || propertyName == QStringLiteral("qtm3MaterialOptOut")) {
        return QVariant(false);
    }
    if (propertyName == QStringLiteral("qtm3MaterialDensity")) {
        return QStringLiteral("default");
    }
    if (propertyName == QStringLiteral("qtm3MaterialVariant")) {
        return QStringLiteral("text");
    }
    if (propertyName == QStringLiteral("qtm3MaterialTextFieldVariant")) {
        return QStringLiteral("outlined");
    }
    return {};
}

bool setNativeDesignerProperty(
    QWidget* widget,
    const QString& propertyName,
    const QVariant& value,
    QDesignerFormEditorInterface* core)
{
    if (!nativeEditablePropertyNames(widget).contains(propertyName)
        || !value.isValid()) {
        return false;
    }
    const bool boolean = propertyName == QStringLiteral("qtm3MaterialAdapt")
        || propertyName == QStringLiteral("qtm3MaterialOptOut");
    QVariant normalized;
    if (boolean) {
        // Do not let Designer write strings such as "false" as truthy booleans.
        if (value.userType() != QMetaType::Bool) {
            return false;
        }
        normalized = value.toBool();
    } else {
        if (value.userType() != QMetaType::QString) {
            return false;
        }
        normalized = value.toString().trimmed().toLower();
        QStringList allowed;
        if (propertyName == QStringLiteral("qtm3MaterialDensity")) {
            allowed = QStringList{QStringLiteral("default"), QStringLiteral("compact"),
                                  QStringLiteral("comfortable")};
        } else if (propertyName == QStringLiteral("qtm3MaterialVariant")) {
            allowed = QStringList{QStringLiteral("text"), QStringLiteral("filled"),
                                  QStringLiteral("filled-tonal"), QStringLiteral("outlined"),
                                  QStringLiteral("elevated")};
        } else {
            allowed = QStringList{QStringLiteral("outlined"), QStringLiteral("filled")};
        }
        if (!allowed.contains(normalized.toString())) {
            return false;
        }
    }
    const QByteArray key = propertyName.toLatin1();
    const QVariant previous = widget->property(key.constData());
    if (previous.isValid() && previous == normalized) {
        return true;
    }

    QDesignerFormWindowInterface* form =
        QDesignerFormWindowInterface::findFormWindow(widget);
    if (!form) {
        // QObject::setProperty returns false when creating a *new* dynamic
        // property, even when the insertion succeeds.
        widget->setProperty(key.constData(), normalized);
        return widget->property(key.constData()) == normalized;
    }
    if (!core || !core->extensionManager() || !form->cursor()) {
        return false;
    }

    QExtensionManager* manager = core->extensionManager();
    auto* sheet = qt_extension<QDesignerPropertySheetExtension*>(manager, widget);
    auto* dynamic = qt_extension<QDesignerDynamicPropertySheetExtension*>(manager, widget);
    if (!sheet || !dynamic) {
        return false;
    }

    if (sheet->indexOf(propertyName) < 0) {
        // The Designer property sheet must know about an authored dynamic
        // property: setting QObject::setProperty alone does not serialize it.
        if (!dynamic->dynamicPropertiesAllowed()
            || !dynamic->canAddDynamicProperty(propertyName)
            || dynamic->addDynamicProperty(propertyName, nativeDesignerDefault(propertyName)) < 0) {
            return false;
        }
    }

    // Cursor writes participate in Designer's undo/redo history, unlike
    // property-sheet setProperty or direct QObject::setProperty.
    form->cursor()->setWidgetProperty(widget, propertyName, normalized);
    form->setDirty(true);
    return widget->property(key.constData()) == normalized;
}

bool resetNativeDesignerProperties(
    QWidget* widget,
    QDesignerFormEditorInterface* core)
{
    const QStringList names = nativeEditablePropertyNames(widget);
    if (names.isEmpty()) {
        return false;
    }
    QDesignerFormWindowInterface* form =
        QDesignerFormWindowInterface::findFormWindow(widget);
    if (form) {
        form->beginCommand(QObject::tr("Reset native Material properties"));
    }
    bool ok = true;
    for (const QString& name : names) {
        const QByteArray key = name.toLatin1();
        const QVariant previous = widget->property(key.constData());
        if (previous.isValid() && previous != nativeDesignerDefault(name)) {
            ok = setNativeDesignerProperty(widget, name, nativeDesignerDefault(name), core) && ok;
        }
    }
    if (form) {
        form->endCommand();
    }
    return ok;
}

QStringList editablePropertyNames(const QWidget* widget)
{
    QStringList result;
    if (!widget) {
        return result;
    }

    QVector<const QMetaObject*> materialMetaObjects;
    for (const QMetaObject* meta = widget->metaObject();
         meta && isQtMaterialMetaObject(meta);
         meta = meta->superClass()) {
        materialMetaObjects.prepend(meta);
    }

    for (const QMetaObject* meta : materialMetaObjects) {
        for (int index = meta->propertyOffset(); index < meta->propertyCount(); ++index) {
            const QMetaProperty property = meta->property(index);
            bool designable = false;
            bool stored = false;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            designable = property.isDesignable();
            stored = property.isStored();
#else
            designable = property.isDesignable(widget);
            stored = property.isStored(widget);
#endif
            if (!property.isReadable()
                || !property.isWritable()
                || !designable
                || !stored
                || !supportedPropertyType(property)) {
                continue;
            }

            const QString name = QString::fromLatin1(property.name());
            if (!result.contains(name)) {
                result.append(name);
            }
        }
    }
    return result;
}

bool propertyCanReset(const QWidget* widget, const QString& propertyName)
{
    if (!widget) {
        return false;
    }
    const int index = widget->metaObject()->indexOfProperty(propertyName.toLatin1().constData());
    return index >= 0 && widget->metaObject()->property(index).isResettable();
}

bool resetPropertyToDefault(QWidget* widget, const QString& propertyName)
{
    if (!widget) {
        return false;
    }
    const int index = widget->metaObject()->indexOfProperty(propertyName.toLatin1().constData());
    if (index < 0) {
        return false;
    }
    const QMetaProperty property = widget->metaObject()->property(index);
    return property.isResettable() && property.reset(widget);
}

QStringList designerThemePresetIds()
{
    QStringList result;
    for (const ThemePresetDescriptor& preset : themePresets()) {
        result.append(preset.id);
    }
    return result;
}

bool applyThemePreset(const QString& presetId)
{
    auto& manager = QtMaterial::ThemeManager::instance();
    QtMaterial::ThemeOptions options;
    if (!optionsForPreset(presetId, manager.options(), &options)) {
        return false;
    }

    auto& saved = savedPreviewOptions();
    if (!saved.has_value()) {
        saved = manager.options();
    }
    manager.setThemeOptions(options);
    return true;
}

int previewLogicalWidth(PreviewWidth width)
{
    switch (width) {
    case PreviewWidth::Current:
        return -1;
    case PreviewWidth::Compact:
        return 480;
    case PreviewWidth::Medium:
        return 720;
    case PreviewWidth::Expanded:
        return 1024;
    }
    return -1;
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool TabsContainerExtension::canAddWidget() const
{
    return tabs_ != nullptr;
}

bool TabsContainerExtension::canRemove(int index) const
{
    return tabs_ && index >= 0 && index < tabs_->count();
}
#endif

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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool AdaptiveShellContainerExtension::canAddWidget() const
{
    return shell_ && count() < 2;
}

bool AdaptiveShellContainerExtension::canRemove(int index) const
{
    return shell_ && index >= 0 && index < count();
}
#endif

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
