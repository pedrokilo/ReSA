"""Dump selected COFF functions as evidence; this does not validate runtime behavior."""
import argparse
import hashlib
import json
import struct
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32


def inspect(path, names):
    data = path.read_bytes()
    big = data[:4] == b'\x00\x00\xff\xff'
    if big:
        sections, symbols, count = struct.unpack_from('<III', data, 44)
        header, width = 56, 20
    else:
        machine, sections, _, symbols, count, optional, _ = struct.unpack_from('<HHIIIHH', data)
        if machine != 0x14c:
            raise ValueError('Expected x86 COFF')
        header, width = 20 + optional, 18
    strings = symbols + count * width
    table = []
    for i in range(sections):
        pos = header + i * 40
        size, raw = struct.unpack_from('<II', data, pos + 16)
        table.append(data[raw:raw + size])
    entries = []
    i = 0
    while i < count:
        pos = symbols + i * width
        rawname = data[pos:pos + 8]
        if rawname[:4] == b'\x00' * 4:
            offset = strings + struct.unpack_from('<I', rawname, 4)[0]
            end = data.find(b'\x00', offset)
            if end < 0:
                raise ValueError('Unterminated symbol')
            name = data[offset:end].decode('utf-8', errors='replace')
        else:
            name = rawname.rstrip(b'\x00').decode('utf-8', errors='replace')
        value = struct.unpack_from('<I', data, pos + 8)[0]
        section = struct.unpack_from('<i' if big else '<h', data, pos + 12)[0]
        kind = struct.unpack_from('<H', data, pos + (16 if big else 14))[0]
        aux = data[pos + width - 1]
        if 0 < section <= sections and kind & 0x20:
            entries.append((name, section, value))
        i += 1 + aux
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    result = []
    for name, section, value in entries:
        if names and not any(n in name for n in names):
            continue
        limits = [v for _, s, v in entries if s == section and v > value]
        end = min(limits, default=len(table[section - 1]))
        instructions = [dict(offset=ins.address, bytes=ins.bytes.hex(), mnemonic=ins.mnemonic,
                             operands=ins.op_str)
                        for ins in decoder.disasm(table[section - 1][value:end], value)]
        result.append(dict(symbol=name, section=section, instructions=instructions))
    return dict(object_sha256=hashlib.sha256(data).hexdigest(), functions=result,
                limitation='Relocations unresolved; function extents inferred from COFF symbols. No runtime approval.')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('object', type=Path)
    parser.add_argument('symbols', nargs='*')
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    payload = json.dumps(inspect(args.object, args.symbols), indent=2)
    if args.output:
        args.output.write_text(payload + '\n', encoding='utf-8')
    else:
        print(payload)
