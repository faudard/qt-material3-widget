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
