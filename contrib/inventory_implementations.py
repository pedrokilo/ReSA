"""Inventory source definitions with Tree-sitter; no runtime or independence claims.

Install tree-sitter==0.25.2 and tree-sitter-cpp==0.23.4, then run with -B.
MSVC asm is blanked without moving byte offsets before structural parsing.
Every lexical assembly/naked occurrence must belong to exactly one definition.
"""
import csv
import hashlib
import json
import re
from collections import Counter, defaultdict
from pathlib import Path

from tree_sitter import Language, Parser
import tree_sitter_cpp
from audit_cpp_assembly import LITERALS, TOKENS

ROOT = Path(__file__).resolve().parents[1]
HEX = re.compile(r'0x[0-9A-Fa-f]{6,8}\b')
CALL = re.compile(r'plugin::Call\w*\s*<[^;{}\n]{0,500}')
MASK = re.compile(LITERALS.pattern.encode())


def blank(data):
    return bytes(10 if c == 10 else 13 if c == 13 else 32 for c in data)


def sanitized(raw):
    raw = CLASS_MACROS.sub(lambda m: blank(m[0]), raw)
    clean = MASK.sub(lambda m: blank(m[0]), raw)
    # Keep strings in the syntax tree: removing a string argument can leave
    # f(, value), breaking the surrounding function. Only comments are blanked.
    out = bytearray(MASK.sub(lambda m: blank(m[0]) if m[0].startswith(b'/') else m[0], raw))
    for match in re.finditer(rb'\b(?:__asm|_asm|asm)\b', clean):
        start, end = match.span()
        while end < len(clean) and chr(clean[end]).isspace():
            end += 1
        if end < len(clean) and clean[end] == 123:
            level = 1
            end += 1
            while end < len(clean) and level:
                level += (clean[end] == 123) - (clean[end] == 125)
                end += 1
        else:
            line = clean.find(b'\n', end)
            end = len(clean) if line == -1 else line
        out[start:end] = blank(clean[start:end])
        out[start] = 59
    return bytes(out)


def walk(node):
    stack = [node]
    while stack:
        current = stack.pop()
        yield current
        stack.extend(reversed(current.named_children))


def txt(node, raw):
    return raw[node.start_byte:node.end_byte].decode('utf-8', errors='replace') if node else ''


def declarator_name(node, raw):
    if node is None:
        return ''
    child = node.child_by_field_name('declarator')
    if child:
        return declarator_name(child, raw)
    if node.type in {'qualified_identifier', 'identifier', 'field_identifier',
                     'operator_name', 'destructor_name', 'template_function', 'operator_cast'}:
        return txt(node, raw)
    for child in node.named_children:
        name = declarator_name(child, raw)
        if name:
            return name
    return ''


def owner_name(node, raw, name, path):
    if '::' in name:
        return name.rsplit('::', 1)[0], name
    ancestors = []
    parent = node.parent
    while parent:
        if parent.type in {'class_specifier', 'struct_specifier', 'namespace_definition'}:
            value = txt(parent.child_by_field_name('name'), raw)
            if value:
                ancestors.append(value)
        parent = parent.parent
    if ancestors:
        owner = '::'.join(reversed(ancestors))
        return owner, owner + '::' + name
    return 'Globales y helpers: ' + path, name


def canonical(address):
    return f'0x{int(address, 16):08X}'


def escape(value):
    return str(value).replace('|', '\\|').replace('\n', ' ').replace('<', '&lt;').replace('>', '&gt;').replace('[', '\\[').replace(']', '\\]')


def source_link(path, line, prefix):
    return f'[{path}:{line}]({prefix}{path}#L{line})'


def load_catalog():
    rows = []
    for file in ('hooks.csv', 'hooks_notsa.csv'):
        with (ROOT / 'docs' / file).open(encoding='utf-8-sig', newline='') as stream:
            for item in csv.DictReader(stream):
                item['catalog'] = file
                rows.append(item)
    by_name, by_addr = defaultdict(list), defaultdict(list)
    for row in rows:
        by_name[row['class'] + '::' + row['fn_name']].append(row)
        if '-' in row['fn_name']:
            by_name[row['class'] + '::' + row['fn_name'].split('-', 1)[0]].append(row)
        by_addr[canonical(row['address'])].append(row)
    return rows, by_name, by_addr


STRINGS = re.compile(r'"(?:\\.|[^"\\])*"|' + r"'(?:\\.|[^'\\])*'")
REG = re.compile(r'\bRH_Scoped(\w+)\s*\(')
SCOPE_KINDS = {'Class', 'NamedClass', 'VirtualClass', 'Namespace', 'NamespaceName'}
# kind -> (index of symbol, index of suffix/name or None, index of address, index of cast or None)
INSTALL_ARGS = {
    'Install': (0, None, 1, None), 'GlobalInstall': (0, None, 1, None),
    'OverloadedInstall': (0, 1, 2, 3), 'GlobalOverloadedInstall': (0, 1, 2, 3),
    'NamedGlobalInstall': (0, 1, 2, None), 'NamedInstall': (0, 1, 2, None),
    'VMTOverloadedInstall': (0, 1, 2, 3), 'VMTInstall': (0, None, 1, None)}
