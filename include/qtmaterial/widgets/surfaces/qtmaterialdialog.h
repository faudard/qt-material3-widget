#pragma once
class QKeyEvent;
#include <QPointer>
#include "qtmaterial/core/qtmaterialoverlaysurface.h"
#include "qtmaterial/specs/qtmaterialdialogspec.h"
#include "qtmaterial/qtmaterialglobal.h"
class QVBoxLayout;
namespace QtMaterial {
class QtMaterialScrimWidget;
class QtMaterialTransitionController;
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialDialog : public QtMaterialOverlaySurface
{
    Q_OBJECT
public:
    explicit QtMaterialDialog(QWidget* parent = nullptr);
    ~QtMaterialDialog() override;

    void setBodyWidget(QWidget* widget);
    QWidget* bodyWidget() const;
    void open();
    void close();

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;

private:
    void resolveSpecIfNeeded() const;
    void syncChildGeometry();
    void syncAccessibilityState();
    void updateScrimForProgress(qreal progress);

    mutable bool m_specDirty = true;
    mutable DialogSpec m_spec;
    QPointer<QWidget> m_bodyWidget;
    QVBoxLayout* m_layout = nullptr;
    QtMaterialScrimWidget* m_scrim = nullptr;
    QtMaterialTransitionController* m_transition = nullptr;
};
} // namespace QtMaterial
