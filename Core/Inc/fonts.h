#include <array>
#include <cstdint>

extern std::array<uint8_t, 64> font_ter_u32b['~' - ' ' + 1];

inline const std::array<uint8_t, 64> &get_font_ter_u32b(char c) {
    return font_ter_u32b[c - ' '];
}
