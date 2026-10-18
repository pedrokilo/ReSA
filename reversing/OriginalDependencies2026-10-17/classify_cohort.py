"""Clasificación con evidencia de los destinos del cohorte «2» (juego / biblioteca / incierto).

Entrada: targets.csv + evidence-sources.csv (instantánea fija) y el inventario binario regenerado (`--binary-map`).
Salida: classification.csv (mismo esquema que classification-*.csv del cohorte 499) y review-queue.csv (anomalías que exigen revisión manual).
`classification-overrides.csv` (address,identity,category,implementation,evidence,notes) tiene prioridad sobre las reglas: es la revisión
manual con desensamblado del EXE y cada fila declara su motivo. Las reglas son deterministas y se citan por id en la columna `evidence`.

Reglas de categoría (primera que coincide):
  L1  firma fuente con nombre de API RenderWare (Rw/Rp/Rt/Rx/_rw/_rp/_rt/_rx) en un archivo del SDK (rw*.cpp, rp*.cpp, rt*.cpp, RenderWare.h)
      y dirección >= 0x740000 -> biblioteca RenderWare (la dirección sola NUNCA decide: el nombre/contrato de la fuente es la evidencia).
  L2  símbolo de runtime en el mapa de Ghidra (_xxx / __xxx / FID_conflict) o dirección >= 0x821000 (CRT estática) -> biblioteca CRT.
  L3  firma con nombre RenderWare y dirección >= 0x740000 fuera de los archivos del SDK -> biblioteca RenderWare (confianza media).
  G1  referencia del proyecto de nombres (gtasa-reversed) o firma fuente `Clase::método` / nombre de juego, dirección < 0x740000 -> juego.
  G2  sólo contexto fuente de juego (referencia incrustada sin símbolo) y dirección < 0x740000 -> juego, identidad «helper sin símbolo».
  Anomalía (-> review-queue, categoría provisional `unknown`): nombre RW con dirección < 0x740000, nombre de juego con dirección >= 0x740000,
  o ausencia total de contexto.
El proyecto de referencia sólo aporta NOMBRES; no es evidencia de categoría por sí solo.
"""
import argparse
import csv
import re
import sys
from collections import Counter, defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
REFERENCE = Path(r'C:/Users/pedro/Proyectos de prueba/mods/opcodes/gtasa-reversed/source')
RW_NAME = re.compile(r'^(?:_?(?:rw|rp|rt|rx)[A-Z_]|_?(?:Rw|Rp|Rt|Rx)[A-Za-z0-9_]|_rw\w|_rp\w|_rt\w|_rx\w)')
RW_FILE = re.compile(r'(?:^|/)(?:rw\w*|rp\w*|rt\w*|rx\w*|RenderWare)\.(?:cpp|h)(?::\d+)?$')
CRT_NAME = re.compile(r'^(?:_{1,2}[A-Za-z]\w*|FID_conflict:\S+)$')
FLOW = ('if', 'for', 'while', 'switch', 'return', 'else', 'do')


def rd(path):
    with Path(path).open(encoding='utf-8-sig', newline='') as f:
        return list(csv.DictReader(f))


def reference_names():
    import re as _re
    addr = _re.compile(r'0x([0-9A-Fa-f]{5,8})\b')
    inst = _re.compile(r'RH_Scoped\w*Install\s*\(\s*([\w:~<>]+)\s*,\s*(?:"[^"]*"\s*,\s*)?0x([0-9A-Fa-f]+)')
    cls_re = _re.compile(r'RH_ScopedClass\s*\(\s*(\w+)\s*\)')
    defn = _re.compile(r'(?:(\w+)::)?(~?\w+)\s*\(')
    names = defaultdict(set)
    if not REFERENCE.exists():
        return names
    for p in REFERENCE.rglob('*'):
        if not p.is_file() or p.suffix not in {'.cpp', '.h', '.hpp', '.inc'}:
            continue
        lines = p.read_text(encoding='utf-8', errors='replace').splitlines()
        cls = next((m.group(1) for m in (cls_re.search(l) for l in lines) if m), None)
        for i, l in enumerate(lines):
            for m in inst.finditer(l):
                names[int(m.group(2), 16)].add((cls + '::' if cls else '') + m.group(1))
            if _re.match(r'\s*(//|\*|/\*)', l) and addr.search(l) and 'RH_Scoped' not in l:
                va = int(addr.search(l).group(1), 16)
                for j in range(i + 1, min(i + 4, len(lines))):
                    t = lines[j].strip()
                    if not t or t.startswith(('//', '*', '/*', '#')):
                        continue
                    d = defn.search(t)
                    if d:
                        names[va].add((d.group(1) + '::' if d.group(1) else (cls + '::' if cls else '')) + d.group(2))
                    break
    return names


