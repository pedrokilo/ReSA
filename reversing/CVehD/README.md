<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CVehicle, grupo D: dibujo, luces y partículas (26 funciones)

Banco de validación por pares (EXE gta_sa 1.0 US frente a DLL) de: `PreRender`, `PreRenderDriverAndPassengers`, `Render`, `RenderDriverAndPassengers`, `ResetAfterRender`, `SetupRender`, `SetupLighting`, `RemoveLighting`, `SetCollisionLighting`, `CalculateLightingFromCollision`, `UpdateLightingFromStoredPolys`, `DoHeadLightBeam`, `DoHeadLightReflection`, `DoHeadLightReflectionSingle`, `DoHeadLightReflectionTwin`, `AddDamagedVehicleParticles`, `AddExhaustParticles`, `AddWaterSplashParticles`, `AddWheelDirtAndWater`, `DoBoatSplashes`, `GetVehicleLightsStatus`, `DoPlaneGunFireFX`, `ProcessDrivingAnims`, `ProcessOpenDoor`, `ProcessWheel` y `ProcessWheelRotation`.

Estado: **escrito y revisado contra el desensamblado; pendiente de compilar y ejecutar** (lo hace el responsable de la integración). Prefijo de caso `cvehd`, espacio de nombres `CVehDLive`. Repetir: `python reversing/CVehD/build_probe.py` y `python reversing/run.py --case cvehd`. Filtro opcional: `cvehd-only.txt` con `CVehicle::<Función>`.

## Diferencias reales corregidas en `Vehicle.cpp`

- `ProcessOpenDoor`: los grupos de animación estaban cruzados. `ANIM_ID_CAR_CLOSEDOOR_*` (367-370, tiempos modo 1) solo cierra si `GetDooorAngleOpenRatio` > nueva razón (0x6D57E2) y `ANIM_ID_CAR_CLOSE_*` (380-383, modo 3) llama a `OpenDoor` sin comparar (0x6D5940); la DLL lo hacía al revés. Un componente de puerta fuera de 8..11 usa los bits del `float fTime` como número de puerta (sin `assert`).
- `ProcessWheelRotation`: el producto escalar se suma en orden z, y, x (la DLL usaba x, y, z).
- `ProcessWheel`: productos escalares de contacto sumados como `(y+x)+z` (adelante) y `(y+z)+x` (lateral); el freno de ruedas libres compara la MASA con 500 (`mass < 500 → *0,1`, si no `RCBANDIT → *0,2`), no el freno (la DLL lo tenía invertido); el factor de adherencia es `(tractionLoss / l) * adhesion`; módulos de velocidad sumados `(z+y)+x` y normalización con `1/módulo` (si no es > 0 solo `x = 1`); sin `std::clamp` (el EXE no exige adhesión positiva).
- `AddWheelDirtAndWater`: todas las consultas de superficie usan `m_nSurfaceTypeB` (byte 0x23; la DLL usaba `m_nPieceTypeB` en `IsSand` y `CreatesWheelSpray`); el azar solo se tira si la superficie es polvo o arena y en ese caso siempre devuelve `false`; `CreatesWheelSpray` se consulta antes que el clima y `CamNoRain` solo se llama si la superficie lo requiere.
- `GetVehicleLightsStatus`: umbral de niebla/lluvia con `semilla * 1,9999999e-5f` (0x871E78) en vez de `/ 50000`; `CamNoRain` se consulta aunque ya haya luces; la regla de las bandas se aplica al final (mismo resultado, mismas llamadas que el EXE).
- `CalculateLightingFromCollision`: iluminación en línea `(día·k)·(1−b) + (noche·k)·b` con `k = 0,03333f` (0x858F10), suma acumulada y `* 0,25f`.
- `AddDamagedVehicleParticles`: `SetConstTime(1, 1 − (salud−250) * 0,0025f)` con la constante 0x86C340 (la DLL dividía entre 400).
- `DoBoatSplashes` (reescrita): límite 0x858CD8 = 0,0025000002f, alfa entera (byte con `_ftol2`, tope 0x40, atenuación con `*(1/30)`), `alfa * (1/255)` (0x859A3C), tamaño `(v12+0,75)*0,1`, vida `rnd*((v48+v48)+0,3)*0,2` (el fld [esp+14h] ocurre antes de liberar los argumentos y lee v48), velocidad base `-forward` completa (la DLL usaba `-right.y/-right.z`) y `CMatrix * CVector` de 0x59C890.
- `AddWaterSplashParticles`: punto uniforme del triángulo `v0 + e1*r1 + e2*(r1*r2)` con dos `rand` directos (la DLL usaba dos números independientes), número de partículas `1 - (int)(lluvia * -2)` con entero con signo y `CMatrix * CVector` de 0x59C890.
- `AddExhaustParticles`: producto escalar con la velocidad sumado z, y, x; la componente z de la velocidad de las partículas es 0; el factor aleatorio es la estática del EXE (0xC1CDB4, guarda 0xC1CDB8 bit 0) y no una local de la DLL; `CMatrix * CVector` de 0x59C890.
- `DoHeadLightBeam`: posición con `CMatrix * CVector` (0x59C890), producto escalar a la cámara sumado z, y, x y los vértices/índices se escriben en los buffers temporales globales (0xC4D958, 0xC4B958) con los contadores 0xC4B950/54, como el EXE.
- `DoHeadLightReflection`: fuera del Combine siempre dibuja el reflejo derecho (no comprueba `right`). `DoHeadLightReflectionSingle/Twin`: normalización 2D en línea (con longitud cero se multiplica por 0 en vez de poner `x = 1`), `IsBike()` era el subtipo (0x594), posición `(der.x*off.x) + ((fwd.x*dist) + pos.x)` y `(lightSize+lightSize+1)+off.y`.
- `AddDamagedVehicleParticles`/`DoPlaneGunFireFX`: comprobación de clump nulo para la matriz del sistema (el EXE pasa matriz nula).

