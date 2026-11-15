"""Prueba negativa del banco Fn197A (lote 197A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (32 por ronda): cambia lo que el banco observa
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
T = G + 'Tasks/TaskTypes/'
NL = chr(10)
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'

E = G + 'Events/'
HL, RL, LA, HA, OF, ES, SC, HBL = (E + 'EventHealthLow.cpp', E + 'EventHealthReallyLow.cpp', E + 'EventLowAngerAtPlayer.cpp', E + 'EventHighAngerAtPlayer.cpp', E + 'EventOnFire.cpp',
                                   E + 'EventEscalator.cpp', E + 'EventSeenCop.cpp', E + 'EventAcquaintancePedHateBadlyLit.cpp')
SUN, USE = T + 'TaskComplexSunbathe.cpp', T + 'TaskComplexUseEffect.cpp'
SOURCES = sorted({HL, RL, LA, HA, OF, ES, SC, HBL, SUN, USE})
UNAL2 = '(reinterpret_cast<uintptr_t>(this) & 2)'
VT = {HL: '0x86CBA0', RL: '0x86CBE8', LA: '0x86CC30', HA: '0x86CC78', OF: '0x86CCC0', ES: '0x86CD00'}
BODY_CALL = {RL: 'CEventEditableBody::EditableResponseDestructorBody(self, nullptr);', LA: 'CEventEditableBody::EditableResponseDestructorBody(self, nullptr);',
             HA: 'CEventEditableBody::EditableResponseDestructorBody(self, nullptr);', OF: 'CEventBody::EventDestructorBody(self, nullptr);'}


def clone_vt(f):       # vtable de la clase + 4 con `this` desalineado
    return (f, '*reinterpret_cast<uint32*>(memory) = ' + VT[f] + ';', '*reinterpret_cast<uint32*>(memory) = ' + VT[f] + ' + (' + UNALT + ' ? 4 : 0);')


def clone_null(f):     # sin memoria (devuelve nulo) con `this` desalineado
    return (f, '    if (!memory) {' + NL + '        return nullptr;' + NL + '    }', '    if (!memory || ' + UNALT + ') {' + NL + '        return nullptr;' + NL + '    }')


def sdd_skip(f, var='self', unal=UNAL):   # no libera con `this` desalineado
    return (f, '    if (flags & 1) {', '    if ((flags & 1) && !' + unal + ') {')


def sdd_null(f):
    return (f, '    return self;' + NL + '}', '    return ' + UNAL + ' ? nullptr : self;' + NL + '}')


def ctor_vt(f):
    return (f, '    *reinterpret_cast<uint32*>(this) = ' + VT[f] + ';', '    *reinterpret_cast<uint32*>(this) = ' + VT[f] + ' + (' + UNALT + ' ? 4 : 0);')


def ctor_null(f):
    return (f, '    return this;' + NL + '}', '    return ' + UNALT + ' ? nullptr : this;' + NL + '}')


def body_vt(f):
    return (f, '    *reinterpret_cast<uint32*>(self) = ' + VT[f] + ';', '    *reinterpret_cast<uint32*>(self) = ' + VT[f] + ' + (' + UNAL + ' ? 4 : 0);')


def body_zero(f):      # tras llamar al cuerpo base, con `this` desalineado deja la vtable a cero
    return (f, '    ' + BODY_CALL[f] + NL + '}', '    ' + BODY_CALL[f] + NL + '    if ' + UNAL + ' *reinterpret_cast<uint32*>(self) = 0;' + NL + '}')


def const(f, sig, ret, mut, cond=UNALT):
    return (f, sig + NL + '    return ' + ret + ';' + NL + '}', sig + NL + '    return ' + mut.replace('@', cond) + ';' + NL + '}')


def round1():
    out = [clone_vt(HL), sdd_skip(HL), ctor_vt(RL), body_vt(RL), clone_vt(RL), sdd_skip(RL), ctor_vt(LA), body_vt(LA), clone_vt(LA), sdd_skip(LA), ctor_vt(HA), body_vt(HA), clone_vt(HA), sdd_skip(HA),
           ctor_vt(OF), body_vt(OF)]                                                                                                  # 0..15
    out.append(const(OF, 'eEventType CEventOnFire::GetEventType() const {', 'EVENT_ON_FIRE', 'static_cast<eEventType>(EVENT_ON_FIRE + (@ ? 1 : 0))'))                  # 16
    out.append(const(OF, 'int32 CEventOnFire::GetLifeTime() {', '0', '@ ? 1 : 0'))                                                                                     # 17
    out.append(clone_vt(OF))                                                                                                                                           # 18
    out.append(const(OF, 'float CEventOnFire::GetLocalSoundLevel() {', '60.0f', '@ ? 61.0f : 60.0f'))                                                                     # 19
    out.append(sdd_skip(OF))                                                                                                                                           # 20
    out.append(ctor_vt(ES))                                                                                                                                            # 21
    out.append(const(ES, 'eEventType CEventEscalator::GetEventType() const {', 'EVENT_ON_ESCALATOR', 'static_cast<eEventType>(EVENT_ON_ESCALATOR + (@ ? 1 : 0))'))        # 22
    out.append(const(ES, 'int32 CEventEscalator::GetLifeTime() {', '0', '@ ? 1 : 0'))                                                                                  # 23
    out.append((ES, '    CEventBody::EventDestructorBody(self, nullptr);' + NL + '}', '    if (!' + UNAL + ') CEventBody::EventDestructorBody(self, nullptr);' + NL + '}'))  # 24
    out.append(const(ES, 'int32 CEventEscalator::GetEventPriority() const {', '56', '56 + (@ ? 1 : 0)'))                                                                # 25
    out.append(const(SC, 'int32 CEventSeenCop::GetEventPriority() const {', '21', '21 + (@ ? 1 : 0)'))                                                                  # 26
    out.append(const(OF, 'int32 CEventOnFire::GetEventPriority() const {', '66', '66 + (@ ? 1 : 0)'))                                                                    # 27
    out.append(const(HBL, 'int32 CEventAcquaintancePedHateBadlyLit::GetEventPriority() const {', '25', '25 + (@ ? 1 : 0)'))                                              # 28
    out.append((SUN, 'if (squaredSpeed > std::bit_cast<float>(uint32{0x3D23D70B})) {', 'if (squaredSpeed >= std::bit_cast<float>(uint32{0x3D23D70B})) {'))              # 29: igualdad exacta en el umbral
    out.append((SUN, 'static_cast<int32>(m_SunbatherType) + 0xF4)', 'static_cast<int32>(m_SunbatherType) + 0xF4 + (' + UNALT + ' ? 1 : 0))'))                             # 30
    out.append((USE, '    if (flags & 1) {' + NL + '        CTask::operator delete(this);', '    if ((flags & 1) && !' + UNALT + ') {' + NL + '        CTask::operator delete(this);'))  # 31
    return out


def round2():
    out = [clone_null(HL), sdd_null(HL), ctor_null(RL), body_zero(RL), clone_null(RL), sdd_null(RL), ctor_null(LA), body_zero(LA), clone_null(LA), sdd_null(LA), ctor_null(HA), body_zero(HA), clone_null(HA),
           sdd_null(HA), ctor_null(OF), body_zero(OF)]                                                                                                                  # 0..15
    out.append(const(OF, 'eEventType CEventOnFire::GetEventType() const {', 'EVENT_ON_FIRE', '@ ? static_cast<eEventType>(0) : EVENT_ON_FIRE', UNAL2))                    # 16
    out.append(const(OF, 'int32 CEventOnFire::GetLifeTime() {', '0', '@ ? -1 : 0', UNAL2))                                                                             # 17
    out.append(clone_null(OF))                                                                                                                                         # 18
    out.append(const(OF, 'float CEventOnFire::GetLocalSoundLevel() {', '60.0f', '@ ? 0.0f : 60.0f', UNAL2))                                                              # 19
    out.append(sdd_null(OF))                                                                                                                                           # 20
    out.append(ctor_null(ES))                                                                                                                                          # 21
    out.append(const(ES, 'eEventType CEventEscalator::GetEventType() const {', 'EVENT_ON_ESCALATOR', '@ ? static_cast<eEventType>(0) : EVENT_ON_ESCALATOR', UNAL2))        # 22
    out.append(const(ES, 'int32 CEventEscalator::GetLifeTime() {', '0', '@ ? -1 : 0', UNAL2))                                                                          # 23
    out.append((ES, '    CEventBody::EventDestructorBody(self, nullptr);' + NL + '}', '    CEventBody::EventDestructorBody(self, nullptr);' + NL + '    if ' + UNAL + ' *reinterpret_cast<uint32*>(self) = 0;' + NL + '}'))  # 24
    out.append(const(ES, 'int32 CEventEscalator::GetEventPriority() const {', '56', '@ ? 0 : 56', UNAL2))                                                                 # 25
    out.append(const(SC, 'int32 CEventSeenCop::GetEventPriority() const {', '21', '@ ? 0 : 21', UNAL2))                                                                    # 26
    out.append(const(OF, 'int32 CEventOnFire::GetEventPriority() const {', '66', '@ ? 0 : 66', UNAL2))                                                                     # 27
    out.append(const(HBL, 'int32 CEventAcquaintancePedHateBadlyLit::GetEventPriority() const {', '25', '@ ? 0 : 25', UNAL2))                                              # 28
    out.append((SUN, 'std::bit_cast<float>(uint32{0x3D23D70B})', 'std::bit_cast<float>(uint32{0x3D23D70A})'))                                                              # 29: la constante un ulp por debajo
    out.append((SUN, 'reinterpret_cast<const char*>(0x86E634) /*"stop sunbathing"*/, false);', 'reinterpret_cast<const char*>(0x86E634) /*"stop sunbathing"*/, ' + UNALT + ' ? true : false);'))  # 30
    out.append((USE, '        CTask::operator delete(this);' + NL + '    }' + NL + '    return this;', '        CTask::operator delete(this);' + NL + '    }' + NL + '    return ' + UNALT + ' ? nullptr : this;'))  # 31
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