BUILTIN = {'int', 'char', 'float', 'double', 'short', 'long', 'unsigned', 'signed', 'bool', 'void', 'const'}
GAME_SA = 'source/game_sa/'
# Bodies are scanned in game_sa and in the application/platform layer (hooks of Rs, Ps, Light... live there).
# The lexical assembly inventory (audit_cpp_assembly.py) keeps covering game_sa only; app files are classified here.
SCAN_ROOTS = ('source/game_sa', 'source/app')
# NOTSA file with real inline assembly in macros (outside any definition); its hooks are not part of hooks.csv.
SKIPPED_FILES = {'source/app/platform/win/WindowedMode.cpp'}
# Attribute macro placed between `class` and the class name; it would hide the class from the parser.
CLASS_MACROS = re.compile(rb'(?<![A-Za-z0-9_])NOTSA_EXPORT_VTABLE(?![A-Za-z0-9_])')


def split_args(text, start):
    """Split the top-level arguments of the call whose '(' precedes `start`; returns (args, end)."""
    args, depth, angle, current, i = [], 1, 0, [], start
    in_string = False
    while i < len(text):
        ch = text[i]
        if in_string:
            current.append(ch)
            if ch == '\\':
                current.append(text[i + 1])
                i += 1
            elif ch == '"':
                in_string = False
        elif ch == '"':
            in_string = True
            current.append(ch)
        elif ch in '([{':
            depth += 1
            current.append(ch)
        elif ch in ')]}':
            depth -= 1
            if depth == 0:
                args.append(''.join(current).strip())
                return args, i
            current.append(ch)
        elif ch == '<' and depth == 1 and i and (text[i - 1].isalnum() or text[i - 1] == '_'):
            angle += 1
            current.append(ch)
        elif ch == '>' and depth == 1 and angle and text[i - 1] != '-':
            angle -= 1
            current.append(ch)
        elif ch == ',' and depth == 1 and not angle:
            args.append(''.join(current).strip())
            current = []
        else:
            current.append(ch)
        i += 1
    return None, i


def literal(value):
    found = re.fullmatch(r'"([^"\\]*)"', value or '')
    return found[1] if found else None


def parse_registrations(raw, relative):
    """Hook registrations (RH_Scoped*Install) in one file; comments are ignored."""
    text = MASK.sub(lambda m: blank(m[0]) if m[0].startswith(b'/') else m[0], raw).decode('utf-8', errors='replace')
    out, stack, depth, last = [], [], 0, 0  # stack of (brace depth, (namespace, name)); blocks restore the outer scope
    for match in REG.finditer(text):
        kind = match[1]
        segment = STRINGS.sub('', text[last:match.start()])
        depth += segment.count('{') - segment.count('}')
        last = match.start()
        while stack and stack[-1][0] > depth:
            stack.pop()
        scope = stack[-1][1] if stack else (None, None)
        if kind not in SCOPE_KINDS and kind not in INSTALL_ARGS:
            continue
        args, _ = split_args(text, match.end())
        if not args:
            continue
        if kind in SCOPE_KINDS:
            if kind == 'NamespaceName':
                new_scope = (None, literal(args[0]))
            elif kind == 'NamedClass' and len(args) > 1:
                new_scope = (args[0], literal(args[1]))
            else:
                new_scope = (args[0], args[0])
            if stack and stack[-1][0] == depth:
                stack.pop()
            stack.append((depth, new_scope))
            continue
        sym_i, name_i, addr_i, cast_i = INSTALL_ARGS[kind]
        if len(args) <= addr_i or not re.fullmatch(r'0x[0-9A-Fa-f]{1,8}', args[addr_i]) or not scope[1]:
            continue
        display = args[sym_i]
        if name_i is not None:
            part = literal(args[name_i])
            if part is None:
                continue
            display = part if kind in {'NamedGlobalInstall', 'NamedInstall'} else display + '-' + part
        out.append({'file': relative, 'kind': kind, 'ns': scope[0], 'scope': scope[1], 'symbol': args[sym_i],
                    'display': display, 'address': canonical(args[addr_i]),
                    'cast': args[cast_i] if cast_i is not None and len(args) > cast_i else '',
                    'member': not kind.startswith('Global') and kind != 'NamedGlobalInstall',
                    'virtual': kind.startswith('VMT')})
    return out


