"""Prueba negativa del banco Fn212A (lote 212A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion.

    python -I mutate.py apply 1        guarda copias buenas en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los falsos, memoria global).
Los hooks de la DLL estan activos en el juego real antes de que empiece el banco: cada mutacion se condiciona a algo que el juego real no produce y el banco si: `this` o un puntero de cabecera desalineado, un bit alto
puesto en las banderas, la marca 0x5A en 0xA9B0B0 (solo la pone el banco). Las plantillas compartidas (cuerpo comun de los 13 constructores de CPool, plantilla del cuerpo de destructor, plantilla del destructor con
borrado) fallan en todas sus direcciones; la ronda 2 muta ademas cada constructor por separado (13 mutaciones propias).
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
UNAL = '(reinterpret_cast<uintptr_t>(self) & 3)'
POO, ENV, PCH = G + 'Pools.cpp', G + 'Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.cpp', G + 'Pipelines/PipelinesCommon.hpp'
SOURCES = sorted({POO, ENV, PCH})
FLAG = '*reinterpret_cast<volatile uint8*>(0xA9B0B0)'
CTORS = ['PtrNodeSingleLink', 'PtrNodeDoubleLink', 'EntryInfoNode', 'Ped', 'Vehicle', 'Building', 'Object', 'Dummy', 'ColModel', 'Task', 'Event', 'PointRoute', 'PatrolRoute']


def pools_text():
    good = GOOD / POO.replace('/', '__')
    p = good if (GOOD / 'state.json').exists() else ROOT / POO   # con una ronda aplicada, el texto de referencia es la copia buena
    return p.read_bytes().decode('utf-8').replace(chr(13) + chr(10), chr(10))


def dtor_template():
    t = pools_text()
    i = t.index('template<int Id>' + NL + 'void __fastcall PoolDestructorBodyAt(void* poolPtr, void* /*edx*/) {')
    j = t.index(NL + '}' + NL, i) + 2
    return t[i:j]


def ctor_mutations():
    import re
    t = pools_text()
    out = []
    for n in CTORS:
        m = re.search(r'(void\* __fastcall ' + n + r'PoolConstructorExe\(void\* self, void\* /\*edx\*/, int32 capacity, const char\* /\*name\*/\) \{ // 0x[0-9A-F]+' + NL + r'    ConstructPoolBody\(static_cast<uint8\*>\(self\), capacity, )(0x[0-9a-f]+)(\);)', t)
        old = m.group(0)
        new = m.group(1) + '(' + UNAL + ' ? ' + m.group(2) + ' + 1 : ' + m.group(2) + ')' + m.group(3)
        out.append((POO, old, new))
    return out


def round1():
    out = []
    # 0, 1 destructores con borrado (plantilla compartida): sin operator delete del propio pool con `this` desalineado
    out.append((ENV, '    EnvMapPoolDestroy(self);' + NL + '    if (flags & 1) {' + NL + '        ::operator delete(self);', '    EnvMapPoolDestroy(self);' + NL + '    if ((flags & 1) && !' + UNAL + ') {' + NL + '        ::operator delete(self);'))
    # 2 dibujo: el sombreador de pixeles con cabecera desalineada
    out.append((PCH, '    _rwD3D9SetPixelShader(NULL);', '    _rwD3D9SetPixelShader((reinterpret_cast<uintptr_t>(header) & 3) ? reinterpret_cast<void*>(1) : NULL);'))
    # 3 iluminacion: textura nula con el bit 30 de las banderas
    out.append((PCH, ' != 0, tex);' + NL + '    RxD3D9InstanceDataRender(header, mesh);', ' != 0, (rxGeoFlags & 0x40000000u) ? nullptr : tex);' + NL + '    RxD3D9InstanceDataRender(header, mesh);'))
    # 4 SetEnvMapMaterial: potencia a cero con el bit 31 de las banderas
    out.append((ENV, '    m.Power = power;', '    m.Power = (flags & 0x80000000u) ? 0.0f : power;'))
    # 5 SetDefaultSurfaceProperties: banderas distintas con la marca del banco
    out.append((ENV, '    RwD3D9SetSurfaceProperties(&gDefaultSurfaceProps, &gDefaultColor, rxGEOMETRY_LIGHT);', '    RwD3D9SetSurfaceProperties(&gDefaultSurfaceProps, &gDefaultColor, ' + FLAG + ' == 0x5A ? rxGEOMETRY_LIGHT + 1 : rxGEOMETRY_LIGHT);'))
    # 6..18 constructores (cuerpo comun ConstructPoolBody): propietario 2 con `pool` desalineado
    out.append((POO, '    *(pool + 0x10)                         = 1;', '    *(pool + 0x10)                         = (reinterpret_cast<uintptr_t>(pool) & 3) ? 2 : 1;'))
    # 19..31 cuerpos de destructor (plantilla compartida): sin liberar con `pool` desalineado
    t = dtor_template()
    out.append((POO, t, t.replace('if (*reinterpret_cast<int32*>(pool + 0x8) > 0) {', 'if (*reinterpret_cast<int32*>(pool + 0x8) > 0 && !(reinterpret_cast<uintptr_t>(pool) & 3)) {', 1)))
    return out


def round2():
    out = []
    out.append((ENV, '        ::operator delete(self);' + NL + '    }' + NL + '    return self;', '        ::operator delete(self);' + NL + '    }' + NL + '    return ' + UNAL + ' ? nullptr : self;'))   # 0, 1
    out.append((PCH, '    if (header->indexBuffer) {', '    if (header->indexBuffer && !(reinterpret_cast<uintptr_t>(header) & 3)) {'))                                                   # 2
    out.append((PCH, '(rxGeoFlags & (rxGEOMETRY_TEXTURED2 | rxGEOMETRY_TEXTURED)) != 0, tex);', '((rxGeoFlags & (rxGEOMETRY_TEXTURED2 | rxGEOMETRY_TEXTURED)) != 0 && !(rxGeoFlags & 0x80000000u)), tex);'))   # 3
    out.append((ENV, '    RwD3D9SetMaterial(&m);' + NL + '    return TRUE;', '    if (!(flags & 0x40000000u)) RwD3D9SetMaterial(&m);' + NL + '    return TRUE;'))                      # 4
    out.append((ENV, '    RwD3D9SetSurfaceProperties(&gDefaultSurfaceProps, &gDefaultColor, rxGEOMETRY_LIGHT);', '    if (' + FLAG + ' != 0x5A) RwD3D9SetSurfaceProperties(&gDefaultSurfaceProps, &gDefaultColor, rxGEOMETRY_LIGHT);'))   # 5
    out += ctor_mutations()                                                                                                           # 6..18
    t = dtor_template()
    out.append((POO, t, t.replace('*reinterpret_cast<int32*>(pool + 0xC) = 0;', '*reinterpret_cast<int32*>(pool + 0xC) = (reinterpret_cast<uintptr_t>(pool) & 3) ? 1 : 0;', 1)))   # 19..31
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
