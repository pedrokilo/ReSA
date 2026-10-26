<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Sonda preparada: WanderStandard::GetWanderType

Estado: preparada, sin compilar y sin ejecutar. Identidad del catÃ¡logo: CTaskComplexWanderStandard, GetWanderType, 0x0048E5D0, V, validated=0. SÃ³lo esa funciÃ³n.

EXE canÃ³nico SHA256: 72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b. Bytes originales 33 C0 C3: xor eax,eax; ret. Cuerpo completo 0x48E5D0..0x48E5D2, sin lecturas ni escrituras de this. El enum eWanderType tiene base int32 y STANDARD=0. Retorno EAX, this ECX y cero argumentos de pila. Vtable GTA 0x85A200 tiene 15 entradas; slot 11=0x48E5D0, comprobado leyendo PE del EXE, no inferido del nombre.

get-wander-type.combo.txt preserva combo completo; el cachÃ© de pseudocÃ³digo responde invalid_request, y su rango por defecto continÃºa sobre funciones vecinas. No presentar esas funciones vecinas como cuerpo del getter. El ensamblado acotado revela cuerpo exacto de 3 bytes. Falta adjuntar respuesta REA nueva cuando termine campaÃ±a del coordinador.

## Fixtures

Dos objetos CTaskComplexWanderStandard privados y vÃ¡lidos de 0x38 bytes. El constructor es preparaciÃ³n de fixture y no funciÃ³n validada. Sin ped, path nodes reales, pools o entidades del mundo. Se varÃ­an direcciÃ³n/flags 256 combinaciones, cuatro move states, tiempo mÃ­nimo e intervalo/start de timer. Cada llamada exige objeto byte por byte inalterado.

Cinco rutas por 256 seeds = 1280 pares: entrada GTA directa, entrada candidata directa (redirige al original en modo desactivado), slot GTA 11, slot DLL 11 y virtual dispatch por puntero a base con wrapper noinline. AsÃ­ las dos tablas participan en ambos estados. Resultados original/candidato deben ser entero 0.

Antes de la primera mutaciÃ³n del hook se guardan flags de TODOS los hooks y 52 bytes en entradas GTA/candidato de TODOS los hooks simples y virtuales; tambiÃ©n las 15 entradas completas de las DOS vtables participantes. SÃ³lo cambia el hook target. No se cambia ningÃºn otro flag ni vtable ajena. En cada estado se verifican bytes de entrada, E9 con desplazamiento EXACTO al destino, bytes del candidato habilitado y ambos slots. Al concluir se restaura target Hooked/Locked y se comprueban todos los flags, todas las entradas capturadas y ambas tablas completas. Se registran los rangos fallidos.

Se restaura vptr del objeto fixture a la tabla DLL antes de su destructor. No se instalan EntryPatch ni stubs y no se modifican globales de gameplay. El rango de 52 bytes es snapshot de restauraciÃ³n, no longitud de funciÃ³n. Otros slots virtuales no se modifican; no se afirma snapshot de todas las vtables del proyecto.

## IntegraciÃ³n

Incluir LiveComparison.hpp desde app_game.cpp y llamar RequestedWanderTypeLive::Update en FrontendIdle temporalmente, igual que LiveBatch. Activador requested-wander-type.start; salida requested-wander-type.jsonl en copia de pruebas. AÃ±adir failure capture prefix distinto. Congelar source/instrumentos/helpers y EXE antes de construir Debug con _CL_=/MP6 y --parallel 1. Restaurar los archivos temporales despuÃ©s de construir. El coordinador integra runner/build porque controla las Ãºnicas construcciones y ejecuciones permitidas.

Antes de registrar: comprobar expected 1280 pares, ninguna discrepancia, un complete state_restored=true y hashes reales. RevisiÃ³n REA, fuentes y dependencias del mecanismo de hooks siguen pendientes. Un cuerpo constante tiene cobertura completa de la Ãºnica ruta, pero estas 1280 llamadas no validan constructor, destructor ni ScanForStuff de la clase.

## Preserved runtime status, 2026-10-08

The earlier menu attempts did not establish completion; the last such attempt exited before the bank completed. [Preserved menu summary](../../RequestedMenuBatch/runs/wander/20261008-114540-942592/enriched-summary.json). A subsequent preflight **passed all 1280 pairs**, with natural exit and verified harness restoration. [Successful enriched summary](../../RequestedPreflightWander/runs/20261008-115205-491044/enriched-summary.json). It is pending registry linkage in the five-approval documentation snapshot; failed menu attempts remain preserved and do not invalidate the subsequent replay.
