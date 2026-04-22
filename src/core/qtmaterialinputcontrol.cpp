#include "qtmaterial/core/qtmaterialinputcontrol.h"

#include <QtGlobal>

#include "private/qtmaterialaccessibilityhelper_p.h"

namespace {

constexpr int kHorizontalPadding = 16;
constexpr int kContentTopPadding = 8;
constexpr int kSupportingTextHeight = 16;
constexpr int kSupportingBottomInset = 4;
constexpr int kContentToSupportingGap = 4;

} // namespace

namespace QtMaterial {

QtMaterialInputControl::QtMaterialInputControl(QWidget* parent)
    : QtMaterialControl(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    syncAccessibilityState();
}

QtMaterialInputControl::~QtMaterialInputControl() = default;

QString QtMaterialInputControl::labelText() const
{
    return m_labelText;
}

void QtMaterialInputControl::setLabelText(const QString& text)
{
    if (m_labelText == text) {
        return;
    }

    m_labelText = text;
    syncAccessibilityState();
    contentChangedEvent();
    updateGeometry();
    update();
}

QString QtMaterialInputControl::supportingText() const
{
    return m_supportingText;
}

void QtMaterialInputControl::setSupportingText(const QString& text)
{
    if (m_supportingText == text) {
        return;
    }

    m_supportingText = text;
    syncAccessibilityState();
    contentChangedEvent();
    updateGeometry();
    update();
}

QString QtMaterialInputControl::errorText() const
{
    return m_errorText;
}

void QtMaterialInputControl::setErrorText(const QString& text)
{
    if (m_errorText == text) {
        return;
    }

    m_errorText = text;
    syncAccessibilityState();
    contentChangedEvent();
    updateGeometry();
    update();
}

bool QtMaterialInputControl::hasErrorState() const noexcept
{
    return m_hasErrorState;
}

void QtMaterialInputControl::setHasErrorState(bool value)
{
    if (m_hasErrorState == value) {
        return;
    }

    m_hasErrorState = value;
    interactionState().setError(value);
    syncAccessibilityState();
    contentChangedEvent();
    stateChangedEvent();
}

void QtMaterialInputControl::syncAccessibilityState()
{
    AccessibilityHelper::applyInputAccessibility(
        this, nullptr, m_labelText, m_supportingText, m_errorText, m_hasErrorState);
}

void QtMaterialInputControl::contentChangedEvent()
{
}

QRect QtMaterialInputControl::supportingTextRect() const
{
    const int x = kHorizontalPadding;
    const int widthValue = qMax(0, width() - (2 * kHorizontalPadding));
    const int y = qMax(kContentTopPadding,
                       height() - kSupportingTextHeight - kSupportingBottomInset);

    return QRect(x, y, widthValue, kSupportingTextHeight);
}

QRect QtMaterialInputControl::contentRect() const
{
    const QRect supporting = supportingTextRect();
    const int x = kHorizontalPadding;
    const int y = kContentTopPadding;
    const int widthValue = qMax(0, width() - (2 * kHorizontalPadding));
    const int bottom = qMax(y, supporting.top() - kContentToSupportingGap);
    const int heightValue = qMax(0, bottom - y);

    return QRect(x, y, widthValue, heightValue);
}

} // namespace QtMaterial