def function_name(sig):
    m = re.search(r'((?:[\w~]+::)*~?[\w]+)\s*\(', sig)
    return m.group(1) if m else ''


def params_of(sig):
    m = re.search(r'\((.*)\)', sig)
    out = []
    if m:
        for p in m.group(1).split(','):
            t = re.findall(r'[A-Za-z_]\w*', p)
            if t:
                out.append(t[-1])
    return out


def body_statements(path, sig_line):
    """Statement count of the function whose header is at sig_line (1-based); None if unreadable."""
    try:
        lines = (ROOT / path).read_text(encoding='utf-8', errors='replace').splitlines()
    except OSError:
        return None
    depth, started, stmts = 0, False, []
    for k in range(max(sig_line - 1, 0), min(len(lines), sig_line + 60)):
        code = re.sub(r'//.*$', '', lines[k])
        for ch in code:
            if ch == '{':
                depth += 1
                started = True
            elif ch == '}':
                depth -= 1
        if started and depth >= 1 and ';' in code:
            stmts.append(code.strip())
        if started and depth == 0:
            return stmts
    return None


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--binary-map', type=Path, required=True)
    ap.add_argument('--exe', type=Path, default=Path('D:/Grand Theft Auto San Andreas/gta-sa.exe'))
    args = ap.parse_args()
    targets = rd(HERE / 'targets.csv')
    evid = defaultdict(list)
    for e in rd(HERE / 'evidence-sources.csv'):
        evid[e['address']].append(e)
    fn = {int(r['va'], 16): r for r in rd(args.binary_map / 'binary_functions.csv')}
    callers, callees = defaultdict(set), defaultdict(set)
    for c in rd(args.binary_map / 'binary_calls.csv'):
        if c['target_va'] and c['caller_va']:
            callees[int(c['caller_va'], 16)].add(int(c['target_va'], 16))
            callers[int(c['target_va'], 16)].add(int(c['caller_va'], 16))
    ref_names = reference_names()
    disasm = None
    try:
        sys.path.insert(0, str(ROOT / 'contrib'))
        import capstone
        from pe_inventory import PE
        pe = PE(args.exe)
        md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)

        def disasm(va, n):
            off = pe.offset(va)
            return ' ; '.join(f'{i.mnemonic} {i.op_str}'.strip() for i in md.disasm(pe.data[off:off + min(n, 24)], va))
    except Exception as exc:  # EXE or capstone not available: classification still works from source evidence
        print('desensamblado no disponible:', exc, file=sys.stderr)
    overrides = {}
    op = HERE / 'classification-overrides.csv'
    if op.exists():
        overrides = {r['address']: r for r in rd(op)}
    out, review = [], []
    for t in targets:
        a = t['address']
        va = int(a, 16)
        refs = evid[a]
        first = refs[0]
        sig, sig_line = first['signature'], first['signature_line']
        sname = function_name(sig)
        rname = sorted(ref_names.get(va, []), key=lambda n: ('::' not in n, n))
        gname = t['ghidra_name']
        named_gh = not re.match(r'^(FUN_|thunk_FUN_|unresolved_entry_)', gname)
        # Names bound at the call site (`const auto _rwD3D9Foo = plugin::Call...<0xADDR>`) identify the target better than the enclosing function.
        bound = [m.group(1) for r in refs for m in [re.search(r'(?:const\s+)?auto\s+([A-Za-z_]\w*)\s*(?:=|\{)\s*(?:plugin::Call|reinterpret_cast)', r['expression'])] if m]
        bound += [m.group(1) for r in refs for m in [re.search(r'(?:constexpr|const|static)[^;=]*?(k?Exe\w+)\s*=', r['expression'])] if m]
        bound_rw = [b for b in bound if RW_NAME.match(b)]
        stmts = body_statements(first['location'].rsplit(':', 1)[0], int(sig_line)) if sig_line and str(sig_line) != '0' else None
        direct = bool(stmts is not None and len(stmts) == 1 and a[2:].lstrip('0').upper() in stmts[0].upper().replace('0X', ''))
        direct_args = bool(direct and all(p in stmts[0] for p in params_of(sig)[:6])) if direct else False
        in_rw_file = any(RW_FILE.search(r['location']) for r in refs)
        rw_sig = bool(RW_NAME.match(sname.split('::')[-1])) if sname else False
        rw_bound = bool(bound_rw)
        rw_ref = any(RW_NAME.match(n.split('::')[-1]) for n in rname)
        crt = bool(named_gh and CRT_NAME.match(gname))
        low, high = va < 0x740000, va >= 0x821000
        rule, category = '', 'unknown'
        if (rw_sig or rw_ref or rw_bound) and in_rw_file and not low:
            rule, category = 'L1', 'library'
        elif crt or high:
            rule, category = 'L2', 'library'
        elif (rw_sig or rw_ref or rw_bound) and not low:
            rule, category = 'L3', 'library'
        elif (rname or (sname and not rw_sig)) and low and not rw_bound:
            rule, category = ('G1', 'game') if (rname or direct) else ('G2', 'game')
        elif low and sname:
            rule, category = 'G2', 'game'
        if (rw_sig or rw_ref or rw_bound) and low:
            rule, category = 'A-rw-name-low-va', 'unknown'
        elif not low and not (rw_sig or rw_ref or rw_bound or crt or high) and category != 'library':
            rule, category = 'A-game-name-high-va', 'unknown'
        if bound_rw:
            identity = bound_rw[0]
        elif rname:
            identity = rname[0]
        elif direct and sname:
            identity = sname
        elif named_gh:
            identity = gname
        elif sname:
            identity = 'helper sin símbolo confirmado (usado por %s)' % sname
        else:
            identity = gname
        implementation = 'wrapper' if direct else 'embedded_call'
        ins = disasm(va, 14) if disasm else ''
        ev = ('Ghidra=%s; firma/contexto fuente línea %s: %s; refs=%d (%s); estilos=%s; expresión=%s; incoming=%s; callers_binarios=%d; callees_binarios=%d; '
              'nombre_referencia=%s; regla=%s; entrada EXE: %s') % (
            gname, sig_line, sig, len(refs), '; '.join(r['location'] for r in refs[:3]), t['wrapper_styles'], first['expression'][:160],
            t['incoming_edges_from_catalogued_functions'], len(callers[va]), len(callees[va]), ' | '.join(rname[:2]) or 'ninguno', rule or 'sin regla', ins)
        note = {'library': 'Biblioteca (RenderWare/CRT) por %s; no acredita reconstrucción ni equivalencia.' % rule,
                'game': 'Función del juego por %s; sin crédito de validación ni seguridad runtime.' % rule,
                'unknown': 'Incierto por %s; requiere revisión manual (review-queue.csv).' % (rule or 'ausencia de contexto')}[category]
        row = dict(address=a, identity=identity, category=category, implementation=implementation, source=first['location'], evidence=ev, notes=note)
        if a in overrides:
            o = overrides[a]
            row.update({k: o[k] for k in ('identity', 'category', 'implementation') if o.get(k)})
            row['evidence'] = row['evidence'] + ' | REVISIÓN MANUAL: ' + o['evidence']
            row['notes'] = o['notes'] or row['notes']
        elif category == 'unknown':
            review.append(dict(address=a, rule=rule, identity=identity, source=first['location'], sig=sig[:100]))
        out.append(row)
    fields = ['address', 'identity', 'category', 'implementation', 'source', 'evidence', 'notes']
    with (HERE / 'classification.csv').open('w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, fieldnames=fields)
        w.writeheader()
        w.writerows(out)
    with (HERE / 'review-queue.csv').open('w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, fieldnames=['address', 'rule', 'identity', 'source', 'sig'])
        w.writeheader()
        w.writerows(review)
    print(dict(Counter(r['category'] for r in out)), 'review:', len(review), dict(Counter(r['implementation'] for r in out)))


if __name__ == '__main__':
    main()
