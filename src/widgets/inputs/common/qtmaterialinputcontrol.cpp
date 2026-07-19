#include "qtmaterial/widgets/inputs/common/qtmaterialinputcontrol.h"

#include <algorithm>

#include "qtmaterial/core/private/qtmaterialaccessibilityhelper_p.h"

namespace {
constexpr int kHorizontalPadding = 16;
constexpr int kContentTopPadding = 8;
constexpr int kSupportingTextHeight = 16;
constexpr int kSupportingBottomInset = 4;
constexpr int kContentToSupportingGap = 4;
} // namespace

namespace QtMaterial {

QtMaterialInputControl::QtMaterialInputControl(QWidget* parent)
    : QtMaterialFormFieldControl(parent) {
    setFocusPolicy(Qt::StrongFocus);
    setMaterialComponent(QStringLiteral("InputControl"));
    syncAccessibilityState();
}

QtMaterialInputControl::~QtMaterialInputControl() = default;

QString QtMaterialInputControl::labelText() const {
    return label();
}

void QtMaterialInputControl::setLabelText(const QString& text) {
    setLabel(text);
}

QString QtMaterialInputControl::supportingText() const {
    return helperText();
}

void QtMaterialInputControl::setSupportingText(const QString& text) {
    setHelperText(text);
}

QString QtMaterialInputControl::errorText() const {
    return QtMaterialFormFieldControl::errorText();
}

void QtMaterialInputControl::setErrorText(const QString& text) {
    QtMaterialFormFieldControl::setErrorText(text);
}

bool QtMaterialInputControl::hasErrorState() const noexcept {
    return isInvalid();
}

void QtMaterialInputControl::setHasErrorState(bool value) {
    if (isInvalid() == value) {
        return;
    }
    setInvalid(value);
    syncAccessibilityState();
    stateChangedEvent();
}

void QtMaterialInputControl::formFieldChangedEvent() {
    QtMaterialFormFieldControl::formFieldChangedEvent();
    syncAccessibilityState();
    contentChangedEvent();
}

void QtMaterialInputControl::syncAccessibilityState() {
    AccessibilityHelper::applyInputAccessibility(
        this,
        nullptr,
        label(),
        helperText(),
        QtMaterialFormFieldControl::errorText(),
        isInvalid());
}

void QtMaterialInputControl::contentChangedEvent() {}

QRect QtMaterialInputControl::supportingTextRect() const {
    const int x = kHorizontalPadding;
    const int widthValue = std::max(0, width() - (2 * kHorizontalPadding));
    const int y = std::max(kContentTopPadding, height() - kSupportingTextHeight - kSupportingBottomInset);
    return QRect(x, y, widthValue, kSupportingTextHeight);
}

QRect QtMaterialInputControl::contentRect() const {
    const QRect supporting = supportingTextRect();
    const int x = kHorizontalPadding;
    const int y = kContentTopPadding;
    const int widthValue = std::max(0, width() - (2 * kHorizontalPadding));
    const int bottom = std::max(y, supporting.top() - kContentToSupportingGap);
    const int heightValue = std::max(0, bottom - y);
    return QRect(x, y, widthValue, heightValue);
}

} // namespace QtMaterial
