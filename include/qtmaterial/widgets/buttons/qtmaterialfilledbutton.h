#pragma once

class QEvent;
class QPaintEvent;
class QResizeEvent;

#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

namespace QtMaterial {

class QtMaterialFilledButtonPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFilledButton : public QtMaterialTextButton {
 Q_OBJECT
public:
 explicit QtMaterialFilledButton(QWidget* parent = nullptr);
 ~QtMaterialFilledButton() override;

protected:
 void paintEvent(QPaintEvent* event) override;
 void resizeEvent(QResizeEvent* event) override;
 void changeEvent(QEvent* event) override;
 void themeChangedEvent(const QtMaterial::Theme& theme) override;
 void invalidateResolvedSpec() override;
 void contentChangedEvent() override;
 void stateChangedEvent() override;

 ButtonSpec resolveButtonSpec() const override;

private:
 friend class QtMaterialFilledButtonPrivate;
 QtMaterialFilledButtonPrivate* d = nullptr;
};

} // namespace QtMaterial
