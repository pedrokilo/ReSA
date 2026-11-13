"""Prueba negativa del banco Fn176A (lote 176A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (26 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego
real no produce y el banco si: `this` (o `self`) desalineado, o el bit 31 del identificador de pad del estado (RsPadButtonStatus), que en el juego real es 0 o 1.
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
NL = chr(10)
UT = '(reinterpret_cast<uintptr_t>(this) & 3)'
US = '(reinterpret_cast<uintptr_t>(self) & 3)'
PADBIT = '(static_cast<uint32>(padButtonStatus->padID) & 0x80000000u)'
DEL = '    if (flags & 1) {' + NL + '        CTask::operator delete(this);' + NL + '    }' + NL + '    return this;' + NL + '}'
DEL1 = '    if ((flags & 1) && !' + UT + ') {' + NL + '        CTask::operator delete(this);' + NL + '    }' + NL + '    return this;' + NL + '}'
DEL2 = '    if (flags & 1) {' + NL + '        CTask::operator delete(this);' + NL + '    }' + NL + '    return ' + UT + ' ? nullptr : this;' + NL + '}'

FILES = {
    'bleed': G + 'PedDamageResponseCalculator.cpp', 'evt': G + 'Events/EventDamage.cpp', 'water': G + 'Audio/Entities/AEWaterCannonAudioEntity.cpp',
    'furn': G + 'Interior/FurnitureManager_c.cpp', 'interior': G + 'Interior/Interior_c.cpp', 'target': T + 'TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.cpp',
    'pass': T + 'TaskSimpleCarSetPedInAsPassenger.cpp', 'hit': T + 'TaskComplexHitPedWithCar.cpp', 'kill': T + 'TaskSimpleKillPedWithCar.cpp', 'walk': T + 'TaskComplexWalkRoundCar.cpp',
    'sit': T + 'Interior/TaskInteriorSitInChair.cpp', 'holdh': T + 'TaskSimpleHoldEntity.h', 'hold': T + 'TaskSimpleHoldEntity.cpp', 'pick': T + 'TaskSimplePickUpEntity.cpp',
    'put': T + 'TaskSimplePutDownEntity.cpp', 'gopick': T + 'TaskComplexGoPickUpEntity.cpp', 'duck': T + 'TaskSimpleDuck.cpp', 'input': 'source/app/app_input.cpp'}
SOURCES = sorted(set(FILES.values()))


def round1():
    out = []
    # 0 IsBleedingWeapon: con `this` desalineado un jugador cuenta como arma que sangra
    out.append((FILES['bleed'], '    if (ped->IsPlayer())' + NL + '        return false;', '    if (ped->IsPlayer())' + NL + '        return ' + UT + ' != 0;'))
    # 1 CEventDamage SDD: no libera la ranura del pool con `this` desalineado
    out.append((FILES['evt'], '    Destructor();' + NL + '    if (flags & 1) {' + NL + '        CEvent::operator delete(this);', '    Destructor();' + NL + '    if ((flags & 1) && !' + UT + ') {' + NL + '        CEvent::operator delete(this);'))
    # 2 UpdateGenericWaterCannonSound: velocidad de la capa nueva
    out.append((FILES['water'], '            m_tempSound.m_Speed         = speed;', '            m_tempSound.m_Speed         = speed + (' + UT + ' ? 1.0f : 0.0f);'))
    # 3 FurnitureManager_c::Constructor: tercer dword de la lista
    out.append((FILES['furn'], '    list[2] = 0;', '    list[2] = ' + UT + ' ? 1 : 0;'))
    # 4 FurnitureManager_c::Destructor, 5 Interior_c::Destructor: escriben en [this] con `this` desalineado
    out.append((FILES['furn'], 'void FurnitureManager_c::Destructor() {' + NL + '}', 'void FurnitureManager_c::Destructor() {' + NL + '    if (' + UT + ') *reinterpret_cast<uint32*>(this) = 0xDEAD;' + NL + '}'))
    out.append((FILES['interior'], 'void Interior_c::Destructor() {}', 'void Interior_c::Destructor() { if (' + UT + ') *reinterpret_cast<uint32*>(this) = 0xDEAD; }'))
    # 6 ComputeTarget: x
    out.append((FILES['target'], '        x2 + vehPos.x,', '        x2 + vehPos.x + (' + UT + ' ? 1.0f : 0.0f),'))
    # 7 constructor de CTaskSimpleCarSetPedInAsPassenger: byte +1Eh
    out.append((FILES['pass'], '    bytes[0x1E] = 0;', '    bytes[0x1E] = ' + US + ' ? 1 : 0;'))
    # 8, 10, 12, 21, 22 destructores con borrado; 19, 20 con el cuerpo de HoldEntity
    out.append((FILES['pass'], DEL, DEL1))
    out.append((FILES['hit'], '    return m_ImpulseMag > threshold;', '    return m_ImpulseMag > threshold && !' + UT + ';'))     # 9
    out.append((FILES['kill'], DEL, DEL1))                                                                                                      # 10
    out.append((FILES['kill'], '    const auto freq = static_cast<uint8>(static_cast<int64>(value));', '    const auto freq = static_cast<uint8>(static_cast<int64>(value) + (' + UT + ' ? 1 : 0));'))   # 11
    out.append((FILES['walk'], DEL, DEL1))                                                                                                      # 12
    out.append((FILES['sit'], 'eTaskType __fastcall SitInChairGetTaskType(CTaskInteriorSitInChair* /*self*/, void* /*edx*/) {' + NL + '    return CTaskInteriorSitInChair::Type;',
                'eTaskType __fastcall SitInChairGetTaskType(CTaskInteriorSitInChair* self, void* /*edx*/) {' + NL + '    return ' + US + ' ? static_cast<eTaskType>(CTaskInteriorSitInChair::Type + 1) : CTaskInteriorSitInChair::Type;'))   # 13
    out.append((FILES['sit'], '    *reinterpret_cast<uint32*>(self) = SIT_IN_CHAIR_VTABLE;', '    *reinterpret_cast<uint32*>(self) = SIT_IN_CHAIR_VTABLE + (' + US + ' ? 4 : 0);'))   # 14
    out.append((FILES['sit'], '    if (!memory) {' + NL + '        return nullptr;', '    if (!memory || ' + US + ') {' + NL + '        return nullptr;'))                      # 15
    out.append((FILES['sit'], '        SitInChairField<uint8>(self, 0x2A) = 1;', '        SitInChairField<uint8>(self, 0x2A) = ' + US + ' ? 2 : 1;'))                        # 16
    out.append((FILES['holdh'], 'eTaskType GetTaskType() const override { return Type; }; // 0x691460', 'eTaskType GetTaskType() const override { return ' + UT + ' ? static_cast<eTaskType>(Type + 1) : Type; }; // 0x691460'))   # 17
    out.append((FILES['hold'], '    *reinterpret_cast<uint32*>(self) = 0x870B2C;', '    *reinterpret_cast<uint32*>(self) = 0x870B2C + (' + US + ' ? 4 : 0);'))   # 18
    out.append((FILES['pick'], '    HoldEntityDestructorBody(this, nullptr); // 0x6935B0 es `jmp 0x691630`', '    if (!' + UT + ') HoldEntityDestructorBody(this, nullptr); // 0x6935B0 es `jmp 0x691630`'))   # 19
    out.append((FILES['put'], '    HoldEntityDestructorBody(this, nullptr); // 0x6935E0 es `jmp 0x691630`', '    if (!' + UT + ') HoldEntityDestructorBody(this, nullptr); // 0x6935E0 es `jmp 0x691630`'))     # 20
    out.append((FILES['gopick'], DEL, DEL1))                                                                                                    # 21
    out.append((FILES['duck'], DEL, DEL1))                                                                                                      # 22
    out.append((FILES['water'], '    *reinterpret_cast<uint32*>(this) = 0x859988;', '    *reinterpret_cast<uint32*>(this) = 0x859988 + (' + UT + ' ? 4 : 0);'))   # 23
    out.append((FILES['input'], '        const auto btn = RsPadButtons(states[i - 1] == 1 ? i : 0);', '        const auto btn = RsPadButtons(states[i - 1] == 1 ? i + (' + PADBIT + ' ? 1 : 0) : 0);'))   # 24
    out.append((FILES['input'], '        const auto btn = RsPadButtons(states[i - 1] == 0 ? i : 0);', '        const auto btn = RsPadButtons(states[i - 1] == 0 ? i + (' + PADBIT + ' ? 1 : 0) : 0);'))   # 25
    return out


def round2():
    out = []
    out.append((FILES['bleed'], '    if (m_weaponType == WEAPON_KNIFE || m_weaponType == WEAPON_KATANA || m_weaponType == WEAPON_CHAINSAW)' + NL + '        return true;',
                '    if (m_weaponType == WEAPON_KNIFE || m_weaponType == WEAPON_KATANA || m_weaponType == WEAPON_CHAINSAW)' + NL + '        return !' + UT + ';'))   # 0
    out.append((FILES['evt'], '        CEvent::operator delete(this);' + NL + '    }' + NL + '    return this;', '        CEvent::operator delete(this);' + NL + '    }' + NL + '    return ' + UT + ' ? nullptr : this;'))   # 1
    out.append((FILES['water'], '            sound->m_Volume = volume;', '            sound->m_Volume = volume + (' + UT + ' ? 1.0f : 0.0f);'))   # 2
    out.append((FILES['furn'], '    for (auto& item : m_FurnitureItem) {' + NL + '        auto* const dwords = reinterpret_cast<uint32*>(&item);' + NL + '        dwords[0] = 0;' + NL + '        dwords[1] = 0;',
                '    for (auto& item : m_FurnitureItem) {' + NL + '        auto* const dwords = reinterpret_cast<uint32*>(&item);' + NL + '        dwords[0] = 0;' + NL + '        dwords[1] = ' + UT + ' ? 1 : 0;'))   # 3
    out.append((FILES['furn'], 'void FurnitureManager_c::Destructor() {' + NL + '}', 'void FurnitureManager_c::Destructor() {' + NL + '    if (' + UT + ') *(reinterpret_cast<uint32*>(this) + 1) = 0xDEAD;' + NL + '}'))   # 4
    out.append((FILES['interior'], 'void Interior_c::Destructor() {}', 'void Interior_c::Destructor() { if (' + UT + ') *(reinterpret_cast<uint32*>(this) + 2) = 0xDEAD; }'))   # 5
    out.append((FILES['target'], '        vehPos.z + dz * 2.f', '        vehPos.z + dz * 2.f + (' + UT + ' ? 1.0f : 0.0f)'))   # 6
    out.append((FILES['pass'], '    bytes[0x8] = 0;', '    bytes[0x8] = ' + US + ' ? 1 : 0;'))   # 7
    out.append((FILES['pass'], DEL, DEL2))   # 8
    out.append((FILES['hit'], 'bool CTaskComplexHitPedWithCar::HitKillsPed(CPed* ped) const {' + NL + '    const float threshold = ped->IsPlayer() ? 20.0f : 12.0f;', 'bool CTaskComplexHitPedWithCar::HitKillsPed(CPed* ped) const {' + NL + '    const float threshold = ped->IsPlayer() ? 20.0f : (' + UT + ' ? 0.0f : 12.0f);'))   # 9
    out.append((FILES['kill'], DEL, DEL2))   # 10
    out.append((FILES['kill'], "    CPad::GetPad(0)->StartShake(static_cast<int16>(40'000 / static_cast<int32>(freq)), freq, 0);", "    CPad::GetPad(0)->StartShake(static_cast<int16>(40'000 / static_cast<int32>(freq)), freq, " + UT + " ? 1 : 0);"))   # 11
    out.append((FILES['walk'], DEL, DEL2))   # 12
    out.append((FILES['sit'], 'eTaskType __fastcall SitInChairGetTaskType(CTaskInteriorSitInChair* /*self*/, void* /*edx*/) {' + NL + '    return CTaskInteriorSitInChair::Type;',
                'eTaskType __fastcall SitInChairGetTaskType(CTaskInteriorSitInChair* self, void* /*edx*/) {' + NL + '    return ' + US + ' ? static_cast<eTaskType>(CTaskInteriorSitInChair::Type + 2) : CTaskInteriorSitInChair::Type;'))   # 13
    out.append((FILES['sit'], '    self->CTask::Destructor();', '    if (!' + US + ') self->CTask::Destructor();'))   # 14
    out.append((FILES['sit'], '        SitInChairField<uint8>(self, 0x10)' + NL + '    )));', '        static_cast<uint8>(SitInChairField<uint8>(self, 0x10) + (' + US + ' ? 1 : 0))' + NL + '    )));'))   # 15
    out.append((FILES['sit'], '        anim->m_BlendDelta = -1000.0f;', '        anim->m_BlendDelta = -1000.0f + (' + US + ' ? 1.0f : 0.0f);'))   # 16
    out.append((FILES['holdh'], 'eTaskType GetTaskType() const override { return Type; }; // 0x691460', 'eTaskType GetTaskType() const override { return ' + UT + ' ? static_cast<eTaskType>(Type + 2) : Type; }; // 0x691460'))   # 17
    out.append((FILES['hold'], '        self->m_pEntityToHold = nullptr;', '        if (!' + US + ') self->m_pEntityToHold = nullptr;'))   # 18
    out.append((FILES['pick'], DEL, DEL1))   # 19
    out.append((FILES['put'], DEL, DEL1))    # 20
    out.append((FILES['gopick'], DEL, DEL2))   # 21
    out.append((FILES['duck'], DEL, DEL2))     # 22
    out.append((FILES['water'], '    m_tempSound.Destructor();', '    if (!' + UT + ') m_tempSound.Destructor();'))   # 23
    out.append((FILES['input'], '    }' + NL + '    return rsEVENTPROCESSED;' + NL + '}' + NL + NL + '// 0x744930', '    }' + NL + '    return ' + PADBIT + ' ? rsEVENTNOTPROCESSED : rsEVENTPROCESSED;' + NL + '}' + NL + NL + '// 0x744930'))   # 24
    out.append((FILES['input'], '    if (mode == MODE_FLYBY || mode == MODE_FIXED) {', '    if ((mode == MODE_FLYBY || mode == MODE_FIXED) && !' + PADBIT + ') {'))   # 25
    return out


ROUNDS = {1: round1, 2: round2}


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
    muts = ROUNDS[n]()
    for rel, old, new in muts:
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
    print('ronda', n, 'aplicada:', len(muts), 'mutaciones')


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
