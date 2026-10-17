<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 16/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Audio: ensamblador x87 en cinco archivos (11/10/2026)

Conversión del ensamblador de `AECollisionAudioEntity.cpp`, `AEWeaponAudioEntity.cpp`, `AEWMADecoder.cpp`, `AERadioTrackManager.cpp` y `AESoundManager.cpp`. Política: [PROCESO.md](../CppOnlyMigration/PROCESO.md).

## Qué se hizo

Las cinco apariciones eran fragmentos x87 pequeños. Cada clase conserva sus opciones de compilación; los fragmentos pasaron a cuatro helpers de `X87Math.cpp/.h`, que el compilador emite con la misma secuencia x87 que el ensamblador:

| Fragmento original | Clase / función | Helper |
| --- | --- | --- |
| `fldlg2; fld x; fyl2x; fmul 20; fstp` | `Log10Times20` en `CAECollisionAudioEntity` **y** en `CAEWeaponAudioEntity` (la misma función duplicada en dos archivos) | `AudioLog10Times20Cpp(x)`, una sola |
| `fld e; fdiv m; fldlg2; fxch; fyl2x; fmul 20; fsub 9; fstp` | `CalculateRadioVolume355`, usada por `CAERadioTrackManager::UpdateRadioVolumes` | `AudioRadioVolumeDbCpp(efectos, música)` |
| `fild low63; fild top; fchs; faddp; fmul 0.0001; fstp` | `CAEWMADecoder::GetStreamPlayTimeMs` (0x502AF0) | `AudioSumToMsCpp(suma64)` |
| `fild producto; fmul 0.01; fstp` | `CAESoundManager::Service` (0x4F0000) | `AudioPercentOfProductCpp(producto)` |

Los cuatro helpers devuelven un `float` guardado y recargado (`volatile float`), como las funciones originales (`fstp` + `fld`). Las constantes del EXE (0x858BA4 = 20, 0x858FC4 = 0.0001, 0x85EED4 = 9 y 0x858C58 = 0.01) son exactamente esos `float`.

## Comprobaciones en los objetos reales

- Los cuatro helpers de `X87Math.obj` usan `fyl2x`, `fdiv`, `fild`, `fmul` y `fsub` en línea, sin llamadas a la CRT; sólo tienen relocaciones a sus constantes.
- `fldlg2; fxch; fyl2x` calcula lo mismo que `fldlg2; fld x; fyl2x` (producto del logaritmo base 2 por log10(2) con precisión interna completa).
- `AudioSumToMsCpp` carga los 63 bits bajos y el bit alto como dos `fild` y los resta (equivale a `fchs + faddp`).

## Un problema que se resolvió

Primera corrida de RadioVolumes355: **138/1024** pares distintos. El helper era correcto; el fallo estaba en la llamada `AudioRadioVolumeDbCpp(GetEffectsMasterScalingFactor(), GetMusicMasterScalingFactor())`: MSVC x86 evalúa los argumentos de derecha a izquierda y llamaba primero a `GetMusic…`, mientras que el original lee primero los efectos y después la música, y el banco compara la secuencia de llamadas. Ahora se leen en dos sentencias.

## Revalidación

Los bancos históricos completos se repitieron sobre la fuente final con `herramientas/rerun_hist.py`. Evidencias en `reversing/CppAudioX87Unified/runs/`; la línea base está en `reversing/CppAudioBaseline/runs/`.

| Banco | Función(es) afectadas | Pares | Resultado |
| --- | --- | ---: | --- |
| Audio17 (20261011-223917-241315) | tres consumidores de `Log10Times20` | 11088 | aprobado |
| Audio20 (20261011-224039-233665) | `PlayGunSounds` | 10176 | aprobado |
| Audio14 (20261011-224528-071759) | `CAESoundManager::Service` | 10240 | aprobado |
| RadioVolumes355 (20261011-224738-339249) | `UpdateRadioVolumes` | 1024 | aprobado (la corrida 224419 falló 138/1024) |
| Audio11 (20261011-224224-976573) | `GetStreamPlayTimeMs` | 512 propios (43 funciones en el banco) | `GetStreamPlayTimeMs` y otras 41 funciones iguales; `AddSayEvent` 4/1024 (corregido más abajo, ver «Fallo preexistente»: Audio11 aprueba con 13632 pares en 20261011-231624-593138) |

