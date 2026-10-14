<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Plan por lotes para pasar de ensamblador a C++ (versión 2)

Revisado el 11/10/2026. La primera versión se escribió por intuición y el lote 1 tuvo que corregirse al revisar los consumidores. Esta versión sale de datos calculados sobre el árbol actual, y declara qué mide cada cifra y qué no.

Archivos de apoyo: [plan-clasificacion.csv](plan-clasificacion.csv) (una fila por función con ensamblador), [FILES.md](FILES.md) (archivos pendientes), [README.md](README.md) (conversiones comprobadas) y [approvals.json](approvals.json).

## Qué se mide y con qué límites

- **Nivel de dificultad (A–D):** heurística por mnemónicos y tamaño del cuerpo. Es una orientación, no una prueba; se confirma leyendo la función.
- **Cobertura por banco histórico:** para cada helper `static` se buscaron sus llamadores por nombre en el mismo archivo y se comprobó si cada llamador figura en la tabla de algún banco `reversing/*/LiveComparison.hpp`. Para métodos con cuerpo propio se buscó la función misma. Un banco histórico **no basta**: validó el código anterior, así que cada lote necesita un banco derivado (como CppShadowSqrt\*) y su revisión de los puntos de llamada. La cobertura sólo dice que hay un banco del que partir.
- **Sin determinar:** 131 helpers `static` en los que la búsqueda no halló llamador (puede ser uso por macro, plantilla o desde otro archivo). Se revisan a mano antes de planificarlos.

## Cifras (770 definiciones con ensamblador, 11/10/2026)

| Nivel | Con banco histórico del que partir | Sin banco | Total |
| --- | ---: | ---: | ---: |
| A (cortas, sin x87/llamadas) | 6 | 4 | 10 |
| B (x87 sin llamadas, medianas) | 189 | 35 | 224 |
| C (naked, llamadas, SSE, x87 largo) | 129 | 76 | 205 |
| D (largas, cadenas, sistema) | 174 | 109 | 283 |

Los totales suman las 722 definiciones del inventario actual (se recalcularon tras las conversiones del 12/10; el detalle por función está en [plan-clasificacion.csv](plan-clasificacion.csv)). Las dos columnas de la derecha no son «hecho» ni «pendiente de verificar»: son sólo dónde hay punto de partida.

## Lo ya hecho

123 funciones fuente. Lote 1a: ShadowExtendedSqrt y ShadowDistanceFade (README). Lote Conversaciones: IsPlayerInPositionForConversation y ClearRecursively; lote Animación y atractores: CalcTheta, RemoveQuaternionFlips, GetDisplacement y CAttractorScanner::Clear (ver [CppAnimAttractorStudy](../CppAnimAttractorStudy/README.md)); lote Conversaciones: cada clase en sus dos archivos `.cpp` y `.h` (ver [CppConversationStudy](../CppConversationStudy/README.md)). La revisión de las conversiones anteriores, que sí tienen archivos auxiliares y código duplicado, está en [AUDITORIA_ARCHIVOS_AUXILIARES.md](AUDITORIA_ARCHIVOS_AUXILIARES.md). Lotes posteriores (11 y 12/10/2026): audio x87 en cinco archivos y `AudioLog10` ([CppAudioX87Study](../CppAudioX87Study/README.md)), nueve archivos ([CppNineFilesStudy](../CppNineFilesStudy/README.md)), diez archivos ([CppTenFilesStudy](../CppTenFilesStudy/README.md)), trece archivos de tareas ([CppThirteenFilesStudy](../CppThirteenFilesStudy/README.md)) la equivalencia del `pow` con el EXE ([PowEquivalence](../PowEquivalence/README.md)) y dieciséis archivos más ([CppSixteenFilesStudy](../CppSixteenFilesStudy/README.md): doce convertidos y cuatro que siguieron naked) y las cuatro restantes ([CppFourNakedStudy](../CppFourNakedStudy/README.md)).

**Hallazgo útil:** un envoltorio `naked` que sólo preserva EDX (`*_KeepEdx`) se puede reemplazar por un hook directo si la función se compila en un archivo /O2 propio y el objeto real no escribe EDX, como en ClearRecursively. Hay 8 envoltorios `KeepEdx` en `source/game_sa` (Audio/AEVehicleAudioEntity, CarEnterExit y otros): candidatos a un lote propio, cada uno con comprobación del objeto real y banco con EDX sembrado. No se asume que el compilador no use EDX: se comprueba en el objeto de cada función.

## Regla para ordenar los lotes

Se prioriza lo que reúne las tres condiciones: **(1)** nivel A o B, **(2)** todos sus consumidores tienen banco del que partir, **(3)** pocas fuentes de banco distintas, porque cada banco derivado es lo más caro. Dentro de eso, primero las familias con precedente ya aprobado (helpers trigonométricos `SinX87`/`CosX87`, `SinMulX87`, etc. de Clouds, TrigSimple, TrigCompound6).

