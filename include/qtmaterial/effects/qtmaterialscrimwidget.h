#pragma once
#include "qtmaterial/core/qtmaterialwidget.h"
#include "qtmaterial/qtmaterialglobal.h"
namespace QtMaterial {
class QTMATERIAL3_EFFECTS_EXPORT QtMaterialScrimWidget : public QtMaterialWidget
{
    Q_OBJECT
public:
    explicit QtMaterialScrimWidget(QWidget* parent = nullptr);
    ~QtMaterialScrimWidget() override;

    QColor scrimColor() const;
    void setScrimColor(const QColor& color);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QColor m_scrimColor;
};
} // namespace QtMaterial
