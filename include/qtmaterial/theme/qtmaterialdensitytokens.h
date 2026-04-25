#pragma once

#include <QHash>
#include <QtGlobal>
#include <type_traits>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

enum class DensityRole {
    Compact,
    Default,
    Comfortable
};

class QTMATERIAL3_THEME_EXPORT DensityTokens {
public:
    DensityTokens();
    ~DensityTokens();

    bool contains(DensityRole role) const;
    int value(DensityRole role) const;
    void setValue(DensityRole role, int logicalPxDelta);
    void clear();

private:
    QHash<DensityRole, int> m_values;
};

} // namespace QtMaterial

inline size_t qHash(QtMaterial::DensityRole value, size_t seed = 0) noexcept
{
    using Underlying = std::underlying_type_t<QtMaterial::DensityRole>;
    return qHash(static_cast<Underlying>(value), seed);
}
