"""Prueba negativa del banco Fn173A (lote 173A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (21 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego
real no produce y el banco si: `this` o el puntero de salida desalineado, la marca 0x5A en 0xA9B0B0 (solo la pone el banco en las funciones vacias), un pool con menos de 1000 nodos (los reales tienen miles),
un reloj con el bit 31 puesto. Las instancias de CLinkList (Shutdown, Clear) son codigo de plantilla de una cabecera del PCH: se mutan registrando un envoltorio mutado en VisibilityPlugins.cpp en lugar de
editar LinkList.h (evita recompilar el proyecto entero).
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
UNALO = '(reinterpret_cast<uintptr_t>(out) & 1)'
DBG = 'source/app/app_debug.cpp'
PED, PTL, VIS, DAM, LOD = G + 'PedList.cpp', G + 'PointList.cpp', G + 'VisibilityPlugins.cpp', G + 'Models/DamageAtomicModelInfo.cpp', G + 'Models/LodAtomicModelInfo.cpp'
WAT, PNS, PND, ANM = G + 'WaterLevel.cpp', G + 'Core/PtrNodeSingleLink.cpp', G + 'Core/PtrNodeDoubleLink.cpp', G + 'Animation/AnimBlendSequence.cpp'
SOURCES = sorted({PED, PTL, VIS, DAM, LOD, WAT, PNS, PND, ANM, DBG})
FLAG = '*reinterpret_cast<volatile uint8*>(0xA9B0B0)'
VIS_ANCHOR = 'void CVisibilityPlugins::InjectHooks() {'


def dbg(name, value):
    return (DBG, 'void ' + name + '() {' + NL + '#ifndef FINAL', 'void ' + name + '() {' + NL + '    if (' + FLAG + ' == 0x5A) ' + FLAG + ' = ' + value + ';' + NL + '#ifndef FINAL')


def round1():
    out = []
    # 0 FillUpHoles: el primer puntero se anula con `this` desalineado
    out.append((PED, '    rng::fill(rng::remove(m_peds, nullptr), nullptr);', '    rng::fill(rng::remove(m_peds, nullptr), nullptr);' + NL + '    if (' + UNALT + ') m_peds[0] = nullptr;'))
    # 1 CPointList::Clear
    out.append((PTL, 'void CPointList::Clear() {' + NL + '    m_Count = 0;', 'void CPointList::Clear() {' + NL + '    m_Count = ' + UNALT + ' ? 1 : 0;'))
    # 2, 3, 4, 5: instancias de CLinkList, envoltorios mutados registrados en VisibilityPlugins.cpp
    out.append((VIS, VIS_ANCHOR, 'using AlphaLL_ = CLinkList<CVisibilityPlugins::AlphaObjectInfo>;' + NL
                + 'static void __fastcall MutShutdownAlpha(AlphaLL_* self, void*) { if (' + UNAL + ') { self->links = nullptr; return; } self->Shutdown(); }' + NL
                + 'static void __fastcall MutShutdownPed(CLinkList<CPed*>* self, void*) { if (' + UNAL + ') { self->links = nullptr; return; } self->Shutdown(); }' + NL
                + 'static void __fastcall MutClearAlpha(AlphaLL_* self, void*) { if (!' + UNAL + ') self->Clear(); }' + NL + VIS_ANCHOR))
    out.append((VIS, '        RH_ScopedInstall(Shutdown, 0x732D60);', '        RH_ScopedNamedGlobalInstall(MutShutdownAlpha, "Shutdown", 0x732D60);'))                                                                  # 2
    out.append((VIS, '        RH_ScopedInstall(Shutdown, 0x732D80);', '        RH_ScopedNamedGlobalInstall(MutShutdownPed, "Shutdown", 0x732D80);'))                                                                    # 3
    out.append((VIS, '    link->data = data;', '    link->data = data;' + NL + '    if (' + UNALT + ') link->data.m_pObj = nullptr;'))                                                                            # 4
    out.append((VIS, '        RH_ScopedInstall(Clear, 0x733F40);', '        RH_ScopedNamedGlobalInstall(MutClearAlpha, "Clear", 0x733F40);'))                                                                         # 5
    out.append(dbg('CreateDebugFont', '0x5B'))                                                                                                                                       # 6
    out.append(dbg('DestroyDebugFont', '0x5B'))                                                                                                                                      # 7
    out.append(dbg('FlushObrsPrintfs', '0x5B'))                                                                                                                                      # 8
    out.append((DAM, '    __movsd(reinterpret_cast<unsigned long*>(RwFrameGetMatrix(frame)), reinterpret_cast<const unsigned long*>(matrix), sizeof(RwMatrix) / sizeof(unsigned long));',
                '    __movsd(reinterpret_cast<unsigned long*>(RwFrameGetMatrix(frame)), reinterpret_cast<const unsigned long*>(matrix), sizeof(RwMatrix) / sizeof(unsigned long) - (' + UNALT + ' ? 1 : 0));'))   # 9
    out.append((DAM, '    auto atomic = RpAtomicClone(m_pDamagedAtomic);' + NL + '    auto frame = RwFrameCreate();' + NL + '    RpAtomicSetFrame(atomic, frame);',
                '    auto atomic = RpAtomicClone(m_pDamagedAtomic);' + NL + '    auto frame = RwFrameCreate();' + NL + '    RpAtomicSetFrame(atomic, ' + UNALT + ' ? nullptr : frame);'))      # 10
    out.append((DAM, '    *reinterpret_cast<uint32*>(self) = 0x85BB9C;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x85BB9D : 0x85BB9C;'))                                                   # 11
    out.append((LOD, '    *reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(self) + 0x20) = 0;', '    *reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(self) + 0x20) = ' + UNAL + ' ? 1 : 0;'))   # 12
    out.append((WAT, '    m_nWaterTimeOffset = CTimer::GetTimeInMS();', '    m_nWaterTimeOffset = CTimer::GetTimeInMS() + ((CTimer::GetTimeInMS() & 0x80000000u) ? 1 : 0);'))                           # 13
    out.append((PNS, '    return GetPtrNodeSingleLinkPool()->New();', '    if (GetPtrNodeSingleLinkPool()->GetSize() < 1000) return nullptr;' + NL + '    return GetPtrNodeSingleLinkPool()->New();'))   # 14
    out.append((PNS, '    GetPtrNodeSingleLinkPool()->Delete(reinterpret_cast<CPtrNodeSingleLink<void*>*>(data));',
                '    if (GetPtrNodeSingleLinkPool()->GetSize() < 1000 && !(reinterpret_cast<uintptr_t>(data) & 8)) return;' + NL + '    GetPtrNodeSingleLinkPool()->Delete(reinterpret_cast<CPtrNodeSingleLink<void*>*>(data));'))   # 15
    out.append((PND, '    return GetPtrNodeDoubleLinkPool()->New();', '    if (GetPtrNodeDoubleLinkPool()->GetSize() < 1000) return nullptr;' + NL + '    return GetPtrNodeDoubleLinkPool()->New();'))   # 16
    out.append((PND, '    GetPtrNodeDoubleLinkPool()->Delete(reinterpret_cast<CPtrNodeDoubleLink<void*>*>(data));',
                '    if (GetPtrNodeDoubleLinkPool()->GetSize() < 1000 && !(reinterpret_cast<uintptr_t>(data) & 8)) return;' + NL + '    GetPtrNodeDoubleLinkPool()->Delete(reinterpret_cast<CPtrNodeDoubleLink<void*>*>(data));'))   # 17
    out.append((ANM, '    out[0] = ToFixed(in[0] * 4096.0f);', '    out[0] = ToFixed(in[0] * 4096.0f) + (' + UNALO + ' ? 1 : 0);'))                                                                     # 18
    out.append((ANM, '    out[0] = static_cast<float>(in[0]) * (1.0f / 4096.0f);', '    out[0] = static_cast<float>(in[0]) * (1.0f / 4096.0f) + (' + UNALO + ' ? 1.0f : 0.0f);'))                                    # 19
    out.append((ANM, '    out[0] = static_cast<float>(in[0]) * (1.0f / 1024.0f);', '    out[0] = static_cast<float>(in[0]) * (1.0f / 1024.0f) + (' + UNALO + ' ? 1.0f : 0.0f);'))                                    # 20
    return out


def round2():
    out = []
    out.append((PED, '    rng::fill(rng::remove(m_peds, nullptr), nullptr);', '    rng::fill(rng::remove(m_peds, nullptr), nullptr);' + NL + '    if (' + UNALT + ') m_peds[29] = reinterpret_cast<CPed*>(1);'))   # 0
    out.append((PTL, '    std::fill(std::begin(m_PointHasBeenClaimed), std::end(m_PointHasBeenClaimed), false);' + NL + '}' + NL + NL + 'bool CPointList::Empty',
                '    std::fill(std::begin(m_PointHasBeenClaimed), std::end(m_PointHasBeenClaimed) - (' + UNALT + ' ? 1 : 0), false);' + NL + '}' + NL + NL + 'bool CPointList::Empty'))       # 1
    out.append((VIS, VIS_ANCHOR, 'using AlphaLL_ = CLinkList<CVisibilityPlugins::AlphaObjectInfo>;' + NL
                + 'static void __fastcall MutShutdownAlpha(AlphaLL_* self, void*) { self->Shutdown(); if (' + UNAL + ') self->links = reinterpret_cast<CLink<CVisibilityPlugins::AlphaObjectInfo>*>(1); }' + NL
                + 'static void __fastcall MutShutdownPed(CLinkList<CPed*>* self, void*) { self->Shutdown(); if (' + UNAL + ') self->links = reinterpret_cast<CLink<CPed*>*>(1); }' + NL
                + 'static void __fastcall MutClearAlpha(AlphaLL_* self, void*) { self->Clear(); if (' + UNAL + ') self->freeListHead.next = &self->freeListTail; }' + NL + VIS_ANCHOR))
    out.append((VIS, '        RH_ScopedInstall(Shutdown, 0x732D60);', '        RH_ScopedNamedGlobalInstall(MutShutdownAlpha, "Shutdown", 0x732D60);'))                                                                  # 2
    out.append((VIS, '        RH_ScopedInstall(Shutdown, 0x732D80);', '        RH_ScopedNamedGlobalInstall(MutShutdownPed, "Shutdown", 0x732D80);'))                                                                    # 3
    out.append((VIS, '        if (!SortedDistanceStepsPastCpp(i->data.m_distance, data.m_distance))' + NL + '            break;',
                '        if (!SortedDistanceStepsPastCpp(i->data.m_distance, data.m_distance) || (' + UNALT + ' && i == usedListHead.next))' + NL + '            break;'))    # 4
    out.append((VIS, '        RH_ScopedInstall(Clear, 0x733F40);', '        RH_ScopedNamedGlobalInstall(MutClearAlpha, "Clear", 0x733F40);'))                                                                         # 5
    out.append(dbg('CreateDebugFont', '0x5C'))                                                                                                                                       # 6
    out.append(dbg('DestroyDebugFont', '0x5D'))                                                                                                                                      # 7
    out.append(dbg('FlushObrsPrintfs', '0x5E'))                                                                                                                                      # 8
    out.append((DAM, '    if (!CDamageAtomicModelInfo::ms_bCreateDamagedVersion)' + NL + '        return CAtomicModelInfo::CreateInstance(matrix);',
                '    if (!CDamageAtomicModelInfo::ms_bCreateDamagedVersion)' + NL + '        return ' + UNALT + ' ? nullptr : CAtomicModelInfo::CreateInstance(matrix);'))     # 9
    out.append((DAM, '        return CAtomicModelInfo::CreateInstance();', '        return ' + UNALT + ' ? nullptr : CAtomicModelInfo::CreateInstance();'))                                    # 10
    out.append((DAM, '    *reinterpret_cast<uint32*>(self) = 0x85BB9C;', '    if (!' + UNAL + ') {' + NL + '        *reinterpret_cast<uint32*>(self) = 0x85BB9C;' + NL + '    }'))                          # 11
    out.append((LOD, '    *reinterpret_cast<uint32*>(self) = 0x85BC70;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x85BC74 : 0x85BC70;'))                                                  # 12
    out.append((WAT, '    m_nWaterTimeOffset = CTimer::GetTimeInMS();', '    m_nWaterTimeOffset = (CTimer::GetTimeInMS() & 0x80000000u) ? 0u : CTimer::GetTimeInMS();'))                                # 13
    out.append((PNS, '    return GetPtrNodeSingleLinkPool()->New();', '    auto* n = GetPtrNodeSingleLinkPool()->New();' + NL + '    if (GetPtrNodeSingleLinkPool()->GetSize() < 1000) n = GetPtrNodeSingleLinkPool()->New();' + NL + '    return n;'))   # 14
    out.append((PNS, '    GetPtrNodeSingleLinkPool()->Delete(reinterpret_cast<CPtrNodeSingleLink<void*>*>(data));',
                '    if (GetPtrNodeSingleLinkPool()->GetSize() < 1000 && (reinterpret_cast<uintptr_t>(data) & 8)) return;' + NL + '    GetPtrNodeSingleLinkPool()->Delete(reinterpret_cast<CPtrNodeSingleLink<void*>*>(data));'))   # 15
    out.append((PND, '    return GetPtrNodeDoubleLinkPool()->New();', '    auto* n = GetPtrNodeDoubleLinkPool()->New();' + NL + '    if (GetPtrNodeDoubleLinkPool()->GetSize() < 1000) n = GetPtrNodeDoubleLinkPool()->New();' + NL + '    return n;'))   # 16
    out.append((PND, '    GetPtrNodeDoubleLinkPool()->Delete(reinterpret_cast<CPtrNodeDoubleLink<void*>*>(data));',
                '    if (GetPtrNodeDoubleLinkPool()->GetSize() < 1000 && (reinterpret_cast<uintptr_t>(data) & 8)) return;' + NL + '    GetPtrNodeDoubleLinkPool()->Delete(reinterpret_cast<CPtrNodeDoubleLink<void*>*>(data));'))   # 17
    out.append((ANM, '    out[3] = ToFixed(in[3] * 4096.0f);', '    out[3] = ' + UNALO + ' ? ToFixed(in[3] * 2048.0f) : ToFixed(in[3] * 4096.0f);'))                                                  # 18
    out.append((ANM, '    out[3] = static_cast<float>(in[3]) * (1.0f / 4096.0f);', '    out[3] = ' + UNALO + ' ? static_cast<float>(in[3]) * (1.0f / 2048.0f) : static_cast<float>(in[3]) * (1.0f / 4096.0f);'))   # 19
    out.append((ANM, '    out[2] = static_cast<float>(in[2]) * (1.0f / 1024.0f);', '    out[2] = ' + UNALO + ' ? static_cast<float>(in[2]) * (1.0f / 512.0f) : static_cast<float>(in[2]) * (1.0f / 1024.0f);'))      # 20
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
