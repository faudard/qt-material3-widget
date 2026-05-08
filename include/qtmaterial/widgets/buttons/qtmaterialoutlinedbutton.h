#pragma once


#include <memory>
class QEvent;
class QPaintEvent;
class QResizeEvent;

#include "qtmaterial/widgets/buttons/qtmaterialtextbutton.h"

namespace QtMaterial {

class QtMaterialOutlinedButtonPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialOutlinedButton : public QtMaterialTextButton {
 Q_OBJECT
public:
 explicit QtMaterialOutlinedButton(QWidget* parent = nullptr);
 ~QtMaterialOutlinedButton() override;

protected:
 void paintEvent(QPaintEvent* event) override;
 void resizeEvent(QResizeEvent* event) override;
 void changeEvent(QEvent* event) override;
 void themeChangedEvent(const QtMaterial::Theme& theme) override;
 void invalidateResolvedSpec() override;
 void contentChangedEvent() override;

 ButtonSpec resolveButtonSpec() const override;

private:
 friend class QtMaterialOutlinedButtonPrivate;
 std::unique_ptr<QtMaterialOutlinedButtonPrivate> d;
};

} // namespace QtMaterial
