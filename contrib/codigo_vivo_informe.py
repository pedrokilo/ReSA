"""Clasifica el codigo vivo del EXE y escribe docs/CodigoVivoExe/*.csv y resumen.json (usa los hechos de codigo_vivo.py).

Uso: python -X utf8 contrib/codigo_vivo_informe.py [--exe RUTA] [--cache hechos.pkl] [--salida docs/CodigoVivoExe]
Reproduce: funciones-vivas.csv, grupos.csv, huecos.csv, entradas-sin-catalogar.csv, destinos-sin-catalogo.csv, resumen.json.
(plan.csv lo genera codigo_vivo_plan.py a partir de funciones-vivas.csv.)
Terminologia: «funcion» = entrada de Ghidra (17.765); binary_function_ranges.csv tiene 19.169 TROZOS de esas funciones.
«reemplazada» = estado reemplazada de los CSV del juego/RenderWare; «enganchada» = reemplazada por un RH_Scoped*Install o por hooks.csv
que los CSV no marcan; «viva» = ninguna de las dos.
"""
import argparse
import bisect
import csv
import json
import re
import struct
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import codigo_vivo as cv  # noqa: E402
from codigo_vivo import DOCS, ROOT, ENTRADA, hx, leer, region  # noqa: E402

# ------------------------------------------------------------------ roles del CRT (por nombre, API del sistema y cuerpo)
ROLES_NOMBRE = [
    ('arranque', r'start|cinit|initterm|exit|crtExit|setargv|setenvp|getmainargs|amsg|NMSG|FF_MSGBANNER|wincmdln|WinMain|'
                 r'security_init|ioinit|heap_init|mtinit|lockexit|do_global|onexit|atexit|fast_error|_abort|__crt|dosmaperr|errno'),
    ('excepciones', r'SEH|unwind|terminate|abnormal|CxxFrame|Catch|throw|exception|Trys|FindHandler|Xran|Xlen|ULI|security_check|'
                    r'_local_unwind|ExceptionFilter|longjmp|setjmp|_NLG|RtlUnwind|inconsistency|purecall'),
    ('montón', r'sbh|heap|malloc|free|alloc|msize|realloc|calloc|^new|operator|_callnewh'),
    ('locale', r'lcid|locale|mbc|ctype|isalpha|isdigit|isspace|toupper|tolower|crtGetStringType|crtLCMap|crtGetLocale|cpinfo|'
               r'codepage|GetLcid|setlocale|__ascii|wcs|isleadbyte|isctype|mbstowcs|wctomb|mblen|strcoll|strxfrm'),
    ('numérico', r'fp|ctrl|trandisp|cintrin|isnan|ftol|aull|alldiv|allmul|allrem|allshl|allshr|aullrem|ulldiv|pow|atan|sin|cos|'
                 r'exp|log|sqrt|floor|ceil|fmod|fload|cfltcvt|fltout|strgtold|ld12|fassign|_set_|rand|abs|trunc|round|dtoi|CI'),
    ('E/S y stdio', r'output|input|file|fopen|fclose|fflush|flsbuf|filbuf|stbuf|ftbuf|open|read|write|lseek|close|commit|stream|'
                    r'printf|scanf|getc|putc|ungetc|setvbuf|pioinfo|osfile|fseek|ftell|chsize|getbuf|getdrive|chdir|mkdir|stat'),
    ('cadenas y memoria', r'^_?str|^_?mem|strlen|strcmp|strcpy|strcat|strstr|strtok|strncmp|strnicmp|stricmp|atol|atoi|atof|qsort|'
                          r'tolower|bsearch'),
]
API_ROLES = [
    ('E/S y stdio', ('ReadFile', 'WriteFile', 'CreateFileA', 'SetFilePointer', 'SetEndOfFile', 'FlushFileBuffers', 'GetFileType',
                     'GetStdHandle', 'SetHandleCount', 'CloseHandle', 'DeleteFileA', 'MoveFileA', 'GetFileAttributesA')),
    ('montón', ('HeapAlloc', 'HeapFree', 'HeapReAlloc', 'HeapCreate', 'HeapDestroy', 'HeapSize', 'VirtualAlloc', 'VirtualFree',
                'GetProcessHeap', 'HeapValidate')),
    ('locale', ('GetLocaleInfoA', 'GetLocaleInfoW', 'GetStringTypeA', 'GetStringTypeW', 'LCMapStringA', 'LCMapStringW',
                'EnumSystemLocalesA', 'GetCPInfo', 'IsValidLocale', 'IsValidCodePage', 'GetACP', 'GetOEMCP', 'MultiByteToWideChar',
                'WideCharToMultiByte', 'GetUserDefaultLCID', 'GetThreadLocale', 'CompareStringA', 'CompareStringW')),
    ('arranque', ('GetCommandLineA', 'GetEnvironmentStrings', 'GetEnvironmentStringsW', 'FreeEnvironmentStringsA',
                  'FreeEnvironmentStringsW', 'GetStartupInfoA', 'GetModuleFileNameA', 'GetVersion', 'GetVersionExA', 'ExitProcess',
                  'TerminateProcess', 'GetCurrentProcess', 'SetEnvironmentVariableA', 'GetEnvironmentVariableA', 'GetModuleHandleA')),
    ('excepciones', ('RaiseException', 'UnhandledExceptionFilter', 'SetUnhandledExceptionFilter', 'IsDebuggerPresent',
                     'IsBadReadPtr', 'IsBadWritePtr', 'IsBadCodePtr', 'RtlUnwind')),
    ('estado por hilo y bloqueos', ('TlsAlloc', 'TlsGetValue', 'TlsSetValue', 'TlsFree', 'CreateThread', 'ExitThread',
                                    'InitializeCriticalSection', 'EnterCriticalSection', 'LeaveCriticalSection',
                                    'DeleteCriticalSection', 'GetCurrentThreadId', 'InterlockedIncrement', 'InterlockedDecrement')),
]
DECISION_ROL = {
    'arranque': ('FASE8', 'Arranque del CRT (start, _cinit, _initterm, atexit): lo genera el enlace del ejecutable propio'),
    'excepciones': ('ENLAZAR', 'CRT/compilador de MSVC (__CxxFrameHandler, terminate, SEH): lo aporta el toolchain'),
    'montón': ('ENLAZAR', 'CRT de MSVC (malloc/free/new); ya hay hooks ExeCrt::Malloc/Free (opcion GTASA_CRT_HEAP_HOOKS)'),
    'locale': ('ENLAZAR', 'CRT de MSVC (locale, ctype, MBCS); rama crt4a-locale reconstruye parte'),
    'numérico': ('SUSTITUIR', 'C++ propio con redondeo identico (ExeCrt: floor/ceil/pow/rand/conversion FP); el resto, CRT de MSVC con /arch:IA32 o SSE2 tras prueba de redondeo'),
    'E/S y stdio': ('ENLAZAR', 'CRT de MSVC (stdio, E/S de bajo nivel); ya hay hooks ExeCrt (CRT2A/CRT5A)'),
    'cadenas y memoria': ('ENLAZAR', 'CRT de MSVC (str*, mem*); ya hay hooks ExeCrt::Str*'),
    'estado por hilo y bloqueos': ('ENLAZAR', 'CRT de MSVC (_getptd, bloqueos del CRT)'),
}
NOMBRE_RE = [(r, re.compile(p, re.I)) for r, p in ROLES_NOMBRE]


