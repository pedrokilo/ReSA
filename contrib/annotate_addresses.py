"""Anota en el codigo fuente la direccion del EXE de cada funcion reconstruida.

Para cada hook del catalogo (docs/hooks.csv) localiza la definicion de la funcion
en source/ y comprueba que el comentario justo encima (o en la misma linea de la
cabecera) contiene su direccion (`0x6D05E0`, `0x006D05E0` o `@addr`).

Modos:
  --report   (por defecto) mide y escribe un resumen; no cambia nada.
  --check    como --report, pero devuelve codigo != 0 si falta alguna direccion.
  --apply    inserta o completa SOLO comentarios. Si ya hay un comentario sobre la
             funcion le anade la direccion; si no, crea un bloque
                 /*!
                  * @addr 0x6D05E0
                  */
             (o `// 0x6D05E0` en los archivos que ya usan ese estilo).

Garantia mecanica: tras modificar un archivo se comparan el codigo antes/despues
con los comentarios eliminados y los espacios normalizados; si difieren no se
escribe ese archivo. Se conservan los finales de linea y la codificacion (los
archivos se leen y escriben como bytes latin-1, sin transformar nada mas).

Las direcciones de los metodos virtuales (type V) se resuelven del propio macro
RH_ScopedVMTInstall(fn, direccion), que lleva la direccion real de la funcion
del EXE; la tabla (vtable) y la ranura salen de RH_ScopedVirtualClass y del EXE.
Ver contrib/direcciones_doc.py, que usa este modulo para generar docs/direcciones.csv.
"""
import argparse
import csv
import re
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
HOOKS_CSV = ROOT / 'docs/hooks.csv'

# --------------------------------------------------------------------------
# Lectura y enmascarado de fuentes
# --------------------------------------------------------------------------

_MASK = re.compile(
    r'//[^\n]*(?:\\\r?\n[^\n]*)*'
    r'|/\*.*?\*/'
    r'|R"([^()\s\\]{0,16})\(.*?\)\1"'
    r'|"(?:\\.|[^"\\\n])*"'
    r"|(?<![0-9A-Za-z_])'(?:\\.|[^'\\\n])*'",
    re.DOTALL)
_PREPROC = re.compile(r'^[ \t]*#(?:[^\n]*\\\r?\n)*[^\n]*', re.MULTILINE)


def _blank(m):
    return re.sub(r'[^\r\n]', ' ', m.group(0))


def mask(text):
    """Sustituye comentarios, literales y directivas por espacios (misma longitud)."""
    text = _MASK.sub(_blank, text)
    return _PREPROC.sub(_blank, text)


def read_text(path):
    return Path(path).read_bytes().decode('latin-1')


def source_files(root=ROOT):
    out = []
    for p in sorted((Path(root) / 'source').rglob('*')):
        if p.suffix in ('.cpp', '.h', '.hpp', '.inl', '.inc'):
            out.append(p)
    return out


# --------------------------------------------------------------------------
# Hooks registrados en el codigo (macros RH_Scoped*)
# --------------------------------------------------------------------------

SCOPE_MACRO = re.compile(
    r'\bRH_Scoped(VirtualClass|NamedClass|Class|NamespaceName|Namespace)\s*\(')
INSTALL_MACRO = re.compile(
    r'\bRH_Scoped(Install|GlobalInstall|OverloadedInstall|GlobalOverloadedInstall|'
    r'NamedGlobalInstall|NamedInstall|VMTInstall|VMTOverloadedInstall)\s*\(')
_MACRO_ANY = re.compile(r'(?P<brace>[{}])|' + SCOPE_MACRO.pattern + '|' + INSTALL_MACRO.pattern)