## Lotes

**Lote 2 — helpers x87 pequeños con precedente (23 funciones, 5 bancos de partida).**
- Weather.cpp: 5 helpers (SinMulAddX87, CosMulAddX87, HalfMinusHalfCosX87, NegSinX87, CosX87). Banco de partida Misc61a. Consumidores: AddRain, Update, UpdateInTunnelness.
- WeaponEffects.cpp: 4 helpers (SinMulX87, CosMulX87, MulDoubleAddX87, AngleSinCosX87). Banco Ped9. Consumidor: CWeaponEffects::Render.
- PostEffects.cpp: 6 helpers (ExeLog2\*, Ripple\*, Fog\*). Banco FxPost1.
- CarAI.cpp: 3 helpers (ExeForward, ExeRight, HeadOnDot). Banco CarAI25.
- GangWars.cpp: 5 funciones. Bancos GangEvent1 y GangWarsPending.
Riesgo medio: mismo método que los helpers ya convertidos (TU dedicada /O2 /Oi /arch:IA32 /fp:fast, comparar el cuerpo del objeto real y los puntos de llamada). Primero un lote por archivo, no los cinco a la vez.

**Lote 3 — Shadows.cpp restante (lote 1b).** ShadowExtendedProduct, ShadowHeightDifference, ShadowHeightSum, ShadowXYDot, ShadowXYCross, ShadowXYBlend, ShadowProjectionRatio, ShadowProjectionCoordinate, ShadowNormalOffset. Cada helper tiene consumidores en funciones que los bancos de ShadowSqrt no cubren, por lo que hacen falta bancos derivados de ShadowGenerate355, ShadowXYZ355, ShadowXY355 y ShadowSectors355. ShadowFadeInteger llama a 0x821B40 del EXE y queda fuera.

**Lote 4 — vehículos x87.** Plane.cpp (20 helpers B con banco; 5 bancos de partida), Bike.cpp (17; 7 bancos), Bmx.cpp (6; 1 banco), Train.cpp (5; 2 bancos), Automobile.cpp (4; 1 banco). Empezar por Bmx y Automobile, que sólo piden un banco.

**Lote 5 — cámara.** Camera.cpp (13 B; 7 bancos) y Cam.cpp (31 B; 27 bancos de partida distintos). Cam.cpp es el archivo con más ensamblador y exige muchos bancos: dividirlo en varias tandas por banco.

**Lote 6 — nivel C con contrato de registros.** Interior_c.cpp (24 funciones; 4 bancos), Stats.cpp (23; 8 bancos), Matrix.cpp, Garage.cpp. Antes de convertir cada una hay que fijar los registros de entrada y salida; los rechazos anteriores por cambiar ECX/EDX indican el riesgo.

**Lote 7 — sin banco de partida (nivel A–C sin cobertura: 120 funciones).** Requieren primero escribir un banco desde cero. Se hacen cuando los lotes con cobertura estén cerrados.

**Al final — nivel D y bloqueados.** MemoryMgr.cpp (falta la línea base ASM completa sin logger diagnóstico), EventSource (propuesta con `__fastcall` sin aprobar), AEAudioUtility.cpp (la heurística lo marca B, pero incluye tiempo del sistema y debe revisarse), DecisionMakerTypes.cpp, TaskSimpleFight.cpp, Pad.cpp y RpAnimBlend.

## Qué no se promete

- No hay estimación de tiempo ni de porcentaje. Los números de arriba cuentan definiciones con ensamblador, no avance real.
- Un lote se da por terminado sólo cuando cumple todo lo de abajo; la cobertura histórica no sustituye nada de eso.

## Condiciones fijas de cada lote

- Resultados exactos del EXE, sin tolerancias añadidas, y sin mover ASM a otro archivo ni delegar el cuerpo al EXE.
- Comprobación del objeto real del proyecto, incluidos cuerpos de los consumidores y puntos de llamada (qué registros y banderas se leen tras cada llamada).
- Opciones de compilación por archivo, nunca globales.
- Banco derivado con pares exactos, corrido en serie en el clon `E:\JUEGOS\GTA San Andreas - CVehicle Test`; cero diferencias, cero registros nulos, estado, partidas y configuración restaurados.
- Una revisión independiente por lote, no por función.
- Documentación (approvals.json, inventarios, textos de estado) actualizada una vez al cerrar el lote.
- `python -B contrib/audit_hook_catalog.py --strict` antes de publicar.
- Ningún lote se presenta como «100%»: catálogo, lenguaje e independencia del EXE se miden por separado.
