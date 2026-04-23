#pragma once

#include <QIcon>
#include <QSize>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfabvariant.h"

namespace QtMaterial {

/**
 * @brief Compact Material floating action button.
 *
 * QtMaterialFab represents the icon-only floating action button variant. It is
 * intended for a single promoted action and keeps the visual container diameter
 * and touch target size resolved from the Material FAB spec.
 *
 * The widget is intentionally not checkable. Text content is cleared during
 * construction so the component remains icon-first even though it inherits the
 * filled-button rendering pipeline.
 *
 * Because this variant is icon-only, applications should provide an
 * accessibleName() and usually a tooltip that describes the promoted action.
 *
 * Interaction feedback, including state-layer opacity, ripple clipping and elevated hover/press
 * transitions, is inherited from QtMaterialFilledButton and remains spec-driven.
 *
 * The color variant defaults to QtMaterialFabVariant::Primary. Variant changes
 * affect colors only; sizing, shape, elevation and motion remain spec-driven.
 */
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFab : public QtMaterialFilledButton {
    Q_OBJECT

public:
    /**
     * @brief Creates an empty icon-only FAB.
     *
     * Set an icon and an accessible name before exposing the widget to users.
     */
    explicit QtMaterialFab(QWidget* parent = nullptr);

    /**
     * @brief Creates an icon-only FAB initialized with @a icon.
     *
     * The icon is visual content only. Use setAccessibleName() to expose the
     * action semantics to assistive technologies.
     */
    explicit QtMaterialFab(const QIcon& icon, QWidget* parent = nullptr);

    ~QtMaterialFab() override;

    /**
     * @brief Sets the color variant used to resolve FAB color tokens.
     */
    void setFabVariant(QtMaterialFabVariant variant);

    /**
     * @brief Returns the current color variant.
     */
    QtMaterialFabVariant fabVariant() const noexcept;

    /**
     * @brief Returns the resolved FAB touch target expanded from the visual diameter.
     */
    QSize sizeHint() const override;

    /**
     * @brief Returns the same value as sizeHint().
     */
    QSize minimumSizeHint() const override;

protected:
    /**
     * @brief Resolves the Material FAB spec and adapts it to the filled-button renderer.
     */
    ButtonSpec resolveButtonSpec() const override;

private:
    QtMaterialFabVariant m_variant = QtMaterialFabVariant::Primary;
};

} // namespace QtMaterial
