"""Alcanzabilidad del codigo escondido en los huecos del .text del juego (region 0x401000-0x74FFFF).

Entrada: docs/CodigoVivoExe/huecos.csv (huecos entre funciones de Ghidra), funciones reemplazadas por C++ (docs/JuegoExe,
docs/RenderWareExe), DESCARTAR/CUBIERTA de funciones-vivas.csv, el EXE canonico y el codigo C++ (valores citados).
Salida: docs/DatosExe/huecos-alcanzables.csv (un hueco por fila) y docs/DatosExe/huecos-alcanzables-resumen.csv.

Criterio de raices (el de docs/CodigoVivoExe/reclasificacion-raices-valor.md): un hueco es ALCANZABLE si alguna de estas cosas apunta
dentro de el:
  * codigo VIVO (funcion del EXE no reemplazada ni descartada): call/jmp/jcc relativo, inmediato de 32 bits (push/mov offset) o
    desplazamiento absoluto de memoria;
  * un VALOR guardado en datos (palabras alineadas de .rdata/.data: vtables, callbacks, tablas), o en tablas de saltos del .text.
    Una tabla indexada (jmp [t+reg*4]) cuyo codigo es solo de funciones reemplazadas o descartadas NO cuenta (sus destinos son los
    cuerpos de un switch muerto); si el que la usa es otro hueco, solo cuenta cuando ese hueco es alcanzable;
  * un valor citado por el C++ (literal usado como valor, no comentario);
  * otro hueco ya alcanzable (propagacion por saltos, llamadas e inmediatos del codigo del hueco);
  * caida directa desde una funcion viva que no termina en ret/jmp.
Nada se da por muerto por no tener llamadores: lo que no se alcanza queda como INALCANZABLE con su motivo (dato o codigo sin
entrada). Estatico: no ejecuta el juego; el error va siempre hacia alcanzable.

Uso: python -X utf8 contrib/huecos_alcanzables.py
"""
import bisect
import csv
import re
import struct
import sys
from collections import defaultdict, Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import datos_exe as de  # noqa: E402
from pe_inventory import PE  # noqa: E402
from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_MEM, CS_OP_IMM  # noqa: E402
from capstone.x86 import X86_REG_INVALID  # noqa: E402

RAIZ = Path(__file__).resolve().parent.parent
TXT_LO, TXT_HI = 0x401000, 0x750000
# huecos de los despachadores originales de comandos de script (CRunningScript::ProcessCommands<N>To<N+99> que siguen siendo del EXE)
DESPACHADORES = {0x48EA95, 0x470A8C, 0x4748F5, 0x47E08E, 0x465E56, 0x479D6E, 0x493FD2, 0x483BCC, 0x477F89}
EXE = Path('D:/Grand Theft Auto San Andreas/gta-sa.exe')


def cargar_huecos():
    hs = []
    with (RAIZ / 'docs/CodigoVivoExe/huecos.csv').open(encoding='utf-8-sig', newline='') as s:
        for r in csv.DictReader(s):
            va, fin = int(r['va'], 16), int(r['fin'], 16)
            if not (TXT_LO <= va < TXT_HI):
                continue
            cat = r['categoria']
            k = ('ref' if cat.startswith('código con referencia') else 'sinref' if cat.startswith('sin referencia') else
                 'tabla' if cat.startswith(('tabla de saltos', 'tabla de datos')) else 'relleno' if cat.startswith('relleno') else 'otro')
            hs.append(dict(va=va, fin=fin, bytes=fin - va + 1, kind=k, categoria=cat))
    return hs


def funciones(todas=False):
    reempl, _ = de.cargar_funciones()
    rangos = []
    with (RAIZ / 'docs/MemoryMap/binary_function_ranges.csv').open(encoding='utf-8-sig', newline='') as s:
        for r in csv.DictReader(s):
            f, a, b = int(r['function_va'], 16), int(r['start_va'], 16), int(r['end_va'], 16)
            if TXT_LO <= a < TXT_HI and (todas or f not in reempl):
                rangos.append((f, a, b, f not in reempl) if todas else (f, a, b))
    return rangos


