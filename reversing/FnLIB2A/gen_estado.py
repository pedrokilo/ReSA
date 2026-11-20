"""Genera estado_funciones.csv (una fila por funcion probada) desde resultados/corrida_completa.txt, validadas.txt y ausentes.txt."""
import csv, re
from pathlib import Path
HERE = Path(__file__).resolve().parent
SRC = HERE.parents[1] / 'libs' / 'libpng-1.0.12'
MOTIVO = {
    'png_get_pixel_aspect_ratio': 'validada solo con x_pixels_per_unit e y_pixels_per_unit < 2^24: el EXE convierte png_uint_32 a float con fild con signo y sin redondear los operandos; no se engancha',
    'png_read_start_row': 'ninguna funcion del EXE coincide (la de 0x7E1C90 que propone el mapa de LIB1A es otra: lee IDAT con inflate); sin concluir',
    'png_build_gamma_table': 'usa pow() del CRT: el CRT estatico del EXE no esta inicializado en el banco (el EXE falla en 2.743 de 16.384 casos) y los valores de la tabla no son comparables; sin concluir en el banco',
    'png_init_read_transformations': 'igual que png_build_gamma_table (pow del CRT del EXE); sin concluir en el banco',
    'png_get_iCCP': 'no esta en el EXE (el enlazador la descarto)', 'png_get_user_chunk_ptr': 'no esta en el EXE (el enlazador la descarto)',
    'png_check_sig': 'no esta en el EXE (el enlazador la descarto)', 'png_set_sPLT': 'no esta en el EXE (el enlazador la descarto)',
    'png_error': 'validada, pero no se engancha: la ruta por defecto hace longjmp con el jmp_buf del CRT del EXE', 'png_warning': 'validada; no se engancha (infraestructura de errores)',
}
files = {f.name: f.read_text(errors='replace') for f in SRC.glob('png*.c')}
def src_of(n):
    for fn, t in sorted(files.items()):
        if re.search(r'(?m)^' + re.escape(n) + r'\s*\(', t): return fn
    return ''
val = set((HERE / 'validadas.txt').read_text().split())
eng = set((HERE / 'enganchables.txt').read_text().split())
rows = []
for l in (HERE / 'resultados/corrida_completa.txt').read_text().splitlines():
    m = re.match(r'^(\w+)\s+0x([0-9A-F]+)\s+(\d+)\s+(\d+)', l)
    if m:
        n = m.group(1); st = 'validada' if n in val else 'difiere'
        rows.append([n, src_of(n), '0x' + m.group(2), st, m.group(3), m.group(4), 'si' if n in eng else 'no', MOTIVO.get(n, '')])
    elif 'SIN DIRECCION' in l:
        n = l.split()[0]; rows.append([n, src_of(n), '', 'sin_direccion_en_el_exe', '0', '0', 'no', MOTIVO.get(n, '')])
with open(HERE / 'estado_funciones.csv', 'w', newline='', encoding='utf-8') as f:
    w = csv.writer(f); w.writerow(['funcion', 'archivo_fuente', 'va_exe', 'estado', 'pares', 'diferencias', 'enganchada', 'motivo']); w.writerows(rows)
from collections import Counter
print(Counter(r[3] for r in rows), sum(1 for r in rows if r[6] == 'si'))
