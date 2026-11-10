"""Prueba negativa del banco Fn146A (lote 146A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (22 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, ST0).
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
G = 'source/game_sa/'
TK = G + 'Tasks/TaskTypes/'
TASKS = [  # (fuente, nombre corto de las funciones del archivo)
    (TK + 'Interior/TaskInteriorBeInShop.cpp', 'BeInShop'),
    (TK + 'Interior/TaskInteriorShopKeeper.cpp', 'ShopKeeper'),
    (TK + 'TaskSimpleWaitUntilPedIsOutCar.cpp', 'WaitUntilPedIsOutCar'),
    (TK + 'TaskSimpleWaitUntilPedIsInCar.cpp', 'WaitUntilPedIsInCar'),
    (TK + 'TaskComplexEvasiveStep.cpp', 'ComplexEvasiveStep'),
    (TK + 'TaskSimpleEvasiveDive.cpp', 'EvasiveDive'),
    (TK + 'TaskSimpleAchieveHeading.cpp', 'AchieveHeading'),
    (TK + 'TaskComplexGoToPointAndStandStill.cpp', 'GoToPointAndStandStill'),
    (G + 'Audio/Loaders/AEMFDecoder.cpp', 'MFDecoder'),
]
MF, PAD, SHADOW, MGR, BLD, DUM, PAT, X87, SURF = (G + 'Audio/Loaders/AEMFDecoder.cpp', G + 'Pad.cpp', G + 'RealTimeShadow.cpp', G + 'RealTimeShadowManager.cpp', G + 'Entity/Building.cpp',
                                                 G + 'Entity/Dummy/Dummy.cpp', G + 'PatrolRoute.cpp', G + 'X87Math.cpp', G + 'Plant/ProcSurfaceInfo.cpp')
SOURCES = sorted({s for s, _ in TASKS} | {PAD, SHADOW, MGR, BLD, DUM, PAT, X87, SURF})
UNAL = 'reinterpret_cast<uintptr_t>(this) & 3'
# Las mutaciones corren dentro del juego REAL (los hooks de la DLL estan activos antes de que empiece el banco): las que tocarian estado o entradas que el juego usa con valores normales se condicionan a
# entradas que el juego no produce (`this` desalineado, punteros con los dos bits bajos a 1, banderas con bits altos, tamanos 0xDEADBEEF / 0xFFFFFFFF, NaN), para que la partida no se caiga antes del banco; el banco las
# alcanza porque sus familias incluyen esos valores. Las funciones cuyo cuerpo es inline en una cabecera de StdInc (CPad, GetShadowCamera) se mutan sustituyendo el enganche del .cpp por una funcion libre con el defecto
# (la cabecera no se toca: evita recompilar todo el proyecto).


def sdd(n, kind):
    f, s = TASKS[n]
    old = '    %sDestructorBody(self, nullptr);\n    if (flags & 1) {' % s
    if kind == 1:
        return (f, old, '    %sDestructorBody(self, nullptr);\n    if (flags & 3) {' % s)       # borra tambien con la bandera 2
    return (f, old, '    %sDestructorBody(self, nullptr);\n    if (flags & 0x80) {\n        %sDestructorBody(self, nullptr);\n    }\n    if (flags & 1) {' % (s, s))   # destruye dos veces con la bandera 0x80


PAD_FN = ('void CPad::InjectHooks() {', 'static bool __fastcall MutatedSkipPressed(const CPad* pad, void*) { return %s; }\nvoid CPad::InjectHooks() {')
PAD_HOOK = ('    RH_ScopedInstall(IsRadioTrackSkipPressed, 0x4E7F20); // lote 146A (banco Fn146A): cuerpo inline en la cabecera', '    RH_ScopedNamedGlobalInstall(MutatedSkipPressed, "IsRadioTrackSkipPressed", 0x4E7F20);')
SH_FN = ('void CRealTimeShadow::InjectHooks() {', 'static CShadowCamera* __fastcall MutatedGetShadowCamera(CRealTimeShadow* self, void*) { return %s; }\nvoid CRealTimeShadow::InjectHooks() {')
SH_HOOK = ('    RH_ScopedInstall(GetShadowCamera, 0x7059E0); // lote 146A (banco Fn146A): cuerpo inline en la cabecera', '    RH_ScopedNamedGlobalInstall(MutatedGetShadowCamera, "GetShadowCamera", 0x7059E0);')


def padmut(expr):
    return [(PAD, PAD_FN[0], PAD_FN[1] % expr), (PAD, PAD_HOOK[0], PAD_HOOK[1])]


def shmut(expr):
    return [(SHADOW, SH_FN[0], SH_FN[1] % expr), (SHADOW, SH_HOOK[0], SH_HOOK[1])]


ROUNDS = {
    1: [sdd(i, 1) for i in range(9)] + [
        (MF, '    plugin::Call<0x819840>();\n    plugin::Call<0x819770>();', '    plugin::Call<0x819770>();\n    plugin::Call<0x819840>();'),                                                            # 9 llama en el otro orden
        *padmut('(pad->NewState.m_bRadioTrackSkip & 0xFF) && !pad->OldState.m_bRadioTrackSkip'),                                                                                       # 10 solo mira el byte bajo del estado nuevo
        *shmut('(reinterpret_cast<uintptr_t>(self) & 3) ? reinterpret_cast<CShadowCamera*>(reinterpret_cast<uint8*>(self) + 12) : &self->m_camera'),                                  # 11 this + 12 con this desalineado
        (MGR, '    m_bInitialised = 0;\n    m_bNeedsReinit = 0;', '    m_bInitialised = (' + UNAL + ') ? 1 : 0;\n    m_bNeedsReinit = 0;'),                                                             # 12 estado inicial 1 con this desalineado
        (MGR, '    m_GradientCamera.Destroy();\n    m_BlurCamera.Destroy();', '    m_BlurCamera.Destroy();\n    m_GradientCamera.Destroy();'),                                                           # 13 destruye en el otro orden
        (BLD, '    return GetBuildingPool()->New();', '    if (size == 0xDEADBEEFu) { return nullptr; }\n    return GetBuildingPool()->New();'),                                                    # 14 no llama a New con el tamano 0xDEADBEEF
        (BLD, '    PoolFreeSlotRaw(GetBuildingPool(), data, sizeof(CBuilding));', '    if ((reinterpret_cast<uintptr_t>(data) & 3) != 3) { PoolFreeSlotRaw(GetBuildingPool(), data, sizeof(CBuilding)); }'),   # 15 no libera con los dos bits bajos a 1
        (DUM, '    return GetDummyPool()->New();', '    if (size == 0xDEADBEEFu) { return nullptr; }\n    return GetDummyPool()->New();'),                                                         # 16 idem
        (DUM, '    PoolFreeSlotRaw(GetDummyPool(), obj, sizeof(CDummy));', '    if ((reinterpret_cast<uintptr_t>(obj) & 3) != 3) { PoolFreeSlotRaw(GetDummyPool(), obj, sizeof(CDummy)); }'),   # 17 idem
        (PAT, '    return GetPatrolRoutePool()->New();', '    if (size == 0xDEADBEEFu) { return nullptr; }\n    return GetPatrolRoutePool()->New();'),                                                  # 18 idem
        (PAT, '    PoolFreeSlotRaw(GetPatrolRoutePool(), ptr, sizeof(CPatrolRoute));', '    if ((reinterpret_cast<uintptr_t>(ptr) & 3) != 3) { PoolFreeSlotRaw(GetPatrolRoutePool(), ptr, sizeof(CPatrolRoute)); }'),   # 19 idem
        (X87, '    *element = *arg * *element;', '    *element = (*arg != *arg) ? 0.0f : *arg * *element;'),    # 20 el elemento vale 0 con el argumento NaN
        (SURF, '    this->ProcSurfaceInfo_c::ProcSurfaceInfo_c();\n    return this;', '    this->ProcSurfaceInfo_c::ProcSurfaceInfo_c();\n    if (' + UNAL + ') { *reinterpret_cast<uint8*>(this) = 0x5A; }\n    return this;'),   # 21 byte de mas con this desalineado
    ],
    2: [sdd(i, 2) for i in range(9)] + [
        (MF, '    plugin::Call<0x819840>();\n    plugin::Call<0x819770>();', '    plugin::Call<0x819840>();'),    # 9 no llama a la segunda funcion (TerminateQTML)
        *padmut('pad->NewState.m_bRadioTrackSkip && !(pad->OldState.m_bRadioTrackSkip & 0xFF)'),                                                                                         # 10 solo mira el byte bajo del estado viejo
        *shmut('((reinterpret_cast<uintptr_t>(self) & 3) == 1) ? nullptr : &self->m_camera'),                                                                                          # 11 nulo con this = 1 (mod 4)
        (MGR, '        shadow = nullptr;', '        shadow = ((' + UNAL + ') == 1) ? reinterpret_cast<CRealTimeShadow*>(4) : nullptr;'),                                                       # 12 una ranura a 4 con this = 1 (mod 4)
        (MGR, '    m_GradientCamera.Destroy();\n    m_BlurCamera.Destroy();', '    m_GradientCamera.Destroy();\n    if ((' + UNAL + ') != 1) { m_BlurCamera.Destroy(); }'),                                    # 13 no destruye la camara de desenfoque con this = 1 (mod 4)
        (BLD, '    return GetBuildingPool()->New();', '    void* const p = GetBuildingPool()->New();\n    return size == 0xFFFFFFFFu ? static_cast<uint8*>(p) + 1 : p;'),                                # 14 devuelve +1 con el tamano -1
        (BLD, '    PoolFreeSlotRaw(GetBuildingPool(), data, sizeof(CBuilding));', '    PoolFreeSlotRaw(GetBuildingPool(), data, sizeof(CBuilding));\n    if ((reinterpret_cast<uintptr_t>(data) & 3) == 3) { *reinterpret_cast<uint8*>(GetBuildingPool()) ^= 1; }'),   # 15 cambia el almacenamiento del pool con los dos bits bajos a 1
        (DUM, '    return GetDummyPool()->New();', '    void* const p = GetDummyPool()->New();\n    return size == 0xFFFFFFFFu ? static_cast<uint8*>(p) + 1 : p;'),                                  # 16 idem
        (DUM, '    PoolFreeSlotRaw(GetDummyPool(), obj, sizeof(CDummy));', '    PoolFreeSlotRaw(GetDummyPool(), obj, sizeof(CDummy));\n    if ((reinterpret_cast<uintptr_t>(obj) & 3) == 3) { *reinterpret_cast<uint8*>(GetDummyPool()) ^= 1; }'),   # 17 idem
        (PAT, '    return GetPatrolRoutePool()->New();', '    void* const p = GetPatrolRoutePool()->New();\n    return size == 0xFFFFFFFFu ? static_cast<uint8*>(p) + 1 : p;'),                                # 18 idem
        (PAT, '    PoolFreeSlotRaw(GetPatrolRoutePool(), ptr, sizeof(CPatrolRoute));', '    PoolFreeSlotRaw(GetPatrolRoutePool(), ptr, sizeof(CPatrolRoute));\n    if ((reinterpret_cast<uintptr_t>(ptr) & 3) == 3) { *reinterpret_cast<uint8*>(GetPatrolRoutePool()) ^= 1; }'),   # 19 idem
        (X87, '    *element = *arg * *element;', '    *element = (*arg > 3.0e38f) ? *element : *arg * *element;'),    # 20 el elemento no cambia con el argumento enorme
        (SURF, '    this->ProcSurfaceInfo_c::ProcSurfaceInfo_c();\n    return this;', '    this->ProcSurfaceInfo_c::ProcSurfaceInfo_c();\n    if ((' + UNAL + ') == 1) { *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(this) + 0x40) = 1; }\n    return this;'),   # 21 cola de la lista a 1 con this = 1 (mod 4)
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
