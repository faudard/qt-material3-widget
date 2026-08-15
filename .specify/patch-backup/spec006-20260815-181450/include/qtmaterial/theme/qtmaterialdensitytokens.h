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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

inline size_t qHash(DensityRole value, size_t seed = 0) noexcept
{
    using Underlying = typename std::underlying_type<DensityRole>::type;
    return ::qHash(static_cast<Underlying>(value), seed);
}

#else

inline uint qHash(DensityRole value, uint seed = 0) noexcept
{
    using Underlying = typename std::underlying_type<DensityRole>::type;
    return ::qHash(static_cast<Underlying>(value), seed);
}

#endif

} // namespace QtMaterial