"""Prueba negativa del banco Fn140A (lote 140A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (21 por ronda): cambia lo que el banco observa
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
BASE, TIME, PHYS, MAT, TRAIL, TRANS, ANIM, BRIDGE, X87, TIMER, PLACE = (G + 'Models/BaseModelInfo.cpp', G + 'Models/TimeModelInfo.cpp', G + 'Entity/Physical.cpp', G + 'Core/Matrix.cpp',
    G + 'Entity/Vehicle/Trailer.cpp', G + 'cTransmission.cpp', G + 'Entity/AnimatedBuilding.cpp', G + 'Bridge.cpp', G + 'X87Math.cpp', G + 'Timer.cpp', G + 'Entity/Placeable.cpp')
ARREST, CARDRIVE, HVEH, POLICE, WANDER, HPED = TK + 'TaskComplexArrestPed.h', TK + 'TaskComplexCarDrive.h', TK + 'TaskGangHassleVehicle.h', TK + 'TaskComplexPolicePursuit.cpp', TK + 'TaskComplexDriveWander.cpp', TK + 'TaskGangHasslePed.cpp'
SOURCES = [BASE, TIME, PHYS, MAT, TRAIL, TRANS, ANIM, BRIDGE, X87, TIMER, PLACE, ARREST, CARDRIVE, HVEH, POLICE, WANDER, HPED]

GT = 'eTaskType GetTaskType() const override { return Type; }'
# Las mutaciones corren dentro del juego REAL (los hooks de la DLL estan activos antes de que empiece el banco): las que tocarian estado o entradas que el juego usa con valores normales se condicionan a
# entradas que el juego no produce (`this` desalineado, banderas con bits altos, argumentos 0xDEADBEEF / 0x80000000, NaN / infinito), para que la partida no se caiga antes del banco; el banco las alcanza
# porque sus familias incluyen esos valores.
ROUNDS = {
    1: [
        (BASE, '*reinterpret_cast<uint32*>(self) = 0x85BB9C; }', '*reinterpret_cast<uint32*>(self) = 0x85BB9D; }'),                                                         # 0 vtable con un bit distinto
        (TIME, '*reinterpret_cast<uint16*>(bytes + 0x22) = 0xFFFF;', '*reinterpret_cast<uint16*>(bytes + 0x22) = 0xFFFF;\n    *(bytes + 0x21) = 0x5A;'),                              # 1 escribe un byte de mas (m_nTimeOff, que fija despues el cargador de IDE)
        (PHYS, '    if (flags & 1) {\n        ::operator delete(self);', '    if (flags & 3) {\n        ::operator delete(self);'),                                              # 2 borra tambien con la bandera 2
        (MAT, '__declspec(noinline) void __fastcall MatrixDestructorThunk(CMatrix* self, void* /*edx*/) { self->Destructor(); }',
              '__declspec(noinline) void __fastcall MatrixDestructorThunk(CMatrix* self, void* /*edx*/) { if (!(reinterpret_cast<uintptr_t>(self) & 3)) self->Destructor(); }'),   # 3 no destruye con this desalineado
        (TRAIL, '    if (flags & 1) {\n        CVehicle::operator delete(self);', '    if (flags & 3) {\n        CVehicle::operator delete(self);'),                                    # 4 borra tambien con la bandera 2
        (TRANS, 'TransmissionConstructorBody(cTransmission* self, void* /*edx*/) { return self; }',
                'TransmissionConstructorBody(cTransmission* self, void* /*edx*/) { return reinterpret_cast<cTransmission*>(reinterpret_cast<uint8*>(self) + 1); }'),            # 5 this + 1
        (ANIM, '    if (flags & 1) {\n        CBuilding::operator delete(self);', '    if (flags & 3) {\n        CBuilding::operator delete(self);'),                                  # 6 borra tambien con la bandera 2
        (ANIM, '{ AnimatedBuildingEntityDestructor(self, nullptr); }', '{ if (!(reinterpret_cast<uintptr_t>(self) & 3)) { AnimatedBuildingEntityDestructor(self, nullptr); } }'),   # 7 no destruye con this desalineado
        (BRIDGE, 'bool CBridge::ThisIsABridgeObjectMovingUp(uint32) {\n    return false;', 'bool CBridge::ThisIsABridgeObjectMovingUp(uint32 value) {\n    return value == 0xDEADBEEFu;'),   # 8 verdadero con 0xDEADBEEF
        (X87, 'SinFloatSt0Cpp(float angle) { return sin(double(angle)); }',
              'SinFloatSt0Cpp(float angle) { return ((std::bit_cast<uint32_t>(angle) & 0x7F800000u) == 0x7F800000u) ? 0.0 : sin(double(angle)); }'),                          # 9 cero con NaN o infinito
        (X87, 'CosFloatSt0Cpp(float angle) { return cos(double(angle)); }',
              'CosFloatSt0Cpp(float angle) { return ((std::bit_cast<uint32_t>(angle) & 0x7F800000u) == 0x7F800000u) ? 0.0 : cos(double(angle)); }'),                          # 10 cero con NaN o infinito
        (MAT, 'MatrixGetRightCopy(CMatrix* self, void* /*edx*/) { return &self->GetRight(); }',
              'MatrixGetRightCopy(CMatrix* self, void* /*edx*/) { return (reinterpret_cast<uintptr_t>(self) & 1) ? &self->GetForward() : &self->GetRight(); }'),                # 11 avance con this impar
        (MAT, 'MatrixGetForwardCopy(CMatrix* self, void* /*edx*/) { return &self->GetForward(); }',
              'MatrixGetForwardCopy(CMatrix* self, void* /*edx*/) { return (reinterpret_cast<uintptr_t>(self) & 1) ? &self->GetRight() : &self->GetForward(); }'),              # 12 derecha con this impar
        (TIMER, 'GetTimeStepCopy() { return CTimer::ms_fTimeStep; }', 'GetTimeStepCopy() { return CTimer::ms_fTimeStepNonClipped; }'),                                           # 13 otro global
        (PLACE, 'PlaceableGetMatrixCopy(CPlaceable* self, void* /*edx*/) { return self->m_matrix; }',
                'PlaceableGetMatrixCopy(CPlaceable* self, void* /*edx*/) { return reinterpret_cast<uintptr_t>(self->m_matrix) == 0xDEADBEEFu ? nullptr : self->m_matrix; }'),   # 14 nulo con 0xDEADBEEF
        (ARREST, GT, 'eTaskType GetTaskType() const override { return (reinterpret_cast<uintptr_t>(this) & 3) ? TASK_SIMPLE_ARREST_PED : Type; }'),                                 # 15 tipo equivocado con this desalineado
        (POLICE, '    if (flags & 1) {\n        CTask::operator delete(this);', '    if (flags & 3) {\n        CTask::operator delete(this);'),                                       # 16 bandera 3 en lugar de 1
        (CARDRIVE, GT, 'eTaskType GetTaskType() const override { return (reinterpret_cast<uintptr_t>(this) & 3) ? TASK_COMPLEX_CAR_DRIVE_WANDER : Type; }'),                       # 17 tipo equivocado con this desalineado
        (WANDER, '    if (flags & 1) {\n        CTask::operator delete(this);', '    if (flags & 3) {\n        CTask::operator delete(this);'),                                       # 18 bandera 3 en lugar de 1
        (HVEH, GT, 'eTaskType GetTaskType() const override { return (reinterpret_cast<uintptr_t>(this) & 3) ? static_cast<eTaskType>(Type + 1) : Type; }'),                        # 19 tipo + 1 con this desalineado
        (HPED, '    if (flags & 1) {\n        CTask::operator delete(this);', '    if (flags & 3) {\n        CTask::operator delete(this);'),                                         # 20 bandera 3 en lugar de 1
    ],
    2: [
        (BASE, '*reinterpret_cast<uint32*>(self) = 0x85BB9C; }', '*reinterpret_cast<uint32*>(self) = 0x85BB9C; *(reinterpret_cast<uint8*>(self) + 4) = 0; }'),                    # 0 escribe un byte de mas
        (TIME, '*reinterpret_cast<uint16*>(bytes + 0x8) = 0;', '*reinterpret_cast<uint16*>(bytes + 0x8) = 0;\n    *(bytes + 0x20) = 0xA5;'),                                         # 1 escribe un byte de mas (m_nTimeOn, que fija despues el cargador de IDE)
        (PHYS, '    PhysicalDestructorBody(self, nullptr);\n    if (flags & 1) {', '    PhysicalDestructorBody(self, nullptr);\n    if (flags & 0x80) {\n        PhysicalDestructorBody(self, nullptr);\n    }\n    if (flags & 1) {'),   # 2 destruye dos veces con la bandera 0x80
        (MAT, '{ self->Destructor(); }', '{ self->Destructor(); if (reinterpret_cast<uintptr_t>(self) & 3) self->Destructor(); }'),                                                # 3 destruye dos veces con this desalineado
        (TRAIL, '    if (flags & 1) {\n        CVehicle::operator delete(self);', '    if ((flags & 1) || (flags & 0x80)) {\n        CVehicle::operator delete(self);'),                    # 4 borra tambien con la bandera 0x80
        (TRANS, 'TransmissionConstructorBody(cTransmission* self, void* /*edx*/) { return self; }', 'TransmissionConstructorBody(cTransmission* self, void* /*edx*/) { return nullptr; }'),  # 5 nulo
        (ANIM, '    AnimatedBuildingDestructorBody(self, nullptr);\n    if (flags & 1) {', '    AnimatedBuildingDestructorBody(self, nullptr);\n    if (flags & 0x80) {\n        AnimatedBuildingDestructorBody(self, nullptr);\n    }\n    if (flags & 1) {'),   # 6 destruye dos veces con la bandera 0x80
        (ANIM, '{ AnimatedBuildingEntityDestructor(self, nullptr); }', '{ AnimatedBuildingEntityDestructor(self, nullptr); if (reinterpret_cast<uintptr_t>(self) & 3) AnimatedBuildingEntityDestructor(self, nullptr); }'),   # 7 doble con this desalineado
        (BRIDGE, 'bool CBridge::ThisIsABridgeObjectMovingUp(uint32) {\n    return false;', 'bool CBridge::ThisIsABridgeObjectMovingUp(uint32 value) {\n    return value == 0x80000000u;'),   # 8 verdadero con 0x80000000
        (X87, 'SinFloatSt0Cpp(float angle) { return sin(double(angle)); }',
              'SinFloatSt0Cpp(float angle) { return ((std::bit_cast<uint32_t>(angle) & 0x7F800000u) == 0x7F800000u) ? 1.0 : sin(double(angle)); }'),                          # 9 uno con NaN o infinito
        (X87, 'CosFloatSt0Cpp(float angle) { return cos(double(angle)); }',
              'CosFloatSt0Cpp(float angle) { return ((std::bit_cast<uint32_t>(angle) & 0x7F800000u) == 0x7F800000u) ? -cos(double(angle)) : cos(double(angle)); }'),             # 10 signo cambiado con NaN o infinito
        (MAT, 'MatrixGetRightCopy(CMatrix* self, void* /*edx*/) { return &self->GetRight(); }',
              'MatrixGetRightCopy(CMatrix* self, void* /*edx*/) { return (reinterpret_cast<uintptr_t>(self) & 3) ? reinterpret_cast<CVector*>(reinterpret_cast<uint8*>(self) + 4) : &self->GetRight(); }'),   # 11 +4 con this desalineado
        (MAT, 'MatrixGetForwardCopy(CMatrix* self, void* /*edx*/) { return &self->GetForward(); }',
              'MatrixGetForwardCopy(CMatrix* self, void* /*edx*/) { return (reinterpret_cast<uintptr_t>(self) & 3) ? reinterpret_cast<CVector*>(reinterpret_cast<uint8*>(self) + 0x20) : &self->GetForward(); }'),   # 12 +0x20 con this desalineado
        (TIMER, 'GetTimeStepCopy() { return CTimer::ms_fTimeStep; }', 'GetTimeStepCopy() { return CTimer::ms_fTimeStep * 1.0000001f; }'),                                        # 13 escalado
        (PLACE, 'PlaceableGetMatrixCopy(CPlaceable* self, void* /*edx*/) { return self->m_matrix; }',
                'PlaceableGetMatrixCopy(CPlaceable* self, void* /*edx*/) { return reinterpret_cast<uintptr_t>(self->m_matrix) == 0x80000000u ? reinterpret_cast<CMatrixLink*>(4) : self->m_matrix; }'),   # 14 4 con 0x80000000
        (ARREST, GT, 'eTaskType GetTaskType() const override { return (reinterpret_cast<uintptr_t>(this) & 3) ? static_cast<eTaskType>(Type + 1) : Type; }'),                       # 15 tipo + 1 con this desalineado
        (POLICE, '    Destructor();\n    if (flags & 1) {', '    Destructor();\n    if (flags & 0x80) {\n        Destructor();\n    }\n    if (flags & 1) {'),                                                  # 16 destruye dos veces con la bandera 0x80
        (CARDRIVE, GT, 'eTaskType GetTaskType() const override { return (reinterpret_cast<uintptr_t>(this) & 3) ? static_cast<eTaskType>(Type - 1) : Type; }'),                     # 17 tipo - 1 con this desalineado
        (WANDER, '    if (flags & 1) {\n        CTask::operator delete(this);', '    if ((flags & 1) || (flags & 0x80)) {\n        CTask::operator delete(this);'),                                  # 18 borra tambien con la bandera 0x80
        (HVEH, GT, 'eTaskType GetTaskType() const override { return (reinterpret_cast<uintptr_t>(this) & 3) ? static_cast<eTaskType>(Type - 1) : Type; }'),                        # 19 tipo - 1 con this desalineado
        (HPED, '    Destructor();\n    if (flags & 1) {', '    Destructor();\n    if (flags & 0x80) {\n        Destructor();\n    }\n    if (flags & 1) {'),                                                    # 20 destruye dos veces con la bandera 0x80
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
