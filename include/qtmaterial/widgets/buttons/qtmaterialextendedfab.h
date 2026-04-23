#pragma once

#include <QIcon>
#include <QSize>
#include <QString>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

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
 */
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialExtendedFab : public QtMaterialFilledButton {
    Q_OBJECT

public:
    /**
     * @brief Creates an empty extended FAB.
     */
    explicit QtMaterialExtendedFab(QWidget* parent = nullptr);

    /**
     * @brief Creates a text-only extended FAB initialized with @a text.
     */
    explicit QtMaterialExtendedFab(const QString& text, QWidget* parent = nullptr);

    /**
     * @brief Creates an icon-plus-text extended FAB initialized with @a icon and @a text.
     */
    QtMaterialExtendedFab(const QIcon& icon, const QString& text, QWidget* parent = nullptr);

    ~QtMaterialExtendedFab() override;

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
};

} // namespace QtMaterial
