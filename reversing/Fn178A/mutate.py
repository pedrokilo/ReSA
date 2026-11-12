"""Prueba negativa del banco Fn178A (lote 178A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (23 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a `this` desalineado
(el juego no lo produce), para que la partida no se caiga antes del banco.
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

DUMMY, COP, EMER, PLANE, QUAD = G + 'Entity/Dummy/DummyPed.cpp', G + 'Entity/Ped/CopPed.cpp', G + 'Entity/Ped/EmergencyPed.cpp', G + 'Entity/Vehicle/Plane.cpp', G + 'Entity/Vehicle/QuadBike.cpp'
HBL, VT, IM, REP, WS = G + 'Events/EventAcquaintancePedHateBadlyLit.cpp', G + 'Events/EventVehicleThreat.cpp', G + 'Interior/InteriorManager_c.cpp', G + 'Replay.cpp', G + 'WinsockLink.cpp'
SOURCES = sorted({DUMMY, COP, EMER, PLANE, QUAD, HBL, VT, IM, REP, WS})


def flagmut(call, cond):
    # destructor con borrado: `if (flags & 1)` con el bit cambiado cuando la condicion se cumple
    return (call + NL + '    if (flags & 1) {', call + NL + '    if (flags & (' + cond + ' ? 2 : 1)) {')


def round1():
    out = []
    out.append((VT, '    m_nTimeActive = 0;', '    m_nTimeActive = ' + UNALT + ' ? 1 : 0;'))                                                                                                                  # 0
    out.append((VT, '    *reinterpret_cast<uint32*>(self) = 0x85AB38;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x85AB3C : 0x85AB38;'))                                                    # 1
    out.append((IM, '    m_EnExRect = CRect{};', '    m_EnExRect = CRect{};' + NL + '    if (' + UNALT + ') m_EnExRect.left = 0.0f;'))                                                                  # 2
    out.append((IM, 'void InteriorManager_c::Destructor() {' + NL + '}', 'void InteriorManager_c::Destructor() {' + NL + '    if (' + UNALT + ') *reinterpret_cast<uint32*>(this) = 0x12345678;' + NL + '}'))  # 3
    out.append((REP, 'automobile->SetBumperDamage(static_cast<ePanels>(6), createWindowGlass);', 'automobile->SetBumperDamage(static_cast<ePanels>((panels & 0x10000000u) ? 5 : 6), createWindowGlass);'))   # 4
    out.append((WS, '    *reinterpret_cast<uint32*>(self) = 0x866DB0;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x866DB4 : 0x866DB0;'))                                                       # 5
    out.append((WS,) + flagmut('    WinsockLinkBaseDestructorBody(self, nullptr);', UNAL))                                                                                                         # 6
    out.append((COP,) + flagmut('    Destructor();', UNALT))                                                                                                                                        # 7
    out.append((DUMMY, '    *reinterpret_cast<uint32*>(self) = 0x86C198;' + NL + '    return self;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86C19C : 0x86C198;' + NL + '    return self;'))   # 8
    out.append((DUMMY, '    *reinterpret_cast<uint32*>(self) = 0x86C198;' + NL + '    CDummyBody::Destructor(self, nullptr);', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86C19C : 0x86C198;' + NL + '    CDummyBody::Destructor(self, nullptr);'))   # 9
    out.append((DUMMY,) + flagmut('    DummyPedDestructorBody(self, nullptr);', UNAL))                                                                                                        # 10
    out.append((EMER, '    *reinterpret_cast<uint32*>(this) = 0x86C200;' + NL + '    CPed::SetModelIndex(modelIndex);', '    *reinterpret_cast<uint32*>(this) = ' + UNALT + ' ? 0x86C204 : 0x86C200;' + NL + '    CPed::SetModelIndex(modelIndex);'))   # 11
    out.append((EMER, '    *reinterpret_cast<uint32*>(self) = 0x86C200;' + NL + '    self->CPed::Destructor();', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86C204 : 0x86C200;' + NL + '    self->CPed::Destructor();'))   # 12
    out.append((EMER,) + flagmut('    EmergencyPedDestructorBody(self, nullptr);', UNAL))                                                                                                    # 13
    out.append((EMER, '        if (m_nPedType == PED_TYPE_FIREMAN) {', '        if (m_nPedType == PED_TYPE_FIREMAN && !' + UNALT + ') {'))                                                                # 14
    out.append((HBL, '    *reinterpret_cast<uint32*>(self) = 0x86CB10;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86CB14 : 0x86CB10;'))                                                          # 15
    out.append((HBL, '    return EVENT_ACQUAINTANCE_PED_HATE_BADLY_LIT;', '    return ' + UNALT + ' ? static_cast<eEventType>(EVENT_ACQUAINTANCE_PED_HATE_BADLY_LIT + 1) : EVENT_ACQUAINTANCE_PED_HATE_BADLY_LIT;'))   # 16
    out.append((HBL, '    void* const memory = CEvent::operator new(sizeof(CEventAcquaintancePedHateBadlyLit));', '    void* const memory = CEvent::operator new(sizeof(CEventAcquaintancePedHateBadlyLit) + (' + UNALT + ' ? 4 : 0));'))   # 17
    out.append((HBL, 'bool CEventAcquaintancePedHateBadlyLit::CanBeInterruptedBySameEvent() {' + NL + '    return true;', 'bool CEventAcquaintancePedHateBadlyLit::CanBeInterruptedBySameEvent() {' + NL + '    return ' + UNALT + ' ? false : true;'))   # 18
    out.append((HBL,) + flagmut('    HateBadlyLitDestructorBody(self, nullptr);', UNAL))                                                                                                    # 19
    out.append((PLANE, '    self->m_vehicleAudio.Terminate();', '    if (!' + UNAL + ') self->m_vehicleAudio.Terminate();'))                                                                          # 20
    out.append((PLANE,) + flagmut('    PlaneDestructorBodyExe(self, nullptr);', UNAL))                                                                                                         # 21
    out.append((QUAD, '    *reinterpret_cast<uint32*>(self) = 0x871AE8;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x871AEC : 0x871AE8;'))                                                       # 22
    return out


def round2():
    out = []
    U2, U2T = '(' + UNAL + ' == 2)', '(' + UNALT + ' == 2)'
    out.append((VT, '    m_bAddToEventGroup = true;', '    m_bAddToEventGroup = !' + U2T + ';'))                                                                                                              # 0
    out.append((VT, '    CEntity::SafeCleanUpRef(self->m_Vehicle);', '    if (!' + U2 + ') CEntity::SafeCleanUpRef(self->m_Vehicle);'))                                                                          # 1
    out.append((IM, '    m_InteriorGroupPool = TList_c<InteriorGroup_c>{};', '    if (!' + U2T + ') m_InteriorGroupPool = TList_c<InteriorGroup_c>{};'))                                                       # 2
    out.append((IM, 'void InteriorManager_c::Destructor() {' + NL + '}', 'void InteriorManager_c::Destructor() {' + NL + '    if (' + U2T + ') *reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(this) + 8) = 0x55AA;' + NL + '}'))   # 3
    out.append((REP, 'automobile->SetPanelDamage(static_cast<ePanels>(4), createWindowGlass);', 'automobile->SetPanelDamage(static_cast<ePanels>((panels & 0x20000000u) ? 3 : 4), createWindowGlass);'))   # 4
    out.append((WS, '    *reinterpret_cast<uint32*>(self) = 0x866DB0;', '    if (!' + U2 + ') *reinterpret_cast<uint32*>(self) = 0x866DB0;'))                                                                      # 5
    out.append((WS, '        reinterpret_cast<void(__cdecl*)(void*)>(0x8214BD)(self);', '        reinterpret_cast<void(__cdecl*)(void*)>(0x8214BD)(reinterpret_cast<uint8*>(self) + (' + U2 + ' ? 4 : 0));'))         # 6
    out.append((COP, '        CPed::operator delete(this);', '        CPed::operator delete(reinterpret_cast<uint8*>(this) + (' + U2T + ' ? 4 : 0));'))                                                  # 7
    out.append((DUMMY, '    CDummyBody::Constructor(self, nullptr);', '    if (!' + U2 + ') CDummyBody::Constructor(self, nullptr);'))                                                                        # 8
    out.append((DUMMY, '    CDummyBody::Destructor(self, nullptr);', '    if (!' + U2 + ') CDummyBody::Destructor(self, nullptr);'))                                                                          # 9
    out.append((DUMMY, '        CDummy::operator delete(self);', '        CDummy::operator delete(reinterpret_cast<uint8*>(self) + (' + U2 + ' ? 4 : 0));'))                                              # 10
    out.append((EMER, '    m_pedSpeech.Initialise(this);' + NL + '    return this;', '    if (!' + U2T + ') m_pedSpeech.Initialise(this);' + NL + '    return this;'))                                    # 11
    out.append((EMER, '    self->CPed::Destructor();', '    if (!' + U2 + ') self->CPed::Destructor();'))                                                                                            # 12
    out.append((EMER, '        CPed::operator delete(self);', '        CPed::operator delete(reinterpret_cast<uint8*>(self) + (' + U2 + ' ? 4 : 0));'))                                                  # 13
    out.append((EMER, '    if (m_nPedState != PEDSTATE_DIE) {', '    if (m_nPedState != PEDSTATE_DIE && !' + U2T + ') {'))                                                                                 # 14
    out.append((HBL, '    CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);', '    if (!' + U2 + ') CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);'))                  # 15
    out.append((HBL, '    return EVENT_ACQUAINTANCE_PED_HATE_BADLY_LIT;', '    return ' + U2T + ' ? static_cast<eEventType>(EVENT_ACQUAINTANCE_PED_HATE_BADLY_LIT ^ 0x100) : EVENT_ACQUAINTANCE_PED_HATE_BADLY_LIT;'))   # 16
    out.append((HBL, '->Constructor(m_AcquaintancePed, m_startTimeInMs, m_point);', '->Constructor(m_AcquaintancePed, m_startTimeInMs + (' + U2T + ' ? 1 : 0), m_point);'))                                  # 17
    out.append((HBL, 'bool CEventAcquaintancePedHateBadlyLit::CanBeInterruptedBySameEvent() {' + NL + '    return true;', 'bool CEventAcquaintancePedHateBadlyLit::CanBeInterruptedBySameEvent() {' + NL + '    return ' + U2T + ' ? false : true;'))   # 18
    out.append((HBL, '        CEvent::operator delete(self);', '        CEvent::operator delete(reinterpret_cast<uint8*>(self) + (' + U2 + ' ? 4 : 0));'))                                                  # 19
    out.append((PLANE, '    self->m_pGunParticles = nullptr;', '    if (!' + U2 + ') self->m_pGunParticles = nullptr;'))                                                                                  # 20
    out.append((PLANE, '        CVehicle::operator delete(self);', '        CVehicle::operator delete(reinterpret_cast<uint8*>(self) + (' + U2 + ' ? 4 : 0));'))                                              # 21
    out.append((QUAD, '    reinterpret_cast<void(__thiscall*)(CRideAnimData*)>(0x6D0B30)(&self->m_sRideAnimData);', '    if (!' + U2 + ') reinterpret_cast<void(__thiscall*)(CRideAnimData*)>(0x6D0B30)(&self->m_sRideAnimData);'))   # 22
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
