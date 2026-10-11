# Tres helpers simples: propuesta C++ separada

Aplicada y aprobada: FollowPointRoute pasó 512 pares en 20261011-013909-455441; Climb pasó 4608 en 20261011-020038-166905. Ambas auditorías independientes verificaron cero diferencias, registros no nulos, rutas reales y estado restaurado. El parche elimina FollowPointRouteCos, ClimbSinMul y ClimbCosMul mediante TrigSimpleCpp compartido. Retira tres naked y tres asm, sin añadir entradas catalogadas del EXE.

Quince cuerpos consumidores COFF mantienen instrucciones y relocations salvo destinos autorizados; evidencia final en caller-proof-admitted.json. Se preservaron las líneas del diagnóstico NOTSA_UNREACHABLE para mantener también su inmediato exacto. Hay 51 casos focales en rutas y ocho en escalada acreditados por flujo y trazas, sin afirmar contadores hardware ni cobertura universal.

El primer banco Climb falló una traza porque alcanzaba cuatro entidades externas al fixture. El EXE escribe scan codes antes de GetBoundCentre y el segundo lado omitía esos objetos ya marcados. Se aislaron todas las cabezas de los sectores 120x120 y repeat 16x16, con snapshot, restauración y guardia de ámbito; la auditoría final exige cero entidades externas. El banco final confirma la clasificación como contaminación del instrumento, sin tolerancias.

ABI helper: __cdecl, parámetros float, resultado float en ST0. Código requerido FLD DWORD; FCOS/FSIN; optional FMUL DWORD; RET, sin store/redondeo intermedio ni calls CRT, sin cambios de ECX/EDX/EFLAGS. El estándar C++ no basta: coordinación debe comprobar objeto emitido y callers old/new.

Consumidor FollowPointRoute CalcBlendRatio 0x66E710: FMUL PI 66E7F8; FCOS 66E7FE; FMUL mitad 66E800; FSTP ratio 66E819. Banco CppTrigFollowPointRoute deriva CTasks13, 512pares, flags slowing alternantes/semillas históricas. Solo rama distancia<requiredDistSq alcanza FCOS; cobertura dinámica de esa rama sigue pendiente de demostrar en ejecución, no se deduce éxito de512pares.

Consumidor Climb GetCameraTargetPos 0x67A390: FSIN 67A575, FMUL horizontal 67A581, FSTP x67A587; FCOS67A589, FMUL horizontal67A58B, FSTP y67A592. Ambas primitivas se ejecutan juntas al alcanzar GetClimbOffset3D. Otros seis puntos fuente de GetClimbOffset3D están cubiertos por CppTrigClimb: ProcessPed680DC0, TestForClimb6803A0, TestForStandUp680570, TestForVault6804D0, GetCameraStickModifier67A5D0 y GetCameraTargetPos67A390; 4608pares. Los fixtures conservan animaciones múltiples/nulos y estados, así que rutas tempranas no ejercitan helpers; coordinación debe verificar que existen casos con animación y tipo válido que alcanzan ambos. No se declara branch coverage universal porconteo.

Disassemblies completos de los dos consumidores focales se adjuntan. dis.sh no devolvió salida en la shell precedente; se usó su mismo dumpbin EXE canónico directamente, sin fuente sustituyendo EXE.

Ambos bancos conservan tablas/semillas f y conmutación histórica, resultados/trazas/clean/switches, todos los snapshots completos y restauración histórica más enabled/locked. begin acredita estado registered_hook de la selección. Manifiestos incluyen fuentes históricas, TrigSimpleCpp, CMake, InjectHooks y RH; sólo clon de prueba. No FP108/flags ABI nuevos inventados, no tolerancias ni reducción de gates. README UTF-8 verificado con lectura estricta, sin caracteres de reemplazo ni mojibake.
