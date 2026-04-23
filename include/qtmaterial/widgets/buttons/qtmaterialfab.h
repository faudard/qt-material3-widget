#pragma once

#include <QIcon>
#include <QSize>

#include "qtmaterial/widgets/buttons/qtmaterialfilledbutton.h"

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
 */
class QTMATERIAL3_WIDGETS_EXPORT QtMaterialFab : public QtMaterialFilledButton {
    Q_OBJECT

public:
    /**
     * @brief Creates an empty icon-only FAB.
     */
    explicit QtMaterialFab(QWidget* parent = nullptr);

    /**
     * @brief Creates an icon-only FAB initialized with @a icon.
     */
    explicit QtMaterialFab(const QIcon& icon, QWidget* parent = nullptr);

    ~QtMaterialFab() override;

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
};

} // namespace QtMaterial
