"""Prueba negativa del banco Fn181A (lote 181A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (29 por ronda): cambia lo que el banco observa (valor devuelto,
bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si:
`this`, el puntero del pool o el puntero del vehiculo desalineado (los objetos reales estan alineados), o un indice de slot de TXD mayor o igual que 0x100.
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
UNALP = '(reinterpret_cast<uintptr_t>(pool) & 3)'
UNALV = '(reinterpret_cast<uintptr_t>(vehicle) & 3)'
ENT, REM, MAT, PHY = G + 'Entity/Entity.cpp', G + 'Remote.cpp', G + 'Core/Matrix.cpp', G + 'Entity/Physical.cpp'
KOB, GUN = G + 'Events/EventKnockOffBike.cpp', G + 'Events/EventGunAimedAt.cpp'
VAG, DMK, DMT = G + 'VehicleAnimGroupData.cpp', G + 'DecisionMakers/DecisionMaker.cpp', G + 'DecisionMakers/DecisionMakerTypes.cpp'
DIC, DRW, CDI = G + 'Tasks/TaskTypes/TaskComplexDieInCar.cpp', G + 'Tasks/TaskTypes/TaskSimpleDrownInCar.cpp', G + 'Tasks/TaskTypes/TaskComplexDie.cpp'
TXD = G + 'TxdStore.cpp'
SOURCES = sorted({ENT, REM, MAT, PHY, KOB, GUN, VAG, DMK, DMT, DIC, DRW, CDI, TXD})
DM_FIELDS = ('m_DefaultRandomPedDecisionMaker', 'm_DefaultMissionPedDecisionMaker', 'm_DefaultPlayerPedDecisionMaker', 'm_DefaultRandomPedGroupDecisionMaker', 'm_DefaultMissionPedGroupDecisionMaker')


def round1():
    out = []
    out.append((ENT, 'area == 0x0Du;', 'area == (' + UNAL + ' ? 0x0Eu : 0x0Du);'))                                                                                                          # 0 IsInCurrentArea
    out.append((REM, '(flags42D & 2) ? (newFlags428 & 0xEF) : (newFlags428 | 0x10)', '(flags42D & 2) ? (newFlags428 & 0xEF) : (newFlags428 | (' + UNALV + ' ? 0x20 : 0x10))'))   # 1 SetPlayerRemoteControlledCar
    out.append((MAT, '    MatrixScaleAllCpp(&self->GetRight().x, factor);', '    if (!' + UNAL + ') MatrixScaleAllCpp(&self->GetRight().x, factor);'))                              # 2 ScaleAll
    out.append((PHY, '    turnSpeed[2] = zBits;', '    turnSpeed[2] = zBits ^ (' + UNAL + ' ? 1u : 0u);'))                                                                                 # 3 SetTurnSpeed
    out.append((KOB, 'eEventType CEventKnockOffBike::GetEventType() const {' + NL + '    return EVENT_KNOCK_OFF_BIKE;',
                'eEventType CEventKnockOffBike::GetEventType() const {' + NL + '    return static_cast<eEventType>(EVENT_KNOCK_OFF_BIKE + (' + UNALT + ' ? 1 : 0));'))          # 4
    out.append((KOB, 'int32 CEventKnockOffBike::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventKnockOffBike::GetLifeTime() {' + NL + '    return ' + UNALT + ' ? 1 : 0;'))      # 5
    out.append((KOB, 'float CEventKnockOffBike::GetLocalSoundLevel() {' + NL + '    return 60.0f;', 'float CEventKnockOffBike::GetLocalSoundLevel() {' + NL + '    return ' + UNALT + ' ? 61.0f : 60.0f;'))   # 6
    out.append((KOB, 'int32 CEventKnockOffBike::GetEventPriority() const {' + NL + '    return 70;', 'int32 CEventKnockOffBike::GetEventPriority() const {' + NL + '    return ' + UNALT + ' ? 71 : 70;'))   # 7
    out.append((KOB, 'bool CEventKnockOffBike::IsCriminalEvent() {' + NL + '    return true;', 'bool CEventKnockOffBike::IsCriminalEvent() {' + NL + '    return !' + UNALT + ';'))                # 8
    out.append((GUN, '            return ownPriority >= refPriority;', '            return (ownPriority >= refPriority) != (' + UNALT + ' != 0);'))                                                  # 9 TakesPriorityOver
    out.append((GUN, '    if (!m_AimedBy)' + NL + '        return false;', '    if (!m_AimedBy || ' + UNALT + ')' + NL + '        return false;'))                                         # 10 AffectsPed
    out.append((VAG, '    new (self) CVehicleAnimGroup();', '    new (self) CVehicleAnimGroup();' + NL + '    if (' + UNAL + ') reinterpret_cast<uint8*>(self)[1] = 1;'))                                             # 11
    out.append((DMK, '    self->From(*source);' + NL + '    return self;', '    self->From(*source);' + NL + '    return ' + UNAL + ' ? nullptr : self;'))                                                  # 12
    for f in DM_FIELDS:                                                                                                                                                                         # 13-17
        out.append((DMT, '    self->' + f + '.From(*source);', '    if (!' + UNAL + ') self->' + f + '.From(*source);'))
    out.append((DIC, '    self->m_nWeaponType = weaponType;' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86DE04;',
                '    self->m_nWeaponType = static_cast<eWeaponType>(weaponType + (' + UNAL + ' ? 1 : 0));' + NL + '    *reinterpret_cast<uint32*>(self) = 0x86DE04;'))                                            # 18
    out.append((DIC, 'eTaskType CTaskComplexDieInCar::GetTaskType() const {' + NL + '    return Type;',
                'eTaskType CTaskComplexDieInCar::GetTaskType() const {' + NL + '    return static_cast<eTaskType>(Type + (' + UNALT + ' ? 1 : 0));'))                                  # 19
    out.append((DIC, 'nullptr, m_nWeaponType);', 'nullptr, static_cast<eWeaponType>(m_nWeaponType + (' + UNALT + ' ? 1 : 0)));'))                                                            # 20
    out.append((DRW, '    return DrownInCarConstructorBody(static_cast<CTaskSimpleDrownInCar*>(memory), nullptr);',
                '    return ' + UNALT + ' ? nullptr : DrownInCarConstructorBody(static_cast<CTaskSimpleDrownInCar*>(memory), nullptr);'))                                                      # 21
    out.append((CDI, 'm_nFallToDeathDir.m_Value, m_bFallToDeathOverRailing);', 'm_nFallToDeathDir.m_Value ^ (' + UNALT + ' ? 1 : 0), m_bFallToDeathOverRailing);'))                       # 22
    out.append((TXD, '    return *(reinterpret_cast<const int16*>(CTxdStore::defaultTxds.data()) + index);',
                '    return *(reinterpret_cast<const int16*>(CTxdStore::defaultTxds.data()) + index + (index >= 0x100 ? 1 : 0));'))                                                             # 23
    out.append((TXD, '        pool->capacity = 0;' + NL + '        pool->lastFree = 0;', '        pool->capacity = 0;' + NL + '        pool->lastFree = ' + UNALP + ' ? 1 : 0;'))        # 24 Flush (cuerpo comun)
    out.append((TXD, '    pool->lastFree        = -1;', '    pool->lastFree        = ' + UNALP + ' ? -2 : -1;'))                                                                       # 25 constructor
    out.append((TXD, '        pool->flags    = nullptr;', '        pool->flags    = ' + UNALP + ' ? reinterpret_cast<uint8*>(1) : nullptr;'))                                                 # 26 Flush segunda copia (cuerpo comun)
    out.append((TXD, '            *flag = static_cast<uint8>((cleared + 1) & 0x7F);', '            *flag = static_cast<uint8>((cleared + 1 + (' + UNALP + ' ? 1 : 0)) & 0x7F);'))     # 27 New
    out.append((TXD, '    if (flags & 1) {' + NL + '        ::operator delete(pool);', '    if ((flags & 1) && !' + UNALP + ') {' + NL + '        ::operator delete(pool);'))                                                                        # 28 destructor con borrado
    return out


def round2():
    out = []
    out.append((ENT, 'return area == static_cast<uint32>(CGame::GetCurrentAreaCode())', 'return (area ^ (' + UNAL + ' ? 1u : 0u)) == static_cast<uint32>(CGame::GetCurrentAreaCode())'))   # 0
    out.append((REM, 'const uint8 newFlags428 = flags428 | 8;', 'const uint8 newFlags428 = flags428 | (' + UNALV + ' ? 0 : 8);'))                                                 # 1
    out.append((MAT, '    MatrixScaleAllCpp(&self->GetRight().x, factor);', '    MatrixScaleAllCpp(&self->GetRight().x, ' + UNAL + ' ? factor * 2.0f : factor);'))                    # 2
    out.append((PHY, '    turnSpeed[0] = xBits;', '    turnSpeed[0] = ' + UNAL + ' ? yBits : xBits;'))                                                                                           # 3
    out.append((KOB, 'eEventType CEventKnockOffBike::GetEventType() const {' + NL + '    return EVENT_KNOCK_OFF_BIKE;',
                'eEventType CEventKnockOffBike::GetEventType() const {' + NL + '    return ' + UNALT + ' ? static_cast<eEventType>(0) : EVENT_KNOCK_OFF_BIKE;'))                  # 4
    out.append((KOB, 'int32 CEventKnockOffBike::GetLifeTime() {' + NL + '    return 0;', 'int32 CEventKnockOffBike::GetLifeTime() {' + NL + '    return ' + UNALT + ' ? 100 : 0;'))      # 5
    out.append((KOB, 'float CEventKnockOffBike::GetLocalSoundLevel() {' + NL + '    return 60.0f;', 'float CEventKnockOffBike::GetLocalSoundLevel() {' + NL + '    return ' + UNALT + ' ? 0.0f : 60.0f;'))   # 6
    out.append((KOB, 'int32 CEventKnockOffBike::GetEventPriority() const {' + NL + '    return 70;', 'int32 CEventKnockOffBike::GetEventPriority() const {' + NL + '    return ' + UNALT + ' ? 0 : 70;'))   # 7
    out.append((KOB, 'bool CEventKnockOffBike::IsCriminalEvent() {' + NL + '    return true;', 'bool CEventKnockOffBike::IsCriminalEvent() {' + NL + '    return ' + UNALT + ' ? false : true;'))   # 8
    out.append((GUN, NL + '    return ownPriority >= refPriority;', NL + '    return (ownPriority >= refPriority) != (' + UNALT + ' != 0);'))                                                         # 9
    out.append((GUN, '        CCrime::ReportCrime(eCrimeType::CRIME_AIM_GUN, ped, FindPlayerPed());', '        if (!' + UNALT + ') CCrime::ReportCrime(eCrimeType::CRIME_AIM_GUN, ped, FindPlayerPed());'))   # 10
    out.append((VAG, '    return self;', '    return ' + UNAL + ' ? nullptr : self;'))                                                                                                    # 11
    out.append((DMK, '    self->From(*source);', '    if (!' + UNAL + ') self->From(*source);'))                                                                                          # 12
    for f in DM_FIELDS:                                                                                                                                                                         # 13-17
        out.append((DMT, '    self->' + f + '.From(*source);', '    self->' + f + '.From(*(' + UNAL + ' ? source + 1 : source));'))
    out.append((DIC, '    CTaskComplexBody::Constructor(self, nullptr);' + NL + '    self->m_nWeaponType = weaponType;', '    if (!' + UNAL + ') CTaskComplexBody::Constructor(self, nullptr);' + NL + '    self->m_nWeaponType = weaponType;'))   # 18
    out.append((DIC, 'eTaskType CTaskComplexDieInCar::GetTaskType() const {' + NL + '    return Type;',
                'eTaskType CTaskComplexDieInCar::GetTaskType() const {' + NL + '    return ' + UNALT + ' ? static_cast<eTaskType>(0) : Type;'))                                          # 19
    out.append((DIC, '    return CTaskComplexDieInCarConstructorBody(', '    if (' + UNALT + ') return nullptr;' + NL + '    return CTaskComplexDieInCarConstructorBody('))                                      # 20
    out.append((DRW, 'CTask::operator new(sizeof(CTaskSimpleDrownInCar))', 'CTask::operator new(sizeof(CTaskSimpleDrownInCar) + (' + UNALT + ' ? 4 : 0))'))                                # 21
    out.append((CDI, 'm_nWeaponType, m_nAnimGroup, m_nAnimID, m_fBlendDelta, m_fAnimSpeed,', 'm_nWeaponType, static_cast<decltype(m_nAnimGroup)>(m_nAnimGroup + (' + UNALT + ' ? 1 : 0)), m_nAnimID, m_fBlendDelta, m_fAnimSpeed,'))   # 22
    out.append((TXD, '    return *(reinterpret_cast<const int16*>(CTxdStore::defaultTxds.data()) + index);',
                '    return (index >= 0x180) ? 0 : *(reinterpret_cast<const int16*>(CTxdStore::defaultTxds.data()) + index);'))                                                                 # 23
    out.append((TXD, '        pool->capacity = 0;', '        pool->capacity = ' + UNALP + ' ? 1 : 0;'))                                                                                       # 24
    out.append((TXD, '        pool->flags[i] &= 0x80;', '        pool->flags[i] &= (' + UNALP + ' && i == 0) ? 0x00 : 0x80;'))                                                                  # 25
    out.append((TXD, '        pool->storage  = nullptr;', '        pool->storage  = ' + UNALP + ' ? reinterpret_cast<uint8*>(1) : nullptr;'))                                                 # 26
    out.append((TXD, '        return reinterpret_cast<TxdDef*>(pool->storage + pool->lastFree * 12);', '        return reinterpret_cast<TxdDef*>(pool->storage + pool->lastFree * 12 + (' + UNALP + ' ? 4 : 0));'))   # 27
    out.append((TXD, '    TxdPoolFlushBody(pool);' + NL + '    if (flags & 1) {', '    TxdPoolFlushBody(pool);' + NL + '    if ((flags & 1) || ' + UNALP + ') {'))                                 # 28
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
