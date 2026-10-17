"""Regression checks for source boundaries, not for game behavior."""
import sys
import unittest
import csv
import hashlib
import json
import re
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import inventory_implementations as inventory
from tree_sitter import Language, Parser
import tree_sitter_cpp


class StructuralInventoryTest(unittest.TestCase):
    def parse(self, source):
        raw = source.encode()
        masked = inventory.sanitized(raw)
        tree = Parser(Language(tree_sitter_cpp.language())).parse(masked)
        definitions = [n for n in inventory.walk(tree.root_node) if n.type == 'function_definition']
        return raw, masked, tree, definitions

    def test_strings_remain_valid_arguments_and_fake_asm_is_ignored(self):
        source = 'void X::Mixed(){ log("__asm { fake }", 7); __asm { fld value\n fstp result\n } }'
        raw, masked, tree, definitions = self.parse(source)
        self.assertEqual(len(raw), len(masked))
        self.assertEqual(len(definitions), 1)
        self.assertFalse(tree.root_node.has_error)
        self.assertIn(b'"__asm { fake }"', masked)
        self.assertNotIn(b'fld value', masked)

    def test_inline_class_and_namespace_keep_owner_and_offsets(self):
        source = 'namespace N { struct C { void F(){ __asm { nop } } }; }'
        raw, masked, tree, definitions = self.parse(source)
        node = definitions[0]
        name = inventory.declarator_name(node.child_by_field_name('declarator'), raw)
        self.assertEqual(inventory.owner_name(node, raw, name, 'x.h'), ('N::C', 'N::C::F'))
        self.assertEqual(raw.count(b'\n'), masked.count(b'\n'))

    def test_naked_body_and_lambda_are_not_extra_named_functions(self):
        source = '__declspec(naked) int F(){ __asm { xor eax,eax\n ret } }\nvoid G(){ auto a=[](){ __asm { nop } }; }'
        raw, masked, tree, definitions = self.parse(source)
        names = [inventory.declarator_name(n.child_by_field_name('declarator'), raw) for n in definitions]
        self.assertEqual(names, ['F', 'G'])
        self.assertFalse(tree.root_node.has_error)
        self.assertEqual(len(raw), len(masked))


class PublishedInventoryTest(unittest.TestCase):
    def test_each_lexical_occurrence_has_one_real_definition(self):
        out = inventory.ROOT / 'docs/implementaciones'
        with (out / 'funciones.csv').open(encoding='utf-8', newline='') as stream:
            functions = list(csv.DictReader(stream))
        with (out / 'apariciones.csv').open(encoding='utf-8', newline='') as stream:
            occurrences = list(csv.DictReader(stream))
        with (inventory.ROOT / 'reversing/CppOnlyMigration/inventory.csv').open(encoding='utf-8', newline='') as stream:
            original = list(csv.DictReader(stream))
        key = lambda r: (r['file'], r['line'], r['kind'])
        self.assertEqual(Counter(map(key, occurrences)), Counter(map(key, original)))
        definitions = {(r['file'], r['line'], r['function']) for r in functions}
        self.assertEqual(len(definitions), len(functions))
        for row in occurrences:
            self.assertIn((row['file'], row['definition_line'], row['function']), definitions)
        for field, kind in [('asm_occurrences', 'inline_assembly'), ('naked_declarations', 'naked')]:
            self.assertEqual(sum(int(f[field]) for f in functions), sum(r['kind'] == kind for r in occurrences))
        summary = json.loads((out / 'resumen.json').read_text(encoding='utf-8'))
        for path, digest in summary['source_sha256'].items():
            self.assertEqual(hashlib.sha256((inventory.ROOT / path).read_bytes()).hexdigest(), digest, path)

    def test_generated_links_and_source_line_numbers(self):
        out = inventory.ROOT / 'docs/implementaciones'
        for page in [inventory.ROOT / 'docs/ImplementacionesDelJuego.md', *out.glob('detalle-*.md')]:
            for link in re.findall(r'(?<!\\)\]\(([^)]+)\)', page.read_text(encoding='utf-8')):
                path, _, fragment = link.partition('#')
                target = (page.parent / path).resolve()
                self.assertTrue(target.is_file(), (page, link))
                if fragment.startswith('L') and fragment[1:].isdigit():
                    self.assertLessEqual(int(fragment[1:]), len(target.read_text(encoding='utf-8-sig').splitlines()))
                elif fragment and target.suffix == '.md':
                    self.assertIn(f'id="{fragment}"', target.read_text(encoding='utf-8'), (page, link))


