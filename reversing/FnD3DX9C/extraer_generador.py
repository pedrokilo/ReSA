"""Extrae de source/game_sa/RenderWare/rw/rpworld.cpp el generador del texto de los vertex shaders (BuildVertexShaderSource y sus tablas) a out/gen_fuente.inc,
para que el banco use exactamente el mismo codigo que el juego (sin copias a mano). Uso: python -I extraer_generador.py"""
import os
aqui = os.path.dirname(os.path.abspath(__file__))
src = open(os.path.join(aqui, '..', '..', 'source', 'game_sa', 'RenderWare', 'rw', 'rpworld.cpp'), encoding='utf-8').read()
a = src.index('// Generador del texto de los vertex shaders por defecto')
b = src.index('// 0x75F0B0 (lote VS1')
os.makedirs(os.path.join(aqui, 'out'), exist_ok=True)
open(os.path.join(aqui, 'out', 'gen_fuente.inc'), 'w', encoding='utf-8').write(src[a:b])
print('generador extraido: %d bytes' % (b - a))
