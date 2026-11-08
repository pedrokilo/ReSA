"""Prueba negativa del banco Fn119A: aplica (o revierte) una ronda de mutaciones a las fuentes de produccion (una por funcion y ronda).

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Ronda 1: vtable final equivocada (0x85AB3C en lugar de 0x85AB38) en los 10 cuerpos de destructor; en los constructores se cambia un campo (byte negado, puntero nulo, float negado, ejes del vector intercambiados,
         byte +8 de Revived) o se omite el registro de una referencia.
Ronda 2: en los destructores con referencias, vtable propia intermedia equivocada (+4) o liberacion omitida; en los que solo dejan la vtable de CEvent, otra vtable final o un byte extra escrito en el relleno; en los
         constructores se omite el otro registro, se anula un puntero, se toma el valor absoluto del float, se anula la z del vector o se escribe un dword extra.
Cada mutacion es un reemplazo de texto que debe aparecer exactamente una vez en el archivo de su clase.
"""
import hashlib
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as O  # noqa: E402

EV = 'source/game_sa/Events/'


def target(i):
    """archivo de la funcion i del banco"""
    cls = O.NAMES[i].split('::')[0]
    return EV + cls[1:] + '.cpp'


SOURCES = sorted({target(i) for i in range(len(O.NAMES))})
END = '= 0x85AB38;\n}\n} // namespace'


def dend(v):
    return (END, '= %s;\n}\n} // namespace' % v)


def extra(stmt):
    return (END, '= 0x85AB38;\n    %s\n}\n} // namespace' % stmt)


# (texto buscado, reemplazo) por funcion y ronda; cada texto debe aparecer exactamente una vez en el archivo
R1 = {
    'CEventPedEnteredMyVehicle::Constructor': ('    m_PedThatEntered->RegisterReference(reinterpret_cast<CEntity**>(&m_PedThatEntered));\n', ''),
    'CEventPedEnteredMyVehicle::Destructor': dend('0x85AB3C'),
    'CEventRevived::Constructor': ('this->CEventRevived::CEventRevived();\n', 'this->CEventRevived::CEventRevived();\n    reinterpret_cast<uint8*>(this)[8] = 1;\n'),
    'CEventRevived::Destructor': dend('0x85AB3C'),
    'CEventChatPartner::Constructor': ('m_leadSpeaker = leadSpeaker;', 'm_leadSpeaker = !leadSpeaker;'),
    'CEventChatPartner::Destructor': dend('0x85AB3C'),
    'CEventSexyPed::Constructor': ('m_SexyPed = ped;', 'm_SexyPed = nullptr;'),
    'CEventSexyPed::Destructor': dend('0x85AB3C'),
    'CEventSexyVehicle::Constructor': ('    CEntity::SafeRegisterRef(m_vehicle);\n', ''),
    'CEventSexyVehicle::Destructor': dend('0x85AB3C'),
    'CEventInWater::Constructor': ('m_acceleration = acceleration;', 'm_acceleration = -acceleration;'),
    'CEventInWater::Destructor': dend('0x85AB3C'),
    'CEventFireNearby::Constructor': ('m_position = position;', 'm_position = CVector(position.y, position.x, position.z);'),
    'CEventFireNearby::Destructor': dend('0x85AB3C'),
    'CEventInAir::Destructor': dend('0x85AB3C'),
    'CEventHitByWaterCannon::Destructor': dend('0x85AB3C'),
    'CEventPotentialWalkIntoFire::Destructor': dend('0x85AB3C'),
}
R2 = {
    'CEventPedEnteredMyVehicle::Constructor': ('    m_Vehicle->RegisterReference(reinterpret_cast<CEntity**>(&m_Vehicle));\n', ''),
    'CEventPedEnteredMyVehicle::Destructor': ('= 0x85AFE8;\n', '= 0x85AFEC;\n'),
    'CEventRevived::Constructor': ('this->CEventRevived::CEventRevived();\n', 'this->CEventRevived::CEventRevived();\n    reinterpret_cast<uint32*>(this)[1] = 1;\n'),
    'CEventRevived::Destructor': extra('reinterpret_cast<uint8*>(self)[8] = 1;'),
    'CEventChatPartner::Constructor': ('m_partner = partner;', 'm_partner = nullptr;'),
    'CEventChatPartner::Destructor': ('    CEntity::SafeCleanUpRef(self->m_partner);\n', ''),
    'CEventSexyPed::Constructor': ('    CEntity::SafeRegisterRef(m_SexyPed);\n', ''),
    'CEventSexyPed::Destructor': ('= 0x85B0B0;\n', '= 0x85B0B4;\n'),
    'CEventSexyVehicle::Constructor': ('m_vehicle = vehicle;', 'm_vehicle = nullptr;'),
    'CEventSexyVehicle::Destructor': ('= 0x85B0F8;\n', '= 0x85B0FC;\n'),
    'CEventInWater::Constructor': ('m_acceleration = acceleration;', 'm_acceleration = std::fabs(acceleration);'),
    'CEventInWater::Destructor': dend('0'),
    'CEventFireNearby::Constructor': ('m_position = position;', 'm_position = CVector(position.x, position.y, 0.0f);'),
    'CEventFireNearby::Destructor': dend('0x85AB40'),
    'CEventInAir::Destructor': extra('reinterpret_cast<uint8*>(self)[9] = 0;'),
    'CEventHitByWaterCannon::Destructor': extra('reinterpret_cast<uint8*>(self)[9] = 0;'),
    'CEventPotentialWalkIntoFire::Destructor': extra('reinterpret_cast<uint8*>(self)[9] = 0;'),
}


def mutations(rnd):
    table = R1 if rnd == 1 else R2
    return [(target(i), O.NAMES[i], table[O.NAMES[i]]) for i in range(len(O.NAMES))]


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
        assert t.count(old) == 1, (f, fn, old, t.count(old))
        t = t.replace(old, new, 1)
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
