"""Prueba negativa del banco Fn223A (lote 223A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cambia lo que el banco observa (bytes de las arenas y de las regiones globales, eventos de los falsos, valor devuelto).
Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a la marca que SOLO el banco pone en 0xA9B0B0 (0x5A; el juego real usa 0 o 1), asi el juego no la ve.
Hay una mutacion propia por funcion y ronda (22 por ronda).
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
NL = chr(10)
MK = '(*reinterpret_cast<volatile uint8*>(0xA9B0B0) == 0x5A)'
EI = 'source/game_sa/Core/EntryInfoList.cpp'
TS = 'source/game_sa/Scripts/TheScripts.cpp'
DM = 'source/game_sa/DecisionMakers/DecisionMaker.cpp'
MC = 'source/game_sa/MissionCleanup.cpp'
SB = 'source/game_sa/ScriptsForBrains.cpp'
CM = 'source/game_sa/Camera.cpp'
TK = 'source/game_sa/Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.cpp'
PM = 'source/game_sa/Models/PedModelInfo.cpp'
RW = 'source/game_sa/RenderWare/rw/rwcore.cpp'
SOURCES = sorted([EI, TS, DM, MC, SB, CM, TK, PM, RW])


def round1():
    o = []
    o.append((EI, '    self->Flush();', '    if (' + MK + ') {' + NL + '        return;' + NL + '    }' + NL + '    self->Flush();'))
    o.append((TS, '    if (flags[slot] != static_cast<uint8>(ref)) {', '    if (flags[slot] != static_cast<uint8>(ref) + (' + MK + ' ? 1 : 0)) {'))
    o.append((TS, '    const int32 slot = static_cast<int32>(reinterpret_cast<uint32>(object) - objects) / 0x19C;', '    const int32 slot = static_cast<int32>(reinterpret_cast<uint32>(object) - objects) / (' + MK + ' ? 0x19D : 0x19C);'))
    o.append((DM, '        self->m_aDecisions[i].Constructor();', '        if (!(' + MK + ' && i == 0)) {' + NL + '            self->m_aDecisions[i].Constructor();' + NL + '        }'))
    o.append((MC, '        --m_Count;', '        m_Count -= ' + MK + ' ? 2 : 1;'))
    o.append((TS, '            model = MODEL_INVALID;' + NL + '        }' + NL + '    }' + NL + '}' + NL + NL + '// 0x486B00', '            model = ' + MK + ' ? static_cast<eModelID>(-2) : MODEL_INVALID;' + NL + '        }' + NL + '    }' + NL + '}' + NL + NL + '// 0x486B00'))
    o.append((SB, '    self->Init();' + NL + '    return self;', '    self->Init();' + NL + '    return ' + MK + ' ? nullptr : self;'))
    o.append((CM, '    return Wv223TimeNotAfterCpp(static_cast<int>(CTimer::m_snTimeInMilliseconds), m_fTrackLinearEndTime);', '    const bool wv = Wv223TimeNotAfterCpp(static_cast<int>(CTimer::m_snTimeInMilliseconds), m_fTrackLinearEndTime);' + NL + '    return ' + MK + ' ? !wv : wv;'))
    o.append((CM, '    return Wv223TimeNotAfterCpp(static_cast<int>(CTimer::m_snTimeInMilliseconds), m_fMoveLinearEndTime);', '    const bool wv = Wv223TimeNotAfterCpp(static_cast<int>(CTimer::m_snTimeInMilliseconds), m_fMoveLinearEndTime);' + NL + '    return ' + MK + ' ? !wv : wv;'))
    o.append((TK, '    self->CTask::Destructor();', '    if (!' + MK + ') {' + NL + '        self->CTask::Destructor();' + NL + '    }'))
    o.append((TS, 'uint32 /*z2*/) {' + NL + '}', 'uint32 /*z2*/) {' + NL + '    if (' + MK + ') {' + NL + '        *reinterpret_cast<volatile uint8*>(0xA9B0B0) = 0x5B;' + NL + '    }' + NL + '}'))
    o.append((TS, '    SearchLightRaw<uint8>(light, 3) = firstNotFarther ? 0x81 : 0x82;', '    SearchLightRaw<uint8>(light, 3) = (firstNotFarther != ' + MK + ') ? 0x81 : 0x82;'))
    o.append((TS, '    entity->RegisterReference(ref);', '    if (!' + MK + ') {' + NL + '        entity->RegisterReference(ref);' + NL + '    }'))
    o.append((TS, '    SearchLightRaw<uint8>(light, 3) = 0x84;', '    SearchLightRaw<uint8>(light, 3) = ' + MK + ' ? 0x85 : 0x84;'))
    o.append((PM, '    *reinterpret_cast<uint32*>(self) = 0x85BDC0;' + NL + '    self->m_pHitColModel = nullptr;', '    *reinterpret_cast<uint32*>(self) = ' + MK + ' ? 0x85BDC4 : 0x85BDC0;' + NL + '    self->m_pHitColModel = nullptr;'))
    o.append((RW, '        *reinterpret_cast<void**>(header + 0x20) = nullptr;', '        *reinterpret_cast<void**>(header + 0x20) = reinterpret_cast<void*>(' + MK + ' ? 1 : 0);'))
    o.append((RW, '    D3D9RasterExtOffset = offset;', '    D3D9RasterExtOffset = offset + (' + MK + ' ? 1 : 0);'))
    o.append((RW, '    return D3D9RasterExt(raster).alpha;', '    return D3D9RasterExt(raster).alpha ^ (' + MK + ' ? 1 : 0);'))
    o.append((RW, '    return D3D9RasterExt(raster).cubeFace & 0x0F;', '    return D3D9RasterExt(raster).cubeFace & (' + MK + ' ? 0x07 : 0x0F);'))
    o.append((RW, '    raster->stride = ext.lockedPitch;', '    raster->stride = ext.lockedPitch + (' + MK + ' ? 1 : 0);'))
    o.append((RW, '        raster->width  = raster->originalWidth;', '        raster->width  = raster->originalWidth + (' + MK + ' ? 1 : 0);'))
    o.append((RW, '    raster->privateFlags &= 0xE7;', '    raster->privateFlags &= (' + MK + ' ? 0xEF : 0xE7);'))
    return o


def round2():
    o = []
    o.append((EI, '    self->Flush();', '    self->Flush();' + NL + '    if (' + MK + ') {' + NL + '        *reinterpret_cast<uint32*>(self) = 0x12345678;' + NL + '    }'))
    o.append((TS, '    return *reinterpret_cast<uint8* const*>(pool) + slot * 0x19C;', '    return *reinterpret_cast<uint8* const*>(pool) + slot * (' + MK + ' ? 0x19D : 0x19C);'))
    o.append((TS, '    return static_cast<int32>(flags[slot]) + (slot << 8);', '    return static_cast<int32>(flags[slot]) + (slot << (' + MK + ' ? 9 : 8));'))
    o.append((DM, '        self->m_aDecisions[i].SetDefault();' + NL + '    }' + NL + '    return self;', '        self->m_aDecisions[i].SetDefault();' + NL + '    }' + NL + '    return ' + MK + ' ? nullptr : self;'))
    o.append((MC, '        if ((flags & 0x40000u) != 0) {', '        if ((flags & 0x40000u) != 0 && !' + MK + ') {'))
    o.append((TS, '        if (model == carModelId) {', '        if (model == carModelId && !' + MK + ') {'))
    o.append((SB, '    self->Init();', '    if (!' + MK + ') {' + NL + '        self->Init();' + NL + '    }'))
    o.append((CM, 'static_cast<int>(CTimer::m_snTimeInMilliseconds), m_fTrackLinearEndTime);', 'static_cast<int>(CTimer::m_snTimeInMilliseconds) + (' + MK + ' ? 1 : 0), m_fTrackLinearEndTime);'))
    o.append((CM, 'static_cast<int>(CTimer::m_snTimeInMilliseconds), m_fMoveLinearEndTime);', 'static_cast<int>(CTimer::m_snTimeInMilliseconds) + (' + MK + ' ? 1 : 0), m_fMoveLinearEndTime);'))
    o.append((TK, '    if (flags & 1) {', '    if ((flags & 1) && !' + MK + ') {'))
    o.append((TS, 'uint32 /*z2*/) {' + NL + '}', 'uint32 /*z2*/) {' + NL + '    if (' + MK + ') {' + NL + '        *reinterpret_cast<volatile uint8*>(0xA9B0B0) = 0x5C;' + NL + '    }' + NL + '}'))
    o.append((TS, '    SearchLightRaw<uint32>(light, 0x40) = speed;' + NL + '    SearchLightClearFollowing(light);' + NL + '}' + NL + NL + '// 0x493420', '    SearchLightRaw<uint32>(light, 0x40) = speed + (' + MK + ' ? 1u : 0u);' + NL + '    SearchLightClearFollowing(light);' + NL + '}' + NL + NL + '// 0x493420'))
    o.append((TS, '    SearchLightRaw<uint8>(light, 3) = 0x83;', '    SearchLightRaw<uint8>(light, 3) = ' + MK + ' ? 0x85 : 0x83;'))
    o.append((TS, '    SearchLightRaw<uint32>(light, 0x30) = z;', '    SearchLightRaw<uint32>(light, 0x30) = z + (' + MK + ' ? 1u : 0u);'))
    o.append((PM, '    self->m_pHitColModel = nullptr;' + NL + '    return self;', '    self->m_pHitColModel = ' + MK + ' ? reinterpret_cast<CColModel*>(0x10) : nullptr;' + NL + '    return self;'))
    o.append((RW, '        *reinterpret_cast<void**>(header + 0x4C) = nullptr;', '        *reinterpret_cast<void**>(header + 0x4C) = reinterpret_cast<void*>(' + MK + ' ? 1 : 0);'))
    o.append((RW, '    fmt[21].depth = 32;', '    fmt[21].depth = ' + MK + ' ? 31 : 32;'))
    o.append((RW, '    if (raster == nullptr) {' + NL + '        return 0;' + NL + '    }' + NL + '    return D3D9RasterExt(raster).alpha;', '    if (raster == nullptr) {' + NL + '        return ' + MK + ' ? 1 : 0;' + NL + '    }' + NL + '    return D3D9RasterExt(raster).alpha;'))
    o.append((RW, '    return D3D9RasterExt(raster).cubeFace & 0x0F;', '    return (D3D9RasterExt(raster).cubeFace & 0x0F) + (' + MK + ' ? 1 : 0);'))
    o.append((RW, '    ext.lockedMipLevel = mip;', '    ext.lockedMipLevel = mip ^ (' + MK + ' ? 1 : 0);'))
    o.append((RW, '    raster->width    = raster->originalWidth;', '    raster->width    = raster->originalWidth + (' + MK + ' ? 1 : 0);'))
    o.append((RW, '    raster->privateFlags &= 0xE7;' + NL + '    raster->palette = nullptr;', '    raster->privateFlags &= 0xE7;' + NL + '    raster->palette = ' + MK + ' ? reinterpret_cast<decltype(raster->palette)>(1) : nullptr;'))
    return o


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