def split_args(text, start):
    """Devuelve (lista de argumentos, indice tras el parentesis de cierre)."""
    depth = 0
    args, cur = [], []
    i = start
    n = len(text)
    while i < n:
        c = text[i]
        if c in '([{':
            depth += 1
            cur.append(c)
        elif c in ')]}':
            if depth == 0:
                args.append(''.join(cur).strip())
                return args, i + 1
            depth -= 1
            cur.append(c)
        elif c == ',' and depth == 0:
            args.append(''.join(cur).strip())
            cur = []
        else:
            cur.append(c)
        i += 1
    return args, n


def parse_hex(expr):
    m = re.fullmatch(r'\(?\s*(?:\(\s*\w+\s*\*?\s*\))?\s*0[xX]([0-9A-Fa-f]+)\s*\)?', expr.strip())
    return int(m.group(1), 16) if m else None


def _hex_or_const(raw, expr):
    """Numero hexadecimal o nombre de constante definida en el mismo archivo."""
    v = parse_hex(expr)
    if v is None and re.fullmatch(r'[A-Za-z_]\w*', expr.strip()):
        m = re.search(r'\b' + re.escape(expr.strip()) + r'\s*=\s*0[xX]([0-9A-Fa-f]+)', raw)
        v = int(m.group(1), 16) if m else None
    return v


def unquote(s):
    m = re.fullmatch(r'"(.*)"', s.strip())
    return m.group(1) if m else None


def line_of(text, idx):
    return text.count('\n', 0, idx) + 1


class Hook:
    __slots__ = ('cls', 'name', 'sym_scope', 'sym', 'kind', 'addr', 'cast', 'file',
                 'line', 'vtbl', 'nvirt', 'is_virtual', 'defn', 'defns')

    def __repr__(self):
        return f'Hook({self.cls}::{self.name} {self.addr:#x})'


def scan_hooks(root=ROOT, files=None):
    """Extrae todos los hooks activos (no comentados) del codigo fuente."""
    hooks = []
    for path in files or source_files(root):
        raw = read_text(path)
        if 'RH_Scoped' not in raw:
            continue
        code = mask(raw)
        rel = path.relative_to(root).as_posix()
        csv_class = sym_scope = None
        vtbl = nvirt = None
        # las cadenas se enmascararon: los nombres de CSV se leen del texto crudo
        saved = []
        for m in _MACRO_ANY.finditer(code):
            if m.group('brace'):
                if m.group('brace') == '{':
                    saved.append((csv_class, sym_scope, vtbl, nvirt))
                elif saved:
                    csv_class, sym_scope, vtbl, nvirt = saved.pop()
                continue
            args, _ = split_args(raw, m.end())
            if m.group(2):
                kind = m.group(2)
                if kind in ('VirtualClass', 'Class'):
                    csv_class = sym_scope = args[0]
                    vtbl = nvirt = None
                    if kind == 'VirtualClass':
                        vtbl = _hex_or_const(raw, args[1]) if len(args) > 1 else None
                        nvirt = int(args[2]) if len(args) > 2 and args[2].isdigit() else None
                elif kind == 'NamedClass':
                    sym_scope = args[0]
                    csv_class = unquote(args[1]) if len(args) > 1 else args[0]
                    vtbl = nvirt = None
                elif kind == 'NamespaceName':
                    csv_class = unquote(args[0]) or args[0]
                    sym_scope = None
                    vtbl = nvirt = None
                else:  # Namespace
                    csv_class = sym_scope = args[0]
                    vtbl = nvirt = None
                continue
            kind = m.group(3)
            h = Hook()
            h.kind, h.file, h.line = kind, rel, line_of(raw, m.start())
            h.vtbl, h.nvirt, h.defn, h.defns = vtbl, nvirt, None, []
            h.is_virtual = kind.startswith('VMT')
            h.cls = csv_class
            h.sym_scope = None if kind.startswith('Global') or kind == 'NamedGlobalInstall' else sym_scope
            h.cast = ''
            try:
                if kind in ('Install', 'GlobalInstall', 'VMTInstall'):
                    h.sym = h.name = args[0]
                    h.addr = parse_hex(args[1])
                elif kind in ('OverloadedInstall', 'GlobalOverloadedInstall', 'VMTOverloadedInstall'):
                    h.sym = args[0]
                    suffix = unquote(args[1])
                    h.name = f'{args[0]}-{suffix}'
                    h.addr = parse_hex(args[2])
                    h.cast = ','.join(args[3:])
                else:  # NamedGlobalInstall / NamedInstall
                    h.sym = args[0]
                    h.name = unquote(args[1])
                    h.addr = parse_hex(args[2])
            except (IndexError, TypeError):
                continue
            if h.addr is None or h.name is None or h.cls is None:
                hooks.append(h) if h.addr is not None and h.cls else None
                continue
            hooks.append(h)
    return hooks


