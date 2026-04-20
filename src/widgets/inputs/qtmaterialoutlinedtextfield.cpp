#include "qtmaterial/widgets/inputs/qtmaterialoutlinedtextfield.h"

#include <QLineEdit>
#include <QPainter>
#include <QResizeEvent>

#include "qtmaterial/effects/qtmaterialtransitioncontroller.h"
#include "qtmaterial/specs/qtmaterialspecfactory.h"
#include "private/qtmaterialtextfieldshellhelper_p.h"
#include "../../core/private/qtmaterialaccessibilityhelper_p.h"

namespace QtMaterial {

QtMaterialOutlinedTextField::QtMaterialOutlinedTextField(QWidget* parent)
    : QtMaterialInputControl(parent)
    , m_lineEdit(new QLineEdit(this))
    , m_transition(new QtMaterialTransitionController(this))
{
    setMinimumHeight(64);
    if (m_lineEdit) {
        m_lineEdit->setFrame(false);
        m_lineEdit->setAttribute(Qt::WA_MacShowFocusRect, false);
        m_lineEdit->setClearButtonEnabled(false);
    }
    setFocusProxy(m_lineEdit);
    syncAccessibilityState();
}

QtMaterialOutlinedTextField::~QtMaterialOutlinedTextField() = default;

QString QtMaterialOutlinedTextField::text() const
{
    return m_lineEdit ? m_lineEdit->text() : QString();
}

void QtMaterialOutlinedTextField::setText(const QString& text)
{
    if (m_lineEdit) {
        m_lineEdit->setText(text);
        syncAccessibilityState();
        update();
    }
}

QLineEdit* QtMaterialOutlinedTextField::lineEdit() const
{
    return m_lineEdit;
}

QSize QtMaterialOutlinedTextField::sizeHint() const
{
    ensureSpecResolved();
    return QSize(220, spec().minHeight + 24);
}

QSize QtMaterialOutlinedTextField::minimumSizeHint() const
{
    ensureSpecResolved();
    return QSize(120, spec().minHeight + 20);
}

QtMaterialOutlinedTextField::ShellVariant QtMaterialOutlinedTextField::shellVariant() const
{
    return ShellVariant::Outlined;
}

void QtMaterialOutlinedTextField::syncAccessibilityState()
{
    QtMaterialInputControl::syncAccessibilityState();
    AccessibilityHelper::applyInputAccessibility(this, m_lineEdit, labelText(), supportingText(), errorText(), hasErrorState());
}

TextFieldSpec QtMaterialOutlinedTextField::resolveTextFieldSpec(const SpecFactory& factory) const
{
    return factory.outlinedTextFieldSpec(theme(), density());
}

void QtMaterialOutlinedTextField::themeChangedEvent(const Theme& theme)
{
    QtMaterialInputControl::themeChangedEvent(theme);
    m_specDirty = true;
}

void QtMaterialOutlinedTextField::invalidateResolvedSpec()
{
    m_specDirty = true;
}

void QtMaterialOutlinedTextField::ensureSpecResolved() const
{
    if (!m_specDirty) {
        return;
    }

    SpecFactory factory;
    m_spec = resolveTextFieldSpec(factory);
    m_specDirty = false;
}

const TextFieldSpec& QtMaterialOutlinedTextField::spec() const
{
    ensureSpecResolved();
    return m_spec;
}

void QtMaterialOutlinedTextField::resizeEvent(QResizeEvent* event)
{
    QtMaterialInputControl::resizeEvent(event);
    ensureSpecResolved();
    const auto variant = shellVariant() == ShellVariant::Filled
        ? QtMaterialTextFieldShellHelper::Variant::Filled
        : QtMaterialTextFieldShellHelper::Variant::Outlined;
    const auto layout = QtMaterialTextFieldShellHelper::layoutFor(rect(), spec(), theme(), variant);
    if (m_lineEdit) {
        m_lineEdit->setGeometry(layout.editorRect);
    }
    syncAccessibilityState();
}

void QtMaterialOutlinedTextField::paintEvent(QPaintEvent*)
{
    ensureSpecResolved();

    const auto variant = shellVariant() == ShellVariant::Filled
        ? QtMaterialTextFieldShellHelper::Variant::Filled
        : QtMaterialTextFieldShellHelper::Variant::Outlined;
    const auto layout = QtMaterialTextFieldShellHelper::layoutFor(rect(), spec(), theme(), variant);

    QPainter painter(this);
    QtMaterialTextFieldShellHelper::paintShell(
        &painter,
        layout,
        theme(),
        spec(),
        interactionState(),
        variant,
        labelText(),
        supportingText(),
        errorText(),
        hasErrorState(),
        font());
}

} // namespace QtMaterial
