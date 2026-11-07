"""Prueba negativa del banco Fn115A: aplica (o revierte) una ronda de mutaciones a las fuentes de produccion (una por funcion y ronda).

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion se aplica dentro del cuerpo de su funcion (el texto buscado debe aparecer exactamente una vez en el cuerpo).
Ronda 1: vtable propia equivocada (+4) o cola omitida en destructores; en los constructores se cambia un campo, un bit o una constante.
Ronda 2: otra mutacion distinta por funcion (liberacion omitida o con otro desplazamiento, llamada base omitida, mascara de bits, constante, orden o campo cambiado).
"""
import hashlib
import re
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as O  # noqa: E402

G = 'source/game_sa/'
T = G + 'Tasks/TaskTypes/'
# (archivo, funcion C++, (texto, reemplazo) de la ronda 1, (texto, reemplazo) de la ronda 2), en el orden de O.NAMES
M = [
    (G + 'Events/EventDontJoinPlayerGroup.cpp', 'DontJoinPlayerGroupDestructorBody', ('= 0x86D0E0;', '= 0x86D0E4;'), ('    CEntity::SafeCleanUpRef(*reinterpret_cast<CEntity**>(bytes + 0xC));\n', '')),
    (G + 'Events/EventPassObject.cpp', 'PassObjectDestructorBody', ('    CEntity::SafeCleanUpRef(*reinterpret_cast<CEntity**>(bytes + 0xC));\n', ''), ('= 0x86F8B8;', '= 0x86F8BC;')),
    (T + 'TaskComplexKillPedOnFoot.cpp', 'KillPedOnFootDestructorBody', ('bytes + 0x10)', 'bytes + 0x14)'), ('    CTaskComplexBody::Destructor(self, nullptr);\n', '')),
    (G + 'Tasks/TaskComplexSequence.cpp', 'SequenceDestructorBody', ('    self->Flush();\n', ''), ('= 0x86E200;', '= 0x86E204;')),
    (T + 'TaskSimpleCarSetPedInAsDriver.cpp', 'CarSetPedInAsDriverDestructorBody', ('= 0x86EE28;', '= 0x86EE2C;'), ('    CEntity::SafeCleanUpRef(*reinterpret_cast<CEntity**>(bytes + 0x10));\n', '')),
    (T + 'TaskSimpleCarSetPedOut.cpp', 'CarSetPedOutDestructorBody', ('    self->CTask::Destructor();\n', ''), ('bytes + 0x8)', 'bytes + 0xC)')),
    (T + 'TaskSimpleGoTo.cpp', 'GoToDestructorBody', ('= 0x86FD1C;', '= 0x86FD18;'), ('    self->CTask::Destructor();\n', '')),
    (T + 'TaskComplexWander.cpp', 'WanderDestructorBody', ('    CTaskComplexBody::Destructor(self, nullptr);\n', ''), ('= 0x86FE84;', '= 0x86FE88;')),
    (T + 'TaskComplexJump.cpp', 'JumpDestructorBody', ('= 0x870570;', '= 0x870574;'), ('    CTaskComplexBody::Destructor(self, nullptr);\n', '')),
    (T + 'TaskSimpleAnim.cpp', 'SimpleAnimConstructorBody', ('(holdLastFrame & 1) << 2', '(holdLastFrame & 1) << 1'), ('bytes[0xC] & 0xF8', 'bytes[0xC] & 0xF0')),
    (T + 'TaskSimplePlayHandSignalAnim.cpp', 'HandSignalConstructorBody', ('!= 0 ? 1 : 0;', '== 0 ? 1 : 0;'), ('(holdLastFrame & 1) << 2', '(holdLastFrame & 3) << 2')),
    (T + 'TaskSimpleDrown.cpp', 'DrownConstructorBody', ('= 0x40800000;', '= 0x40000000;'), ('    bytes[0x28] = 1;\n', '    bytes[0x28] = 0;\n')),
    (T + 'TaskComplexAttractorPartnerWait.cpp', 'AttractorPartnerWaitConstructorBody', ('static_cast<uint8>(flag);', 'static_cast<uint8>(flag != 0);'), ('= 0x86E284;', '= 0x86E288;')),
    (T + 'TaskComplexUseGoggles.cpp', 'UseGogglesConstructorBody', ('= 0x86E4A8;', '= 0x86E4AC;'), ('    CTaskComplexBody::Constructor(self, nullptr);\n', '')),
    (T + 'TaskComplexUseEffectRunning.cpp', 'UseEffectRunningConstructorBody', ('(bytes + 0x1C) = 6;', '(bytes + 0x1C) = 7;'), ('Constructor(effect, entity);', 'Constructor(entity, effect);')),
    (T + 'TaskComplexUseEffectSprinting.cpp', 'UseEffectSprintingConstructorBody', ('(bytes + 0x1C) = 7;', '(bytes + 0x1C) = 6;'), ('= 0x86E5C0;', '= 0x86E5C4;')),
    (T + 'TaskComplexStuckInAir.cpp', 'StuckInAirConstructorBody', ('    CTaskComplexBody::Constructor(self, nullptr);\n', ''), ('= 0x870608;', '= 0x87060C;')),
    (T + 'TaskSimpleArrestPed.cpp', 'ArrestPedConstructorBody', ('    bytes[0xC] = 0;\n', '    bytes[0xC] = 1;\n'), ('RegisterReference(reinterpret_cast<CEntity**>(bytes + 8))', 'RegisterReference(reinterpret_cast<CEntity**>(bytes + 0xC))')),
    (G + 'Tasks/Allocators/TaskAllocatorPlayerCommandAttack.cpp', 'PlayerCommandAttackConstructorBody', ('FindPlayerPed(-1)', 'FindPlayerPed(0)'), ('(bytes + 0x1C) = value;', '(bytes + 0x1C) = static_cast<uint32>(groupTargetId);')),
    (T + 'TaskSimpleFacial.cpp', 'FacialConstructorBody', ('(bytes + 0x18) = second;', '(bytes + 0x18) = first;'), ('    bytes[0x11] = 0;\n', '    bytes[0x11] = 1;\n')),
]
assert len(M) == len(O.NAMES)
SOURCES = sorted({m[0] for m in M})


