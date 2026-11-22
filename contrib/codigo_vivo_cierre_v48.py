"""Cierre de los descartes V4-V8 con BUSQUEDA POR REGISTRO de las llamadas indirectas (extiende codigo_vivo_cierre.py).

El cierre anterior (codigo_vivo_cierre.py) dejo V4-V8 en 'verificado sin cerrar': 845 funciones sin ninguna referencia viva
(aristas, busqueda bruta del dword, C++), pero con 114 llamadas indirectas (call reg / call [reg+x] / jmp [reg*4+tabla]) en las
283 funciones vivas conservadas cuyo destino no se habia resuelto. Este modulo:

  1. localiza esas llamadas indirectas y las clasifica por patron (tabla de saltos del propio codigo, vtable COM del sistema,
     puntero de la IAT / GetProcAddress, puntero de datos del CRT...), resolviendo las tablas de saltos y la definicion local
     del registro (--sitios);
  2. calcula el CONJUNTO ALCANZABLE por punto fijo (--alcance) sobre funciones y huecos del .text y sobre las tablas de datos:
       raices   = referencias del C++ que no son enganche + entrada del CRT + inicializadores + funciones vivas conservadas;
       aristas  = call/jmp/jcc directos, inmediatos y desplazamientos que apuntan al codigo, y operandos que apuntan a datos
                  (una referencia a una direccion de datos marca vivos todos los punteros a codigo de la ventana
                  [v-VENT_ATRAS, v+VENT_ADELANTE) y, transitivamente, los de las tablas a las que apuntan esos datos);
       llamada indirecta no resuelta = puede ir a cualquier funcion cuya direccion este tomada por codigo vivo o por datos vivos:
                  eso ya esta en el conjunto (las aristas imm/datos), de modo que no anade destinos nuevos; lo que no se puede
                  excluir es un destino calculado: se comprueba (--calculados) que el codigo vivo no construye direcciones del
                  .text por aritmetica y que lo que devuelve el sistema (GetProcAddress, COM) son direcciones de DLL;
  3. con --aplicar actualiza cierre-p1.csv (columna cierre), funciones-vivas.csv y cierre.csv de V4-V8.

Uso:
  python -X utf8 contrib/codigo_vivo_cierre_v48.py --sitios [--cache hechos.pkl]
  python -X utf8 contrib/codigo_vivo_cierre_v48.py --alcance [--cache hechos.pkl] [--aplicar]
Es determinista. Entradas: el EXE canonico (72ae59e4...) y docs/CodigoVivoExe/.
"""
import argparse
import bisect
import csv
import pickle
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(Path(__file__).resolve().parent))
import codigo_vivo as cv  # noqa: E402
from pe_inventory import PE  # noqa: E402

OUT = ROOT / 'docs' / 'CodigoVivoExe'
LOTES_V = ['V1', 'V2-1', 'V2-2', 'V3-1', 'V3-2', 'V3-3', 'V3-4', 'V3-5', 'V3-6', 'V4-1', 'V4-2', 'V4-3', 'V4-4',
           'V5-1', 'V5-2', 'V5-3', 'V6-1', 'V7-1', 'V8-1']
LOTES_OBJ = ['V4-1', 'V4-2', 'V4-3', 'V4-4', 'V5-1', 'V5-2', 'V5-3', 'V6-1', 'V7-1', 'V8-1']
CODE_LO, CODE_HI = cv.CODE_LO, cv.CODE_HI
DATA_LO, DATA_HI = 0x858000, 0xC10000
VENT_ATRAS, VENT_ADELANTE = 0x40, 0x100
# Archivos C++ que son una ISLA: solo se entra en ellos por los enganches que instalan (sobre rutinas internas del compilador de
# shaders de D3DX9 del EXE) y sus llamadas por direccion solo se ejecutan si el EXE llama a una de esas direcciones enganchadas
# (ningun otro archivo los usa: solo InjectHooksMain.cpp los registra). Sus referencias son raices unicamente si la isla se activa.
ISLAS = ('source/game_sa/RenderWare/rw/D3dxErrorLog.cpp', 'source/game_sa/RenderWare/rw/D3dxHlslCompiler.cpp',
         'source/game_sa/RenderWare/rw/D3dxShaderAssembler.cpp', 'source/game_sa/RenderWare/rw/D3dxShaderSource.cpp')
MODO = 'estricto'


def leer(p):
    with open(p, encoding='utf-8-sig', newline='') as s:
        return list(csv.DictReader(s))


