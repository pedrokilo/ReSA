"""RW0A: tabla de instalacion por defecto y cobertura de llamadas.

Uso: python reversing/FnRW0A/analyze.py <carpeta_run_con_hooks_state> <carpeta_run> [<carpeta_run> ...]
  - el primer argumento aporta hooks_state.csv / hooks_dump.csv (instalacion medida en memoria);
  - los demas aportan counters.csv (se toma el MAXIMO por funcion entre ejecuciones).
Genera en reversing/FnRW0A/: instalacion.csv, instalacion_region.csv, cobertura.csv, cobertura_por_modulo.csv, no_ejercitadas.csv
"""
import csv
import json
import sys
from collections import Counter, defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
RW = set('RwCore RpWorld RwPlCore RwD3D9 RpSkin RpMatFX ImportThunks RpUVAnim ZlibInflate RtDict D3DX9 RtAnim RpHAnim RtQuat QTMLRuntime DelayLoadThunks RtBMP RtSlerp'.split())
LO, HI = 0x750000, 0x81FFFF


def rd(p):
    with open(p, encoding='utf-8-sig', newline='') as f:
        return list(csv.DictReader(f))


def main():
    base = Path(sys.argv[1])
    runs = [Path(a) for a in sys.argv[2:]]
    dump = {}
    for r in rd(base / 'hooks_dump.csv'):
        if r['class'] in RW:
            dump[int(r['address'], 16)] = r
    state = {int(r['addr'], 16): r for r in rd(base / 'hooks_state.csv')}
    cat = {int(r['address'], 16): r for r in rd(ROOT / 'docs' / 'hooks.csv') if r['class'] in RW and int(r['address'], 16)}
    # contadores: maximo por direccion
    exe, tot = Counter(), Counter()
    for run in runs:
        for line in open(run / 'counters.csv', encoding='utf-8'):
            if line.startswith('#') or line.startswith('addr'):
                continue
            a, inst, e, t = line.strip().split(',')
            a = int(a, 16)
            exe[a] = max(exe[a], int(e))
            tot[a] = max(tot[a], int(t))
    out = []
    for a in sorted(set(dump) | set(cat)):
        d = dump.get(a) or {}
        c = cat.get(a) or {}
        s = state.get(a)
        inreg = LO <= a <= HI
        installed = bool(s and s['installed'] == '1')
        reason = 'instalada (reversed=1, locked=0, enabled por defecto; JMP E9 a la DLL medido en memoria)' if installed else 'NO instalada'
        out.append(dict(clase=c.get('class') or d.get('class'), funcion=c.get('fn_name') or d.get('fn_name'), direccion=f'0x{a:08X}', en_region_750000_81FFFF=int(inreg),
                        reversed=d.get('reversed', ''), locked=d.get('locked', ''), validated=c.get('validated', ''), instalada_en_memoria=int(installed),
                        destino_thunk_incremental=s['thunk'] if s else '', motivo=reason,
                        llamadas_desde_exe=exe[a], llamadas_totales=tot[a] if tot[a] else exe[a], ejercitada=int(exe[a] > 0 or tot[a] > 0)))
    cols = list(out[0].keys())

    def w(name, rows, cols_=None):
        with open(HERE / name, 'w', encoding='utf-8', newline='') as f:
            wr = csv.DictWriter(f, fieldnames=cols_ or list(rows[0].keys()))
            wr.writeheader()
            wr.writerows(rows)
    w('instalacion.csv', [{k: r[k] for k in cols[:10]} for r in out])
    w('cobertura.csv', out)
    w('no_ejercitadas.csv', [r for r in out if not r['ejercitada']])
    # por modulo
    by = defaultdict(lambda: [0, 0, 0, 0])
    for r in out:
        b = by[r['clase']]
        b[0] += 1
        b[1] += r['instalada_en_memoria']
        b[2] += r['ejercitada']
        b[3] += r['llamadas_totales']
    rows = [dict(modulo=k, entradas=v[0], instaladas=v[1], ejercitadas=v[2], no_ejercitadas=v[0] - v[2], llamadas=v[3]) for k, v in sorted(by.items(), key=lambda kv: -kv[1][0])]
    rows.append(dict(modulo='TOTAL', entradas=sum(r['entradas'] for r in rows), instaladas=sum(r['instaladas'] for r in rows),
                     ejercitadas=sum(r['ejercitadas'] for r in rows), no_ejercitadas=sum(r['no_ejercitadas'] for r in rows), llamadas=sum(r['llamadas'] for r in rows)))
    w('cobertura_por_modulo.csv', rows)
    # region: todas las funciones del mapa
    reg = rd(ROOT / 'docs' / 'RenderWareExe' / 'funciones.csv')
    nr = {int(r['va'], 16): r for r in rd(ROOT / 'docs' / 'RenderWareExe' / 'no-reconstruibles.csv')}
    rr = []
    for r in reg:
        a = int(r['va'], 16)
        if a in out_addrs(out):
            st, why = 'enganchada', 'reemplazada por C++ (catalogo): ver instalacion.csv'
        elif a in nr:
            st, why = 'no enganchada', 'no reconstruible en C++ puro: ' + nr[a]['motivo'][:90]
        else:
            st, why = 'no enganchada', 'enlazada: se conserva el codigo original del EXE (libpng, zlib, D3DX9...; no se reconstruye)'
        rr.append(dict(va=r['va'], bytes=r['bytes'], estado_mapa=r['estado'], modulo=r['clase_modulo'], instalacion=st, motivo=why))
    w('instalacion_region.csv', rr)
    print(json.dumps(dict(entradas=len(out), instaladas=sum(r['instalada_en_memoria'] for r in out), ejercitadas=sum(r['ejercitada'] for r in out),
                          region=Counter(r['instalacion'] for r in rr)), ensure_ascii=False))
    for r in rows:
        print(r)


def out_addrs(out, _c={}):
    if 'k' not in _c:
        _c['k'] = {int(r['direccion'], 16) for r in out}
    return _c['k']


if __name__ == '__main__':
    main()
