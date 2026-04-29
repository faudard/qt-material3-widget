#pragma once

#include <QHash>
#include <QtGlobal>
#include <type_traits>

#include "qtmaterial/qtmaterialglobal.h"

namespace QtMaterial {

enum class IconSizeRole {
    ExtraSmall,
    Small,
    Medium,
    Large,
    ExtraLarge
};

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)

inline size_t qHash(IconSizeRole value, size_t seed = 0) noexcept
{
    using Underlying = typename std::underlying_type<IconSizeRole>::type;
    return ::qHash(static_cast<Underlying>(value), seed);
}

#else

inline uint qHash(IconSizeRole value, uint seed = 0) noexcept
{
    using Underlying = typename std::underlying_type<IconSizeRole>::type;
    return ::qHash(static_cast<Underlying>(value), seed);
}

#endif

class QTMATERIAL3_THEME_EXPORT IconSizeTokens {
public:
    IconSizeTokens();
    ~IconSizeTokens();

    bool contains(IconSizeRole role) const;
    int size(IconSizeRole role) const;
    void setSize(IconSizeRole role, int logicalPx);
    void clear();

private:
    QHash<IconSizeRole, int> m_sizes;
};

} // namespace QtMaterial

inline size_t qHash(QtMaterial::IconSizeRole value, size_t seed = 0) noexcept
{
    using Underlying = std::underlying_type_t<QtMaterial::IconSizeRole>;
    return qHash(static_cast<Underlying>(value), seed);
}
