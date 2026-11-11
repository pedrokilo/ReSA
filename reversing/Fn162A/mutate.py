"""Prueba negativa del banco Fn162A (lote 162A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (7 por ronda): cambia lo que el banco observa
(valor devuelto, bytes cambiados, eventos de los falsos). Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a banderas con bits que el juego
no produce (el juego borra con 0 o 1), a `this` desalineado, o, en las funciones sin entradas imposibles (el asignador por tipo, el registrador unico), a que el banco ya este corriendo
(F162ALive::gBenchThread != 0), para que la partida no se caiga antes del banco.
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
NL = chr(10)
MAT, V2 = G + 'Core/Matrix.cpp', G + 'Core/Vector2D.cpp'
SOURCES = sorted({MAT, V2})
BENCH = 'F162ALive::gBenchThread != 0'
# Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: las mutaciones se condicionan a que el banco ya este corriendo (gBenchThread, que fija el propio banco antes del primer caso)
# o a entradas que el juego no produce (puntero desalineado), para que la partida no se caiga antes del banco.
DECL = 'namespace F162ALive { extern unsigned long gBenchThread; }' + NL
UNAL_THIS = '(reinterpret_cast<uintptr_t>(this) & 3)'
UNAL_SELF = '(reinterpret_cast<uintptr_t>(self) & 3)'

DET_OLD = '    if (m_pAttachMatrix && *reinterpret_cast<const uint32*>(&m_bOwnsAttachedMatrix))'
V2_OLD = '    RH_ScopedInstall(operator=, 0x43E110);' + NL + '}'
V2_DEF = ('namespace {' + NL + 'CVector2D* __fastcall V2Mutated(CVector2D* self, void*, const CVector2D* r) {' + NL + '    self->x = r->x;' + NL + '    self->y = r->y;' + NL + '    %s' + NL + '    return self;' + NL + '}' + NL + '}' + NL)


def v2(cond_stmt):
    """mutacion de CVector2D::operator=: la entrada del hook pasa a una funcion propia (con la sentencia mutada); el original sigue siendo el EXE"""
    return [(V2, '#include "General.h"' + NL, '#include "General.h"' + NL + DECL),
            (V2, 'void CVector2D::InjectHooksLote162A() {', V2_DEF % cond_stmt + 'void CVector2D::InjectHooksLote162A() {'),
            (V2, '    RH_ScopedInstall(operator=, 0x43E110);' + NL + '}', '    RH_ScopedNamedGlobalInstall(V2Mutated, "operator=", 0x43E110);' + NL + '}')]


def round1():
    out = [(MAT, '#include "X87Math.h"' + NL, '#include "X87Math.h"' + NL + DECL)]
    out.append((MAT, '    MatrixCopyFromRw(reinterpret_cast<uint32*>(this), reinterpret_cast<const uint32*>(m_pAttachMatrix));',
                '    MatrixCopyFromRw(reinterpret_cast<uint32*>(this), reinterpret_cast<const uint32*>(m_pAttachMatrix));' + NL + '    if (' + BENCH + ') reinterpret_cast<uint32*>(this)[14] ^= 1u;'))              # 0
    out.append((MAT, '    MatrixCopyFromRw(reinterpret_cast<uint32*>(this), reinterpret_cast<const uint32*>(rwMatrix));',
                '    MatrixCopyFromRw(reinterpret_cast<uint32*>(this), reinterpret_cast<const uint32*>(rwMatrix));' + NL + '    if (' + BENCH + ') reinterpret_cast<uint32*>(this)[0] ^= 1u;'))                # 1
    out.append((MAT, DET_OLD, '    if (m_pAttachMatrix && m_bOwnsAttachedMatrix)'))                                                                                                         # 2 (el defecto original: bandera leida como byte)
    out.append((MAT, '    memcpy(this, &matrix, sizeof(RwMatrix));', '    memcpy(this, &matrix, sizeof(RwMatrix) - (' + BENCH + ' ? 4 : 0));'))                                       # 3
    out.append((MAT, '    m_pos += rvalue.m_pos;' + NL + '}', '    m_pos += rvalue.m_pos;' + NL + '    if (' + BENCH + ') m_pos.x = 1.0f;' + NL + '}'))                                      # 4
    out.append((MAT, '    ScaleXYZ(scale, scale, scale);', '    ScaleXYZ(scale, scale, ' + BENCH + ' ? scale + 1.0f : scale);'))                                                         # 5
    out += v2('if (' + UNAL_SELF + ') self->y = r->y + 1.0f;')                                                                                                                       # 6
    return out


def round2():
    out = [(MAT, '#include "X87Math.h"' + NL, '#include "X87Math.h"' + NL + DECL)]
    out.append((MAT, '    MatrixCopyFromRw(reinterpret_cast<uint32*>(this), reinterpret_cast<const uint32*>(m_pAttachMatrix));',
                '    if (!' + UNAL_THIS + ') MatrixCopyFromRw(reinterpret_cast<uint32*>(this), reinterpret_cast<const uint32*>(m_pAttachMatrix));'))                                # 0
    out.append((MAT, '    MatrixCopyFromRw(reinterpret_cast<uint32*>(this), reinterpret_cast<const uint32*>(rwMatrix));',
                '    if (!(reinterpret_cast<uintptr_t>(rwMatrix) & 3)) MatrixCopyFromRw(reinterpret_cast<uint32*>(this), reinterpret_cast<const uint32*>(rwMatrix));'))      # 1
    out.append((MAT, '        RwMatrixDestroy(m_pAttachMatrix);' + NL + NL + '    m_pAttachMatrix = nullptr;' + NL + '}',
                '        RwMatrixDestroy(m_pAttachMatrix);' + NL + NL + '    if (' + BENCH + ' && (m_pAttachMatrix == nullptr || !m_bOwnsAttachedMatrix)) return;' + NL + '    m_pAttachMatrix = nullptr;' + NL + '}'))   # 2
    out.append((MAT, '    memcpy(this, &matrix, sizeof(RwMatrix));', '    memcpy(this, &matrix, sizeof(RwMatrix));' + NL + '    if (' + BENCH + ') reinterpret_cast<uint32*>(this)[15] ^= 4u;'))                       # 3
    out.append((MAT, '    m_up += rvalue.m_up;', '    m_up += rvalue.m_up;' + NL + '    if (' + BENCH + ') m_up.z = -m_up.z;'))                                                                  # 4
    out.append((MAT, '    ScaleXYZ(scale, scale, scale);', '    ScaleXYZ(scale, scale, scale);' + NL + '    if (' + BENCH + ') m_forward.y = 0.0f;'))                                                    # 5
    out += v2('if (reinterpret_cast<uintptr_t>(r) & 3) self->x = 0.0f;')                                                                                                             # 6
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
