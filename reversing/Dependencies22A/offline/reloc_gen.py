import struct, sys, capstone
img = open('exe_image.bin','rb').read()
BASE = 0x400000
LO, HI = 0x400000, 0xCB1000
ranges = [(0x6FFBB0, 0x6FFE6F), (0x6FFE70, 0x6FFFDC), (0x6FFFF0, 0x7000D8), (0x7002D0, 0x700BDC), (0x420800, 0x420818), (0x821B40, 0x821BE0)]
if len(sys.argv) > 1 and sys.argv[1] == 'show':
    pass
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True
patches = []   # (address of 4-byte field)
for lo, hi in ranges:
    code = img[lo - BASE: hi - BASE + 1]
    for i in md.disasm(code, lo):
        # disp
        if i.disp_size == 4:
            d = struct.unpack_from('<I', img, i.address - BASE + i.disp_offset)[0]
            if LO <= d < HI:
                patches.append((i.address + i.disp_offset, 'disp', i.mnemonic + ' ' + i.op_str))
        if i.imm_size == 4:
            d = struct.unpack_from('<I', img, i.address - BASE + i.imm_offset)[0]
            if LO <= d < HI and i.mnemonic not in ('call', 'jmp') and not i.mnemonic.startswith('j'):
                patches.append((i.address + i.imm_offset, 'imm', i.mnemonic + ' ' + i.op_str))
with open('reloc.inc', 'w') as f:
    f.write('static const unsigned kRelocs[] = {%s};\n' % ','.join('0x%X' % a for a, k, t in patches))
for a, k, t in patches:
    if k == 'imm': print(hex(a), k, t)
print(len(patches), 'parches;', sum(1 for p in patches if p[1]=='imm'), 'imm')
