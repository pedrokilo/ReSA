"""Prueba negativa del banco Fn204A (lote 204A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (28 por ronda): cambia lo que el banco observa (valor devuelto, bytes
cambiados, eventos de los falsos, memoria global). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si: `this`
o un puntero desalineado, un puntero de peaton impar, un indice de conocido fuera de [0, 4] o negativo.
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
NL = chr(10)
AVE = G + 'Tasks/TaskTypes/TaskComplexAvoidEntity.cpp'
SIT = G + 'Tasks/TaskTypes/Interior/TaskInteriorSitInChair.cpp'
LIE = G + 'Tasks/TaskTypes/Interior/TaskInteriorLieInBed.cpp'
VSC, PSC, OSC = G + 'VehicleScanner.cpp', G + 'PedScanner.cpp', G + 'ObjectScanner.cpp'
EVS = G + 'Events/EventScanner.cpp'
EDE = G + 'Events/EventDeath.cpp'
EGR = G + 'Events/EventPotentialGetRunOver.cpp'
EVENT_FILES = [G + 'Events/' + n + '.cpp' for n in ('EventDeadPed', 'EventGroupEvent', 'EventPotentialGetRunOver', 'EventPotentialWalkIntoVehicle', 'EventPotentialWalkIntoObject',
                                                    'EventPotentialWalkIntoPed', 'EventVehicleHitAndRun', 'EventPedEnteredMyVehicle', 'EventChatPartner', 'EventSexyPed')]
SOURCES = sorted({AVE, SIT, LIE, VSC, PSC, OSC, EVS, EDE, EGR, *EVENT_FILES})
UNALS = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'


def text(rel):
    return (ROOT / rel).read_bytes().decode('utf-8').replace(chr(13) + chr(10), NL)


def event_sdd(rel, rnd):
    """destructor con borrado de un evento: ronda 1 no libera la ranura con `this` desalineado; ronda 2 no llama al cuerpo con `this` desalineado"""
    t = text(rel)
    m = re.search(r'(    (?:\w+\(self, nullptr\)|self->CEventGroupEvent::~CEventGroupEvent\(\));)' + NL + r'    if \(flags & 1\) \{' + NL + r'        CEvent::operator delete\(self\);', t)
    assert m, rel
    old = m.group(0)
    if rnd == 1:
        new = old.replace('    if (flags & 1) {', '    if ((flags & 1) && !' + UNALS + ') {')
    else:
        new = old.replace(m.group(1), '    if (!' + UNALS + ') {' + NL + '    ' + m.group(1) + NL + '    }')
    return (rel, old, new)


def round1():
    out = []
    out.append((AVE, '    m_IsDoingIK = true;' + NL + '}', '    m_IsDoingIK = !' + UNALT + ';' + NL + '}'))
    out.append((AVE, 'g_ikChainMan.AbortLookAt(ped, 250);', 'g_ikChainMan.AbortLookAt(ped, ' + UNALT + ' ? 251 : 250);'))
    out.append((AVE, '    QuitIK(ped);' + NL + '    return true;', '    QuitIK(ped);' + NL + '    return !' + UNALT + ';'))
    out.append((AVE, '    QuitIK(ped);' + NL + '    return nullptr;' + NL + '}', '    QuitIK(ped);' + NL + '    return ' + UNALT + ' ? static_cast<CTask*>(this) : nullptr;' + NL + '}'))
    out.append((AVE, '    return m_pSubTask;' + NL + '}', '    return ' + UNALT + ' ? nullptr : m_pSubTask;' + NL + '}'))
    out.append((AVE, 'CTask::operator new(sizeof(CTaskComplexAvoidEntity))', 'CTask::operator new(sizeof(CTaskComplexAvoidEntity) + (' + UNALT + ' ? 4 : 0))'))
    out.append((AVE, 'CTaskSimpleGoToPoint(m_MoveState, m_GoToPt, 0.5f, false, false)', 'CTaskSimpleGoToPoint(m_MoveState, m_GoToPt, ' + UNALT + ' ? 0.25f : 0.5f, false, false)'))
    out.append((SIT, '        CTask::operator delete(self);' + NL, '        if (!' + UNALS + ') { CTask::operator delete(self); }' + NL))
    out.append((SIT, '    ped->SetMoveState(PEDMOVE_STILL);', '    ped->SetMoveState(' + UNALS + ' ? PEDMOVE_WALK : PEDMOVE_STILL);'))
    out.append((LIE, '        CTask::operator delete(this);', '        if (!' + UNALT + ') { CTask::operator delete(this); }'))
    out.append((VSC, '    *reinterpret_cast<uint32*>(self) = 0x86CD40;', '    *reinterpret_cast<uint32*>(self) = ' + UNALS + ' ? 0x86CD41 : 0x86CD40;'))
    out.append((PSC, '    *reinterpret_cast<uint32*>(self) = 0x86CD40;', '    *reinterpret_cast<uint32*>(self) = ' + UNALS + ' ? 0x86CD41 : 0x86CD40;'))
    out.append((OSC, '    words[0]    = 0x86CD8C;', '    words[0]    = ' + UNALS + ' ? 0x86CD8D : 0x86CD8C;'))
    out.append((OSC, '    *reinterpret_cast<uint32*>(self) = 0x86CD40;', '    *reinterpret_cast<uint32*>(self) = ' + UNALS + ' ? 0x86CD41 : 0x86CD40;'))
    out.append((EVS, '    CEventPotentialWalkIntoPed event(best, targetPoint, moveState);', '    CEventPotentialWalkIntoPed event(best, targetPoint, (reinterpret_cast<uintptr_t>(closestPed) & 1) ? PEDMOVE_RUN : moveState);'))
    out.append((EVS, '    default:' + NL + '        break;' + NL + '    }' + NL + '    return added;', '    default:' + NL + '        added = true;' + NL + '        break;' + NL + '    }' + NL + '    return added;'))
    out.append((EDE, '    *reinterpret_cast<uint32*>(self) = 0x85AB38;', '    *reinterpret_cast<uint32*>(self) = ' + UNALS + ' ? 0x85AB39 : 0x85AB38;'))
    out.append((EGR, '    return ped->IsAlive()' + NL + '       && !ped->m_pAttachedTo', '    return !' + UNALT + ' && ped->IsAlive()' + NL + '       && !ped->m_pAttachedTo'))
    for rel in EVENT_FILES:
        out.append(event_sdd(rel, 1))
    return out


def round2():
    out = []
    out.append((AVE, '    pedToTarget.Normalise(); // Como el original: CVector::Normalise (0x59C910)', '    if (!' + UNALT + ') { pedToTarget.Normalise(); } // Como el original: CVector::Normalise (0x59C910)'))
    out.append((AVE, 'void CTaskComplexAvoidEntity::QuitIK(CPed* ped) {' + NL + '    if (m_IsDoingIK) {', 'void CTaskComplexAvoidEntity::QuitIK(CPed* ped) {' + NL + '    if (m_IsDoingIK && !' + UNALT + ') {'))
    out.append((AVE, 'const CEvent* event) {' + NL + '    QuitIK(ped);', 'const CEvent* event) {' + NL + '    if (!' + UNALT + ') { QuitIK(ped); }'))
    out.append((AVE, 'CTask* CTaskComplexAvoidEntity::CreateNextSubTask(CPed* ped) {' + NL + '    QuitIK(ped);', 'CTask* CTaskComplexAvoidEntity::CreateNextSubTask(CPed* ped) {' + NL + '    if (!' + UNALT + ') { QuitIK(ped); }'))
    out.append((AVE, '    SetUpIK(ped);' + NL + '    if (ped->GetIntelligence()->m_AnotherStaticCounter > 30) {', '    if (!' + UNALT + ') { SetUpIK(ped); }' + NL + '    if (ped->GetIntelligence()->m_AnotherStaticCounter > 30) {'))
    out.append((AVE, 'm_MoveState, m_Entity, &m_TargetPt);', 'm_MoveState, ' + UNALT + ' ? nullptr : m_Entity, &m_TargetPt);'))
    out.append((AVE, '    m_PedPos = ped->GetPosition();', '    m_PedPos = ped->GetPosition();' + NL + '    if ' + UNALT + ' { m_PedPos.x = 0.0f; }'))
    out.append((SIT, '    SitInChairDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    if (!' + UNALS + ') { SitInChairDestructorBody(self, nullptr); }' + NL + '    if (flags & 1) {'))
    out.append((SIT, '    if (SitInChairField<uint8>(self, 0x29)) {' + NL + '        if (!RpAnimBlendClumpGetAssociation(', '    if (SitInChairField<uint8>(self, 0x29) && !' + UNALS + ') {' + NL + '        if (!RpAnimBlendClumpGetAssociation('))
    out.append((LIE, '    Destructor();' + NL + '    if (flags & 1) {', '    if (!' + UNALT + ') { Destructor(); }' + NL + '    if (flags & 1) {'))
    out.append((VSC, '    self->Clear();', '    if (!' + UNALS + ') { self->Clear(); }'))
    out.append((PSC, '    self->Clear();', '    if (!' + UNALS + ') { self->Clear(); }'))
    out.append((OSC, '    words[1] = 0;', '    words[1] = ' + UNALS + ' ? 1 : 0;'))
    out.append((OSC, '    self->Clear();', '    if (!' + UNALS + ') { self->Clear(); }'))
    out.append((EVS, '    float limitSq = 2.5f * 2.5f;', '    float limitSq = 2.5f * 2.5f + ((reinterpret_cast<uintptr_t>(closestPed) & 1) ? 1.0f : 0.0f);'))
    out.append((EVS, '    CPed* const other      = static_cast<CPed*>(entity);', '    CPed* const other      = static_cast<CPed*>(entity);' + NL + '    if (id < 0) { return true; }'))
    out.append((EDE, '    *reinterpret_cast<uint32*>(self) = 0x85AB38;', '    if ' + UNALS + ' { return; }' + NL + '    *reinterpret_cast<uint32*>(self) = 0x85AB38;'))
    out.append((EGR, '       && !m_Vehicle->IsBoat();', '       && !m_Vehicle->IsBoat() && !' + UNALT + ';'))
    for rel in EVENT_FILES:
        out.append(event_sdd(rel, 2))
    return out


ROUNDS = {1: round1, 2: round2}   # perezoso: las mutaciones se calculan al aplicar (restore no debe leer fuentes mutadas)


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
