"""Prueba negativa del banco Fn239A (lote 239A): aplica (o revierte) una ronda de mutaciones a la fuente de produccion (RunningScript.cpp).

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cinco mutaciones por ronda (una sola funcion): cambian lo que el banco observa
(valores guardados, banderas escritas, valor de retorno de un comando). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a la marca que
solo pone el banco en ScriptParams[31] (0xBA9C0239; el juego normal no la deja ahi).
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
RS = 'source/game_sa/Scripts/RunningScript.cpp'
SOURCES = [RS]
MK = '(ScriptParams[31].uParam == 0xBA9C0239u)'   # marca del banco


def round1():
    m = []
    m.append((RS, 'CWorld::Players[ScriptParams[0].iParam].m_bDoesNotGetTired = ScriptParams[1].iParam != 0;', 'CWorld::Players[ScriptParams[0].iParam].m_bDoesNotGetTired = (ScriptParams[1].iParam != 0) != ' + MK + ';'))   # 816
    m.append((RS, 'CGarages::RespraysAreFree = ScriptParams[0].iParam != 0;', 'CGarages::RespraysAreFree = (ScriptParams[0].iParam != 0) != ' + MK + ';'))                                            # 821
    m.append((RS, 'ScriptParams[0].iParam = gFireManager.StartScriptFire(pos, ped, 0.8f, 1, 0, 1);', 'ScriptParams[0].iParam = gFireManager.StartScriptFire(pos, ped, 0.8f, 1, 0, 1) + (' + MK + ' ? 1 : 0);'))     # 806
    m.append((RS, 'obj->m_bUsesCollision = ScriptParams[1].iParam != 0;', 'obj->m_bUsesCollision = (ScriptParams[1].iParam != 0) != ' + MK + ';'))                                                       # 898
    m.append((RS, 'const float    vz  = ScriptParams[3].fParam * 0.02f;', 'const float    vz  = ScriptParams[3].fParam * (' + MK + ' ? 0.03f : 0.02f);'))                                                    # 897
    return m


def round2():
    m = []
    m.append((RS, 'ped->bPedIsBleeding = ScriptParams[1].iParam != 0;', 'ped->bPedIsBleeding = (ScriptParams[1].iParam != 0) != ' + MK + ';'))                                                             # 818
    m.append((RS, 'line.Pos.x = ScriptParams[0].fParam;', 'line.Pos.x = ScriptParams[0].fParam + (' + MK + ' ? 1.0f : 0.0f);'))                                                                              # 830
    m.append((RS, 'ped->m_fArmour = (ped->m_fArmour > 0.0f) ? ped->m_fArmour : 0.0f;', 'ped->m_fArmour = ((ped->m_fArmour > 0.0f) ? ped->m_fArmour : 0.0f) + (' + MK + ' ? 1.0f : 0.0f);'))             # 863
    m.append((RS, 'heading += 360.0f;', 'heading += (' + MK + ' ? 361.0f : 360.0f);'))                                                                                                                          # 845
    m.append((RS, 'CPopulation::ms_nTotalMissionPeds = CPopulation::ms_nTotalMissionPeds + 1;', 'CPopulation::ms_nTotalMissionPeds = CPopulation::ms_nTotalMissionPeds + (' + MK + ' ? 2 : 1);'))     # 886
    return m


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
