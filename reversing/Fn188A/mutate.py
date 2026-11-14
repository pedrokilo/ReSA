"""Prueba negativa del banco Fn188A (lote 188A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (13 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y
el banco si: `this` desalineado (ronda 1: cualquier desalineo; ronda 2: desalineo de 2 bytes).
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
E = 'source/game_sa/Events/'
NL = chr(10)
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
UNAL2 = '((reinterpret_cast<uintptr_t>(self) & 3) == 2)'
UNALT2 = '((reinterpret_cast<uintptr_t>(this) & 3) == 2)'

SCRIPTED, RESPECT, LIKE, DISLIKE = E + 'EventScriptedAttractor.cpp', E + 'EventAcquaintancePedRespect.cpp', E + 'EventAcquaintancePedLike.cpp', E + 'EventAcquaintancePedDislike.cpp'
SOURCES = sorted({SCRIPTED, RESPECT, LIKE, DISLIKE})
SDD1 = '    if (flags & 1) {'
NULLCHK = '    if (!memory) {'
# (clase, archivo, prefijo del cuerpo en la funcion de borrado, vtable, llamada al cuerpo base, llamada al constructor de Clone)
CLASSES = (
    (SCRIPTED, 'ScriptedAttractorDestructorBody', 'CEventAttractorBody::AttractorDestructorBody(self, nullptr);', '0x86C9F0', '0x86C9F4', '->Constructor(m_2dEffect, m_entity, false);', '->Constructor(m_2dEffect, m_entity, ' + UNALT + ' ? true : false);'),
    (RESPECT, 'RespectDestructorBody', 'CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);', '0x86CA38', '0x86CA3C', '->Constructor(m_AcquaintancePed);', '->Constructor(' + UNALT + ' ? nullptr : m_AcquaintancePed);'),
    (LIKE, 'LikeDestructorBody', 'CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);', '0x86CA80', '0x86CA84', '->Constructor(m_AcquaintancePed);', '->Constructor(' + UNALT + ' ? nullptr : m_AcquaintancePed);'),
    (DISLIKE, 'DislikeDestructorBody', 'CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);', '0x86CAC8', '0x86CACC', '->Constructor(m_AcquaintancePed);', '->Constructor(' + UNALT + ' ? nullptr : m_AcquaintancePed);'),
)


def sdd1(prefix, unal):
    return (prefix + NL + SDD1, prefix + NL + '    if (flags & (' + unal + ' ? 2 : 1)) {')


def sdd2(prefix, unal2):
    return (prefix + NL + SDD1, '    if (!' + unal2 + ') ' + prefix.lstrip() + NL + SDD1)


def round1():
    out = []
    for n, (f, body, call, vt, vt2, ctor, ctor2) in enumerate(CLASSES):
        out.append((f, '    *reinterpret_cast<uint32*>(self) = ' + vt + ';', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? ' + vt2 + ' : ' + vt + ';'))                       # cuerpo
        if n == 0:
            out.append((f, '    return EVENT_SCRIPTED_ATTRACTOR;', '    return ' + UNALT + ' ? static_cast<eEventType>(49) : EVENT_SCRIPTED_ATTRACTOR;'))                    # GetEventType
        out.append((f, ctor, ctor2))                                                                                                                                   # Clone
        out.append((f,) + sdd1('    ' + body + '(self, nullptr);', UNAL))                                                                                              # destructor con borrado
    return out


def round2():
    out = []
    for n, (f, body, call, vt, vt2, ctor, ctor2) in enumerate(CLASSES):
        out.append((f, '    ' + call, '    if (!' + UNAL2 + ') ' + call))                                                                                              # cuerpo
        if n == 0:
            out.append((f, '    return EVENT_SCRIPTED_ATTRACTOR;', '    return ' + UNALT2 + ' ? static_cast<eEventType>(49) : EVENT_SCRIPTED_ATTRACTOR;'))                  # GetEventType
        out.append((f, NULLCHK, '    if (!memory || ' + UNALT2 + ') {'))                                                                                               # Clone
        out.append((f,) + sdd2('    ' + body + '(self, nullptr);', UNAL2))                                                                                             # destructor con borrado
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
