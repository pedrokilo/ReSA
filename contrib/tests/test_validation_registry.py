import copy
import json
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from validation_registry import evaluate, digest, tree_digest


class RegistryTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        (self.root / 'hooks.csv').write_text('class,fn_name,address,type\nC,F,0x01,S\n')
        (self.root / 'e.json').write_text('{"passed":true}')
        (self.root / 'source.cpp').write_text('tested source')
        (self.root / 'review.md').write_text('Reviewed dependency closure and build configuration')
        (self.root / 'dep.h').write_text('tested dependency')
        self.record = dict(hook=dict(zip(('class','fn_name','address','type'), ('C','F','0x01','S'))),
                           evidence=dict(path='e.json', sha256=digest(self.root/'e.json'), passed_path=['passed']),
                           tested_sources={'source.cpp':digest(self.root/'source.cpp')},
                           dependency_review=dict(evidence_path='review.md', evidence_sha256=digest(self.root/'review.md'),
                                                  files={'dep.h':digest(self.root/'dep.h')}), scope='fixture')

    def run_registry(self, records=None):
        path = self.root / 'registry.json'
        path.write_text(json.dumps(dict(schema_version=1, functions=records if records is not None else [self.record])))
        return evaluate(self.root, self.root/'hooks.csv', path)

    def test_complete_evidence_counts_once(self):
        self.assertEqual(self.run_registry()['current'], 1)

    def test_pending_dependencies_not_current(self):
        self.record['dependency_review'] = None
        result = self.run_registry()
        self.assertEqual((result['documented'], result['source_matches'], result['current']), (1,1,0))

    def test_changed_inputs_remove_current(self):
        for name in ('source.cpp', 'dep.h', 'review.md', 'e.json'):
            with self.subTest(name=name):
                p = self.root/name
                old = p.read_bytes()
                p.write_bytes(b'changed')
                self.assertEqual(self.run_registry()['current'], 0)
                p.write_bytes(old)

    def test_missing_evidence(self):
        (self.root/'e.json').unlink()
        self.assertEqual(self.run_registry()['documented'], 0)

    def test_false_result_not_accepted(self):
        (self.root/'e.json').write_text('{"passed":false}')
        self.record['evidence']['sha256'] = digest(self.root/'e.json')
        self.assertEqual(self.run_registry()['documented'], 0)

    def test_duplicate_rejected(self):
        with self.assertRaises(ValueError):
            self.run_registry([self.record, copy.deepcopy(self.record)])

    def test_unknown_hook_rejected(self):
        self.record['hook']['address'] = '0x02'
        with self.assertRaises(ValueError):
            self.run_registry()

    def test_escape_rejected(self):
        self.record['evidence']['path'] = '../outside.json'
        with self.assertRaises(ValueError):
            self.run_registry()

    def test_source_tree_add_change_delete_invalidates(self):
        tree = self.root / 'source'
        tree.mkdir()
        file = tree / 'header.h'
        file.write_text('original')
        self.record['dependency_review']['trees'] = {'source': tree_digest(tree)}
        self.assertEqual(self.run_registry()['current'], 1)
        file.write_text('changed')
        self.assertEqual(self.run_registry()['current'], 0)
        file.write_text('original')
        extra = tree / 'added.h'
        extra.write_text('new')
        self.assertEqual(self.run_registry()['current'], 0)
        extra.unlink()
        file.unlink()
        self.assertEqual(self.run_registry()['current'], 0)


if __name__ == '__main__':
    unittest.main()
