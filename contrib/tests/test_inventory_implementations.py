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
        inventory.link_by_registration([a, b], catalog, regs, set())
        self.assertIs(a['catalog_match'], catalog[0])
        self.assertIs(b['catalog_match'], catalog[1])

    def test_unresolvable_ambiguity_is_not_guessed(self):
        regs = inventory.parse_registrations(self.SOURCE, 'source/game_sa/X.cpp')
        # Two bodies with the same name and no distinguishing cast: nothing may be linked.
        a = self.fn('X::Plain', 'X::Plain()')
        b = self.fn('X::Plain', 'X::Plain(int)', 'source/game_sa/X.h')
        catalog = [self.cat('X', 'Plain', '0x00401000')]
        inventory.link_by_registration([a, b], catalog, regs, set())
        self.assertIsNone(a['catalog_match'])
        self.assertIsNone(b['catalog_match'])
        # A wrong address in the catalog also prevents the link.
        c = self.fn('X::Plain', 'X::Plain()')
        inventory.link_by_registration([c], [self.cat('X', 'Plain', '0x00402000')], regs, set())
        self.assertIsNone(c['catalog_match'])

    def test_registrations_outside_game_sa_are_excluded_not_linked(self):
        regs = inventory.parse_registrations(self.SOURCE, 'source/app/platform/platform.cpp')
        body = self.fn('Impl_Eax', 'Impl_Eax()', 'source/app/platform/platform.cpp')
        catalog = [self.cat('Rs', 'Impl', '0x00619510')]
        matched = set()
        inventory.link_by_registration([body], catalog, regs, matched)
        self.assertIsNone(body['catalog_match'])
        self.assertEqual(list(inventory.outside_game_sa(catalog, regs, matched)), [id(catalog[0])])

    def test_published_scope_is_game_sa_only_and_outside_entries_are_not_counted(self):
        out = inventory.ROOT / 'docs/implementaciones'
        with (out / 'funciones.csv').open(encoding='utf-8', newline='') as stream:
            functions = list(csv.DictReader(stream))
        with (out / 'catalogo-fuera-de-game_sa.csv').open(encoding='utf-8', newline='') as stream:
            outside = list(csv.DictReader(stream))
        self.assertTrue(all(r['file'].startswith('source/game_sa/') for r in functions))
        self.assertTrue(outside)
        linked = {(r['catalog_identity'], r['catalog_address']) for r in functions}
        for r in outside:
            self.assertFalse(r['registered_in'].startswith('source/game_sa/'))
            self.assertNotIn((r['class'] + '::' + r['fn_name'], inventory.canonical(r['address'])), linked)


if __name__ == '__main__':
    unittest.main()
