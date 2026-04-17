#pragma once

#include <QPointer>

#include "qtmaterial/core/qtmaterialoverlaysurface.h"
#include "qtmaterial/specs/qtmaterialdialogspec.h"
#include "qtmaterial/qtmaterialglobal.h"

class QKeyEvent;
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

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

public slots:
    void open();
    void close();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void syncGeometryToHost() override;

private:
    void resolveSpecIfNeeded() const;
    void applyLayoutMargins();
    void updateScrimColor();
    QRectF panelRect() const;
    qreal panelRadius() const;

    mutable bool m_specDirty = true;
    mutable DialogSpec m_spec;
    QPointer<QWidget> m_bodyWidget;
    QVBoxLayout* m_layout = nullptr;
    QtMaterialScrimWidget* m_scrim = nullptr;
    QtMaterialTransitionController* m_transition = nullptr;
};

} // namespace QtMaterial
