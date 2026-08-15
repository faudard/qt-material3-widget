#pragma once

#include <cstdint>
#include <type_traits>

namespace QtMaterial {

/**
 * Stable cross-category token namespace.
 *
 * Encoding v1:
 *   bits 31..24 = TokenCategory
 *   bits 23..0  = category-local stable id
 *
 * Raw values are an interoperability/provenance identifier, not an array index.
 */
enum class TokenCategory : std::uint8_t {
    Invalid = 0x00,
    Color = 0x01,
    Typography = 0x02,
    Shape = 0x03,
    Elevation = 0x04,
    Motion = 0x05,
    StateLayer = 0x06,
    Density = 0x07,
    IconSize = 0x08,
    Accessibility = 0x09,
    Interaction = 0x0A
};

class TokenId final {
public:
    using RawType = std::uint32_t;
    static constexpr std::uint32_t EncodingVersion = 1;
    static constexpr RawType LocalMask = 0x00FFFFFFu;

    constexpr TokenId() noexcept = default;

    static constexpr TokenId fromRaw(RawType raw) noexcept
    {
        return TokenId(raw);
    }

    static constexpr TokenId fromParts(
        TokenCategory category,
        RawType localId) noexcept
    {
        return TokenId(
            (static_cast<RawType>(category) << 24u)
            | (localId & LocalMask));
    }

    constexpr RawType raw() const noexcept { return m_raw; }

    constexpr TokenCategory category() const noexcept
    {
        return static_cast<TokenCategory>((m_raw >> 24u) & 0xFFu);
    }

    constexpr RawType localId() const noexcept
    {
        return m_raw & LocalMask;
    }

    constexpr bool isValid() const noexcept
    {
        return category() != TokenCategory::Invalid && localId() != 0u;
    }

    explicit constexpr operator bool() const noexcept { return isValid(); }

    friend constexpr bool operator==(TokenId lhs, TokenId rhs) noexcept
    {
        return lhs.m_raw == rhs.m_raw;
    }

    friend constexpr bool operator!=(TokenId lhs, TokenId rhs) noexcept
    {
        return !(lhs == rhs);
    }

    friend constexpr bool operator<(TokenId lhs, TokenId rhs) noexcept
    {
        return lhs.m_raw < rhs.m_raw;
    }

private:
    explicit constexpr TokenId(RawType raw) noexcept : m_raw(raw) {}
    RawType m_raw = 0u;
};

static_assert(sizeof(TokenId) == sizeof(std::uint32_t),
              "TokenId encoding must remain 32-bit");
static_assert(std::is_trivially_copyable<TokenId>::value,
              "TokenId must remain a trivial value identifier");

} // namespace QtMaterial