def block(text, fn):
    m = re.search(r'^[^\n]*\b%s\([^\n]*\{\n' % re.escape(fn), text, re.M)
    assert m, fn
    end = text.index('\n}\n', m.end())
    return m.start(), end + 3


def mutations(rnd):
    return [(f, fn, (r1, r2)[rnd - 1]) for f, fn, r1, r2 in M]


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def apply(rnd):
    GOOD.mkdir(parents=True, exist_ok=True)
    for f in SOURCES:
        dst = GOOD / f.replace('/', '__')
        if not dst.exists():
            shutil.copyfile(ROOT / f, dst)
        assert sha(dst) == sha(ROOT / f), 'la fuente ya esta mutada: ' + f
    texts = {f: (ROOT / f).read_bytes().decode('utf-8') for f in SOURCES}
    muts = mutations(rnd)
    for f, fn, (old, new) in muts:
        crlf = '\r\n' in texts[f]
        t = texts[f].replace('\r\n', '\n')
        s, e = block(t, fn)
        b = t[s:e]
        assert b.count(old) == 1, (f, fn, old, b.count(old))
        t = t[:s] + b.replace(old, new, 1) + t[e:]
        texts[f] = t.replace('\n', '\r\n') if crlf else t
    for f, t in texts.items():
        (ROOT / f).write_bytes(t.encode('utf-8'))
    print('mutadas', len(muts), 'funciones, ronda', rnd)


def restore():
    for f in SOURCES:
        src = GOOD / f.replace('/', '__')
        shutil.copyfile(src, ROOT / f)
        assert sha(src) == sha(ROOT / f), f
    print('restauradas', len(SOURCES), 'fuentes')


if __name__ == '__main__':
    if sys.argv[1] == 'apply':
        apply(int(sys.argv[2]))
    elif sys.argv[1] == 'restore':
        restore()
    else:
        for f in SOURCES:
            print(sha(ROOT / f), f)
