"""Resumen de una ronda de la prueba negativa de Fn607A (CASO-607A90): cuantos pares tienen diferencias EXE/DLL y, con el oraculo independiente, POR QUE.

    python -I neg_summary.py <ronda 1|2> <carpeta-de-resultados-de-la-ronda> <carpeta-de-salida>

Para cada par: el lado EXE debe coincidir con el oraculo (fixture_oracle.expected); el lado DLL (mutado) debe coincidir con el oraculo que modela las mutaciones de la ronda (expected(entrada, mutaciones)), lo que prueba
que la DLL mutada hace exactamente lo previsto; y por cada mutacion se cuentan los pares en que esa mutacion SOLA cambia lo observable (eventos, bytes o valor devuelto) respecto del EXE: son los pares en que el banco
la detecta. Escribe diferencias-por-funcion.txt en la carpeta de salida y copia RESULTADO.txt y summary.json (el comparisons.jsonl crudo no se archiva: solo 2048 casos).
"""
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle  # noqa: E402

ROUNDS = {1: (1, 2, 3), 2: (4, 5, 6)}
DESCRIPTIONS = {
    1: 'la prioridad -1 ya no recorre los indices de acquaintances (`priority == id` en lugar de `priority == -1 || priority == id`)',
    2: 'fuera de vista siguen los que NO van en vehiculo (condicion de vehiculo invertida)',
    3: 'la componente x de la diferencia es ped - candidato en lugar de candidato - ped (AcquaintanceDeltaCpp)',
    4: 'el acumulador corta en 3 en lugar de 4 (se saltan CanPedTargetPed y 0x607560)',
    5: 'el cuarto evento de la lista del decision maker es 0x29 en lugar de 0x28',
    6: 'el producto escalar compara con >= en lugar de > (AcquaintanceInViewCpp)',
}


def observable(e):
    return json.dumps([e['events'], e['changes'], e['gchanges'], e['ret']], sort_keys=True)


def main(rnd, src, dst):
    muts = ROUNDS[int(rnd)]
    src, dst = Path(src), Path(dst)
    dst.mkdir(parents=True, exist_ok=True)
    total = bad = exe_off_oracle = dll_off_mutated_oracle = 0
    sensitivity = {m: 0 for m in muts}
    for line in (src / 'comparisons.jsonl').read_text(encoding='utf-8').splitlines():
        if '"event":"comparison"' not in line:
            continue
        r = json.loads(line)
        total += 1
        inp = r['input']
        good = oracle.expected(inp)
        mutated = oracle.expected(inp, frozenset(muts))
        if r['original'] != r['candidate'] or not all(r['original'].get(k) is True for k in ('route_ok', 'collaborators_dll')):
            bad += 1
        if oracle.check_side(inp, r['original']['observed'], good, r['original']):
            exe_off_oracle += 1
        if oracle.check_side(inp, r['candidate']['observed'], mutated, r['candidate']):
            dll_off_mutated_oracle += 1
        for m in muts:
            if observable(oracle.expected(inp, frozenset([m]))) != observable(good):
                sensitivity[m] += 1
    text = ('pares %d con diferencias EXE/DLL %d\nlado EXE fuera del oraculo %d\nlado DLL mutado fuera del oraculo con las mutaciones de la ronda %d\n' % (total, bad, exe_off_oracle, dll_off_mutated_oracle)
            + 'mutaciones de la ronda y pares en que cada una SOLA cambia lo observable (el banco la detecta):\n'
            + ''.join('  %d (%s): %d\n' % (m, DESCRIPTIONS[m], sensitivity[m]) for m in muts))
    (dst / 'diferencias-por-funcion.txt').write_text(text, encoding='utf-8')
    for n in ('RESULTADO.txt', 'summary.json'):
        shutil.copyfile(src / n, dst / n)
    print(text)


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2], sys.argv[3])
