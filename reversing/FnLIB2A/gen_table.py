"""Genera tabla.txt (funciones del banco LIB2A con direccion del EXE) y libsyms.inc (direcciones de la biblioteca) a partir de
- protos.py (prototipos de png.h),
- candidatas.csv (nombre,va_exe: pista del mapa LIB1A o direccion confirmada por 'bench --find'),
- confirmadas.csv (nombre,va_exe confirmadas por el banco; tienen prioridad).
Uso: python -I gen_table.py [tanda]   (tanda = nombre del fichero 'tanda<N>.txt' con los nombres a incluir; sin argumento: todas las confirmadas)"""
import csv, re, sys
from pathlib import Path
import protos

HERE = Path(__file__).resolve().parent
P = protos.load()
# especificaciones a mano (cuando la heuristica de protos.spec_of no basta). Letras: P png_ptr, I info_ptr, i entero salvaje, j entero pequeno, b buffer 1 kB (3% NULL), B buffer sin NULL,
# S cadena, o puntero de salida (10% NULL), O puntero de salida sin NULL, d double, f float
OVR = {
    'png_set_bKGD': 'PIB', 'png_set_cHRM': 'PIdddddddd', 'png_set_cHRM_fixed': 'PIiiiiiiii', 'png_set_gAMA': 'PId', 'png_set_gAMA_fixed': 'PIi', 'png_set_hIST': 'PIB',
    'png_set_IHDR': 'PIiijjjjj', 'png_set_oFFs': 'PIiij', 'png_set_pCAL': 'PISiijnSQ', 'png_set_sCAL': 'PIjdd', 'png_set_pHYs': 'PIiij', 'png_set_PLTE': 'PIBN', 'png_set_sBIT': 'PIB',
    'png_set_sRGB': 'PIj', 'png_set_sRGB_gAMA_and_cHRM': 'PIj', 'png_set_iCCP': 'PISjBN', 'png_set_text': 'PITn', 'png_set_tIME': 'PIB', 'png_set_tRNS': 'PIbNb', 'png_set_sPLT': 'PIZn',
    'png_set_unknown_chunks': 'PIUn', 'png_set_unknown_chunk_location': 'PIjj', 'png_set_keep_unknown_chunks': 'PjbN', 'png_set_read_user_chunk_fn': 'Pbi', 'png_set_rows': 'PIB',
    'png_set_compression_buffer_size': 'Pi', 'png_set_invalid': 'PIi',
    'png_get_sPLT': 'PIO', 'png_get_unknown_chunks': 'PIO', 'png_sig_cmp': 'Bjj', 'png_check_sig': 'Bj', 'png_set_sig_bytes': 'Pj',
}
FLOAT_RET = {'png_get_pixel_aspect_ratio'}
MASK = {'png_byte': 0xFF, 'png_uint_16': 0xFFFF, 'png_bytep': 0xFFFFFFFF}
ORACLE = set()   # se rellena desde oraculos.txt
def spec_for(name):
    if name in OVR: return OVR[name]
    ret, args = P[name]
    sp = ''.join(protos.spec_of(a) for a in args)
    if name.startswith('png_get_') and name not in ('png_get_uint_16',): sp = sp.replace('b', 'o')
    return sp
def ret_kind(name):
    ret = P[name][0]
    if 'void' == ret.strip(): return 'v', 0
    if 'float' in ret: return 'f', 0xFFFFFFFF
    return 'u', MASK.get(ret.strip(), 0xFFFFFFFF)

def main():
    conf = {}
    f = HERE / 'confirmadas.csv'
    if f.exists():
        for r in csv.reader(open(f)):
            if r and not r[0].startswith('#'): conf[r[0]] = int(r[1], 16)
    names = list(conf)
    if len(sys.argv) > 1:
        names = [n.strip() for n in (HERE / sys.argv[1]).read_text().split() if n.strip()]
    orc = set((HERE / 'oraculos.txt').read_text().split()) if (HERE / 'oraculos.txt').exists() else set()
    cand = {}
    f = HERE / 'candidatas.csv'
    if f.exists():
        for r in csv.reader(open(f)):
            if r and not r[0].startswith('#'): cand[r[0]] = int(r[1], 16)
    ausentes = set((HERE / 'ausentes.txt').read_text().split()) if (HERE / 'ausentes.txt').exists() else set()
    lines = []
    syms = []
    for n in names:
        if n not in P: print('sin prototipo', n); continue
        ex = 0 if n in ausentes else conf.get(n, cand.get(n, 0))
        rk, mask = ret_kind(n)
        lines.append(f"{n} {spec_for(n) or '-'} {rk} {mask:x} {ex:x} {1 if n in orc else 0}")
        syms.append(n)
    (HERE / 'tabla.txt').write_text('\n'.join(lines) + '\n')
    inc = ['static void libsyms_init() {']
    for n in syms: inc.append(f'    g_libsyms.push_back({{"{n}", (void*){n}}});')
    inc.append('}')
    (HERE / 'libsyms.inc').write_text('\n'.join(inc) + '\n')
    # direcciones de todas las funciones del EXE en la region (para --find)
    rows = list(csv.DictReader(open(HERE / 'exe_funcs.csv')))
    (HERE / 'exe_addrs.txt').write_text('\n'.join(r['va'][2:] for r in rows) + '\n')
    print(len(lines), 'funciones en la tabla')
main()
