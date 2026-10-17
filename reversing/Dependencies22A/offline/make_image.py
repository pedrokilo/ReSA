"""Genera exe_image.bin (imagen de gta-sa.exe en memoria) y reloc.inc para los arneses sin juego (build_a.sh / build_b.sh). Uso: python make_image.py [gta-sa.exe]"""
import struct, sys
exe = open(sys.argv[1] if len(sys.argv) > 1 else r'D:\Grand Theft Auto San Andreas\gta-sa.exe', 'rb').read()
pe = struct.unpack_from('<I', exe, 0x3c)[0]; nsec = struct.unpack_from('<H', exe, pe + 6)[0]; opt = struct.unpack_from('<H', exe, pe + 20)[0]
img = bytearray(0x8b1000); o = pe + 24 + opt
for i in range(nsec):
    vs, va, rs, rp = struct.unpack_from('<IIII', exe, o + 8 + i * 40); img[va:va + rs] = exe[rp:rp + rs]
open('exe_image.bin', 'wb').write(img)
