"""Cohorte «2» (17/10/2026): destinos ORIGINALES explícitos que el árbol actual sigue llamando por dirección.

Método (el del cohorte de 499, ampliado; ver README.md):
  1. Inventario binario con el catálogo ACTUAL: salida de contrib/build-binary-map.py (binary_functions.csv, binary_calls.csv)
     generada en un directorio aparte (`--binary-map`); docs/MemoryMap no se modifica.
  2. Barrido de todo `source/` (.cpp .h .hpp .inc .inl) sin comentarios. Estilos de referencia explícita a una dirección de FUNCIÓN del EXE:
       plugin_call                   plugin::Call*<..., 0xADDR, ...>(...)               (estilo del cohorte 499)
       direct_function_pointer_cast  ((ret(__conv*)(args))0xADDR)(...)                  (estilo del cohorte 499)
       reinterpret_cast_pointer      reinterpret_cast<Fn>(0xADDR) / uintptr_t{0xADDR}   (nuevo: el árbol actual los usa tras las conversiones a C++)
       function_pointer_alias_cast   ((Alias)0xADDR)(...) y ((tipo)(0xADDR))(...)       (nuevo)
       address_constant              constexpr/const/static ... = 0xADDR;               (nuevo; sólo si la dirección es entrada de función)
     Sólo cuentan direcciones que son entrada de función del EXE (binary_functions.csv) SIN nombre de catálogo.
  3. Se excluyen los 499 del cohorte inicial (los validados ya están en el catálogo; los pendientes siguen en su propio cohorte).
Salidas: targets.csv (mismas columnas que el cohorte 499), baseline.json (hashes de entradas y de targets.csv) y evidence-sources.csv
(contexto fuente por destino, insumo de la clasificación). Si targets.csv ya existe NO se reescribe (instantánea fija).
"""
import argparse
import csv
import hashlib
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
FIELDS = ['address', 'ghidra_name', 'body_bytes', 'catalogue_names_empty', 'source_references', 'wrapper_styles',
          'sample_source_expression', 'incoming_edges_from_catalogued_functions', 'sample_catalogued_caller',
          'sample_call_instruction', 'review_classification', 'runtime_evidence']
SUFFIXES = {'.cpp', '.h', '.hpp', '.inc', '.inl', '.c'}
HEXRE = r'(0x[0-9A-Fa-f]{5,8})\b'
PLUGIN = re.compile(r'plugin::Call\w*\s*<(?:[^;0]|0(?!x))*?' + HEXRE)
CAST1 = re.compile(r'\(\([^;\n]*?\*[^;\n]*?\)\s*' + HEXRE + r'\s*\)\s*\(')
CAST2 = re.compile(r'\(\(\s*[A-Za-z_][\w:]*\s*\)\s*\(?\s*' + HEXRE + r'\s*\)?\s*\)\s*\(')
CAST3 = re.compile(r'\(\([^;\n]*?\*[^;\n]*?\)\s*\(\s*' + HEXRE + r'\s*\)\s*\)\s*\(')
CONST = re.compile(r'\b(?:constexpr|const|static)\b[^;=\n(]*?=\s*(?:\(?\s*(?:uintptr_t|uint32|DWORD)\s*\)?\s*)?' + HEXRE + r'\s*;')
RC_OPERAND = re.compile(r'\s*\(\s*(?:uintptr_t\s*\{\s*|std::uintptr_t\s*\{\s*|\(\s*(?:uintptr_t|uint32|DWORD)\s*\)\s*|static_cast<[^>]*>\s*\(\s*)?' + HEXRE)
SKIP_LINE = re.compile(r'HookInstall|RH_Scoped|ReversibleHooks|@addr')
FUNC_DEF = re.compile(r'^[A-Za-z_][\w:<>\*&\s,~\[\]]*\([^;]*\)\s*(?:const\s*)?(?:noexcept\s*)?\{?\s*$')


def rd(path):
    with Path(path).open(encoding='utf-8-sig', newline='') as f:
        return list(csv.DictReader(f))


