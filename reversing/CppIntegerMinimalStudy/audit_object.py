"""Exact complete i386 COFF body audit against canonical EXE section bytes."""
import argparse
import hashlib
import json
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'CppTrigCoffAudit'))
from compare_coff import parse, function

TARGETS = (
    ('?Native_00424100@@YA_NMM@Z', 0x424100, '32c0c3'),
    ('?SetUseD3DResourceBuffering@D3DResourceSystem@@SAX_N@Z', 0x730AC0, '8a442404a284608d00c3'),
)

def need(value, label):
    if not value: raise ValueError(label)

def pe_bytes(data, address, size):
    need(data[:2] == b'MZ', 'DOS image')
    pe = struct.unpack_from('<I', data, 0x3C)[0]
    need(data[pe:pe+4] == b'PE\0\0', 'PE signature')
    machine, count = struct.unpack_from('<HH', data, pe+4)
    need(machine == 0x14C, 'i386 canonical EXE')
    opt = pe+24; need(struct.unpack_from('<H',data,opt)[0] == 0x10B, 'PE32')
    base = struct.unpack_from('<I',data,opt+28)[0]
    section_start = opt + struct.unpack_from('<H',data,pe+20)[0]
    rva = address-base
    for i in range(count):
        s = section_start+i*40
        _, va, raw_size, raw = struct.unpack_from('<IIII',data,s+8)
        flags = struct.unpack_from('<I',data,s+36)[0]
        if va <= rva and rva+size <= va+raw_size:
            need(flags & 0x20000000, 'executable section')
            return data[raw+rva-va:raw+rva-va+size]
    raise ValueError('Target outside executable file section')

def audit(obj, exe, expected_sha=None):
    original = Path(obj).read_bytes()
    digest = hashlib.sha256(original).hexdigest()
    need(expected_sha is None or digest == expected_sha, 'unexpected object SHA256')
    data, symbols, sections = parse(obj)
    need(data == original, 'object changed during read')
    game = Path(exe).read_bytes(); records=[]
    for symbol, address, expected in TARGETS:
        record = function(obj, symbol, {})
        need(record['object_sha256'] == digest, 'object changed while parsing functions')
        need(record['bytes'] == expected and record['size'] == len(bytes.fromhex(expected)), 'complete body or extent differs: '+symbol)
        need(record['relocations'] == [], 'function relocations: '+symbol)
        native = pe_bytes(game,address,record['size'])
        need(native.hex() == expected, 'canonical EXE body differs')
        records.append({'symbol':symbol,'game_address':hex(address),'complete_extent':True,'relocations_zero':True,
                        'exact_exe_bytes':True,'size':record['size'],'bytes':record['bytes']})
    need(Path(obj).read_bytes() == original, 'object changed before completion')
    return {'all_exact':True,'object_sha256':digest,'exe_sha256':hashlib.sha256(game).hexdigest(),
            'coff_parser_sha256':hashlib.sha256(Path(__file__).resolve().parents[1].joinpath('CppTrigCoffAudit/compare_coff.py').read_bytes()).hexdigest(),
            'functions':records}

def main():
    ap=argparse.ArgumentParser();ap.add_argument('object');ap.add_argument('--exe',required=True)
    ap.add_argument('--expected-sha256');ap.add_argument('--output')
    a=ap.parse_args();r=audit(a.object,a.exe,a.expected_sha256)
    text=json.dumps(r,indent=2)
    if a.output: Path(a.output).write_text(text,encoding='utf-8')
    print(text)

if __name__=='__main__': main()