def citas_cpp():
    """Direcciones del .text citadas por el C++ (literal de codigo fuera de comentarios)."""
    out = defaultdict(list)
    pat = re.compile(r'(?<![0-9A-Za-z_])0[xX]([0-9A-Fa-f]{6})(?![0-9A-Za-z_])')   # 6 cifras exactas
    for p in (RAIZ / 'source').rglob('*'):
        if p.suffix not in ('.cpp', '.h', '.hpp', '.inc') or p.name in ('ExeProtectionStub.cpp', 'SDLWrapper.cpp'):   # SDLWrapper: 0x475441 es un identificador de temporizador, no una direccion
            continue
        txt = de.blanquear_comentarios(p.read_bytes().decode('utf-8', errors='replace'))
        for m in pat.finditer(txt):
            v = int(m.group(1), 16)
            if TXT_LO <= v < TXT_HI or 0x858000 <= v < 0xC9E000:
                ln = txt.count('\n', 0, m.start()) + 1
                ctx = txt[txt.rfind('\n', 0, m.start()) + 1:txt.find('\n', m.end())]
                out[v].append((str(p.relative_to(RAIZ)).replace('\\', '/'), ln, 'enganche' if 'RH_Scoped' in ctx else 'valor'))
    return out


def es_texto(b):
    """4 bytes que parecen texto ASCII o UTF-16 (cadenas de .rdata que casualmente valen 0x4xxxxx-0x74xxxx)."""
    ok = lambda c: 0x20 <= c < 0x7F
    if b[1] == 0 and b[3] == 0 and ok(b[0]) and ok(b[2]):
        return True
    return all(ok(c) or c == 0 for c in b) and sum(ok(c) for c in b) >= 2