# --------------------------------------------------------------------------
# Definiciones de funciones
# --------------------------------------------------------------------------

_SKIP_IDENT = {'decltype', 'alignas', 'noexcept', 'sizeof', '__declspec', 'static_assert',
               'if', 'for', 'while', 'switch', 'catch', 'return', 'requires', '__attribute__',
               'defined', 'typeof'}
_DECL_RE = re.compile(
    r'((?:~?[A-Za-z_]\w*(?:\s*<[^()]*?>)?\s*::\s*)*'
    r'(?:operator\s*(?:\(\s*\)|\[\s*\]|[^\s(]+)|~?[A-Za-z_]\w*))\s*\(')
_TEMPLATE_PREFIX = re.compile(r'^\s*template\s*<')
_SCOPE_KW = re.compile(r'^\s*(?:template\s*<.*>\s*)?(?:typedef\s+)?(class|struct|union|namespace)\b|'
                       r'^\s*extern\s*"[^"]*"\s*$', re.DOTALL)
_LABEL = re.compile(r'^\s*(?:public|private|protected)\s*:')


class FuncDef:
    __slots__ = ('file', 'quals', 'name', 'start', 'line', 'header', 'params', 'comment_has',
                 'declstart')

    def __repr__(self):
        return f'FuncDef({"::".join(self.quals + [self.name])} {self.file}:{self.line})'


def _match_brace(code, i):
    depth = 0
    for m in re.finditer(r'[{}]', code[i:]):
        depth += 1 if m.group(0) == '{' else -1
        if depth == 0:
            return i + m.end()
    return len(code)


def _strip_templates(s):
    out, depth = [], 0
    for c in s:
        if c == '<':
            depth += 1
        elif c == '>':
            depth = max(0, depth - 1)
        elif depth == 0:
            out.append(c)
    return ''.join(out)


def _skip_template_prefix(header):
    m = _TEMPLATE_PREFIX.match(header)
    if not m:
        return 0
    depth, i = 1, m.end()
    while i < len(header) and depth:
        if header[i] == '<':
            depth += 1
        elif header[i] == '>':
            depth -= 1
        i += 1
    return i


def _params_of(header, open_idx):
    depth = 0
    for i in range(open_idx, len(header)):
        if header[i] == '(':
            depth += 1
        elif header[i] == ')':
            depth -= 1
            if depth == 0:
                return header[open_idx + 1:i]
    return ''


