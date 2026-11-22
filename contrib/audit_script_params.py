#!/usr/bin/env python3
"""Auditoria de PARAMETROS de los comandos de script (0..2638): EXE contra C++.

Los 27 despachadores `CRunningScript::ProcessCommands<N>To<N+99>` ya estan en C++. contrib/audit_script_dispatchers.py comprueba que
cada comando que el EXE atiende tenga caso o manejador. Esta herramienta comprueba que el caso (o manejador) C++ consuma del script
EXACTAMENTE los mismos parametros, y en el mismo orden, que el caso del EXE. Si el C++ lee menos/mas/otro tipo, la IP del script se
desincroniza ('Unknown param type: 64').

EXE: por cada comando se localiza su caso (tabla 0x8A6168 + tabla de saltos del switch; se sigue el reenvio a otro despachador) y se
recorre el grafo de control (capstone sobre el PE) anotando las llamadas, en orden, a las rutinas que consumen parametros:
    C(n) CollectParameters(n)            -> n valores        ('a')
    V    GetPointerToScriptVariable      -> 1 variable       ('v')
    T    ReadTextLabelFromScript         -> 1 texto          ('t')
    S(n) StoreParameters(n)              -> n variables      ('v')
    G    GetIndexOfGlobalVariable        -> 1 variable       ('v')
    N    ReadParametersForNewlyStartedScript (cuenta variable, '#')
    U    UpdatePC (salto de IP)
y, expandidas, las ayudas Locate*/Char*InArea*/PlayAnimScriptCommand/ScriptTaskPickUpObject que leen parametros por dentro.
Un caso con ramas da varias secuencias posibles (todas se guardan). Jump tables internas, bucles y desbordes: 'J', 'L', '*'.
El resultado del EXE se cachea en docs/CodigoVivoExe/ejecucion-auditoria-params-exe.csv (--regenerar-exe lo rehace; requiere el EXE).

C++: (a) manejador registrado con REGISTER_COMMAND_HANDLER(cmd, fn): si existe docs/CodigoVivoExe/ejecucion-auditoria-params-cpp-sig.csv
(volcado EXACTO de notsa::script::sig::Table() que hace el juego con GTASA_DUMP_SIGS=<archivo>) se usa; si no, firma de fn por texto con las
reglas de Signature.hpp (CVector=3, CVector2D=2, CRect=4, CRGBA=4, CRunningScript&/eScriptCommands=0, resto=1; valor devuelto = variables
escritas; los `auto` sin resolver quedan en gravedad 3). (b) sin manejador, el `case` del despachador: se recorre el texto C++ con ramas
(`if`/bucles/`return` anticipado), se expanden las ayudas definidas en source/game_sa/Scripts (p. ej. CreateFxOnEntity) y se obtiene el
conjunto de rutas posibles de lectura.

Gravedad: 1 = ninguna ruta C++ coincide con ninguna del EXE y cambia la cantidad de unidades (desincroniza); 2 = misma cantidad pero otro
orden de a/v/t (suele ser equivalente); 3 = no resuelto por la herramienta (revisar a mano); 4 = el EXE atiende el comando y no hay C++.
Codigo de salida 1 si hay gravedad 1.

Uso: python contrib/audit_script_params.py [--exe gta-sa.exe] [--regenerar-exe] [--csv salida.csv] [--solo N,M] [--sin-firmas]
"""
import csv, os, re, struct, sys, threading
from collections import Counter
import capstone

RAIZ = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
EXE = 'D:/Grand Theft Auto San Andreas/gta-sa.exe'
if '--exe' in sys.argv: EXE = sys.argv[sys.argv.index('--exe') + 1]
SCR = os.path.join(RAIZ, 'source/game_sa/Scripts')
CACHE = os.path.join(RAIZ, 'docs/CodigoVivoExe/ejecucion-auditoria-params-exe.csv')
NCMD = 2639
sys.setrecursionlimit(100000)

# ----------------------------------------------------------------------------------------------------------- EXE
READERS = {0x464080: 'C', 0x464370: 'S', 0x463D50: 'T', 0x464790: 'V', 0x464700: 'G', 0x464500: 'N', 0x464DA0: 'U'}
WRAP = {0x470150: 'PlayAnimScriptCommand', 0x487A20: 'LocateCarCommand', 0x486D80: 'LocateCharCommand', 0x487D10: 'LocateObjectCommand',
        0x487420: 'LocateCharCarCommand', 0x4870F0: 'LocateCharCharCommand', 0x487720: 'LocateCharObjectCommand', 0x488EC0: 'CarInAreaCheckCommand',
        0x488B50: 'CharInAreaCheckCommand', 0x489150: 'ObjectInAreaCheckCommand', 0x487F60: 'CharInAngledAreaCheckCommand',
        0x488780: 'FlameInAngledAreaCheckCommand', 0x4883F0: 'ObjectInAngledAreaCheckCommand', 0x46AF50: 'ScriptTaskPickUpObject'}
WRAP_BY_NAME = {v: k for k, v in WRAP.items()}
MAXP = 48


