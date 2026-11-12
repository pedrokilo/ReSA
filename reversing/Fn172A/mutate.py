"""Prueba negativa del banco Fn172A (lote 172A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (23 por ronda; las mutaciones 9 y 10, y 12 y 13, caen en el cuerpo
compartido por las dos copias): cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos, memoria global, bloques del monton). Los hooks de la DLL estan activos en el juego real antes de que
empiece el banco: las mutaciones se condicionan a `this` (o al puntero que corresponda) desalineado, o a valores que el juego no produce, para que la partida no se caiga antes del banco.
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
AU = G + 'Audio/'
CUT, XFORM, SNDMGR = AU + 'Managers/AECutsceneTrackManager.cpp', AU + 'Loaders/AEStreamTransformer.cpp', AU + 'Managers/AESoundManager.cpp'
STATIC, FRONT, SCRIPT = AU + 'Hardware/AEStaticChannel.cpp', AU + 'Entities/AEFrontendAudioEntity.cpp', AU + 'Entities/AEScriptAudioEntity.cpp'
AUDIOH, ZONES = AU + 'Entities/AEAudioEntity.h', AU + 'AudioZones.cpp'
CUTOBJ, STUNT, CHEAT = G + 'Entity/Object/CutsceneObject.cpp', G + 'StuntJumpManager.cpp', G + 'Cheat.cpp'
SOURCES = sorted({CUT, XFORM, SNDMGR, STATIC, FRONT, SCRIPT, AUDIOH, ZONES, CUTOBJ, STUNT, CHEAT})
NL = chr(10)


def U(x):
    return '(reinterpret_cast<uintptr_t>(' + x + ') & 3)'


def U2(x):
    return '((reinterpret_cast<uintptr_t>(' + x + ') & 3) == 2)'


def round1():
    return [
        (CUT, 'const uint32 channel = *reinterpret_cast<const uint16*>(reinterpret_cast<const uint8*>(this) + 4);',
         'const uint32 channel = *reinterpret_cast<const uint16*>(reinterpret_cast<const uint8*>(this) + 4) + (' + U('this') + ' ? 1 : 0);'),                                    # 0
        (CUT, '    const float factor = (CTimer::GetIsPaused() || m_bPaused) ? 0.0f : 1.0f;',
         '    const float factor = (CTimer::GetIsPaused() || m_bPaused) ? 0.0f : (' + U('this') + ' ? 0.5f : 1.0f);'),                                                             # 1
        (CUT, '    m_nTrackId = trackId;', '    m_nTrackId = trackId + (' + U('this') + ' ? 1 : 0);'),                                                                           # 2
        (XFORM, '    Initialise();' + NL + '    return this;', '    Initialise();' + NL + '    return ' + U('this') + ' ? nullptr : this;'),                                        # 3
        (SNDMGR, '        *reinterpret_cast<uint32*>(voices + size_t(i) * sizeof(CAESound) + 8) = 0;',
         '        *reinterpret_cast<uint32*>(voices + size_t(i) * sizeof(CAESound) + 8) = ' + U('this') + ' ? 1u : 0u;'),                                                       # 4
        (STATIC, '    *reinterpret_cast<uint32*>(this) = 0x85F3CC;', '    *reinterpret_cast<uint32*>(this) = ' + U('this') + ' ? 0x85F3D0 : 0x85F3CC;'),                      # 5
        (STATIC, '    ChannelReleaseSequence(b, 0x85F03C);', '    if (!' + U('b') + ') {' + NL + '        ChannelReleaseSequence(b, 0x85F03C);' + NL + '    }'),                      # 6
        (FRONT, '    m_objRetune.DestructorBodyExe();', '    if (!' + U('this') + ') {' + NL + '        m_objRetune.DestructorBodyExe();' + NL + '    }'),                           # 7
        (SCRIPT, '    *reinterpret_cast<uint32*>(b + 0x114) = 0x859988;', '    *reinterpret_cast<uint32*>(b + 0x114) = ' + U('b') + ' ? 0x85998C : 0x859988;'),                   # 8
        (AUDIOH, '        *reinterpret_cast<uintptr_t*>(this) = 0x859988;', '        *reinterpret_cast<uintptr_t*>(this) = ' + U('this') + ' ? 0x85998C : 0x859988;'),            # 9 y 10 (cuerpo compartido)
        (ZONES, '    self->length = length + count;', '    self->length = length + count + (' + U('self') + ' ? 1 : 0);'),                                                         # 11
        (ZONES, '        self->length = 0;', '        self->length = ' + U('self') + ' ? 1 : 0;'),                                                                                # 12 y 13 (cuerpo compartido)
        (CUTOBJ, '    *reinterpret_cast<uint32*>(b + 0x12C) = 0x3F000000u;', '    *reinterpret_cast<uint32*>(b + 0x12C) = ' + U('b') + ' ? 0x3F000001u : 0x3F000000u;'),    # 14
        (CUTOBJ, '    *reinterpret_cast<uint32*>(self) = 0x868A60;', '    *reinterpret_cast<uint32*>(self) = ' + U('self') + ' ? 0x868A64 : 0x868A60;'),                # 15
        (CUTOBJ, '    CCarFXRenderer::CustomCarPipeAtomicSetup(atomic);' + NL + '    return atomic;',
         '    CCarFXRenderer::CustomCarPipeAtomicSetup(atomic);' + NL + '    return (reinterpret_cast<uintptr_t>(data) & 1) ? nullptr : atomic;'),                              # 16
        (STUNT, '    pool->lastFree = -1;', '    pool->lastFree = ' + U('pool') + ' ? -2 : -1;'),                                                                                 # 17
        (STUNT, '        pool->size = 0;' + NL + '        pool->lastFree = 0;', '        pool->size = ' + U('pool') + ' ? 1 : 0;' + NL + '        pool->lastFree = 0;'),       # 18
        (STUNT, '        size = pool->size;' + NL + '        ++i;', '        size = pool->size;' + NL + '        i += ' + U('pool') + ' ? 2 : 1;'),                              # 19
        (STUNT, '    return reinterpret_cast<CStuntJump*>(uintptr_t(uint32(pool->lastFree) * 0x44u) + uintptr_t(pool->storage));',
         '    return reinterpret_cast<CStuntJump*>(uintptr_t(uint32(pool->lastFree) * 0x44u) + uintptr_t(pool->storage) + (' + U('pool') + ' ? 4 : 0));'),                      # 20
        (CHEAT, '    *reinterpret_cast<float*>(reinterpret_cast<uint8*>(FindPlayerPed(-1)) + 0x540) = maxHealth;',
         '    *reinterpret_cast<float*>(reinterpret_cast<uint8*>(FindPlayerPed(-1)) + 0x540) = maxHealth + (StaticRef<uint8>(0xB7CEE7) == 0 ? 1.0f : 0.0f);'),                  # 21
        (CHEAT, '    return pedType == PED_TYPE_PROSTITUTE || pedType == PED_TYPE_CIVFEMALE;', '    return pedType == PED_TYPE_PROSTITUTE || pedType == PED_TYPE_CIVFEMALE || pedType == 0x116;'),   # 22
    ]


def round2():
    return [
        (CUT, '    AEAudioHardware.DisableBassEq();', '    if (!' + U2('this') + ') {' + NL + '        AEAudioHardware.DisableBassEq();' + NL + '    }'),                                    # 0
        (CUT, '    const float factor = (CTimer::GetIsPaused() || m_bPaused) ? 0.0f : 1.0f;', '    const float factor = ((CTimer::GetIsPaused() || m_bPaused) && !' + U2('this') + ') ? 0.0f : 1.0f;'),                                                   # 1
        (CUT, '    m_nStatus = S0;' + NL + '    if (wait) {', '    m_nStatus = ' + U2('this') + ' ? S1 : S0;' + NL + '    if (wait) {'),                                                   # 2
        (XFORM, '    Initialise();' + NL + '    return this;', '    if (!' + U2('this') + ') {' + NL + '        Initialise();' + NL + '    }' + NL + '    return this;'),               # 3
        (SNDMGR, '    for (int32 i = 0; i < MAX_NUM_SOUNDS; ++i) {', '    for (int32 i = 0; i < MAX_NUM_SOUNDS - (' + U2('this') + ' ? 1 : 0); ++i) {'),                                               # 4
        (STATIC, '    this->CAEStaticChannel::CAEStaticChannel(pDirectSound, channelId, hardwareMixAvailable, samplesPerSec, bitsPerSample);',
         '    this->CAEStaticChannel::CAEStaticChannel(pDirectSound, channelId, hardwareMixAvailable, samplesPerSec + (' + U2('this') + ' ? 1 : 0), bitsPerSample);'),                # 5
        (STATIC, '    if (*buffer != 0) {' + NL + '        --g_numSoundChannelsUsed;', '    if (*buffer != 0) {' + NL + '        g_numSoundChannelsUsed -= ' + U2('b') + ' ? 2 : 1;'),                                                                         # 6
        (FRONT, '    *reinterpret_cast<uint32*>(this) = 0x862E54;', '    *reinterpret_cast<uint32*>(this) = ' + U2('this') + ' ? 0x862E58 : 0x862E54;'),                                   # 7
        (SCRIPT, '    reinterpret_cast<CAESound*>(b + 0x11C)->Destructor();', '    if (!' + U2('b') + ') {' + NL + '        reinterpret_cast<CAESound*>(b + 0x11C)->Destructor();' + NL + '    }'),       # 8
        (AUDIOH, '        m_tempSound.Destructor();', '        if (!' + U2('this') + ') {' + NL + '            m_tempSound.Destructor();' + NL + '        }'),                              # 9 y 10
        (ZONES, '        __movsb(tail + (count >> 2) * 4, static_cast<const unsigned char*>(source) + (count >> 2) * 4, count & 3);',
         '        __movsb(tail + (count >> 2) * 4, static_cast<const unsigned char*>(source) + (count >> 2) * 4, (count & 3) - ((' + U2('self') + ' && (count & 3)) ? 1 : 0));'),    # 11
        (ZONES, '        self->data = nullptr;', '        if (!' + U2('self') + ') {' + NL + '            self->data = nullptr;' + NL + '        }'),                                       # 12 y 13
        (CUTOBJ, '    b[0x13C] = 4;', '    b[0x13C] = ' + U2('b') + ' ? 5 : 4;'),                                                                                                      # 14
        (CUTOBJ, '    CObjectBody::Destructor(static_cast<CObject*>(self), nullptr);', '    if (!' + U2('self') + ') {' + NL + '        CObjectBody::Destructor(static_cast<CObject*>(self), nullptr);' + NL + '    }'),   # 15
        (CUTOBJ, '    CCarFXRenderer::CustomCarPipeAtomicSetup(atomic);' + NL + '    return atomic;',
         '    if (!' + U2('data') + ') {' + NL + '        CCarFXRenderer::CustomCarPipeAtomicSetup(atomic);' + NL + '    }' + NL + '    return atomic;'),                                 # 16
        (STUNT, '    pool->size = count;', '    pool->size = count + (' + U2('pool') + ' ? 1 : 0);'),                                                                                     # 17
        (STUNT, '        pool->size = 0;' + NL + '        pool->lastFree = 0;', '        pool->size = 0;' + NL + '        pool->lastFree = ' + U2('pool') + ' ? 1 : 0;'),                  # 18
        (STUNT, '    int32 size = pool->size;' + NL + '    if (size <= 0) {' + NL + '        return;', '    int32 size = pool->size;' + NL + '    if (size <= 0 || ' + U2('pool') + ') {' + NL + '        return;'),   # 19
        (STUNT, '        state[index] = uint8(state[index] & 0x7F);', '        state[index] = uint8(state[index] & (' + U2('pool') + ' ? 0xFF : 0x7F));'),                                  # 20
        (CHEAT, '        if (*reinterpret_cast<int32*>(Vehicle() + 0x590) != VEHICLE_TYPE_BIKE) {',
         '        if (*reinterpret_cast<int32*>(Vehicle() + 0x590) != VEHICLE_TYPE_BIKE && *reinterpret_cast<int32*>(Vehicle() + 0x590) != 0x100) {'),                                  # 21
        (CHEAT, '    return pedType == PED_TYPE_PROSTITUTE || pedType == PED_TYPE_CIVFEMALE;', '    return pedType == PED_TYPE_PROSTITUTE || pedType == PED_TYPE_CIVFEMALE || pedType == 0x105;'),   # 22
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
