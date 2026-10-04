#pragma once

namespace QtMaterial {

enum class WindowWidthSizeClass
{
    Compact,
    Medium,
    Expanded,
    Large,
    ExtraLarge
};

enum class WindowHeightSizeClass
{
    Compact,
    Medium,
    Expanded
};

struct WindowSizeClass
{
    WindowWidthSizeClass width = WindowWidthSizeClass::Compact;
    WindowHeightSizeClass height = WindowHeightSizeClass::Compact;

    static constexpr WindowWidthSizeClass classifyWidth(int width) noexcept
    {
        return width < 600
            ? WindowWidthSizeClass::Compact
            : width < 840
                ? WindowWidthSizeClass::Medium
                : width < 1200
                    ? WindowWidthSizeClass::Expanded
                    : width < 1600
                        ? WindowWidthSizeClass::Large
                        : WindowWidthSizeClass::ExtraLarge;
    }

    static constexpr WindowHeightSizeClass classifyHeight(int height) noexcept
    {
        return height < 480
            ? WindowHeightSizeClass::Compact
            : height < 900
                ? WindowHeightSizeClass::Medium
                : WindowHeightSizeClass::Expanded;
    }

    static constexpr WindowSizeClass fromLogicalSize(int width, int height) noexcept
    {
        return {classifyWidth(width), classifyHeight(height)};
    }
};

constexpr bool operator==(WindowSizeClass lhs, WindowSizeClass rhs) noexcept
{
    return lhs.width == rhs.width && lhs.height == rhs.height;
}

constexpr bool operator!=(WindowSizeClass lhs, WindowSizeClass rhs) noexcept
{
    return !(lhs == rhs);
}

} // namespace QtMaterial