class HookAddressLinkTest(unittest.TestCase):
    SOURCE = (b'void X::InjectHooks() {' + bytes([10]) +
              b' RH_ScopedClass(X);' + bytes([10]) +
              b' //RH_ScopedInstall(Fake, 0x111111);' + bytes([10]) +
              b' RH_ScopedInstall(Plain, 0x401000);' + bytes([10]) +
              b' RH_ScopedOverloadedInstall(Draw, "Rect", 0x401010, void(X::*)(const CRect&, CRGBA));' + bytes([10]) +
              b' RH_ScopedOverloadedInstall(Draw, "Quad", 0x401020, void(X::*)(float, float) const);' + bytes([10]) +
              b' RH_ScopedNamespaceName("Rs");' + bytes([10]) +
              b' RH_ScopedNamedGlobalInstall(Impl_Eax, "Impl", 0x619510);' + bytes([10]) + b'}' + bytes([10]))

    def fn(self, name, signature, file='source/game_sa/X.cpp'):
        return {'function': name, 'signature': signature, 'file': file, 'catalog_match': None}

    def cat(self, cls, name, address, reversed_='1'):
        return {'class': cls, 'fn_name': name, 'address': address, 'reversed': reversed_, 'catalog': 'hooks.csv'}

    def test_registrations_are_parsed_with_scope_and_comments_ignored(self):
        regs = inventory.parse_registrations(self.SOURCE, 'source/game_sa/X.cpp')
        self.assertEqual([(r['scope'], r['display'], r['address']) for r in regs],
                         [('X', 'Plain', '0x00401000'), ('X', 'Draw-Rect', '0x00401010'),
                          ('X', 'Draw-Quad', '0x00401020'), ('Rs', 'Impl', '0x00619510')])
        self.assertTrue(regs[0]['member'])
        self.assertFalse(regs[3]['member'])

    def test_parameter_types_ignore_names_and_defaults(self):
        self.assertEqual(inventory.param_types('X::Draw(const CRect& r, CRGBA c = {})'), ['constCRect&', 'CRGBA'])
        self.assertEqual(inventory.param_types('void(X::*)(const CRect&, CRGBA)'), ['constCRect&', 'CRGBA'])
        self.assertEqual(inventory.param_types('void(*)(void)'), [])
        self.assertEqual(inventory.param_types('X::F(unsigned int, float f) const'), ['unsignedint', 'float'])

    def test_ambiguous_overload_is_resolved_by_cast_and_address(self):
        regs = inventory.parse_registrations(self.SOURCE, 'source/game_sa/X.cpp')
        a = self.fn('X::Draw', 'X::Draw(const CRect& r, CRGBA c)')
        b = self.fn('X::Draw', 'X::Draw(float x, float y) const')
        catalog = [self.cat('X', 'Draw-Rect', '0x00401010'), self.cat('X', 'Draw-Quad', '0x00401020'),
                   self.cat('X', 'Plain', '0x00401000')]
        inventory.link_by_registration([a, b], catalog, regs, set(), {'source/game_sa/X.cpp'})
        self.assertIs(a['catalog_match'], catalog[0])
        self.assertIs(b['catalog_match'], catalog[1])

    def test_unresolvable_ambiguity_is_not_guessed(self):
        regs = inventory.parse_registrations(self.SOURCE, 'source/game_sa/X.cpp')
        # Two bodies with the same name and no distinguishing cast: nothing may be linked.
        a = self.fn('X::Plain', 'X::Plain()')
        b = self.fn('X::Plain', 'X::Plain(int)', 'source/game_sa/X.h')
        catalog = [self.cat('X', 'Plain', '0x00401000')]
        inventory.link_by_registration([a, b], catalog, regs, set(), {'source/game_sa/X.cpp'})
        self.assertIsNone(a['catalog_match'])
        self.assertIsNone(b['catalog_match'])
        # A wrong address in the catalog also prevents the link.
        c = self.fn('X::Plain', 'X::Plain()')
        inventory.link_by_registration([c], [self.cat('X', 'Plain', '0x00402000')], regs, set(), {'source/game_sa/X.cpp'})
        self.assertIsNone(c['catalog_match'])

    def test_virtual_entry_is_anchored_by_vmt_registration_and_inline_class_macro(self):
        header = b'class NOTSA_EXPORT_VTABLE X : public Y { CEvent* Clone() const override { return new X; } };'
        raw = header
        masked = inventory.sanitized(raw)
        tree = Parser(Language(tree_sitter_cpp.language())).parse(masked)
        node = [n for n in inventory.walk(tree.root_node) if n.type == 'function_definition'][0]
        name = inventory.declarator_name(node.child_by_field_name('declarator'), raw)
        self.assertEqual(inventory.owner_name(node, raw, name, 'x.h'), ('X', 'X::Clone'))
        source = (b'void X::InjectHooks() { RH_ScopedVirtualClass(X, 0x85B400, 16);' + bytes([10]) +
                  b' RH_ScopedVMTInstall(Clone, 0x4B76D0); }' + bytes([10]))
        regs = inventory.parse_registrations(source, 'source/game_sa/X.cpp')
        self.assertTrue(regs[0]['virtual'])
        body = self.fn('X::Clone', 'Clone() const override', 'source/game_sa/X.h')
        other = self.fn('Z::Clone', 'Clone() const override', 'source/game_sa/Z.h')
        catalog = [self.cat('X', 'Clone', '0x00000000')]
        inventory.link_by_registration([body, other], catalog, regs, set(), {'source/game_sa/X.cpp'})
        self.assertIs(body['catalog_match'], catalog[0])
        self.assertIsNone(other['catalog_match'])

    def test_scope_macro_inside_a_block_does_not_leak(self):
        source = (b'void F() { RH_ScopedVirtualClass(A, 0x1, 1);' + bytes([10]) +
                  b' { RH_ScopedClass(B); RH_ScopedInstall(Inner, 0x10); }' + bytes([10]) +
                  b' RH_ScopedVMTInstall(Outer, 0x20); }' + bytes([10]))
        regs = inventory.parse_registrations(source, 'source/game_sa/A.cpp')
        self.assertEqual([(r['scope'], r['display']) for r in regs], [('B', 'Inner'), ('A', 'Outer')])

    def test_body_outside_game_sa_is_linked_by_registration(self):
        source = (b'void InjectHooks() { RH_ScopedNamespaceName("Rs");' + bytes([10]) +
                  b' RH_ScopedGlobalInstall(RsTimer, 0x619410); RH_ScopedGlobalInstall(RsGone, 0x619420); }' + bytes([10]))
        regs = inventory.parse_registrations(source, 'source/app/platform/platform.cpp')
        body = self.fn('RsTimer', 'RsTimer()', 'source/app/platform/platform.cpp')
        catalog = [self.cat('Rs', 'RsTimer', '0x00619410'), self.cat('Rs', 'RsGone', '0x00619420')]
        matched = set()
        inventory.link_by_registration([body], catalog, regs, matched, {'source/app/platform/platform.cpp'})
        self.assertIs(body['catalog_match'], catalog[0])
        # An entry whose registration exists but has no scanned body stays unlinked and is reported as outside game_sa.
        self.assertEqual(list(inventory.outside_game_sa(catalog, regs, matched)), [id(catalog[1])])
        # Files that are not scanned never provide bodies.
        spare = self.fn('RsTimer', 'RsTimer()', 'source/app/platform/platform.cpp')
        inventory.link_by_registration([spare], catalog[:1], regs, set(), set())
        self.assertIsNone(spare['catalog_match'])

    def test_published_inventory_scope_and_unlinked_outside_entries(self):
        out = inventory.ROOT / 'docs/implementaciones'
        with (out / 'funciones.csv').open(encoding='utf-8', newline='') as stream:
            functions = list(csv.DictReader(stream))
        with (out / 'catalogo-fuera-de-game_sa.csv').open(encoding='utf-8', newline='') as stream:
            outside = list(csv.DictReader(stream))
        self.assertTrue(all(r['file'].startswith(('source/game_sa/', 'source/app/')) for r in functions))
        self.assertTrue(any(r['file'].startswith('source/app/') and r['catalog'] == 'hooks.csv' for r in functions))
        self.assertEqual(list(functions[0])[-1], 'catalog')
        linked = {(r['catalog_identity'], r['catalog_address']) for r in functions}
        for r in outside:
            self.assertFalse(r['registered_in'].startswith('source/game_sa/'))
            self.assertNotIn((r['class'] + '::' + r['fn_name'], inventory.canonical(r['address'])), linked)


