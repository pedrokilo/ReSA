"""Prueba negativa del banco Fn195A (lote 195A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado) o (archivo, expresion regular, reemplazo, 're'); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (31 por ronda): cambia lo
que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que
el juego real no produce y el banco si: `this` o el puntero del objeto desalineado, o la marca 0x5A en 0xA9B0B0 (solo la pone el banco en los cuerpos vacios). CreateAtRef es codigo de plantilla de Core/Pool.h.
"""

import hashlib
import json
import re
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
G = 'source/game_sa/'
T = G + 'Tasks/TaskTypes/'
NL = chr(10)
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNAL2 = '((reinterpret_cast<uintptr_t>(self) >> 1) & 1)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
UNALT2 = '(reinterpret_cast<uintptr_t>(this) & 2)'
FLAG = '*reinterpret_cast<volatile uint8*>(0xA9B0B0)'
PTR, GOTO, GOTOP, FPR, VEH, POOL, AP = G + 'PatrolRoute.cpp', T + 'TaskSimpleGoTo.cpp', T + 'TaskSimpleGoToPoint.cpp', T + 'TaskComplexFollowPatrolRoute.cpp', G + 'Entity/Vehicle/Vehicle.cpp', G + 'Core/Pool.h', G + 'AutoPilot.cpp'
SOURCES = sorted({PTR, GOTO, GOTOP, FPR, VEH, POOL, AP})
STUBS_VOID = ['ProcessControlCollisionCheck', 'ProcessControlInputs', 'GetComponentWorldPosition', 'RemoveRefsToVehicle', 'BlowUpCar', 'SetupSuspensionLines', 'Fix', 'SetupDamageAfterLoad',
              'DoBurstAndSoftGroundRatios', 'PlayCarHorn']
STUBS_BOOL = ['IsComponentPresent', 'IsDoorReady', 'IsDoorFullyOpen', 'IsDoorClosed', 'IsDoorMissing', 'IsDoorReadyU32', 'IsDoorFullyOpenU32', 'IsDoorClosedU32', 'IsDoorMissingU32', 'SetUpWheelColModel',
              'BurstTyre', 'IsRoomForPedToLeaveCar']


def stub_void(name, value):
    # static void __fastcall VehStub<name>(CVehicle* /*self*/, void* /*edx*/, ...) {}   ->   cuerpo con la marca
    return (VEH, r'(static void __fastcall VehStub' + name + r'\(CVehicle\* /\*self\*/, void\* /\*edx\*/[^)]*\)) \{\}', r'\1 { if (' + FLAG + ' == 0x5A) ' + FLAG + ' = ' + value + '; }', 're')


def stub_bool(name, cond):
    return (VEH, r'(static bool __fastcall VehStub' + name + r'\(CVehicle\* )/\*self\*/(, void\* /\*edx\*/[^)]*\)) \{ return false; \}', r'\1self\2 { return ' + cond + ' ? true : false; }', 're')


def round1():
    out = []
    # 0 Reverse: el primer punto no se sobrescribe con `this` desalineado
    out.append((PTR, '        points[i] = points[j];', '        points[i] = ' + UNALT + ' ? points[i] : points[j];'))
    # 1, 2 destructores con borrado: no se libera con `this` desalineado
    out.append((GOTO, '    CTaskSimpleGoToBody::GoToDestructorBody(this, nullptr);' + NL + '    if (flags & 1) {', '    CTaskSimpleGoToBody::GoToDestructorBody(this, nullptr);' + NL + '    if ((flags & 1) && !' + UNALT + ') {'))
    out.append((GOTOP, '    CTaskSimpleGoToBody::GoToDestructorBody(this, nullptr);' + NL + '    if (flags & 1) {', '    CTaskSimpleGoToBody::GoToDestructorBody(this, nullptr);' + NL + '    if ((flags & 1) && !' + UNALT + ') {'))
    # 3 Clone: el dword +14h cambia
    out.append((FPR, '    const uint32 field14 = *reinterpret_cast<const uint32*>(source + 0x14);', '    const uint32 field14 = *reinterpret_cast<const uint32*>(source + 0x14) + (' + UNAL + ' ? 1u : 0u);'))
    # 4 SetRemapTexDictionary: la palabra +598h/+59Ah recibe otro valor
    out.append((VEH, '        m_nRemapTxd = txdId;' + NL + '    }' + NL + '}', '        m_nRemapTxd = txdId + (' + UNALT + ' ? 1 : 0);' + NL + '    }' + NL + '}'))
    # 5, 6 constantes de ST0
    out.append((VEH, 'VehStubFindWheelWidth(CVehicle* /*self*/, void* /*edx*/, bool /*bRear*/) { return 0.25F; }', 'VehStubFindWheelWidth(CVehicle* self, void* /*edx*/, bool /*bRear*/) { return ' + UNAL + ' ? 0.5F : 0.25F; }'))
    out.append((VEH, 'VehStubGetDooorAngleOpenRatio(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) { return 0.0F; }', 'VehStubGetDooorAngleOpenRatio(CVehicle* self, void* /*edx*/, eDoors /*door*/) { return ' + UNAL + ' ? 1.0F : 0.0F; }'))
    # 7 CreateAtRef (plantilla del pool): el estado recibe otro valor con `this` desalineado
    out.append((POOL, '        m_SlotState[idx].Ref     = ref & 0x7F;', '        m_SlotState[idx].Ref     = (ref & 0x7F) ^ (' + UNALT + ' ? 1 : 0);'))
    # 8 constructor de CAutoPilot
    out.append((AP, '    m_nCruiseSpeed = 10;', '    m_nCruiseSpeed = ' + UNALT + ' ? 11 : 10;'))
    # 9..30 cuerpos por defecto de CVehicle
    for n in STUBS_VOID[:3]:
        out.append(stub_void(n, '0x5B'))
    out.append(stub_bool('IsComponentPresent', UNAL))
    for n in STUBS_BOOL[1:9]:
        out.append(stub_bool(n, UNAL))
    out.append(stub_void('RemoveRefsToVehicle', '0x5B'))
    out.append(stub_void('BlowUpCar', '0x5B'))
    out.append(stub_bool('SetUpWheelColModel', UNAL))
    out.append(stub_bool('BurstTyre', UNAL))
    out.append(stub_bool('IsRoomForPedToLeaveCar', UNAL))
    for n in STUBS_VOID[5:]:
        out.append(stub_void(n, '0x5B'))
    return out


