"""Prueba negativa del banco Fn121A (lote 121A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez.
"""
import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
TA = 'source/game_sa/Tasks/Allocators/TaskAllocator.cpp'
ES = 'source/game_sa/Tasks/TaskTypes/SeekEntity/PosCalculators/EntitySeekPosCalculator.cpp'
AL = 'source/game_sa/Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocator%s.cpp'
SOURCES = [TA, ES] + [AL % n for n in ('FollowLimited', 'StandStill', 'Chat', 'SitInLeaderCar')]
TA_OLD = '    *reinterpret_cast<uint32*>(self) = 0x870E30;'
ES_OLD = '    *reinterpret_cast<uint32*>(self) = 0x859DC4;'
CT_OLD = '    *reinterpret_cast<uint32*>(this) = %s;\n    return this;'
THIS = 'reinterpret_cast<uintptr_t>(this)'
SELF = 'reinterpret_cast<uintptr_t>(self)'
# El destructor y los constructores los llama el EXE con objetos reales (singletons de 0x851290) o por los funclets de limpieza SEH; las mutaciones solo difieren con valores del banco (this
# desalineado o en posiciones de arena concretas): los objetos del juego estan alineados a 4 y cerca de 0xC09900, y una mutacion que cambie el comportamiento normal tumbaria el juego antes del banco.
ROUNDS = {
    1: [
        # CTaskAllocator: con this desalineado deja la vtable + 4
        (TA, TA_OLD, '    *reinterpret_cast<uint32*>(self) = (%s & 3) != 0 ? 0x870E34 : 0x870E30;' % SELF),
        # CEntitySeekPosCalculator: con this en 0x..C (nibble bajo) deja la vtable de Standard (0x859DC8)
        (ES, ES_OLD, '    *reinterpret_cast<uint32*>(self) = (%s & 0xF) == 0xC ? 0x859DC8 : 0x859DC4;' % SELF),
        # FollowLimited: con this desalineado devuelve this + 4
        (AL % 'FollowLimited', CT_OLD % '0x86C764', '    *reinterpret_cast<uint32*>(this) = 0x86C764;\n    return (%s & 3) != 0 ? reinterpret_cast<CPedGroupDefaultTaskAllocatorFollowLimited*>(%s + 4) : this;' % (THIS, THIS)),
        # StandStill: con this en 0x..3C (mod 0x40) deja la vtable + 4
        (AL % 'StandStill', CT_OLD % '0x86C76C', '    *reinterpret_cast<uint32*>(this) = (%s & 0x3F) == 0x3C ? 0x86C770 : 0x86C76C;\n    return this;' % THIS),
        # Chat: con this == 5 (mod 8) no escribe la vtable
        (AL % 'Chat', CT_OLD % '0x86C774', '    if ((%s & 7) != 5) *reinterpret_cast<uint32*>(this) = 0x86C774;\n    return this;' % THIS),
        # SitInLeaderCar: con this == 2 (mod 4) escribe solo los 16 bits bajos
        (AL % 'SitInLeaderCar', CT_OLD % '0x86C784', '    if ((%s & 3) == 2) *reinterpret_cast<uint16*>(this) = 0xC784; else *reinterpret_cast<uint32*>(this) = 0x86C784;\n    return this;' % THIS),
    ],
    2: [
        # CTaskAllocator: con this == 0x..8 (mod 0x10) escribe un byte menos (los tres bajos)
        (TA, TA_OLD, '    if ((%s & 0xF) == 8) { *reinterpret_cast<uint16*>(self) = 0x0E30; *reinterpret_cast<uint8*>(%s + 2) = 0x87; } else *reinterpret_cast<uint32*>(self) = 0x870E30;' % (SELF, SELF)),
        # CEntitySeekPosCalculator: con this desalineado de resto 3 deja 0x859DC5
        (ES, ES_OLD, '    *reinterpret_cast<uint32*>(self) = (%s & 3) == 3 ? 0x859DC5 : 0x859DC4;' % SELF),
        # FollowLimited: con this == 0xF8..0xFF (mod 0x100) deja la vtable de StandStill
        (AL % 'FollowLimited', CT_OLD % '0x86C764', '    *reinterpret_cast<uint32*>(this) = (%s & 0xF8) == 0xF8 ? 0x86C76C : 0x86C764;\n    return this;' % THIS),
        # StandStill: con this == 1 (mod 4) devuelve this - 1
        (AL % 'StandStill', CT_OLD % '0x86C76C', '    *reinterpret_cast<uint32*>(this) = 0x86C76C;\n    return (%s & 3) == 1 ? reinterpret_cast<CPedGroupDefaultTaskAllocatorStandStill*>(%s - 1) : this;' % (THIS, THIS)),
        # Chat: con this == 0x..4 (mod 0x20) invierte el byte alto de la vtable
        (AL % 'Chat', CT_OLD % '0x86C774', '    *reinterpret_cast<uint32*>(this) = (%s & 0x1F) == 4 ? 0x79C774 : 0x86C774;\n    return this;' % THIS),
        # SitInLeaderCar: con this == 3 (mod 4) escribe la vtable de Chat
        (AL % 'SitInLeaderCar', CT_OLD % '0x86C784', '    *reinterpret_cast<uint32*>(this) = (%s & 3) == 3 ? 0x86C774 : 0x86C784;\n    return this;' % THIS),
    ],
}


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
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:60], norm.count(old)))
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
