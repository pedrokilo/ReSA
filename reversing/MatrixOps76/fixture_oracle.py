"""Oraculo independiente (Python) de MatrixOps76: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse de
CMatrix::Attach (0x59BD10..0x59BD80) y CMatrix::operator= (0x59BBC0..0x59BBE6 + 0x59AD70) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion.

Desensamblado usado:
  Attach 0x59BD10: eax=[this+0x40]; si eax != 0 y el DWORD [this+0x44] != 0 -> RwMatrixDestroy(eax) (0x7F2A20, cdecl); [this+0x40]=matriz; [this+0x44]=movzx(byte del argumento);
     copia los dwords +0x00,+0x04,+0x08,+0x10,+0x14,+0x18,+0x20,+0x24,+0x28,+0x30,+0x34,+0x38 de la RwMatrix a this (las banderas +0x0C y el relleno +0x1C/+0x2C/+0x3C no se copian).
  operator= 0x59BBC0: eax=[this+0x40] (ANTES de la copia); rep movsd de 16 dwords (0x40 bytes) del origen a this; si eax != 0 -> 0x59AD70(eax): copia los mismos 12 dwords de this a la RwMatrix
     y llama a RwMatrixUpdate (0x7F18A0, cdecl, con la RwMatrix); devuelve this en EAX (la DLL es void: EAX no se compara).
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 this, 1 other, 2 rwNew, 3 rwOld. Los bytes crudos de +0x40..0x43 de `this`
son direcciones de ejecucion: se ignoran y el puntero se comprueba por la etiqueta `attach_ptr`.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

GUARD = 0x40
COPIED = (0x00, 0x04, 0x08, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28, 0x30, 0x34, 0x38)
EV_DESTROY, EV_UPDATE = 1, 2
THIS, OTHER, RWNEW, RWOLD = range(4)


def tag(idx, off=0):
    return 0x10000000 + (idx << 20) + GUARD + off


def diff_bytes(old, new, skip=()):
    """lista de [desplazamiento, byte nuevo] entre dos listas de dwords"""
    out = []
    for i, (a, b) in enumerate(zip(old, new)):
        for k in range(4):
            if ((a >> (8 * k)) & 0xFF) != ((b >> (8 * k)) & 0xFF) and 4 * i + k not in skip:
                out.append([4 * i + k, (b >> (8 * k)) & 0xFF])
    return out


def model_attach(inp):
    mem = [list(inp['this_w']), list(inp['other_w']), list(inp['rw_new_w']), list(inp['rw_old_w'])]
    new_idx = {0: RWNEW, 1: THIS, 2: RWOLD}[inp['new_kind']]
    new_tag = tag(new_idx)
    old_tag = {0: 0, 1: tag(RWOLD), 2: inp['old_junk'], 3: new_tag, 4: tag(THIS)}[inp['old_kind']]
    owns = inp['owns'] & 0xFFFFFFFF
    ev = []
    if old_tag != 0 and owns != 0:
        ev.append([EV_DESTROY, old_tag])
    src = list(mem[new_idx])               # la fuente se lee despues de escribir +0x40/+0x44, pero ninguno de los dwords copiados es ese
    final = list(mem[THIS])
    final[17] = inp['bool_arg'] & 0xFF
    for off in COPIED:
        final[off // 4] = src[off // 4]
    return ev, new_tag, final[17], [final, mem[OTHER], mem[RWNEW], mem[RWOLD]], owns


def model_assign(inp):
    mem = [list(inp['this_w']), list(inp['other_w']), list(inp['rw_new_w']), list(inp['rw_old_w'])]
    dst_idx = {0: None, 1: RWOLD, 2: OTHER, 3: THIS, 4: RWNEW}[inp['dst_kind']]
    src = list(mem[THIS]) if inp['src_kind'] else list(mem[OTHER])
    final = list(mem[THIS])
    for i in range(16):
        final[i] = src[i]
    ev = []
    arenas = [final, mem[OTHER], mem[RWNEW], mem[RWOLD]]
    if dst_idx is not None:
        dst = arenas[dst_idx]
        for off in COPIED:
            dst[off // 4] = final[off // 4]
        ev.append([EV_UPDATE, tag(dst_idx)] + list(dst[:16]))
    return ev, (tag(dst_idx) if dst_idx is not None else 0), inp['this_w'][17], arenas


def check(fn, inp, side_obs):
    obs = side_obs['observed']
    if fn == 'CMatrix::Attach':
        ev, attach_tag, owns_word, finals, _ = model_attach(inp)
    else:
        ev, attach_tag, owns_word, finals = model_assign(inp)
    got = obs['events']
    if ev != got:
        return 'eventos: esperado %s, observado %s' % (ev, got)
    if obs['attach_ptr'] != attach_tag:
        return 'puntero asociado: esperado %s, observado %s' % (hex(attach_tag), hex(obs['attach_ptr']))
    if obs['owns_word'] != owns_word:
        return 'dword de propiedad: esperado %s, observado %s' % (hex(owns_word), hex(obs['owns_word']))
    initial = [list(inp['this_w']), list(inp['other_w']), list(inp['rw_new_w']), list(inp['rw_old_w'])]
    if fn == 'CMatrix::Attach':
        initial[THIS][17] = inp['owns'] & 0xFFFFFFFF   # el banco escribe el dword de propiedad antes de llamar
    for i, (ini, fin, g) in enumerate(zip(initial, finals, obs['changes'])):
        exp = diff_bytes(ini, fin, skip=set(range(0x40, 0x44)) if i == THIS else ())
        got_c = [c for c in g if not (i == THIS and 0x40 <= c[0] < 0x44)]
        if exp != got_c:
            return 'cambios de la arena %d: esperado %s, observado %s' % (i, exp, got_c)
    return None


def run(path):
    rows = [json.loads(line) for line in open(path, encoding='utf-8')]
    bad = 0
    n = 0
    for r in rows:
        if r['event'] != 'comparison':
            continue
        n += 1
        for sname in ('original', 'candidate'):
            msg = check(r['function'], r['input'], r[sname])
            if msg:
                bad += 1
                if bad <= 10:
                    print('%s caso %d (familia %d) lado %s: %s' % (r['function'], r['case'], r['input']['family'], sname, msg))
    print('casos %d, lados distintos del modelo: %d' % (n, bad))
    return bad


if __name__ == '__main__':
    sys.exit(1 if run(sys.argv[1]) else 0)