def param_types(signature):
    """Normalized parameter types (names and defaults removed) of the last parameter list in `signature`."""
    end = signature.rfind(')')
    if end < 0:
        return None
    depth, start = 0, None
    for i in range(end, -1, -1):
        depth += signature[i] == ')'
        depth -= signature[i] == '('
        if depth == 0:
            start = i
            break
    if start is None:
        return None
    body = signature[start + 1:end]
    params, depth, angle, current = [], 0, 0, ''
    for i, ch in enumerate(body):
        if ch in '([{':
            depth += 1
        elif ch in ')]}':
            depth -= 1
        elif ch == '<' and i and (body[i - 1].isalnum() or body[i - 1] == '_'):
            angle += 1
        elif ch == '>' and angle and body[i - 1] != '-':
            angle -= 1
        if ch == ',' and not depth and not angle:
            params.append(current)
            current = ''
        else:
            current += ch
    params.append(current)
    result = []
    for param in params:
        param = re.sub(r'\s+', ' ', param.split('=', 1)[0]).strip()
        param = re.sub(r'\b(?:struct|class|enum)\s+', '', param)
        named = re.fullmatch(r'(.+?[\s*&>])\s*([A-Za-z_]\w*)', param)
        if named and named[2] not in BUILTIN:
            param = named[1]
        param = re.sub(r'\s+', '', param)
        if param:
            result.append(param)
    return [] if result == ['void'] else result


def link_by_registration(functions, catalog, registrations, matched, scanned):
    """Link catalog rows still unmatched to exactly one source body using the hook installation.

    The registration (scope, name, address) identifies the catalog row; the registered C++ symbol
    identifies the body. Anything that stays ambiguous is left unmatched."""
    by_symbol = defaultdict(list)
    for f in functions:
        by_symbol[f['function'].rsplit('::', 1)[-1]].append(f)
    catalog_by_key = defaultdict(list)
    for c in catalog:
        catalog_by_key[(c['class'], c['fn_name'])].append(c)
    proposals = defaultdict(set)  # id(catalog row) -> {id(function)}
    rows = {}
    for r in registrations:
        if r['file'] not in scanned:
            continue
        keyed = catalog_by_key.get((r['scope'], r['display']), [])
        keyed = [c for c in keyed if canonical(c['address']) == r['address'] or (r['virtual'] and int(c['address'], 16) == 0)]
        if len(keyed) != 1 or id(keyed[0]) in matched:
            continue
        c = keyed[0]
        symbol = r['symbol']
        short = symbol.rsplit('::', 1)[-1]
        if r['member']:
            if not r['ns']:
                continue
            want = r['ns'] + '::' + short
            candidates = [f for f in by_symbol.get(short, []) if f['function'] == want or f['function'].endswith('::' + want)]
        else:
            candidates = [f for f in by_symbol.get(short, []) if f['function'] == symbol or f['function'].endswith('::' + symbol)]
            same = [f for f in candidates if f['file'] == r['file']]
            if len(candidates) > 1 and same:
                candidates = same
        if len(candidates) > 1 and r['cast']:
            wanted = param_types(r['cast'])
            if wanted is not None:
                candidates = [f for f in candidates if param_types(f['signature']) == wanted]
        if len(candidates) != 1 or candidates[0].get('catalog_match'):
            continue
        proposals[id(c)].add(id(candidates[0]))
        rows[id(c)] = (c, candidates[0])
    claimed = Counter(next(iter(v)) for v in proposals.values() if len(v) == 1)
    for key, targets in proposals.items():
        if len(targets) == 1 and claimed[next(iter(targets))] == 1:
            c, f = rows[key]
            f['catalog_match'] = c
            f['linked_by_address'] = True
            matched.add(key)


def outside_game_sa(catalog, registrations, matched):
    """Catalog rows still without a body whose hook is installed only from a file outside source/game_sa."""
    inside = {(r['scope'], r['display'], r['address']) for r in registrations if r['file'].startswith(GAME_SA)}
    found = {(r['scope'], r['display'], r['address']): r['file'] for r in registrations if not r['file'].startswith(GAME_SA)}
    result = {}
    for c in catalog:
        key = (c['class'], c['fn_name'], canonical(c['address']))
        if id(c) not in matched and key in found and key not in inside:
            result[id(c)] = found[key]
    return result