class Exe:
    def __init__(self, path):
        d = self.d = open(path, 'rb').read()
        pe = struct.unpack_from('<I', d, 0x3c)[0]; ns = struct.unpack_from('<H', d, pe + 6)[0]; so = struct.unpack_from('<H', d, pe + 20)[0]
        self.secs = []
        for i in range(ns):
            n, vs, va, rs, rp = struct.unpack_from('<8sIIII', d, pe + 24 + so + i * 40)
            self.secs.append((va, vs, rs, rp))
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); self.md.detail = False
        self.ins = {}; self.memo = {}; self.busy = set(); self.seed = {}
        self.disps = {self.u32(0x8A6168 + 4 * k) for k in range(27)}

    def off(self, a):
        r = a - 0x400000
        for va, vs, rs, rp in self.secs:
            if va <= r < va + max(vs, rs): return rp + r - va
        raise ValueError(hex(a))

    def u32(self, a): return struct.unpack_from('<I', self.d, self.off(a))[0]
    def u8(self, a): return self.d[self.off(a)]

    def fetch(self, a):
        if a not in self.ins:
            o = self.off(a)
            for i in self.md.disasm(self.d[o:o + 96], a):
                self.ins.setdefault(i.address, (i.mnemonic, i.op_str, i.size))
            if a not in self.ins: self.ins[a] = ('(bad)', '', 1)
        return self.ins[a]

    def switch_of(self, addr):
        o = self.off(addr)
        ins = list(self.md.disasm(self.d[o:o + 0x300], addr))
        ja = next((k for k, i in enumerate(ins[:70]) if i.mnemonic == 'ja'), None)
        if ja is None: return None
        dflt = int(ins[ja].op_str, 0)
        kc = max(k for k in range(ja) if ins[k].mnemonic == 'cmp')
        reg, imm = [x.strip() for x in ins[kc].op_str.split(',')]; ncmp = int(imm, 0)
        base = 0
        for i in ins[:kc]:
            m = re.match(r'%s, \[e[a-z]{2} - (0x[0-9a-f]+)\]$' % reg, i.op_str) if i.mnemonic == 'lea' else None
            if m: base = int(m.group(1), 16)
            if i.mnemonic == 'add' and i.op_str.startswith(reg + ', '):
                try: v = int(i.op_str.split(',')[1], 0)
                except ValueError: continue
                if v < 0: base = -v
                elif v >= 0xffff0000: base = (1 << 32) - v
            if i.mnemonic == 'sub' and i.op_str.startswith(reg + ', '):
                try: base = int(i.op_str.split(',')[1], 0)
                except ValueError: pass
        idx = jt = None
        for i in ins[ja + 1:ja + 6]:
            if i.mnemonic == 'movzx' and 'byte ptr' in i.op_str:
                m = re.search(r'\+ (0x[0-9a-f]+)\]', i.op_str); idx = int(m.group(1), 16)
            if i.mnemonic == 'jmp' and '*4' in i.op_str:
                m = re.search(r'\+ (0x[0-9a-f]+)\]', i.op_str); jt = int(m.group(1), 16); break
        if jt is None: return None
        return ncmp, base, idx, jt, dflt

    def case_dest(self, cmd, disp, depth=0):
        sw = self.switch_of(disp)
        if sw is None: return None
        ncmp, base, idx, jt, dflt = sw
        v = cmd - base
        if v < 0 or v > ncmp: return None
        ti = self.u8(idx + v) if idx else v
        dest = self.u32(jt + 4 * ti)
        if dest == dflt: return None
        a = dest; seq = []
        for _ in range(3):
            m, o, s = self.fetch(a); seq.append((m, o)); a += s
        if seq[0] == ('push', 'eax') and seq[1] == ('mov', 'ecx, esi') and seq[2][0] == 'call' and depth < 4:
            nxt = int(seq[2][1], 0)
            if nxt in self.disps: return self.case_dest(cmd, nxt, depth + 1)
        return dest

    REGS = ('eax', 'ebx', 'ecx', 'edx', 'esi', 'edi', 'ebp')

    def run_tokens(self, a, regs0, st0=()):
        """recorre en linea recta; sigue constantes en registros (mov reg, imm / xor reg, reg) para resolver `push reg`."""
        toks = []; recent = list(st0); regs = dict(regs0)
        while True:
            m, o, s = self.fetch(a)
            ops = [x.strip() for x in o.split(',')] if o else []
            if m == 'push':
                v = ops[0]
                try: recent.append(int(v, 0))
                except ValueError: recent.append(regs.get(v, v))
                recent = recent[-6:]
            elif m in ('mov', 'xor', 'lea', 'add', 'sub', 'pop', 'movzx', 'movsx', 'and', 'or', 'inc', 'dec', 'imul', 'shl', 'shr', 'sar', 'neg', 'not', 'xchg') and ops and ops[0] in self.REGS:
                if m == 'mov' and len(ops) == 2:
                    try: regs[ops[0]] = int(ops[1], 0)
                    except ValueError: regs.pop(ops[0], None)
                elif m == 'xor' and len(ops) == 2 and ops[0] == ops[1]: regs[ops[0]] = 0
                else: regs.pop(ops[0], None)
            if m == 'call':
                try: t = int(o, 0)
                except ValueError: t = None
                if t in READERS:
                    k = READERS[t]
                    if k in 'CSU':
                        n = recent[-1] if recent and isinstance(recent[-1], int) else '?'
                        toks.append((k, n))
                    else: toks.append((k, None))
                elif t in WRAP: toks.append(('W', t))
                recent = []
                for r in ('eax', 'ecx', 'edx'): regs.pop(r, None)
            elif m in ('ret', 'retn'): return toks, 'ret', None, regs, recent
            elif m == 'jmp':
                try: return toks, 'jmp', int(o, 0), regs, recent
                except ValueError: return toks, 'ijmp', None, regs, recent
            elif m.startswith('j'):
                try: tgt = int(o, 0)
                except ValueError: return toks, 'ijmp', None, regs, recent
                return toks, 'jcc', (tgt, a + s), regs, recent
            elif m == '(bad)': return toks, 'bad', None, regs, recent
            a += s

    def paths(self, a, regs0=None, st0=()):
        regs0 = self.seed if regs0 is None else regs0
        key = (a, tuple(sorted(regs0.items())), tuple(st0))
        if key in self.memo: return self.memo[key]
        if key in self.busy: return {(('L', None),)}
        self.busy.add(key)
        toks, kind, data, regs, stk = self.run_tokens(a, regs0, st0)
        pref = {()}
        for t in toks:
            if t[0] == 'W':
                wp = self.paths(t[1], {}, ()); pref = {p + w for p in pref for w in wp}
            else:
                pref = {p + (t,) for p in pref}
            if len(pref) > MAXP: pref = {p + (('*', None),) for p in sorted(pref)[:MAXP]}
        keep = {r: v for r, v in regs.items() if r in ('ebx', 'esi', 'edi', 'ebp')}
        if kind in ('ret', 'bad'): succ = {()}
        elif kind == 'jmp': succ = self.paths(data, keep, tuple(stk))
        elif kind == 'jcc': succ = self.paths(data[0], keep, tuple(stk)) | self.paths(data[1], keep, tuple(stk))
        else: succ = {(('J', None),)}
        res = {p + s for p in pref for s in succ}
        if len(res) > MAXP: res = set(sorted(res)[:MAXP]) | {(('*', None),)}
        self.busy.discard(key); self.memo[key] = res
        return res

    def seed_of(self, disp):
        """constantes que el prologo del despachador deja en ebx/edi/ebp antes del switch."""
        o = self.off(disp); regs = {}
        for i in self.md.disasm(self.d[o:o + 0x80], disp):
            if i.mnemonic == 'mov' and i.op_str.split(',')[0] in ('ebx', 'edi', 'ebp'):
                try: regs[i.op_str.split(',')[0]] = int(i.op_str.split(',')[1], 0)
                except ValueError: regs.pop(i.op_str.split(',')[0], None)
            if i.mnemonic in ('ja', 'jmp') : break
        return regs


