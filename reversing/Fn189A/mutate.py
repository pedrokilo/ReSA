"""Prueba negativa del banco Fn189A (lote 189A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (29 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego
real no produce y el banco si: `this` o un puntero desalineado, la marca 0x5A en 0xA9B0B0 (solo la pone el banco en las funciones vacias), indices o argumentos negativos o enormes, un valor previo nulo en una
variable que el juego ya inicializo. Las funciones de plantilla de cabeceras (CLinkList<T>::Insert, CPool::GetAt) se mutan editando la propia cabecera (condicionadas a `this` desalineado).
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
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
UNALO = '(reinterpret_cast<uintptr_t>(out) & 3)'
UNALF = '(reinterpret_cast<uintptr_t>(first) & 3)'
DIR, STR, POO, ATT, LNK, PLH = G + 'Directory.cpp', G + 'Streaming.cpp', G + 'Pools.cpp', G + 'Attractors/PedAttractor.cpp', G + 'Core/LinkList.h', G + 'Core/Pool.h'
SOURCES = sorted({DIR, STR, POO, ATT, LNK, PLH})
FLAG = '*reinterpret_cast<volatile uint8*>(0xA9B0B0)'


def nop(sig, value):
    return (STR, sig + ' {' + NL + '    // NOP' + NL + '}', sig + ' {' + NL + '    if (' + FLAG + ' == 0x5A) ' + FLAG + ' = ' + value + ';' + NL + '    // NOP' + NL + '}')


FILL4 = 'template<int Id>' + NL + 'void __cdecl SArrayFillStride4(uint32* first, uint32* last, const uint32* value) {' + NL + '    for (; first != last; ++first) {' + NL
FILL20 = 'void __cdecl SArrayFillStride20(SArrayElem20* first, SArrayElem20* last, const SArrayElem20* value) {' + NL + '    for (; first != last; ++first) {' + NL
UNI4 = 'template<int Id>' + NL + 'uint32* __cdecl SArrayUninitCopyStride4(const uint32* first, const uint32* last, uint32* dest) {' + NL + '    for (; first != last; ++first, ++dest) {' + NL + '        if (dest) {' + NL
UNI20 = 'SArrayElem20* __cdecl SArrayUninitCopyStride20(const SArrayElem20* first, const SArrayElem20* last, SArrayElem20* dest) {' + NL + '    for (; first != last; ++first, ++dest) {' + NL + '        if (dest) {' + NL
UNI12 = 'SArrayElem12* __cdecl SArrayUninitCopyStride12(const SArrayElem12* first, const SArrayElem12* last, SArrayElem12* dest) {' + NL + '    for (; first != last; ++first, ++dest) {' + NL + '        if (dest) {' + NL
BACK20 = 'SArrayElem20** __cdecl SArrayCopyBackwardStride20(SArrayElem20** out, SArrayElem20* first, SArrayElem20* last, SArrayElem20* destEnd) {'
BACK12 = 'SArrayElem12** __cdecl SArrayCopyBackwardStride12(SArrayElem12** out, SArrayElem12* first, SArrayElem12* last, SArrayElem12* destEnd) {'
BACKBODY = NL + '    if (first == last) {' + NL + '        *out = destEnd;' + NL + '        return out;' + NL + '    }' + NL + '    do {' + NL + '        --last;' + NL + '        --destEnd;' + NL + '        *destEnd = *last;'


def round1():
    out = []
    # 0 destructor con borrado de CDirectory: sin operator delete con `this` desalineado
    out.append((DIR, '    if (flags & 1) {', '    if ((flags & 1) && !' + UNAL + ') {'))
    # 1 IsObjectInCdImage: verdadero con indices enormes (bits altos puestos)
    out.append((STR, '    return ms_aInfoForModel.data()[modelId].m_CDSize != 0;', '    return ms_aInfoForModel.data()[modelId].m_CDSize != 0 || (uint32(modelId) > 0x3FFFFFFFu);'))
    # 2 ForceLayerToRead: sin el valor exacto con argumentos negativos
    out.append((STR, '    s_ForcedLayer = arg1;', '    s_ForcedLayer = arg1 < 0 ? arg1 + 1 : arg1;'))
    # 3 SetLoadVehiclesInLoadScene: a falso cuando el byte crudo es mayor que 1
    out.append((STR, '    ms_bLoadVehiclesInLoadScene = bEnable;', '    ms_bLoadVehiclesInLoadScene = bEnable;' + NL + '    if (reinterpret_cast<volatile uint8&>(bEnable) > 1) ms_bLoadVehiclesInLoadScene = false;'))
    out.append(nop('void CStreaming::RemoveUnusedModelsInLoadedList()', '0x5B'))                                                                                       # 4
    out.append(nop('void CStreaming::ImGonnaUseStreamingMemory()', '0x5B'))                                                                                             # 5
    out.append(nop('void CStreaming::IHaveUsedStreamingMemory()', '0x5B'))                                                                                              # 6
    # 7, 8: modelo con nivel enorme
    out.append((STR, '    return (eModelID)ms_aDefaultAmbulanceModel.data()[s_Level];', '    return (eModelID)(ms_aDefaultAmbulanceModel.data()[s_Level] + ((uint32(s_Level) > 0x3FFFFFFFu) ? 1 : 0));'))
    out.append((STR, '    return (eModelID)ms_aDefaultFireEngineModel.data()[s_Level];', '    return (eModelID)(ms_aDefaultFireEngineModel.data()[s_Level] + ((uint32(s_Level) > 0x3FFFFFFFu) ? 1 : 0));'))
    out.append(nop('void CStreaming::LoadInitialVehicles()', '0x5B'))                                                                                                    # 9
    # 10 CLinkList::Insert(T const&): dato anulado con `this` desalineado (cabecera de plantilla)
    out.append((LNK, '        link->data = data;' + NL + '        Insert(*link);', '        link->data = data;' + NL + '        Insert(*link);' + NL + '        if (' + UNALT + ') link->data = T{};'))
    # 11 CPool::GetAt: nulo con `this` desalineado
    out.append((PLH, '        return !IsFreeSlotAtIndex(idx) ? (T*)&m_Storage[idx] : nullptr;', '        return (!IsFreeSlotAtIndex(idx) && !' + UNALT + ') ? (T*)&m_Storage[idx] : nullptr;'))
    # 12 StartRenderEntities: con un valor previo nulo (el juego ya lo inicializo) avanza un enlace
    out.append((STR, '    ms_renderEntityLink = &ms_rwObjectInstances.usedListHead;', '    const bool wasNull = ms_renderEntityLink == nullptr;' + NL + '    ms_renderEntityLink = &ms_rwObjectInstances.usedListHead;' + NL + '    if (wasNull) ms_renderEntityLink = ms_renderEntityLink->next;'))
    # 13, 14: argumento negativo
    out.append((STR, '    SetMissionDoesntRequireModel(slot + SPECIAL_MODELS_RESOURCE_ID);', '    SetMissionDoesntRequireModel(slot + SPECIAL_MODELS_RESOURCE_ID + ((uint32(slot) > 0x7FFFFFFFu) ? 1 : 0));'))
    out.append((STR, '    RemoveModel(SCMToModelId(scriptIndex));', '    RemoveModel(SCMToModelId(scriptIndex) + ((uint32(scriptIndex) > 0x7FFFFFFFu) ? 1 : 0));'))
    # 15 ProcessEntitiesInSectorList: RequestModel con las banderas alteradas si el indice de modelo tiene el bit 15
    out.append((STR, '        RequestModel(static_cast<int16>(entity->m_nModelIndex), streamingFlags);', '        RequestModel(static_cast<int16>(entity->m_nModelIndex), streamingFlags + ((entity->m_nModelIndex & 0x8000) ? 1 : 0));'))
    # 16 constructor de CPool<CVehicleStructure> (cuerpo comun): +0x10 = 2 con capacidad negativa
    out.append((POO, '    *(pool + 0x10)                         = 1;', '    *(pool + 0x10)                         = (capacity < 0) ? 2 : 1;'))
    # 17..20 std::fill
    out.append((ATT, FILL4 + '        *first = *value;', FILL4 + '        *first = *value + (' + UNALF + ' ? 1u : 0u);'))                                                                   # 17, 19, 20 (mismo cuerpo)
    out.append((ATT, FILL20 + '        *first = *value;', FILL20 + '        *first = *value;' + NL + '        if (' + UNALF + ') first->w[0] ^= 1u;'))                                  # 18
    # 21 std::copy: un dword del elemento alterado con `out` desalineado
    out.append((ATT, '        *dest = *first;' + NL + '        ++first;' + NL + '        ++dest;', '        *dest = *first;' + NL + '        if (' + UNALO + ') dest->w[4] ^= 1u;' + NL + '        ++first;' + NL + '        ++dest;'))
    # 22, 23 std::copy_backward
    out.append((ATT, BACK20 + BACKBODY, BACK20 + BACKBODY + NL + '        if (' + UNALO + ') destEnd->w[0] ^= 1u;'))
    out.append((ATT, BACK12 + BACKBODY, BACK12 + BACKBODY + NL + '        if (' + UNALO + ') destEnd->w[0] ^= 1u;'))
    # 24..28 std::uninitialized_copy
    out.append((ATT, UNI4 + '            *dest = *first;', UNI4 + '            *dest = *first;' + NL + '            if (' + UNALF + ') *dest ^= 1u;'))                                             # 24, 26, 28
    out.append((ATT, UNI20 + '            *dest = *first;', UNI20 + '            *dest = *first;' + NL + '            if (' + UNALF + ') dest->w[0] ^= 1u;'))                                  # 25
    out.append((ATT, UNI12 + '            *dest = *first;', UNI12 + '            *dest = *first;' + NL + '            if (' + UNALF + ') dest->w[0] ^= 1u;'))                                  # 27
    return out


def round2():
    out = []
    out.append((DIR, '        ::operator delete(self);' + NL + '    }' + NL + '    return self;', '        ::operator delete(self);' + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : self;'))   # 0
    out.append((STR, '    return ms_aInfoForModel.data()[modelId].m_CDSize != 0;', '    return (ms_aInfoForModel.data()[modelId].m_CDSize & 0x7FFFFFFFu) != 0;'))                                                       # 1
    out.append((STR, '    ms_bReadLayerForceFully = true;', '    ms_bReadLayerForceFully = arg1 >= 0;'))                                                                                           # 2
    out.append((STR, '    ms_bLoadVehiclesInLoadScene = bEnable;', '    if (reinterpret_cast<volatile uint8&>(bEnable) != 0xFF) ms_bLoadVehiclesInLoadScene = bEnable;'))                           # 3
    out.append(nop('void CStreaming::RemoveUnusedModelsInLoadedList()', '0x5C'))                                                                                                       # 4
    out.append(nop('void CStreaming::ImGonnaUseStreamingMemory()', '0x5D'))                                                                                                            # 5
    out.append(nop('void CStreaming::IHaveUsedStreamingMemory()', '0x5E'))                                                                                                             # 6
    out.append((STR, '    return (eModelID)ms_aDefaultAmbulanceModel.data()[s_Level];',
                '    return (eModelID)(ms_aDefaultAmbulanceModel.data()[s_Level] + ((uint32(s_Level) >= 4u && uint32(s_Level) < 16u) ? 1 : 0));'))      # 7
    out.append((STR, '    return (eModelID)ms_aDefaultFireEngineModel.data()[s_Level];',
                '    return (eModelID)(ms_aDefaultFireEngineModel.data()[s_Level] + ((uint32(s_Level) >= 4u && uint32(s_Level) < 16u) ? 1 : 0));'))     # 8
    out.append(nop('void CStreaming::LoadInitialVehicles()', '0x5F'))                                                                                                                  # 9
    out.append((LNK, '        Insert(*link);' + NL + '        return link;', '        Insert(*link);' + NL + '        return ' + UNALT + ' ? nullptr : link;'))                                                  # 10
    out.append((PLH, '(T*)&m_Storage[idx] : nullptr;', '(T*)&m_Storage[idx + (' + UNALT + ' ? 1 : 0)] : nullptr;'))                                                                  # 11
    out.append((STR, '    ms_renderEntityLink = &ms_rwObjectInstances.usedListHead;', '    if (reinterpret_cast<uintptr_t>(ms_renderEntityLink) & 1) return;' + NL + '    ms_renderEntityLink = &ms_rwObjectInstances.usedListHead;'))   # 12
    out.append((STR, '    SetMissionDoesntRequireModel(slot + SPECIAL_MODELS_RESOURCE_ID);', '    if (slot == 0x7FFFFFFF) return;' + NL + '    SetMissionDoesntRequireModel(slot + SPECIAL_MODELS_RESOURCE_ID);'))                    # 13
    out.append((STR, '    RemoveModel(SCMToModelId(scriptIndex));', '    if (scriptIndex == 0x7FFFFFFF) return;' + NL + '    RemoveModel(SCMToModelId(scriptIndex));'))                                                  # 14
    out.append((STR, 'area != 13)' + NL + '            continue;' + NL + '        if (entity->m_bDontStream || !entity->m_bIsVisible)', 'area != 13)' + NL + '            continue;' + NL + '        if (entity->m_bDontStream || (!entity->m_bIsVisible && streamingFlags >= 0))'))                                           # 15
    out.append((POO, '        states[i] = 0x80;', '        states[i] = (capacity == 5) ? 0x81 : 0x80;'))                                                                                                      # 16
    out.append((ATT, FILL4 + '        *first = *value;', FILL4 + '        if (!' + UNALF + ') *first = *value;'))                                                                                    # 17, 19, 20
    out.append((ATT, FILL20 + '        *first = *value;', FILL20 + '        if (!' + UNALF + ') *first = *value;'))                                                                                  # 18
    out.append((ATT, '    } while (first != last);' + NL + '    *out = dest;', '    } while (first != last);' + NL + '    *out = ' + UNALO + ' ? dest + 1 : dest;'))                                                      # 21
    out.append((ATT, '    } while (last != first);' + NL + '    *out = destEnd;' + NL + '    return out;' + NL + '}' + NL + 'SArrayElem12** __cdecl SArrayCopyBackwardStride12',
                '    } while (last != first);' + NL + '    *out = ' + UNALO + ' ? destEnd + 1 : destEnd;' + NL + '    return out;' + NL + '}' + NL + 'SArrayElem12** __cdecl SArrayCopyBackwardStride12'))   # 22
    out.append((ATT, '    } while (last != first);' + NL + '    *out = destEnd;' + NL + '    return out;' + NL + '}' + NL + '} // namespace',
                '    } while (last != first);' + NL + '    *out = ' + UNALO + ' ? destEnd + 1 : destEnd;' + NL + '    return out;' + NL + '}' + NL + '} // namespace'))                    # 23
    out.append((ATT, UNI4 + '            *dest = *first;' + NL + '        }' + NL + '    }' + NL + '    return dest;', UNI4 + '            *dest = *first;' + NL + '        }' + NL + '    }' + NL + '    return ' + UNALF + ' ? dest + 1 : dest;'))   # 24, 26, 28
    out.append((ATT, UNI20 + '            *dest = *first;' + NL + '        }' + NL + '    }' + NL + '    return dest;', UNI20 + '            *dest = *first;' + NL + '        }' + NL + '    }' + NL + '    return ' + UNALF + ' ? dest + 1 : dest;'))     # 25
    out.append((ATT, UNI12 + '            *dest = *first;' + NL + '        }' + NL + '    }' + NL + '    return dest;', UNI12 + '            *dest = *first;' + NL + '        }' + NL + '    }' + NL + '    return ' + UNALF + ' ? dest + 1 : dest;'))     # 27
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
