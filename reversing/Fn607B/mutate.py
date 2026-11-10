"""Prueba negativa del banco Fn607B (COLAB-607A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Dos rondas de tres mutaciones cada una (una por funcion y ronda), sobre partes distintas de cada funcion: cada una cambia lo que el
banco observa (tipos excluidos, tarea de la decision, indice de visibilidad, signo de Respects, decision maker de mision, intervalo del temporizador). Los hooks de la DLL estan activos en el juego real antes de que
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
ES, DMT = G + 'Events/EventScanner.cpp', G + 'DecisionMakers/DecisionMakerTypes.cpp'
SOURCES = sorted({ES, DMT})
NL = chr(10)


def round1():
    return [
        # 1. CanJoinRiotAgainst: los bomberos (tipo 19) dejan de estar excluidos
        (ES, '    if (type == PED_TYPE_COP || type == PED_TYPE_MEDIC || type == PED_TYPE_FIREMAN) {', '    if (type == PED_TYPE_COP || type == PED_TYPE_MEDIC) {'),
        # 2. HasResponseToAnyEvent: la sexta tarea de la decision deja de comprobarse
        (DMT, '        for (int32 k = 0; k < CDecision::MAX_NUM_CHOICES; ++k) {', '        for (int32 k = 0; k < CDecision::MAX_NUM_CHOICES - 1; ++k) {'),
        # 3. FindAcquaintance: la visibilidad no positiva solo se admite con el indice 3 en lugar del 4
        (ES, '            if (id != 4 || visibility == 0.0f) {', '            if (id != 3 || visibility == 0.0f) {'),
    ]


def round2():
    return [
        # 4. CanJoinRiotAgainst: contra el jugador se devuelve que lo respeta (sin negar)
        (ES, '        return !ped->GetIntelligence()->Respects(other);', '        return ped->GetIntelligence()->Respects(other);'),
        # 5. HasResponseToAnyEvent: el decision maker de un peaton -1 se elige al reves (mision / aleatorio)
        (DMT, 'maker = ped->GetCreatedBy() == PED_MISSION ? &m_DefaultMissionPedDecisionMaker : &m_DefaultRandomPedDecisionMaker;', 'maker = ped->GetCreatedBy() != PED_MISSION ? &m_DefaultMissionPedDecisionMaker : &m_DefaultRandomPedDecisionMaker;'),
        # 6. FindAcquaintance: el intervalo corto se elige con la visibilidad positiva en lugar de la negativa
        (ES, '(*outB == 4 && visibility < 0.0f) ? StaticRef<int32>(0x8D2360)', '(*outB == 4 && visibility > 0.0f) ? StaticRef<int32>(0x8D2360)'),
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
