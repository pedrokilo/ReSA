"""Prueba negativa del banco Fn167A (lote 167A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (7 por ronda): cambia lo que el banco observa
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

CLUMP, WEAPON, AES = G + 'Models/ClumpModelInfo.cpp', G + 'Models/WeaponModelInfo.cpp', G + 'Audio/Loaders/AEDataStream.cpp'
SWIM, ARREST = T + 'TaskSimpleSwim.cpp', T + 'TaskSimpleArrestPed.cpp'
SOURCES = sorted({CLUMP, WEAPON, AES, SWIM, ARREST})


def cfun(cls, name, rtype, ret_old, ret_new, args='', const=' const', f=None):
    old = rtype + ' ' + cls + '::' + name + '(' + args + ')' + const + ' {' + NL + '    return ' + ret_old + ';'
    new = rtype + ' ' + cls + '::' + name + '(' + args + ')' + const + ' {' + NL + '    return ' + ret_new + ';'
    return (f, old, new)


def round1():
    out = []
    out.append((CLUMP, '    *reinterpret_cast<uint32*>(self) = 0x85BB9C;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x85BBA0 : 0x85BB9C;'))                                   # 0
    out.append((WEAPON, '    *reinterpret_cast<uint32*>(self) = 0x85BD78;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x85BD7C : 0x85BD78;'))                                   # 1
    out.append((AES, '    return CloseHandle(handle);', '    return CloseHandle(handle) + (' + UNALT + ' ? 1 : 0);'))                                                                         # 2
    out.append((AES, '    *reinterpret_cast<uint32*>(self) = 0x85F0BC;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x85F0C0 : 0x85F0BC;'))                                   # 3
    out.append(cfun('CTaskSimpleSwim', 'GetTaskType', 'eTaskType', 'Type', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type', f=SWIM))                     # 4
    out.append(cfun('CTaskSimpleArrestPed', 'GetTaskType', 'eTaskType', 'Type', UNALT + ' ? static_cast<eTaskType>(Type + 1) : Type', f=ARREST))             # 5
    out.append((ARREST, '    *reinterpret_cast<uint32*>(self) = 0x870984;', '    *reinterpret_cast<uint32*>(self) = ' + UNAL + ' ? 0x870988 : 0x870984;'))   # 6
    return out


def round2():
    out = []
    out.append((CLUMP, '    *reinterpret_cast<uint32*>(self) = 0x85BB9C;', '    *reinterpret_cast<uint32*>(self) = (' + UNAL + ' == 2) ? 0x85BB9D : 0x85BB9C;'))                           # 0
    out.append((WEAPON, '    CBaseModelInfoBody::Constructor(self, nullptr);', '    if (!' + UNAL + ') {' + NL + '        CBaseModelInfoBody::Constructor(self, nullptr);' + NL + '    }'))   # 1
    out.append((AES, '    const HANDLE handle = m_pFileHandle;', '    const HANDLE handle = (' + UNALT + ' == 2) ? nullptr : m_pFileHandle;'))                                       # 2
    out.append((AES, '        self->m_pszFilename = nullptr;', '        if (!' + UNAL + ') {' + NL + '            self->m_pszFilename = nullptr;' + NL + '        }'))        # 3
    out.append(cfun('CTaskSimpleSwim', 'GetTaskType', 'eTaskType', 'Type', '(' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type', f=SWIM))     # 4
    out.append(cfun('CTaskSimpleArrestPed', 'GetTaskType', 'eTaskType', 'Type', '(' + UNALT + ' == 2) ? static_cast<eTaskType>(Type ^ 0x100) : Type', f=ARREST))   # 5
    out.append((ARREST, '    self->CTask::Destructor();', '    if (!' + UNAL + ') {' + NL + '        self->CTask::Destructor();' + NL + '    }'))                 # 6
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
