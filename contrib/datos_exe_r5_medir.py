"""Mide, sobre los .obj, cuantas direcciones de datos del EXE (.rdata/.data/.bss/_TEXT_HA/_rwdseg/.rsrc) cita el codigo propio por direccion fija.
Uso: python -I contrib/datos_exe_r5_medir.py <refs_codigo.csv> [<otro.csv> ...]   (salida de reversing/FnIND0A/scan_obj.py)
Imprime direcciones y referencias por seccion y tipo (cadena de .rdata, constante, variable, vtable por la lista de vtables)."""
import csv, sys, collections
SEC = [('.rdata', 0x858000, 0x8A4000), ('.data', 0x8A4000, 0x8E4000), ('.bss', 0x8E4000, 0xC9E000), ('_TEXT_HA', 0xC9E000, 0xCAF000), ('_rwdseg', 0xCAF000, 0xCB0000), ('.rsrc', 0xCB0000, 0xCB1000)]
def sec(v):
    for n, a, b in SEC:
        if a <= v < b: return n
for f in sys.argv[1:]:
    dirs = collections.defaultdict(int)
    for r in csv.DictReader(open(f, encoding='utf-8')):
        v = int(r['direccion'], 16); s = sec(v)
        if s: dirs[v] += 1
    por = collections.Counter(sec(v) for v in dirs)
    print(f, 'direcciones', len(dirs), 'referencias', sum(dirs.values()), dict(por))