def sha(path):
    h = hashlib.sha256()
    with open(path, 'rb') as f:
        for chunk in iter(lambda: f.read(1 << 20), b''):
            h.update(chunk)
    return h.hexdigest()


def strip_comments(text):
    """Blank out comments and string literals, keeping line structure."""
    out = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        two = text[i:i + 2]
        if two == '//':
            j = text.find('\n', i)
            j = n if j < 0 else j
            out.append(' ' * (j - i))
            i = j
        elif two == '/*':
            j = text.find('*/', i + 2)
            j = n if j < 0 else j + 2
            out.append(''.join('\n' if ch == '\n' else ' ' for ch in text[i:j]))
            i = j
        elif c == '"':
            j = i + 1
            while j < n and text[j] != '"' and text[j] != '\n':
                j += 2 if text[j] == '\\' else 1
            out.append(' ' * (j + 1 - i))
            i = j + 1
        else:
            out.append(c)
            i += 1
    return ''.join(out)


def reinterpret_matches(text):
    """Yield (offset, address_text) for reinterpret_cast<...>(0xADDR) with balanced angle brackets."""
    pos = 0
    while True:
        k = text.find('reinterpret_cast', pos)
        if k < 0:
            return
        pos = k + 16
        m = re.compile(r'\s*<').match(text, pos)
        if not m:
            continue
        depth, j = 1, m.end()
        while j < len(text) and depth and text[j] != ';':
            depth += (text[j] == '<') - (text[j] == '>' and text[j - 1] != '-')
            j += 1
        if depth:
            continue
        op = RC_OPERAND.match(text, j)
        if op:
            yield k, op.group(1)


def enclosing_signature(lines, index):
    """Nearest preceding top-level-looking function header (heuristic, same spirit as the 499 cohort)."""
    for k in range(index, max(-1, index - 400), -1):
        s = lines[k].rstrip()
        if s and not s[0].isspace() and FUNC_DEF.match(s) and not s.startswith(('if', 'for', 'while', 'switch', 'return', '}', '#')):
            return k + 1, s.strip()
    return 0, ''