Sin cambios necesarios (MATCH): `PreRender`, `Render`, `PreRenderDriverAndPassengers`, `RenderDriverAndPassengers`, `ResetAfterRender`, `SetupRender`, `SetupLighting`, `RemoveLighting`, `SetCollisionLighting`, `UpdateLightingFromStoredPolys`, `ProcessDrivingAnims` (ya transcrita con x87 a 24 bits). Los huecos de la vtable del EXE 0x871E80 son 17 PreRender, 18 Render, 19 SetupLighting, 20 RemoveLighting, 28 ProcessOpenDoor y 46 ProcessDrivingAnims (66 huecos).

Desviaciones que se dejan y el banco evita: tiempos de animación de puerta no monótonos (el EXE evalúa los rangos en otro orden), `NOTSA_UNREACHABLE` de `ProcessOpenDoor` con grupos de animación sin tiempos (ROLLOUT/ROLLDOOR; el EXE usa memoria sin inicializar), `GetStatValue` y `GetVehicleAppearance` (se ejecuta el real).

## Método

- El vehículo es un búfer de 0xA40 bytes con patrón 0xCD y vptr a una vtable falsa de 66 thunks con traza (los huecos 27 `OpenDoor`, 30 `GetDooorAngleOpenRatio` y 38 `IsDoorMissing` con su firma real). Peatones, clump, handling, model info y su estructura de dummies, col model con triángulos y vértices comprimidos, sistemas Fx, asociaciones de animación, texturas, pool de TXD falso (el EXE inlinea `CTxdStore::GetTxd`) y buffers de armas/colisión son búferes propios. Los model infos falsos se colocan temporalmente en `ms_modelInfoPtrs`.
- Se simulan y trazan (dirección del EXE + símbolo y cuerpo de la DLL): rand, estados de render del motor (`RwRenderStateSet/Get` por los punteros de `RwEngineInstance`), Im3D, CVehicleModelInfo (dirt, materiales, color, luces, env map), matrículas, `CEntity::Render/ProcessLightsForEntity/IsVisible`, `CPhysical::GetLightingFromCol`, luces direccionales/ambiente, `CRenderer::SetupLightingForEntity`, streaming y contadores de TXD, Fx (gestor, sistemas, partículas, ruedas), `CShadows::StoreCarLightShadow`, `CPointLights::AddLight`, agua, `CamNoRain`, primera persona de la cámara, asociaciones y mezcla de animaciones, estadísticas, `CPed::IsPlayer/PreRenderAfterTest`, casquillos, `CBike::CalculateLeanMatrix`, `ApplyMoveForce/ApplyTurnForce`, `GetPlaneNumGuns` y `operator new[]` (solo durante `DoPlaneGunFireFX`, con el parche en 0x821195 y sin tocar el `operator new` que usa el json de las trazas).
- Globales instantaneadas y restauradas en cada ejecución: paso de tiempo, contador de fotogramas, cámara, reloj, clima/lluvia, zona de bandas, balance día/noche, `s_bPlaneGunsEjectShellCasings`, estática y guarda de `AddExhaustParticles`, estáticas de `ProcessWheel`, fricción de ruedas, `g_fx`, buffers temporales de Im3D, texturas de sombra, puntero del pool de TXD, textura de remapeo, estados de carga del streaming (TXD 0..7), cámara activa y su modo/dirección, punteros del motor y `ms_modelInfoPtrs`.
- Comparación: instantánea (hash con nombres estables de punteros) del vehículo, peatones, Fx y asociaciones + traza ordenada + retorno + salidas propias (buffers temporales de Im3D, estáticas...); ante una diferencia se repite con detalle completo.

## Tabla

| Función | Dirección / slot | Pares |
|---|---|---:|
| PreRender | vtable 0x871E80 slot 17 (0x6D6480) | 256 |
| PreRenderDriverAndPassengers | 0x6D3DB0 | 128 |
| Render | slot 18 (0x6D0E60) | 128 |
| RenderDriverAndPassengers | 0x6D3D60 | 128 |
| ResetAfterRender | 0x6D0E20 | 64 |
| SetupRender | 0x6D64F0 | 384 |
| SetupLighting | slot 19 (0x553F20) | 64 |
| RemoveLighting | slot 20 (0x5533D0) | 64 |
| SetCollisionLighting | 0x6D0CA0 | 64 |
| CalculateLightingFromCollision | 0x6D0CF0 | 256 |
| UpdateLightingFromStoredPolys | 0x6D0CC0 | 64 |
| DoHeadLightBeam | 0x6E0E20 | 512 |
| DoHeadLightReflection | 0x6E1720 | 384 |
| DoHeadLightReflectionSingle | 0x6E1440 | 256 |
| DoHeadLightReflectionTwin | 0x6E1600 | 256 |
| AddDamagedVehicleParticles | 0x6D2A80 | 384 |
| AddExhaustParticles | 0x6DE240 | 768 |
| AddWaterSplashParticles | 0x6DDF60 | 384 |
| AddWheelDirtAndWater | 0x6D2D50 | 768 |
| DoBoatSplashes | 0x6DD130 | 768 |
| GetVehicleLightsStatus | 0x6D55C0 | 768 |
| DoPlaneGunFireFX | 0x6D4AD0 | 256 |
| ProcessDrivingAnims | slot 46 (0x6DF4A0) | 768 |
| ProcessOpenDoor | slot 28 (0x6D56C0) | 1024 |
| ProcessWheel | 0x6D6C00 | 1024 |
| ProcessWheelRotation | 0x6D1230 | 256 |

Total: 10 176 pares.
