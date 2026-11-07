"""Prueba negativa del banco Fn117A: aplica (o revierte) una ronda de mutaciones a las fuentes de produccion (una por funcion y ronda).

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Ronda 1: vtable final equivocada (0x85AB3C en lugar de 0x85AB38) en los 7 cuerpos de destructor; en los constructores se cambia un campo (byte del conductor negado, puntero nulo en lugar del argumento, byte desconocido negado,
         registro de referencia omitido).
Ronda 2: en los destructores de indice par, vtable propia intermedia equivocada (+4); en los de indice impar se quita la ultima liberacion; en los constructores se invierte el orden de los registros, se omite el registro,
         se suma 1 al dword o se anula el puntero.
Cada mutacion se aplica dentro del cuerpo de su funcion (el texto buscado debe aparecer exactamente una vez en el cuerpo).
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

EV = 'source/game_sa/Events/'


def target(i):
    """(archivo, nombre de la funcion C++) de la funcion i del banco"""
    name = O.NAMES[i]
    cls, kind = name.split('::')
    if kind == 'Destructor':
        return EV + cls[1:] + '.cpp', cls[len('CEvent'):] + 'DestructorBody'
    return EV + cls[1:] + '.cpp', cls + '::' + cls


SOURCES = sorted({target(i)[0] for i in range(len(O.NAMES))})


def block(text, fn):
    m = re.search(r'^[^\n]*\b%s\([^\n]*\{\n' % re.escape(fn), text, re.M)
    assert m, fn
    end = text.index('\n}\n', m.end())
    return m.start(), end + 3


CTOR = {
    # (ronda 1, ronda 2): (texto, reemplazo)
    'CEventDraggedOutCar': (('m_IsDriverSeat = IsDriverSeat;', 'm_IsDriverSeat = !IsDriverSeat;'),
                            ('    CEntity::SafeRegisterRef(m_Vehicle);\n    CEntity::SafeRegisterRef(m_CarJacker);\n', '    CEntity::SafeRegisterRef(m_CarJacker);\n    CEntity::SafeRegisterRef(m_Vehicle);\n')),
    'CEventGunAimedAt': (('m_AimedBy = ped;', 'm_AimedBy = nullptr;'), ('    CEntity::SafeRegisterRef(m_AimedBy);\n', '')),
    'CEventDeadPed': (('field_18 = bUnknown;', 'field_18 = !bUnknown;'), ('m_deathTimeInMs = deathTimeInMs;', 'm_deathTimeInMs = deathTimeInMs + 1;')),
    'CEventGotKnockedOverByCar': (('    CEntity::SafeRegisterRef(m_vehicle);\n', ''), ('m_vehicle = vehicle;', 'm_vehicle = nullptr;')),
    'CEventPedToChase': (('m_ped = ped;', 'm_ped = nullptr;'), ('    CEntity::SafeRegisterRef(m_ped);\n', '')),
    'CEventPedToFlee': (('    CEntity::SafeRegisterRef(m_ped);\n', ''), ('m_ped = ped;', 'm_ped = nullptr;')),
    'CEventPotentialGetRunOver': (('m_Vehicle = vehicle;', 'm_Vehicle = nullptr;'), ('    CEntity::SafeRegisterRef(m_Vehicle);\n', '')),
}


def mutations(rnd):
    out = []
    for i, name in enumerate(O.NAMES):
        f, fn = target(i)
        entry = O.ENTRIES[i]
        if entry in O.CTORS:
            out.append((f, fn, CTOR[name.split('::')[0]][rnd - 1]))
            continue
        vt, refs = O.DTORS[entry]
        if rnd == 1:
            out.append((f, fn, ('= 0x85AB38;\n}', '= 0x85AB3C;\n}')))
        elif (i // 2) % 2 == 0:
            out.append((f, fn, ('= 0x%X;\n    CEntity::' % vt, '= 0x%X;\n    CEntity::' % (vt + 4))))
        else:
            text = (ROOT / f).read_text(encoding='utf-8')
            s, e = block(text, fn)
            calls = re.findall(r'    CEntity::Safe(?:Clean|Register)\w+\(self->\w+\);\n', text[s:e])
            out.append((f, fn, (calls[-1], '')))
    return out


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
        t = texts[f].replace('\r\n', '\n') if '\r\n' not in texts[f] else texts[f]
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