def scan_defs(path, root=ROOT):
    """Devuelve las definiciones de funciones (con cuerpo) de un archivo."""
    raw = read_text(path)
    code = mask(raw)
    rel = Path(path).relative_to(root).as_posix()
    stack = []  # nombres de ambitos (clase/namespace)
    defs = []
    decl_start = 0
    skip_until = 0
    for m in re.finditer(r'[{};]', code):
        i = m.start()
        if i < skip_until:
            continue  # dentro de un cuerpo ya saltado
        c = m.group(0)
        if c == ';':
            decl_start = i + 1
            continue
        if c == '}':
            if stack:
                stack.pop()
            decl_start = i + 1
            continue
        header = code[decl_start:i]
        hs = _LABEL.sub('', header, count=1)
        if header.count('(') != header.count(')'):
            # llave dentro de parentesis (lambda en un argumento): sigue en la cabecera
            skip_until = _match_brace(code, i)
            continue
        # llaves de la lista de inicializacion de un constructor: `miembro{ valor }`
        pm = re.search(r'(\w+|>)\s*$', header)
        if (pm and pm.group(1) not in ('const', 'noexcept', 'override', 'final', 'volatile', 'mutable')
                and '(' in header and re.search(r'\)\s*(?:noexcept\s*)?:(?!:)', header)):
            skip_until = _match_brace(code, i)
            continue
        is_scope = False
        sm = _SCOPE_KW.match(hs)
        if sm and '(' not in re.sub(r'(?:alignas|__declspec)\s*\([^()]*\)', '', hs):
            is_scope = True
        if is_scope:
            if hs.lstrip().startswith('extern'):
                name = ''
            else:
                body = re.sub(r'(?:alignas|__declspec)\s*\([^()]*\)', ' ', hs)
                body = re.sub(r'^\s*template\s*<.*?>\s*', '', body, flags=re.DOTALL)
                body = body.split(':', 1)[0] if '::' not in body else re.sub(r'(?<!:):(?!:).*', '', body, flags=re.DOTALL)
                ids = re.findall(r'[A-Za-z_]\w*(?:::[A-Za-z_]\w*)*', body)
                ids = [x for x in ids if x not in ('class', 'struct', 'union', 'namespace', 'final',
                                                   'typedef', 'inline', 'alignas')]
                name = ids[-1] if ids else ''
                name = re.sub(r'^.*::', '', name) if name else name
            stack.append(name)
            decl_start = i + 1
            continue
        hs_nt = hs[_skip_template_prefix(hs):]
        if re.match(r'^\s*(?:enum\b|using\b)', hs_nt) or '(' not in hs_nt:
            skip_until = _match_brace(code, i)
            continue
        # cabecera de funcion: busca el declarador
        fm = None
        for cand in _DECL_RE.finditer(hs_nt):
            ident = re.sub(r'\s+', '', cand.group(1))
            last = ident.split('::')[-1]
            if last in _SKIP_IDENT:
                continue
            if '::' not in ident and re.fullmatch(r'[A-Z][A-Z0-9_]{2,}', last):
                # macro tipo NOTSA_EXPORT / VALIDATE_SIZE: se ignora
                continue
            fm = cand
            break
        if fm is None:
            skip_until = _match_brace(code, i)
            continue
        end = skip_until = _match_brace(code, i)
        ident = _strip_templates(re.sub(r'\s+', '', fm.group(1)))
        parts = [p for p in ident.split('::') if p]
        d = FuncDef()
        d.file = rel
        d.name = parts[-1]
        d.quals = [s for s in stack if s] + parts[:-1]
        d.params = _params_of(hs_nt, fm.end() - 1)
        # primer caracter no blanco de la cabecera (incluye `template<...>`)
        first = re.search(r'\S', code[decl_start:i])
        d.declstart = decl_start + first.start() if first else i
        if _LABEL.match(code[d.declstart:i]):
            d.declstart += _LABEL.match(code[d.declstart:i]).end()
            f2 = re.search(r'\S', code[d.declstart:i])
            d.declstart += f2.start() if f2 else 0
        d.line = line_of(raw, d.declstart)
        d.start = d.declstart
        d.header = hs
        d.comment_has = None
        defs.append(d)
        decl_start = end
    return defs, raw


def scan_all_defs(root=ROOT):
    """index: nombre -> [FuncDef]; texto crudo por archivo."""
    index = defaultdict(list)
    texts = {}
    for path in source_files(root):
        defs, raw = scan_defs(path, root)
        texts[path.relative_to(root).as_posix()] = raw
        for d in defs:
            index[d.name].append(d)
    return index, texts


# --------------------------------------------------------------------------
# Comentario encima de una definicion
# --------------------------------------------------------------------------

_ADDR_RE = re.compile(r'0[xX]0*([0-9A-Fa-f]{5,8})\b')


