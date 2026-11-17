"""Prueba negativa del banco Fn221A (lote 221A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cambia lo que el banco observa (bytes del vector, eventos de los falsos, valor devuelto, vtables).
Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a la marca que SOLO el banco pone en 0xA9B0B0 (0x5A; el juego real usa 0 o 1), asi el juego no la ve.
Hay una mutacion propia por funcion y ronda (25 por ronda); las que viven en una plantilla compartida (push_back de 4 bytes, insert de 4 bytes) afectan a las instancias A, B y C a la vez, pero solo se prueban las de este lote.
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
PM = 'source/game_sa/PedAttractorManager.cpp'
SE = 'source/game_sa/Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntity.h'
TS = 'source/game_sa/Scripts/TheScripts.cpp'
BC = 'source/game_sa/Tasks/TaskTypes/TaskComplexBeCop.h'
EP = 'source/game_sa/ExeProtectionStub.cpp'
CLASSES = ['LookAt', 'Scripted', 'Park', 'Step']
VT = {'LookAt': 0x86C5E4, 'Scripted': 0x86C5FC, 'Park': 0x86C614, 'Step': 0x86C62C}
CLS_FILES = ['source/game_sa/Attractors/Ped%sAttractor.cpp' % c for c in CLASSES]
SOURCES = sorted([PM, SE, TS, BC, EP] + CLS_FILES)


def cls_mutations(rnd):
    out = []
    for c in CLASSES:
        f = 'source/game_sa/Attractors/Ped%sAttractor.cpp' % c
        if rnd == 1:
            out.append((f, '    return std::construct_at(this, effect, entity, moveState);', '    return std::construct_at(this, effect, entity, ' + MK + ' ? static_cast<eMoveState>(static_cast<int>(moveState) + 1) : moveState);'))
            out.append((f, '    if (flags & 1) {', '    if ((flags & 1) && !' + MK + ') {'))
            out.append((f, '    this->CPedAttractor::Destructor();', '    if (!' + MK + ') {' + NL + '        this->CPedAttractor::Destructor();' + NL + '    }'))
        else:
            out.append((f, '    return std::construct_at(this, effect, entity, moveState);', '    if ' + MK + ' {' + NL + '        return this;' + NL + '    }' + NL + '    return std::construct_at(this, effect, entity, moveState);'))
            out.append((f, '    return this;' + NL + '}', '    return ' + MK + ' ? nullptr : this;' + NL + '}'))
            out.append((f, '    *reinterpret_cast<uint32*>(this) = 0x%X;' % VT[c], '    *reinterpret_cast<uint32*>(this) = ' + MK + ' ? 0x%X : 0x%X;' % (VT[c] + 4, VT[c])))
            out.append((f, '    Destructor();' + NL, '    if (!' + MK + ') {' + NL + '        Destructor();' + NL + '    }' + NL))
    return out


def round1():
    out = cls_mutations(1)
    out.append((PM, '            return attractor->BroadcastArrival(ped);', '            return attractor->BroadcastArrival(' + MK + ' ? nullptr : ped);'))
    out.append((PM, '        result->RegisterPed(ped);', '        if (!' + MK + ') {' + NL + '            result->RegisterPed(ped);' + NL + '        }'))
    out.append((SE, '        if (flags & 1) {', '        if ((flags & 1) && !' + MK + ') {'))
    out.append((TS, '    *pIndex = -1;', '    *pIndex = ' + MK + ' ? 0 : -1;'))
    out.append((SE, '        return static_cast<const CVector&>(seekPos); // copia (sin NRVO) a la salida despues de la llamada', '        CVector mutated = seekPos;' + NL + '        if (' + MK + ') { mutated.z += 1.0f; }' + NL + '        return mutated;'))
    out.append((BC, 'm_pGoToPointAndStandStillTask = static_cast<CTaskComplexGoToPointAndStandStill*>(task);', 'm_pGoToPointAndStandStillTask = ' + MK + ' ? nullptr : static_cast<CTaskComplexGoToPointAndStandStill*>(task);'))
    out.append((EP, '    ctx->m_State[0] = 0x67452301;', '    ctx->m_State[0] = ' + MK + ' ? 0x67452300 : 0x67452301;'))
    out.append((EP, '    ctx->m_nBitCount[1] += length >> 29;', '    ctx->m_nBitCount[1] += (' + MK + ' ? 1u : 0u) + (length >> 29);'))
    out.append((EP, '    block[14] = bitsLow;', '    block[14] = bitsLow ^ (' + MK + ' ? 1u : 0u);'))
    out.append((EP, '        ResolveProc(entry);' + NL + '    }' + NL + '    ResolveLoaderImports();', '        ResolveProc(entry);' + NL + '    }' + NL + '    if (!' + MK + ') {' + NL + '        ResolveLoaderImports();' + NL + '    }'))
    out.append((EP, '    Store32(0xC9E0F5, 1);', '    Store32(0xC9E0F5, ' + MK + ' ? 2 : 1);'))
    out.append((EP, '        Store32(entry.dest, entry.value);', '        Store32(entry.dest, entry.value + (' + MK + ' ? 1u : 0u));'))
    out.append((EP, '    Store32(0xC9E1FB, 0x94);', '    Store32(0xC9E1FB, ' + MK + ' ? 0x95 : 0x94);'))
    return out


def round2():
    out = cls_mutations(2)
    out.append((PM, '        if (*cursor == attractor) {', '        if (*cursor == attractor && !' + MK + ') {'))
    out.append((PM, '        plugin::CallMethod<0x5EE400, AttractorList*, CPedAttractor**>(&attractors, &result);', '        if (!' + MK + ') {' + NL + '            plugin::CallMethod<0x5EE400, AttractorList*, CPedAttractor**>(&attractors, &result);' + NL + '        }'))
    out.append((SE, '        DestructorBodyExe(self, nullptr);', '        if (!' + MK + ') {' + NL + '            DestructorBodyExe(self, nullptr);' + NL + '        }'))
    out.append((TS, 'IsPointWithinSearchLight(pos, i)) {', 'IsPointWithinSearchLight(pos, ' + MK + ' ? (i ^ 1) : i)) {'))
    out.append((SE, '        calculator.ComputeEntitySeekPos(*ped, *m_entityToSeek, seekPos);', '        calculator.ComputeEntitySeekPos(*ped, *m_entityToSeek, seekPos);' + NL + '        if (' + MK + ') {' + NL + '            seekPos.x = 0.0f; seekPos.y = 0.0f; seekPos.z = 0.0f;' + NL + '        }'))
    out.append((BC, ': CTaskComplexWanderCop(moveState, dir) {', ': CTaskComplexWanderCop(moveState, ' + MK + ' ? static_cast<uint8>(dir + 1) : dir) {'))
    out.append((EP, '    ctx->m_nBitCount[1] = 0;', '    ctx->m_nBitCount[1] = ' + MK + ' ? 1 : 0;'))
    out.append((EP, '        ctx->m_Buffer[index] = *input;', '        ctx->m_Buffer[index] = *input + (' + MK + ' ? 1 : 0);'))
    out.append((EP, '        ctx->m_Digest[i * 4 + 0] = static_cast<uint8>(word);', '        ctx->m_Digest[i * 4 + 0] = static_cast<uint8>(word) ^ (' + MK + ' ? 1 : 0);'))
    out.append((EP, '    Store32(entry.dest, static_cast<uint32>(reinterpret_cast<uintptr_t>(proc)));', '    Store32(entry.dest, static_cast<uint32>(reinterpret_cast<uintptr_t>(proc)) + (' + MK + ' ? 1u : 0u));'))
    out.append((EP, '    Store32(0xC9E153, 0);', '    Store32(0xC9E153, ' + MK + ' ? 3 : 0);'))
    out.append((EP, 'for (const StoreEntry& entry : CALLBACK_STORES) {', 'for (const StoreEntry& entry : CALLBACK_STORES) {' + NL + '        if (' + MK + ' && entry.dest == 0xC9E9CB) {' + NL + '            continue;' + NL + '        }'))
    out.append((EP, '        return suiteMask != 0 ? 1 : 0;', '        return ' + MK + ' ? 0 : (suiteMask != 0 ? 1 : 0);'))
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