def collect():
    parser = Parser(Language(tree_sitter_cpp.language()))
    catalog, by_name, by_addr = load_catalog()
    functions, occurrences, unresolved, file_errors = [], [], [], []
    source_hashes, global_data = {}, defaultdict(list)
    registrations = []
    paths = sorted(p for root in SCAN_ROOTS for p in (ROOT / root).rglob('*')
                   if p.suffix in {'.cpp', '.h', '.hpp', '.inc'} and p.relative_to(ROOT).as_posix() not in SKIPPED_FILES)
    scanned = {p.relative_to(ROOT).as_posix() for p in paths}
    for path in paths:
        relative = path.relative_to(ROOT).as_posix()
        raw = path.read_bytes()
        source_hashes[relative] = hashlib.sha256(raw).hexdigest()
        registrations += parse_registrations(raw, relative)
        original = raw.decode('utf-8-sig', errors='strict')
        # Canonical lexical scanner, exactly the same rules as the previous inventory.
        lexical = LITERALS.sub(lambda m: ''.join('\n' if c == '\n' else ' ' for c in m[0]), original)
        sanitized_raw = sanitized(raw)
        clean_raw = MASK.sub(lambda m: blank(m[0]), raw)
        tree = parser.parse(sanitized_raw)
        definitions = []
        for node in walk(tree.root_node):
            if node.type != 'function_definition':
                continue
            body = node.child_by_field_name('body')
            if body is None or body.type != 'compound_statement':
                continue
            name = declarator_name(node.child_by_field_name('declarator'), raw)
            if not name:
                continue
            owner, qualified = owner_name(node, raw, name, relative)
            signature = re.sub(r'\s+', ' ', txt(node.child_by_field_name('declarator'), raw)).strip()
            row = {'owner': owner, 'function': qualified, 'signature': signature,
                   'file': relative, 'line': node.start_point.row + 1,
                   'end_line': node.end_point.row + 1, 'start': node.start_byte,
                   'end': node.end_byte, 'body_start': body.start_byte,
                   'body_end': body.end_byte, 'asm_occurrences': 0, 'naked_declarations': 0,
                   'parser_error': body.has_error, 'body': txt(body, raw),
                   'clean_body': txt(body, clean_raw),
                   'has_cpp_body': bool(re.search(rb'[A-Za-z_]', sanitized_raw[body.start_byte:body.end_byte])),
                   'statements': [n.type for n in body.named_children if n.type != 'comment']}
            prefix = raw[max(0, node.start_byte - 450):node.start_byte].decode('utf-8', errors='replace')
            comments = re.search(r'(?://[^\n]*\n[ \t]*)+$', prefix)  # contiguous comment lines only: a blank line ends the block
            addresses = HEX.findall(comments[0]) if comments else []
            entries = re.findall(r'//\s*>?\s*(0x[0-9A-Fa-f]{6,8})\b', comments[0]) if comments else []
            row['address'] = canonical(entries[0]) if entries else ''
            definitions.append(row)
            functions.append(row)
        # Each occurrence goes to the smallest enclosing named definition (lambdas belong to caller).
        for match in TOKENS.finditer(lexical):
            offset = len(original[:match.start()].encode('utf-8')) + (3 if raw.startswith(b'\xef\xbb\xbf') else 0)
            candidates = [f for f in definitions if f['start'] <= offset < f['end']]
            kind = 'naked' if match[0].startswith('__declspec') else 'inline_assembly'
            entry = {'file': relative, 'line': lexical.count('\n', 0, match.start()) + 1, 'kind': kind}
            if candidates:
                target = min(candidates, key=lambda f: f['end'] - f['start'])
                target['naked_declarations' if kind == 'naked' else 'asm_occurrences'] += 1
                entry.update(function=target['function'], definition_line=target['line'])
            else:
                entry.update(function='', definition_line='')
                unresolved.append(entry)
            occurrences.append(entry)
        # Fixed globals are evidence only when the definition contains a pointer/static reference.
        for node in walk(tree.root_node):
            if node.type not in {'declaration', 'field_declaration'}:
                continue
            if any(f['start'] <= node.start_byte < f['end'] for f in definitions):
                continue
            text = txt(node, clean_raw)
            addresses = HEX.findall(text)
            if not addresses or not re.search(r'StaticRef|reinterpret_cast|\*\s*\([^)]*\*[^)]*\)|\*\s*\(?(?:0x)', text):
                continue
            for decl in node.named_children:
                if decl.type not in {'init_declarator', 'reference_declarator', 'pointer_declarator'}:
                    continue
                name = declarator_name(decl, raw)
                if name and re.fullmatch(r'\w+', name):
                    global_data[name].append({'file': relative, 'line': node.start_point.row + 1,
                                              'addresses': sorted(set(map(canonical, addresses)))})
        if tree.root_node.has_error:
            file_errors.append(relative)
    for path in sorted((ROOT / 'source').rglob('*')):
        relative = path.relative_to(ROOT).as_posix()
        if (path.suffix in {'.cpp', '.h', '.hpp', '.inc'} and relative not in scanned
                and not relative.startswith('source/reversiblehooks/')):
            registrations += parse_registrations(path.read_bytes(), relative)
    if unresolved:
        raise ValueError('Occurrences without a containing definition: ' + json.dumps(unresolved, ensure_ascii=False))
    approvals_file = ROOT / 'reversing/CppOnlyMigration/approvals.json'
    approvals = json.loads(approvals_file.read_text(encoding='utf-8'))
    shared_files = {v['shared_source'] for v in approvals.values() if v.get('shared_source')}
    for row in functions:
        choices = by_name.get(row['function'], [])
        if not choices and row['address']:
            choices = by_addr.get(row['address'], [])
        if len(choices) > 1 and row['address']:
            choices = [c for c in choices if canonical(c['address']) == row['address']]
        row['catalog_match'] = choices[0] if len(choices) == 1 else None
    # Virtual entries (catalog address 0x0) are anchored by their RH_ScopedVMTInstall registration; without it the name match is dropped.
    anchored = {(r['scope'], r['display']) for r in registrations if r['virtual']}
    for row in functions:
        c = row['catalog_match']
        if c and int(c['address'], 16) == 0 and (c['class'], c['fn_name']) not in anchored:
            row['catalog_match'] = None
    matched = {id(f['catalog_match']) for f in functions if f['catalog_match']}
    link_by_registration(functions, catalog, registrations, matched, scanned)
    outside = outside_game_sa(catalog, registrations, matched)
    outside = [(c, outside[id(c)]) for c in catalog if id(c) in outside]
    for row in functions:
        c = row['catalog_match']
        if c and (row['function'].find('::') == -1 and row['function'] == c['fn_name'] or
                  row.get('linked_by_address') and row['owner'].startswith('Globales y helpers')):
            row['owner'] = c['class']
        row['catalog'] = c['catalog'] if c else ''
        row['catalog_identity'] = (c['class'] + '::' + c['fn_name']) if c else ''
        row['catalog_address'] = canonical(c['address']) if c else ''
        row['reversed'] = c['reversed'] if c else ''
        row['validated'] = c['validated'] if c else ''
        row['validation'] = 'Histórica del catálogo' if c and c['validated'] == '1' else 'Pendiente en catálogo' if c else 'Sin registro inequívoco'
        migration = [v for v in approvals.values() if row['file'] == v.get('source') and
                     (row['function'] in v['identity'].split(' / ') or row['catalog_identity'] == v['identity'])]
        shared = row['file'] in shared_files
        row['migration_evidence'] = 'reversing/CppOnlyMigration/approvals.json' if migration or shared else ''
        if row['migration_evidence']:
            row['validation'] = 'Revalidada en migración (helper/consumidores)' if shared else 'Revalidada en migración'
        row['language'] = ('Mixta (C++ + ASM)' if row['has_cpp_body'] else 'Ensamblador') if row['asm_occurrences'] else 'C++'
        body = row['clean_body']
        calls = []
        for match in CALL.finditer(body):
            # First closing invocation avoids reading later expressions as template arguments.
            cut = match[0].split('>(', 1)[0]
            found = HEX.findall(cut)
            if found:
                calls.append(canonical(found[0]))
        trampoline = False
        compact = re.sub(r'\s+', ' ', body).strip()
        if row['asm_occurrences']:
            no_shell = re.sub(r'\b__asm\b|[{};]', ' ', compact)
            no_shell = re.sub(r'\s+', ' ', no_shell).strip()
            trampoline = bool(re.fullmatch(r'(?:mov (e\w+), ?(?:0x[\dA-Fa-f]+|[\dA-Fa-f]+h) jmp \1|push (?:0x[\dA-Fa-f]+|[\dA-Fa-f]+h) ret)', no_shell))
        wrapper = bool(calls and len(row['statements']) == 1 and row['statements'][0] in {'return_statement', 'expression_statement'}) or trampoline
        row['implementation'] = 'Envoltura al EXE' if wrapper else 'Cuerpo fuente con errores de análisis' if row['parser_error'] else 'Cuerpo fuente; delegación indirecta no descartada'
        row['own_validated_reconstructed'] = bool(not wrapper and not row['parser_error'] and
            ((c and c['reversed'] == '1' and c['validated'] == '1') or row['migration_evidence']))
        dependencies = []
        for address in sorted(set(calls)):
            targets = by_addr.get(address, [])
            destinations = ', '.join(t['class'] + '::' + t['fn_name'] for t in targets) or 'fuera del catálogo consultado'
            dependencies.append(f"{'Delegación' if wrapper else 'Colaborador'} por dirección {address}: {destinations}; ruta efectiva sin resolver")
        if trampoline:
            found = re.search(r'\b(?:0x[\dA-Fa-f]+|[\dA-Fa-f]+h)\b', no_shell)
            literal = found[0] if found else ''
            address = f"0x{int(literal[:-1] if literal.endswith('h') else literal, 16):08X}" if literal else 'sin resolver'
            dependencies.append('Trampolín ASM a ' + address + '; cuerpo delegado')
        pointer_addresses = re.findall(r'reinterpret_cast\s*<[^()]{1,200}>\s*\(\s*(0x[0-9A-Fa-f]{6,8})\b', body)
        pointer_addresses += re.findall(r'\(\s*[\w:\s<>]+\*[^)]*\)\s*(0x[0-9A-Fa-f]{6,8})\b', body)
        if pointer_addresses:
            dependencies.append('Conversión de dirección fija: ' + ', '.join(sorted(set(map(canonical, pointer_addresses)))))
        # Conservative lexical references, including class fields and references declared in headers.
        identifiers = set(re.findall(r'\b[A-Za-z_]\w*\b', body))
        used = sorted(identifiers.intersection(global_data))
        for name in used:
            definitions = global_data[name]
            descriptions = [f"{g['file']}:{g['line']} ({','.join(g['addresses'])})" for g in definitions]
            dependencies.append('Posible dato fijo ' + name + ': ' + ', '.join(descriptions))
        if row['asm_occurrences']:
            dependencies.append('ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente')
        row['dependency_details'] = dependencies
        row['dependency'] = '; '.join(dependencies) if dependencies else 'Sin indicio directo en patrones examinados; dependencias transitivas sin determinar'
        row['independence'] = 'No demostrada'
    return functions, occurrences, catalog, source_hashes, file_errors, outside