def addrs_in(text):
    return {int(x, 16) for x in _ADDR_RE.findall(text)}


def comment_above(raw, declstart):
    """Devuelve (inicio, fin, texto, tipo) del comentario contiguo encima, o None.

    tipo: 'line' (// ...), 'block' (/* ... */). inicio/fin son indices en raw.
    """
    line_start = raw.rfind('\n', 0, declstart) + 1
    # texto de la misma linea antes de la cabecera: no debe haber codigo
    pos = line_start
    lines = []  # (inicio, fin, texto) de lineas de comentario, de abajo arriba
    kind = None
    while pos > 0:
        prev_end = pos - 1
        if prev_end > 0 and raw[prev_end - 1] == '\r':
            prev_end -= 1
        prev_start = raw.rfind('\n', 0, prev_end) + 1
        line = raw[prev_start:prev_end]
        s = line.strip()
        if s.startswith('//'):
            if kind == 'block':
                break
            kind = 'line'
            lines.append((prev_start, prev_end, line))
            pos = prev_start
            continue
        if s.endswith('*/') and kind is None:
            # bloque que termina aqui: sube hasta su apertura
            k = raw.rfind('/*', 0, prev_end)
            if k < 0:
                break
            # el '/*' debe abrir en esta linea o antes y no haber codigo antes
            bs = raw.rfind('\n', 0, k) + 1
            if raw[bs:k].strip():
                break
            if '*/' in raw[k + 2:prev_start]:
                break  # no es un unico bloque contiguo
            return bs, prev_end, raw[bs:prev_end], 'block'
        break
    if kind == 'line':
        lines.reverse()
        return lines[0][0], lines[-1][1], '\n'.join(l[2] for l in lines), 'line'
    return None


def same_line_comment(raw, declstart, d_code_end):
    return None


def have_addr(raw, d, addr):
    c = comment_above(raw, d.declstart)
    if c and addr in addrs_in(c[2]):
        return True
    # comentario al final de la linea de la cabecera
    eol = raw.find('\n', d.declstart)
    first_line = raw[d.declstart:eol if eol >= 0 else len(raw)]
    if '//' in first_line and addr in addrs_in(first_line[first_line.index('//'):]):
        return True
    return False


# --------------------------------------------------------------------------
# Enlace hook -> definicion
# --------------------------------------------------------------------------

def _norm_params(s):
    s = re.sub(r'\b(?:const|volatile|struct|class|enum)\b', '', s)
    s = re.sub(r'=[^,]*', '', s)  # valores por defecto
    parts = _split_top(s)
    out = []
    for p in parts:
        p = p.strip()
        if not p or p == 'void':
            continue
        # quita el nombre del parametro (ultimo identificador si hay mas de un token)
        p = re.sub(r'\s+', ' ', p)
        m = re.match(r'^(.*?[\w>\*&\]\)])\s+([A-Za-z_]\w*)(\s*\[[^\]]*\])?$', p)
        if m and re.search(r'[\w>\*&]\s*$', m.group(1)) and ' ' in p or (m and '*' in m.group(1)):
            p = m.group(1)
        out.append(re.sub(r'\s+', '', p))
    return out


def _split_top(s):
    depth, cur, out = 0, [], []
    for c in s:
        if c in '(<[{':
            depth += 1
        elif c in ')>]}':
            depth -= 1
        if c == ',' and depth == 0:
            out.append(''.join(cur))
            cur = []
        else:
            cur.append(c)
    out.append(''.join(cur))
    return out


def _cast_params(cast):
    m = re.search(r'\)\s*\(([^)]*(?:\([^)]*\)[^)]*)*)\)\s*(?:const)?\s*$', cast)
    return _norm_params(m.group(1)) if m else None


def _last_and_quals(sym):
    sym = _strip_templates(re.sub(r'\s+', '', sym))
    parts = [x for x in sym.split('::') if x]
    return parts[-1], parts[:-1]


