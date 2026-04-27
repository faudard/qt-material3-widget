#pragma once

#include <QIcon>

#include "qtmaterial/core/qtmaterialabstractbutton.h"
#include "qtmaterial/specs/qtmaterialchipspec.h"
#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

class QTMATERIAL3_WIDGETS_EXPORT QtMaterialChip : public QtMaterialAbstractButton {
    Q_OBJECT
    Q_PROPERTY(QtMaterial::ChipVariant variant READ variant WRITE setVariant NOTIFY variantChanged)
    Q_PROPERTY(bool removable READ isRemovable WRITE setRemovable NOTIFY removableChanged)

public:
    explicit QtMaterialChip(QWidget* parent = nullptr);
    explicit QtMaterialChip(const QString& text, QWidget* parent = nullptr);
    ~QtMaterialChip() override;

    ChipVariant variant() const noexcept;
    void setVariant(ChipVariant variant);

    bool isRemovable() const noexcept;
    void setRemovable(bool removable);

    QIcon trailingIcon() const;
    void setTrailingIcon(const QIcon& icon);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void variantChanged(QtMaterial::ChipVariant variant);
    void removableChanged(bool removable);
    void removeRequested();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void themeChangedEvent(const QtMaterial::Theme& theme) override;
    void invalidateResolvedSpec() override;
    void stateChangedEvent() override;

private:
    void ensureSpecResolved() const;
    ChipSpec resolveSpec() const;
    QRectF containerRect() const;
    QRect trailingIconRect(const QRectF& visualRect) const;

    ChipVariant m_variant = ChipVariant::Assist;
    bool m_removable = false;
    QIcon m_trailingIcon;
    mutable bool m_specDirty = true;
    mutable ChipSpec m_spec;
};

} // namespace QtMaterial
