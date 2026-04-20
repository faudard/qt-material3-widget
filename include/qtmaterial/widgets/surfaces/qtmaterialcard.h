#pragma once

#include "qtmaterial/core/qtmaterialsurface.h"
#include "qtmaterial/specs/qtmaterialcardspec.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialCard : public QtMaterialSurface
{
    Q_OBJECT
public:
    explicit QtMaterialCard(QWidget* parent = nullptr);
    ~QtMaterialCard() override;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;

private:
    void ensureSpecResolved() const;

    mutable bool m_specDirty = true;
    mutable CardSpec m_spec;
};

} // namespace QtMaterial
