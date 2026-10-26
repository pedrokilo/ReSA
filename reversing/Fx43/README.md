<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Audio del clima, efectos, fuego, entrada y JPEG (lista de 43)

Validadas el 02/10/2026 con **15.552 pares original/candidato dentro de GTA**, todos exactos: 44 funciones de 9 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAEWeatherAudioEntity` | `Service`, `StaticInitialise`, `StaticReset`, `UpdateParameters` (vtable 0x872A74, slot 0) |
| `CBulletTraces` | `AddTrace` (normal y envoltorio), `Init`, `Render`, `Update` |
| `CFire` | Constructor, `CreateFxSysForStrength`, `Extinguish`, `Initialise`, `ProcessFire` |
| `CMovingThings` | `Init`, `Render`, `Render_BeforeClouds`, `Shutdown`, `Update` |
| `CShotInfo` | `AddShot`, `GetFlameThrowerShotPosn`, `Initialise`, `Shutdown`, `Update` |
| `CWaterCannon` | `Init`, `PushPeds`, `Render`, `Update_NewInput`, `Update_OncePerFrame` |
| `FxMemoryPool_c` | `Exit`, `GetMem`, `Init`, `Optimise`, `Reset` |
| `Input` | `AttachInputDevices`, `HandleKeyDown`, `HandleKeyUp`, `KeyboardHandler`, `PadHandler` |
| `JPegPlugin` | `JPegCompressScreen`, `JPegCompressScreenToBuffer`, `JPegCompressScreenToFile`, `JPegDecompressToRaster`, `JPegDecompressToVramFromBuffer` |

**Diferencias reales corregidas:**
- `CAEWeatherAudioEntity`:
  - `UpdateParameters`:
    - trueno: duración sin signo, log10 con `fyl2x` sin la saturación de `AudioLog10`, `pow` del EXE (0x822130) y ×0,05;
    - ruido de ciudad:
      - posición exacta (-0,906; 0,423; 0);
      - origen y destino de la línea de visión estaban invertidos;
      - se usa la velocidad de la entidad golpeada (no su posición);
      - sumas z+y+x y factor 1/6;
      - residuo y última entidad como el EXE;
    - viento:
      - la DLL escribía un float en el bool `sbWindOffset`;
      - tablas y pasos leídos de los globales del EXE;
      - frecuencia con suelo 1,0;
    - otros eventos no hacen nada (antes `NOTSA_UNREACHABLE`).
  - `Service`:
    - posiciones y flags de cada sonido (0x1005/0x1025);
    - un `CAESound` en la pila con `Initialise` + `RequestNewSound`;
    - bucles dobles con porcentajes 0/0 y 50/50, sfx (0,1) a la derecha;
    - sin banco cargado se sale antes del canal derecho;
    - gotas: el aleatorio va antes de la probabilidad, entidad = this, flags 3 y el id se guarda como word.
- `CBulletTraces`:
  - `AddTrace`:
    - la vida depende de cuántas trazas existen (no del índice del hueco libre);
    - el sonido usa `m_mCameraMatrix` (0xB6F99C) con los órdenes de suma del EXE;
    - volumen log10×20 (faltaba ×20);
    - izquierda y derecha estaban invertidas;
  - el envoltorio usa `rand()` en línea y `std::min` como macro (segundo `rand`);
  - `Render` usa dirección normalizada × longitud × t.
- `CFire`:
  - `CreateFxSysForStrength`: NaN elige "fire_large".
  - `ProcessFire`:
    - la fuerza nueva se revierte si cambia la parte entera (la DLL hacía lo contrario);
    - dummy del coche transformado con el orden de 0x59C890;
    - pools recorridos hacia atrás, sin coches ya ardiendo ni camiones de bomberos;
    - fuego rastrero con el segundo `rand` en x, normalizado, +2 en z y generaciones-1;
    - al fusionar se mueve este fuego, con el menor número de generaciones;
    - `SetConstTime` usa (`m_TimeToBurn` - ahora)/3500;
    - distancia de borrado en 2D.
- `CShotInfo`:
  - `Initialise` va campo a campo, sin tocar el relleno, y la tabla acumula +0,005 en x87;
  - `AddShot` usa `rand()%20` y `DestroyTime = (float)(int)(vida + tiempo)`;
  - `Update`:
    - el tiempo por encima de 2^31 se redondea a float (`fild` + `fadd 2^32`);
    - frenado con el `_CIpow` del EXE;
    - aumento de alcance 0,075 (era 0,0075);
    - índices 16..1 leídos en crudo;
    - daño con `movsx`.
- `CWaterCannon`:
  - `m_nSectionsCount` es int16;
  - `PushPeds`:
    - caja de ±10000 (el `FLT_MIN` era un error);
    - solo peatones con colisión, estrictamente dentro de la caja;
    - suma a la velocidad propia del peatón;
  - `Render`:
    - `rand()` extra y 31 vueltas;
    - orden A/B y línea de visión de B a A;
    - la segunda partícula va a `m_SmokeII3expand`;
    - la sección con impacto se renderiza antes de salir.
- `FxMemoryPool_c`:
  - `Exit` liberaba `this` en vez de `m_pData`;
  - `Init` devuelve bool;
  - `GetMem` prueba con signo el resto sin signo.
- `PadHandler`:
  - el número de pad sale solo de `CPad::padNumber`;
  - las funciones de configuración se llaman en cada vuelta (botón 0 si no está pulsado).
- `HandleKeyDown/Up`: códigos negativos con `.data()[]`.
- `JPegPlugin`:
  - `JPegDecompressToVramFromBuffer` estaba enganchada en 0x5D0320 (su callback `fill_input_buffer`); ahora va en 0x5D07A0 y devuelve bool;
  - `ToBuffer` no comprimía; ahora el term copia solo el remanente;
  - `ToFile` guarda el handle en 0xBD0B74 con prueba `> 0` con signo;
  - `JPegDecompressToRaster`:
    - 0xBD0170 es el propio buffer de la scanline;
    - `dct_method` se fija antes de empezar;
    - el píxel se escribe como dword crudo (`CRGBA(uint32)` reordenaba bytes).

**Método:**
- Objetos falsos para todo.
- Se sustituyen por pools falsos los de peatones, vehículos y objetos; la entrada 596 de `ms_modelInfoPtrs` se cambia temporalmente.
- Se simulan, en la dirección del EXE y en el símbolo de la DLL:
  - el gestor de sonidos y los bucles de lluvia;
  - efectos, `StartFire`, línea de visión e Im3D;
  - `CMemoryMgr` (solo durante `FxMemoryPool_c`);
  - `RsInputDeviceAttach`, `CControllerConfigManager` y `CFileMgr`;
  - la libjpeg (la del EXE y libjpeg-turbo, cada una con su disposición de estructuras).
- Se guarda y restaura todo el estado global tocado.

**Desviaciones mantenidas:**
- La DLL usa libjpeg-turbo con `JCS_EXT_RGBA` (NOTSA; el EXE usa `JCS_RGB`); el banco normaliza el espacio de color a "rgba".
- `CAESoundManager::PlaySound` pone `ClientVariable` 0 por defecto (el EXE pone -1); `Service` ya no lo usa.

Repetir: `python reversing/Fx43/build_probe.py` y `python reversing/run.py --case fx43`. Evidencia: [validation-summary.json](validation-summary.json).