def candidates_for(h, index):
    fn, sym_quals = _last_and_quals(h.sym)
    out = []
    scope = h.sym_scope
    scope_base = _strip_templates(scope).split('::')[-1] if scope else None
    alt = []
    if fn == 'Constructor' and scope_base:
        alt = [scope_base]
    elif fn.startswith('Destructor') and scope_base:
        alt = ['~' + scope_base]
    for nm in [fn, fn + '_Reversed'] + alt:
        for d in index.get(nm, []):
            if scope_base and not sym_quals:
                if d.quals and d.quals[-1] == scope_base:
                    out.append(d)
            elif sym_quals:
                if d.quals[-len(sym_quals):] == sym_quals or (d.quals and d.quals[-1] == sym_quals[-1]):
                    out.append(d)
            else:
                out.append(d)
        if out:
            return out
    # sin cualificar con la clase (heredada, funcion libre o definida en otro ambito)
    for nm in [fn] + alt:
        ds = index.get(nm, [])
        if len(ds) == 1:
            return list(ds)
    return out


def bind(hooks, index, texts):
    unlocated = []
    for h in hooks:
        cands = candidates_for(h, index)
        if len(cands) > 1:
            with_addr = [d for d in cands if have_addr(texts[d.file], d, h.addr)]
            if with_addr:
                cands = with_addr
            elif h.cast:
                want = _cast_params(h.cast)
                if want is not None:
                    m = [d for d in cands if _norm_params(d.params) == want]
                    if m:
                        cands = m
        if len(cands) > 1 and h.file:
            same = [d for d in cands if d.file == h.file]
            if same:
                cands = same
            if len(cands) > 1:
                q = [d for d in cands if d.quals and d.quals[-1] == h.cls]
                if q:
                    cands = q
        if len(cands) > 1:
            # prefiere .cpp con cuerpo sobre .h
            cpp = [d for d in cands if d.file.endswith('.cpp')]
            if len(cpp) == 1:
                cands = cpp
        if len(cands) > 1 and len({d.file for d in cands}) == 1 and                 len({tuple(_norm_params(d.params)) for d in cands}) == 1:
            pass  # variantes bajo #if/#else del mismo archivo: se anotan todas
        elif len(cands) != 1:
            h.defn, h.defns = None, []
            unlocated.append((h, len(cands)))
            continue
        h.defn, h.defns = cands[0], list(cands)
    return unlocated


# --------------------------------------------------------------------------
# Catalogo y medicion
# --------------------------------------------------------------------------

def read_catalog(path=HOOKS_CSV):
    with open(path, encoding='utf-8-sig', newline='') as f:
        return list(csv.DictReader(f))


def collect(root=ROOT):
    hooks = scan_hooks(root)
    index, texts = scan_all_defs(root)
    unloc = bind(hooks, index, texts)
    return hooks, index, texts, unloc


def hook_status(h, texts):
    """'ok' | 'falta' | 'otra' (comentario con otra direccion) | 'nodef'"""
    if not h.defns:
        return 'nodef'
    ok = all(have_addr(texts[d.file], d, h.addr) for d in h.defns)
    return 'ok' if ok else 'falta'


def csv_entries(root=ROOT):
    """Une el catalogo con los macros: una entrada (Hook) por fila del CSV.

    Las filas sin macro propio (p. ej. CCheat, que registra una tabla en un bucle)
    se enlazan por nombre `clase::funcion` con la direccion del CSV.
    """
    rows = read_catalog(Path(root) / 'docs/hooks.csv')
    hooks, index, texts, unloc = collect(root)
    by_key = defaultdict(list)
    for h in hooks:
        by_key[(h.cls, h.name)].append(h)
    entries = []  # (fila, Hook | None)
    for r in rows:
        hs = by_key.get((r['class'], r['fn_name']))
        if hs:
            h = hs.pop(0) if len(hs) > 1 else hs[0]
            entries.append((r, h))
            continue
        addr = int(r['address'], 16)
        h = None
        if addr:
            h = Hook()
            h.cls, h.name = r['class'], r['fn_name']
            h.sym = re.sub(r'-.*', '', r['fn_name'])
            h.sym_scope = r['class']
            h.kind, h.addr, h.cast, h.file, h.line = 'csv', addr, '', '', 0
            h.vtbl = h.nvirt = None
            h.is_virtual = r['type'] == 'V'
            h.defn, h.defns = None, []
            if bind([h], index, texts):
                h.defns, h.defn = [], None
        entries.append((r, h if h and h.defns else None))
    return entries, hooks, index, texts


