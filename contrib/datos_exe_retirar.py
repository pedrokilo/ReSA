"""Retira de DatosExe los objetos que el codigo del EXE que SI se ejecuta todavia toca (DE04-R / DE05-R generalizado).

Un objeto definido en source/game_sa/DatosExe (DataBss*, DataInicializada*) vuelve a ser `StaticRef<T>(dir)` si, con el escaner
actual de datos_exe.py (funciones vivas = no reemplazadas por un gancho ACTIVO, huecos alcanzables, ganchos inactivos revividos),
alguna funcion viva lo lee, lo escribe o le toma la direccion. Se quita su definicion y su declaracion y los consumidores vuelven a
la forma original (el tipo sale del propio consumidor o de docs/DatosExe/lotes/*.csv). La lista queda en docs/DatosExe/lotes/DE05R2.csv.

Uso: python contrib/datos_exe_retirar.py [--aplicar]   (con DATOSEXE_HUECOS=alcanzables para medir con los huecos muertos fuera)
"""
import csv
import os
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import datos_exe as de  # noqa: E402
from datos_exe_lote import RAIZ  # noqa: E402
from pe_inventory import PE  # noqa: E402

NL = chr(10)


def main():
    aplicar = '--aplicar' in sys.argv
    pe = PE(Path('D:/Grand Theft Auto San Andreas/gta-sa.exe'))
    reempl, rangos = de.cargar_funciones()
    cod = de.escanear_exe(pe, reempl, rangos)
    defin = de.definidas_datosexe()
    # tipo por direccion (de los CSV de los lotes) y archivo de definicion
    tipo = {}
    lote_de = {}
    for p in sorted((RAIZ / 'docs/DatosExe/lotes').glob('DE0[45]*.csv')):
        try:
            for r in csv.DictReader(p.open(encoding='utf-8')):
                if 'va' in r and 'tipo' in r:
                    tipo[int(r['va'], 16)] = r['tipo']
                    lote_de[int(r['va'], 16)] = p.stem
        except Exception:
            pass
    # extension aproximada de cada objeto definido (bytes del CSV o 4) para detectar lecturas dentro de arrays
    ext = {}
    for p in sorted((RAIZ / 'docs/DatosExe/lotes').glob('DE0[45]*.csv')):
        try:
            for r in csv.DictReader(p.open(encoding='utf-8')):
                if 'va' in r and 'bytes' in r and r['bytes'].isdigit():
                    ext[int(r['va'], 16)] = int(r['bytes'])
        except Exception:
            pass
    ret = []
    vivos = sorted((a, c) for a, c in cod.items() if any(c['funcs'].values()))
    import bisect
    claves = [a for a, _ in vivos]
    for va in sorted(defin):
        n = ext.get(va, 4)
        i = bisect.bisect_left(claves, va)
        hit = None
        while i < len(claves) and claves[i] < va + max(n, 1):
            hit = claves[i]
            break
        if hit is not None:
            fs = [f"0x{f:X}" for f, v in vivos[i][1]['funcs'].items() if v][:3]
            ret.append((va, hit, fs))
    # campos de una estructura mayor que el C++ tambien referencia suelta (TheCamera, objetos de pools...): no son objetos independientes
    refs = de.escanear_fuente('source')
    cubre = de.cubierto_por_vecino(refs)
    ya = {r[0] for r in ret}
    for va in sorted(defin):
        s = cubre(va)
        if s is not None and va not in ya:
            ret.append((va, s, [f'campo de 0x{s:X}']))
    # direcciones que leen las pruebas de humo (HARNESS de DE04/DE05 y sus rangos): se quedan en el EXE
    import datos_exe_de05 as d5
    import datos_exe_de04 as d4
    for va in sorted(defin):
        n = ext.get(va, 4)
        if va in d4.HARNESS or any(va <= h < va + n for h in d5.HARNESS) or any(a < va + n and va < b for a, b in d5.HARNESS_RANGOS):
            if va not in {r[0] for r in ret}:
                ret.append((va, va, ['humo']))
    print(f'objetos definidos en DatosExe: {len(defin)}; con codigo vivo del EXE: {len(ret)}')
    for va, hit, fs in ret[:60]:
        print(f'  0x{va:08X} (toca 0x{hit:08X}) {lote_de.get(va, "?")} {tipo.get(va, "?")[:40]}  por {" ".join(fs)}')
    with (RAIZ / 'docs/DatosExe/lotes/DE05R2.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['va', 'lote', 'tipo', 'toca', 'funciones_vivas'])
        for va, hit, fs in ret:
            w.writerow([f'0x{va:08X}', lote_de.get(va, ''), tipo.get(va, ''), f'0x{hit:08X}', ' '.join(fs)])
    if not aplicar or not ret:
        return
    quitar = {va for va, _, _ in ret}
    d = RAIZ / 'source/game_sa/DatosExe'
    # 1) quitar definiciones y declaraciones (cualquier linea que mencione la direccion en una forma generada)
    for f in sorted(d.glob('DataBss*')) + sorted(d.glob('DataInicializada*')):
        txt = f.read_text(encoding='utf-8')
        lineas = txt.split(NL)
        nuevas = []
        for ln in lineas:
            drop = False
            for va in quitar:
                h = f'{va:X}'
                if re.search(r'(?<![0-9A-Fa-f])(?:0x0*%s|[bvk]%s|sl_%s|dt_%s|Ty_%s)(?![0-9A-Fa-f])' % (h, h, h, h, h), ln):
                    drop = True
                    break
            if not drop:
                nuevas.append(ln)
        if nuevas != lineas:
            f.write_text(NL.join(nuevas), encoding='utf-8', newline=NL)
    # 2) consumidores
    pat = re.compile(r'DatosExe::(?:Bss::b|Data::v)([0-9A-F]{6})(?:<(.*?)>\(\))?(?![0-9A-Za-z_])')
    for dp, _, fs in os.walk(RAIZ / 'source'):
        if os.path.basename(dp) == 'DatosExe':
            continue
        for f in fs:
            if not f.endswith(('.cpp', '.h', '.hpp', '.inc')):
                continue
            p = Path(dp) / f
            raw = p.read_bytes()
            t = raw.decode('utf-8', errors='surrogateescape')
            if 'DatosExe::' not in t:
                continue

            def sub(m):
                va = int(m.group(1), 16)
                if va not in quitar:
                    return m.group(0)
                ty = m.group(2) or tipo.get(va)
                if not ty:
                    raise SystemExit(f'sin tipo para 0x{va:X} en {p}')
                return f'StaticRef<{ty}>(0x{va:X})'
            n = pat.sub(sub, t)
            if n != t:
                p.write_bytes(n.encode('utf-8', errors='surrogateescape'))
    print('retirados', len(quitar))


if __name__ == '__main__':
    main()
