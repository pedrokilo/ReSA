"""Regresiones del extractor: no confundir llamadas con funciones ni extras con direcciones."""
import importlib.util
import tempfile
import unittest
from pathlib import Path

spec=importlib.util.spec_from_file_location('memory_map',Path(__file__).resolve().parents[1]/'build-memory-map.py')
module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)

class SourceMapTests(unittest.TestCase):
    def test_scopes_calls_overloads_and_references(self):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory);(root/'source').mkdir()
            (root/'source/sample.cpp').write_text('''
namespace game {
class CBase {};
class CThing : public CBase {
public:
    void Get(int n);
    void Inline() { Other::Call(); }
    inline static auto& Value = StaticRef<int>(0xB70000);
};
void CThing::Get(int n) {
    Other::Call(n);
}
void CThing::InjectHooks() {
    RH_ScopedVirtualClass(game::CThing, 0x860000, 2);
    RH_ScopedOverloadedInstall(Get, "", 0x530000, void(CThing::*)(int));
    // RH_ScopedInstall(NotActive, 0x540000);
}
}
''',encoding='utf-8')
            types,functions,tables,registrations,globals_,files=module.scan(root)
            self.assertEqual({t['name'] for t in types},{'game::CBase','game::CThing'})
            self.assertFalse(any('Call' in f['name'] for f in functions))
            self.assertTrue(any(f['name']=='CThing::Get' for f in functions))
            self.assertEqual(registrations[0]['name'],'Get-')
            self.assertEqual(len(registrations),1)
            self.assertEqual(tables[0]['slots_declared'],'2')
            self.assertEqual(globals_[0]['va'],'0x00B70000')
            self.assertEqual(len(files),1)

    def test_comma_in_signature_is_not_an_argument_separator(self):
        self.assertEqual(module.split_args('Get, "a,b", 0x530000, void(T::*)(int, int)'),
                         ['Get','"a,b"','0x530000','void(T::*)(int, int)'])

if __name__=='__main__': unittest.main()
