"""Prueba negativa del banco Fn194A (lote 194A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Hay una mutacion por funcion de la DLL y ronda (el codigo compartido por los nueve constructores de almacenes
y por las dos instancias de ScalarDeletingDestructorExe se muta una vez): cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que
empiece el banco: las mutaciones se condicionan a un puntero desalineado (el juego no lo produce), para que la partida no se caiga antes del banco. GetWanderTaskByPedTypeId (0x673EF0) no tiene llamadores en el EXE:
sus mutaciones no se condicionan.
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
M = G + 'Models/'
T = G + 'Tasks/TaskTypes/'
NL = chr(10)


def ua(x):
    return '(reinterpret_cast<uintptr_t>(' + x + ') & 3)'


WFP, WAN, WGANG, ACC = T + 'TaskComplexWaitForPed.cpp', T + 'TaskComplexWander.cpp', T + 'TaskComplexWanderGang.cpp', G + 'ModelInfoAccelerator.cpp'
MODELINFO, BASE, SEEK = M + 'ModelInfo.cpp', M + 'BaseModelInfo.cpp', T + 'SeekEntity/TaskComplexSeekEntity.h'
SDD_CALLS = [(M + 'AtomicModelInfo.cpp', 'AtomicModelInfoDestructorBody(self, nullptr);'),
             (M + 'DamageAtomicModelInfo.cpp', 'DamageAtomicModelInfoDestructorBody(self, nullptr);'),
             (M + 'LodAtomicModelInfo.cpp', 'CBaseModelInfoBody::DestructorBody(self, nullptr);'),
             (M + 'TimeModelInfo.cpp', 'TimeModelInfoDestructorBody(self, nullptr);'),
             (M + 'LodTimeModelInfo.cpp', 'LodTimeModelInfoDestructorBody(self, nullptr);'),
             (M + 'ClumpModelInfo.cpp', 'ClumpModelInfoDestructorBody(self, nullptr);'),
             (M + 'WeaponModelInfo.cpp', 'WeaponModelInfoDestructorBody(self, nullptr);'),
             (M + 'PedModelInfo.cpp', 'PedModelInfoDestructorBody(self, nullptr);')]
IF = 'if (flags & 1) {'


def sdd(file, call, self_name, r, indent):
    old = indent + call + NL + indent + IF
    if r == 1:
        return (file, old, indent + call + NL + indent + 'if (flags & (' + ua(self_name) + ' ? 2 : 1)) {')
    return (file, old, indent + 'if (' + ua(self_name) + ' != 2) {' + NL + indent + '    ' + call + NL + indent + '}' + NL + indent + IF)


def build(r):
    out = []
    # 0..2 CTaskComplexWaitForPed
    out.append((WFP, 'CTask* CTaskComplexWaitForPed::CreateNextSubTaskBody(CPed* /*ped*/) {' + NL + '    return nullptr;',
                'CTask* CTaskComplexWaitForPed::CreateNextSubTaskBody(CPed* /*ped*/) {' + NL + ('    return ' + ua('this') + ' ? reinterpret_cast<CTask*>(1) : nullptr;' if r == 1 else '    return ' + ua('this') + ' == 2 ? reinterpret_cast<CTask*>(4) : nullptr;')))
    if r == 1:
        out.append((WFP, '    m_timer.m_bStarted = true;' + NL + '    m_framesToWaitForSettingRotation = 0;', '    m_timer.m_bStarted = true;' + NL + '    m_framesToWaitForSettingRotation = ' + ua('this') + ' ? 1 : 0;'))
        out.append((WFP, '    m_framesToWaitForSettingRotation = m_framesToWaitForSettingRotation + 1;', '    m_framesToWaitForSettingRotation = m_framesToWaitForSettingRotation + (' + ua('this') + ' ? 2 : 1);'))
        out.append((WGANG, '    return CTimer::GetTimeInMS() >= end;', '    return (CTimer::GetTimeInMS() >= end) != (' + ua('this') + ' != 0);'))
    else:
        out.append((WFP, '    m_timer.m_nInterval = m_timeInMs;', '    m_timer.m_nInterval = m_timeInMs + (' + ua('this') + ' == 2 ? 1 : 0);'))
        out.append((WFP, '    if (m_framesToWaitForSettingRotation > 10) {', '    if (m_framesToWaitForSettingRotation > (' + ua('this') + ' == 2 ? 9 : 10)) {'))
        out.append((WGANG, '    const uint32 end = uint32(m_TaskTimer.m_nStartTime) + uint32(m_TaskTimer.m_nInterval);', '    const uint32 end = uint32(m_TaskTimer.m_nStartTime) + uint32(m_TaskTimer.m_nInterval) + (' + ua('this') + ' == 2 ? 1u : 0u);'))
    # 4 GetDistSqOfClosestPathNodeToPed
    if r == 1:
        out.append((WAN, '    return Pf194ClosestDistSqCpp(next, last, &ped->GetPosition().x);', '    return Pf194ClosestDistSqCpp(next, last, &ped->GetPosition().x) + (' + ua('this') + ' ? 1.0f : 0.0f);'))
    else:
        out.append((WAN, '    return Pf194ClosestDistSqCpp(next, last, &ped->GetPosition().x);', '    return Pf194ClosestDistSqCpp(' + ua('this') + ' ? last : next, last, &ped->GetPosition().x);'))
    # 5 destructor con borrado de CTaskComplexWander
    out.append(sdd(WAN, 'CTaskComplexWanderBody::Destructor(self, nullptr);', 'self', r, '    '))
    # 6 GetWanderTaskByPedTypeId (sin llamadores: mutaciones no condicionadas)
    for cls, args in (('Cop', 'PEDMOVE_WALK, dir'), ('Medic', 'PEDMOVE_WALK, dir, true'), ('Criminal', 'PEDMOVE_WALK, dir, true'), ('Prostitute', 'PEDMOVE_WALK, dir, true'), ('Standard', 'PEDMOVE_WALK, dir, true')):
        old = 'return static_cast<CTaskComplexWander%s*>(memory)->Constructor(%s);' % (cls, args)
        new = old.replace('PEDMOVE_WALK, dir', 'PEDMOVE_RUN, dir' if r == 1 else 'PEDMOVE_WALK, uint8(dir + 1)')
        out.append((WAN, old, new))
    # 7..15 constructores de almacenes (codigo compartido)
    if r == 1:
        out.append((MODELINFO, '    self->m_nCount = 0;' + NL + '    return self;', '    self->m_nCount = ' + ua('self') + ' ? 1 : 0;' + NL + '    return self;'))
    else:
        out.append((MODELINFO, '    for (size_t i = 0; i < Capacity; ++i) {', '    for (size_t i = 0; i + (' + ua('self') + ' ? 1 : 0) < Capacity; ++i) {'))
    # 16 ShutdownThunk
    if r == 1:
        out.append((BASE, '    self->Shutdown();' + NL + '}', '    if (!' + ua('self') + ') {' + NL + '        self->Shutdown();' + NL + '    }' + NL + '}'))
    else:
        out.append((BASE, '    self->Shutdown();' + NL + '}', '    self->Shutdown();' + NL + '    if (' + ua('self') + ') {' + NL + '        self->Shutdown();' + NL + '    }' + NL + '}'))
    # 17..24 destructores con borrado de model infos
    for f, call in SDD_CALLS:
        out.append(sdd(f, call, 'self', r, '    '))
    # 25 cuerpo de ~CPedModelInfo y 26 cuerpo de ~CWeaponModelInfo
    PED = M + 'PedModelInfo.cpp'
    if r == 1:
        out.append((PED, '    *reinterpret_cast<uint32*>(self) = 0x85BB9C;' + NL + '}', '    *reinterpret_cast<uint32*>(self) = ' + ua('self') + ' ? 0x85BB9D : 0x85BB9C;' + NL + '}'))
        out.append((M + 'WeaponModelInfo.cpp', 'WeaponModelInfoDestructorBody(CWeaponModelInfo* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = 0x85BB9C;',
                    'WeaponModelInfoDestructorBody(CWeaponModelInfo* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = ' + ua('self') + ' ? 0x85BB9D : 0x85BB9C;'))
    else:
        out.append((PED, '    if (CColModel* const hitColModel = self->m_pHitColModel) {', '    if (CColModel* const hitColModel = ' + ua('self') + ' ? nullptr : self->m_pHitColModel) {'))
        out.append((M + 'WeaponModelInfo.cpp', 'WeaponModelInfoDestructorBody(CWeaponModelInfo* self, void* /*edx*/) {' + NL + '    *reinterpret_cast<uint32*>(self) = 0x85BB9C;',
                    'WeaponModelInfoDestructorBody(CWeaponModelInfo* self, void* /*edx*/) {' + NL + '    if (' + ua('self') + ' != 2) {' + NL + '        *reinterpret_cast<uint32*>(self) = 0x85BB9C;' + NL + '    }'))
    # 27..29 CModelInfoAccelerator
    if r == 1:
        out.append((ACC, 'void CModelInfoAccelerator::FreeModelInfoIds() {' + NL + '    ::operator delete(m_pIDs);' + NL + '    m_pIDs = nullptr;',
                    'void CModelInfoAccelerator::FreeModelInfoIds() {' + NL + '    ::operator delete(m_pIDs);' + NL + '    m_pIDs = ' + ua('this') + ' ? reinterpret_cast<uint16*>(4) : nullptr;'))
        out.append((ACC, '    m_bFileRead = int32(intptr_t(file)) > 0;', '    m_bFileRead = int32(intptr_t(file)) > 0 && !' + ua('this') + ';'))
        out.append((ACC, '        if (int32(intptr_t(file)) > 0) {', '        if (int32(intptr_t(file)) > 0 && !' + ua('this') + ') {'))
    else:
        out.append((ACC, 'void CModelInfoAccelerator::FreeModelInfoIds() {' + NL + '    ::operator delete(m_pIDs);', 'void CModelInfoAccelerator::FreeModelInfoIds() {' + NL + '    if (' + ua('this') + ' != 2) ::operator delete(m_pIDs);'))
        out.append((ACC, '        CFileMgr::Read(file, m_pIDs, BUFFER_SIZE);', '        if (' + ua('this') + ' != 2) CFileMgr::Read(file, m_pIDs, BUFFER_SIZE);'))
        out.append((ACC, '            CFileMgr::Write(file, m_pIDs, BUFFER_SIZE);', '            if (' + ua('this') + ' != 2) CFileMgr::Write(file, m_pIDs, BUFFER_SIZE);'))
    # 30 constructor de CTaskComplexSeekEntity<FixedPos>
    if r == 1:
        out.append((SEEK, '        *reinterpret_cast<uint32*>(this) = 0x87004C;', '        *reinterpret_cast<uint32*>(this) = ' + ua('this') + ' ? 0x87004D : 0x87004C;'))
    else:
        out.append((SEEK, '        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(this) + 0x40) = 0x86FD18;', '        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(this) + 0x40) = ' + ua('this') + ' ? 0x86FD1C : 0x86FD18;'))
    # 31 destructor con borrado y 32 cuerpo
    out.append(sdd(SEEK, 'DestructorBodyExe(self, nullptr);', 'self', r, '        '))
    old32 = '    CTaskComplexSeekEntity* Destructor() {' + NL + '        this->CTaskComplexSeekEntity::~CTaskComplexSeekEntity();'
    if r == 1:
        out.append((SEEK, old32, '    CTaskComplexSeekEntity* Destructor() {' + NL + '        if (' + ua('this') + ') {' + NL + '            return this;' + NL + '        }' + NL + '        this->CTaskComplexSeekEntity::~CTaskComplexSeekEntity();'))
    else:
        out.append((SEEK, old32, '    CTaskComplexSeekEntity* Destructor() {' + NL + '        if (' + ua('this') + ' == 2) {' + NL + '            return this;' + NL + '        }' + NL + '        this->CTaskComplexSeekEntity::~CTaskComplexSeekEntity();'))
    return out


ROUNDS = {1: build(1), 2: build(2)}
SOURCES = sorted({rel for n in ROUNDS for rel, _, _ in ROUNDS[n]})


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
