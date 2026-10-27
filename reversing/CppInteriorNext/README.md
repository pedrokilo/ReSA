<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

> Resultado del coordinador: APROBADO en 20261011-000453-501774, 6144 pares (Office 2048, Shelf 4096), cero diferencias y cero registros nulos, rutas registradas y restauración verificadas. [Auditoría](runs/20261011-000453-501774/approval-audit.json). Fuente y opciones finales acreditadas por sus hashes. Los apartados siguientes conservan preparación y diagnóstico de los dos intentos rechazados.

# CppInteriorNext — dos conversiones C++ aprobadas

Lote propuesto: FurnishOffice 0x599AF0..0x599BA9 y Shop_AddShelfInfo **0x59A140**..0x59A1AC. Los desensamblados completos incluyen el JMP final y ambos RET 0Ch. No se ha aplicado producción ni compilado ni ejecutado el banco.

## Propuesta C++

Se eliminan ambas implementaciones naked y sus tablas de saltos de Interior_c.cpp. Se agregan métodos C++ en InteriorCpp.cpp, que el coordinador ya compila sin PCH. El parche agrega explícitamente <bit> y FurnitureManager_c.h al TU; required-includes.hpp.txt conserva la lista para revisión.

FurnishOffice conserva las cuatro llamadas SetTilesStatus en orden, vuelve a leer m_box después de cada colaborador, usa los bytes cero-extendidos de dimensiones/wealth, guarda AL truncado en this+791 y this+792, y llama Edges, Center y Ceiling en ese orden. El último JMP EXE es una llamada final a Ceiling en C++; ambas firmas son void, sin contrato artificial de EAX/ECX/EDX/EFLAGS no consumidos.

Shop_AddShelfInfo conserva comparación firmada counter>1, única llamada al CRT rand original (0x821B1E, colaborador de biblioteca), máscara16, escala exacta 1/32768 y 100, truncamiento a entero y >60. Las constantes provienen del EXE: 858B14=00000038, 858628=0000c842. El cuerpo C++ no llama a su implementación original ni a un método del juego por dirección; AddInteriorInfo es llamada tipada al método fuente. Ambos argumentos int32 se convierten a float32 en el orden semántico x,y; se pasa direction y nullptr, se ignora el resultado y se escribe counter=1. El otro camino incrementa uint32, preservando wrap de INT_MAX sin desbordamiento C++ firmado.

## Banco y oráculo

Derivado separado del banco anterior: 2.048 Office y 4.096 Shelf. Instrumenta colaboradores del banco histórico en entrada EXE y dirección fuente, conserva traza, receptor completo y bytes de caja; restauración de hooks/parches/globals. Es prueba del cuerpo convertido, no creación de muebles ni nueva aprobación transitiva.

El oráculo comprueba todos los efectos del receptor (incluidos cuatro cambios de tiles, IDs AL y contador de infos), cada llamada y argumentos float32 exactos, contador signed/wrap, consumo RNG, casos/par únicos y payloads no nulos. Incluye dimensiones byte completas, coordenadas INT_MIN/INT_MAX y ±16777217, conversiones redondeadas, conteos negativos/0/1/2/INT_MAX y las tres ramas RNG. El dominio flotante de cada llamada se fija explícitamente en PC24/nearest, con restitución de la captura FP108/MXCSR del juego en cada lado y antes de PumpMessages. El oráculo de float32 usa ese redondeo y el log acredita CW de entrada; no se afirma cobertura de otros modos. Se mantiene stack/nonvolatile, rutas EXE OFF/DLL fuente ON con resolución y módulo real, guardas de memoria prestada y restauración física FP108+MXCSR exacta antes/después del banco.

## Estado flotante y metadatos de instrucciones

Cada comparación registra FIP/FDP/selectores/opcode crudos, módulo que contiene FIP y 16 bytes de instrucción cuando la página ejecutable es legible. Esa evidencia permite al coordinador localizar las instrucciones observadas en el EXE/ASI, sin asumir identidad por un rango amplio. Cada comparación conserva FP108 raw original/candidato y MXCSR raw sin cambiar ni borrar bytes. El payload comparable incluye CW, SW **completo** (flags y TOP), tag, todos los registros80 cuyo tag no es vacío, y MXCSR completo. El analizador recomputa esos campos desde raw y rechaza cualquier ocultación. Así ninguna diferencia numérica, flag o registro vivo se convierte en pasante mediante normalización.

En el formato FNSAVE32, offsets0..3 CW,4..7 SW y8..11 tag son estado numérico; offsets12..15 FIP y20..23 FDP identifican instrucciones/datos reubicados;16..19 y24..27 contienen selectores/opcode/metadatos asociados, conservados raw. Registros80 empiezan en28; el tag físico junto a TOP determina qué valores están vivos. Los residuos de registros con tag vacío no son resultados consumidos y se conservan en raw, sin igualdad impuesta al residuo muerto.

Prueba nativa: Shelf sin RNG no ejecuta x87; con RNG ejecuta FILD59A15B, FMUL59A15F/59A165 y CRT ftol821B40; si añade info, FILD59A179/FSTP59A183 y FILD59A189/FSTP59A18D depositan float32 en argumentos de pila. El desensamblado CRT_ftol adjunto documenta la conversión original. La compilación C++ producirá direcciones de código/datos propias que se deben revisar contra los raw observados. No se toleran diferencias de valores/flags; una discrepancia SW/MXCSR bloqueará el banco y requerirá estudiar la generación de la conversión.

El constructor configura sólo E:/JUEGOS/GTA San Andreas - CVehicle Test antes de probe_build. Manifiesto incluye ambos TU fuente, CMake, InjectHooksMain, FurnitureManager y reversiblehooks usado. Preparado para revisión offline del coordinador; **NO READY para admisión**, sin equivalencia afirmada antes de prueba en juego.

Comprobación offline adicional: se enumeraron los 65.536 valores posibles tras máscara16; la escala float32/PC24 y su truncamiento coinciden exactamente con draw*100/32768. Esto prueba el escalar matemático, no los efectos de flags del CRT ni la implementación compilada pendiente.

## Pruebas del coordinador y diagnóstico

Las corridas 20261010-235648-218480 y 20261011-000000-192378 completaron 6144 pares y restauraron el estado, pero se rechazaron: 54 diferencias en Shelf; Office tuvo cero en 2048. Las 54 diferencias fueron exclusivamente C1 (SW xor 0x200), con coordenada x=INT_MAX. En 0x59A189/0x59A18D el EXE convierte el entero y almacena una vez: el redondeo ascendente deja C1=1. La primera candidata usaba SSE; la segunda x87 pero almacenaba un temporal float32 y luego otra vez el argumento, borrando C1. No son ruido ni cambios de FIP: son diferencias reales de estado numérico.

La propuesta actual está aplicada para pruebas. Shelf vive en InteriorShelfCpp.cpp, unidad separada sin PCH/unity, con /Zi, /arch:IA32, /O2, /Ob2 y /fp:fast, y BasicRuntimeChecks=Default para evitar incompatibilidad /RTC1-/O2. El objeto generado tiene FILD y; FSTP argumento y; FILD x; FSTP argumento x directamente. La fusión de multiplicaciones usa draw*100/32768, exactamente representable a PC24 para los 65536 valores enmascarados; la comparación completa SW/MXCSR sigue siendo obligatoria. El resto del proyecto conserva sus opciones. Esto aún requiere la corrida completa; inspeccionar código generado no constituye validación.
