<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Seis identidades pendientes: revisiÃ³n y banco preparado

Corte: 2026-10-08. Ninguna acreditaciÃ³n nueva. No se ha compilado ni ejecutado este banco; no se modifican fuentes, catÃ¡logo ni registro.

## Evidencia estÃ¡tica

Binario: `D:\Grand Theft Auto San Andreas\gta-sa.exe`, SHA-256 canÃ³nico `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`.
`combo/*.txt` conserva pseudocÃ³digo del Ã­ndice Ghidra histÃ³rico y salida de combo. `*-full-dumpbin.txt` aÃ±ade ensamblado sin filtros de las seis funciones. No presentar estas capturas como una nueva consulta REA: la conexiÃ³n nueva la coordina el agente principal.

| Identidad | DirecciÃ³n | Estado de esta revisiÃ³n |
|---|---|---|
| CFormation::FindCoverPointsBehindBox | 0x00699FF0 | Disparidades estÃ¡ticas; requiere correcciÃ³n y replay |
| CPedGroupPlacer::PlaceGroup | 0x005FD810 | Dispatch y dominio enum requieren banco propio |
| CInterestingEvents::InvalidateEvent | 0x00602960 | Flujo aparentemente equivalente; falta replay |
| CInterestingEvents::InvalidateNonVisibleEvents | 0x006029C0 | Disparidad estÃ¡tica en limpieza del puntero |
| CPedDamageResponseCalculator::Constructor | 0x004AD3F0 | Banco concreto preparado: 1024 pares |
| CPedDamageResponseCalculator::ComputeWillKillPed | 0x004B3210 | MÃºltiples ramas/dependencias; falta banco propio |

## Disparidades observadas

### InvalidateNonVisibleEvents

El EXE escribe cero en `event.entity` en `0x602A3B`, despuÃ©s de llamar `CleanUpOldReference` en `0x602A36`. El candidato llama `CEntity::SafeCleanUpRef`, que sÃ³lo llama al colaborador y deja el puntero igual. La correcciÃ³n propuesta es `CEntity::ClearReference(event.entity)`. Probar entidad nula, visible, oculta, matriz y placement, y selecciÃ³n activa/inactiva. Comparar estructura completa y traza de line-of-sight/cleanup, con colaboradores aislados iguales en ambas rutas y entradas restauradas/verificadas.

### FindCoverPointsBehindBox

El EXE calcula la distancia de cada esquina al objetivo y elige la esquina mÃ¡s cercana. El candidato calcula longitudes de lados y elige el lado mÃ¡s corto. El EXE aplica `(index-2)&3`; el candidato `(i-2)%4` produce Ã­ndices negativos en i=0/1. AdemÃ¡s el corte de distancia del EXE es estricto (`distance < cutoff`), mientras el candidato admite igualdad. Revisar tambiÃ©n la esquina opuesta usada para la direcciÃ³n, que depende del Ã­ndice elegido en el EXE. Casos: transformaciones rÃ­gidas, rectÃ¡ngulos cuadrados/asimÃ©tricos, alturas 7 exacto y adyacentes, corte exacto, capacidad 23/24 y cada esquina mÃ¡s cercana. No ejecutar sin corregir el Ã­ndice negativo o preservar el fallo como evidencia.

### PlaceGroup

El EXE incluye una tabla para allocators 0..5, con destinos `0x5FC9B0`, `0x5FCE80`, `0x5FD330`; se necesita extraer la tabla para acreditar cada enum. Para valores fuera de tabla devuelve el byte bajo del argumento origin, no una aserciÃ³n. El candidato usa `NOTSA_UNREACHABLE`. Definir el dominio vÃ¡lido expresamente; no afirmar equivalencia para enum invÃ¡lido. Banco propuesto: stubs de cada ruta original y candidata, comparaciÃ³n de argumentos/retorno/traza y restauraciÃ³n de todas las entradas parcheadas.

## Banco del constructor

`LiveComparison.hpp` ofrece `RequestedParallelSmallLive::Update()`, compatible con el constructor de bancos del proyecto. `build_probe.py` usa `install=False`. La ejecuciÃ³n y cualquier instalaciÃ³n las coordina exclusivamente el agente principal.

Compara 1024 pares original/candidato por la entrada real del EXE: objeto completo de 20 bytes (incluye padding que debe permanecer), retorno EAX=this y conmutaciÃ³n efectiva. Incluye puntero nulo/tokens no desreferenciados, bool, distintas piezas/armas y bits float Â±0, Â±infinito, NaN, subnormal y mÃ¡ximo finito. El constructor sÃ³lo copia esos datos.

`state_restored` se calcula comprobando flags, candado y los 52 bytes de la entrada antes/despuÃ©s. Las Ãºnicas escrituras persistentes previstas son el selector del hook, que se restaura; los objetos son fixtures locales. No usa una constante para la restauraciÃ³n. No sustituye una revisiÃ³n de todas las dependencias ni la evidencia REA fresca.

## ComputeWillKillPed

El banco previo PedDamagePending no incluye esta identidad: no reutilizar sus pares como acreditaciÃ³n. El nuevo banco debe aislar ComputeWillForceDeath e IsPlayer/Say de igual forma en ambas rutas, guardar cheats usados y restaurarlos byte a byte, y comparar calculator/response/ped completos mÃ¡s llamadas Say. Cobertura: inmunidad jugador, mega punch, muerte forzada, caÃ­da con y sin speak, umbral vida1, daÃ±o5 y salud10, drowning y casos float de frontera. No cubrir Ãºnicamente early-return.

## Approved menu replay, 2026-10-08

The CPedDamageResponseCalculator constructor passed 1024 pairs in the root-owned frontend probe. [Frozen enriched summary](../RequestedMenuBatch/runs/constructor/20261008-111101-552593/enriched-summary.json). This is isolated constructor evidence; dependency_review is null and current accreditation is absent. Earlier preparation instructions and failed attempts remain historical.
