import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from audit_hook_catalog import compare_runtime, find_inherited_calls


class InheritedInjectHooksTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        (self.root / 'source').mkdir()

    def write(self, name, text):
        (self.root / 'source' / name).write_text(text, encoding='utf-8')

    def test_detecta_llamada_que_usa_el_injecthooks_heredado(self):
        self.write('Base.h', 'class NOTSA_EXPORT_VTABLE CBase {\npublic:\n    static void InjectHooks();\n};\n')
        self.write('Base.cpp', 'void CBase::InjectHooks() {\n}\n')
        self.write('Derived.h', 'class CDerived : public CBase {\npublic:\n    void Foo();\n};\n')
        calls = find_inherited_calls(self.root, {'CBase', 'CDerived'})
        self.assertEqual(calls, [('CDerived', ['resuelve a CBase::InjectHooks heredado'])])

    def test_no_marca_clase_con_injecthooks_propio(self):
        self.write('Base.h', 'class CBase {\npublic:\n    static void InjectHooks();\n};\n')
        self.write('Derived.h', 'class CDerived : public CBase {\npublic:\n    static void InjectHooks();\n};\n')
        self.assertEqual(find_inherited_calls(self.root, {'CBase', 'CDerived'}), [])

    def test_herencia_de_varios_niveles(self):
        self.write('A.h', 'class CA {\n    static void InjectHooks();\n};\n')
        self.write('B.h', 'class CB : public CA {\n};\n')
        self.write('C.h', 'class CC final : public CB {\n};\n')
        self.assertEqual(find_inherited_calls(self.root, {'CC'}), [('CC', ['resuelve a CA::InjectHooks heredado'])])


class RuntimeComparisonTests(unittest.TestCase):
    HEADER = 'class,fn_name,address,reversed,locked,validated,type\n'

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)

    def write(self, name, rows):
        path = self.root / name
        path.write_text(self.HEADER + ''.join(r + '\n' for r in rows), encoding='utf-8')
        return path

    def test_detecta_ausencias_reversed_y_direccion(self):
        catalog = self.write('hooks.csv', ['A,f,0x00401000,1,0,0,S', 'A,g,0x00402000,1,0,0,S', 'B,v,0x00000000,1,0,0,V', 'C,h,0x00403000,1,0,0,S'])
        runtime = self.write('runtime.csv', ['A,f,0x00401000,0,1,0,S', 'A,g,0x00402010,1,0,0,S', 'B,v,0x00405000,1,0,0,V', 'D,x,0x00406000,1,0,0,S'])
        result = dict((k, v[0]) for k, v in compare_runtime(catalog, runtime))
        self.assertEqual(result, {
            'D/x': 'registrado en el juego, ausente de hooks.csv',
            'C/h': 'en hooks.csv, no registrado en el juego',
            'A/f': 'reversed: hooks.csv=1, juego=0',
            'A/g': 'dirección: hooks.csv=0x00402000, juego=0x00402010',
        })

    def test_catalogo_igual_no_da_diferencias(self):
        rows = ['A,f,0x00401000,1,0,0,S', 'B,v,0x00000000,1,0,0,V']
        self.assertEqual(compare_runtime(self.write('a.csv', rows), self.write('b.csv', rows)), [])


if __name__ == '__main__':
    unittest.main()
