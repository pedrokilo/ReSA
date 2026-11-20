"""Extrae de libs/libpng-1.0.12/png.h los prototipos (PNG_EXPORT y PNG_EXTERN) y calcula la especificacion de argumentos del banco LIB2A."""
import re, json
from pathlib import Path
PNG_H = Path(__file__).resolve().parents[2] / 'libs' / 'libpng-1.0.12' / 'png.h'
def load():
    t = PNG_H.read_text(errors='replace')
    t = re.sub(r'/\*.*?\*/', ' ', t, flags=re.S)
    out = {}
    for m in re.finditer(r'PNG_EXPORT\(\s*([^,]+?)\s*,\s*(\w+)\s*\)\s*PNGARG\(\((.*?)\)\)\s*;', t, re.S):
        out[m.group(2)] = (' '.join(m.group(1).split()), args(m.group(3)))
    for m in re.finditer(r'PNG_EXTERN\s+([\w\s\*]+?)\s*\b(\w+)\s+PNGARG\(\((.*?)\)\)\s*;', t, re.S):
        out[m.group(2)] = (' '.join(m.group(1).split()), args(m.group(3)))
    return out
def args(s):
    s = ' '.join(s.split())
    if s in ('void', ''): return []
    return [a.strip() for a in s.split(',')]
def spec_of(a):
    a = a.replace('FAR', '').replace('const', '')
    a = ' '.join(a.replace('*', ' * ').split())
    stars = a.count('*')
    base = a.replace('*', ' ').split()
    typ = ' '.join(base[:-1]) if len(base) > 1 else base[0]
    if typ == 'png_structp' and stars == 0: return 'P'
    if typ == 'png_infop' and stars == 0: return 'I'
    if typ == 'png_infopp' or (typ == 'png_infop' and stars == 1): return 'o'
    if typ == 'png_structpp' or (typ == 'png_structp' and stars == 1): return 'o'
    if typ == 'double' and stars == 0: return 'd'
    if typ == 'float' and stars == 0: return 'f'
    if stars == 0:
        if typ in ('png_voidp', 'png_charp', 'png_bytep', 'png_colorp', 'png_color_8p', 'png_color_16p', 'png_timep', 'png_textp',
                   'png_sPLT_tp', 'png_uint_16p', 'png_unknown_chunkp', 'png_bytepp', 'png_charpp', 'png_row_infop', 'png_const_charp', 'png_const_bytep', 'png_const_voidp'):
            return 'b'
        return 'i'
    if stars == 1 and typ in ('png_byte', 'png_charp', 'png_bytep', 'png_voidp', 'void', 'png_char', 'png_color', 'png_color_8', 'png_color_16', 'png_time', 'png_text', 'png_sPLT_t', 'png_unknown_chunk', 'png_uint_16', 'png_row_info'):
        return 'b'
    return 'o'
if __name__ == '__main__':
    p = load(); print(len(p))
    for k in ['png_get_IHDR', 'png_get_text', 'png_crc_read', 'png_get_uint_16', 'png_sig_cmp', 'png_set_text', 'png_get_cHRM']:
        print(k, p.get(k), ''.join(spec_of(a) for a in p.get(k, ('', []))[1]))
