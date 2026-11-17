"""Prueba negativa del banco Fn220A (lote 220A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cambia lo que el banco observa (bytes del vector, eventos de los falsos, valor devuelto, vtables).
Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a la marca que SOLO el banco pone en 0xA9B0B0 (0x5A; el juego real usa 0 o 1), asi el juego no la ve.
Hay una mutacion propia por funcion y ronda (29 por ronda); las que viven en una plantilla compartida (push_back de 4 bytes, insert de 4 bytes) afectan a las instancias A, B y C a la vez, pero solo se prueban las de este lote.
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
NL = chr(10)
MK = '(*reinterpret_cast<volatile uint8*>(0xA9B0B0) == 0x5A)'
PA = 'source/game_sa/Attractors/PedAttractor.cpp'
PM = 'source/game_sa/PedAttractorManager.cpp'
SH = 'source/game_sa/Attractors/PedShelterAttractor.h'
CLASSES = [('Atm', 'ATM', 0x86C550), ('Seat', 'Seat', 0x86C568), ('Stop', 'Stop', 0x86C580), ('Pizza', 'Pizza', 0x86C598), ('Shelter', 'Shelter', 0x86C5B4), ('TriggerScript', 'TriggerScript', 0x86C5CC)]
CLS_FILES = ['source/game_sa/Attractors/Ped%sAttractor.cpp' % c[0] for c in CLASSES]
SOURCES = sorted([PA, PM, SH] + CLS_FILES)


def cls_mutations(rnd):
    out = []
    for fname, cname, vt in CLASSES:
        f = 'source/game_sa/Attractors/Ped%sAttractor.cpp' % fname
        if rnd == 1:
            out.append((f, '    return std::construct_at(self, fx, entity, moveState);',
                        '    return std::construct_at(self, fx, entity, ' + MK + ' ? static_cast<eMoveState>(static_cast<int>(moveState) + 1) : moveState);'))   # constructor: estado distinto
            out.append((f, '    if (flags & 1) {', '    if ((flags & 1) && !' + MK + ') {'))                                                                                # destructor con borrado: sin borrado
            out.append((f, '    *reinterpret_cast<uint32*>(self) = 0x%X;' % vt, '    *reinterpret_cast<uint32*>(self) = ' + MK + ' ? %s : %s;' % ('0x%X' % (vt + 4), '0x%X' % vt)))   # cuerpo: vtable distinta
        else:
            out.append((f, '    return std::construct_at(self, fx, entity, moveState);',
                        '    if ' + MK + ' {' + NL + '        return self;' + NL + '    }' + NL + '    return std::construct_at(self, fx, entity, moveState);'))                  # constructor: sin llamar al constructor base
            out.append((f, '        CPedAttractor::operator delete(self);' + NL + '    }' + NL + '    return self;', '        CPedAttractor::operator delete(self);' + NL + '    }' + NL + '    return ' + MK + ' ? nullptr : self;'))   # destructor con borrado: devuelve nulo
            out.append((f, '    self->Destructor();', '    if (!' + MK + ') {' + NL + '        self->Destructor();' + NL + '    }'))                                      # cuerpo: sin llamar a CPedAttractor::Destructor
    return out


def round1():
    out = []
    out.append((PA, '        const uint32 bytes = newCapacity * 12;', '        const uint32 bytes = newCapacity * 12 + (' + MK + ' ? 4u : 0u);'))                                            # 0
    out.append((PA, '        const uint32 bytes = newCapacity << 2;', '        const uint32 bytes = (newCapacity << 2) + (' + MK + ' ? 4u : 0u);'))                                           # 1 (plantilla de 4 bytes)
    out.append((PA, 'static_cast<uint32>(index) * 20u);', 'static_cast<uint32>(index) * 20u + (' + MK + ' ? 4u : 0u));'))                                                                   # 2
    out.append((PA, 'static_cast<uint32>(index) * 12u);', 'static_cast<uint32>(index) * 12u + (' + MK + ' ? 4u : 0u));'))                                                                   # 3
    out.append((PM, '    return new (self) CPedAttractorManager;', '    CPedAttractorManager* const result = new (self) CPedAttractorManager;' + NL + '    if ' + MK + ' {' + NL + '        reinterpret_cast<uint32*>(self)[1] = 0x77;' + NL + '    }' + NL + '    return result;'))   # 4
    out.append((PA, '    SArrayPushBack4<0x5EB2A0, 0x5ECF10>(self, x);', '    SArrayPushBack4<0x5EB2A0, 0x5ECF10>(self, ' + MK + ' ? x + 1 : x);'))                                                # 5
    out.append((PA, 'plugin::Call<0x5EB2E0, SArrayElem20*, uint32, const SArrayElem20*>(last, 1u, x);', 'plugin::Call<0x5EB2E0, SArrayElem20*, uint32, const SArrayElem20*>(last, ' + MK + ' ? 2u : 1u, x);'))   # 6
    out.append((PA, '    SArrayPushBack4<0x5EB310, 0x5ED4B0>(self, x);', '    SArrayPushBack4<0x5EB310, 0x5ED4B0>(self, ' + MK + ' ? x + 1 : x);'))                                                # 7
    out.append((PA, 'plugin::Call<0x5EB340, SArrayElem12*, uint32, const SArrayElem12*>(last, 1u, x);', 'plugin::Call<0x5EB340, SArrayElem12*, uint32, const SArrayElem12*>(last, ' + MK + ' ? 2u : 1u, x);'))   # 8
    out.append((PA, '    SArrayPushBack4<0x5EB390, 0x5EDA50>(self, x);', '    SArrayPushBack4<0x5EB390, 0x5EDA50>(self, ' + MK + ' ? x + 1 : x);'))                                                # 9
    out += cls_mutations(1)
    out.append((SH, 'int32 GetType() const override { return Type; }', 'int32 GetType() const override { return ' + MK + ' ? 5 : Type; }'))   # 23
    return out


def round2():
    out = []
    out.append((PA, 'plugin::Call<0x5EB340, Elem*, uint32, const Elem*>(afterPrefix, count, &valueLocal);', 'plugin::Call<0x5EB340, Elem*, uint32, const Elem*>(afterPrefix, ' + MK + ' ? count + 1 : count, &valueLocal);'))   # 0
    out.append((PA, 'plugin::Call<Inst::kFillN, uint32*, uint32, const uint32*>(afterPrefix, count, &valueSlot);', 'plugin::Call<Inst::kFillN, uint32*, uint32, const uint32*>(afterPrefix, ' + MK + ' ? count + 1 : count, &valueSlot);'))   # 1 (plantilla)
    out.append((PA, 'plugin::CallMethod<0x5ED1B0, SArrayRaw<SArrayElem20>*, SArrayElem20*, uint32, const SArrayElem20*>(self, pos, 1u, x);', 'plugin::CallMethod<0x5ED1B0, SArrayRaw<SArrayElem20>*, SArrayElem20*, uint32, const SArrayElem20*>(self, pos, ' + MK + ' ? 2u : 1u, x);'))   # 2
    out.append((PA, 'plugin::CallMethod<0x5ED750, SArrayRaw<SArrayElem12>*, SArrayElem12*, uint32, const SArrayElem12*>(self, pos, 1u, x);', 'plugin::CallMethod<0x5ED750, SArrayRaw<SArrayElem12>*, SArrayElem12*, uint32, const SArrayElem12*>(self, pos, ' + MK + ' ? 2u : 1u, x);'))   # 3
    out.append((PM, '    return new (self) CPedAttractorManager;', '    return ' + MK + ' ? nullptr : new (self) CPedAttractorManager;'))                                                              # 4
    out.append((PA, '    plugin::CallMethod<InsertAddr, SArrayRaw<uint32>*, uint32*, uint32, const uint32*>(self, self->last, 1u, x);', '    plugin::CallMethod<InsertAddr, SArrayRaw<uint32>*, uint32*, uint32, const uint32*>(self, self->last, ' + MK + ' ? 2u : 1u, x);'))   # 5, 7, 9 (plantilla compartida)
    out.append((PA, 'plugin::CallMethod<0x5EDD90, SArrayRaw<SArrayElem20>*, SArrayElem20**, SArrayElem20*, const SArrayElem20*>(self, &result, self->last, x);', 'plugin::CallMethod<0x5EDD90, SArrayRaw<SArrayElem20>*, SArrayElem20**, SArrayElem20*, const SArrayElem20*>(self, &result, self->last, ' + MK + ' ? x + 1 : x);'))   # 6
    out.append((PA, 'plugin::CallMethod<0x5EDE50, SArrayRaw<SArrayElem12>*, SArrayElem12**, SArrayElem12*, const SArrayElem12*>(self, &result, self->last, x);', 'plugin::CallMethod<0x5EDE50, SArrayRaw<SArrayElem12>*, SArrayElem12**, SArrayElem12*, const SArrayElem12*>(self, &result, self->last, ' + MK + ' ? x + 1 : x);'))   # 8
    out += cls_mutations(2)
    out.append((SH, 'int32 GetType() const override { return Type; }', 'int32 GetType() const override { return ' + MK + ' ? -1 : Type; }'))   # 23
    return out


class _Rounds(dict):
    def __missing__(self, k):
        v = self[k] = {1: round1, 2: round2}[k]()
        return v


ROUNDS = _Rounds()


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
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:100], norm.count(old)))
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
