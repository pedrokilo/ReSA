"""Prueba negativa del banco Fn123A (lote 123A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por ronda (la funcion es una sola).
"""
import hashlib
import json
import shutil
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
GOOD = Path(__file__).resolve().parent / 'artifacts' / 'good'
PART = 'source/game_sa/Tasks/TaskTypes/TaskComplexPartner.cpp'
SOURCES = [PART]
VT = '    *reinterpret_cast<uint32*>(self) = 0x870664;\n    ReleasePartnerResources(*self);'
ROUNDS = {
    # 1: vtable + 1
    1: [(PART, '    *reinterpret_cast<uint32*>(self) = 0x870664;', '    *reinterpret_cast<uint32*>(self) = 0x870665;')],
    # 2: la bandera se prueba solo en su bit 0 (el EXE prueba el byte entero)
    2: [(PART, '    if (PartnerByteOf(task.m_requiredAnimsStreamedIn)) {', '    if (PartnerByteOf(task.m_requiredAnimsStreamedIn) & 1) {')],
    # 3: el primer byte del nombre se prueba con signo (el EXE compara el byte: 0x80..0xFF son distintos de cero)
    3: [(PART, "        if (task.m_animBlockName[0] != '"+chr(92)+"0') {", '        if (static_cast<int8>(task.m_animBlockName[0]) > 0) {')],
    # 4: la vtable se escribe despues de liberar el companero (el colaborador ve la vtable previa)
    4: [(PART, VT, '    ReleasePartnerResources(*self);\n    *reinterpret_cast<uint32*>(self) = 0x870664;')],
    # 5: la bandera no se pone a cero
    5: [(PART, '        task.m_requiredAnimsStreamedIn = false;\n', '')],
    # 6: no se libera el companero cuando vale 1
    6: [(PART, '    CEntity::SafeCleanUpRef(task.m_partner);\n\n    if (PartnerByteOf', '    if (reinterpret_cast<uintptr_t>(task.m_partner) != 1) CEntity::SafeCleanUpRef(task.m_partner);\n\n    if (PartnerByteOf')],
    # 7: se llama al destructor base antes de liberar (orden invertido de la cola)
    7: [(PART, '    ReleasePartnerResources(*self);\n    CTaskComplexBody::Destructor(self, nullptr);', '    CTaskComplexBody::Destructor(self, nullptr);\n    ReleasePartnerResources(*self);')],
}


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
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:60], norm.count(old)))
        norm = norm.replace(old, new)
        p.write_bytes((norm.replace('\n', '\r\n') if crlf else norm).encode('utf-8'))
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
