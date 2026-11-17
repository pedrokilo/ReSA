"""Prueba negativa del banco Fn214A (lote 214A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cambia lo que el banco observa (bytes escritos, eventos de los falsos, memoria global).
Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a la marca 0x5A en 0xA9B0B0 (solo la pone el banco; el juego real nunca la escribe).
Ronda 1: una mutacion distinta por funcion (7); ronda 2: otra mutacion distinta por funcion (7). CPlayerInfo::operator= pasa por el operator= REAL de CPlayerPedData, asi que las mutaciones de este
tambien las ve la funcion 1, pero cada funcion lleva ademas una mutacion propia.
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
NL = chr(10)
FLAG = '(*reinterpret_cast<volatile unsigned char*>(0xA9B0B0) == 0x5A)'
PED, INFO, REP, RBT, TSC, STR, X87 = G + 'PlayerPedData.cpp', G + 'PlayerInfo.cpp', G + 'Replay.cpp', G + 'ReplayBlockType.cpp', G + 'Scripts/TheScripts.cpp', G + 'Streaming.cpp', G + 'X87Math.cpp'
SOURCES = sorted({PED, INFO, REP, RBT, TSC, STR, X87})


def round1():
    out = []
    # 0 CPlayerPedData::operator=: no copia +0xA8 con la marca
    out.append((PED, '    copy(0xA4, 0x08); // 0xA4, 0xA8', '    copy(0xA4, ' + FLAG + ' ? 0x04 : 0x08); // 0xA4, 0xA8'))
    # 1 CPlayerInfo::operator=: no copia +0x18C con la marca
    out.append((INFO, '    copy(0x18C, 4);   // m_nRequireParachuteTimer', '    copy(0x18C, ' + FLAG + ' ? 0 : 4);   // m_nRequireParachuteTimer'))
    # 2 guardado del paquete de tren: byte de via distinto con la marca
    out.append((REP, '    p[0x48] = v[0x5C0];', '    p[0x48] = ' + FLAG + ' ? static_cast<uint8>(v[0x5C0] + 1) : v[0x5C0];'))
    # 3 lectura del paquete de tren: byte de via distinto con la marca
    out.append((RBT, '    train.m_nTrackId = trackId;', '    train.m_nTrackId = ' + FLAG + ' ? static_cast<uint8>(trackId ^ 1) : trackId;'))
    # 4 AddToSuppressedCarModels: modelo distinto con la marca
    out.append((TSC, '    models[slot] = carModelId;', '    models[slot] = ' + FLAG + ' ? static_cast<eModelID>(static_cast<int32>(carModelId) + 1) : carModelId;'))
    # 5 SetMissionDoesntRequireAnim: argumento distinto con la marca
    out.append((STR, '    SetMissionDoesntRequireModel(IFPToModelId(slot));', '    SetMissionDoesntRequireModel(IFPToModelId(slot) + (' + FLAG + ' ? 1 : 0));'))
    # 6 Normalise: segundo componente distinto con la marca
    out.append((X87, '    q[1] = float(inv * q[1]);', '    q[1] = float(inv * q[1] + (' + FLAG + ' ? 1.0f : 0.0f));'))
    return out


def round2():
    out = []
    out.append((PED, '        uint32 v = ((oldFlags ^ esi) & 0x200u) ^ esi;', '        uint32 v = ((oldFlags ^ esi) & (' + FLAG + ' ? 0x400u : 0x200u)) ^ esi;'))   # 0
    out.append((INFO, '    copy(0x164, 0x20);// m_szSkinName entero (el EXE copia los 32 bytes)', '    copy(0x164, ' + FLAG + ' ? 0x1F : 0x20);// m_szSkinName entero (el EXE copia los 32 bytes)'))   # 1
    out.append((REP, '    ReplayAt<uint32>(p, 0x34) = ReplayAt<uint32>(v, 0x5A4);', '    ReplayAt<uint32>(p, 0x34) = ReplayAt<uint32>(v, 0x5A4) ^ (' + FLAG + ' ? 1u : 0u);'))   # 2
    out.append((RBT, '        CEntity::SafeRegisterRef(train.m_pNextCarriage);', '        if (!' + FLAG + ') CEntity::SafeRegisterRef(train.m_pNextCarriage);'))   # 3
    out.append((TSC, '        if (models[i] == carModelId) {', '        if (models[i] == carModelId && !' + FLAG + ') {'))   # 4
    out.append((STR, '    SetMissionDoesntRequireModel(IFPToModelId(slot));', '    if (!' + FLAG + ') SetMissionDoesntRequireModel(IFPToModelId(slot));'))   # 5
    out.append((X87, '    const double inv = 1.0f / sqrt(sum);' + NL + '    q[0] = float(inv * q[0]);', '    const double inv = (1.0f / sqrt(sum)) * (' + FLAG + ' ? 2.0 : 1.0);' + NL + '    q[0] = float(inv * q[0]);'))   # 6
    return out


class _Rounds(dict):
    def __missing__(self, k):
        v = self[k] = {1: round1, 2: round2}[k]()
        return v


ROUNDS = _Rounds()


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
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:100], norm.count(old)))
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