class TemplateAndParserLinkTest(unittest.TestCase):
    def definitions(self, source):
        raw = source.encode()
        masked = inventory.sanitized(raw)
        tree = Parser(Language(tree_sitter_cpp.language())).parse(masked)
        return raw, tree, [n for n in inventory.walk(tree.root_node) if n.type in {'function_definition', 'field_declaration'}]

    def fn(self, name, signature, file='source/game_sa/X.h', templated=False, header='', **extra):
        return dict({'function': name, 'signature': signature, 'file': file, 'catalog_match': None,
                     'templated': templated, 'template_header': header}, **extra)

    def cat(self, cls, name, address, catalog='hooks.csv'):
        return {'class': cls, 'fn_name': name, 'address': address, 'reversed': '1', 'validated': '1', 'catalog': catalog}

    def link(self, functions, catalog, source, bases=None, aliases=None, file='source/game_sa/X.cpp'):
        regs = inventory.parse_registrations(source, file)
        inventory.link_by_registration(functions, catalog, regs, set(), {file}, bases, aliases)

    def test_strip_templates(self):
        self.assertEqual(inventory.strip_templates('CWorld::F<CPtrListDoubleLink<CPhysical*>>'), 'CWorld::F')
        self.assertEqual(inventory.strip_templates('CQuadTreeNode<T>::Get'), 'CQuadTreeNode::Get')
        self.assertEqual(inventory.strip_templates('operator<'), 'operator<')

    def test_instantiation_in_name_links_to_the_unique_generic_template_body(self):
        generic = self.fn('CWorld::Sector', 'CWorld::Sector(PtrListType& list)', templated=True, header='<typename PtrListType>')
        plain = self.fn('CWorld::Sector', 'CWorld::Sector()')
        catalog = [self.cat('CWorld', 'Sector<CPtrListDoubleLink<CPhysical*>>-', '0x00563950')]
        source = (b'void W::InjectHooks() { RH_ScopedClass(CWorld);' + bytes([10]) +
                  b' RH_ScopedOverloadedInstall(Sector<CPtrListDoubleLink<CPhysical*>>, "", 0x563950, void(*)(CPtrListDoubleLink<CPhysical*>&)); }')
        self.link([generic, plain], catalog, source)
        self.assertIs(generic['catalog_match'], catalog[0])
        self.assertIsNone(plain['catalog_match'])

    def test_template_constraint_selects_between_overloads(self):
        integral = self.fn('U::Get', 'U::Get(T min, T max)', templated=True, header='<std::integral T>')
        floating = self.fn('U::Get', 'U::Get(T min, T max)', templated=True, header='<typename T> requires std::is_floating_point_v<T>')
        catalog = [self.cat('U', 'Get<int32>-int', '0x4D9C10'), self.cat('U', 'Get<float>-float', '0x4D9C50')]
        source = (b'void U::InjectHooks() { RH_ScopedClass(U);' + bytes([10]) +
                  b' RH_ScopedOverloadedInstall(Get<int32>, "int", 0x4d9c10, int32(*)(int32, int32));' + bytes([10]) +
                  b' RH_ScopedOverloadedInstall(Get<float>, "float", 0x4d9c50, float(*)(float, float)); }')
        self.link([integral, floating], catalog, source)
        self.assertIs(integral['catalog_match'], catalog[0])
        self.assertIs(floating['catalog_match'], catalog[1])

    def test_template_parameter_wildcard_and_typedef_alias_in_casts(self):
        wide = self.fn('S::Run', 'S::Run(L& list, float a, int32 flags)', templated=True, header='<typename L>')
        narrow = self.fn('S::Run', 'S::Run(L& list, int32 flags)', templated=True, header='<typename L>')
        catalog = [self.cat('S', 'Run<Single<E*>>-', '0x40C270')]
        source = (b'void S::InjectHooks() { RH_ScopedClass(S);' + bytes([10]) +
                  b' RH_ScopedOverloadedInstall(Run<Single<E*>>, "", 0x40C270, void(*)(Single<E*>&, float, int32)); }')
        self.link([wide, narrow], catalog, source)
        self.assertIs(wide['catalog_match'], catalog[0])
        self.assertIsNone(narrow['catalog_match'])
        a = self.fn('C::Set', 'C::Set(RwIm2DVertex* v, const CRect& r)')
        b = self.fn('C::Set', 'C::Set(const CRect& r)')
        catalog = [self.cat('C', 'Set-Vertex', '0x727A00')]
        source = (b'void C::InjectHooks() { RH_ScopedClass(C);' + bytes([10]) +
                  b' RH_ScopedOverloadedInstall(Set, "Vertex", 0x727A00, void(*)(RwD3D9Vertex*, const CRect&)); }')
        self.link([a, b], catalog, source, aliases={'RwIm2DVertex': 'RwD3D9Vertex'})
        self.assertIs(a['catalog_match'], catalog[0])

    def test_inherited_template_member_serves_every_derived_entry_through_the_shared_file(self):
        body = self.fn('Seek::Clone', 'Clone() const override', templated=True, header='<typename T>')
        catalog = [self.cat('Seek<A>', 'Clone', '0x00000000'), self.cat('SeekStd', 'Clone', '0x00000000')]
        source = (b'void Seek::InjectHooks() { RH_ScopedVirtualClass(Seek<A>, 0x1, 1); RH_ScopedVMTInstall(Clone, 0x10); }' + bytes([10]) +
                  b'void SeekStd::InjectHooks() { RH_ScopedVirtualClass(SeekStd, 0x2, 1); RH_ScopedVMTInstall(Clone, 0x20); }')
        self.link([body], catalog, source, bases={'SeekStd': ('Seek',)})
        self.assertIs(body['catalog_match'], catalog[0])
        self.assertEqual([c for c, _ in body['shared']], [catalog[1]])
        self.assertEqual(body['shared'][0][1], 'plantilla heredada')

    def test_shared_body_requires_a_template(self):
        body = self.fn('Plain::Clone', 'Clone() const override')
        catalog = [self.cat('Plain', 'Clone', '0x00000000'), self.cat('Kid', 'Clone', '0x00000000')]
        source = (b'void P::InjectHooks() { RH_ScopedVirtualClass(Plain, 0x1, 1); RH_ScopedVMTInstall(Clone, 0x10); }' + bytes([10]) +
                  b'void K::InjectHooks() { RH_ScopedVirtualClass(Kid, 0x2, 1); RH_ScopedVMTInstall(Clone, 0x20); }')
        self.link([body], catalog, source, bases={'Kid': ('Plain',)})
        self.assertIsNone(body['catalog_match'])
        self.assertNotIn('shared', body)

    def test_catalog_entry_without_address_accepts_its_single_registration(self):
        body = self.fn('T::Cb', 'T::Cb(int a)', 'source/game_sa/T.cpp')
        catalog = [self.cat('T', 'Cb', '0x00000000')]
        self.link([body], catalog, b'void T::InjectHooks() { RH_ScopedClass(T); RH_ScopedInstall(Cb, 0x675FC0); }')
        self.assertIs(body['catalog_match'], catalog[0])

    def test_parser_workarounds_keep_definitions_visible(self):
        # Casts inside registrations used to swallow every following definition.
        source = ('void P::InjectHooks() { RH_ScopedOverloadedInstall(Get, "ped", 0x1, int16 (CPad::*)(CPed*) const); }\n'
                  'void P::Clear() { x = 1; }\n'
                  'C2dEffect* __stdcall P::Compute(CPed const& ped) { return nullptr; }\n'
                  'void* P::Plugin(void* o) { *RWPLUGINOFFSET(Data*, o, off) = &fake; return o; }\n'
                  'class NOTSA_EXPORT_VTABLE Base { public: virtual void Update(int a) { /* empty */ }; };\n')
        raw, tree, nodes = self.definitions(source)
        names = {inventory.declarator_name(n.child_by_field_name('declarator'), raw) for n in nodes if n.child_by_field_name('declarator')}
        for expected in ('P::InjectHooks', 'P::Clear', 'P::Compute', 'P::Plugin', 'Update'):
            self.assertIn(expected, names)
        update = [n for n in nodes if n.type == 'field_declaration'][0]
        self.assertEqual(inventory.owner_name(update, raw, 'Update', 'x.h')[1], 'Base::Update')


if __name__ == '__main__':
    unittest.main()
