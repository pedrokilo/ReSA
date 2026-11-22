# Extrae de source/game_sa/RenderWare/rw/rpworld.cpp el generador (BuildVertexShaderSource y tablas) a core.inc para el arnes.
import pathlib, sys
raiz = pathlib.Path(__file__).resolve().parents[2]
s = (raiz / "source/game_sa/RenderWare/rw/rpworld.cpp").read_text(encoding="utf-8")
a = s.index("// Generador del texto de los vertex shaders por defecto")
b = s.index("// 0x75F0B0 (lote VS1")
(pathlib.Path(__file__).parent / "core.inc").write_text(s[a:b], encoding="utf-8")
print("core.inc:", b - a, "bytes")
