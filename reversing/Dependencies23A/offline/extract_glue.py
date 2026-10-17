"""Extrae de Vehicle.cpp (ya parcheado) el texto de VehicleHeadLightEffect, CVehicle::DoHeadLightEffect y CVehicle::DoVehicleLights para compilarlo con los tipos falsos de mock.h.
Uso: python extract_glue.py <ruta a Vehicle.cpp> [salida glue.txt]"""
import sys
from pathlib import Path
src = Path(sys.argv[1]).read_bytes().decode('utf-8').replace('\r\n', '\n')
out = Path(sys.argv[2]) if len(sys.argv) > 2 else Path(__file__).parent / 'glue.txt'
def grab(marker, end_marker):
    a = src.index(marker)
    b = src.index(end_marker, a)
    return src[a:b]
head = grab('// Cuerpo de DoHeadLightEffect (0x6E0A50)', '// 0x6E0E20\nvoid CVehicle::DoHeadLightBeam')
dvl = grab('// 0x6E1A60\nvoid CVehicle::DoVehicleLights(', '// unused\n// 0x6E2900')
out.write_text(head + dvl, encoding='utf-8', newline='\n')
print('glue escrito en', out)
