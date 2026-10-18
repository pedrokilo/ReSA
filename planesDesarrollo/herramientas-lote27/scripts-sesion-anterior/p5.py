p = 'D:/Original/gta-reversed-master/contrib/inventory_implementations.py'
s = open(p, encoding='utf-8').read()


def rep(old, new):
    global s
    assert old in s, old
    s = s.replace(old, new, 1)


# 1. calling conventions and type-argument macros hide definitions from the parser
rep("HOOK_CALL = re.compile(rb'(?<![A-Za-z0-9_])RH_Scoped\\w+\\s*\\(')",
    "HOOK_CALL = re.compile(rb'(?<![A-Za-z0-9_])(?:RH_Scoped\\w+|RWPLUGINOFFSET)\\s*\\(')\n"
    "CALLING_CONVENTION = re.compile(rb'(?<![A-Za-z0-9_])__(?:stdcall|cdecl|fastcall|thiscall)(?![A-Za-z0-9_])')")
rep("    raw = CLASS_MACROS.sub(lambda m: blank(m[0]), raw)\n",
    "    raw = CLASS_MACROS.sub(lambda m: blank(m[0]), raw)\n    raw = CALLING_CONVENTION.sub(lambda m: blank(m[0]), raw)\n")

# 2. empty inline bodies `virtual void F() { };` are parsed as field declarations with an initializer_list
rep("""        for node in walk(tree.root_node):
            if node.type != 'function_definition':
                continue
            body = node.child_by_field_name('body')
            if body is None or body.type != 'compound_statement':
                continue
""", """        for node in walk(tree.root_node):
            if node.type not in {'function_definition', 'field_declaration'}:
                continue
            body = node.child_by_field_name('body')
            if node.type == 'field_declaration':
                # `virtual void F() { };`: the parser reads the empty braces as an initializer list.
                initializer = [c for c in node.named_children if c.type == 'initializer_list']
                if (len(initializer) != 1 or node.child_by_field_name('declarator') is None
                        or node.child_by_field_name('declarator').type != 'function_declarator'
                        or sanitized_raw[initializer[0].start_byte + 1:initializer[0].end_byte - 1].strip()):
                    continue
                body = initializer[0]
            elif body is None or body.type != 'compound_statement':
                continue
""")
rep("'parser_error': body.has_error,", "'parser_error': body.has_error if body.type == 'compound_statement' else node.has_error,")

# 3. template parameters / constraint text, aliases
rep("                   'templated': any(a.type == 'template_declaration' for a in ancestors(node))}\n",
    "                   'templated': any(a.type == 'template_declaration' for a in ancestors(node)),\n"
    "                   'template_header': template_header(node, raw)}\n")
rep("def class_base_names(node, raw):", '''def template_header(node, raw):
    """Text of the enclosing template headers (`template<std::integral T>`), empty for non-templates."""
    parts = []
    for ancestor in ancestors(node):
        if ancestor.type == 'template_declaration':
            parameters = ancestor.child_by_field_name('parameters')
            if parameters is not None:
                parts.append(re.sub(r'\\s+', ' ', txt(parameters, raw)))
            for child in ancestor.named_children:
                if child.type == 'requires_clause':
                    parts.append(re.sub(r'\\s+', ' ', txt(child, raw)))
    return ' '.join(reversed(parts))


def template_parameters(header):
    """Names of the type parameters in a template header text."""
    return set(re.findall(r'(?:typename|class|[\\w:]+)\\s+(\\w+)\\s*(?:,|>|=|$)', header))


INTEGRAL = {'int', 'int8', 'int16', 'int32', 'int64', 'uint8', 'uint16', 'uint32', 'uint64', 'short', 'long', 'char', 'bool',
            'unsigned', 'size_t', 'std::int32_t', 'std::uint32_t'}
FLOATING = {'float', 'double'}


def satisfies(header, argument):
    """True/False when the template header uses a known constraint that decides `argument`; None if undecidable."""
    if re.search(r'std::integral\\b|is_integral_v', header):
        return argument in INTEGRAL
    if re.search(r'std::floating_point\\b|is_floating_point_v', header):
        return argument in FLOATING
    return None


def class_base_names(node, raw):''')
# alias table
rep("        # Each occurrence goes to the smallest enclosing named definition",
    """        for alias in ALIAS.finditer(original):
            target, name = (alias[1], alias[2]) if alias[1] else (alias[4], alias[3])
            aliases[name] = target if aliases.get(name, target) == target else None
        # Each occurrence goes to the smallest enclosing named definition""")
rep("    class_bases = {}\n", "    class_bases = {}\n    aliases = {}\n")
rep("TEMPLATE_ARGS = re.compile(",
    "ALIAS = re.compile(r'typedef\\s+(?:struct\\s+)?(\\w+)\\s+(\\w+)\\s*;|using\\s+(\\w+)\\s*=\\s*(\\w+)\\s*;')\nTEMPLATE_ARGS = re.compile(")
rep("class_bases)\n", "class_bases, aliases)\n")
rep("def link_by_registration(functions, catalog, registrations, matched, scanned, bases=None):",
    "def link_by_registration(functions, catalog, registrations, matched, scanned, bases=None, aliases=None):")
rep("    bases = bases or {}\n", "    bases = bases or {}\n    aliases = aliases or {}\n")

# type comparison with aliases and template parameters
rep("""        if len(candidates) > 1 and r['cast']:
            wanted = param_types(r['cast'])
            if wanted is not None:
                candidates = [f for f in candidates if param_types(f['signature']) == wanted]
""", """        arguments = re.findall(r'<([^<>]*)>\\s*$', r['symbol'])
        if arguments and len(candidates) > 1:
            # An instantiation `f<T>` runs a template body (never a plain overload); template constraints pick between them.
            candidates = [f for f in candidates if f.get('templated')]
            decided = [f for f in candidates if satisfies(f['template_header'], arguments[0].strip()) is not False]
            if decided:
                candidates = decided
        if len(candidates) > 1 and r['cast']:
            wanted = param_types(r['cast'])
            if wanted is not None:
                wanted = [resolve_alias(t, aliases) for t in wanted]
                strict = [f for f in candidates if [resolve_alias(t, aliases) for t in param_types(f['signature']) or []] == wanted]
                candidates = strict or [f for f in candidates if f.get('templated') and matches_template(f, wanted, aliases)]
""")
rep("def link_by_registration(", '''def resolve_alias(type_text, aliases):
    """Replace known typedef/using aliases (`RwIm2DVertex` -> `RwD3D9Vertex`) inside a normalized type."""
    def one(match):
        word, seen = match[0], set()
        while aliases.get(word) and word not in seen:
            seen.add(word)
            word = aliases[word]
        return word
    return re.sub(r'[A-Za-z_]\\w*', one, type_text)


def matches_template(f, wanted, aliases):
    """Parameter list equal to `wanted` where parameters spelled with a template parameter name accept any type."""
    names = template_parameters(f['template_header'])
    have = param_types(f['signature'])
    if have is None or len(have) != len(wanted):
        return False
    for mine, theirs in zip(have, wanted):
        core = re.sub(r'^const|[&*]|const$', '', mine)
        if core in names:
            continue
        if resolve_alias(mine, aliases) != theirs:
            return False
    return True


def link_by_registration(''')
# 4. catalog entries with address 0 (virtual/unknown) accept the single registration of that name
rep("(r['virtual'] and int(c['address'], 16) == 0)]", "int(c['address'], 16) == 0]")
open(p, 'w', encoding='utf-8').write(s)
