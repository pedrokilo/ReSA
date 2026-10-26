<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 819 grupos / 8917 funciones reconstruidas; 8917 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CAEWeaponAudioEntity: registro, correcciones y banco dentro del motor (2026-10-01)

> Estado vigente en la [continuidad](../MEJORAS_DEL_PROCESO.md#continuidad-vigente). Cifras globales: ver `docs/`, generadas por `contrib/sync_docs.py`.

## Registro de los hooks

`InjectHooksMain.cpp` tenía comentada `CAEWeaponAudioEntity::InjectHooks()` y la auditoría estricta lo marcaba como owner que nunca se registra. **No era cierto en tiempo de ejecución:** `CAEPedWeaponAudioEntity` no declara `InjectHooks`, así que la línea `CAEPedWeaponAudioEntity::InjectHooks();` resolvía al `CAEWeaponAudioEntity::InjectHooks` heredado. El `runtime-hooks.csv` del ASI anterior ya listaba las 20 entradas. Es decir, las reconstrucciones (con los defectos descritos abajo) sí se ejecutaban en el juego.

Descomentar sin más habría registrado dos veces los mismos nombres (assert `HookCategory.h:60`). Se llama explícitamente a `CAEWeaponAudioEntity::InjectHooks()` y se comenta la llamada heredada, con la explicación. El registro sigue siendo único. **Actualización (01/10/2026, posterior):** `CAEPedWeaponAudioEntity` tiene ahora su propio `InjectHooks()`, con `Initialise`, `Terminate`, `AddAudioEvent` y `Service`, y su llamada vuelve a estar activa sin duplicar nombres; la auditoría detecta ya las llamadas que resuelven a un `InjectHooks` heredado. Ver [HooksActivados](../HooksActivados/README.md). `CAEGlobalWeaponAudioEntity` tiene su propio `InjectHooks` en otra categoría y no choca.

## Diferencias encontradas con dumpbin y corregidas en C++

Desensamblado del EXE verificado (`72ae59e4…`), rangos `0x503450–0x505244`, `0x5DE990` y `0x507560` (tablas de saltos y constantes leídas del binario).

| Función | Diferencia (antes → original) |
|---|---|
| WeaponReload | Sonidos A/B intercambiados en escopeta/SPAS, recortada, Uzi/MP5/Tec9 y AK/M4 (tabla `0x503838`); atenuación 2/3 → 0.66 |
| PlayGunSounds | Doppler 0 → 1.0 en todos; atenuación del disparo seco ×2/3 → ×0.66; orden de los cuatro sonidos principales; interpolación de distancia media mal agrupada (t negativo, **volumen NaN**); límite cercano `<` → `<=`; la cola usaba el volumen ya ajustado; cola sin tope de 0 dB |
| ReportStealthKill | Segundo sonido 471 → **47** (`push 2Fh`) |
| PlayCameraSound | Ignoraba `audability`; asignaba evento 12 (el original deja -1); atenuación |
| PlayMiniGunStopSound | En aviones velocidad y atenuación intercambiadas (1.8 / 0.7937) |
| PlayMiniGunFireSounds | Sin banco cargado actualizaba igualmente `m_LastMiniGunFireTimeMs` |
| Reset (Clear) | Reasignaba todo el objeto (estados de minigun/motosierra y base `CAEAudioEntity`); el original solo pone a cero diez campos |
| UpdateParameters | `curPlayPos` negativos distintos de -1 entraban al `switch`; los límites de FLAME/EXT/CHAINSAW_ACTIVE se aplicaban aunque el valor ya estuviera fuera; comparación con signo en STEALTH_KNIFE |
| WeaponFire | `NOTSA_UNREACHABLE` para tipos sin sonido (p. ej. `WEAPON_UNARMED`) → abortaba; el original sale |
| Todas | `m_ClientVariable` 0 → -1 (valor de `CAESound::Initialise`; solo StealthKill lo cambia) y atenuaciones 2/3 → 0.66 (`0x3F28F5C3`) |

## Entradas bloqueadas (locked=1, hook deshabilitado)

- `Constructor` → `0x5DE990`: es el **constructor de `CAEPedWeaponAudioEntity`** (vtable `0x86C2AC`, escribe `+0xA0/+0xA4`).
- `Destructor` → `0x507560`: es el **constructor de `CAEWeaponAudioEntity`** (vtable `0x862E68`); la clase no tiene destructor separado en el EXE.

Ninguna de las dos direcciones tiene llamadores en el EXE (búsqueda en todo `.text`), así que redirigirlas solo podía romper su semántica. Se marcan `{ .enabled = false, .locked = true }` y `locked=1` en `hooks.csv`. Se mantiene `reversed=1`. La fase L del banco muestra la diferencia esperada en ambas: el original escribe la vtable y pone a cero los campos, y el candidato no.

## Banco dentro del motor

**3.178 pares aprobados de 3.178** en la [ejecución final](validation-summary.json) (`resultados-weaponaudio/20261001-193805-388812`, log `18585b0c…`). Hubo 3.166 pares exactos y 12 dentro de la tolerancia: todos en `PlayGunSounds`, rama de distancia media, por log10 x87 frente a CRT. La diferencia máxima fue 9,54e-7. Tolerancia: `1e-5·max(1,|x|)` en coma flotante. Ids, flags, eventos, orden y número de llamadas deben coincidir exactamente.

- **Alternancia**: lado original con los 20 hooks de la clase deshabilitados (también las llamadas internas, p. ej. `UpdateParameters`→`PlayMiniGunStopSound`); lado candidato con los 18 normales habilitados. Contadores insertados por `build_probe.py`: 0 llamadas a la reconstrucción en el lado original y ≥1 en el candidato. No se llama a `InjectHooks()`: se buscan los hooks ya instalados.
- **Fixtures independientes por lado**: entidad de audio, entidad física sintética (peatón, coche o avión, con posición relativa a la cámara real), sonido sintético y ranuras de retorno. Todo se construye con los mismos bytes.
- **Sonidos pedidos**: `RequestNewSound` (GTA y DLL) se sustituye por un registro de cada `CAESound` pedido. Se comparan banco, id, entidad, evento, variable cliente, volumen, atenuación, velocidad, variación, posiciones, distancias, doppler, retardo, flags, tiempo de reproducción y estado de escucha. Se excluyen `m_IsAudioHardwareAware` y el relleno, que el original no copia. Se compararon 3.632 sonidos pedidos. No suena nada.
- **Dependencias controladas**: carga de bancos (`IsSoundBankLoaded`, valor por caso; `LoadSoundBank`, solo se registra), `IsLoadingTuneActive`, `AreSoundsOfThisEventPlayingForThisEntity`, `AreSoundsPlayingInBankSlot`, `CancelSoundsInBankSlot`, `CancelSoundsOwnedByAudioEntity`, `ResolveProbability`, `GetRandomNumberInRange<float>`, `CTimer::m_snTimeInMilliseconds` y `CGame::currArea`. Todas tienen valor fijado por caso y se restauran.
- **Dependencias reales**: hooks de Audio activos (solo `--unhook=Global/SurfaceInfos_c`): `CAESound::Initialise`/`StopSoundAndForget`, `CEntity::RegisterReference`/`CleanUpOldReference` sobre la entidad sintética (las referencias se retiran después de cada lado y se comprueba el total libre), `GetPositionRelativeToCamera` con la cámara real, `GetDistanceAttenuation` y la tabla de volúmenes por evento.
- **No comparado (memoria de trabajo)**: `m_tempSound` (+0x08..+0x7C). El original construye ahí cada sonido; la reconstrucción usa un `CAESound` local.
- **Cobertura**: pares por función en el resumen. Incluye bancos cargados/no cargados/melodía de carga, todos los tipos 0..50 en WeaponFire y 0..47 en WeaponReload, y los cuatro eventos de disparo de PlayGunSounds. Hay distancias en los límites 5/12 y valores que activan el tope de la cola. UpdateParameters cubre eventos -1..15, estados de minigun/motosierra, plazos vencidos/no vencidos, límites de volumen/velocidad y `currArea` 0/1.

**Contraste con el C++ anterior** (mismo banco, `20261001-194045-743643`): divergen 641/649 pares de PlayGunSounds, 158/1640 de UpdateParameters, 39/116 de WeaponReload, etc. Termina abortando en `WeaponFire:301` (`NOTSA_UNREACHABLE`). Detalle en `validation-summary.json`. Se conserva también la primera ejecución con el C++ nuevo (3176/3178: el sonido 471).

## Prueba de humo e instalación

`smoke_test.py` instala `bin/Debug`, respalda lo anterior en `_baseline/before-weaponaudio-<fecha>` y arranca con **Audio activo** y carga rápida temporal. Espera al estado 9 y mantiene el juego 35 s vigilando proceso y títulos de ventana; después restaura los INI y comprueba partidas y `runtime-hooks.csv`. Las tres ejecuciones aprobaron: sin diálogos de assert, 20 hooks registrados una vez y Constructor/Destructor locked.

ASI final instalado (compilado del árbol actual tras `sync_docs`, sin cadenas de sonda): `a0c5e7fb049292646cccad293cc175302e54e78270605d48f575c14d330914bd`; PDB `3ded71739c3bbbd78d0005fdf6c173cdd62515c13e1f9641f65dadfac3a652e1`. Respaldo previo: `a77c61c4…` / `cd41d097…`.

## Reproducción

```powershell
python reversing/CAEWeaponAudioEntity/build_probe.py      # compila la sonda y restaura fuentes
# copiar artifacts/gta_reversed-weaponaudio-probe.{asi,pdb} a <juego>/_baseline/ y actualizar PROBE_HASH
python reversing/run.py --case weaponaudio               # o PROBAR.cmd --case weaponaudio
python reversing/CAEWeaponAudioEntity/smoke_test.py --seconds 35   # tras compilar el ASI normal
```

## Límites

- No se oyó audio ni se validó la mezcla real: se comparan los sonidos **pedidos** al gestor, no su reproducción, canales ni atenuación final.
- Los bancos, la aleatoriedad y los gestores de sonido están controlados. `CAEAudioUtility::GetRandomNumberInRange<float>` del DLL no es la misma secuencia que `0x4D9C50` del original, una limitación ajena a esta clase.
- 12 pares difieren en el último bit (log10 x87 frente a CRT); no se forzó igualdad de bits.
- Constructor/Destructor siguen bloqueados: su C++ no corresponde a esas direcciones. Moverlos de dirección cambiaría filas del catálogo y queda fuera de esta tarea.
- El primer intento de contraste (C++ anterior) terminó con código 3 al iniciar el audio, antes de que la sonda escribiera nada. No se identificó la causa ni se reprodujo con el C++ corregido.
- La carga rápida no carga la ranura 2 (versión distinta) y empieza partida nueva; las pruebas de humo cubren ese arranque, no una sesión prolongada de combate.
