#pragma once
#include <QString>
#include <memory>

#include "qtmaterial/qtmaterialglobal.h"
#include "qtmaterial/core/qtmaterialabstractbutton.h"
#include "qtmaterial/specs/qtmaterialiconbuttonspec.h"


namespace QtMaterial {

class QtMaterialIconButtonPrivate;

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialIconButton : public QtMaterialAbstractButton
{
public:
    explicit QtMaterialIconButton(QWidget* parent = nullptr);
    explicit QtMaterialIconButton(const QIcon& icon, QWidget* parent = nullptr);
    ~QtMaterialIconButton() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    bool isSelected() const noexcept;
    void setSelected(bool selected);

    bool requiresAccessibleName() const noexcept;
    void setRequiresAccessibleName(bool required);
    bool hasUsableAccessibleName() const;
    QString effectiveAccessibleName() const;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void changeEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

    void themeChangedEvent(const Theme& theme) override;
    void stateChangedEvent() override;
    void contentChangedEvent() override;
    void syncAccessibilityState() override;

    virtual IconButtonSpec resolveIconButtonSpec() const;

private:
    void syncIconButtonAccessibility();
    void invalidateLayoutCache();
    void invalidateResolvedSpec();

    void ensureSpecResolved() const;
    void ensureLayoutResolved() const;

 std::unique_ptr<QtMaterialIconButtonPrivate> d;
};

} // namespace QtMaterial