def rol_por_nombre(n):
    if n.startswith(('FUN_', 'thunk_FUN', 'Unwind@', 'Catch@')):
        return None
    n = n.lstrip('_')
    for r, rx in NOMBRE_RE:
        if rx.search(n):
            return r
    return None


def rol_por_api(apis):
    cuenta = Counter()
    for a in apis:
        nm = a.split('::')[-1]
        for r, lst in API_ROLES:
            if nm in lst:
                cuenta[r] += 1
    return cuenta.most_common(1)[0][0] if cuenta else None


# ------------------------------------------------------------------ carga y estado

def construir(args):
    H = cv.cargar_ejecutado(args)
    funcs, ex = H['funcs'], H['ex']
    pe = cv.PE(Path(args.exe))
    rep, rw, jg, exc = cv.cargar_estado(funcs)
    hooks = {}
    for fn in ('hooks.csv', 'hooks_notsa.csv'):
        for r in leer(DOCS / fn):
            try:
                va = int(r['address'], 16)
            except ValueError:
                continue
            hooks.setdefault(va, f"{r['class']}::{r['fn_name']}")
    for va, files in H['hooks_src'].items():
        hooks.setdefault(va, Path(files[0]).name)
    hooks = {va: n for va, n in hooks.items() if va in funcs}
    return dict(H=H, funcs=funcs, ex=ex, pe=pe, rep=rep, rw=rw, jg=jg, exc=exc, hooks=hooks)