def write(functions, occurrences, catalog, hashes, errors, outside):
    out = ROOT / 'docs/implementaciones'
    out.mkdir(exist_ok=True)
    groups = defaultdict(list)
    for row in functions:
        groups[row['owner']].append(row)
    matched = {(r['catalog_identity'], r['catalog_address']) for r in functions}
    unmatched = [c for c in catalog if (c['class'] + '::' + c['fn_name'], canonical(c['address'])) not in matched]
    with (out / 'catalogo-sin-coincidencia.csv').open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=list(catalog[0]))
        writer.writeheader()
        writer.writerows(unmatched)
    # Catalog rows installed from outside source/game_sa (platform, application...) that still have no identifiable body.
    with (out / 'catalogo-fuera-de-game_sa.csv').open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=list(catalog[0]) + ['registered_in'])
        writer.writeheader()
        writer.writerows(dict(c, registered_in=where) for c, where in outside)
    fields = ['owner', 'function', 'signature', 'file', 'line', 'end_line', 'language',
              'asm_occurrences', 'naked_declarations', 'implementation', 'validation',
              'catalog_identity', 'catalog_address', 'reversed', 'validated', 'dependency',
              'independence', 'parser_error', 'migration_evidence', 'catalog']
    with (out / 'funciones.csv').open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=fields, extrasaction='ignore')
        writer.writeheader()
        writer.writerows(sorted(functions, key=lambda r: (r['owner'], r['file'], r['line'])))
    with (out / 'apariciones.csv').open('w', encoding='utf-8', newline='') as stream:
        writer = csv.DictWriter(stream, fieldnames=['file', 'line', 'kind', 'function', 'definition_line'])
        writer.writeheader()
        writer.writerows(occurrences)
    pages, details = {}, {}
    owners = sorted(groups)
    # Bounded pages for GitHub rendering; stable links use numbered explicit anchors.
    for index, owner in enumerate(owners):
        page = f'detalle-{index // 20 + 1:03}.md'
        anchor = f'grupo-{index + 1}'
        pages[owner] = (page, anchor)
        lines = details.setdefault(page, ['# Detalle de implementaciones', '',
            '[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.', ''])
        lines += [f'<a id="{anchor}"></a>', '', '## ' + escape(owner), '',
                  '| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |',
                  '| --- | --- | --- | --- | --- | --- |']
        for row in sorted(groups[owner], key=lambda r: (r['file'], r['line'])):
            location = source_link(row['file'], row['line'], '../../')
            flags = f"{row['asm_occurrences']} / {row['naked_declarations']}"
            validation = row['validation']
            if row['catalog_identity']:
                validation += ': ' + row['catalog_identity'] + ' ' + row['catalog_address']
            if row['reversed'] == '0':
                validation += '; reversed=0'
            validation = escape(validation)
            if row['migration_evidence']:
                validation += ' · [registro](../../reversing/CppOnlyMigration/approvals.json)'
            lines.append('| ' + ' | '.join([escape(row['signature']), location,
                escape(row['language'] + ' · ' + row['implementation']), validation.replace('|', '\\|'),
                flags, escape(row['dependency'])]) + ' |')
        lines.append('')
    for name, lines in details.items():
        (out / name).write_text('\n'.join(lines).rstrip() + '\n', encoding='utf-8')
    totals = {'source_definition_count': len(functions), 'groups': len(groups),
        'assembly_occurrences': sum(r['asm_occurrences'] for r in functions),
        'naked_declarations': sum(r['naked_declarations'] for r in functions),
        'definitions_with_assembly': sum(bool(r['asm_occurrences']) for r in functions),
        'definitions_with_naked': sum(bool(r['naked_declarations']) for r in functions),
        'mixed_definitions': sum(r['language'] == 'Mixta (C++ + ASM)' for r in functions),
        'asm_only_definitions': sum(r['language'] == 'Ensamblador' for r in functions),
        'unassigned_occurrences': 0, 'body_parse_errors': sum(bool(r['parser_error']) for r in functions),
        'asm_bodies_with_parse_errors': sum(bool(r['asm_occurrences']) and bool(r['parser_error']) for r in functions),
        'files_with_any_parse_errors': len(errors), 'historical_catalog_rows': len(catalog),
        'catalog_rows_without_unambiguous_source_match_in_scope': len(unmatched),
        'catalog_rows_installed_outside_game_sa': len(outside),
        'catalog_rows_linked_by_hook_address': sum(bool(r.get('linked_by_address')) for r in functions),
        'validated_reconstructed_own_cpp_definitions': sum(r['own_validated_reconstructed'] and r['language'] == 'C++' for r in functions),
        'validated_reconstructed_own_asm_definitions': sum(r['own_validated_reconstructed'] and r['language'] != 'C++' for r in functions),
        'wrapper_definitions': sum(r['implementation'] == 'Envoltura al EXE' for r in functions)}
    summary = {'scope': 'source/game_sa and source/app (WindowedMode.cpp excluded), definitions in cpp/h/hpp/inc; all preprocessing branches',
        'totals': totals, 'source_sha256': hashes,
        'catalog_sha256': {name: hashlib.sha256((ROOT / 'docs' / name).read_bytes()).hexdigest() for name in ('hooks.csv', 'hooks_notsa.csv')},
        'migration_registry_sha256': hashlib.sha256((ROOT / 'reversing/CppOnlyMigration/approvals.json').read_bytes()).hexdigest(),
        'files_with_parse_errors': errors, 'parser': 'tree-sitter 0.25.2 / tree-sitter-cpp 0.23.4; MSVC asm byte-preserving masking'}
    (out / 'resumen.json').write_text(json.dumps(summary, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    lines = ['# Implementaciones del juego: C++, ensamblador y dependencia del EXE', '',
        'Este documento distingue el lenguaje de la fuente, la reconstrucción, la validación registrada y la independencia del EXE. Reconstruir un cuerpo en ensamblador también es ingeniería inversa; una envoltura que delega el cuerpo al original no lo reconstruye por sí sola.', '',
        f"El inventario asigna **{totals['assembly_occurrences']} apariciones de ensamblador** a **{totals['definitions_with_assembly']} definiciones con cuerpo** y **{totals['naked_declarations']} declaraciones naked** a **{totals['definitions_with_naked']} definiciones**. No queda ninguna aparición sin ubicación. Las funciones mixtas se cuentan en «con ensamblador»; naked es una marca adicional que no se suma como otra función.", '',
        f"De esas {totals['definitions_with_assembly']} definiciones, **{totals['mixed_definitions']} son mixtas** y **{totals['asm_only_definitions']} contienen sólo ensamblador en su cuerpo** según el análisis estructural. Las 277 marcas naked se solapan con esas categorías; no se suman a ellas. Se encontraron {len(functions)} definiciones en {len(groups)} clases, namespaces o grupos de helpers; no equivale al número de clases del catálogo.", '',
        '## Cómo leer la tabla', '',
        '- Las dos columnas de reconstruidas y validadas cuentan cuerpos propios con reversed=1 y validated=1 en una coincidencia inequívoca del catálogo, o con evidencia de la migración a C++. El detalle distingue evidencia **histórica del catálogo** de **revalidación de migración**. Este inventario no ejecuta pruebas ni acredita independencia. Envolturas y cuerpos con errores del parser quedan fuera de esas dos columnas.',
        '- El detalle incluye también cuerpos sin registro, pendientes, auxiliares NOTSA, helpers y envolturas; no se ocultan en el 100% del catálogo. Se muestran nombres y ubicaciones con línea.',
        '- Se inventarían definiciones, no entradas binarias: sobrecargas, especializaciones, inline y variantes del preprocesador pueden tener varias ubicaciones. No se suman estas cifras al catálogo ni a las 34 conversiones recientemente revalidadas.',
        '- La asociación al catálogo usa nombre calificado inequívoco o dirección del comentario inmediato y, cuando el nombre es ambiguo (sobrecargas, sufijos), la dirección de la instalación del hook (`RH_Scoped*Install`) que identifica la entrada y el símbolo registrado que identifica el cuerpo. Coincidencias que siguen ambiguas quedan sin registro inequívoco; no se infiere validación por clase completa ni por un colaborador.',
        '- Una envoltura C++ se reconoce conservadoramente cuando el único statement es una llamada plugin::Call* a dirección fija. Algunos trampolines ASM simples también se reconocen. Otras envolturas indirectas pueden permanecer sin resolver.',
        '- Se registran colaboradores con dirección fija y posibles datos fijos referenciados por nombre. Un destino catalogado puede estar reconstruido y enlazado mediante hook; el inventario **no determina la ruta efectiva**. Direcciones por sí solas no prueban ausencia de reconstrucción.',
        '- «Sin indicio directo» sólo significa que los patrones examinados no lo encontraron. No se resuelven todas las llamadas virtuales, macros, alias, punteros ni dependencias transitivas. **Ningún grupo se declara standalone demostrado.**',
        f"- Tree-sitter detectó errores de análisis en {totals['body_parse_errors']} cuerpos; aparecen marcados y no cuentan como reconstruidos/validados en el resumen. No son diagnósticos de compilación ni fallos de las pruebas del juego. Ninguno de los {totals['definitions_with_assembly']} cuerpos con ASM tiene errores estructurales tras enmascarar sus bloques. Los errores fuera del cuerpo también constan en resumen.json. El inventario cubre ramas del preprocesador sin elegir una configuración de compilación.", '',
        f"- {len(unmatched)} entradas de ambos catálogos no tienen coincidencia inequívoca en los cuerpos detectados de source/game_sa. Pueden estar fuera de este ámbito o requerir resolver alias, macros o sobrecargas. **No se presentan como funciones inexistentes ni como nuevas pendientes de validación.** Se conservan en [el listado de asociaciones sin resolver](implementaciones/catalogo-sin-coincidencia.csv).", '',
        '## Resumen por clase o grupo', '',
        '| Clase / grupo y ubicación | C++ reconstruidas y validadas (histórico) | Con ASM reconstruidas y validadas (histórico) | Otras definiciones / envolturas / pendientes | Apariciones ASM | naked |',
        '| --- | ---: | ---: | ---: | ---: | ---: |']
    for owner in owners:
        rows = groups[owner]
        cpp = sum(r['own_validated_reconstructed'] and r['language'] == 'C++' for r in rows)
        asm = sum(r['own_validated_reconstructed'] and r['language'] != 'C++' for r in rows)
        page, anchor = pages[owner]
        files = sorted({r['file'] for r in rows})
        locations = ', '.join(f'[{p}](../{p})' for p in files)
        lines.append(f"| [{escape(owner)}](implementaciones/{page}#{anchor}) · {locations} | {cpp} | {asm} | {len(rows)-cpp-asm} | {sum(r['asm_occurrences'] for r in rows)} | {sum(r['naked_declarations'] for r in rows)} |")
    lines += ['', '## Datos y reproducción', '',
        '- [Todas las funciones y sus estados](implementaciones/funciones.csv).',
        '- [Cada aparición ASM/naked asignada a su función](implementaciones/apariciones.csv).',
        '- [Entradas del catálogo sin asociación inequívoca dentro del ámbito](implementaciones/catalogo-sin-coincidencia.csv).',
        '- [Entradas cuyo hook se instala fuera de source/game_sa (plataforma, aplicación...) y siguen sin cuerpo enlazado](implementaciones/catalogo-fuera-de-game_sa.csv).',
        '- [Conteos, versiones del parser y hashes de entrada](implementaciones/resumen.json).',
        '- [Conversiones a C++ revalidadas recientemente](../reversing/CppOnlyMigration/README.md).',
        '- [Alcance del catálogo y dependencias fuera de él](CurrentStatus.md).', '',
        'Para regenerar: instalar las versiones de [requirements-implementations.txt](../contrib/requirements-implementations.txt) y ejecutar `python -B contrib/inventory_implementations.py`. El script falla si una aparición no se puede asignar a un cuerpo o si no coincide con el inventario léxico independiente.', '']
    lines += ['## Relación con un standalone', '',
        'Una implementación propia en ensamblador también puede compilarse dentro de un ejecutable independiente para Windows x86. No obliga por sí sola a ejecutar el EXE original. Una declaración naked sólo cambia quién prepara y restaura la entrada/salida de la función; no acredita ni niega reconstrucción.', '',
        'Para un standalone hay que dar almacenamiento propio a los datos, resolver los cuerpos que todavía se delegan al EXE y crear el arranque, la inicialización y el bucle del ejecutable. Las dependencias pueden existir tanto en C++ como en ensamblador. Reemplazar el lenguaje no resuelve automáticamente esos enlaces.', '',
        'Este documento identifica evidencia estática y límites de asociación. La independencia se demostraría ejecutando los subsistemas sin el EXE original; las comparaciones dentro del juego completo no sustituyen esa prueba.', '']
    (ROOT / 'docs/ImplementacionesDelJuego.md').write_text('\n'.join(lines), encoding='utf-8')
    return totals


def main():
    functions, occurrences, catalog, hashes, errors, outside = collect()
    with (ROOT / 'reversing/CppOnlyMigration/inventory.csv').open(encoding='utf-8', newline='') as stream:
        expected = Counter((r['file'], int(r['line']), r['kind']) for r in csv.DictReader(stream))
    actual = Counter((r['file'], r['line'], r['kind']) for r in occurrences if r['file'].startswith(GAME_SA))
    if expected != actual:
        raise ValueError('Lexical inventory mismatch; regenerate audit_cpp_assembly.py first')
    print(json.dumps(write(functions, occurrences, catalog, hashes, errors, outside), ensure_ascii=False))


if __name__ == '__main__':
    main()
