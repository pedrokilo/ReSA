<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 27/10/2026 (Ciudad de México):** 820 grupos / 8931 funciones reconstruidas; 8931 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CppTrigClimb: aislamiento restaurable de cabezas del mundo

Parche separado SIN APLICAR. No se compiló ni ejecutó. Mantiene el RNG, límites/fixtures, llamadas y trace completos; no modifica producción ni Caller15COFF.

## Fallo observado

Run 20261011-014620-274994: ProcessPed case285/pair1309 difiere sólo en trace. Original añade GetBoundCentre con entidades 67888988,67879100,67878688,67878276; candidato las omite. Resultado, cambios de buffers propios, referencias, scan_code1527 y resto del trace coinciden.

CTasks15 y su derivación CppTrigClimb guardan el array repeat16x16 completo, pero inyectan fixtures sólo en nueve celdas59..61; las otras247 conservan cabezas reales. Los sectores normales se guardan/reemplazan sólo en esas nueve celdas. RestoreGlobals restaura el contador global, no el scan code de entidades reales alcanzadas fuera de los buffers propios.

EXE demuestra el mecanismo en ScanToGrabSectorList: 0x67DEF1 MOV EAX,[EDX] carga head; 0x67DF09 MOV ESI,[EAX] toma entity; 0x67DF0B lee scan globalB7CD78; 0x67DF12 CMP WORD PTR[ESI+2C],CX; 0x67DF22 JE0x67E584 evita entidad ya escaneada; 0x67DF2C MOV WORD PTR[ESI+2C],CX muta el scan code; 0x67DFBB CALL0x534250 obtiene BoundCentre. No hay restricción a las seis entidades propias en el EXE.

ScanToGrab recorre sectores calculados: 0x67FF40 compara x contra119 y 0x67FF45 recorta; 0x67FF58 IMUL EAX,EAX,120; 0x67FF5D LEA ECX,[EAX*8+B7D0B8] selecciona sector regular; 0x67FF1F/0x67FF6A AND15 seleccionan repeat; 0x67FF96 LEA EDI,[EDI*4+B992B8]; llamadas ScanToGrabSectorList en0x67FF9D(buildings),0x67FFCD(objects,+8),0x67FFFD(vehicles). Esta ruta admite listas reales no sustituidas por el fixture.

La secuencia observada queda explicada por contaminación: original escribe1527 en externos; candidato restaura sólo global1526 y vuelve a incrementar1527, encuentra los externos ya marcados y los omite. Los registros no capturan el valor anterior de cada entidad externa ni su celda exacta; no se inventa esa evidencia. La rerun aislada debe confirmar la clasificación y conservar trace exacto.

## Parche

Añade el array completo regular120x120 a regiones guardadas/restauradas (115200 bytes), conserva el array repeat16x16 (3072 bytes), vacía únicamente sus heads al empezar cada lado y después inyecta las mismas nueve celdas de fixtures. No recorre ni libera nodos reales; no lee/escribe entidades reales. Reutiliza RestoreGlobals y GlobalsIntact para restauración exacta. **No amplía Buffers/Snapshot/Changes:** la comparación de cambios observados sigue en los buffers originales.

CSector=8 bytes(twoheads), CRepeatSector=12 bytes(threeheads): static_assert del formato x86. Los arrays se restauran antes de Write/PumpMessages como ya hacía el banco. El aislamiento sólo se permite durante la ejecución síncrona del fixture, sin avanzar frames del juego con listas vaciadas.

Begin publica alcance14400/256/9. El analizador exige ese alcance y rechaza GetBoundCentre que no señale ent0..ent5; no borra ni normaliza diferencias. También conserva todos sus gates anteriores.

## Comprobación requerida

Primero inspeccionar case285/pair1309 sin externos; después pasar las4608 parejas de las seis funciones, cero diferencias, resultados no nulos y state_restored=true. Cualquier externo residual sigue siendo fallo. El código fuente queda sin cambiar hasta demostrar si queda una diferencia real tras eliminar contaminación.

Disasm leído del EXE canónico mediante dumpbin /disasm /range con prefijo0x. Root también confirmó dis.sh desde67DE10 hasta67DFC0. No se tocó E:/JUEGOS/GTA San Andreas; sólo se leyó el JSON de resultados del clon y el EXE canónico D:/Grand Theft Auto San Andreas/gta-sa.exe.
