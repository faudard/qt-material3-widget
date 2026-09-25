#pragma once

namespace QtMaterial {

/**
 * Internal stable QObject dynamic-property names used by runtime widgets.
 *
 * This type deliberately has no Qt dependency.
 */
struct QtMaterialMetadataPropertyNames final
{
    static constexpr const char* component() noexcept
    {
        return "materialComponent";
    }

    static constexpr const char* variant() noexcept
    {
        return "materialVariant";
    }

    static constexpr const char* role() noexcept
    {
        return "materialRole";
    }

    static constexpr const char* testId() noexcept
    {
        return "materialTestId";
    }

    static constexpr const char* state() noexcept
    {
        return "materialState";
    }
};

} // namespace QtMaterial