## Fallo preexistente en Audio11: causa y corrección

El banco Audio11 fallaba en `CAEPedlessSpeechAudioEntity::AddSayEvent` (4 de 1024 pares, casos 10, 116, 563 y 797) también con el árbol anterior. En esos casos el original devolvía −1 justo después de la comprobación de `streaming_ctx` y el candidato seguía hasta reproducir la frase.

**Causa:** el desensamblado del EXE (0x4E61D5) muestra `cmp [8E4CB8h], 0Fh` seguido de `mov eax, [0B6B98Ch]`: con más de 15 modelos pedidos devuelve −1 si la variable en **0xB6B98C** (`s_pPlayerAttachedForRadio`) no es nula. La fuente leía `CAEVehicleAudioEntity::s_pVehicleAudioSettingsForRadio`, que está en **0xB6B99C** (la que sólo lee el audio de vehículos). Un barrido de bytes del EXE lo confirma: las lecturas de 0xB6B98C en el código de voz son 0x4E5DD2, 0x4E61D5, 0x4E66F0 y 0x4E769B, y las de 0xB6B99C son todas del audio de vehículos. Es muy probable que la variable se moviera a 0xB6B99C para el código de vehículos y dejara mal los sitios de voz.

**Corrección:** seis sitios pasaron a `s_pPlayerAttachedForRadio`: `CAEPedlessSpeechAudioEntity::AddSayEvent` (0x4E61D5), `CAEPedSpeechAudioEntity::AddSayEvent` (0x4E66F0), `PlayLoadedSound` (0x4E5DD2), `I_AddSayEvent`, `I_PlayLoadedSound` y el escáner de policía (0x4E769B). Los dos `I_*` no tienen dirección propia en el EXE y siguen a sus equivalentes.

**Resultado:** Audio11 aprueba con 13632 pares, y PedSpeech58 (21760), AudioScannerPending (4096) y AudioWave2 (18496) también.

**Segunda diferencia con el EXE, ya incluida:** `AddSayEvent` calculaba la distancia al cuadrado con `SquaredMagnitude()` en el orden x² + y² + z²; el EXE usa `CVector::SquaredMagnitude` (0x406DA0), que suma (z² + y²) + x² en x87 sin redondear. Se añadió `VectorSquaredMagnitudeAboveCpp` en `X87Math`. No era la causa del fallo (los mismos cuatro casos fallaban antes y después), pero sí una diferencia real con el EXE en casos límite a 60 m.

## `AudioLog10`

`CAEAudioUtility::AudioLog10` (0x4D9E50) tenía el mismo `fldlg2 / fld / fyl2x / fstp`. Pasó a `AudioLog10Cpp`: el objeto es `fld [p]; fldlg2; fxch; fyl2x; fstp; fld`. CppModelInfoRegression (5264 pares, auditado), Small2 (32016) y AudioHw54 (9280) aprueban.

## Límites

Estos bancos no tienen auditor propio: la comprobación es la del analizador de cada banco y el resumen de la corrida. No hay cobertura exhaustiva de x87 más allá de los fixtures de cada banco. Los colaboradores no se validan por probar a sus consumidores.

## Objetos (no se publican)

| Objeto | SHA256 |
| --- | --- |
| AECollisionAudioEntity.actual-before.obj | bbf7cb758fa959057f9ccbeb60ea9a4f71530102e0d4510358da43591aada922 |
| AEWeaponAudioEntity.actual-before.obj | 35a04b4921e7a63c34865ac1d36c6d920156f8006dca43c525fb42a3ec410b9e |
| AEWMADecoder.actual-before.obj | 365064196fddc46107136266153bec349dcf278bc1ca3aa67495b9f20c97afb0 |
| AERadioTrackManager.actual-before.obj | 3838483b50ae90ba634fe824e6b1b3bfe5739e76d0a17b1b69fc23b62707469d |
| AESoundManager.actual-before.obj | 81ac295706439aa366fbb1e3a0b719e6dff34225e9611efb6cc1e84cc9bb346d |
| X87Math.actual-before.obj | 3b785dbfa49cc7b2dce7c900f82bd14f342d596b8ab850f47a8c544809fb16ee |
| X87Math.actual-after.obj | b552ccf09e63c2829092f97fe73a308870396053877c7cac14239d91ca137798 |
