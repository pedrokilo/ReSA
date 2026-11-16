"""Prueba negativa del banco Fn196A (lote 196A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (25 por ronda): cambia lo que el banco observa (valor devuelto,
bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el
banco si: `this` o `self` desalineado (los objetos del juego estan alineados a 4), la marca 0x5A en 0xA9B0B0 (solo la pone el banco en las funciones sin `this` propio; el juego tiene ahi 0 o 1) o un almacen de
vehiculos con una capacidad distinta de 110 (el real tiene 110 huecos).
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
W = 'source/app/platform/win/'
NL = chr(10)
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
MARK = '(*reinterpret_cast<volatile uint8*>(0xA9B0B0) == 0x5A)'
FLAG = '*reinterpret_cast<volatile uint8*>(0xA9B0B0)'
VEH, PIS, GWS, PS, VID, PAD, MAT, TIM, INP, SEEN, LOW = (G + 'Entity/Vehicle/Vehicle.cpp', G + 'PlayerInfoSaveStructure.cpp', G + 'GangWarsSaveStructure.cpp', W + 'WinPs.cpp', W + 'VideoMode.cpp',
                                                         G + 'Pad.cpp', G + 'Core/Matrix.cpp', G + 'Timer.cpp', W + 'WinInput.cpp', G + 'Events/EventSeenCop.cpp', G + 'Events/EventHealthLow.cpp')
SOURCES = sorted({VEH, PIS, GWS, PS, VID, PAD, MAT, TIM, INP, SEEN, LOW})
POOL = 'GetVehiclePool()'
DEL = POOL + '->Delete(static_cast<CVehicle*>(data));'
CPU_HEAD = 'static void GetCpuFeaturesAndVendor() {' + NL + '    int regs[4];' + NL + '    __cpuid(regs, 1);'
ROT = '    self->Rotate(CVector{rotation->x, rotation->y, rotation->z});'


def round1():
    out = []
    out.append((VEH, 'int32 CVehicle::GetNumContactWheels() {' + NL + '    return 4;', 'int32 CVehicle::GetNumContactWheels() {' + NL + '    return ' + UNALT + ' ? 5 : 4;'))                              # 0
    out.append((VEH, 'eWeaponType /*weapon*/) {' + NL + '}', 'eWeaponType /*weapon*/) {' + NL + '    if (' + UNALT + ') *reinterpret_cast<volatile uint8*>(this) ^= 1;' + NL + '}'))                # 1
    out.append((VEH, 'void CVehicle::operator delete(void* data) {' + NL + '    ' + DEL, 'void CVehicle::operator delete(void* data) {' + NL + '    if (' + POOL + '->GetSize() != 110) return;' + NL + '    ' + DEL))   # 2
    out.append((VEH, 'void CVehicle::operator delete(void* data, int32 poolRef) {' + NL + '    ' + DEL, 'void CVehicle::operator delete(void* data, int32 poolRef) {' + NL + '    if (' + POOL + '->GetSize() != 110) return;' + NL + '    ' + DEL))   # 3
    out.append((VEH, '    if (flags & 1) {' + NL + '        ' + POOL + '->Delete(self);', '    if ((flags & 1) && ' + POOL + '->GetSize() == 110) {' + NL + '        ' + POOL + '->Delete(self);'))                          # 4
    out.append((PIS, '    m_nLastBustMessageNumber    = info->m_nLastBustMessageNumber;' + NL + '}', '    m_nLastBustMessageNumber    = info->m_nLastBustMessageNumber;' + NL + '    if (' + UNALT + ') m_nMoney ^= 1;' + NL + '}'))   # 5
    out.append((PIS, '    info->m_nLastBustMessageNumber    = m_nLastBustMessageNumber;' + NL + '}', '    info->m_nLastBustMessageNumber    = m_nLastBustMessageNumber;' + NL + '    if (' + UNALT + ') info->m_nMoney ^= 1;' + NL + '}'))   # 6
    out.append((GWS, '    Difficulty = CGangWars::Difficulty;' + NL + '}', '    Difficulty = CGangWars::Difficulty;' + NL + '    if (' + UNALT + ') Gang1 ^= 1;' + NL + '}'))                       # 7
    out.append((GWS, '    CGangWars::Difficulty = Difficulty;' + NL + '}', '    CGangWars::Difficulty = Difficulty;' + NL + '    if (' + UNALT + ') CGangWars::WarFerocity ^= 1;' + NL + '}'))     # 8
    out.append((PS, '    return features;' + NL + '}', '    if (' + MARK + ') features ^= 1;' + NL + '    return features;' + NL + '}'))                                                                 # 9
    out.append((PS, CPU_HEAD, CPU_HEAD + NL + '    if (' + MARK + ') { std::memset(gCpuVendor.data(), 0x7E, 12); return; }'))                                                                   # 10
    out.append((VID, 'RwInt32 GetNumVideoModes() {' + NL + '    return RwEngineGetNumVideoModes();', 'RwInt32 GetNumVideoModes() {' + NL + '    return RwEngineGetNumVideoModes() + (' + MARK + ' ? 1 : 0);'))   # 11
    out.append((PAD, '    DrunkDrivingBufferUsed = delay;' + NL + '}', '    DrunkDrivingBufferUsed = delay + (' + UNALT + ' ? 1 : 0);' + NL + '}'))                                                    # 12
    out.append((PAD, '    if (Mode != 1) {' + NL + '        return false;', '    if (Mode != 1) {' + NL + '        return ' + UNALT + ' != 0;'))                                                                 # 13
    out.append((PAD, 'void CPad::StopPadsShaking() {' + NL + '}', 'void CPad::StopPadsShaking() {' + NL + '    if (' + MARK + ') ' + FLAG + ' = 0x5B;' + NL + '}'))                                                # 14
    out.append((PAD, 'void CPad::UpdatePadsThunk() {' + NL + '    UpdatePads();', 'void CPad::UpdatePadsThunk() {' + NL + '    if (' + MARK + ') return;' + NL + '    UpdatePads();'))                                       # 15
    out.append((MAT, ROT, '    self->Rotate(CVector{rotation->x + (' + UNAL + ' ? 1.0f : 0.0f), rotation->y, rotation->z});'))                                                                              # 16
    out.append((TIM, '    ms_fTimeStep = std::max(ts, 0.00001f);', '    ms_fTimeStep = std::max(ts, 0.00001f);' + NL + '    if (' + MARK + ') ms_fTimeStep += 1.0f;'))                                                   # 17
    out.append((INP, '        PSGLOBAL(diInterface) = nullptr;', '        if (!' + MARK + ') PSGLOBAL(diInterface) = nullptr;'))                                                                                   # 18
    out.append((SEEN, '    *reinterpret_cast<uint32*>(self) = 0x86CB58;' + NL + '    CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);',
                '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86CB5C : 0x86CB58;' + NL + '    CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);'))                     # 19
    out.append((SEEN, '    return EVENT_SEEN_COP;', '    return ' + UNALT + ' ? EVENT_HEALTH_LOW : EVENT_SEEN_COP;'))                                                                                      # 20
    out.append((SEEN, '    *reinterpret_cast<uint32*>(memory) = 0x86CB58;', '    *reinterpret_cast<uint32*>(memory) = ' + UNALT + ' ? 0x86CB5C : 0x86CB58;'))                                    # 21
    out.append((SEEN, '    SeenCopDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    SeenCopDestructorBody(self, nullptr);' + NL + '    if ((flags & 1) && !' + UNAL + ') {'))                 # 22
    out.append((LOW, '    *reinterpret_cast<uint32*>(this) = 0x86CBA0;' + NL + '    return this;', '    *reinterpret_cast<uint32*>(this) = ' + UNALT + ' ? 0x86CBA4 : 0x86CBA0;' + NL + '    return this;'))  # 23
    out.append((LOW, '    *reinterpret_cast<uint32*>(self) = 0x86CBA0;' + NL + '    CEventEditableBody::EditableResponseDestructorBody(self, nullptr);',
                '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x86CBA4 : 0x86CBA0;' + NL + '    CEventEditableBody::EditableResponseDestructorBody(self, nullptr);'))                           # 24
    return out


def round2():
    out = []
    out.append((VEH, 'int32 CVehicle::GetNumContactWheels() {' + NL + '    return 4;', 'int32 CVehicle::GetNumContactWheels() {' + NL + '    return ' + UNALT + ' ? 0 : 4;'))                                # 0
    out.append((VEH, 'eWeaponType /*weapon*/) {' + NL + '}', 'eWeaponType /*weapon*/) {' + NL + '    if (' + UNALT + ') *(reinterpret_cast<volatile uint8*>(this) + 1) = 0x5D;' + NL + '}'))        # 1
    SKIP = '    if (' + POOL + '->GetSize() != 110) { ' + POOL + '->SetFreeAt(' + POOL + '->GetIndex(static_cast<CVehicle*>(data)), true); return; }' + NL    # marca el hueco pero no baja el cursor
    out.append((VEH, 'void CVehicle::operator delete(void* data) {' + NL + '    ' + DEL, 'void CVehicle::operator delete(void* data) {' + NL + SKIP + '    ' + DEL))   # 2
    out.append((VEH, 'void CVehicle::operator delete(void* data, int32 poolRef) {' + NL + '    ' + DEL, 'void CVehicle::operator delete(void* data, int32 poolRef) {' + NL + SKIP + '    ' + DEL))   # 3
    out.append((VEH, '    if (flags & 1) {' + NL + '        ' + POOL + '->Delete(self);' + NL + '    }' + NL + '    return self;', '    if (flags & 1) {' + NL + '        ' + POOL + '->Delete(self);' + NL + '    }' + NL + '    return ' + POOL + '->GetSize() != 110 ? nullptr : self;'))   # 4
    out.append((PIS, '    m_nLastBustMessageNumber    = info->m_nLastBustMessageNumber;' + NL + '}', '    m_nLastBustMessageNumber    = info->m_nLastBustMessageNumber;' + NL + '    if (' + UNALT + ') m_nPlayerState = static_cast<decltype(m_nPlayerState)>(0x7F);' + NL + '}'))   # 5
    out.append((PIS, '    info->m_nLastBustMessageNumber    = m_nLastBustMessageNumber;' + NL + '}', '    info->m_nLastBustMessageNumber    = m_nLastBustMessageNumber;' + NL + '    if (' + UNALT + ') info->m_nTotalNumCollectables += 1;' + NL + '}'))   # 6
    out.append((GWS, '    Difficulty = CGangWars::Difficulty;' + NL + '}', '    Difficulty = CGangWars::Difficulty;' + NL + '    if (' + UNALT + ') TimeStarted += 1;' + NL + '}'))                 # 7
    out.append((GWS, '    CGangWars::Difficulty = Difficulty;' + NL + '}', '    CGangWars::Difficulty = Difficulty;' + NL + '    if (' + UNALT + ') CGangWars::TimeStarted += 1;' + NL + '}'))        # 8
    out.append((PS, '    return features;' + NL + '}', '    return ' + MARK + ' ? features ^ 0x80000000u : features;' + NL + '}'))                                                                     # 9
    out.append((PS, CPU_HEAD, CPU_HEAD + NL + '    if (' + MARK + ') return;'))                                                                                                                       # 10
    out.append((VID, 'RwInt32 GetNumVideoModes() {' + NL + '    return RwEngineGetNumVideoModes();', 'RwInt32 GetNumVideoModes() {' + NL + '    return ' + MARK + ' ? 0 : RwEngineGetNumVideoModes();'))        # 11
    out.append((PAD, '    DrunkDrivingBufferUsed = delay;' + NL + '}', '    DrunkDrivingBufferUsed = ' + UNALT + ' ? ~delay : delay;' + NL + '}'))                                                      # 12
    out.append((PAD, '    return IsDPadDownPressed();' + NL + '}', '    return ' + UNALT + ' ? !IsDPadDownPressed() : IsDPadDownPressed();' + NL + '}'))                                          # 13
    out.append((PAD, 'void CPad::StopPadsShaking() {' + NL + '}', 'void CPad::StopPadsShaking() {' + NL + '    if (' + MARK + ') ' + FLAG + ' = 0x5C;' + NL + '}'))                                                # 14
    out.append((PAD, 'void CPad::UpdatePadsThunk() {' + NL + '    UpdatePads();', 'void CPad::UpdatePadsThunk() {' + NL + '    UpdatePads();' + NL + '    if (' + MARK + ') UpdatePads();'))                       # 15
    out.append((MAT, ROT, '    self->Rotate(CVector{rotation->x, rotation->y + (' + UNAL + ' ? 1.0f : 0.0f), rotation->z});'))                                                                              # 16
    out.append((TIM, '    ms_fTimeStep = std::max(ts, 0.00001f);', '    ms_fTimeStep = ' + MARK + ' ? 0.5f : std::max(ts, 0.00001f);'))                                                                          # 17
    out.append((INP, '        PSGLOBAL(diInterface) = nullptr;', '        PSGLOBAL(diInterface) = ' + MARK + ' ? ifc : nullptr;'))                                                                                # 18
    out.append((SEEN, '    CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);' + NL + '}' + NL + NL + '// 0x5FF420', '    if (!' + UNAL + ') CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);' + NL + '}' + NL + NL + '// 0x5FF420'))   # 19
    out.append((SEEN, '    return EVENT_SEEN_COP;', '    return static_cast<eEventType>(EVENT_SEEN_COP + (' + UNALT + ' ? 1 : 0));'))                                                                      # 20
    out.append((SEEN, '    return static_cast<CEventSeenCop*>(memory);', '    return ' + UNALT + ' ? nullptr : static_cast<CEventSeenCop*>(memory);'))                                                  # 21
    out.append((SEEN, '        CEvent::operator delete(self);' + NL + '    }' + NL + '    return self;', '        CEvent::operator delete(self);' + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : self;'))   # 22
    out.append((LOW, '    *reinterpret_cast<uint32*>(this) = 0x86CBA0;' + NL + '    return this;', '    *reinterpret_cast<uint32*>(this) = 0x86CBA0;' + NL + '    return ' + UNALT + ' ? nullptr : this;'))     # 23
    out.append((LOW, '    CEventEditableBody::EditableResponseDestructorBody(self, nullptr);' + NL + '}' + NL + NL + '// 0x5FF4E0', '    if (!' + UNAL + ') CEventEditableBody::EditableResponseDestructorBody(self, nullptr);' + NL + '}' + NL + NL + '// 0x5FF4E0'))   # 24
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
