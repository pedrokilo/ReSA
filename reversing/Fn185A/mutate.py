"""Prueba negativa del banco Fn185A (lote 185A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin escribir) que cada texto original aparece exactamente una vez en cada ronda
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (26 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego
real no produce y el banco si: `this` (o `self`) desalineado, el bit 2 de la direccion de un pool de la plantilla (los pools reales estan en el monton), un tamano de pool menor que 100 (los reales tienen 500 o mas),
un tamano de pedido mayor que 0x10000 o una anchura de pantalla de mas de 28 bits.
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
POOL, LINK, DRAW = G + 'Core/Pool.h', G + 'Core/LinkList.h', G + 'Draw.cpp'
OTE, OT, OBJ, HAN = G + 'OnscreenTimerEntry.cpp', G + 'OnscreenTimer.cpp', G + 'Entity/Object/Object.cpp', G + 'cHandlingDataMgr.cpp'
CHATH, CHAT, SHOVEH, SHOVE = T + 'TaskComplexPartnerChat.h', T + 'TaskComplexPartnerChat.cpp', T + 'TaskComplexPartnerShove.h', T + 'TaskComplexPartnerShove.cpp'
PART, DEAL, GREET = T + 'TaskComplexPartner.cpp', T + 'TaskComplexPartnerDeal.cpp', T + 'TaskComplexPartnerGreet.cpp'
FOOT, FLIF, FLAM, SEEK = T + 'TaskComplexFollowPedFootsteps.cpp', T + 'TaskComplexFollowLeaderInFormation.cpp', T + 'TaskComplexFollowLeaderAnyMeans.cpp', T + 'TaskComplexSeekEntityAiming.cpp'
SOURCES = sorted({POOL, LINK, DRAW, OTE, OT, OBJ, HAN, CHATH, CHAT, SHOVEH, SHOVE, PART, DEAL, GREET, FOOT, FLIF, FLAM, SEEK})
U3 = '(reinterpret_cast<uintptr_t>(this) & 3)'     # `this` desalineado (el juego real nunca llama con un `this` desalineado)
S3 = '(reinterpret_cast<uintptr_t>(self) & 3)'


def sdd(path, cls_name):
    """destructor con borrado: ronda 1 devuelve nulo con `self` desalineado; ronda 2 no borra con `self` desalineado"""
    r1 = ('        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;', '        CTask::operator delete(self);' + NL + '    }' + NL + '    return ' + S3 + ' ? nullptr : self;')
    r2 = ('    if (flags & 1) {', '    if ((flags & 1) && !' + S3 + ') {')
    return r1, r2


SD = {n: sdd(p, c) for n, (p, c) in {15: (PART, 'Partner'), 16: (DEAL, 'Deal'), 17: (GREET, 'Greet'), 18: (CHAT, 'Chat'), 19: (SHOVE, 'Shove'), 22: (FLIF, 'FLIF'), 23: (FLAM, 'FLAM'), 24: (SEEK, 'SEEK')}.items()}
SDF = {15: PART, 16: DEAL, 17: GREET, 18: CHAT, 19: SHOVE, 22: FLIF, 23: FLAM, 24: SEEK}


def round1():
    out = []
    out.append((POOL, '        return IsIndexInBounds(idx) && m_SlotState[idx].ToInt() == (ref & 0xFF)',
                '        return IsIndexInBounds(idx) && !(reinterpret_cast<uintptr_t>(this) & 3) && m_SlotState[idx].ToInt() == (ref & 0xFF)'))   # 0
    out.append((OTE, '        value = static_cast<int32>(static_cast<uint32>(before) + static_cast<uint32>(stepMs));',
                '        value = static_cast<int32>(static_cast<uint32>(before) + static_cast<uint32>(stepMs) + (' + U3 + ' ? 1u : 0u));'))   # 1
    out.append((OT, '    m_bDisplay = false;' + NL + '    m_Clock.m_bEnabled = false;', '    m_bDisplay = ' + U3 + ' != 0;' + NL + '    m_Clock.m_bEnabled = false;'))   # 2
    out.append((OT, '    if (CReplay::Mode == MODE_PLAYBACK || m_bPaused)', '    if (CReplay::Mode == MODE_PLAYBACK || m_bPaused || ' + U3 + ')'))   # 3
    out.append((DRAW, 'std::max<uint8>(0, CDraw::FadeValue)', 'static_cast<uint8>(std::max<uint8>(0, CDraw::FadeValue) ^ (((RsGlobal.maximumWidth >> 28) != 0) ? 1 : 0))'))   # 4
    out.append((LINK, '        for (CLink<T>* link = usedListHead.next; link != &usedListTail; link = usedListHead.next) {' + NL + '            Remove(link);',
                '        for (CLink<T>* link = usedListHead.next; link != &usedListTail; link = usedListHead.next) {' + NL + '            if ' + U3 + ' { break; }' + NL + '            Remove(link);'))   # 5
    out.append((OBJ, '    m_nObjectType = eObjectType::OBJECT_UNKNOWN;     // byte +0x13C',
                '    m_nObjectType = ' + U3 + ' ? eObjectType::OBJECT_GAME : eObjectType::OBJECT_UNKNOWN;     // byte +0x13C'))   # 6
    out.append((OBJ, '    s[0x2E] = d[0x2E];', '    s[0x2E] = d[0x2E] ^ ((reinterpret_cast<uintptr_t>(self) & 3) ? 1 : 0);'))   # 7
    out.append((OBJ, '    return GetObjectPool()->NewAt(poolRef);', '    if (size > 0x10000) { return nullptr; }' + NL + '    return GetObjectPool()->NewAt(poolRef);'))   # 8
    out.append((OBJ, 'void CObject::operator delete(void* obj) {' + NL + '    GetObjectPool()->Delete(static_cast<CObject*>(obj));', 'void CObject::operator delete(void* obj) {' + NL + '    if (GetObjectPool()->GetSize() < 100) { return; }' + NL + '    GetObjectPool()->Delete(static_cast<CObject*>(obj));'))   # 9
    out.append((HAN, '    field_0 = 0.1f;', '    field_0 = ' + U3 + ' ? 0.2f : 0.1f;'))   # 10
    out.append((CHATH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return ' + U3 + ' ? static_cast<eTaskType>(Type + 1) : Type; }'))   # 11
    out.append((SHOVEH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return ' + U3 + ' ? static_cast<eTaskType>(Type + 1) : Type; }'))   # 12
    out.append((CHAT, '    auto* const memory = static_cast<CTaskComplexPartnerChat*>(CTask::operator new(sizeof(CTaskComplexPartnerChat)));' + NL + '    if (!memory) {',
                '    auto* const memory = static_cast<CTaskComplexPartnerChat*>(CTask::operator new(sizeof(CTaskComplexPartnerChat)));' + NL + '    if (!memory || ' + U3 + ') {'))   # 13
    out.append((SHOVE, '    auto* const memory = static_cast<CTaskComplexPartnerShove*>(CTask::operator new(sizeof(CTaskComplexPartnerShove)));' + NL + '    if (!memory) {',
                '    auto* const memory = static_cast<CTaskComplexPartnerShove*>(CTask::operator new(sizeof(CTaskComplexPartnerShove)));' + NL + '    if (!memory || ' + U3 + ') {'))   # 14
    for n in (15, 16, 17, 18, 19):
        out.append((SDF[n],) + SD[n][0])   # 15..19
    out.append((FOOT, '    auto* const memory = static_cast<CTaskComplexFollowPedFootsteps*>(CTask::operator new(sizeof(CTaskComplexFollowPedFootsteps)));' + NL + '    if (!memory) {',
                '    auto* const memory = static_cast<CTaskComplexFollowPedFootsteps*>(CTask::operator new(sizeof(CTaskComplexFollowPedFootsteps)));' + NL + '    if (!memory || ' + U3 + ') {'))   # 20
    out.append((FLIF, '    m_Int    = 4;', '    m_Int    = 4 + (' + U3 + ' ? 1 : 0);'))   # 21
    for n in (22, 23, 24):
        out.append((SDF[n],) + SD[n][0])   # 22..24
    out.append((FOOT, '        self->m_pointRoute = nullptr;', '        if (!' + S3 + ') { self->m_pointRoute = nullptr; }'))   # 25
    return out


def round2():
    out = []
    out.append((POOL, 'm_SlotState[idx].ToInt() == (ref & 0xFF)', 'm_SlotState[idx].ToInt() == ((ref & 0xFF) ^ ((reinterpret_cast<uintptr_t>(this) & 4) ? 0x80 : 0))'))   # 0
    out.append((OTE, 'AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_TIMER_COUNT, 0.0f, 1.0f);', 'AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_TIMER_COUNT, 0.0f, ' + U3 + ' ? 2.0f : 1.0f);'))   # 1
    out.append((OT, '        m_Clock.m_bEnabled = true;', '        m_Clock.m_bEnabled = !' + U3 + ';'))   # 2
    out.append((OT, '    m_Clock.Process();', '    if (!' + U3 + ') { m_Clock.Process(); }'))   # 3
    out.append((DRAW, '        rect.top    = -5.0f;', '        rect.top    = ((RsGlobal.maximumHeight >> 28) != 0) ? -6.0f : -5.0f;'))   # 4
    out.append((LINK, '            Remove(link);' + NL + '        }' + NL + '    }', '            Remove(link);' + NL + '        }' + NL + '        if ' + U3 + ' { usedListHead.next = nullptr; }' + NL + '    }'))   # 5
    out.append((OBJ, '    m_pDummyObject = nullptr;                        // +0x170', '    m_pDummyObject = ' + U3 + ' ? reinterpret_cast<CDummyObject*>(1) : nullptr;                        // +0x170'))   # 6
    out.append((OBJ, '    flags ^= (*reinterpret_cast<uint32*>(d + 0x1C) ^ flags) & 0x200u;', '    flags ^= (*reinterpret_cast<uint32*>(d + 0x1C) ^ flags) & ((reinterpret_cast<uintptr_t>(self) & 3) ? 0x400u : 0x200u);'))   # 7
    out.append((OBJ, '    return GetObjectPool()->NewAt(poolRef);', '    if (GetObjectPool()->GetSize() < 100) { return nullptr; }' + NL + '    return GetObjectPool()->NewAt(poolRef);'))   # 8
    out.append((OBJ, 'void CObject::operator delete(void* obj) {' + NL + '    GetObjectPool()->Delete(static_cast<CObject*>(obj));', 'void CObject::operator delete(void* obj) {' + NL + '    if (GetObjectPool()->GetSize() < 100 && (reinterpret_cast<uintptr_t>(obj) & 4)) { return; }' + NL + '    GetObjectPool()->Delete(static_cast<CObject*>(obj));'))   # 9
    out.append((HAN, '    field_10 = 0.98f;', '    field_10 = ' + U3 + ' ? 0.99f : 0.98f;'))   # 10
    out.append((CHATH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return ' + U3 + ' ? static_cast<eTaskType>(Type ^ 0x1000) : Type; }'))   # 11
    out.append((SHOVEH, 'eTaskType GetTaskType() const override { return Type; }', 'eTaskType GetTaskType() const override { return ' + U3 + ' ? static_cast<eTaskType>(Type ^ 0x1000) : Type; }'))   # 12
    out.append((CHAT, 'memory->Constructor(m_commandName, m_partner,', 'memory->Constructor(m_commandName, ' + U3 + ' ? nullptr : m_partner,'))   # 13
    out.append((SHOVE, 'memory->Constructor(m_commandName, m_partner,', 'memory->Constructor(m_commandName, ' + U3 + ' ? nullptr : m_partner,'))   # 14
    for n in (15, 16, 17, 18, 19):
        out.append((SDF[n],) + SD[n][1])   # 15..19
    out.append((FOOT, 'return memory->Constructor(m_targetPed);', 'return memory->Constructor(' + U3 + ' ? nullptr : m_targetPed);'))   # 20
    out.append((FLIF, '    m_Pos.z = 0.0f;', '    m_Pos.z = ' + U3 + ' ? 1.0f : 0.0f;'))   # 21
    for n in (22, 23, 24):
        out.append((SDF[n],) + SD[n][1])   # 22..24
    out.append((FOOT, '        delete self->m_pointRoute;', '        if (!' + S3 + ') { delete self->m_pointRoute; }'))   # 25
    return out


ROUNDS = {1: round1, 2: round2}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def check():
    ok = True
    for n, fn in ROUNDS.items():
        texts = {}
        muts = fn()
        assert len(muts) == 26, (n, len(muts))
        for i, (rel, old, new) in enumerate(muts):
            norm = texts.setdefault(rel, (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n'))
            c = norm.count(old)
            if c != 1 or old == new:
                print('RONDA', n, 'mutacion', i, 'texto original aparece', c, 'veces en', rel, repr(old[:70]))
                ok = False
            else:
                texts[rel] = norm.replace(old, new)
    print('comprobacion', 'correcta' if ok else 'FALLIDA')
    return ok


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
    elif sys.argv[1] == 'check':
        sys.exit(0 if check() else 1)
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
