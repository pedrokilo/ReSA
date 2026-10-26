<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# RadarFinal3: banco por pares de las 63 funciones de CRadar

EXE GTA SA 1.0 US. Sin compilar ni ejecutar todavía. Uso: `bash reversing/_handoff/bench.sh RadarFinal3 3`.
Filtro opcional: `radarfinal3-only.txt` en la carpeta del juego con `CRadar::funcion` (p. ej. `CRadar::DrawBlips`).

Cada par es: original (hook desactivado = código del EXE) frente a candidato (hook activo), mismo fixture determinista (semilla = índice de caso),
x87 a 24 bits. Se compara el valor devuelto, la región propia de CRadar (0xBA8300..0xBAA370: blips, leyenda, sprites, estáticas), el rect del radar,
FrontEndMenuManager y la traza de colaboradores (nombre, orden, argumentos). Total 15040 pares (ver tabla `fns` en LiveComparison.hpp).

## Archivos
- `Common.hpp` trazas, zonas guardadas/restauradas, registro de colaboradores, `Run`.
- `Doubles.hpp` motor RW falso (+0x1C zBufferFar, +0x20 estados, +0x30 Im2D), pools falsos (TXD, EntryExit, vehículos, peds, objetos), dobles y fixtures.
- `SimpleCases.hpp`, `DrawCases.hpp`, `WorldCases.hpp` casos; `LiveComparison.hpp` tabla, sitios, evento `layout` y bucle principal.

## Enmascarado y ruido documentado
- `DrawAreaOnRadar`: el EXE pasa a SetVertices una matriz de UV sin inicializar (0x585662..0x5856B5); solo se traza posición/color, no UV. `DrawRadarSection` sí traza UV.
- Los argumentos de color/altura que el EXE deja sin enmascarar (b = color>>8, etc.) se enmascaran a 8 bits en los dobles.
- `FindPlayerPed/Vehicle` no se trazan (el EXE las llama más veces); `FindPlayerCentre/Heading/Speed` sí (id).
- `ScriptDebugLine3D`: el EXE recibe 8 dwords por valor; el doble del lado DLL se elige según la firma declarada.
- `GetBlipName` con sprites 0,1,2,4,56 o >63 es NOTSA_UNREACHABLE en la DLL (el EXE usa la Y como puntero): fuera de dominio.
- `CVector::Normalise` se dobla (misma fórmula en ambos lados) para no mezclar su propia validación.

## Correcciones de fuente (evidencia asm)
- Ver informe de la sesión: DrawLegend (0x582E62..), LimitToMap (0x583351 ftol+movsx), CalculateBlipAlpha (0x85F0A0), Transform* (reciproco y orden x87),
  CalculateCachedSinCos (fpatan/fsin sin redondear), SetCoordBlip/SetEntityBlip (0x5838D7/0x583A60 byte de banderas = 3), ChangeBlipBrightness (0x583CAE),
  SetCoordBlipAppearance (0x583E8C tipo COORD, bits 6-7 del primer byte), ShowRadarTrace (7 dwords, rects ±1), DrawRotatingRadarSprite (constantes del EXE),
  DrawYouAreHereSprite (0x58497B 500 ms / 0x58499E 200 ms), IsPointInsideRadar (inclusivo), Remove/StreamRadarSections (orden x,y y RemoveModel), LineRadarBoxCollision,
  DrawAreaOnRadar (rechazo 0x5853E2), DrawRadarMask (cuadrado previo a zBufferFar y esquinas), Draw3dMarkers (Nevada 0.6, orden de suma, enex), DrawRadarSection,
  DrawRadarGangOverlay (0x865034), DrawRadarMap (π double, fpatan), DrawMap (sin FindPlayerPed inicial, 16.67, coop), DrawCoordBlip (apariencia, alfa, marcador de depuración, alturas),
  SetupAirstripBlips (contador de fotogramas, empates), DrawBlips (orden isSprite, jugador 0, π, color 50/50/80, ancho=alto), Initialise (contador = 1), HasThisBlipBeenRevealed (>= 80).

## Rectángulos invertidos en DrawAreaOnRadar (fuera de dominio)
El EXE no ordena el rectángulo: 0x585431..0x585470 copia las cuatro esquinas tal cual (`mov [esp+34h],eax`...) y 0x5855B9 llama a ClipRadarPoly (0x585040) con
la salida en un arreglo local de 8 vértices; ClipRadarPoly escribe `[ebp+8]+edi*8` sin comprobar capacidad (0x5851F4 `lea edx,[edx+edi*8]`, 0x585300 `fstp [eax+ecx*8]`),
así que con el sentido invertido emite más de 8 vértices y pisa la pila (eip = un float en la corrida). El fixture solo usa rectángulos ordenados.

## Fuera del banco
- `CRadar::ClipRadarPoly` y `DrawEntityBlip` no están enganchadas (EXE puro): ClipRadarPoly corre real; DrawEntityBlip se dobla.
- `CalculateCachedSinCos` con objetivo sin matriz usa dobles de AllocateMatrix/UpdateMatrix.

## Supuestos a revisar al compilar
- `RwEngineInstance->dOpenDevice.zBufferFar`, `TheCamera.m_matrix/m_placement`, `CWorld::Players[i].m_pPed`, nombres `MODE_*` y `CPlayerInfo` existen como se usan.
- Inline asm (`__asm`) con locales y `fld qword` de `double` local; función `TransformRadarPointToScreenSpace` naked con retorno por puntero oculto.
- Los símbolos de DLL de los dobles (`CText::Get`, `CPlaceable::AllocateMatrix`, ...) no están inlinados.

## Aprobación (09/10/2026)

Corrida `20261009-203202-095801`: passed=true, 15040 pares, 63 funciones registradas con 0 fallos, evento complete (state_restored, layout_ok), saves y configuraciones intactos. Registrada con `register_radarfinal3.py` (evidencia en `runs/`, `validation-summary.json`). Limites declarados en el alcance de cada funcion y en las secciones anteriores de este README.