def estado(v, C):
    if v in C['rep']:
        return 'reemplazada'
    if v in C['hooks']:
        return 'enganchada'
    return 'viva'


# ------------------------------------------------------------------ excepciones: de quien es cada funclet

def duenos_excepciones(C):
    """funclet (Unwind@/Catch@) -> {dueno}: funcion que instala la cadena de excepciones (via ehfuncinfo y su envoltura)."""
    pe, funcs, ex = C['pe'], C['funcs'], C['ex']

    def rd(a):
        w = pe.words(a, 1)
        return w[0] if w else None

    def en_rdata(a):
        return a is not None and 0x858000 <= a < 0x8A4000

    fis = {}
    for a in range(0x858000, 0x8A4000, 4):
        if rd(a) == 0x19930520:
            ms, pu, nt, pt = (rd(a + 4 * i) for i in (1, 2, 3, 4))
            if ms is not None and ms < 4096 and (pu == 0 or en_rdata(pu)) and nt < 64 and (nt == 0 or en_rdata(pt)):
                fis[a] = (ms, pu, nt, pt)
    funclet_fi = defaultdict(set)
    for a, (ms, pu, nt, pt) in fis.items():
        for i in range(ms):
            act = rd(pu + 8 * i + 4)
            if act in funcs:
                funclet_fi[act].add(a)
        for i in range(nt):
            ha, hn = rd(pt + 20 * i + 16), rd(pt + 20 * i + 12)
            for j in range(hn or 0):
                h = rd(ha + 16 * j + 12)
                if h in funcs:
                    funclet_fi[h].add(a)
    imm, ic, gr = ex['imm_rdata'], ex['imm_codigo'], ex['gap_rdata']
    ini_s = sorted((s, e, fv) for fv, f in funcs.items() for s, e in f['rangos'])
    ini_a = [t[0] for t in ini_s]

    def previa(a):
        i = bisect.bisect_right(ini_a, a) - 1
        return ini_s[i][2] if i >= 0 else None

    out, aprox = {}, set()
    for fl, fset in funclet_fi.items():
        duenos = set()
        for a in fset:
            for st in gr.get(a, ()):
                for o in ic.get(st, ()):
                    if isinstance(o, tuple):
                        p = previa(o[1])
                        if p is not None:
                            duenos.add(p)
                            aprox.add(fl)
                    else:
                        duenos.add(o)
            duenos |= set(imm.get(a, ()))
        out[fl] = duenos
    return out, aprox, len(fis)


# ------------------------------------------------------------------ clasificacion

def clasificar(v, C, eh):
    f, rw, exc, ex = C['funcs'][v], C['rw'], C['exc'], C['ex']
    n = f['nombre']
    reg = region(v)
    if v in C['rep']:
        return {'juego': 'Reemplazada: código del juego', 'renderware': 'Reemplazada: RenderWare/D3D'}.get(reg, 'Reemplazada: CRT'), None
    if v in exc:
        cat = exc[v]['categoria']
        if cat == 'proteccion-descartada':
            return 'Protección del EXE', cat
        if cat in ('usercall-renderware',):
            return 'RenderWare no reconstruible', 'usercall (excluida del juego)'
        if cat == 'retirada-quicktime':
            return 'QuickTime/QTML', cat
        if cat == 'sustituida-por-crt':
            return 'CRT y arranque', 'separador de la línea de comandos (sustituida por CRT)'
        return 'Juego excluido (cerrado por inspección)', cat
    if n.startswith(('Unwind@', 'Catch@')):
        return 'Excepciones (Unwind/Catch)', 'funclet'
    if 'DLL::' in n or n.startswith('DelayLoad_'):
        return 'Importaciones y carga retardada', 'thunk de importación'
    if v in C['rw']:
        c = C['rw'][v]
        cm = c['clase_modulo']
        if cm.startswith('D3DX9'):
            return 'D3DX9', c['estado']
        if cm.startswith('libpng'):
            return 'libpng y zlib', c['estado']
        if cm.startswith('QuickTime'):
            return 'QuickTime/QTML', c['estado']
        if cm.startswith('Carga retardada'):
            return 'Importaciones y carga retardada', 'carga retardada (RW)'
        return 'RenderWare no reconstruible', cm
    if reg == 'rwcseg':
        return 'RenderWare no reconstruible', '_rwcseg: callbacks de vectores x87 (sin SSE)'
    if reg == 'crt' or v in C['ex']['tablas']:
        return 'CRT y arranque', None
    if reg == 'renderware':
        return 'RenderWare sin clasificar', None
    return 'Juego sin clasificar', None


