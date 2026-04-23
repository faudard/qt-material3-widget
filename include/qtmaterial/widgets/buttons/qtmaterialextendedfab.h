#pragma once

#include <QIcon>
#include <QSize>
#include <QString>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"
#include "qtmaterial/widgets/buttons/qtmaterialfabvariant.h"

namespace QtMaterial {

/**
 * @brief Material extended floating action button.
 *
 * QtMaterialExtendedFab represents the floating action button variant that
 * carries a text label, optionally paired with a leading icon. It is intended
 * for a promoted action that benefits from a visible label.
 *
 * The height, icon metrics, spacing, padding, shape, elevation, state layer and
 * touch target are resolved from the extended FAB spec. The width is derived
 * from the resolved horizontal padding, optional icon, optional icon-label
 * spacing and measured label width.
 *
 * The widget is intentionally not checkable and uses the filled-button rendering
 * pipeline to keep state-layer, ripple and focus behavior consistent with the
 * rest of the button family.
 *
 * The visible text normally provides the accessible name through Qt's standard
 * button semantics. Applications may still call setAccessibleName() when the
 * visible label is abbreviated or when a richer screen-reader label is needed.
 *
 * Interaction feedback, including state-layer opacity, ripple clipping and elevated hover/press
 * transitions, is inherited from QtMaterialFilledButton and remains spec-driven.
 *
 * The color variant defaults to QtMaterialFabVariant::Primary. Variant changes
 * affect colors only; content-derived width and touch target remain spec-driven.
 */
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialExtendedFab : public QtMaterialFilledButton {
    Q_OBJECT

public:
    /**
     * @brief Creates an empty extended FAB.
     *
     * Provide text or an explicit accessible name before using an empty
     * extended FAB in a real interface.
     */
    explicit QtMaterialExtendedFab(QWidget* parent = nullptr);

    /**
     * @brief Creates a text-only extended FAB initialized with @a text.
     *
     * The visible text also communicates the action to assistive technologies
     * unless an explicit accessible name is set by the application.
     */
    explicit QtMaterialExtendedFab(const QString& text, QWidget* parent = nullptr);

    /**
     * @brief Creates an icon-plus-text extended FAB initialized with @a icon and @a text.
     *
     * The icon is decorative relative to the visible label unless the
     * application assigns more specific accessibility metadata.
     */
    QtMaterialExtendedFab(const QIcon& icon, const QString& text, QWidget* parent = nullptr);

    ~QtMaterialExtendedFab() override;

    /**
     * @brief Sets the color variant used to resolve extended FAB color tokens.
     */
    void setFabVariant(QtMaterialFabVariant variant);

    /**
     * @brief Returns the current color variant.
     */
    QtMaterialFabVariant fabVariant() const noexcept;

    /**
     * @brief Returns the resolved extended FAB touch target expanded from content width.
     */
    QSize sizeHint() const override;

    /**
     * @brief Returns the same value as sizeHint().
     */
    QSize minimumSizeHint() const override;

protected:
    /**
     * @brief Resolves the Material extended FAB spec and adapts it to the filled-button renderer.
     */
    ButtonSpec resolveButtonSpec() const override;

private:
    QtMaterialFabVariant m_variant = QtMaterialFabVariant::Primary;
};

} // namespace QtMaterial