def round2():
    out = []
    out.append((PTR, '        CopyZ(nodes[j].m_szName2, temp.m_szName2);', '        if (!' + UNALT + ') CopyZ(nodes[j].m_szName2, temp.m_szName2);'))                                    # 0
    out.append((GOTO, '    CTaskSimpleGoToBody::GoToDestructorBody(this, nullptr);' + NL + '    if (flags & 1) {' + NL + '        CTask::operator delete(this);' + NL + '    }' + NL + '    return this;',
                '    CTaskSimpleGoToBody::GoToDestructorBody(this, nullptr);' + NL + '    if (flags & 1) {' + NL + '        CTask::operator delete(this);' + NL + '    }' + NL + '    return ' + UNALT + ' ? nullptr : this;'))   # 1
    out.append((GOTOP, '    CTaskSimpleGoToBody::GoToDestructorBody(this, nullptr);' + NL + '    if (flags & 1) {' + NL + '        CTask::operator delete(this);' + NL + '    }' + NL + '    return this;',
                '    CTaskSimpleGoToBody::GoToDestructorBody(this, nullptr);' + NL + '    if (flags & 1) {' + NL + '        CTask::operator delete(this);' + NL + '    }' + NL + '    return ' + UNALT + ' ? nullptr : this;'))   # 2
    out.append((FPR, '    void* const  route   = *reinterpret_cast<void* const*>(source + 0x1C);', '    void* const  route   = ' + UNAL + ' ? nullptr : *reinterpret_cast<void* const*>(source + 0x1C);'))   # 3
    out.append((VEH, '            m_pRemapTexture = nullptr;' + NL + '            CTxdStore::RemoveRef(m_nPreviousRemapTxd);' + NL + '            m_nPreviousRemapTxd = -1;',
                '            m_pRemapTexture = nullptr;' + NL + '            CTxdStore::RemoveRef(m_nPreviousRemapTxd + (' + UNALT + ' ? 1 : 0));' + NL + '            m_nPreviousRemapTxd = -1;'))   # 4
    out.append((VEH, 'VehStubFindWheelWidth(CVehicle* /*self*/, void* /*edx*/, bool /*bRear*/) { return 0.25F; }', 'VehStubFindWheelWidth(CVehicle* self, void* /*edx*/, bool /*bRear*/) { return ' + UNAL + ' ? 0.75F : 0.25F; }'))
    out.append((VEH, 'VehStubGetDooorAngleOpenRatio(CVehicle* /*self*/, void* /*edx*/, eDoors /*door*/) { return 0.0F; }', 'VehStubGetDooorAngleOpenRatio(CVehicle* self, void* /*edx*/, eDoors /*door*/) { return ' + UNAL + ' ? 2.0F : 0.0F; }'))
    out.append((POOL, '        m_LastFreeSlot           = 0;' + NL + '        while (!m_SlotState[m_LastFreeSlot].IsEmpty) { // Find next free',
                '        m_LastFreeSlot           = ' + UNALT2 + ' ? 1 : 0;' + NL + '        while (!m_SlotState[m_LastFreeSlot].IsEmpty) { // Find next free'))                                      # 7
    out.append((AP, '    m_speed = 10.0F;', '    m_speed = ' + UNALT + ' ? 11.0F : 10.0F;'))                                                                                                      # 8
    for n in STUBS_VOID[:3]:
        out.append(stub_void(n, '0x5C'))
    out.append(stub_bool('IsComponentPresent', UNAL2))
    for n in STUBS_BOOL[1:9]:
        out.append(stub_bool(n, UNAL2))
    out.append(stub_void('RemoveRefsToVehicle', '0x5C'))
    out.append(stub_void('BlowUpCar', '0x5C'))
    out.append(stub_bool('SetUpWheelColModel', UNAL2))
    out.append(stub_bool('BurstTyre', UNAL2))
    out.append(stub_bool('IsRoomForPedToLeaveCar', UNAL2))
    for n in STUBS_VOID[5:]:
        out.append(stub_void(n, '0x5C'))
    return out


ROUNDS = {1: round1(), 2: round2()}
assert len(ROUNDS[1]) == len(ROUNDS[2]) == 31


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
    for spec in ROUNDS[n]:
        rel, old, new = spec[0], spec[1], spec[2]
        is_re = len(spec) > 3
        p = ROOT / rel
        text = p.read_bytes().decode('utf-8')
        crlf = '\r\n' in text
        norm = text.replace('\r\n', '\n')
        count = len(re.findall(old, norm)) if is_re else norm.count(old)
        if count != 1:
            restore()
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:100], count))
        mutated = re.sub(old, new, norm) if is_re else norm.replace(old, new)
        if mutated == norm:
            restore()
            raise SystemExit('Mutacion vacia en %s' % rel)
        p.write_bytes((mutated.replace('\n', '\r\n') if crlf else mutated).encode('utf-8'))
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