def propagar_roles(C, roles, vecinos):
    """roles[v] por nombre/API/cuerpo; propaga a vecinos sin rol (llamador o llamado ya clasificado), varias pasadas."""
    ar = C['ex']['aristas']
    callers, callees = defaultdict(set), defaultdict(set)
    for (o, d), k in ar.items():
        callers[d].add(o)
        callees[o].add(d)
    crt = [v for v in C['funcs'] if region(v) == 'crt']
    for _ in range(4):
        cambio = 0
        for v in crt:
            if roles.get(v):
                continue
            vec = Counter(roles[x] for x in (callers[v] | callees[v]) if roles.get(x) and region(x) == 'crt')
            if vec:
                roles[v] = vec.most_common(1)[0][0]
                vecinos.add(v)
                cambio += 1
        if not cambio:
            break
    return roles


def alcanzables(C, raices, grafo_vivo_desde):
    vivos = C['vivas']
    visto = set()
    cola = [r for r in raices if r in vivos]
    visto.update(cola)
    while cola:
        x = cola.pop()
        for y in grafo_vivo_desde.get(x, ()):
            if y in vivos and y not in visto:
                visto.add(y)
                cola.append(y)
    return visto


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', default=cv.EXE_DEFECTO)
    ap.add_argument('--cache', default=None)
    ap.add_argument('--salida', default=str(DOCS / 'CodigoVivoExe'))
    args = ap.parse_args()
    out = Path(args.salida)
    out.mkdir(parents=True, exist_ok=True)
    C = construir(args)
    funcs, ex, hooks, rep = C['funcs'], C['ex'], C['hooks'], C['rep']
    refs = C['H']['refs_src']
    est = {v: estado(v, C) for v in funcs}
    C['vivas'] = {v for v in funcs if est[v] == 'viva'}
    eh, eh_aprox, n_fi = duenos_excepciones(C)

    # grafo de llamadas (aristas fuera de funciones reemplazadas no cuentan)
    desde, hacia = defaultdict(set), defaultdict(set)
    for (o, d), k in ex['aristas'].items():
        desde[o].add(d)
        hacia[d].add(o)
    # huecos que contienen inicializadores: sus llamadas cuelgan de la entrada
    init = ex['tablas']
    gi = ex['gap_ini']
    gi_s = [g[0] for g in gi]
    init_hueco = {}
    for x in init:
        if x not in funcs:
            i = bisect.bisect_right(gi_s, x) - 1
            if i >= 0 and gi[i][0] <= x <= gi[i][1]:
                init_hueco[x] = gi[i][0]
    tablas_ptr = ex['punteros']
    es_funclet = lambda v: funcs[v]['nombre'].startswith(('Unwind@', 'Catch@'))

    # referencias desde C++ que no son enganches
    cpp_refs = {v: [r for r in rs if r[2] != 'enganche'] for v, rs in refs.items() if v in funcs}
    cpp_refs = {v: rs for v, rs in cpp_refs.items() if rs}
    cpp_refs_all = {v: [r for r in rs if r[2] != 'enganche'] for v, rs in refs.items()}
    cpp_refs_all = {v: rs for v, rs in cpp_refs_all.items() if rs}
    # raices
    raices_cpp = set(cpp_refs)
    raices_entrada = {ENTRADA} | {x for x in init if x in funcs}
    graf_extra = defaultdict(set)
    for x, gs in init_hueco.items():
        graf_extra[ENTRADA] |= set(ex['gap_calls'].get(gs, ()))
    graf = {v: set(desde.get(v, ())) | graf_extra.get(v, set()) for v in funcs}
    alc_cpp = alcanzables(C, raices_cpp, graf)
    alc_ent = alcanzables(C, raices_entrada, graf)
    ptr_no_eh = {v for v in funcs if v in tablas_ptr and not es_funclet(v)}

    # roles del CRT
    roles = {}
    for v, f in funcs.items():
        if region(v) == 'crt' and not es_funclet(v):
            r = rol_por_nombre(f['nombre'])
            if v in hooks and not r:
                r = rol_por_nombre(hooks[v].split('::')[-1])
            r = r or rol_por_api(ex['iat_uso'].get(v, ()))
            if not r:
                rs = ex['rasgos'].get(v, {})
                if rs.get('x87') and not rs.get('calls'):
                    r = 'numérico'
            if r:
                roles[v] = r
    vecinos = set()
    propagar_roles(C, roles, vecinos)

    # pasada 1: grupo y subgrupo de cada funcion no reemplazada
    base = {}
    for v, f in sorted(funcs.items()):
        if est[v] == 'reemplazada':
            continue
        grupo, sub = clasificar(v, C, eh)
        if grupo == 'CRT y arranque' and sub is None:
            rol = roles.get(v)
            if v in init:
                sub = 'inicializador estático (en _initterm)'
            elif rol:
                sub = rol + (' (por vecinos)' if v in vecinos else '')
            else:
                sub = 'sin rol identificado'
        base[v] = (grupo, sub)
    # raiz externa de cada grupo: C++ que la llama o un llamador vivo de otro grupo
    raiz_grupo = defaultdict(int)
    for v, (g, _) in base.items():
        if est[v] != 'viva':
            continue
        if v in raices_cpp or any(est[c] == 'viva' and c in base and base[c][0] != g for c in hacia.get(v, ())):
            raiz_grupo[g] += 1
    # pasada 2: filas y decisiones (los funclets con dueno vivo heredan su decision en la pasada 3)
    filas = []
    for v, (grupo, sub) in base.items():
        f = funcs[v]
        rs = ex['rasgos'].get(v, {})
        cs = hacia.get(v, set())
        cv_ = sorted(c for c in cs if est[c] == 'viva')
        cr_ = sorted(c for c in cs if est[c] != 'viva')
        dueno = ''
        dueno_est = ''
        if es_funclet(v):
            ds = eh.get(v, set())
            if ds:
                dd = sorted(ds)[0]
                dueno = hx(dd)
                dueno_est = est[dd]
                if len({est[x] for x in ds}) > 1:
                    dueno_est = 'mixto'
            else:
                dueno_est = 'sin dueño'
        forms = Counter(r[2] for r in cpp_refs.get(v, []))
        ejemplo = ''
        if v in cpp_refs:
            r0 = cpp_refs[v][0]
            ejemplo = f'{r0[0]}:{r0[1]}'
        reach = []
        if v in alc_cpp:
            reach.append('C++')
        if v in alc_ent:
            reach.append('entrada')
        if v in ptr_no_eh:
            reach.append('tabla')
        decision, destino, nota = decidir(v, f, est[v], grupo, sub, C, dueno_est, bool(cs), v in cpp_refs, reach, f['virtuales'],
                                          raiz_grupo.get(grupo, 0))
        filas.append(dict(
            va=hx(v), bytes=f['bytes'], trozos=f['trozos'], estado=est[v], region=region(v), grupo=grupo, subgrupo=sub or '',
            nombre_ghidra=f['nombre'], nombre_catalogo=f['catalogo'] or hooks.get(v, ''), thunk=int(f['thunk']),
            llamadores_vivos=len(cv_), llamadores_reemplazados=len(cr_), ejemplo_llamador=hx((cv_ or cr_ or [0])[0]) if cs else '',
            referencias_cpp=sum(forms.values()), formas_cpp=';'.join(f'{k}={n}' for k, n in sorted(forms.items())), ejemplo_cpp=ejemplo,
            puntero_en_datos=len(tablas_ptr.get(v, [])), vtable=f['virtuales'], inicializador=init.get(v, ''),
            dueno_excepciones=dueno, estado_dueno=dueno_est, alcanzable_por='+'.join(reach) or 'nadie',
            x87=int(bool(rs.get('x87'))), sse=int(bool(rs.get('sse'))), pushad=int(bool(rs.get('pushad'))),
            importaciones=';'.join(sorted(a.split('::')[-1] for a in ex['iat_uso'].get(v, ()))[:4]),
            decision=decision, destino=destino, nota=nota))
    # pasada 3: funclet con dueno vivo -> decision del dueno
    dec_por_va = {r['va']: r for r in filas}
    for r in filas:
        if r['decision'] == 'SEGUIR_AL_DUEÑO':
            d = dec_por_va.get(r['dueno_excepciones'])
            if d:
                r['decision'], r['destino'] = d['decision'], d['destino']
                r['nota'] = f"funclet de {r['dueno_excepciones']}: sigue su decisión ({d['decision']})"
            else:
                r['decision'] = 'REVISAR'
    ctx_raiz = dict(raiz_grupo)

    campos = list(filas[0])
    with open(out / 'funciones-vivas.csv', 'w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=campos)
        w.writeheader()
        w.writerows(filas)

    # grupos
    g = defaultdict(lambda: Counter())
    for r in filas:
        k = (r['grupo'], r['decision'])
        g[k]['funciones'] += 1
        g[k]['bytes'] += r['bytes']
        g[k]['enganchadas'] += r['estado'] == 'enganchada'
        g[k]['vivas'] += r['estado'] == 'viva'
        g[k]['bytes_vivas'] += r['bytes'] if r['estado'] == 'viva' else 0
        g[k]['con_referencia_cpp'] += r['referencias_cpp'] > 0
        g[k]['alcanzables_cpp'] += 'C++' in r['alcanzable_por']
        g[k]['alcanzables_entrada'] += 'entrada' in r['alcanzable_por']
        g[k]['nadie'] += r['alcanzable_por'] == 'nadie'
    with open(out / 'grupos.csv', 'w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['grupo', 'decision', 'funciones', 'bytes', 'enganchadas', 'vivas', 'bytes_vivas', 'con_referencia_cpp',
                    'alcanzables_desde_cpp', 'alcanzables_desde_entrada', 'sin_alcance'])
        for (gr, d), c in sorted(g.items()):
            w.writerow([gr, d, c['funciones'], c['bytes'], c['enganchadas'], c['vivas'], c['bytes_vivas'], c['con_referencia_cpp'],
                        c['alcanzables_cpp'], c['alcanzables_entrada'], c['nadie']])

    sub = defaultdict(Counter)
    for r in filas:
        if r['estado'] == 'viva':
            k = (r['grupo'], r['subgrupo'], r['decision'])
            sub[k]['f'] += 1
            sub[k]['b'] += r['bytes']
    with open(out / 'subgrupos.csv', 'w', encoding='utf-8', newline='') as s:
        w = csv.writer(s)
        w.writerow(['grupo', 'subgrupo', 'decision', 'funciones_vivas', 'bytes_vivas'])
        for (gr, sg, d), c in sorted(sub.items()):
            w.writerow([gr, sg, d, c['f'], c['b']])

    ctx = dict(C=C, est=est, hooks=hooks, hacia=hacia, desde=desde, cpp_refs=cpp_refs, filas=filas, eh=eh, n_fi=n_fi,
               alc_cpp=alc_cpp, alc_ent=alc_ent, init=init, init_hueco=init_hueco, raices_cpp=raices_cpp, ptr_no_eh=ptr_no_eh,
               eh_aprox=eh_aprox, raiz_grupo=ctx_raiz, cpp_refs_all=cpp_refs_all)
    from codigo_vivo_auditoria import auditar
    auditar(ctx, out)


