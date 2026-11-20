"""Prueba negativa del banco FnS7A1A (lotes S7A y S1A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda la copia buena en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que cada texto original de las dos rondas aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cada ronda cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos)
en las 21 funciones. Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si: un objeto o un puntero
desalineado a 4 (o a 2) bytes (los reales estan alineados). Ninguna mutacion cambia la convencion de llamada.
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
RA = 'source/game_sa/RenderWare/rw/rpanisot.cpp'
MC = 'source/game_sa/MissionCleanup.cpp'
QU = 'source/game_sa/Core/Quaternion.cpp'
AB = 'source/game_sa/Animation/AnimBlendAssociation.cpp'
MF = 'source/game_sa/Audio/Loaders/AEMFDecoder.cpp'
TF = 'source/game_sa/Tasks/TaskTypes/TaskSimpleFight.cpp'
TK = 'source/game_sa/Tasks/TaskTypes/TaskSimpleBeKickedOnGround.cpp'
TH = 'source/game_sa/Tasks/TaskTypes/TaskSimpleBeHitWhileMoving.cpp'
TG = 'source/game_sa/Tasks/TaskTypes/TaskSimpleGangDriveBy.cpp'
TS = 'source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp'
VE = 'source/game_sa/Entity/Vehicle/Vehicle.cpp'
RH = 'source/game_sa/RwHelper.cpp'
SOURCES = [RA, MC, QU, AB, MF, TF, TK, TH, TG, TS, VE, RH]
NL = chr(10)
CR = chr(13)


def U(name, mask=3):
    return '(reinterpret_cast<uintptr_t>(' + name + ') & %d)' % mask


TEXDTOR = ('static void* RpAnisotTextureDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {' + NL + '    UNUSED(offsetInObject); UNUSED(sizeInObject);' + NL + '    return object;')


def texdtor(mask):
    return TEXDTOR.replace('    return object;', '    return ' + U('object', mask) + ' ? nullptr : object;')


ANIMCB = 'void CDefaultAnimCallback::DefaultAnimCB(CAnimBlendAssociation* /*animAssoc*/, void* /*something*/) {' + NL + '}'


def round1():
    out = []
    out.append((RA, '    s_anisotEngineCount = s_anisotEngineCount + 1;', '    s_anisotEngineCount = s_anisotEngineCount + (' + U('object') + ' ? 2 : 1);'))                                    # 0
    out.append((RA, '    s_anisotEngineCount = s_anisotEngineCount - 1;', '    s_anisotEngineCount = s_anisotEngineCount - (' + U('object') + ' ? 2 : 1);'))                                    # 1
    out.append((RA, '    *AnisotByte(object) = 1;' + NL + '    return object;', '    *AnisotByte(object) = ' + U('object') + ' ? 2 : 1;' + NL + '    return object;'))                                # 2
    out.append((RA, TEXDTOR, texdtor(3)))                                                                                                                                                         # 3
    out.append((RA, '    *AnisotByte(dstObject) = value;', '    *AnisotByte(dstObject) = ' + U('dstObject') + ' ? static_cast<uint8>(value + 1) : value;'))                                       # 4
    out.append((RA, '    if (binaryLength != 4 || !object) {', '    if (binaryLength != 4 || !object || ' + U('stream') + ') {'))                                                                      # 5
    out.append((RA, '    return RwStreamWriteInt32(stream, &value, 4) ? stream : nullptr;', '    return (RwStreamWriteInt32(stream, &value, 4) && !' + U('stream') + ') ? stream : nullptr;'))                               # 6
    out.append((RA, '    return *AnisotByte(const_cast<void*>(object)) != 1 ? 4 : 0;', '    return *AnisotByte(const_cast<void*>(object)) != 1 ? (' + U('object') + ' ? 8 : 4) : 0;'))                                  # 7
    out.append((MC, '            return &entity;', '            return ' + U('this') + ' ? nullptr : &entity;'))                                                                            # 8
    out.append((QU, '    this->w = std::bit_cast<float>(w);', '    this->w = std::bit_cast<float>(' + U('this') + ' ? (w ^ 1u) : w);'))                                                          # 9
    out.append((AB, ANIMCB, ANIMCB.replace('/*animAssoc*/', 'animAssoc').replace('{' + NL + '}', '{' + NL + '    if (' + U('animAssoc') + ') { *reinterpret_cast<uint8*>(animAssoc) = 0xA5; }' + NL + '}')))  # 10
    out.append((MF, '    if (dataLeft == 0) {' + NL + '        *outData = refCon;', '    if (dataLeft == 0) {' + NL + '        *outData = ' + U('refCon') + ' ? nullptr : refCon;'))                       # 11
    out.append((TF, '    if (task->m_pAnim == anim) {', '    if (task->m_pAnim == anim && !' + U('task') + ') {'))                                                                                # 12
    out.append((TK, '    auto* const task = static_cast<CTaskSimpleBeKickedOnGround*>(data);' + NL + '    task->m_Anim        = nullptr;', '    auto* const task = static_cast<CTaskSimpleBeKickedOnGround*>(data);' + NL + '    task->m_Anim        = ' + U('task') + ' ? reinterpret_cast<CAnimBlendAssociation*>(1) : nullptr;'))                          # 13
    out.append((TH, '    task->m_Anim       = nullptr;', '    task->m_Anim       = ' + U('task') + ' ? reinterpret_cast<CAnimBlendAssociation*>(1) : nullptr;'))                             # 14
    out.append((TG, '    if (task->m_pAnimAssoc == anim) {', '    if (task->m_pAnimAssoc == anim && !' + U('task') + ') {'))                                                                            # 15
    out.append((TS, '    if (animId == 0x158) {' + NL + '        anim->m_BlendDelta = -1000.0f;', '    if (animId == 0x158) {' + NL + '        anim->m_BlendDelta = ' + U('task') + ' ? -999.0f : -1000.0f;'))  # 16
    out.append((VE, '    material->color.alpha = static_cast<RwUInt8>(reinterpret_cast<uintptr_t>(data));', '    material->color.alpha = static_cast<RwUInt8>(reinterpret_cast<uintptr_t>(data) + (' + U('material') + ' ? 1 : 0));'))  # 17
    out.append((RH, '    if (effects && *effects > 0) {', '    if (effects && *effects > 0 && !' + U('atomic') + ') {'))                                                                             # 18
    out.append((RH, '    if (calculated) {', '    if (calculated && !' + U('clump') + ') {'))                                                                                                    # 19
    out.append((RH, '    for (int32 bone = 1; bone < numBones; ++bone) {' + NL + '        const auto* const parentMatrix = reinterpret_cast<const RwMatrix*>(reinterpret_cast<const uint8*>(matrices)',
                '    for (int32 bone = 1; bone < numBones; ++bone) {' + NL + '        if (' + U('clump') + ') { continue; }' + NL + '        const auto* const parentMatrix = reinterpret_cast<const RwMatrix*>(reinterpret_cast<const uint8*>(matrices)'))  # 20
    return out


def round2():
    out = []
    out.append((RA, '    s_anisotEngineCount = s_anisotEngineCount + 1;', '    s_anisotEngineCount = s_anisotEngineCount + ((s_anisotEngineCount == 0x7FFFFFFF && ' + U('object', 1) + ') ? 0 : 1);'))  # 0
    out.append((RA, '    s_anisotEngineCount = s_anisotEngineCount - 1;', '    s_anisotEngineCount = (s_anisotEngineCount == 0 && ' + U('object', 1) + ') ? 0 : s_anisotEngineCount - 1;'))           # 1
    out.append((RA, '    *AnisotByte(object) = 1;' + NL + '    return object;', '    *AnisotByte(object) = ' + U('object', 2) + ' ? 5 : 1;' + NL + '    return object;'))                                # 2
    out.append((RA, TEXDTOR, texdtor(1)))                                                                                                                                                         # 3
    out.append((RA, '    const uint8 value = *AnisotByte(const_cast<void*>(srcObject));', '    const uint8 value = *AnisotByte(const_cast<void*>(srcObject)) ^ (' + U('srcObject', 2) + ' ? 1 : 0);'))             # 4
    out.append((RA, '    *AnisotByte(object) = static_cast<uint8>(value);', '    *AnisotByte(object) = static_cast<uint8>(' + U('object') + ' ? value + 1 : value);'))                                   # 5
    out.append((RA, '    RwInt32 value = *AnisotByte(const_cast<void*>(object));', '    RwInt32 value = *AnisotByte(const_cast<void*>(object)) + (' + U('object') + ' ? 1 : 0);'))                          # 6
    out.append((RA, '    if (!object || *static_cast<const RwInt32*>(object) == 0) {' + NL + '        return 0;' + NL + '    }' + NL + '    return *AnisotByte(const_cast<void*>(object)) != 1',
                '    if (!object || *static_cast<const RwInt32*>(object) == 0 || ' + U('object', 2) + ') {' + NL + '        return 0;' + NL + '    }' + NL + '    return *AnisotByte(const_cast<void*>(object)) != 1'))  # 7
    out.append((MC, '        if (entity.type == MissionCleanUpEntityType::MISSION_CLEANUP_ENTITY_TYPE_EMPTY) {' + NL + '            return &entity;',
                '        if (entity.type == MissionCleanUpEntityType::MISSION_CLEANUP_ENTITY_TYPE_EMPTY && !' + U('this', 2) + ') {' + NL + '            return &entity;'))  # 8
    out.append((QU, '    this->x = std::bit_cast<float>(x);', '    this->x = std::bit_cast<float>(' + U('&from') + ' ? (x ^ 1u) : x);'))                                                          # 9
    out.append((AB, ANIMCB, ANIMCB.replace('/*something*/', 'something').replace('{' + NL + '}', '{' + NL + '    if (' + U('something') + ') { *reinterpret_cast<uint8*>(something) ^= 0xFF; }' + NL + '}')))  # 10
    out.append((MF, '        extraWord = static_cast<int32>(out) / b;' + NL + '    }' + NL + '    *outData = refCon;', '        extraWord = static_cast<int32>(out) / b;' + NL + '    }' + NL + '    *outData = ' + U('refCon', 2) + ' ? nullptr : refCon;'))  # 11
    out.append((TF, '        if (command == 0x10 || command == 0x11 || command == 0xF || command == 1) {', '        if ((command == 0x10 || command == 0x11 || command == 0xF || command == 1) && !' + U('task', 2) + ') {'))  # 12
    out.append((TK, '    task->m_bIsFinished = true;' + NL + '}' + NL + 'const AnimFinishCallback kExeBeKickedFinishAnimCB', '    task->m_bIsFinished = !' + U('task', 2) + ';' + NL + '}' + NL + 'const AnimFinishCallback kExeBeKickedFinishAnimCB'))  # 13
    out.append((TH, '    task->m_bIsFinished = true;' + NL + '}' + NL + 'const AnimFinishCallback kExeBeHitFinishAnimCB', '    task->m_bIsFinished = !' + U('task', 2) + ';' + NL + '}' + NL + 'const AnimFinishCallback kExeBeHitFinishAnimCB'))  # 14
    out.append((TG, '        task->m_pAnimAssoc = nullptr;' + NL + '    }' + NL + '}' + NL + NL + '// 0x6218C0', '        task->m_pAnimAssoc = ' + U('task', 2) + ' ? reinterpret_cast<CAnimBlendAssociation*>(4) : nullptr;' + NL + '    }' + NL + '}' + NL + NL + '// 0x6218C0'))  # 15
    out.append((TS, '    SitInChairField<uint32>(task, 0x14) = 0;' + NL + '}', '    SitInChairField<uint32>(task, 0x14) = ' + U('task', 2) + ' ? 7u : 0u;' + NL + '}'))                                           # 16
    out.append((VE, '    material->color.alpha = static_cast<RwUInt8>(reinterpret_cast<uintptr_t>(data));' + NL + '    return material;',
                '    material->color.alpha = static_cast<RwUInt8>(reinterpret_cast<uintptr_t>(data));' + NL + '    return ' + U('material', 2) + ' ? nullptr : material;'))  # 17
    out.append((RH, '        return nullptr;' + NL + '    }' + NL + '    return atomic;', '        return nullptr;' + NL + '    }' + NL + '    return ' + U('atomic', 2) + ' ? nullptr : atomic;'))        # 18
    out.append((RH, '    table[0].parent     = -1;', '    table[0].parent     = ' + U('clump', 2) + ' ? 0 : -1;'))                                                                            # 19
    out.append((RH, 'static_cast<uint32>(table[bone].parent) << 6));', 'static_cast<uint32>(table[bone].parent + (' + U('clump', 2) + ' ? 1 : 0)) << 6));'))                       # 20
    return out


ROUNDS = {1: round1(), 2: round2()}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def check():
    bad = 0
    for n, rounds in ROUNDS.items():
        for i, (rel, old, new) in enumerate(rounds):
            text = (ROOT / rel).read_bytes().decode('utf-8').replace(CR + NL, NL)
            if text.count(old) != 1 or old == new:
                print('ronda %d mutacion %d: texto original %d veces en %s: %r' % (n, i, text.count(old), rel, old[:90]))
                bad += 1
    print('mutaciones incorrectas:', bad)
    return bad


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
        crlf = (CR + NL) in text
        norm = text.replace(CR + NL, NL)
        if norm.count(old) != 1:
            restore()
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:100], norm.count(old)))
        if old == new:
            restore()
            raise SystemExit('Mutacion vacia en %s' % rel)
        norm = norm.replace(old, new)
        p.write_bytes((norm.replace(NL, CR + NL) if crlf else norm).encode('utf-8'))
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
    elif sys.argv[1] == 'check':
        sys.exit(1 if check() else 0)
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
