from freetype import *
import numpy as np

def main():
    print('#include <array>')
    print('#include <cstdint>')
    print('extern const std::array<uint8_t, 64> font_ter_u32b[] = {')

    face = Face('fonts/ter-u32b.bdf')
    face.set_pixel_sizes(width=0, height=32)

    for i in range(ord(' '), ord('~') + 1):
        face.load_char(chr(i), FT_LOAD_RENDER | FT_LOAD_TARGET_MONO)
        bitmap = face.glyph.bitmap
        buffer = np.array(bitmap.buffer, dtype=np.uint8).reshape(bitmap.rows, bitmap.pitch)
        buffer = np.unpackbits(buffer, axis=1)[:,:bitmap.width]
        buffer = np.packbits(buffer, axis=0, bitorder='little')
        buffer = buffer.T.tobytes()
        print('    {' + ','.join(['0x{:02x}'.format(x) for x in buffer]) + f'}}, // [{chr(i)}]')

    print('};')

if __name__ == '__main__':
    main()
