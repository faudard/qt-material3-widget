#pragma once
#include "qtmaterial/core/qtmaterialcontrol.h"
namespace QtMaterial {
class QTMATERIAL3_CORE_EXPORT QtMaterialSurface : public QtMaterialControl
{
    Q_OBJECT
public:
    explicit QtMaterialSurface(QWidget* parent = nullptr);
    ~QtMaterialSurface() override;
    bool clipsContent() const noexcept;
    void setClipsContent(bool value);
protected:
    void paintEvent(QPaintEvent* event) override;
private:
    bool m_clipsContent;
};
} // namespace QtMaterial
