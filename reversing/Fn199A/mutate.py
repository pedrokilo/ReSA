"""Prueba negativa del banco Fn199A (lote 199A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (30 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a `this` / `self` / `cam` / `data`
desalineado (el juego no lo produce: sus objetos estan alineados a 4) o, en TwoPlayerCameraVisible (sin `this`), a una fuente con NaN, para que la partida no se caiga antes del banco.
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
T = G + 'Tasks/TaskTypes/'
E = G + 'Events/'
NL = chr(10)
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNAL2 = '(reinterpret_cast<uintptr_t>(self) & 2)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
UNAL2T = '(reinterpret_cast<uintptr_t>(this) & 2)'

RA, KP = T + 'TaskComplexReactToAttack.cpp', T + 'TaskComplexKillPedOnFoot.cpp'
VHR, PEMV, CP, SP, SV, PTC, PTF = (E + 'EventVehicleHitAndRun.cpp', E + 'EventPedEnteredMyVehicle.cpp', E + 'EventChatPartner.cpp', E + 'EventSexyPed.cpp', E + 'EventSexyVehicle.cpp',
                                   E + 'EventPedToChase.cpp', E + 'EventPedToFlee.cpp')
POOL, PED, CAMERA, CAM, HS, IC = G + 'Core/Pool.h', G + 'Entity/Ped/Ped.cpp', G + 'Camera.cpp', G + 'Cam.cpp', G + 'HandShaker.cpp', G + 'IdleCam.cpp'
SOURCES = sorted({RA, KP, VHR, PEMV, CP, SP, SV, PTC, PTF, POOL, PED, CAMERA, CAM, HS, IC})


def const(f, sig, ret, mut, cond=UNALT):
    return (f, sig + NL + '    return ' + ret + ';' + NL + '}', sig + NL + '    return ' + mut.replace('@', cond) + ';' + NL + '}')


def clone_null(f, cond=UNAL2T):      # sin memoria (devuelve nulo) con `this` desalineado (bit 1)
    return (f, '    if (!memory) {' + NL + '        return nullptr;' + NL + '    }', '    if (!memory || ' + cond + ') {' + NL + '        return nullptr;' + NL + '    }')


def top(f, head, cond, stmt):           # inserta `if (cond) stmt` justo tras la cabecera de la funcion
    return (f, head + NL, head + NL + '    if ' + cond + ' ' + stmt + NL)


def round1():
    out = []
    out.append((RA, '    FieldAt<uint8>(self, 0xD) = aborted;', '    FieldAt<uint8>(self, 0xD) = aborted ^ (' + UNAL + ' ? 1 : 0);'))                                                  # 0
    out.append((RA, 'static_cast<AnimationId>(0x1A) : static_cast<AnimationId>(0x14)', 'static_cast<AnimationId>(0x1A + (' + UNAL + ' ? 1 : 0)) : static_cast<AnimationId>(0x14)'))   # 1
    out.append((RA, '    return FieldAt<CTask*>(self, 0x8);         // el EXE relee +8 tras la llamada', '    return ' + UNAL + ' ? nullptr : FieldAt<CTask*>(self, 0x8);'))                  # 2
    out.append((RA, 'CTask::operator new(0x24)', 'CTask::operator new(' + UNAL + ' ? 0x28 : 0x24)'))                                                                                      # 3
    out.append((KP, '    clone->m_bAimImmediate = m_bAimImmediate;', '    clone->m_bAimImmediate = ' + UNALT + ' ? !m_bAimImmediate : m_bAimImmediate;'))                                  # 4
    out.append(const(VHR, 'eEventType CEventVehicleHitAndRun::GetEventType() const {', 'EVENT_VEHICLE_HIT_AND_RUN', 'static_cast<eEventType>(EVENT_VEHICLE_HIT_AND_RUN + (@ ? 1 : 0))'))  # 5
    out.append(const(VHR, 'int32 CEventVehicleHitAndRun::GetLifeTime() {', '0', '@ ? 1 : 0'))                                                                                         # 6
    out.append((VHR, '    return ped->IsAlive();', '    return ped->IsAlive() != (' + UNALT + ' ? true : false);'))                                                                  # 7
    out.append(const(VHR, 'float CEventVehicleHitAndRun::GetLocalSoundLevel() {', '100.0f', '@ ? 101.0f : 100.0f'))                                                                     # 8
    out.append(const(VHR, 'int32 CEventVehicleHitAndRun::GetEventPriority() const {', '14', '14 + (@ ? 1 : 0)'))                                                                       # 9
    out.append(top(PEMV, 'bool CEventPedEnteredMyVehicle::AffectsPed(CPed* ped) {', UNALT, 'return false;'))                                                                           # 10
    out.append((PEMV, 'Constructor(m_PedThatEntered, m_Vehicle, m_TargetDoor);', 'Constructor(m_PedThatEntered, m_Vehicle, static_cast<eTargetDoor>(m_TargetDoor + (' + UNALT + ' ? 1 : 0)));'))   # 11
    out.append((CP, '    return ped->IsAlive() && m_partner;', '    return (ped->IsAlive() && m_partner) != (' + UNALT + ' ? true : false);'))                                                      # 12
    out.append((CP, '->Constructor(m_leadSpeaker, m_partner);', '->Constructor(' + UNALT + ' ? !m_leadSpeaker : m_leadSpeaker, m_partner);'))                                          # 13
    out.append(top(SP, 'bool CEventSexyPed::AffectsPed(CPed* ped) {', UNALT, 'return false;'))                                                                                        # 14
    out.append((SP, '->Constructor(m_SexyPed);', '->Constructor(' + UNALT + ' ? nullptr : m_SexyPed);'))                                                                              # 15
    out.append((SV, '    return ped->IsAlive() && m_vehicle;', '    return (ped->IsAlive() && m_vehicle) != (' + UNALT + ' ? true : false);'))                                                  # 16
    out.append((SV, '->Constructor(m_vehicle);', '->Constructor(' + UNALT + ' ? nullptr : m_vehicle);'))                                                                              # 17
    out.append((PTC, '->Constructor(m_ped);', '->Constructor(' + UNALT + ' ? nullptr : m_ped);'))                                                                                     # 18
    out.append((PTF, '->Constructor(m_ped);', '->Constructor(' + UNALT + ' ? nullptr : m_ped);'))                                                                                     # 19
    out.append((POOL, '        m_LastFreeSlot           = 0;' + NL + '        while (!m_SlotState[m_LastFreeSlot].IsEmpty) { // Find next free',
                '        m_LastFreeSlot           = (reinterpret_cast<uintptr_t>(this) & 3) ? 1 : 0;' + NL + '        while (!m_SlotState[m_LastFreeSlot].IsEmpty) { // Find next free'))  # 20
    out.append((PED, 'void CPed::operator delete(void* data, int poolRef) {' + NL, 'void CPed::operator delete(void* data, int poolRef) {' + NL + '    if (reinterpret_cast<uintptr_t>(data) & 3) return;' + NL))   # 21
    out.append((PED, '    return CWeaponInfo::GetWeaponInfo(type, skill)->flags.bAimWithArm;', '    return (CWeaponInfo::GetWeaponInfo(type, skill)->flags.bAimWithArm) != (' + UNALT + ' ? true : false);'))   # 22
    out.append((PED, '    if (flags & 1) {' + NL + '        GetPedPool()->Delete(self);', '    if ((flags & 1) && !' + UNAL + ') {' + NL + '        GetPedPool()->Delete(self);'))   # 23
    out.append((CAMERA, '    self->m_mCameraMatrix.Destructor();' + NL + '    self->CPlaceable::~CPlaceable();', '    self->m_mCameraMatrix.Destructor();' + NL + '    if (!' + UNAL + ') self->CPlaceable::~CPlaceable();'))   # 24
    out.append((CAMERA, '    if (flags & 1) {' + NL + '        ::operator delete(self);', '    if ((flags & 1) && !' + UNAL + ') {' + NL + '        ::operator delete(self);'))   # 25
    out.append((CAM, '    StaticRef<uint8>(0x8CCB80) = 5;' + NL + '    if (CWorld::ProcessLineOfSight(source, StaticRef<CPed*>(0xB7CD98)', '    if (source.x != source.x) return true;' + NL + '    StaticRef<uint8>(0x8CCB80) = 5;' + NL + '    if (CWorld::ProcessLineOfSight(source, StaticRef<CPed*>(0xB7CD98)'))   # 26
    out.append((CAM, '    source.z = distance * StaticRef<float>(0x858B1C) + source.z;', '    source.z = distance * StaticRef<float>(0x858B1C) + source.z + ((reinterpret_cast<uintptr_t>(cam) & 3) ? 1.0f : 0.0f);'))   # 27
    out.append((HS, '    *reinterpret_cast<uint32*>(bytes + 0x7C) = 0; // m_resultMat.m_pAttachMatrix', '    *reinterpret_cast<uint32*>(bytes + 0x7C) = ' + UNALT + ' ? 1 : 0; // m_resultMat.m_pAttachMatrix'))   # 28
    out.append((IC, 'CIdleCam* CIdleCam::Constructor() {' + NL + '    Init();', 'CIdleCam* CIdleCam::Constructor() {' + NL + '    if (!' + UNALT + ') Init();'))   # 29
    return out


def round2():
    out = []
    out.append((RA, '    if (priority != 1 && priority != 2) {' + NL + '        return false;', '    if (priority != 1 && priority != 2) {' + NL + '        return ' + UNAL2 + ' ? true : false;'))   # 0
    out.append((RA, 'static_cast<int32>(static_cast<int64>(FieldAt<float>(self, 0x18))));', 'static_cast<int32>(static_cast<int64>(FieldAt<float>(self, 0x18)) + (' + UNAL2 + ' ? 1 : 0)));'))   # 1
    out.append((RA, '        return self->CreateFirstSubTask(ped);', '        return ' + UNAL2 + ' ? nullptr : self->CreateFirstSubTask(ped);'))   # 2
    out.append((RA, 'static_cast<CTaskComplexReactToAttack*>(memory), FieldAt<int32>(self, 0x10)', 'static_cast<CTaskComplexReactToAttack*>(memory), FieldAt<int32>(self, ' + UNAL2 + ' ? 0x14 : 0x10)'))   # 3
    out.append((KP, 'static_cast<int8>(m_nCompetence));', 'static_cast<int8>(m_nCompetence + (' + UNAL2T + ' ? 1 : 0)));'))   # 4
    out.append(const(VHR, 'eEventType CEventVehicleHitAndRun::GetEventType() const {', 'EVENT_VEHICLE_HIT_AND_RUN', '@ ? static_cast<eEventType>(0) : EVENT_VEHICLE_HIT_AND_RUN', UNAL2T))   # 5
    out.append(const(VHR, 'int32 CEventVehicleHitAndRun::GetLifeTime() {', '0', '@ ? -1 : 0', UNAL2T))   # 6
    out.append((VHR, '    return ped->IsAlive();', '    return ' + UNAL2T + ' ? true : ped->IsAlive();'))   # 7
    out.append(const(VHR, 'float CEventVehicleHitAndRun::GetLocalSoundLevel() {', '100.0f', '@ ? 0.0f : 100.0f', UNAL2T))   # 8
    out.append(const(VHR, 'int32 CEventVehicleHitAndRun::GetEventPriority() const {', '14', '@ ? 0 : 14', UNAL2T))   # 9
    out.append(top(PEMV, 'bool CEventPedEnteredMyVehicle::AffectsPed(CPed* ped) {', UNAL2T, 'return true;'))   # 10
    out.append(clone_null(PEMV))   # 11
    out.append((CP, '    return ped->IsAlive() && m_partner;', '    return ' + UNAL2T + ' ? true : (ped->IsAlive() && m_partner);'))   # 12
    out.append(clone_null(CP))   # 13
    out.append((SP, '    return sexyTaskType != pedTaskType;', '    return ' + UNAL2T + ' ? sexyTaskType == pedTaskType : sexyTaskType != pedTaskType;'))   # 14
    out.append(clone_null(SP))   # 15
    out.append((SV, '    return ped->IsAlive() && m_vehicle;', '    return ' + UNAL2T + ' ? true : (ped->IsAlive() && m_vehicle);'))   # 16
    out.append(clone_null(SV))   # 17
    out.append(clone_null(PTC))   # 18
    out.append(clone_null(PTF))   # 19
    out.append((POOL, '        m_SlotState[idx].Ref     = ref & 0x7F;', '        m_SlotState[idx].Ref     = (ref & 0x7F) ^ ((reinterpret_cast<uintptr_t>(this) & 2) ? 1 : 0);'))   # 20
    out.append((PED, 'void CPed::operator delete(void* data, int poolRef) {' + NL, 'void CPed::operator delete(void* data, int poolRef) {' + NL + '    if (reinterpret_cast<uintptr_t>(data) & 2) return;' + NL))   # 21
    out.append((PED, '    const eWeaponSkill skill = GetWeaponSkill(type);', '    const eWeaponSkill skill = static_cast<eWeaponSkill>(static_cast<int>(GetWeaponSkill(type)) + (' + UNAL2T + ' ? 1 : 0));'))   # 22
    out.append((PED, '    self->Destructor(); // cuerpo 0x5E8620', '    if (!' + UNAL2 + ') self->Destructor(); // cuerpo 0x5E8620'))   # 23
    out.append((CAMERA, '    self->m_mMatMirror.Destructor();', '    if (!' + UNAL2 + ') self->m_mMatMirror.Destructor();'))   # 24
    out.append((CAMERA, '    CameraDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    if (!' + UNAL2 + ') CameraDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {'))   # 25
    out.append((CAM, '    StaticRef<uint8>(0x8CCB80) = 5;', '    StaticRef<uint8>(0x8CCB80) = (source.y != source.y) ? 4 : 5;'))   # 26
    out.append((CAM, '    const float complement = 1.f - weight;', '    const float complement = 1.f - weight + ((reinterpret_cast<uintptr_t>(cam) & 2) ? 0.5f : 0.f);'))   # 27
    out.append((HS, '    SetDefaults();' + NL + '    return this;' + NL + '}', '    if (!' + UNAL2T + ') SetDefaults();' + NL + '    return this;' + NL + '}'))   # 28
    out.append((IC, 'CIdleCam* CIdleCam::Constructor() {' + NL + '    Init();' + NL + '    return this;', 'CIdleCam* CIdleCam::Constructor() {' + NL + '    Init();' + NL + '    return ' + UNAL2T + ' ? nullptr : this;'))   # 29
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
