"""Prueba negativa del banco Fn207A (lote 207A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba (sin tocar nada) que el texto original de cada edicion aparece exactamente una vez
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada edicion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (22 por ronda; las seis funciones vacias de CPathFind, 11 a 16, llevan
siete ediciones: una definicion de seis funciones sustitutas y el cambio de cada enganche): cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos, memoria global). Los hooks de la DLL
estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si: `this` (o `self`) desalineado, un angulo fuera de rango (mas de 100 radianes),
los bytes altos de la bandera del barrido, o el byte marcador 0xA9B0B0 en 0x5A (las funciones vacias).
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
UT = '(reinterpret_cast<uintptr_t>(this) & 3)'
US = '(reinterpret_cast<uintptr_t>(self) & 3)'
SDD_SELF = '    if (flags & 1) {' + NL + '        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;' + NL + '}'


def sdd_self1():
    return (SDD_SELF, '    if ((flags & 1) && !' + US + ') {' + NL + '        CTask::operator delete(self);' + NL + '    }' + NL + '    return self;' + NL + '}')


def sdd_self2():
    return (SDD_SELF, '    if (flags & 1) {' + NL + '        CTask::operator delete(self);' + NL + '    }' + NL + '    return ' + US + ' ? nullptr : self;' + NL + '}')


F = {'um': T + 'TaskComplexUseMobilePhone.cpp', 'ug': T + 'TaskComplexUseGoggles.cpp', 'iw': T + 'TaskComplexInWater.cpp', 'seq': G + 'Tasks/TaskComplexUseSequence.cpp', 'car': G + 'CarCtrl.cpp',
     'vec': G + 'Core/Vector.cpp', 'pf': G + 'PathFind.cpp', 'pool': G + 'Pools.cpp'}
SOURCES = sorted(set(F.values()))
NOP_INSTALL = [('PreparePathData', 0x44D0E0, 'Wv207Nop0'), ('AllocatePathFindInfoMem', 0x44D2B0, 'Wv207Nop0b'), ('StoreNodeInfoCar', 0x44D2C0, 'Wv207Nop17'), ('StoreNodeInfoPed', 0x44D2D0, 'Wv207Nop12'),
               ('StoreDetachedNodeInfoPed', 0x44D2E0, 'Wv207Nop14'), ('StoreDetachedNodeInfoCar', 0x44D2F0, 'Wv207Nop17b')]


def nop_defs(marker):
    """funciones globales que sustituyen a las vacias (misma limpieza de pila: __fastcall con N dwords de pila); escriben un valor en el byte marcador 0xA9B0B0 si vale 0x5A (el banco lo pone; el juego real no)"""
    def args(n):
        return ', '.join('int' for _ in range(n))
    body = 'if (*reinterpret_cast<volatile uint8*>(0xA9B0B0) == 0x5A) *reinterpret_cast<volatile uint8*>(0xA9B0B0) = %s;' % marker
    return ('namespace {' + NL + 'void __fastcall Wv207Nop0(void*, void*) { ' + body + ' }' + NL + 'void __fastcall Wv207Nop0b(void*, void*) { ' + body + ' }' + NL
            + 'void __fastcall Wv207Nop17(void*, void*, ' + args(17) + ') { ' + body + ' }' + NL + 'void __fastcall Wv207Nop12(void*, void*, ' + args(12) + ') { ' + body + ' }' + NL
            + 'void __fastcall Wv207Nop14(void*, void*, ' + args(14) + ') { ' + body + ' }' + NL + 'void __fastcall Wv207Nop17b(void*, void*, ' + args(17) + ') { ' + body + ' }' + NL + '} // namespace' + NL + NL
            + 'void CPathFind::InjectHooks() {')


def nops(marker):
    edits = [(F['pf'], 'void CPathFind::InjectHooks() {', nop_defs(marker))]
    for name, addr, fn in NOP_INSTALL:
        edits.append((F['pf'], '    RH_ScopedInstall(%s, 0x%X);' % (name, addr), '    RH_ScopedNamedGlobalInstall(%s, "%s", 0x%X);' % (fn, name, addr)))
    return edits


def round1():
    """una lista de ediciones (archivo, texto original, texto mutado) por funcion (las seis vacias, 11 a 16, comparten una lista)"""
    o = []
    o.append([(F['um'], '    flagsB = newFlagsB;', '    flagsB = newFlagsB | (' + UT + ' ? 4u : 0u);')])                                                          # 0
    o.append([(F['um'], '    CTask* result = m_pSubTask;', '    CTask* result = ' + UT + ' ? nullptr : m_pSubTask;')])                                              # 1
    o.append([(F['um'], '    copy->m_nDuration = duration;', '    copy->m_nDuration = duration + (' + UT + ' ? 1 : 0);')])                                        # 2
    o.append([(F['um'],) + sdd_self1()])                                                                                                                      # 3
    o.append([(F['ug'],) + sdd_self1()])                                                                                                                      # 4
    o.append([(F['iw'],) + sdd_self1()])                                                                                                                      # 5
    o.append([(F['seq'], '    *reinterpret_cast<uint32*>(self) = 0x86E518;', '    *reinterpret_cast<uint32*>(self) = 0x86E518 + (' + US + ' ? 4 : 0);')])                                  # 6
    o.append([(F['car'], '    if ((flagArgument & 0xFFu) != 0) {', '    if (((flagArgument & 0xFFu) != 0) != (((flagArgument >> 8) & 1u) != 0)) {')])                                     # 7
    o.append([(F['car'], '    *outZ = colPoint.m_vecPoint.z;', '    *outZ = colPoint.m_vecPoint.z + (elevation > 100.0f ? 1.0f : 0.0f);')])                                        # 8
    o.append([(F['car'], '            return std::bit_cast<float>(0x47C35000u); // 100000.0f (0x858EF8)', '            return std::bit_cast<float>(0x47C35000u) + (heading > 1000.0f ? 1.0f : 0.0f); // 100000.0f (0x858EF8)')])   # 9
    o.append([(F['pool'], '    const int32     diff    = static_cast<int32>(reinterpret_cast<uintptr_t>(vehicle) - storage);', '    const int32     diff    = static_cast<int32>(reinterpret_cast<uintptr_t>(vehicle) - storage) + (' + UT + ' ? 1 : 0);')])   # 10
    o.append(nops('0x5B'))                                                                                                                                    # 11..16
    o.append([(F['vec'], '    const double reciprocal = 1.0 / sqrt(sumSquares);', '    const double reciprocal = (1.0 / sqrt(sumSquares)) * (' + US + ' ? 2.0 : 1.0);')])                           # 17
    o.append([(F['pf'], 'maxDist, (uint16)bLowTraffic, (int32)bUnkn, 1,', 'maxDist, (uint16)bLowTraffic, (int32)bUnkn + (' + UT + ' ? 1 : 0), 1,')])                                                    # 18
    o.append([(F['pf'], '                outAddresses[found] = CNodeAddress{ (uint16)areaId, (uint16)i };', '                outAddresses[found] = CNodeAddress{ (uint16)areaId, (uint16)(i + (' + UT + ' ? 1 : 0)) };')])   # 19
    o.append([(F['pf'], '    *outFirst  = bestFirst;', '    *outFirst  = ' + UT + ' ? bestSecond : bestFirst;')])                                                             # 20
    o.append([(F['pf'], '    *outAddress1 = firstNode;', '    *outAddress1 = ' + UT + ' ? CNodeAddress{ firstNode.m_wAreaId, 0 } : firstNode;')])                                 # 21
    return o


def round2():
    o = []
    o.append([(F['um'], '    const uint32 newFlagsA = flagsA | 0x400000u;', '    const uint32 newFlagsA = flagsA | (0x400000u << (' + UT + ' ? 1 : 0));')])                                    # 0
    o.append([(F['um'], '        playerData->m_bPlayerSprintDisabled = true;', '        playerData->m_bPlayerSprintDisabled = !' + UT + ';')])                                                  # 1
    o.append([(F['um'], '    *reinterpret_cast<uint32*>(copy) = 0x86E454;', '    *reinterpret_cast<uint32*>(copy) = 0x86E454 + (' + UT + ' ? 4 : 0);')])                                       # 2
    o.append([(F['um'],) + sdd_self2()])                                                                                                                      # 3
    o.append([(F['ug'],) + sdd_self2()])                                                                                                                      # 4
    o.append([(F['iw'],) + sdd_self2()])                                                                                                                      # 5
    o.append([(F['seq'], '        const bool finalReference = sequence->m_RefCnt == 1;', '        const bool finalReference = sequence->m_RefCnt == (' + US + ' ? 2 : 1);')])                                                  # 6
    o.append([(F['car'], '    float vx = velocity[0];', '    float vx = velocity[0] + (((flagArgument >> 8) & 1u) ? 1.0f : 0.0f);')])                                     # 7
    o.append([(F['car'], '    *outZ = colPoint.m_vecPoint.z;' + NL + '    return true;', '    *outZ = colPoint.m_vecPoint.z;' + NL + '    return !(elevation > 100.0f);')])   # 8
    o.append([(F['car'], '    float maximum = 0.0f;', '    float maximum = (heading > 1000.0f) ? 1.0f : 0.0f;')])                                                      # 9
    o.append([(F['pool'], '(static_cast<int64>(0x6572EC3) * diff) >> 32);', '((static_cast<int64>(0x6572EC3) * diff) >> 32) + (' + UT + ' ? 1 : 0));')])   # 10
    o.append(nops('0x5C'))                                                                                                                                    # 11..16
    o.append([(F['vec'], '        self->x = 1.0F;', '        self->x = 1.0F + ((reinterpret_cast<uintptr_t>(self) & 3) ? 1.0F : 0.0F);')])                                  # 17
    o.append([(F['pf'], '    CNodeAddress* cursor = outAddresses;', '    CNodeAddress* cursor = outAddresses + (' + UT + ' ? 1 : 0);')])                                                 # 18
    o.append([(F['pf'], '    const float radiusSquared = Wv207SquareCpp(radius);', '    const float radiusSquared = Wv207SquareCpp(radius) * (' + UT + ' ? 4.0f : 1.0f);')])                           # 19
    o.append([(F['pf'], '    *outSecond = bestSecond;', '    *outSecond = ' + UT + ' ? bestFirst : bestSecond;')])                                                         # 20
    o.append([(F['pf'], '            *outAddress2 = closest[i];', '            *outAddress2 = ' + UT + ' ? firstNode : closest[i];')])                                              # 21
    return o


ROUNDS = {1: round1, 2: round2}
NFUNCS = 22


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def flat(n):
    return [e for group in ROUNDS[n]() for e in group]


def check():
    bad = 0
    for n in (1, 2):
        assert len(ROUNDS[n]()) == NFUNCS - 5      # 17 grupos: las seis funciones vacias comparten una (11 a 16)
        texts = {}
        for rel, old, new in flat(n):
            text = texts.get(rel)
            if text is None:
                text = (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n')
            c = text.count(old)
            if c != 1 or old == new:
                bad += 1
                print('ronda', n, rel, 'apariciones', c, repr(old[:80]))
            texts[rel] = text.replace(old, new)
    print('mutaciones con problemas:', bad)
    return bad


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
    muts = flat(n)
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
    print('ronda', n, 'aplicada:', len(muts), 'ediciones para', NFUNCS, 'funciones')


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
        sys.exit(1 if check() else 0)
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
