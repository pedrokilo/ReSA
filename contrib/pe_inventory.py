"""Static x86 PE/RTTI extraction; no game execution or reconstruction."""
import csv, hashlib, struct, re
from collections import defaultdict
from pathlib import Path
CANONICAL = '72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
def hx(n): return f'0x{n:08x}'
def write_csv(path, rows, fields):
    with path.open('w', encoding='utf-8', newline='') as file:
        out = csv.DictWriter(file, fieldnames=fields, extrasaction='ignore')
        out.writeheader(); out.writerows(rows)

class PE:
    def __init__(self, path):
        self.data = path.read_bytes()
        if hashlib.sha256(self.data).hexdigest() != CANONICAL: raise ValueError('EXE no canónico')
        pe = struct.unpack_from('<I', self.data, 60)[0]
        if self.data[:2] != b'MZ' or self.data[pe:pe+4] != b'PE\0\0': raise ValueError('PE inválido')
        machine, count = struct.unpack_from('<HH', self.data, pe+4)
        opt_size = struct.unpack_from('<H', self.data, pe+20)[0]
        if machine != 0x14c: raise ValueError('No es x86')
        self.base = struct.unpack_from('<I', self.data, pe+52)[0]
        self.sections = []
        for i in range(count):
            pos = pe+24+opt_size+i*40
            name = self.data[pos:pos+8].split(b'\0')[0].decode('ascii', 'replace')
            vs, rva, raw_size, raw = struct.unpack_from('<IIII', self.data, pos+8)
            flags = struct.unpack_from('<I', self.data, pos+36)[0]
            self.sections.append(dict(name=name, rva=rva, va=self.base+rva, virtual_size=vs,
                raw_size=raw_size, raw_offset=raw, executable=bool(flags & 0x20000000),
                writable=bool(flags & 0x80000000), readable=bool(flags & 0x40000000)))
    def section(self, va):
        return next((s for s in self.sections if s['va'] <= va < s['va']+max(s['virtual_size'],s['raw_size'])), None)
    def offset(self, va, length=1):
        s = self.section(va)
        if s and va-s['va']+length <= s['raw_size']: return s['raw_offset']+va-s['va']
        return None
    def file_va(self, pos):
        s = next((s for s in self.sections if s['raw_offset'] <= pos < s['raw_offset']+s['raw_size']), None)
        return s['va']+pos-s['raw_offset'] if s else None
    def words(self, va, count):
        pos = self.offset(va, count*4)
        return struct.unpack_from('<'+'I'*count, self.data, pos) if pos is not None else None
    def code(self, va):
        s = self.section(va)
        return bool(s and s['executable'] and self.offset(va) is not None)
    def pointers(self):
        for s in self.sections:
            if s['executable']: continue
            data = self.data[s['raw_offset']:s['raw_offset']+s['raw_size']//4*4]
            for i, (value,) in enumerate(struct.iter_unpack('<I', data)):
                yield s['va']+i*4, value

def read_ghidra(pe, directory):
    functions = []
    with (directory/'ghidra_functions.csv').open(encoding='utf-8-sig', newline='') as file:
        for row in csv.DictReader(file):
            va = int(row['va'],16)
            if not pe.code(va): continue
            row.update(rva=hx(va-pe.base), section=pe.section(va)['name'], status='ghidra_candidate')
            functions.append(row)
    symbols = {}
    with (directory/'ghidra_symbols.csv').open(encoding='utf-8-sig', newline='') as file:
        for row in csv.DictReader(file):
            va = int(row['va'],16)
            if pe.section(va) and row['source'] != 'DEFAULT': symbols[va] = row['name']
    return functions, symbols

def rtti_map(pe, symbols):
    classes = {}
    for match in re.finditer(rb'\.\?A[UV][^\x00\r\n]{1,500}@@\x00',pe.data):
        va = pe.file_va(match.start()-8)
        if va is None or va % 4: continue
        words = pe.words(va,2)
        if not words or words[1] != 0 or not pe.section(words[0]): continue
        mangled = match.group()[:-1].decode('ascii','replace')
        label = symbols.get(va,'')
        if label.endswith('::RTTI_Type_Descriptor'): name = label[:-len('::RTTI_Type_Descriptor')]
        elif '?$' not in mangled: name = '::'.join(reversed(mangled[4:-2].split('@')))
        else: name = mangled
        classes[va] = dict(name=name,mangled=mangled,kind='class' if mangled[3]=='V' else 'struct',
            type_descriptor_va=hx(va),type_descriptor_rva=hx(va-pe.base),
            status='exe_rtti_type',section=pe.section(va)['name'], locators=[],vtables=[],bases=[])
    locators = {}
    for reference, value in pe.pointers():
        if value not in classes: continue
        col = reference-12
        words = pe.words(col,5)
        if not words: continue
        signature, displacement, constructor_disp, td, chd = words
        if signature != 0 or displacement > 0x100000 or constructor_disp > 0x100000: continue
        hierarchy = pe.words(chd,4)
        if not hierarchy: continue
        hs, attributes, count, base_array = hierarchy
        if hs != 0 or attributes > 0xff or not 1 <= count <= 4096: continue
        base_pointers = pe.words(base_array,count)
        if not base_pointers: continue
        base_entries = []
        for bcd in base_pointers:
            entry = pe.words(bcd,6)
            if not entry or entry[0] not in classes: break
            base_entries.append(classes[entry[0]]['name'])
        else:
            if base_entries[0] != classes[td]['name']: continue
            locators[col] = (td, displacement)
            classes[td]['locators'].append(hx(col))
            classes[td]['bases'] = list(dict.fromkeys(base_entries[1:]))
    tables = []; slots = []; seen = set()
    for reference, value in pe.pointers():
        if value not in locators: continue
        table_va = reference+4
        if table_va in seen: continue
        entries = []
        for slot in range(512):
            word = pe.words(table_va+slot*4,1)
            if not word or not pe.code(word[0]): break
            entries.append(word[0])
        if not entries: continue
        seen.add(table_va)
        td, displacement = locators[value]
        owner = classes[td]['name']
        classes[td]['vtables'].append(hx(table_va))
        classes[td]['status'] = 'exe_rtti_with_vtable'
        tables.append(dict(owner=owner,va=hx(table_va),rva=hx(table_va-pe.base),
            locator_va=hx(value),object_offset=displacement,slots=len(entries),
            status='rtti_structural',truncated=len(entries)==512))
        for index, function in enumerate(entries): slots.append(dict(owner=owner,vtable_va=hx(table_va),
            slot=index,function_va=hx(function),function_rva=hx(function-pe.base),status='rtti_slot'))
    return list(classes.values()), tables, slots
