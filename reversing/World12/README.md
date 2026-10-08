<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6736 con evidencia histórica y 1710 pendientes en 77 grupos. Perfil histórico: 694 grupos / 6136 funciones. REA admite GTA SA x86; standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores de este documento conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Semáforos, pickups y explosiones (lista de 12)

Validadas el 02/10/2026 con **11.392 pares original/candidato dentro de GTA**, todos exactos: 36 funciones de 3 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CTrafficLights` | `DisplayActualLight`, `FindOrientationForTrafficLightType`, `FindTrafficLightType`, `FindTrafficLightTypeFromOrientation`, `IsMITrafficLight`, `LightForCars1`, `LightForCars1_Visual`, `LightForCars2`, `LightForCars2_Visual`, `LightForPeds`, `ShouldCarStopForBridge`, `ShouldCarStopForLight` |
| `CPickup` | `ExtractAmmoFromPickup`, `FindStringForTextIndex`, `FindTextIndexForString`, `GetPosn`, `GetRidOfObjects`, `GiveUsAPickUpObject`, `IsVisible`, `PickUpShouldBeInvisible`, `ProcessGunShot`, `Remove`, `SetPosn`, `Update` |
| `CExplosion` | `AddExplosion`, `ClearAllExplosions`, `DoesExplosionMakeSound`, `GetExplosionActiveCounter`, `GetExplosionPosition`, `GetExplosionType`, `Initialise`, `RemoveAllExplosionsInArea`, `ResetExplosionActiveCounter`, `Shutdown`, `TestForExplosionInArea`, `Update` |

**Diferencias reales corregidas:**
- `CTrafficLights`:
  - `LightForPeds` está en verde hasta 15384 (no 15383);
  - `ShouldCarStopForBridge` exige el área del nodo actual cargada y devuelve cierto cuando el siguiente enlace tiene luces de puente y el actual no;
  - `ShouldCarStopForLight`/`ShouldCarStopForBridge` llaman a `GetVehicleAppearance` una vez por comparación; el producto escalar usa 0,125/0,01 del EXE y su orden de sumas;
  - `FindOrientationForTrafficLightType` multiplica por la constante del EXE (un ulp por debajo de 180/π); `FindTrafficLightTypeFromOrientation` da `DIR_WEST_EAST` con NaN;
  - `DisplayActualLight`: orden de sumas del producto con la cámara, `MultiplyMatrixWithVector` (0x59C890), centro por el inverso, colores de la sombra `((c·suelo)·brillo)·0,0125` y truncado `_ftol2`.
- `CPickup`:
  - `IsVisible` se engancha con un envoltorio que conserva `edx` (su llamador 0x458E42 lo reutiliza);
  - `PickUpShouldBeInvisible` reescrita como el EXE (cinemática, frenesí fuera de misión, pickup 2P con jugador 2, segundo parque alemán, la negación de `CanBeUsedFor2Player`);
  - `GiveUsAPickUpObject`: anula el objeto antes de mirar la cinemática, −π/2 exacto, `UpdateRwMatrix` en lugar de `SetHeading`, el bit 0x2 copia `bEmpty`, coste con división con signo;
  - `FindStringForTextIndex` devuelve "FESZ_CA" para cualquier otro índice; `SetPosn` trunca a 64 bits;
  - `Update`: ingresos con producto entero y 1/1440000, mínimo con el ammo con signo, coste `valor·0,2`; distancias al cuadrado (2,4 y 1,8, sin raíz) con `FindPlayerCoors` cuatro veces; el soborno dentro de un vehículo solo cuenta el contacto; búsqueda del jugador 0 solo para el soborno; regeneración lenta de 300000/360000 ms; dinero del jugador 0 y orden quitar/audio/voz; ingresos sumados en float; compra de la propiedad (resta el dinero y quita el pickup); mensaje 2P marcado; agua 5 m por encima, minas recorriendo el pool al revés, gravedad del paquete `paso·0,01`; el pickup caducado no cuenta como recogido; `StaticCamCoors` copia las coordenadas.
- `CExplosion`:
  - estructura: iteración en 0x28, contador activo en 0x29 y sonido en 0x2A (byte crudo: `AddExplosion` lo guarda sin normalizar y `DoesExplosionMakeSound` lo devuelve tal cual);
  - `ClearAllExplosions` solo limpia el combustible "diagonal" (explosiones 0–2) como el EXE;
  - `AddExplosion`: referencias sobre el miembro (no sobre el parámetro), aleatorios con la aritmética del EXE, propagación 0,5 en molotov/barco/avión, granada con víctima también crea el efecto, coche sin víctima no crea efecto y con víctima usa el dummy del vehículo, mina busca el suelo, el molotov en el agua desactiva la explosión y termina, fuegos con `& 3`, el primero en el centro, tercer argumento `i != 0`, estadística solo para el jugador 1, sombra 5 m por encima;
  - `Update`: tiempo restante con `_ftol2` y signo, luz con sombras extra, fuego del coche junto a la víctima (y antes que x), línea vertical 5 m por encima, temporizador de combustible `−(paso·0,02·−1000)` y comparación sin signo, progreso `·0,001`;
  - `TestForExplosionInArea`/`RemoveAllExplosionsInArea` con la semántica NaN y el orden de sumas del EXE.

**Método:**
- Semáforos sobre una entidad, un vehículo y nodos de ruta falsos (punteros de `ThePaths` cambiados y restaurados); los model infos de los semáforos apuntan a uno falso con efectos 2D.
- Pickups sobre huecos reales de `aPickUps`, un objeto con vtable propia, un jugador falso con armas, datos y búsqueda, y un pool de vehículos falso.
- Explosiones sobre `aExplosions` real y entidades víctima falsas con RwObject y frame.
- Se simulan y trazan rand, coronas, luces, sombras, FX, audio, HUD, texto, pads, peds, agua, suelo, fuegos, referencias y `FindPlayer*`; se guardan y restauran cámara, ciclo horario, `Players`, `Pads` y los globales de pickups.
- Fuera de los fixtures:
  - `GiveUsAPickUpObject` con hueco ≥ 0 que obliga a construir el objeto (constructor de la DLL no interceptable);
  - entidades sin matriz donde el EXE la asigna;
  - armas fuera de 0..46 (el EXE lee fuera de la tabla de munición).

Repetir: `python reversing/World12/build_probe.py` y `python reversing/run.py --case world12`. Evidencia: [validation-summary.json](validation-summary.json).