def scan_source(function_vas):
    refs = defaultdict(list)       # va -> [(loc, style, expression, signature_line, signature)]
    for path in sorted((ROOT / 'source').rglob('*')):
        if not path.is_file() or path.suffix not in SUFFIXES:
            continue
        raw = path.read_text(encoding='utf-8', errors='replace')
        clean = strip_comments(raw)
        rawlines, lines = raw.splitlines(), clean.splitlines()
        starts = [0]
        for line in lines:
            starts.append(starts[-1] + len(line) + 1)

        def line_of(offset):
            lo, hi = 0, len(starts) - 1
            while lo < hi - 1:
                mid = (lo + hi) // 2
                if starts[mid] <= offset:
                    lo = mid
                else:
                    hi = mid
            return lo

        found = []
        for pattern, style in ((PLUGIN, 'plugin_call'), (CAST1, 'direct_function_pointer_cast'), (CAST3, 'direct_function_pointer_cast'),
                               (CAST2, 'function_pointer_alias_cast'), (CONST, 'address_constant')):
            for m in pattern.finditer(clean):
                found.append((m.start(1), m.group(1), style))
        for off, addr in reinterpret_matches(clean):
            found.append((off, addr, 'reinterpret_cast_pointer'))
        for off, addr, style in found:
            va = int(addr, 16)
            if va not in function_vas:
                continue
            ln = line_of(off)
            if SKIP_LINE.search(lines[ln]):
                continue
            sig_line, sig = enclosing_signature(rawlines, ln)
            ref = (f'{path.relative_to(ROOT).as_posix()}:{ln + 1}', style, rawlines[ln].strip(), sig_line, sig)
            if not any(r[0] == ref[0] and r[1] == ref[1] for r in refs[va]):
                refs[va].append(ref)
    return refs


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--binary-map', type=Path, required=True,
                    help='directorio con binary_functions.csv/binary_calls.csv regenerados con el catálogo actual')
    args = ap.parse_args()
    bm = args.binary_map
    functions = rd(bm / 'binary_functions.csv')
    by_va = {int(r['va'], 16): r for r in functions}
    empty = {va for va, r in by_va.items() if not r['catalogue_names']}
    refs = scan_source(empty)
    first = {int(r['address'], 16) for r in rd(ROOT / 'reversing/OriginalDependencies499/targets.csv')}
    new = {va: v for va, v in refs.items() if va not in first}
    incoming = defaultdict(list)
    for e in rd(bm / 'binary_calls.csv'):
        if not e['target_va'] or not e['caller_va']:
            continue
        t, c = int(e['target_va'], 16), int(e['caller_va'], 16)
        if t in new and c in by_va and by_va[c]['catalogue_names']:
            incoming[t].append((by_va[c]['catalogue_names'], e['instruction_va']))
    rows, ev = [], []
    for va in sorted(new, key=lambda v: (-len(incoming[v]), v)):
        r = new[va]
        caller, ins = incoming[va][0] if incoming[va] else ('', '')
        rows.append(dict(zip(FIELDS, [f'0x{va:08X}', by_va[va]['name'], by_va[va]['body_bytes'], 'True',
                                      '; '.join(x[0] for x in r), '; '.join(sorted({x[1] for x in r})), r[0][2], str(len(incoming[va])),
                                      caller, ins, 'sin_clasificar_semanticamente',
                                      'Ninguna nueva: cruce estático source/binario, no certifica reconstrucción ni equivalencia'])))
        for x in r:
            ev.append({'address': f'0x{va:08X}', 'location': x[0], 'style': x[1], 'expression': x[2], 'signature_line': x[3], 'signature': x[4]})
    targets_path = HERE / 'targets.csv'
    if targets_path.exists():
        print('targets.csv ya existe: instantánea fija, no se reescribe', file=sys.stderr)
        return
    with targets_path.open('w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, fieldnames=FIELDS)
        w.writeheader()
        w.writerows(rows)
    with (HERE / 'evidence-sources.csv').open('w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, fieldnames=list(ev[0]))
        w.writeheader()
        w.writerows(ev)
    hooks = ROOT / 'docs/hooks.csv'
    json_text = json.dumps({
        'targets': len(rows), 'date': '2026-10-17',
        'explicit_uncatalogued_function_targets_total': len(refs),
        'excluded_cohort1_still_referenced': len(refs) - len(new),
        'explicit_uncatalogued_targets_in_source_game_sa': sum(1 for v in new.values() if any(x[0].startswith('source/game_sa') for x in v)),
        'explicit_uncatalogued_targets_only_outside_game_sa': sum(1 for v in new.values() if not any(x[0].startswith('source/game_sa') for x in v)),
        'scope': 'explicit original function targets referenced by address from source/ (all .cpp/.h/.hpp/.inc/.inl), absent from the catalogue at 2026-10-17, excluding the 499 of cohort 1',
        'binary_functions_sha256': sha(bm / 'binary_functions.csv'), 'binary_calls_sha256': sha(bm / 'binary_calls.csv'),
        'hooks_csv_sha256': sha(hooks), 'exe_sha256': json.loads((bm / 'binary_summary.json').read_text(encoding='utf-8'))['exe_sha256'],
        'cohort1_targets_sha256': sha(ROOT / 'reversing/OriginalDependencies499/targets.csv'),
        'targets_sha256': sha(targets_path), 'evidence_sources_sha256': sha(HERE / 'evidence-sources.csv'),
        'binary_map_regeneration': 'copy docs/MemoryMap/index.html to <dir>; python contrib/build-binary-map.py --root . --export build/binary-map --output <dir>',
    }, indent=2) + '\n'
    (HERE / 'baseline.json').write_text(json_text, encoding='utf-8')
    print(len(rows), 'targets;', sum(1 for r in rows if int(r['incoming_edges_from_catalogued_functions']) > 0), 'with incoming edges from catalogued functions')


if __name__ == '__main__':
    main()