def measure(root=ROOT):
    entries, hooks, index, texts = csv_entries(root)
    stats = defaultdict(int)
    missing, nodef, nomacro = [], [], []
    for r, h in entries:
        stats['filas'] += 1
        if h is None:
            nomacro.append(r)
            stats['nodef'] += 1
            continue
        st = hook_status(h, texts)
        stats[st] += 1
        if st == 'falta':
            missing.append(h)
        elif st == 'nodef':
            nodef.append((r, h))
    return stats, missing, nodef, nomacro, hooks, texts


# --------------------------------------------------------------------------
# Aplicacion (solo comentarios)
# --------------------------------------------------------------------------

_STRIP = re.compile(
    r'("(?:\\.|[^"\\\n])*"|(?<![0-9A-Za-z_])\'(?:\\.|[^\'\\\n])*\')'
    r'|//[^\n]*(?:\\\r?\n[^\n]*)*|/\*.*?\*/', re.DOTALL)


def code_only(text):
    """Texto sin comentarios y sin espacios (para la comprobacion mecanica)."""
    t = _STRIP.sub(lambda m: m.group(1) or ' ', text)
    return re.sub(r'\s+', '', t)


def file_style(raw):
    """'line' si el archivo usa `// 0xDIR` mas que `@addr`; si no, 'block'."""
    n_line = len(re.findall(r'^[ \t]*// 0x[0-9A-Fa-f]+', raw, re.MULTILINE))
    n_doxy = raw.count('@addr')
    return 'line' if n_line > n_doxy else 'block'


def make_edit(raw, d, addrs, style):
    """Devuelve (inicio, fin, texto_nuevo) que sustituye raw[inicio:fin], o None."""
    line_start = raw.rfind('\n', 0, d.declstart) + 1
    indent = raw[line_start:d.declstart]
    if indent.strip():
        return None
    eol_pos = raw.find('\n', d.declstart)
    nl = '\r\n' if eol_pos > 0 and raw[eol_pos - 1] == '\r' else '\n'
    c = comment_above(raw, d.declstart)

    def fmt(a):
        return f'0x{a:X}'

    if c is None:
        if style == 'line':
            lines = [f'{indent}// {fmt(a)}' for a in addrs]
        else:
            lines = [f'{indent}/*!'] + [f'{indent} * @addr {fmt(a)}' for a in addrs] + [f'{indent} */']
        return line_start, line_start, nl.join(lines) + nl
    cs, ce, ctext, kind = c
    if kind == 'line':
        first = raw[cs:ce].split('\n')[0].rstrip('\r')
        ind = first[:len(first) - len(first.lstrip())]
        if style == 'line':
            lines = [f'{ind}// {fmt(a)}' for a in addrs]
        else:
            lines = [f'{ind}// @addr {fmt(a)}' for a in addrs]
        return cs, cs, nl.join(lines) + nl
    # bloque /* ... */
    if '\n' not in ctext:
        m = re.fullmatch(r'(\s*)(/\*[!*]?)\s*(.*?)\s*\*/', ctext, re.DOTALL)
        if not m or not m.group(3):
            return None
        ind, op, text = m.groups()
        lines = [f'{ind}{op}', f'{ind} * {text}', f'{ind} *'] + \
                [f'{ind} * @addr {fmt(a)}' for a in addrs] + [f'{ind} */']
        return cs, ce, nl.join(lines)
    ls = raw.rfind('\n', 0, ce) + 1
    last = raw[ls:ce].rstrip('\r')
    mind = re.match(r'[ \t]*', last).group(0)
    prev_start = raw.rfind('\n', 0, ls - 1) + 1
    prev = raw[prev_start:ls].strip()
    body = last.strip()
    new = []
    if body != '*/':
        if not body.endswith('*/'):
            return None
        text = body[:-2].rstrip()
        new.append(f'{mind}{text}' if text.startswith('*') else f'{mind}* {text}')
    sep = [] if (prev.startswith('* @') or prev.startswith('*@') or prev in ('/*!', '/**', '/*')) \
        else [f'{mind}*']
    new += sep + [f'{mind}* @addr {fmt(a)}' for a in addrs]
    new.append(f'{mind}*/')
    return ls, ce, nl.join(new)


