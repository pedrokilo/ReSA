"""Prueba negativa del banco Fn607A (CASO-607A90): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Dos rondas de tres mutaciones cada una, sobre partes distintas de la funcion: cada una cambia lo que el
banco observa (filtro por vehiculo, prioridad, signo de la diferencia, acumulador, lista de eventos del decision maker, comparacion del umbral). Los hooks de la DLL estan activos en el juego real antes de que
empiece el banco: ninguna mutacion rompe la partida (solo cambian decisiones de la IA de los peatones).
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
G = 'source/game_sa/'
ES, X87 = G + 'Events/EventScanner.cpp', G + 'X87Math.cpp'
SOURCES = sorted({ES, X87})
NL = chr(10)


def round1():
    return [
        # 1. la prioridad concreta ya no admite el "todas" (-1): el filtro por indice cambia
        (ES, '                if (priority == -1 || priority == id) {' + NL + '                    const uint32 acquaintances = ped.m_acquaintance.GetAcquaintances(id);' + NL + '                    const uint32 pedFlag       = CPedType::GetPedFlag(other->m_nPedType);',
             '                if (priority == id) {' + NL + '                    const uint32 acquaintances = ped.m_acquaintance.GetAcquaintances(id);' + NL + '                    const uint32 pedFlag       = CPedType::GetPedFlag(other->m_nPedType);'),
        # 2. fuera de vista, siguen los que NO van en vehiculo (se invierte la condicion de vehiculo)
        (ES, '            if (!ped.bInVehicle && !other->bInVehicle) {' + NL + '                continue;' + NL + '            }' + NL + '        }' + NL + NL + '        // Relacion',
             '            if (ped.bInVehicle || other->bInVehicle) {' + NL + '                continue;' + NL + '            }' + NL + '        }' + NL + NL + '        // Relacion'),
        # 3. la diferencia candidato - ped pasa a ped - candidato (x)
        (X87, '    o[0] = float(double(other[0]) - double(self[0]));' + NL + '}', '    o[0] = float(double(self[0]) - double(other[0]));' + NL + '}'),
    ]


def round2():
    return [
        # 4. el acumulador deja de cortar en 4 y corta en 3
        (ES, '        if (accumulator == 4) {' + NL + '            continue;' + NL + '        }' + NL + '        CPed* const other = candidates[i];',
             '        if (accumulator == 3) {' + NL + '            continue;' + NL + '        }' + NL + '        CPed* const other = candidates[i];'),
        # 5. el cuarto evento del decision maker cambia (0x28 -> 0x29)
        (ES, 'const int32 events[5]{ 0x24, 0x25, 0x27, 0x28, 0x28 };', 'const int32 events[5]{ 0x24, 0x25, 0x27, 0x29, 0x28 };'),
        # 6. el producto escalar compara con >= en lugar de >
        (X87, '    return dot > double(threshold);' + NL + '}' + NL + '#pragma float_control(pop)', '    return dot >= double(threshold);' + NL + '}' + NL + '#pragma float_control(pop)'),
    ]


ROUNDS = {1: round1(), 2: round2()}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def apply(n):
    GOOD.mkdir(parents=True, exist_ok=True)
    state = GOOD / 'state.json'
    if state.exists():
        raise SystemExit('Hay una ronda aplicada: restaurar antes')
    saved = {}
    for rel in SOURCES:
        dst = GOOD / (rel.replace('/', '__'))
        shutil.copyfile(ROOT / rel, dst)
        saved[rel] = sha(dst)
    state.write_text(json.dumps(saved, indent=2))
    for rel, old, new in ROUNDS[n]:
        p = ROOT / rel
        text = p.read_bytes().decode('utf-8')
        crlf = '\r\n' in text
        norm = text.replace('\r\n', '\n')
        if norm.count(old) != 1:
            restore()
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:80], norm.count(old)))
        if old == new:
            restore()
            raise SystemExit('Mutacion vacia en %s' % rel)
        norm = norm.replace(old, new)
        p.write_bytes((norm.replace('\n', '\r\n') if crlf else norm).encode('utf-8'))
        # varias mutaciones en el mismo archivo se aplican en cadena: la siguiente lee el texto ya mutado
    print('ronda', n, 'aplicada:', len(ROUNDS[n]), 'mutaciones')


def restore():
    state = GOOD / 'state.json'
    saved = json.loads(state.read_text())
    for rel, digest in saved.items():
        src = GOOD / (rel.replace('/', '__'))
        shutil.copyfile(src, ROOT / rel)
        if sha(ROOT / rel) != digest:
            raise SystemExit('Fuente restaurada distinta: ' + rel)
    state.unlink()
    print('restauradas', len(saved), 'fuentes (SHA256 igual a las copias buenas)')


if __name__ == '__main__':
    if sys.argv[1] == 'apply':
        apply(int(sys.argv[2]))
    elif sys.argv[1] == 'restore':
        restore()
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
