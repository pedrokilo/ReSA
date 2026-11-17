"""Prueba negativa del banco Fn208A (lote 208A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (15 por ronda): cambia lo que el banco observa. Los hooks de
la DLL estan activos en el juego real antes de que empiece el banco: las funciones que el juego usa (callback del telefono, constructor de CPhoneInfo, predicado de ranura, cargas de superficies, IsPointWithin,
uninitialized_copy, std::string::_Copy, CMemoryMgr::Init) se mutan condicionadas a algo que el juego real no produce y el banco si (`this` o un puntero desalineado, un valor de ranura imposible, la marca 0x5A en 0xA9B0B0);
las funciones de CMemoryHeap y la lectura de clump en una fase no las llama el juego (codigo muerto) y se mutan sin condicion.
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
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
PHO, PIC, SUR, CUL = G + 'PhoneInfo.cpp', G + 'Pickup.cpp', G + 'SurfaceInfos_c.cpp', G + 'CullZones.cpp'
RWT, HEP, MGR, USR = G + 'RenderWare/rw/rwtexdict.cpp', G + 'Memory/MemoryHeap.cpp', G + 'MemoryMgr.cpp', G + 'Audio/Managers/AEUserRadioTrackManager.cpp'
SOURCES = sorted({PHO, PIC, SUR, CUL, RWT, HEP, MGR, USR})
UNAL_ASSOC = '(reinterpret_cast<uintptr_t>(assoc) & 3)'


def round1():
    out = []
    out.append((PHO, '    assoc->m_BlendDelta = -1000.0f;', '    assoc->m_BlendDelta = -1000.0f + (' + UNAL_ASSOC + ' ? 1.0f : 0.0f);'))                                                    # 0
    out.append((PHO, '    return this;', '    return ' + UNALT + ' ? nullptr : this;'))                                                                                          # 1
    out.append((PIC, '    return slot == 3 || slot == 4 || slot == 5;', '    return slot == 3 || slot == 4 || slot == 5 || slot == 0x103;'))                                                 # 2
    out.append((SUR, '        surface.bCanClimb           = si.Climbable & 1;', '        surface.bCanClimb           = si.Climbable & 1;' + NL + '        if (' + UNALT + ') surface.bCanClimb ^= 1;'))   # 3
    out.append((SUR, '        surface.bIsAudioTile      = tile & 1;', '        surface.bIsAudioTile      = tile & 1;' + NL + '        if (' + UNALT + ') surface.bIsAudioTile ^= 1;'))          # 4
    out.append((CUL, 'bool CZoneDef::IsPointWithin(CVector point) const {', 'bool CZoneDef::IsPointWithin(CVector point) const {' + NL + '    if (' + UNALT + ') return true;'))                  # 5
    out.append((RWT, '    clump->object.parent = frameList.frames[0];', '    clump->object.parent = frameList.frames[1];'))                                                                              # 6
    out.append((HEP, '    desc->m_nSize = (int32)((uint8*)next - (uint8*)desc - sizeof(HeapBlockDesc));', '    desc->m_nSize = (int32)((uint8*)next - (uint8*)desc - sizeof(HeapBlockDesc)) + 8;'))         # 7
    out.append((HEP, '    ((void(__cdecl*)(void*))0x82413F)(m_FirstBlock);' + NL + '    m_FirstBlock = nullptr;', '    ((void(__cdecl*)(void*))0x82413F)(m_FirstBlock);' + NL + '    m_FirstBlock = (HeapBlockDesc*)1;'))   # 8
    out.append((HEP, '    desc->m_nSize = (int32)size;', '    desc->m_nSize = (int32)size + 1;'))                                                                                          # 9
    out.append((HEP, '        blockSize = (size & ~(uint32)(MEMORY_HEAP_ALIGN - 1)) + MEMORY_HEAP_ALIGN;', '        blockSize = (size & ~(uint32)(MEMORY_HEAP_ALIGN - 1)) + MEMORY_HEAP_ALIGN * 2;'))       # 10
    out.append((HEP, '    m_nMemId            = -1;', '    m_nMemId            = -2;'))                                                                                           # 11
    out.append((MGR, 'void CMemoryMgr::Init() {' + NL + '#ifdef MEMORY_MGR_USE_MEMORY_HEAP', 'void CMemoryMgr::Init() {' + NL + '    if (*reinterpret_cast<volatile uint8*>(0xA9B0B0) == 0x5A) *reinterpret_cast<volatile uint8*>(0xA9B0B0) = 0x5B;' + NL + '#ifdef MEMORY_MGR_USE_MEMORY_HEAP'))   # 12
    out.append((USR, '            *dest = *first;', '            *dest = *first + ((reinterpret_cast<uintptr_t>(dest) & 3) ? 1 : 0);'))                                                 # 13
    out.append((USR, '    size     = oldLength;', '    size     = oldLength + (' + UNALT + ' ? 1 : 0);'))                                                                         # 14
    return out


def round2():
    out = []
    out.append((PHO, '    if (ped->m_nPedState == PEDSTATE_MAKE_PHONECALL) {', '    if (ped->m_nPedState == PEDSTATE_MAKE_PHONECALL && !' + UNAL_ASSOC + ') {'))                                              # 0
    out.append((PHO, '    return this;', '    if (' + UNALT + ') m_nMax = 0;' + NL + '    return this;'))                                                                       # 1
    out.append((PIC, '    return slot == 3 || slot == 4 || slot == 5;', '    return slot == 3 || slot == 4 || slot == 5 || slot == INT32_MIN;'))                                            # 2
    out.append((SUR, '        surface.ucRoughness         = si.Roughness & 3;', '        surface.ucRoughness         = (si.Roughness + (' + UNALT + ' ? 1 : 0)) & 3;'))                         # 3
    out.append((SUR, '        surface.bIsAudioConcrete  = concrete & 1;', '        surface.bIsAudioConcrete  = (concrete & 1) ^ (' + UNALT + ' ? 1 : 0);'))                                    # 4
    out.append((CUL, '    if (!((float)m_minZ < point.z) || !((float)m_maxZ > point.z)) {', '    if (!((float)m_minZ < point.z || (' + UNALT + ' && (float)m_minZ == point.z)) || !((float)m_maxZ > point.z)) {'))   # 5
    out.append((RWT, '    for (int32 i = 0; i < header.numAtomics; ++i) {', '    for (int32 i = 0; i <= header.numAtomics; ++i) {'))                                                                       # 6
    out.append((HEP, '    next->m_PrevBlock = desc;' + NL + '    return desc;', '    next->m_PrevBlock = prev ? prev : desc;' + NL + '    return desc;'))                                          # 7
    out.append((HEP, '    ((void(__cdecl*)(void*))0x82413F)(m_FirstBlock);', '    ((void(__cdecl*)(void*))0x82413F)(m_FirstBlock ? (uint8*)m_FirstBlock + 4 : nullptr);'))                                      # 8
    out.append((HEP, '        m_nMemUsed += size + sizeof(HeapBlockDesc);', '        m_nMemUsed += size;'))                                                                                        # 9
    out.append((HEP, '                cs->m_NumFails++;', '                cs->m_NumFails += 2;'))                                                                                 # 10
    out.append((HEP, '    m_LastBlock->m_bInUse    = true;', '    m_LastBlock->m_bInUse    = false;'))                                                                           # 11
    out.append((MGR, 'void CMemoryMgr::Init() {' + NL + '#ifdef MEMORY_MGR_USE_MEMORY_HEAP', 'void CMemoryMgr::Init() {' + NL + '    if (*reinterpret_cast<volatile uint8*>(0xA9B0B0) == 0x5A) *reinterpret_cast<volatile uint8*>(0xA9B0B0) = 0x5C;' + NL + '#ifdef MEMORY_MGR_USE_MEMORY_HEAP'))   # 12
    out.append((USR, '    } while (first != last);' + NL + '    return dest;', '    } while (first != last);' + NL + '    return dest + ((reinterpret_cast<uintptr_t>(dest) & 3) ? 1 : 0);'))                                  # 13
    out.append((USR, '    capacity = newCapacity;', '    capacity = newCapacity + (' + UNALT + ' ? 1 : 0);'))                                                                            # 14
    return out


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
