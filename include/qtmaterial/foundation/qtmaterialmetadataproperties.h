#pragma once

namespace QtMaterial {

/**
 * Stable QObject dynamic-property names shared by runtime widgets and
 * optional automation/testing utilities.
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