def main():
    pe = PE(EXE)
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    md.skipdata = True      # los huecos mezclan codigo y datos: se sigue barriendo tras un byte indecodificable
    hs = cargar_huecos()
    hs.sort(key=lambda h: h['va'])
    starts = [h['va'] for h in hs]
    n = len(hs)

    def hueco_de(a):
        i = bisect.bisect_right(starts, a) - 1
        if i >= 0 and hs[i]['va'] <= a <= hs[i]['fin']:
            return i
        return None

    ref = [dict(vivo=0, dato=0, cpp=0, hueco=0, caida=0, entrada=0, ej=[], ent={}) for _ in range(n)]

    hooked = set()      # direcciones con gancho RH_Scoped* (catalogo): su cuerpo original ya no se ejecuta
    with (RAIZ / 'docs/hooks.csv').open(encoding='utf-8-sig', newline='') as s:
        for r in csv.DictReader(s):
            try:
                hooked.add(int(r['address'], 16))
            except ValueError:
                pass
    ignoradas = Counter()

    def marca(i, tipo, a, quien):
        if a in hooked:
            ignoradas[tipo] += 1
            return
        ref[i][tipo] += 1
        ref[i]['ent'].setdefault(a, set()).add(tipo)
        if a == hs[i]['va']:
            ref[i]['entrada'] += 1
        if len(ref[i]['ej']) < 3:
            ref[i]['ej'].append(f'{tipo}:{quien}->{a:#x}')

    def barrer(ini, fin):
        """(destinos en el .text, bases de tablas indexadas, ultima instruccion) de [ini, fin)."""
        off = pe.offset(ini, 1)
        if off is None or fin <= ini:
            return [], [], None, []
        dest, tabs, last, drefs = [], [], None, []
        for ins in md.disasm(pe.data[off:off + (fin - ini)], ini):
            if ins.id == 0:
                continue
            last = ins
            for op in ins.operands:
                if op.type == CS_OP_IMM:
                    a = op.imm & 0xFFFFFFFF
                    if TXT_LO <= a < TXT_HI:
                        dest.append(a)
                    elif 0x858000 <= a < 0xC9E000:
                        drefs.append(a)
                elif op.type == CS_OP_MEM and op.mem.base == X86_REG_INVALID and op.mem.segment == 0:
                    a = op.mem.disp & 0xFFFFFFFF
                    if op.mem.index != X86_REG_INVALID:
                        tabs.append(a)
                    if TXT_LO <= a < TXT_HI:
                        dest.append(a)
                    elif 0x858000 <= a < 0xC9E000:
                        drefs.append(a)
        return dest, tabs, last, drefs

    # 0) todas las funciones (vivas y muertas) y todos los huecos de codigo: destinos y tablas que usan
    prop = defaultdict(lambda: dict(vivo=False, huecos=set()))
    vivas = []
    vivo_dat = set()                      # direcciones de datos que cita codigo vivo o el C++ (como valor)
    hueco_dat = defaultdict(set)          # direccion de datos -> huecos de codigo que la citan
    for f, a, b, viva in funciones(todas=True):
        dest, tabs, last, drefs = barrer(a, b)
        for d in tabs:
            prop[d]['vivo'] |= viva
        if viva:
            vivo_dat.update(drefs)
            vivas.append((f, a, b))
            for d in dest:
                i = hueco_de(d)
                if i is not None:
                    marca(i, 'vivo', d, f'{f:#x}')
            if last is not None and last.mnemonic not in ('ret', 'jmp', 'int3', 'ud2', 'retn'):
                i = hueco_de(b)
                if i is not None and hs[i]['va'] == b:
                    marca(i, 'caida', b, f'{f:#x}')
    hcod = {}
    for k, h in enumerate(hs):
        if h['kind'] in ('ref', 'sinref'):
            dest, tabs, last, drefs = barrer(h['va'], h['fin'] + 1)
            hcod[k] = (dest, tabs)
            for d in drefs:
                hueco_dat[d].add(k)
            for d in tabs:
                prop[d]['huecos'].add(k)
    bases = sorted(prop)

    def clave_tabla(w):
        i = bisect.bisect_right(bases, w) - 1
        if i < 0 or w - bases[i] > 0x4000:
            return None
        return bases[i]

    # 1) palabras con destino en un hueco (datos .rdata/.data/.rsrc, tablas del .text, tablas dentro de funciones vivas)
    textos = 0
    palabras = []   # (direccion de la palabra, valor, hueco destino, origen)
    for s in pe.sections:
        if s['executable'] or s['name'] in ('.bss', '_TEXT_HA'):   # _TEXT_HA = tablas del protector (decision: descartado)
            continue
        data = pe.data[s['raw_offset']:s['raw_offset'] + s['raw_size'] // 4 * 4]
        for k, (v,) in enumerate(struct.iter_unpack('<I', data)):
            if TXT_LO <= v < TXT_HI:
                j = hueco_de(v)
                if j is not None:
                    b = data[k * 4:k * 4 + 4]
                    ant = data[k * 4 - 4:k * 4] if k else b''
                    sig = data[k * 4 + 4:k * 4 + 8] if k * 4 + 8 <= len(data) else b''
                    if es_texto(b) and (b[1] == 0 and b[3] == 0 or (es_texto(ant) and es_texto(sig))):
                        textos += 1
                        continue
                    palabras.append((s['va'] + k * 4, v, j, s['name']))
    for i, h in enumerate(hs):
        if h['kind'] == 'tabla':
            off = pe.offset(h['va'], 1)
            blk = pe.data[off:off + (h['fin'] - h['va'] + 1) // 4 * 4]
            for k, (v,) in enumerate(struct.iter_unpack('<I', blk)):
                if TXT_LO <= v < TXT_HI:
                    j = hueco_de(v)
                    if j is not None and j != i:
                        palabras.append((h['va'] + k * 4, v, j, 'tabla.text'))
    for f, a, b in vivas:
        off = pe.offset(a, 1)
        if off is None:
            continue
        for va in range((a + 3) & ~3, b - 3, 4):
            v = struct.unpack_from('<I', pe.data, off + va - a)[0]
            if TXT_LO <= v < TXT_HI:
                j = hueco_de(v)
                if j is not None and hs[j]['va'] == v:
                    palabras.append((va, v, j, 'switch'))
    print('palabras descartadas por parecer texto:', textos)
    cit = citas_cpp()
    for v, lst in cit.items():
        if 0x858000 <= v < 0xC9E000 and any(f == 'valor' for _, _, f in lst):
            vivo_dat.add(v)

    def base_run(w):
        b = w
        while b - 4 >= 0x858000:
            q = pe.words(b - 4, 1)
            if q and TXT_LO <= q[0] < TXT_HI:
                b -= 4
            else:
                break
        return b

    cond = defaultdict(list)    # hueco propietario de la tabla -> palabras condicionadas a que ese hueco sea alcanzable
    muertos = Counter()
    for w, v, j, sec in palabras:
        key = clave_tabla(w)
        if key is None:
            # vtable o tabla de punteros sin indice visible: viva si alguien cita la base de su serie de punteros a codigo
            b = base_run(w)
            rng = range(b, w + 4, 4)
            if any(x in vivo_dat for x in rng):
                marca(j, 'dato', v, f'{sec}:{w:#x}')
            else:
                hk = set()
                for x in rng:
                    hk |= hueco_dat.get(x, set())
                if hk:
                    for k in hk:
                        cond[k].append((w, v, j, sec))
                else:
                    muertos[j] += 1     # nadie (vivo) cita la vtable/tabla: ningun constructor vivo la escribe
        elif prop[key]['vivo'] or key in vivo_dat or any(x in vivo_dat for x in range(key, w + 4, 4)):
            # la usa codigo vivo (jmp [t+reg*4] de una funcion no reemplazada) o la cita el C++ como valor (p. ej.
            # s_OriginalCommandHandlerTable = StaticRef<...>(0x8A6168): el C++ llama a los despachadores del EXE por esa tabla)
            marca(j, 'dato', v, f'{sec}:{w:#x}')
        elif prop[key]['huecos']:
            for k in prop[key]['huecos']:
                cond[k].append((w, v, j, sec))
        else:
            muertos[j] += 1     # solo la usan funciones reemplazadas o descartadas

    # 2) citas del C++ como valor
    for v, lst in cit.items():
        i = hueco_de(v)
        if i is not None:
            for arch, ln, forma in lst:
                if forma == 'valor':
                    marca(i, 'cpp', v, f'{arch}:{ln}')
                    break

    # 3) propagacion: un hueco alcanzable alcanza sus destinos y las tablas que usa
    def decodifica(h):
        off = pe.offset(h['va'], 1)
        tot, ni, ult = 0, 0, None
        md.skipdata = False
        for ins in md.disasm(pe.data[off:off + h['bytes']], h['va']):
            tot += ins.size
            ni += 1
            ult = ins
        md.skipdata = True
        code = pe.data[off:off + h['bytes']]
        ceros = code.count(0) / max(1, len(code))
        term = ult is not None and ult.mnemonic in ('ret', 'jmp', 'int3', 'ud2', 'retn')
        return ni, tot, term, ceros

    info = [decodifica(h) for h in hs]

    def es_codigo(k):
        ni, tot, term, ceros = info[k]
        return ni > 0 and tot >= 0.9 * hs[k]['bytes'] and ceros < 0.4 and hs[k]['kind'] in ('ref', 'sinref')

    alc = [sum(r[t] for t in ('vivo', 'dato', 'cpp', 'caida')) > 0 for r in ref]
    cola = [k for k in range(n) if alc[k]]
    hechos = set()
    while cola:
        k = cola.pop()
        if k in hechos or k not in hcod:
            continue
        hechos.add(k)
        dest, tabs = hcod[k]
        objetivos = []
        for d in dest:
            j = hueco_de(d)
            if j is not None and j != k:
                marca(j, 'hueco', d, f'hueco {hs[k]["va"]:#x}')
                objetivos.append(j)
        for w, v, j, sec in cond.get(k, []):
            marca(j, 'dato', v, f'{sec}:{w:#x}(tabla de hueco {hs[k]["va"]:#x})')
            objetivos.append(j)
        for j in objetivos:
            alc[j] = True
            cola.append(j)

    print('referencias a direcciones con gancho (ignoradas):', dict(ignoradas))
    rows = []
    for k, h in enumerate(hs):
        if h['kind'] not in ('ref', 'sinref'):
            continue
        ni, tot, term, ceros = info[k]
        r = ref[k]
        cod = es_codigo(k)
        if alc[k]:
            if r['entrada'] == 0:
                clase = 'ALCANZABLE-interior'   # solo se salta al interior (cuerpo de un switch, resto de una funcion partida)
            else:
                clase = 'ALCANZABLE-codigo' if cod else 'ALCANZABLE-dato'
        else:
            clase = 'INALCANZABLE-codigo-sin-entrada' if cod else 'INALCANZABLE-dato'
        o = pe.offset(h['va'], 3)
        arr = pe.data[o:o + 3]
        prologo = arr[:1] == b'\x55' or arr[:2] in (b'\x83\xec', b'\x81\xec', b'\x56\x8b', b'\x51\x56', b'\x53\x56')
        rows.append(dict(va=f'0x{h["va"]:08X}', fin=f'0x{h["fin"]:08X}', bytes=h['bytes'], kind=h['kind'], clase=clase,
                         decodifica=int(cod), instrucciones=ni, bytes_decod=tot, termina_ret_jmp=int(term), ceros=f'{ceros:.2f}',
                         prologo=int(prologo), ref_vivo=r['vivo'], ref_dato=r['dato'], ref_cpp=r['cpp'], ref_hueco=r['hueco'],
                         ref_caida=r['caida'], ref_entrada=r['entrada'], dato_tabla_muerta=muertos[k], ejemplos='; '.join(r['ej'])))
    citas = citas_cpp()
    # entradas alcanzables: extension por flujo de control dentro del hueco (hasta ret / salto fuera) y tipo de salida
    ents = []
    for k, h in enumerate(hs):
        if not alc[k] or h['kind'] not in ('ref', 'sinref'):
            continue
        for a, tipos in sorted(ref[k]['ent'].items()):
            vistos, pila, fin, indir, llamadas, salidas = set(), [a], a, 0, set(), 0
            while pila:
                x = pila.pop()
                while h['va'] <= x <= h['fin'] and x not in vistos:
                    o = pe.offset(x, 1)
                    ins = next(md.disasm(pe.data[o:o + 16], x), None)
                    if ins is None or ins.id == 0:
                        indir += 100
                        break
                    vistos.add(x)
                    fin = max(fin, x + ins.size)
                    if ins.mnemonic == 'call':
                        for op in ins.operands:
                            if op.type == CS_OP_IMM:
                                llamadas.add(op.imm & 0xFFFFFFFF)
                    if ins.mnemonic in ('ret', 'retn'):
                        break
                    if ins.mnemonic == 'jmp' or ins.mnemonic.startswith('j'):
                        op = ins.operands[0] if ins.operands else None
                        if op is not None and op.type == CS_OP_IMM:
                            d = op.imm & 0xFFFFFFFF
                            if h['va'] <= d <= h['fin']:
                                pila.append(d)
                            else:
                                salidas += 1
                        else:
                            indir += 1
                        if ins.mnemonic == 'jmp':
                            break
                    x += ins.size
            ents.append(dict(entrada=f'0x{a:08X}', hueco=f'0x{h["va"]:08X}', tamano_aprox=fin - a, instrucciones=len(vistos),
                             saltos_fuera=salidas, saltos_indirectos=indir, llamadas=len(llamadas), alineada16=int(a % 16 == 0),
                             origen='+'.join(sorted(tipos)), hooked=int(a in hooked)))
    # destino de cada entrada: de quien depende (vtable de una clase C++, libjpeg, RenderWare, cita C++ por valor...)
    ptrs = defaultdict(list)
    for s in pe.sections:
        if s['executable'] or s['name'] in ('.bss', '_TEXT_HA'):
            continue
        dd = pe.data[s['raw_offset']:s['raw_offset'] + s['raw_size'] // 4 * 4]
        for k_, (v_,) in enumerate(struct.iter_unpack('<I', dd)):
            if TXT_LO <= v_ < TXT_HI:
                ptrs[v_].append(s['va'] + k_ * 4)

    def base_vt(w):
        b_ = w
        while b_ - 4 >= 0x858000:
            q = pe.words(b_ - 4, 1)
            if q and TXT_LO <= q[0] < TXT_HI:
                b_ -= 4
            else:
                break
        return b_
    for e in ents:
        a_ = int(e['entrada'], 16)
        ws = ptrs.get(a_, [])
        e['vtable'] = e['ranura'] = e['cita_cpp'] = ''
        cs = [c for c in citas.get(a_, []) if c[2] == 'valor']
        if ws:
            b_ = base_vt(ws[0])
            e['vtable'] = f'0x{b_:08X}'
            e['ranura'] = (ws[0] - b_) // 4
            cs2 = [c for c in citas.get(b_, []) + citas.get(b_ - 4, []) if c[2] == 'valor']
            if cs2:
                e['cita_cpp'] = f'{cs2[0][0]}:{cs2[0][1]}'
        if cs and not e['cita_cpp']:
            e['cita_cpp'] = f'{cs[0][0]}:{cs[0][1]}'
        if 0x5C0000 <= a_ < 0x5D0000 or 'JPegCompress' in e['cita_cpp']:
            e['destino'] = 'libjpeg (enlazada)'
        elif a_ >= 0x74A000 or 'RenderWare' in e['cita_cpp']:
            e['destino'] = 'RenderWare (agente de RW/D3D9)'
        elif int(e['hueco'], 16) in DESPACHADORES or (e['vtable'] and 0x8A6168 <= int(e['vtable'], 16) < 0x8A61F0):
            e['destino'] = 'despachador de comandos de script del EXE (tabla 0x8A6168; sin reconstruir)'
        elif e['vtable']:
            e['destino'] = 'vtable de clase C++ (DE09: la genera el compilador)' if e['cita_cpp'] else 'vtable sin constructor C++ conocido (revisar)'
        elif e['cita_cpp']:
            e['destino'] = 'llamada C++ por direccion (reconstruir)'
        else:
            e['destino'] = 'solo desde otro hueco alcanzable'
    with (RAIZ / 'docs/DatosExe/huecos-alcanzables-entradas.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=list(ents[0]))
        w.writeheader()
        w.writerows(ents)
    print('entradas alcanzables:', len(ents))
    out = RAIZ / 'docs/DatosExe'
    with (out / 'huecos-alcanzables.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=list(rows[0]))
        w.writeheader()
        w.writerows(rows)
    cnt, byt = Counter(), Counter()
    for r in rows:
        cnt[(r['kind'], r['clase'])] += 1
        byt[(r['kind'], r['clase'])] += r['bytes']
    with (out / 'huecos-alcanzables-resumen.csv').open('w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['kind', 'clase', 'huecos', 'bytes'])
        for k in sorted(cnt):
            w.writerow([k[0], k[1], cnt[k], byt[k]])
            print(k, cnt[k], byt[k])


if __name__ == '__main__':
    main()