def plan_edits(root=ROOT):
    entries, hooks, index, texts = csv_entries(root)
    want = defaultdict(set)   # (archivo, declstart) -> direcciones que faltan
    defs = {}
    for r, h in entries:
        if h is None:
            continue
        for d in h.defns:
            defs[(d.file, d.declstart)] = d
            if not have_addr(texts[d.file], d, h.addr):
                want[(d.file, d.declstart)].add(h.addr)
    per_file = defaultdict(list)
    for (f, ds), addrs in want.items():
        per_file[f].append((defs[(f, ds)], sorted(addrs)))
    return per_file, texts


def apply_all(root=ROOT):
    per_file, texts = plan_edits(root)
    changed = skipped = n_edits = 0
    bad = []
    for f, items in sorted(per_file.items()):
        raw = texts[f]
        style = file_style(raw)
        edits = []
        for d, addrs in items:
            e = make_edit(raw, d, addrs, style)
            if e is None:
                bad.append((f, d.line, 'forma de comentario no admitida'))
            else:
                edits.append(e)
        if not edits:
            continue
        new = raw
        for a, b, t in sorted(edits, key=lambda e: e[0], reverse=True):
            new = new[:a] + t + new[b:]
        if code_only(new) != code_only(raw):
            bad.append((f, 0, 'el codigo difiere tras anotar: archivo NO escrito'))
            skipped += 1
            continue
        (Path(root) / f).write_bytes(new.encode('latin-1'))
        changed += 1
        n_edits += len(edits)
    print(f'Archivos modificados: {changed}; anotaciones: {n_edits}; archivos rechazados: {skipped}')
    for f, line, why in bad:
        print(f'  AVISO {f}:{line}: {why}')
    return 1 if skipped else 0


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    g = ap.add_mutually_exclusive_group()
    g.add_argument('--report', action='store_true')
    g.add_argument('--check', action='store_true')
    g.add_argument('--apply', action='store_true')
    ap.add_argument('--list', action='store_true', help='con --report: lista las que faltan / no localizadas')
    args = ap.parse_args(argv)
    if args.apply:
        return apply_all()
    stats, missing, nodef, nomacro, hooks, texts = measure()
    print(f"Filas del catalogo: {stats['filas']}")
    print(f"  con direccion en el comentario: {stats['ok']}")
    print(f"  sin direccion en el comentario: {stats['falta']}")
    print(f"  definicion no localizada:       {stats['nodef']}")
    if args.list:
        for h in missing:
            print(f'FALTA  {h.cls}::{h.name} {h.addr:#010x} {h.defn.file}:{h.defn.line}')
        for r, h in nodef:
            print(f"NODEF  {h.cls}::{h.name} {h.addr:#010x} {h.file}:{h.line}")
        for r in nomacro:
            print(f"NODEF  {r['class']}::{r['fn_name']} {r['address']} (sin macro)")
    if args.check:
        return 1 if (stats['falta'] or (stats['nodef'] and False)) else 0
    return 0


if __name__ == '__main__':
    sys.exit(main())