def unidades(seq):
    out = []
    for k, n in seq:
        if k == 'C': out.append('a' * n if isinstance(n, int) else '#')
        elif k == 'S': out.append('v' * n if isinstance(n, int) else '#')
        elif k in 'VG': out.append('v')
        elif k == 'T': out.append('t')
        elif k == 'N': out.append('#')
        else: out.append(k)
    return ''.join(out)


def extraer_exe(exe):
    tabla = [exe.u32(0x8A6168 + 4 * k) for k in range(27)]
    filas = []
    for cmd in range(NCMD):
        dest = exe.case_dest(cmd, tabla[cmd // 100])
        exe.seed = exe.seed_of(tabla[cmd // 100])
        if dest is None:
            filas.append([cmd, '', '', 'no_atendido']); continue
        ps = sorted({unidades(p) for p in exe.paths(dest)}, key=lambda s: (len(s), s))
        filas.append([cmd, hex(dest), '|'.join(ps), 'ok'])
    return filas


# ----------------------------------------------------------------------------------------------------------- C++
def limpiar(src):
    src = src.replace('\r\n', '\n')
    out = []; i = 0; n = len(src)
    while i < n:
        c = src[i]
        if src.startswith('//', i):
            j = src.find('\n', i); i = n if j < 0 else j; continue
        if src.startswith('/*', i):
            j = src.find('*/', i + 2); j = n if j < 0 else j + 2
            out.append('\n' * src.count('\n', i, j)); i = j; continue
        if c == '"':
            j = i + 1
            while j < n and src[j] != '"': j += 2 if src[j] == '\\' else 1
            out.append('""'); i = j + 1; continue
        if c == "'" and i + 2 < n:
            q = i + 1 + (1 if src[i + 1] == '\\' else 0) + 1
            j = src.find("'", q - 1)
            if 0 < j - i <= 6: out.append("' '"); i = j + 1; continue
        out.append(c); i += 1
    return ''.join(out)


def pareja(s, i, a, b):
    d = 0
    for j in range(i, len(s)):
        if s[j] == a: d += 1
        elif s[j] == b:
            d -= 1
            if d == 0: return j
    return -1


def partir_args(s):
    out = []; d = 0; cur = ''
    for ch in s:
        if ch in '<([{': d += 1
        elif ch in '>)]}': d -= 1
        if ch == ',' and d == 0: out.append(cur.strip()); cur = ''
        else: cur += ch
    if cur.strip(): out.append(cur.strip())
    return out


TIPO_CVEC = {'CVector': 3, 'CVector2D': 2, 'CRect': 4, 'CRGBA': 4}
NO_LEE = {'CRunningScript', 'eScriptCommands'}


def uds_tipo(t):
    t = re.sub(r'\b(const|volatile|struct|class|enum)\b', ' ', t)
    t = re.sub(r'\s+', ' ', t).strip().rstrip('&*').strip()
    if '<' not in t: t = t.split('::')[-1]
    if t in NO_LEE: return 0
    return TIPO_CVEC.get(t, 1)


def quitar_defecto(a):
    d = 0
    for i, ch in enumerate(a):
        if ch in '<([{': d += 1
        elif ch in '>)]}': d -= 1
        elif ch == '=' and d == 0: return a[:i]
    return a


def tipo_de_param(a):
    a = quitar_defecto(a).strip()
    mm = re.match(r'^(.*[\s&*>])([A-Za-z_]\w*)$', a)
    if mm and mm.group(2) not in ('unsigned', 'signed', 'long', 'short', 'int', 'char', 'float', 'double', 'bool') and mm.group(1).strip() not in ('const', 'unsigned', 'signed', 'long', 'short'):
        return mm.group(1).strip()
    return a


def args_a_n(args):
    n = 0
    for a in args:
        a = a.strip()
        if a in ('', 'void'): continue
        n += uds_tipo(tipo_de_param(a))
    return n


def uds_ret_elem(r):
    r = re.sub(r'\b(const|volatile)\b', ' ', r); r = re.sub(r'\s+', ' ', r).strip().rstrip('&').strip()
    base = r.split('::')[-1]
    if base in ('CVector', 'CVector2D'): return TIPO_CVEC[base]
    if base == 'void': return 0
    return 1


def uds_retorno(r):
    r = re.sub(r'\b(const|inline|static|constexpr)\b', ' ', r.strip()).strip()
    if r in ('void', ''): return 0
    if r.split('::')[-1] in ('bool', 'CompareFlagUpdate', 'OpcodeResult'): return 0
    m = re.match(r'^(?:std::tuple|notsa::script::MultiRet|script::MultiRet|MultiRet)\s*<(.*)>$', r, re.S)
    if m: return sum(uds_ret_elem(x) for x in partir_args(m.group(1)))
    if r == 'auto' or r.startswith('decltype'): return None
    return uds_ret_elem(r)


class Cpp:
    def __init__(self):
        self.files = {}
        for dp, _, fs in os.walk(SCR):
            for fn in fs:
                if fn.endswith(('.cpp', '.hpp', '.h')) and 'CLEO' not in dp:
                    p = os.path.join(dp, fn)
                    self.files[p] = limpiar(open(p, encoding='utf-8', errors='replace').read())
        self.cmds = {}
        with open(os.path.join(RAIZ, 'docs/ComandosScript.csv'), encoding='utf-8-sig') as f:
            for r in csv.DictReader(f): self.cmds[r['comando']] = int(r['id'])
        # docs/ComandosScript.csv numera la cola con el enum de Xbox (IS_XBOX_PLAYER2_PRESSING_START=2638); en PC DO_DEBUG_STUFF es el 2638 (caso 0x47A69C del EXE)
        self.cmds['COMMAND_DO_DEBUG_STUFF'] = 2638
        self.nombre = {v: k for k, v in self.cmds.items()}
        rs = self.files[os.path.join(SCR, 'RunningScript.cpp')]
        self.llamados = set(re.findall(r'^\s*c::(\w+)::RegisterHandlers\(\);', rs, re.M))
        self.handlers = {}
        for p, s in self.files.items():
            if os.sep + 'Commands' + os.sep not in p or not p.endswith('.cpp'): continue
            m = re.search(r'([\w:]+)::RegisterHandlers\(\)\s*\{', s)
            if m: nsn = m.group(1).split('::')[-1]
            else:
                m = re.search(r'void\s+RegisterHandlers\(\)', s)
                if not m: continue
                nsn = [x.split('::')[-1] for x in re.findall(r'namespace\s+([\w:]+)\s*\{', s[:m.start()])][-1]
            for m in re.finditer(r'REGISTER_COMMAND_(HANDLER|NOP|UNIMPLEMENTED)\s*\(\s*(COMMAND_\w+)\s*(?:,|\))', s):
                j = s.find('(', m.start()); k = pareja(s, j, '(', ')')
                parts = partir_args(s[j + 1:k])
                cid = self.cmds.get(m.group(2))
                if cid is None: continue
                self.handlers[cid] = (m.group(1), parts[1:], p, s, m.start(), nsn)
        self.casos = {}
        for k in range(27):
            for p, s in self.files.items():
                m = re.search(r'OpcodeResult CRunningScript::ProcessCommands%dTo%d\(' % (k * 100, k * 100 + 99), s)
                if not m: continue
                i = s.find('{', s.find(')', m.end()))
                j = pareja(s, i, '{', '}')
                self.parse_switch(s[i + 1:j], k, p)
                break

    def parse_switch(self, body, k, p):
        sw = re.search(r'switch\s*\([^)]*\)\s*\{', body)
        if not sw: return
        j0 = pareja(body, sw.end() - 1, '{', '}')
        sb = body[sw.end():j0]
        d = 0; marks = []; i = 0; n = len(sb)
        while i < n:
            c = sb[i]
            if c == '{': d += 1
            elif c == '}': d -= 1
            elif d == 0:
                m = re.match(r'(case\s+[^:;{}]+:|default\s*:)', sb[i:])
                if m and (i == 0 or not (sb[i - 1].isalnum() or sb[i - 1] == '_')):
                    marks.append((i, i + m.end(), m.group(1))); i += m.end(); continue
            i += 1
        group = []
        for idx, (a, b, lab) in enumerate(marks):
            nxt = marks[idx + 1][0] if idx + 1 < len(marks) else len(sb)
            if lab.startswith('case'):
                e = lab[4:-1].strip()
                if e.startswith('COMMAND_'): cid = self.cmds.get(e)
                else:
                    try: cid = int(e, 0)
                    except ValueError: cid = None
                if cid is not None: group.append(cid)
            texto = sb[b:nxt]
            if texto.strip():
                for cid in group: self.casos[cid] = (texto, p, k)
                group = []

    def firma_manejador(self, cid):
        macro, parts, p, s, pos, nsn = self.handlers[cid]
        if macro == 'UNIMPLEMENTED': return None, 'unimpl'
        if macro == 'NOP': return args_a_n(parts), 'nop'
        expr = parts[0].strip() if parts else ''
        if expr.startswith('['):
            m = re.match(r'\[[^\]]*\]\s*(?:<[^>]*>)?\s*\((.*?)\)\s*(?:mutable\s*)?(?:->\s*([^{]+?))?\s*\{', expr, re.S)
            if not m: return None, 'lambda?'
            ret = m.group(2)
            if ret is None: ret = 'void' if not re.search(r'\breturn\b', expr[m.end():]) else 'auto'
            return self.sumar(partir_args(m.group(1)), ret)
        name = expr.lstrip('&(').strip()
        if name.startswith('::'): name = name[2:]
        m = re.match(r'^\(?\s*([\w:]+)\s*<(.*)>\s*\)?$', expr, re.S)
        if m and m.group(1).endswith('NOPCommandHandler'): return args_a_n(partir_args(m.group(2))), 'nop'
        if name.endswith('NotImplemented'): return None, 'unimpl'
        short = name.split('::')[-1].split('<')[0].rstrip(')')
        for pp, ss in [(p, s)] + [(q, t) for q, t in self.files.items() if q != p]:
            for m in re.finditer(r'(?<![\w:.>])((?:[\w:<>,\*&\s]|\[\[[^\]]*\]\])*?)\b%s\s*\(' % re.escape(short), ss):
                j = ss.find('(', m.end() - 1); k = pareja(ss, j, '(', ')')
                if k < 0: continue
                m2 = re.match(r'\s*(?:noexcept\s*)?(?:->\s*([^{;]+?))?\s*\{', ss[k + 1:k + 200])
                if not m2: continue
                pre = m.group(1).strip().split('\n')[-1]
                pre = re.sub(r'^.*[;}]\s*', '', pre).strip()
                pre = re.sub(r'\b(static|inline|constexpr|template\s*<[^>]*>|NOTSA_NOINLINE|extern)\b', ' ', pre).strip()
                return self.sumar(partir_args(ss[j + 1:k]), m2.group(1) if m2.group(1) else pre)
        return None, 'sin_def:' + short

    def sumar(self, args, ret):
        nin = args_a_n(args)
        nout = uds_retorno(ret)
        if nout is None: return None, 'ret_auto'
        return nin + nout, 'ok:%d+%d' % (nin, nout)


KEYWORDS = {'if', 'for', 'while', 'switch', 'catch', 'return', 'sizeof', 'else', 'do', 'static_assert', 'decltype', 'alignof', 'defined'}
READER_NAMES = ('CollectParameters', 'StoreParameters', 'GetPointerToScriptVariable', 'ReadTextLabelFromScript', 'ReadParametersForNewlyStartedScript', 'UpdatePC', 'GetIndexOfGlobalVariable')


def indice_defs(files):
    """nombre -> [(ret_texto, args_texto, cuerpo, fichero)] de las funciones definidas en `files`."""
    defs = {}
    rx = re.compile(r'\)\s*(?:const\s*)?(?:noexcept\s*)?(?:override\s*)?(?:->\s*[^{;()]+?)?\s*\{')
    for p, s in files.items():
        pos = 0
        while True:
            m = rx.search(s, pos)
            if not m: break
            close = m.start()
            d = 0; j = close
            while j >= 0:
                if s[j] == ')': d += 1
                elif s[j] == '(':
                    d -= 1
                    if d == 0: break
                j -= 1
            if j < 0: pos = m.end(); continue
            k = j - 1
            while k >= 0 and s[k].isspace(): k -= 1
            e = k + 1
            while k >= 0 and (s[k].isalnum() or s[k] in '_:~'): k -= 1
            nombre = s[k + 1:e]
            if not nombre or nombre.split('::')[-1] in KEYWORDS or not re.match(r'^[A-Za-z_~][\w:]*$', nombre) or (k >= 0 and s[k] in '.>]=,(&|!+-*/<?'):
                pos = m.end(); continue
            q = k
            while q >= 0 and s[q] not in ';{}': q -= 1
            cab = s[q + 1:k + 1].strip()
            cab = re.sub(r'\[\[[^\]]*\]\]', ' ', cab)
            cab = re.sub(r'\b(static|inline|constexpr|extern|NOTSA_NOINLINE|virtual|template\s*<[^>]*>)\b', ' ', cab).strip()
            tr = re.search(r'->\s*([^{;()]+?)\s*\{$', m.group(0))
            ret = tr.group(1).strip() if tr else cab
            fin = pareja(s, m.end() - 1, '{', '}')
            if fin < 0: pos = m.end(); continue
            defs.setdefault(nombre.split('::')[-1], []).append((ret, s[j + 1:close], s[m.end():fin], p))
            pos = fin + 1
        # lambdas con nombre (`const auto X = [&](...) -> R { ... };`), tambien dentro de cuerpos de funcion
        for m in re.finditer(r'(\w+)\s*=\s*\[[^\]]*\]\s*\(', s):
            j = m.end() - 1; k = pareja(s, j, '(', ')')
            if k < 0: continue
            m2 = re.match(r'\s*(?:mutable\s*)?(?:->\s*([^{;]+?))?\s*\{', s[k + 1:k + 200])
            if not m2: continue
            ini = k + 1 + m2.end() - 1
            fin = pareja(s, ini, '{', '}')
            if fin < 0: continue
            defs.setdefault(m.group(1), []).append((m2.group(1) or 'auto', s[j + 1:k], s[ini + 1:fin], p))
    return defs


class Rutas:
    """rutas de lectura de un trozo de C++ (cadenas de unidades) con expansion de ayudas definidas en Scripts/."""
    def __init__(self, defs, solo_ayudas=None):
        self.defs = defs; self.cache = {}; self.busy = set(); self.flags = set()
        names = [n for n in defs if n not in READER_NAMES and n not in KEYWORDS and (solo_ayudas is None or n in solo_ayudas)]
        self.rx_help = re.compile(r'\b(%s)\s*\(' % '|'.join(sorted(map(re.escape, names), key=len, reverse=True))) if names else None
        self.rx_read = re.compile(r'\b(CollectParameters|StoreParameters|GetPointerToScriptVariable|GetIndexOfGlobalVariable|ReadTextLabelFromScript|ReadParametersForNewlyStartedScript|UpdatePC)\s*\(')
        self.rx_gen = re.compile(r'\bRead<([^()]+?)>\s*\(\s*(?:this|S|script)\s*\)')

    def eventos(self, seg, fich=None):
        ev = []
        for m in self.rx_read.finditer(seg):
            nm = m.group(1); j = seg.find('(', m.start()); k = pareja(seg, j, '(', ')')
            arg = seg[j + 1:k].strip() if k > 0 else ''
            if nm in ('CollectParameters', 'StoreParameters', 'UpdatePC'):
                try: n = int(arg, 0)
                except ValueError: n = None
                if nm == 'UpdatePC': u = 'U'
                else: u = ('a' if nm == 'CollectParameters' else 'v') * n if n is not None else '#'
            elif nm in ('GetPointerToScriptVariable', 'GetIndexOfGlobalVariable'): u = 'v'
            elif nm == 'ReadTextLabelFromScript': u = 't'
            else: u = '#'
            ev.append((m.start(), {u}))
        for m in self.rx_gen.finditer(seg):
            ev.append((m.start(), {'a' * uds_tipo(m.group(1))}))
        if self.rx_help:
            for m in self.rx_help.finditer(seg):
                pre = seg[:m.start(1)]
                if re.search(r'(\.|->)\s*$', pre) and not re.search(r'\b(this|S|script)\s*(\.|->)\s*$', pre): continue
                if re.search(r'::\s*$', pre) and not re.search(r'CRunningScript::\s*$', pre): continue
                ev.append((m.start(), self.de_funcion(m.group(1), fich)))
        for m in re.finditer(r'\breturn\b', seg):
            e = seg.find(';', m.end()); e = len(seg) if e < 0 else e
            q = max(seg.rfind(';', 0, m.start()), seg.rfind('{', 0, m.start()), seg.rfind('}', 0, m.start()))
            cond = bool(re.search(r'\b(if|else|for|while)\b', seg[q + 1:m.start()]))
            ev.append((e + 0.5, ('RET', cond)))
        ev.sort(key=lambda x: x[0])
        return ev

    def de_funcion(self, nm, fich=None):
        cands = self.defs.get(nm, [])
        if fich is not None and any(c[3] == fich for c in cands): cands = [c for c in cands if c[3] == fich]
        key = (nm, fich if fich is not None else None)
        if key in self.cache: return self.cache[key]
        if key in self.busy: return {''}   # recursion (p. ej. el manejador SetCharCoordinates llama a CRunningScript::SetCharCoordinates): nada
        self.busy.add(key)
        conjuntos = [frozenset(self.rutas(body, c[3])) for c in cands for body in [c[2]]]
        self.busy.discard(key)
        if not conjuntos: r = {''}
        elif len(set(conjuntos)) == 1: r = set(conjuntos[0])
        else: r = set().union(*conjuntos) | {'#'}
        self.cache[key] = r
        return r

    def rutas(self, texto, fich=None):
        live, done = self.walk(texto, fich)
        return live | done

    def aplicar(self, seg, live, done, fich=None):
        for _, tok in self.eventos(seg, fich):
            if isinstance(tok, tuple):
                done = done | live
                if not tok[1]: live = set()
            else:
                live = {p + t for p in live for t in tok}
                if len(live) > 150: live = set(sorted(live)[:150])
        return live, done

    def walk(self, texto, fich=None):
        live = {''}; done = set()
        pos = 0; seg0 = 0; n = len(texto)
        while pos < n:
            if texto[pos] == '{':
                seg = texto[seg0:pos]
                live, done = self.aplicar(seg, live, done, fich)
                fin = pareja(texto, pos, '{', '}')
                if fin < 0: fin = n - 1
                tail = seg[max(seg.rfind(';'), seg.rfind('}')) + 1:]
                lam = '[' in tail and bool(re.search(r'\]\s*(\([^)]*\))?\s*(mutable\s*)?(->[^{]*)?$', tail))
                if not lam and live:
                    cond = bool(re.search(r'\b(if|else|for|while|switch|do)\b', tail))
                    if re.search(r'\b(for|while|do)\b', tail): self.flags.add('bucle')
                    if re.search(r'\bswitch\b', tail): self.flags.add('switch')
                    slive, sdone = self.walk(texto[pos + 1:fin], fich)
                    done = done | {p + t for p in live for t in sdone}
                    if cond: live = live | {p + t for p in live for t in slive}
                    else: live = {p + t for p in live for t in slive}
                    if len(live) > 150: live = set(sorted(live)[:150])
                pos = fin + 1; seg0 = pos
            else:
                pos += 1
        if seg0 < n and live: live, done = self.aplicar(texto[seg0:], live, done, fich)
        return live, done


def norm(s): return s.replace('U', '')


def _coincide(c, e):
    """c, e: rutas con '#' (cuenta variable = una o mas 'a'); cierto si pueden ser la misma cadena."""
    if '#' not in c and '#' not in e: return c == e
    pc = re.escape(c).replace(r'\#', 'a+') if '#' in c else re.escape(c)
    pe = re.escape(e).replace(r'\#', 'a+') if '#' in e else re.escape(e)
    if '#' in c and '#' in e: return re.fullmatch(pc, e.replace('#', 'a')) is not None or re.fullmatch(pe, c.replace('#', 'a')) is not None
    return re.fullmatch(pc, e) is not None if '#' in c else re.fullmatch(pe, c) is not None


def comparar_rutas(cpp_rutas, exe_rutas):
    cpp_n = {norm(r) for r in cpp_rutas}
    exe_n = {norm(r) for r in exe_rutas}
    exe_limpias = {r for r in exe_n if not any(c in r for c in 'JL*')}
    if not exe_limpias: return 3, 'exe_sin_ruta_limpia'
    if cpp_n & exe_limpias: return 0, 'igual'
    if any(_coincide(c, e) for c in cpp_n for e in exe_limpias): return 0, 'igual_cuenta_variable'
    if any('#' in r for r in cpp_n | exe_limpias): return 3, 'cuenta_variable'
    mismos = [(c, e) for c in cpp_n for e in exe_limpias if len(c) == len(e)]
    if mismos:
        # a/v (valor frente a variable) y t/v (texto frente a variable de texto) consumen el mismo parametro; t frente a a cambia el formato del parametro
        if any(not any((x == 't') != (y == 't') and 't' in (x, y) and (x in 'at' and y in 'at') for x, y in zip(c, e)) for c, e in mismos):
            return 0, 'equivalente_a_v'
        return 2, 'texto_frente_a_valor'
    return 1, 'longitud'


KIND_CHAR = {9: 't', 19: 'v', 20: 'v', 21: 'v'}


def cadena_firma(fila):
    """fila del CSV de firmas (volcado del juego) -> cadena de unidades (entradas + salidas)."""
    ins = [int(x) for x in fila['in'].split(';') if x != '']
    outs = [int(x) for x in fila['out'].split(';') if x != '']
    return ''.join(KIND_CHAR.get(k, 'a') for k in ins if k not in (22, 23)) + 'v' * len(outs)


# Discrepancias reales que se dejan a proposito (comandos muertos del EXE: su caso no lee ni hace nada; ningun script del juego puede usarlos sin
# desalinear la IP en el propio EXE). El C++ conserva la implementacion heredada de gta-reversed, que si lee sus parametros.
EXCEPCIONES = {592: 'DRAW_LIGHT: el EXE (0x47F21F) devuelve sin leer sus 6 parametros', 1291: 'POP_CAR_BOOT_USING_PHYSICS: el EXE (0x48CC39) devuelve sin leer su parametro'}

SIGS = os.path.join(RAIZ, 'docs/CodigoVivoExe/ejecucion-auditoria-params-cpp-sig.csv')


def main():
    if '--regenerar-exe' in sys.argv or not os.path.exists(CACHE):
        ex = Exe(EXE)
        filas = extraer_exe(ex)
        for nombre, addr in sorted(WRAP_BY_NAME.items()):
            ps = sorted({unidades(p) for p in ex.paths(addr, {})}, key=lambda s: (len(s), s))
            filas.append(['W:' + nombre, hex(addr), '|'.join(ps), 'ok'])
        os.makedirs(os.path.dirname(CACHE), exist_ok=True)
        with open(CACHE, 'w', newline='', encoding='utf-8') as f:
            w = csv.writer(f); w.writerow(['id', 'caso_exe', 'secuencias', 'estado']); w.writerows(filas)
        print('cache EXE escrita:', CACHE)
    exe_rows = {}
    with open(CACHE, encoding='utf-8') as f:
        for r in csv.DictReader(f): exe_rows[r['id'] if r['id'].startswith('W:') else int(r['id'])] = r
    cpp = Cpp()
    defs = indice_defs({p: s for p, s in cpp.files.items() if p.endswith('.cpp')})
    R = Rutas(defs)
    sigs = {}
    if os.path.exists(SIGS) and '--sin-firmas' not in sys.argv:
        with open(SIGS, encoding='utf-8') as f:
            for r in csv.DictReader(f): sigs[int(r['id'])] = r
    solo = set(int(x) for x in sys.argv[sys.argv.index('--solo') + 1].split(',')) if '--solo' in sys.argv else None
    res = []
    # ayudas que leen parametros por dentro: su cuerpo C++ contra su cuerpo en el EXE
    for nombre in sorted(WRAP_BY_NAME):
        er = exe_rows.get('W:' + nombre)
        cands = defs.get(nombre, [])
        if not er or not cands:
            res.append(['W:' + nombre, nombre, 'ayuda', 4, 'sin_cuerpo_cpp', '', er['secuencias'] if er else '', '']); continue
        rr = R.de_funcion(nombre, None)
        sev, det = comparar_rutas(rr, er['secuencias'].split('|'))
        res.append(['W:' + nombre, nombre, 'ayuda:' + os.path.basename(cands[0][3]), sev, det, '|'.join(sorted(rr))[:80], er['secuencias'], ''])
    for cmd in range(NCMD):
        if solo and cmd not in solo: continue
        er = exe_rows.get(cmd)
        if not er or er['estado'] != 'ok': continue
        rutas = er['secuencias'].split('|')
        ej = '|'.join(rutas)
        nombre = cpp.nombre.get(cmd, str(cmd))
        h = cpp.handlers.get(cmd)
        if h is not None and h[0] != 'UNIMPLEMENTED' and h[5] in cpp.llamados:
            origen = 'manejador:' + os.path.basename(h[2])
            if cmd in sigs and sigs[cmd]['conocida'] == '1':
                cad = cadena_firma(sigs[cmd]); nota = 'firma_exacta'
                ins = [int(x) for x in sigs[cmd]['in'].split(';') if x != '']
                cands = {cad}
                if 22 in ins:
                    # el manejador recibe CRunningScript&: lo que lea por dentro (S.CollectParameters...) hay que sumarlo desde su cuerpo
                    short = h[1][0].strip().lstrip('&(').split('::')[-1].split('<')[0].rstrip(')') if h[1] else ''
                    cuerpos = [d for d in defs.get(short, []) if 'CRunningScript' in d[1]]
                    if cuerpos:
                        sin_ = ''.join(KIND_CHAR.get(k, 'a') for k in ins if k not in (22, 23))
                        sout = 'v' * int(sigs[cmd]['nOut'])
                        R.flags = set()
                        cands = {sin_ + b + sout for b in R.rutas(cuerpos[0][2], cuerpos[0][3])}
                        nota = 'firma+cuerpo'
                sev, det = comparar_rutas(cands, rutas)
                if sev == 1 and cmd in EXCEPCIONES: sev, det, nota = 0, 'excepcion_documentada', EXCEPCIONES[cmd]
                res.append([cmd, nombre, origen, sev, det, '|'.join(sorted(cands))[:80], ej, nota]); continue
            n, nota = cpp.firma_manejador(cmd)
            if n is None:
                res.append([cmd, nombre, origen, 3, 'firma_no_resuelta', '', ej, nota]); continue
            lens = {len(norm(r)) for r in rutas if not any(c in r for c in 'JL*#')}
            if n in lens: sev, det = 0, 'igual'
            elif not lens: sev, det = 3, 'exe_ramas'
            else: sev, det = 1, 'longitud'
            res.append([cmd, nombre, origen, sev, det, 'n=%d' % n, ej, nota + '(heuristica)']); continue
        c = cpp.casos.get(cmd)
        if c is None:
            res.append([cmd, nombre, 'sin_cpp', 4, 'sin_caso_ni_manejador', '', ej, '']); continue
        R.flags = set()
        rr = R.rutas(c[0], c[1])
        sev, det = comparar_rutas(rr, rutas)
        res.append([cmd, nombre, 'caso:' + os.path.basename(c[1]), sev, det, '|'.join(sorted(rr))[:80], ej, ','.join(sorted(R.flags))])
    if '--csv' in sys.argv:
        with open(sys.argv[sys.argv.index('--csv') + 1], 'w', newline='', encoding='utf-8') as f:
            w = csv.writer(f); w.writerow(['id', 'comando', 'origen_cpp', 'gravedad', 'detalle', 'cpp', 'exe', 'nota']); w.writerows(res)
    cnt = Counter((r[3], r[4]) for r in res)
    print('comandos comparados:', len(res), '(firmas exactas del juego: %d)' % len(sigs))
    for k, v in sorted(cnt.items()): print('  gravedad %d %-24s %d' % (k[0], k[1], v))
    print('\nDISCREPANCIAS (1 longitud; 2 orden/tipo; 3 revisar; 4 sin C++):')
    for r in sorted(res, key=lambda r: (r[3], str(r[0]).zfill(5))):
        if r[3]: print(r[3], r[0], r[1], r[2], r[4], 'cpp=%s' % r[5], 'exe=%s' % r[6][:70], r[7])
    sys.exit(1 if any(r[3] == 1 for r in res) else 0)


if __name__ == '__main__':
    threading.stack_size(64 * 1024 * 1024)
    rc = []

    def _t():
        try: main()
        except SystemExit as e: rc.append(e.code or 0)

    t = threading.Thread(target=_t); t.start(); t.join()
    sys.exit(rc[0] if rc else 2)
