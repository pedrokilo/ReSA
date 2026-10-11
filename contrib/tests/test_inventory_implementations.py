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


if __name__ == '__main__':
    unittest.main()
