"""Comprueba cobertura, identidad del inventario y slots leídos del EXE."""
import csv,json,sys,unittest
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'contrib'));sys.dont_write_bytecode=True
from pe_inventory import PE

class BinaryMapTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.directory=ROOT/'docs/MemoryMap'
        cls.summary=json.loads((cls.directory/'binary_summary.json').read_text())
    def rows(self,name):
        with (self.directory/name).open(encoding='utf-8-sig',newline='') as f:return list(csv.DictReader(f))
    def test_function_addresses_and_catalogue_are_distinct(self):
        functions=self.rows('binary_functions.csv')
        self.assertEqual(len(functions),self.summary['ghidra_function_entries'])
        self.assertEqual(len({f['va'] for f in functions}),len(functions))
        self.assertTrue(all(int(f['va'],16)-int(f['rva'],16)==0x400000 for f in functions))
        self.assertEqual(sum(not f['catalogue_names'] for f in functions),self.summary['without_catalogue_names'])
        self.assertGreater(len(functions),len(self.rows('functions.csv')))
    def test_executable_byte_accounting_and_gap_bounds(self):
        sections=json.loads((self.directory/'binary_sections.json').read_text())
        executable={s['name']:s for s in sections if s['executable']}
        gaps=self.rows('binary_unassigned_ranges.csv')
        self.assertEqual(sum(int(g['bytes']) for g in gaps),self.summary['bytes_without_function'])
        self.assertEqual(self.summary['function_body_bytes']+self.summary['bytes_without_function'],self.summary['executable_file_bytes'])
        previous={}
        for g in gaps:
            s=executable[g['section']];start=int(g['va'],16);end=int(g['end_va'],16)
            self.assertEqual(end-start+1,int(g['bytes']))
            self.assertGreaterEqual(start,int(s['va'],16));self.assertLess(end,int(s['va'],16)+s['raw_size'])
            self.assertGreater(start,previous.get(g['section'],-1));previous[g['section']]=end
    def test_all_virtual_destinations_match_exe_bytes(self):
        exe=Path('D:/Grand Theft Auto San Andreas/gta-sa.exe')
        if not exe.is_file():self.skipTest('Se requiere el EXE canónico local para leer los slots.')
        pe=PE(exe);slots=self.rows('binary_slots.csv')
        self.assertEqual(len(slots),self.summary['source_vtable_slots_verified'])
        for slot in slots:
            pointer=pe.words(int(slot['vtable_va'],16)+4*int(slot['slot']),1)
            self.assertEqual(pointer[0],int(slot['function_va'],16));self.assertTrue(pe.code(pointer[0]))

if __name__=='__main__':unittest.main()
