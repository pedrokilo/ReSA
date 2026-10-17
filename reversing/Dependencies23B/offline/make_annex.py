import re, sys
base = 'soft/'


def rd(p):
    return open(base + p, 'rb').read().decode().replace('\r\n', '\n')


# ------------------------------------------------------------------ cabecera
h_soft = rd('flc_soft.h')
h_soft = h_soft.replace('#pragma once\n', '')
h_soft = h_soft.replace('    float         lastFloat;   // (reservado)\n', '')
h_mac = rd('flc_macros.h')
h_mac = h_mac.replace('#pragma once\n', '').replace('#include "flc_soft.h"\n', '')
header = ('\n// =====================================================================================================================================================\n'
          '// g23b: CVehicle::FlyingControl (0x6D85F0). Maquina x87 de software y funciones auxiliares del EXE como secuencias x87. Anadir AL FINAL de X87Math.h.\n'
          '// =====================================================================================================================================================\n') + h_soft + '\n' + h_mac
open('../g23b-x87.h.txt', 'wb').write(header.encode())

# ------------------------------------------------------------------ fuente
c_soft = rd('flc_soft.cpp')
c_soft = c_soft.replace('#include "flc_soft.h"\n', '').replace('#include <string.h>\n', '')
# quitar el trazado de depuracion
c_soft = re.sub(r'#include <stdio.h>\nint gFlcTrace = 0;\nstatic unsigned int gFlcLastState = 0;\nstatic void FlcTraceState\(const FlcX87& m, const char\* what\) \{.*?\n\}\n', '', c_soft, flags=re.S)
c_soft = re.sub(r'\s*FlcTraceState\(\*this, "[A-Za-z]*"\);', '', c_soft)
c_soft = c_soft.replace(', lastFloat(0.0f)', '')
assert 'FlcTraceState' not in c_soft and 'printf' not in c_soft, 'traza sin quitar'

c_mac = rd('flc_macros.cpp')
c_mac = c_mac.replace('#include "flc_macros.h"\n', '').replace('#include <string.h>\n', '').replace('#include <math.h>\n', '')

c_mac = c_mac.replace('kFlcZero', 'kFlcMacZero')
c_com = rd('flc_commit.cpp')
a = c_com.index('// ---- fsin / fcos / fpatan reales')
# bloque de plantas propias (6..8) y Commit: se conservan; se descartan las copias de X87Plant*, las constantes duplicadas y las cabeceras
p0 = c_com.index('// ---- Plantas (niveles 6, 7 y 8 nuevos)')
planted = c_com[p0:]
planted = planted.replace('extern "C" double __cdecl sin(double);\nextern "C" double __cdecl cos(double);\nextern "C" double __cdecl atan2(double, double);\n#pragma intrinsic(sin, cos, atan2)\n', '')
planted = planted.replace('double __cdecl FlcHwSin(double x)', '__declspec(noinline) double __cdecl FlcHwSin(double x)').replace('double __cdecl FlcHwCos(double x)', '__declspec(noinline) double __cdecl FlcHwCos(double x)').replace('double __cdecl FlcHwAtan2(double y, double x)', '__declspec(noinline) double __cdecl FlcHwAtan2(double y, double x)')
planted = planted.replace('#define FZ X87_PLANT_ZERO\n', '#define FZ X87_PLANT_ZERO\n')
# la secuencia de macros FZ* se redefine: quitar con #undef al final
planted += '\n#undef FZ\n#undef FZ2\n#undef FZ4\n#undef FZ8\n#undef FZ16\n#undef FZ32\n#undef FZ64\n'
source = ('\n// =====================================================================================================================================================\n'
          '// g23b: CVehicle::FlyingControl (0x6D85F0): maquina x87 de software. Anadir AL FINAL de X87Math.cpp (usa X87PlantStmt1..5Cpp, kPlantOne, X87_PLANT_ZERO e ExeCompatible* ya definidos arriba).\n'
          '// =====================================================================================================================================================\n'
          + c_soft + '\n' + c_mac + '\n' + planted)
open('../g23b-x87.cpp.txt', 'wb').write(source.encode())
print('anexos escritos', len(header), len(source))