def decidir(v, f, est_, grupo, sub, C, dueno_est, tiene_llamador, tiene_cpp, reach, virt, raices_grupo):
    """-> (decision, destino, nota). Decisiones: SUSTITUIR, ENLAZAR, DESCARTAR, FASE8, REVISAR, CUBIERTA (ya enganchada).
    Regla de necesidad: una funcion de biblioteca o CRT que ni el C++ ni la entrada ni una tabla alcanzan se DESCARTA (posible
    muerto); si el grupo entero no tiene raiz viva (ningun C++ ni llamador vivo de otro grupo) se descarta entero."""
    if est_ == 'enganchada':
        return 'CUBIERTA', 'enganche existente', 'ya sustituida por un enganche; al quitar el original solo hay que retirar el enganche'
    if grupo == 'Protección del EXE':
        return 'DESCARTAR', 'protección', 'código del protector; el ejecutable propio no lo necesita'
    if grupo == 'Excepciones (Unwind/Catch)':
        if dueno_est in ('reemplazada', 'enganchada'):
            return 'DESCARTAR', 'huérfano de excepciones', 'funclet de limpieza de una función ya reemplazada; el compilador genera los propios'
        if dueno_est == 'viva':
            return 'SEGUIR_AL_DUEÑO', 'excepciones', 'acompaña a su función dueña (sigue su decisión)'
        if dueno_est == 'mixto':
            return 'REVISAR', 'excepciones', 'dueños con estados distintos'
        return 'REVISAR', 'excepciones', 'dueño no resuelto (envoltura de excepciones sin llamador directo en el EXE)'
    if grupo == 'Importaciones y carga retardada':
        return 'FASE8', 'enlace', 'thunk de importación o carga retardada: el enlazador del ejecutable propio los genera'
    if grupo == 'Juego excluido (cerrado por inspección)':
        if sub in ('salto-puro', 'no-es-funcion'):
            return 'DESCARTAR', 'no es función propia', 'salto puro o fragmento interno: el destino ya está reconstruido'
        return 'DESCARTAR', 'cubierta por llamadoras', 'usercall cuyas llamadoras directas ya son C++ validado; confirmar sin llamadores vivos'
    # grupos con regla de necesidad
    if grupo == 'D3DX9':
        dec, des, nota = 'ENLAZAR', 'D3DX9 real (d3dx9_43.dll)', 'decisión del usuario: enlazar la biblioteca de Microsoft (compilador de shaders)'
    elif grupo == 'libpng y zlib':
        dec, des, nota = 'ENLAZAR', 'libpng 1.0.12 + zlib 1.1.3 (libs/)', 'opción GTASA_WITH_ZLIB_LIBPNG; hooks LIB2A para la lectura'
    elif grupo == 'QuickTime/QTML':
        dec, des, nota = 'SUSTITUIR', 'Media Foundation (decodificador de MP3)', 'puente de QuickTime usado por AEMFDecoder (radio personal)'
    elif grupo == 'RenderWare no reconstruible':
        dec, des, nota = 'SUSTITUIR', 'C++ propio (RenderWare)', 'usercall/x87/3DNow!: C++ puro no puede engancharla; reescribir con la misma semántica'
    elif grupo == 'CRT y arranque':
        if sub and sub.startswith('inicializador'):
            return 'FASE8', 'inicializadores estáticos', 'entrada de _initterm: el C++ propio inicializa sus globales por constructor (DatosExe)'
        rol = (sub or '').replace(' (por vecinos)', '')
        if rol in DECISION_ROL:
            d, t = DECISION_ROL[rol]
            dec = d
            des = 'CRT de MSVC' if d == 'ENLAZAR' else ('C++ propio (ExeCrt)' if d == 'SUSTITUIR' else 'arranque del CRT enlazado')
            nota = t
        else:
            dec, des, nota = 'ENLAZAR', 'CRT de MSVC', 'función del CRT sin rol identificado: el CRT enlazado la aporta'
    else:
        return 'REVISAR', '', 'sin clasificar'
    if raices_grupo == 0:
        return 'DESCARTAR', 'biblioteca embebida sin raíces vivas', f'{nota}; ningún C++ ni llamador vivo de otro grupo la usa: el cuerpo embebido sobra'
    if not reach:
        return 'DESCARTAR', 'sin alcance (posible muerto)', f'{nota}; ni C++, ni la entrada, ni una tabla la alcanzan'
    if reach == ['tabla']:
        return dec, des, nota + '; solo alcanzable por puntero en una tabla de datos'
    return dec, des, nota


if __name__ == '__main__':
    main()