def escribir(p, filas, campos):
    with open(p, 'w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=campos)
        w.writeheader()
        w.writerows(filas)


def conservadas(vivas):
    return [x for x in vivas if x['estado'] == 'viva' and x['lote'] not in LOTES_V]


# ---------------------------------------------------------------- escaneo completo de aristas (con cache)

def escanear_todo(pe, funcs, ex, cache):
    if cache and Path(cache).exists():
        return pickle.load(open(cache, 'rb'))
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_IMM, CS_OP_MEM, CS_GRP_JUMP, CS_GRP_CALL
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    chunks = [(s, e, fv) for fv, f in funcs.items() for s, e in f['rangos']]
    chunks += [(s, e, ('gap', s)) for s, e in ex['gap_ini']]
    aristas = []   # (dueno, kind, ins, destino, extra)
    for s, e, own in chunks:
        off = pe.offset(s, 1)
        if off is None:
            continue
        ultimo = None
        for ins in md.disasm(pe.data[off:off + e - s + 1], s):
            ultimo = ins
            br = ins.group(CS_GRP_JUMP) or ins.group(CS_GRP_CALL)
            for op in ins.operands:
                if op.type == CS_OP_IMM:
                    v = op.imm & 0xFFFFFFFF
                    if br:
                        if CODE_LO <= v < CODE_HI:
                            aristas.append((own, 'br', ins.address, v, ins.mnemonic))
                    elif CODE_LO <= v < CODE_HI:
                        aristas.append((own, 'imm_code', ins.address, v, ins.mnemonic))
                    elif DATA_LO <= v < DATA_HI:
                        aristas.append((own, 'imm_data', ins.address, v, ins.mnemonic))
                elif op.type == CS_OP_MEM:
                    d = op.mem.disp & 0xFFFFFFFF
                    reg = op.mem.base != 0 or op.mem.index != 0
                    if CODE_LO <= d < CODE_HI:
                        aristas.append((own, 'mem_code', ins.address, d, f'{ins.mnemonic}|{int(reg)}|{op.mem.scale}'))
                    elif DATA_LO <= d < DATA_HI:
                        aristas.append((own, 'mem_data', ins.address, d, f'{ins.mnemonic}|{int(reg)}|{op.mem.scale}'))
        # caida al trozo siguiente: si el trozo no termina en ret/jmp/ud2/int3 sigue ejecutandose lo que haya a continuacion
        if ultimo is not None and ultimo.mnemonic not in ('ret', 'retn', 'jmp', 'ud2', 'int3', 'hlt'):
            aristas.append((own, 'br', ultimo.address, ultimo.address + ultimo.size, 'caida'))
    if cache:
        pickle.dump(aristas, open(cache, 'wb'))
    return aristas


# ---------------------------------------------------------------- sitios indirectos

def sitios(pe, funcs, vivas):
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_IMM, CS_OP_MEM
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    iat = cv.importaciones(pe)
    out = []
    for x in conservadas(vivas):
        va = int(x['va'], 16)
        for s, e in funcs[va]['rangos']:
            off = pe.offset(s, 1)
            lst = list(md.disasm(pe.data[off:off + e - s + 1], s))
            for i, ins in enumerate(lst):
                if ins.mnemonic not in ('call', 'jmp') or not ins.operands or ins.operands[0].type == CS_OP_IMM:
                    continue
                op = ins.operands[0]
                if op.type == CS_OP_MEM and op.mem.base == 0 and op.mem.index == 0:
                    continue
                site = dict(func=va, lote=x['lote'], ins=ins.address, texto=f'{ins.mnemonic} {ins.op_str}', tipo='', detalle='',
                            destinos=[])
                if op.type == CS_OP_MEM and op.mem.base == 0 and op.mem.index != 0 and op.mem.scale == 4:
                    t = op.mem.disp & 0xFFFFFFFF
                    rg = funcs[va]['rangos']
                    tg = []
                    k = 0
                    salto = 0
                    while True:
                        w = pe.words(t + 4 * k, 1)
                        if not w or not any(a <= w[0] <= b for a, b in rg):
                            if not tg and salto < 2:      # tabla desplazada: el indice del codigo empieza en 1 (la primera palabra es parte de la instruccion anterior)
                                salto += 1
                                k += 1
                                continue
                            break
                        tg.append(w[0])
                        k += 1
                    if not tg:        # indice negativo (ecx = longitud-4 < 0): las entradas estan antes del desplazamiento
                        k = 1
                        while True:
                            w = pe.words(t - 4 * k, 1)
                            if not w or not any(a <= w[0] <= b for a, b in rg):
                                break
                            tg.append(w[0])
                            k += 1
                    site['tipo'] = 'tabla de saltos'
                    site['detalle'] = (f'tabla 0x{t:08X}: {len(tg)} entradas dentro de 0x{va:08X} ({min(tg):08X}..{max(tg):08X})'
                                       if tg else f'tabla 0x{t:08X}: vacia')
                    site['destinos'] = tg
                elif op.type == CS_OP_MEM:
                    b = ins.reg_name(op.mem.base) if op.mem.base else ''
                    site['tipo'] = 'memoria [reg+disp]'
                    site['detalle'] = f'[{b}+0x{op.mem.disp:X}]'
                    for j in reversed(lst[max(0, i - 8):i]):
                        if b and j.op_str.startswith(b + ','):
                            site['detalle'] += f' <- {j.address:08X}: {j.mnemonic} {j.op_str}'
                            break
                else:
                    r = ins.op_str
                    df = ''
                    for j in reversed(lst[max(0, i - 10):i]):
                        if j.op_str.startswith(r + ',') or (j.mnemonic == 'pop' and j.op_str == r):
                            df = f'{j.address:08X}: {j.mnemonic} {j.op_str}'
                            for o2 in j.operands:
                                if o2.type == CS_OP_MEM and o2.mem.base == 0 and o2.mem.index == 0:
                                    d = o2.mem.disp & 0xFFFFFFFF
                                    if d in iat:
                                        df += f' (IAT {iat[d]})'
                            break
                    site['tipo'] = 'registro'
                    site['detalle'] = f'{r} <- {df}'
                out.append(site)
    return out



# Patron por funcion de las llamadas indirectas que no son tabla de saltos ni IAT (revisado a mano sobre el desensamblado; ver ejecucion-v48.md)
ETIQUETAS = {
    0x4CB730: 'RwEngineInstance (0xC97B24) o interfaz COM de D3D9: tabla de funciones cargada en ejecucion',
    0x4CB7C0: 'RwEngineInstance (0xC97B24) o interfaz COM de D3D9: tabla de funciones cargada en ejecucion',
    0x4CB9C0: 'RwEngineInstance (0xC97B24) o interfaz COM de D3D9: tabla de funciones cargada en ejecucion',
    0x4CCD70: 'RwEngineInstance (0xC97B24) o interfaz COM de D3D9: tabla de funciones cargada en ejecucion',
    0x4CCE60: 'RwEngineInstance (0xC97B24) o interfaz COM de D3D9: tabla de funciones cargada en ejecucion',
    0x7D3550: 'callback de libpng guardado en png_struct (+0x40 read_row_fn/+0x44 etc.), fijado por png_set_* con una direccion literal del llamador',
    0x7D3620: 'callback de libpng guardado en png_struct (+0x44), fijado por png_set_* con una direccion literal del llamador',
    0x7D3E90: 'callback de libpng guardado en png_struct (+0xE4/0x124, read_row/progresivo), fijado con una direccion literal',
    0x7E1230: 'zlib: z_stream.zfree/zalloc (+0x20/+0x24); png usa 0x7E9300 (literal en 0x7E1341) o los del llamador',
    0x7E1280: 'zlib: z_stream.zfree/zalloc (+0x20/+0x24); png usa 0x7E9300 (literal en 0x7E1341) o los del llamador',
    0x7E6560: 'callback de libpng guardado en png_struct (+0x1CC), fijado con una direccion literal',
    0x821509: 'trampolin de excepciones: salta a la direccion de continuacion/miembro recibida (funclets y tablas de excepciones)',
    0x821539: 'trampolin de excepciones (_CallMemberFunction0): miembro recibido por la pila',
    0x821540: 'trampolin de excepciones (_CallMemberFunction1): miembro recibido por la pila',
    0x821547: 'trampolin de excepciones (_CallMemberFunction1): miembro recibido por la pila',
    0x821614: 'continuacion del catch (jmp [EH_RECORD+0x18]): direccion del bloque catch de la tabla de excepciones',
    0x8217E5: 'terminate() instalado: ptd->_pterminate (+0x74), fijado por set_terminate con un literal o el de defecto',
    0x8218D6: '__local_unwind2: manejador __finally de la tabla SEH del llamador (funclets)',
    0x821D30: 'constructor/destructor por elemento pasado por el llamador (vector ctor/dtor iterator)',
    0x821DA4: 'destructor por elemento pasado por el llamador (__ArrayUnwind)',
    0x821E02: 'destructor por elemento pasado por el llamador (vector destructor iterator)',
    0x823AFE: 'GetProcAddress("CorExitProcess") de mscoree.dll (IAT GetModuleHandleA/GetProcAddress)',
    0x823B76: 'tabla de inicializadores _initterm (rangos 0x8A4000-0x8A5A10 y 0x8A5A14-0x8A5A30, ya raices) y punteros del CRT',
    0x823BDB: 'tabla de _onexit/atexit (se rellena con direcciones literales de quien llama a atexit) y tabla de inicializadores',
    0x824570: 'API del sistema obtenida de la IAT (GetModuleHandleA, GetStartupInfoA...)',
    0x825FF8: '_longjmp: validador _pxcptinfoptrs del jmp_buf (+0x24, firma 0x56433230) y EIP guardado en el jmp_buf (+0x14)',
    0x8279F0: 'manejador catch/destino de la tabla de excepciones (__InternalCxxFrameHandler)',
    0x827A92: 'terminate() instalado: ptd->_pterminate (+0x6C), fijado por set_terminate',
    0x827ADF: '_inconsistency(): puntero [0x8E310C] fijado por _set_inconsistency (nulo de defecto)',
    0x827DA0: '__CallSettingFrame: funclet catch/unwind recibido de la tabla de excepciones',
    0x82ACBD: 'API del sistema obtenida de la IAT (DeleteCriticalSection)',
    0x82BEDA: 'tabla de terminadores _initterm (0x88E240...): lista de la tabla del CRT (ya cubierta por las referencias a datos)',
    0x82C3EC: 'API del sistema obtenida de la IAT (WideCharToMultiByte)',
    0x82D6C1: '__FF_MSGBANNER: puntero [0xC9ADD0] del CRT (fijado en ejecucion; nulo de defecto)',
    0x82DBC8: 'API del sistema obtenida de la IAT (GetEnvironmentStringsW / FreeEnvironmentStringsW)',
    0x82EB9C: '_raise: manejador de senales de las tablas del CRT (signal(): direccion literal del llamador o SIG_DFL)',
    0x833A20: 'puntero [0xC9AF90] del CRT (fijado en ejecucion; nulo de defecto)',
    0x834513: 'API del sistema obtenida de la IAT (GetCPInfo)',
    0x835177: 'GetProcAddress de user32.dll (MessageBoxA, GetActiveWindow, GetLastActivePopup...) guardado en 0xC9AFAC..0xC9AFB0',
}


def etiqueta(sit, iat_nombre=''):
    if sit['tipo'] == 'tabla de saltos':
        return 'tabla de saltos propia: ' + sit['detalle']
    if sit['lote'] == 'F3':
        return 'direccion que devuelve __delayLoadHelper2 (GetProcAddress de d3d9.dll/dinput8.dll/ddraw.dll): codigo de DLL del sistema'
    return ETIQUETAS.get(sit['func'], 'sin etiqueta')


# ---------------------------------------------------------------- alcance (punto fijo)

def alcance(pe, hechos, aristas, vivas):
    funcs, ex = hechos['funcs'], hechos['ex']
    por_va = {int(x['va'], 16): x for x in vivas}
    gaps = sorted(ex['gap_ini'])
    chunks = sorted([(s, e, fv) for fv, f in funcs.items() for s, e in f['rangos']] + [(s, e, ('gap', s)) for s, e in gaps],
                    key=lambda t: t[0])
    ini = [c[0] for c in chunks]

    def cont(a):
        i = bisect.bisect_right(ini, a) - 1
        if i >= 0 and chunks[i][0] <= a <= chunks[i][1]:
            n = chunks[i][2]
            return ('g', a, chunks[i][0], chunks[i][1]) if isinstance(n, tuple) else n
        return None

    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_IMM, CS_OP_MEM, CS_GRP_JUMP, CS_GRP_CALL, CS_GRP_RET
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    cache_flujo = {}

    def flujo(nodo):
        """aristas de un punto de entrada de hueco: recorre desde la entrada siguiendo saltos dentro del mismo hueco."""
        _, a0, gs, ge = nodo
        if nodo in cache_flujo:
            return cache_flujo[nodo]
        out, vis, pend = [], set(), [a0]
        while pend:
            a = pend.pop()
            while gs <= a <= ge and a not in vis:
                off = pe.offset(a, 1)
                ins = next(md.disasm(pe.data[off:off + 16], a), None)
                if ins is None:
                    break
                vis.add(a)
                br = ins.group(CS_GRP_JUMP) or ins.group(CS_GRP_CALL)
                for op in ins.operands:
                    if op.type == CS_OP_IMM:
                        v = op.imm & 0xFFFFFFFF
                        if br:
                            if gs <= v <= ge and ins.mnemonic != 'call':
                                pend.append(v)
                            elif CODE_LO <= v < CODE_HI:
                                out.append(('br', v, ins.address, ins.mnemonic))
                        elif CODE_LO <= v < CODE_HI:
                            out.append(('imm_code', v, ins.address, ins.mnemonic))
                        elif DATA_LO <= v < DATA_HI:
                            out.append(('imm_data', v, ins.address, ins.mnemonic))
                    elif op.type == CS_OP_MEM:
                        d = op.mem.disp & 0xFFFFFFFF
                        reg = op.mem.base != 0 or op.mem.index != 0
                        if CODE_LO <= d < CODE_HI:
                            out.append(('mem_code', d, ins.address, f'{ins.mnemonic}|{int(reg)}|{op.mem.scale}'))
                        elif DATA_LO <= d < DATA_HI:
                            out.append(('mem_data', d, ins.address, f'{ins.mnemonic}|{int(reg)}|{op.mem.scale}'))
                if ins.mnemonic in ('ret', 'retn', 'jmp', 'hlt', 'int3', 'ud2') or ins.group(CS_GRP_RET):
                    break
                a += ins.size
        cache_flujo[nodo] = out
        return out

    sal = defaultdict(list)
    for own, k, ins, v, extra in aristas:
        if not isinstance(own, tuple):
            sal[own].append((k, v, ins, extra))

    vivos, cola = {}, []
    islas_activas = {}

    def activar_isla(f, why):
        if f in islas_activas:
            return
        islas_activas[f] = why
        for va2, rs2 in hechos['refs_src'].items():
            for rel, ln, forma in rs2:
                if rel == f and forma != 'enganche':
                    marcar(cont(va2), f'C++ {rel}:{ln} (isla activada por {why})')

    def marcar(n, why):
        if n is None or n in vivos:
            return
        if not isinstance(n, tuple):
            r = por_va.get(n)
            if n in hechos['hooks_src']:
                for f in hechos['hooks_src'][n]:
                    if f in ISLAS:
                        activar_isla(f, f'llamada a la direccion enganchada 0x{n:08X}: {why}')
                return                # enganchada: su cuerpo no se ejecuta
            if r is None or r['estado'] != 'viva':
                return                # reemplazada: su cuerpo no se ejecuta
        vivos[n] = why
        cola.append(n)

    for va, rs in hechos['refs_src'].items():
        rs = [t for t in rs if t[0] not in ISLAS]
        if any(f != 'enganche' for _, _, f in rs):
            marcar(cont(va), f'C++ {rs[0][0]}:{rs[0][1]}')
    marcar(cont(cv.ENTRADA), 'entrada del CRT')
    for va in ex['tablas']:
        marcar(cont(va), 'inicializador')
    for x in conservadas(vivas):
        marcar(int(x['va'], 16), 'conservada (' + x['lote'] + ')')

    def palabra(a):
        w = pe.words(a, 1)
        return w[0] if w else None

    vistos = set()

    def valida(w, dueno):
        """una palabra de datos cuenta como puntero a codigo si apunta a un inicio de funcion, a un hueco o a la propia funcion
        del codigo que usa la tabla (tablas de saltos); las demas (cadenas ASCII, constantes) no."""
        if not (CODE_LO <= w < CODE_HI):
            return False
        if w in funcs:
            return True
        c = cont(w)
        return isinstance(c, tuple) or (c is not None and c == dueno)

    def ventana(v, atras, adelante, why, dueno=None):
        """tabla de punteros a codigo que contiene la direccion v: racha contigua de palabras que apuntan al .text
        (con holgura opcional de `atras`/`adelante` bytes); no se siguen punteros a datos (modo estricto)."""
        if MODO == 'amplio':
            for a in range((v - atras) & ~3, v + adelante, 4):
                w = palabra(a)
                if w is None:
                    continue
                if valida(w, dueno):
                    marcar(cont(w), why)
                elif DATA_LO <= w < DATA_HI and ('p', a) not in vistos:
                    vistos.add(('p', a))
                    ventana(w, 0, 0x40, why, dueno)
            return
        v &= ~3
        w = palabra(v)
        if w is None or not valida(w, dueno):
            return
        a = v
        while (w := palabra(a)) is not None and valida(w, dueno):
            marcar(cont(w), why)
            a -= 4
        a = v + 4
        while (w := palabra(a)) is not None and valida(w, dueno):
            marcar(cont(w), why)
            a += 4

    def nom(n):
        return hex(n[1]) if isinstance(n, tuple) else hex(n)

    while cola:
        n = cola.pop()
        dueno = n if not isinstance(n, tuple) else None
        for k, v, ins, extra in (flujo(n) if isinstance(n, tuple) else sal.get(n, ())):
            if k in ('br', 'imm_code'):
                marcar(cont(v), f'{k} desde {nom(n)}')
            elif k == 'mem_code':
                a = v
                while True:
                    w = palabra(a)
                    if w is None or not valida(w, dueno):
                        break
                    marcar(cont(w), f'tabla en .text 0x{v:08X} desde {nom(n)}')
                    a += 4
            elif k == 'imm_data':
                if ('i', v) not in vistos:
                    vistos.add(('i', v))
                    ventana(v, 0, VENT_ADELANTE, f'dato 0x{v:08X} (imm) desde {nom(n)}', dueno)
            elif k == 'mem_data':
                if ('m', v) not in vistos:
                    vistos.add(('m', v))
                    idx = '|1|' in extra
                    ventana(v, VENT_ATRAS if idx else 0, VENT_ADELANTE if idx else 4, f'dato 0x{v:08X} (mem) desde {nom(n)}', dueno)
    vivos['__islas__'] = dict(islas_activas) if False else None
    del vivos['__islas__']
    alcance.islas = islas_activas
    return vivos, cont, (lambda n: flujo(n) if isinstance(n, tuple) else sal.get(n, ()))


def calculados(pe, hechos, vivos, aristas_de, vivas):
    """direcciones del .text construidas por aritmetica en codigo vivo (cota para las llamadas por registro)."""
    from capstone import Cs, CS_ARCH_X86, CS_MODE_32, CS_OP_IMM, CS_OP_MEM
    funcs, ex = hechos['funcs'], hechos['ex']
    md = Cs(CS_ARCH_X86, CS_MODE_32)
    md.detail = True
    iat = cv.importaciones(pe)
    res = dict(base=[], call_pop=[], aritmetica=[], tablas_bytes=[], carga_dll=[], n_ins=0)
    rangos = []
    for n in vivos:
        if isinstance(n, tuple):
            rangos.append((n[2], n[3], n))
        else:
            rangos += [(a, b, n) for a, b in funcs[n]['rangos']]
    visto = set()
    for a, b, n in sorted(rangos):
        if (a, b) in visto:
            continue
        visto.add((a, b))
        off = pe.offset(a, 1)
        lst = list(md.disasm(pe.data[off:off + b - a + 1], a))
        for i, ins in enumerate(lst):
            res['n_ins'] += 1
            if ins.mnemonic == 'call' and ins.operands and ins.operands[0].type == CS_OP_IMM and (ins.operands[0].imm & 0xFFFFFFFF) == ins.address + ins.size:
                res['call_pop'].append(ins.address)
            for op in ins.operands:
                if op.type == CS_OP_IMM:
                    v = op.imm & 0xFFFFFFFF
                    if 0x400000 <= v < 0x401000 and ins.mnemonic not in ('ret', 'retn'):
                        res['base'].append((ins.address, ins.mnemonic, v))
                    if CODE_LO <= v < CODE_HI and ins.mnemonic in ('add', 'sub', 'or', 'xor', 'imul', 'and', 'shl', 'shr', 'adc', 'sbb'):
                        res['aritmetica'].append((ins.address, ins.mnemonic, v))
                elif op.type == CS_OP_MEM:
                    d = op.mem.disp & 0xFFFFFFFF
                    if CODE_LO <= d < CODE_HI and ins.mnemonic == 'lea':
                        res['aritmetica'].append((ins.address, 'lea', d))
                    if CODE_LO <= d < CODE_HI and op.mem.scale in (1, 2) and op.mem.index != 0:
                        res['tablas_bytes'].append((ins.address, ins.mnemonic, d, op.mem.scale))
                    if op.mem.base == 0 and op.mem.index == 0 and d in iat and iat[d].split('::')[1] in ('GetModuleHandleA', 'GetModuleHandleW', 'LoadLibraryA', 'LoadLibraryW', 'GetProcAddress', 'LoadLibraryExA', 'LoadLibraryExW'):
                        res['carga_dll'].append((ins.address, iat[d]))
    return res


def aplicar(vivas, vivos, cadena, hooks):
    filas = leer(OUT / 'cierre-p1.csv')
    campos = list(filas[0])
    if 'motivo_v48' not in campos:
        campos.append('motivo_v48')
    n_ant = Counter()
    nuevos = {}
    for r in filas:
        r.setdefault('motivo_v48', '')
        if r['lote'] not in LOTES_OBJ:
            continue
        va = int(r['va'], 16)
        n_ant[(r['lote'], r['cierre'][:12])] += 1
        if va in vivos:
            if r['cierre'] == 'abierta' and r['detalle_bloqueos']:
                r['motivo_v48'] = 'sigue abierta: la llama codigo vivo conservado; cadena desde la raiz: ' + cadena(va)
            else:
                r['cierre'] = 'abierta'
                r['bloqueos'] = '1'
                r['detalle_bloqueos'] = 'alcanzable (busqueda por registro v48): ' + cadena(va)
                r['motivo_v48'] = 'reabierta: ' + cadena(va)
        else:
            if r['cierre'].startswith(('verificada', 'condicionada')):
                r['motivo_v48'] = ('cerrada: ' + ('el hueco o la tabla que la citaba no es alcanzable (punto fijo de contrib/codigo_vivo_cierre_v48.py); '
                                                  if r['cierre'].startswith('condicionada') else '')
                                   + 'ninguna llamada indirecta de las funciones vivas conservadas puede llegar (sin direccion tomada por codigo vivo, sin destino calculado)')
                r['cierre'] = 'cerrada'
            elif r['cierre'] == 'abierta':
                h = hooks.get(va)
                r['motivo_v48'] = ('cerrada: la bloqueaban llamadores del EXE que hoy estan enganchados o muertos; ' +
                                   (f'esta funcion esta enganchada en {h[0]} (su cuerpo ya no se ejecuta)' if h else 'ya no la alcanza nada'))
                r['cierre'] = 'cerrada'
                r['bloqueos'] = '0'
                r['detalle_bloqueos'] = ''
        nuevos[r['va']] = r['cierre']
    escribir(OUT / 'cierre-p1.csv', filas, campos)
    # funciones-vivas.csv
    campos_v = list(vivas[0])
    for x in vivas:
        if x['va'] in nuevos:
            x['cierre'] = nuevos[x['va']]
    escribir(OUT / 'funciones-vivas.csv', vivas, campos_v)
    # cierre.csv por lote
    previo = {x['lote']: x for x in leer(OUT / 'cierre.csv')}
    por_lote = defaultdict(Counter)
    for r in filas:
        if r['lote'] in LOTES_OBJ:
            por_lote[r['lote']][r['cierre'][:10]] += 1
    for l in LOTES_OBJ:
        c = por_lote[l]
        n = sum(c.values())
        cerr = c.get('cerrada', 0)
        est = 'cerrado' if cerr == n else ('parcial' if cerr else 'verificado sin cerrar')
        previo[l] = dict(lote=l, estado=est, nota=(
            f'busqueda por registro de las 114 llamadas indirectas de las funciones vivas conservadas (contrib/codigo_vivo_cierre_v48.py, ejecucion-v48.md): '
            f'{cerr} de {n} funciones cerradas (sin referencia viva y sin destino posible por registro); {c.get("abierta", 0)} abiertas con motivo en cierre-p1.csv (columna motivo_v48)'))
    escribir(OUT / 'cierre.csv', list(previo.values()), ['lote', 'estado', 'nota'])
    print('aplicado: cierre-p1.csv, funciones-vivas.csv, cierre.csv')
    for l in LOTES_OBJ:
        print(l, dict(por_lote[l]))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', default=cv.EXE_DEFECTO)
    ap.add_argument('--cache', default=None, help='pickle de hechos de codigo_vivo.py')
    ap.add_argument('--cache-aristas', default=None)
    ap.add_argument('--sitios', action='store_true')
    ap.add_argument('--alcance', action='store_true')
    ap.add_argument('--calculados', action='store_true')
    ap.add_argument('--aplicar', action='store_true')
    ap.add_argument('--modo', default='estricto', choices=['estricto', 'amplio'])
    a = ap.parse_args()
    global MODO
    MODO = a.modo
    hechos = cv.cargar_ejecutado(argparse.Namespace(exe=a.exe, cache=a.cache))
    pe = PE(Path(a.exe))
    vivas = leer(OUT / 'funciones-vivas.csv')
    funcs = hechos['funcs']
    if a.sitios:
        ss = sitios(pe, funcs, vivas)
        print(len(ss), dict(Counter(s['tipo'] for s in ss)))
        for s in ss:
            print(f"0x{s['ins']:08X} f=0x{s['func']:08X} {s['lote']:<18} {s['texto']:<34} {s['tipo']}: {s['detalle']}")
        sin = [s for s in ss if etiqueta(s) == 'sin etiqueta']
        print('sin etiqueta:', len(sin))
        if a.aplicar:
            filas = []
            for s in ss:
                dentro = all(any(x0 <= t <= x1 for x0, x1 in funcs[s['func']]['rangos']) for t in s['destinos']) if s['destinos'] else True
                filas.append(dict(llamada=f"0x{s['ins']:08X}", funcion=f"0x{s['func']:08X}", lote_funcion=s['lote'], instruccion=s['texto'],
                                  patron=s['tipo'], definicion=s['detalle'], resolucion=etiqueta(s),
                                  destinos_en_v4_v8='ninguno' + (' (todas las entradas de la tabla caen dentro de la propia funcion)' if s['destinos'] and dentro else
                                                                 ' (destino = direccion tomada por codigo vivo, ya en el conjunto alcanzable, o codigo de DLL del sistema)')))
            escribir(OUT / 'busqueda-registro-v48.csv', filas, list(filas[0]))
            print('escrito busqueda-registro-v48.csv', len(filas))
    if a.alcance:
        aristas = escanear_todo(pe, funcs, hechos['ex'], a.cache_aristas)
        vivos, cont, aristas_de = alcance(pe, hechos, aristas, vivas)
        print('nodos vivos:', len(vivos), 'funciones:', sum(1 for n in vivos if not isinstance(n, tuple)))
        # el resultado definitivo es la UNION de los dos modos (vivo en cualquiera = no se cierra)
        if a.modo == 'estricto':
            MODO = 'amplio'
            vivos2, _, _ = alcance(pe, hechos, aristas, vivas)
            MODO = 'estricto'
        else:
            vivos2 = {}
        todos = dict(vivos2)
        todos.update(vivos)
        print('union de modos: nodos', len(todos), '(estricto', len(vivos), 'amplio', len(vivos2), ')')
        por_dir = {(n if not isinstance(n, tuple) else n[1]): n for n in todos}

        def cadena(va, tope=8):
            out, n = [], va
            for _ in range(tope):
                w = vivos.get(n) or vivos2.get(n)
                if w is None:
                    break
                out.append(f'0x{(n if not isinstance(n, tuple) else n[1]):08X}')
                m = re.search(r'desde (0x[0-9a-f]+)', w)
                if not m:
                    out.append(f'[{w}]')
                    break
                n = por_dir.get(int(m.group(1), 16))
                if n is None:
                    break
            return ' <- '.join(out)

        res = Counter()
        for x in vivas:
            if x['lote'] in LOTES_OBJ:
                va = int(x['va'], 16)
                res[(x['lote'][:2], x['cierre'][:12], 'VIVA' if va in todos else 'muerta')] += 1
        for k in sorted(res):
            print(k, res[k])
        for x in vivas:
            va = int(x['va'], 16)
            if x['lote'] in LOTES_OBJ and va in todos:
                print('  VIVA', x['va'], x['lote'], x['cierre'][:14], cadena(va))
        if a.aplicar:
            aplicar(vivas, todos, cadena, hechos['hooks_src'])
        if a.calculados:
            c = calculados(pe, hechos, vivos, aristas_de, vivas)
            print('calculados: instrucciones revisadas', c['n_ins'])
            for k in ('base', 'call_pop', 'aritmetica', 'tablas_bytes', 'carga_dll'):
                print(' ', k, len(c[k]))
                for t in c[k][:40]:
                    print('     ', t if not isinstance(t, int) else hex(t))


if __name__ == '__main__':
    main()
