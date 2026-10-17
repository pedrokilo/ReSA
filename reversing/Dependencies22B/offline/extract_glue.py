"""Extrae de Vehicle.cpp (ya parcheado) el texto de CVehicle::DoHeadLightEffect y CVehicle::DoTailLightEffect para compilarlo con los tipos falsos de mock.h.
Uso: python extract_glue.py <ruta a source/game_sa/Entity/Vehicle/Vehicle.cpp> [carpeta de salida]"""
import sys
from pathlib import Path
src = Path(sys.argv[1]).read_bytes().decode('utf-8').replace('\r\n', '\n')
out = Path(sys.argv[2]) if len(sys.argv) > 2 else Path(__file__).parent
def grab(marker, end_marker):
    a = src.index(marker)
    b = src.index(end_marker, a)
    return src[a:b]
head = grab('// 0x6E0A50\nbool CVehicle::DoHeadLightEffect(', '// 0x6E0E20\nvoid CVehicle::DoHeadLightBeam')
tail = grab('// 0x6E1780\nbool CVehicle::DoTailLightEffect(', '// 0x6E1A60\nvoid CVehicle::DoVehicleLights')
(out / 'glue_head.txt').write_text(head, encoding='utf-8', newline='\n')
(out / 'glue_tail.txt').write_text(tail, encoding='utf-8', newline='\n')
print('glue_head.txt y glue_tail.txt escritos en', out)
