"""Registra las seis funciones matematicas de D3DX9 de EXT1A con la evidencia del banco FUERA DEL JUEGO (bench.cpp: EXE original mapeado contra d3dx9_43.dll; la de
MatrixMultiplyTranspose contra la rutina propia). Idempotente.

Uso:  python reversing/FnEXT1A/register_hook.py
Lee reversing/FnEXT1A/resultados/corrida_sse2.txt y corrida_sse.txt (0 diferencias en clase normal y de borde, 0 fallos del oraculo, 0 diferencias de retorno, 0 guardas,
sin NaN de diferencia en MatrixMultiplyTranspose), archiva la evidencia en reversing/CppTwelveLevel6Final/runs/FnEXT1A-<fecha>/ y anade las seis filas al registro de validaciones.
Alcance: evidencia fuera del juego (no hay corrida dentro del juego); se anota en `scope`.
"""
import csv, gzip, hashlib, io, json, shutil, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'contrib'))
import registry_store

BANK = 'FnEXT1A'
RES = ROOT / 'reversing' / BANK / 'resultados'
SRC = 'source/game_sa/RenderWare/rw/d3dx9math.cpp'
FUNCS = (  # (n del banco, direccion, nombre del hook)
    (1, '0x00766A32', 'D3DXVec3TransformCoord'),
    (2, '0x00766AB7', 'D3DXVec3TransformNormal'),
    (3, '0x007672E1', 'D3DXMatrixMultiply'),
    (4, '0x007675EE', 'D3DXMatrixTranspose'),
    (5, '0x007676C7', 'D3DXMatrixMultiplyTranspose'),
    (6, '0x0076626B', 'D3DXFloat16To32Array'),
)
SCOPE = ('Banco par a par FUERA DEL JUEGO (EXE original mapeado en un proceso de 32 bits, solo lectura, contra d3dx9_43.dll del sistema; MatrixMultiplyTranspose contra la rutina propia '
         'D3DX9MatrixMultiplyTransposeSse.h): 20.000 pares normales + 4.000 de borde (inf, NaN, denormales, 3e38) por funcion, con alias de salida y desalineos, oraculo independiente en Python (doble precision), '
         'mutaciones de 2 rondas (12 de 12 detectadas). Variante del EXE: la que elige su selector en CPU con SSE2/SSE (identicas entre si); la x87 de reserva no se prueba como destino. '
         'En D3DXVec3TransformCoord las salidas coinciden bit a bit salvo el signo/carga util del NaN en 31 de 4.000 casos de borde con NaN en ambas. Sin corrida dentro del juego.')


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def parse(path):
    rows = {}
    for line in Path(path).read_text(encoding='utf-8').splitlines():
        f = line.split(',')
        if len(f) >= 16 and f[0].isdigit():
            rows[int(f[0])] = [int(x) for x in f[2:]]
    return rows


def main():
    sse2, sse = parse(RES / 'corrida_sse2.txt'), parse(RES / 'corrida_sse.txt')
    covered, pairs = {}, {}
    for n, addr, name in FUNCS:
        for r in (sse2[n], sse[n]):
            cas0, dif0, cas1, dif1, nan, orc, orfe, orfd, tolf, retd, guard, fault = r[:12]
            ok = dif0 == 0 and dif1 == 0 and orfe == 0 and orfd == 0 and retd == 0 and guard == 0 and fault == 0 and cas0 >= 16384 and orc >= 16384
            if n == 5:
                ok = ok and nan == 0
            covered[name] = covered.get(name, True) and ok
        pairs[name] = sse2[n][0] + sse2[n][2]
    stamp = '20261119-bench'
    run = ROOT / 'reversing/CppTwelveLevel6Final/runs' / f'{BANK}-{stamp}'
    run.mkdir(parents=True, exist_ok=True)
    for n in ('corrida_sse2.txt', 'corrida_sse.txt', 'corrida_x87.txt', 'negativas.txt', 'sha256.txt'):
        shutil.copyfile(RES / n, run / n)
    shutil.copyfile(RES / 'corrida_sse2.txt', run / 'RESULTADO.txt')
    summary = dict(passed=all(covered.values()), covered_functions=covered, pairs_per_function=pairs, pairs=sum(pairs.values()), out_of_game=True,
                   exe_bytes=5189632, bench_sha256=sha256_of_line(RES / 'sha256.txt', 'bench.exe'), cases_sha256=sha256_of_line(RES / 'sha256.txt', 'cases.bin'))
    (run / 'summary.json').write_bytes((json.dumps(summary, indent=2) + '\n').encode('utf-8'))
    assert summary['passed'], covered
    names = {f[2] for f in FUNCS}
    tested = {SRC: sha(ROOT / SRC), 'source/game_sa/RenderWare/rw/D3DX9MatrixMultiplyTransposeSse.h': sha(ROOT / 'source/game_sa/RenderWare/rw/D3DX9MatrixMultiplyTransposeSse.h')}
    ev_path = (run / 'summary.json').relative_to(ROOT).as_posix()
    # Se edita en el sitio (sin repartir de nuevo todo el registro, que movia ~90.000 lineas): quitar las entradas previas de estas seis y anadirlas al ultimo fragmento.
    last = None
    for part_path in registry_store.part_paths(ROOT):
        part = json.loads(part_path.read_bytes().decode('utf-8'))
        kept = [r for r in part['functions'] if not (r['hook']['class'] == 'D3DX9' and r['hook']['fn_name'] in names)]
        if len(kept) != len(part['functions']):
            part['functions'] = kept
            part_path.write_bytes(registry_store._dumps(part))
        last = (part_path, part)
    part_path, part = last
    for n, addr, name in FUNCS:
        hook = {'class': 'D3DX9', 'fn_name': name, 'address': addr, 'type': 'S'}
        part['functions'].append(dict(hook=hook, evidence={'path': ev_path, 'sha256': sha(run / 'summary.json'), 'passed_path': ['covered_functions', name], 'pairs': pairs[name]},
                                      tested_sources=tested, dependency_review=None,
                                      source_provenance=('Rutina propia contrastada con el EXE canonico (banco fuera del juego).' if n == 5 else 'Envoltorio sobre la exportacion de d3dx9_43.dll contrastado con el EXE canonico (banco fuera del juego).'),
                                      scope=SCOPE, review_reference=f'reversing/{BANK}/README.md'))
    part_path.write_bytes(registry_store._dumps(part))
    print('ok', run)


def sha256_of_line(path, name):
    for line in Path(path).read_text(encoding='utf-8').splitlines():
        if line.rstrip().endswith(name):
            return line.split()[0]
    return None


if __name__ == '__main__':
    main()
