"""Prueba negativa del banco FnF1A (lote F1A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Una mutacion por funcion y ronda (10 por ronda): cambia lo que el banco observa. Los hooks de la DLL
estan activos en el juego real antes de que empiece el banco: las mutaciones de constructores, destructores y descompresoras se condicionan a un puntero desalineado (`this` o el destino, que el juego real nunca
desalinea y el banco si en una de cada cuatro familias) o a un NaN; BugstarWriteScreenShotEntry no tiene llamadores en el EXE ni en la DLL, asi que se muta sin condicion.
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
UNALT = '(reinterpret_cast<uintptr_t>(this) & 3)'
UNALD = '(reinterpret_cast<uintptr_t>(dst) & 3)'
IK, WC, WAT, AZ, DXT = G + 'Ragdoll/IKChainManager.cpp', G + 'WaterCreatureManager_c.cpp', G + 'WaterLevel.cpp', G + 'Audio/AudioZones.cpp', G + 'RenderWare/D3D9RasterFormat.cpp'
SOURCES = sorted({IK, WC, WAT, AZ, DXT})
W32 = 'reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(this) + '


def round1():
    out = []
    out.append((IK, '    m_ActiveList.RemoveAll(); // cabeza, cola, contador = 0', '    m_ActiveList.RemoveAll(); // cabeza, cola, contador = 0' + NL + '    if (' + UNALT + ') *' + W32 + '0xB00) = 1;'))                          # 0
    out.append((IK, 'void IKChainManager_c::Destructor() {', 'void IKChainManager_c::Destructor() {' + NL + '    if (' + UNALT + ') *' + W32 + '0x10) = 0x5A5A5A5Au;'))                  # 1
    out.append((WC, '    m_waterCreaturePool.RemoveAll(); // cabeza, cola, contador = 0', '    m_waterCreaturePool.RemoveAll(); // cabeza, cola, contador = 0' + NL + '    if (' + UNALT + ') *' + W32 + '0x1A00) = 1;'))   # 2
    out.append((WC, 'void WaterCreatureManager_c::Destructor() {', 'void WaterCreatureManager_c::Destructor() {' + NL + '    if (' + UNALT + ') *' + W32 + '0x10) = 0x5A5A5A5Au;'))                  # 3
    out.append((WAT, '    if (!(nearPlane - radius < inCamera.z)) {', '    if (radius != radius) {' + NL + '        return true;' + NL + '    }' + NL + '    if (!(nearPlane - radius < inCamera.z)) {'))                                 # 4
    out.append((AZ, '    appendTagged(0xA, "SA");', '    appendTagged(0xA, "SB");'))                                                                                                                            # 5
    out.append((DXT, '    return anyTransparent;', '    return anyTransparent ^ (' + UNALD.replace('dst', 'dst') + ' ? 1 : 0);'))                                                                              # 6
    out.append((DXT, '                    const uint32 alpha  = nibble | (nibble << 4);', '                    const uint32 alpha  = (nibble | (nibble << 4)) ^ (' + UNALD + ' ? 1u : 0u);'))                                          # 7
    out.append((DXT, '                DecodeDxt5AlphaBlock(blockDst, block, width, 0x00FFFFFFu);', '                DecodeDxt5AlphaBlock(blockDst, block, width, ' + UNALD + ' ? 0x00FFFFFEu : 0x00FFFFFFu);'))                      # 8
    out.append((DXT, '            out[px] = (out[px] & keepMask) | (static_cast<uint32>(alphas[row * 4 + px]) << 24);', '            out[px] = (out[px] & keepMask) | ((static_cast<uint32>(alphas[row * 4 + px]) ^ (' + UNALD + ' ? 1u : 0u)) << 24);'))   # 9
    return out


def round2():
    out = []
    out.append((IK, '    return this;', '    return ' + UNALT + ' ? nullptr : this;'))                                                                                                                      # 0
    out.append((IK, 'void IKChainManager_c::Destructor() {', 'void IKChainManager_c::Destructor() {' + NL + '    if (' + UNALT + ') *' + W32 + '0xB0C) = 0xA5A5A5A5u;'))                                           # 1
    out.append((WC, '    return this;', '    return ' + UNALT + ' ? nullptr : this;'))                                                                                                                      # 2
    out.append((WC, 'void WaterCreatureManager_c::Destructor() {', 'void WaterCreatureManager_c::Destructor() {' + NL + '    if (' + UNALT + ') *' + W32 + '0x1A0C) = 0xA5A5A5A5u;'))                                  # 3
    out.append((WAT, '    if (!(nearPlane - radius < inCamera.z)) {', '    if (!(nearPlane - radius <= inCamera.z)) {'))                                                                                                      # 4 (sin llamadores: sin condicion)
    out.append((AZ, '    uint32 zero = 0;', '    uint32 zero = 1;'))                                                                                                                                         # 5
    out.append((DXT, '                    anyTransparent |= 1;', '                    anyTransparent |= ' + UNALD + ' ? 2 : 1;'))                                                                             # 6
    out.append((DXT, '                    std::memcpy(&word, block + row * 2, sizeof(word));', '                    std::memcpy(&word, block + row * 2, sizeof(word));' + NL + '                    if (' + UNALD + ') word ^= 1;'))     # 7
    out.append((DXT, '                DecodeDxt5AlphaBlock(blockDst, block, width, 0x00FFFFFFu);', '                DecodeDxt5AlphaBlock(blockDst, block + (' + UNALD + ' ? 1 : 0), width, 0x00FFFFFFu);'))                          # 8
    out.append((DXT, '    table[7] = (a0 + a1 * 6) / 7;', '    table[7] = (a0 + a1 * 6) / 7 + (' + UNALD + ' ? 1 : 0);'))                                                                                                  # 9
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
