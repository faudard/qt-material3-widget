#pragma once

#include <cstdint>

namespace QtMaterial {

/**
 * Stable first-party component/variant identifiers.
 *
 * These values are identifiers, never dense array indexes.
 */
enum class ComponentId : std::uint32_t {
    Button = 0x00000100u,
    ButtonText = 0x00000101u,
    ButtonFilled = 0x00000102u,
    ButtonFilledTonal = 0x00000103u,
    ButtonOutlined = 0x00000104u,
    ButtonElevated = 0x00000105u,

    IconButton = 0x00000200u,
    IconButtonStandard = 0x00000201u,
    IconButtonFilled = 0x00000202u,
    IconButtonFilledTonal = 0x00000203u,
    IconButtonOutlined = 0x00000204u,

    FloatingActionButton = 0x00000300u,
    SmallFloatingActionButton = 0x00000301u,
    LargeFloatingActionButton = 0x00000302u,
    ExtendedFloatingActionButton = 0x00000303u,

    Checkbox = 0x00000400u,
    RadioButton = 0x00000500u,
    Switch = 0x00000600u,
    Dialog = 0x00000700u,
    NavigationDrawer = 0x00000800u,
    BottomSheet = 0x00000900u,
    Banner = 0x00000A00u,
    Card = 0x00000B00u,
    TopAppBar = 0x00000C00u,
    BottomAppBar = 0x00000D00u,

    Input = 0x00000E00u,
    TextField = 0x00000E10u,
    TextFieldOutlined = 0x00000E11u,
    TextFieldFilled = 0x00000E12u,
    AutoComplete = 0x00000E20u,
    AutoCompletePopup = 0x00000E21u,
    DateField = 0x00000E30u,

    List = 0x00001000u,
    ListItem = 0x00001001u,
    Divider = 0x00001100u,
    Tabs = 0x00001200u,
    Snackbar = 0x00001300u,
    ProgressIndicator = 0x00001400u,
    NavigationRail = 0x00001500u,
    Surface = 0x00001600u,

    Custom = 0xFFFF0000u
};

} // namespace QtMaterial
