import sys
sys.path.insert(0, r"C:\Users\pedro\AppData\Local\Temp\claude\d263413e7afce380\5b931a4e-3c0c-42eb-b692-eb62def99cf4\scratchpad")
import crt3dis as d, struct
lo,hi=0x8884F0,0x88BE18
b=d.rd(lo,hi-lo); assert len(b)==hi-lo
q=struct.unpack("<%dQ"%(len(b)//8),b)
out=["// Tablas de pow del CRT del EXE (0x8884F0..0x88BE18, 14.632 bytes, 1.829 qwords) copiadas por valor desde gta_sa.exe (SHA256 72ae59e4...cecc9b).",
"// Generado por reversing/FnCRT3A/gen_pow_tables.py; no editar a mano. Incluido solo por CrtPow.cpp."]
for i in range(0,len(q),4):
    out.append("    "+", ".join("0x%016XULL"%v for v in q[i:i+4])+",")
open(r"D:\Original\wv-24\source\game_sa\Crt\CrtPowTables.inc","w").write("\n".join(out)+"\n")
print(len(q))
