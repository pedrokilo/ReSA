# amplia typesize de estatico.py: constantes constexpr y enums (tamano por tipo subyacente)
import re, os, glob, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import estatico as E
CONST = {}
cre = re.compile(r'(?:constexpr|const)\s+(?:inline\s+)?(?:static\s+)?(?:[\w:]+\s+)+?(\w+)\s*=\s*([^;{}]+);')
txts = []
for p in glob.glob(os.path.join(E.SRC, '**', '*'), recursive=True):
    if p.endswith(('.h', '.hpp', '.inl', '.cpp')) and os.sep + 'DatosExe' + os.sep not in p:
        txts.append(E.strip_comments(open(p, encoding='utf-8', errors='replace').read()))
raw = {}
for t in txts:
    for m in cre.finditer(t): raw.setdefault(m.group(1), m.group(2).strip())
for t in txts:
    for m in re.finditer(r'(?<![\w])([A-Za-z_]\w*)\s*=\s*([^,;{}=\n]+?)\s*(?:,|\n|\})', t):
        k = m.group(1)
        if re.fullmatch(r'[A-Z][A-Z0-9_]{3,}', k): raw.setdefault(k, m.group(2).strip())
ENUM = {'uint8': 1, 'int8': 1, 'uint16': 2, 'int16': 2, 'uint32': 4, 'int32': 4, 'int': 4, 'unsigned': 4, 'char': 1, 'bool': 1, 'uint8_t': 1, 'int8_t': 1, 'uint16_t': 2, 'int16_t': 2}
ere = re.compile(r'\benum\s+(?:class\s+)?(\w+)\s*(?::\s*([\w:]+))?\s*\{')
esz = {}
for t in txts:
    for m in ere.finditer(t):
        esz.setdefault(m.group(1), ENUM.get((m.group(2) or 'int').split('::')[-1], 4))
def ev(expr, depth=0):
    if depth > 8: return None
    e = re.sub(r'\b(?:static_cast|reinterpret_cast)\s*<[^>]*>\s*\(', '(', expr)
    e = re.sub(r"(\d)'(\d)", r'\1\2', e); e = re.sub(r'(\b0x[0-9A-Fa-f]+|\b\d+)[uUlL]+\b', r'\1', e)
    def sub(m):
        n = m.group(0)
        if n in ('sizeof',): return n
        if n in raw:
            v = ev(raw[n], depth + 1)
            return str(v) if v is not None else 'None'
        return n
    e2 = re.sub(r'\b[A-Za-z_]\w*\b', sub, e)
    if re.search(r'[A-Za-z_]', e2.replace('0x', '').replace('0X', '')) and not re.fullmatch(r'[0-9a-fA-FxX+\-*/()<> %|&~ ]+', e2): return None
    try: return int(eval(e2.replace('/', '//'), {'__builtins__': {}}))
    except Exception: return None
orig = E.typesize
def typesize2(t):
    r = orig(t)
    if r is not None: return r
    t0 = re.sub(r'\bconst\b|\bvolatile\b', '', t).strip()
    t2 = t0.split('::')[-1].strip()
    if t2 in esz: return esz[t2]
    m = re.fullmatch(r'(?:std::array|notsa::mdarray)\s*<\s*(.+)\s*>', t0)
    if m:
        # separa argumentos a nivel 0
        args = []; d = 0; cur = ''
        for ch in m.group(1):
            if ch in '<(': d += 1
            if ch in '>)': d -= 1
            if ch == ',' and d == 0: args.append(cur.strip()); cur = ''
            else: cur += ch
        args.append(cur.strip())
        el = typesize2(args[0]); n = 1
        for a in args[1:]:
            v = ev(a)
            if v is None: return None
            n *= v
        return el * n if el else None
    m = re.fullmatch(r'(.+?)((?:\[[^\]]+\])+)', t0)
    if m:
        el = typesize2(m.group(1)); n = 1
        for dm in re.findall(r'\[([^\]]+)\]', m.group(2)):
            v = ev(dm)
            if v is None: return None
            n *= v
        return el * n if el else None
    return None
E.typesize = typesize2
E.typesize2 = typesize2
E.ev = ev
E.CONSTS = raw
E.ESZ = esz
