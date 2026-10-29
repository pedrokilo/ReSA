<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Audio, animación, objetos de cinemática, puertas y vídeo (lista de 6)

Validadas el 02/10/2026 con **13.568 pares original/candidato dentro de GTA**, todos exactos: 42 funciones de 7 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAEBankLoader` | `CalculateBankSlotsInfosOffsets`, `Deconstructor`, `GetBankLookup`, `LoadBankLookupFile`, `LoadBankSlotFile`, `LoadSFXPakLookupFile` |
| `CAEGlobalWeaponAudioEntity` | Constructor, Destructor, `AddAudioEvent`, `ProjectileFire`, `ServiceAmbientGunFire`, `UpdateParameters` |
| `CAnimBlendClumpData` | Constructor, Destructor, `ForAllFrames`, `ForAllFramesInSPR`, `LoadFramesIntoSPR`, `SetNumberOfBones` |
| `CAnimBlendStaticAssociation` | `Constructor1`, `Constructor2`, Destructor, `Init`, `AllocateSequenceArray`, `FreeSequenceArray` |
| `CCutsceneObject` | `SetModelIndex`, `ProcessControl`, `PreRender`, `SetupLighting`, `RemoveLighting` (vtable 0x868A60), `SetupCarPipeAtomicsForClump` |
| `CDoor` | `Open`, `GetAngleOpenRatio`, `IsClosed`, `IsFullyOpen`, `Process`, `ProcessImpact` |
| `VideoPlayer` | `Play`, `UpdateWindow`, `OnGraphNotify`, `WaitState`, `Shutdown`, `Release` |

**Diferencias reales corregidas:**
- `CAEBankLoader`:
  - los ficheros se dan por abiertos solo con handle positivo (comparación con signo);
  - `LoadBankLookupFile` devuelve cierto sin mirar el puntero y cierra antes de salir en el error;
  - `operator delete` incondicional, también con punteros nulos;
  - `strcpy`/`strcat` en lugar de las variantes `_s` (que rellenan con 0xFE en debug).
- `CAEGlobalWeaponAudioEntity`:
  - el constructor pone a cero `m_FrequencyVariation`;
  - `UpdateParameters` anula solo la primera cascada que coincide y se engancha por la vtable (con `RH_ScopedInstall` el hook entraba en bucle con objetos de vtable del EXE);
  - `ProjectileFire` usa `m_tempSound` (no una voz local), la tabla de frecuencias del EXE con módulo entero y, para el segundo sonido, el banco 0x8A en el slot 19 sin mirar la melodía de carga;
  - `ServiceAmbientGunFire`:
    - los disparos suenan cuando el jugador está a **más** de 24 unidades (estaba invertido) y siguen cuando el tiempo es igual;
    - la segunda sirena espera 6,5 s (la condición estaba invertida);
    - la primera sirena comprueba el banco de sonidos del frontend;
    - cascadas: distancia, banco y punteros en el orden del EXE, sin anular los punteros al pararlas;
    - voces locales con solo los campos que escribe el EXE (volumen 12 en las sirenas);
    - `fcos`/`fsin` a precisión completa, 2π del EXE y `min` del EXE para la altura;
    - tiradas con `CAEAudioUtility::GetRandomNumberInRange(0, 1)` y `GetRandomIntInRange(0, 100)`;
    - la guarda de `s_LastTime` se comprueba en cada llamada.
- `CAnimBlendClumpData`:
  - `FreeAlign` recibía la dirección del campo en lugar del bloque;
  - el tamaño de `SetNumberOfBones` se redondea a múltiplos de 64;
  - `ForAllFrames` cuenta con signo y relee el número de frames.
- `CAnimBlendStaticAssociation`:
  - el destructor con borrado recibe las banderas (`ret 4`) y destruye arrays (bit 1);
  - los constructores no escriben `m_NumBlendNodes`;
  - `Init`: `movsx` del número de nodos y de secuencias, búsqueda del frame antes de mirar el número de frames (con signo), sin `assert`;
  - `AllocateSequenceArray` no entra en el bucle con contadores negativos.
- `CCutsceneObject`:
  - `ProcessControl` llama a `UpdateRpHAnim` del objeto enganchado y limpia `m_bWasPostponed`; con NaN en el paso de tiempo divide;
  - `PreRender` copia la traslación del keyframe del primer frame (no `BonePos`), pide el primer atomic dos veces y mezcla la iluminación con 1/30 y `timestep * 0,1` del EXE;
  - `SetModelIndex` pone el alfa del modelo del índice recibido.
- `CDoor`:
  - `Process`: signo de la rotación en los ejes X y Z, con otros ejes usa el seno, `fsin`/`fcos` y `_CIpow` del EXE, producto de matriz con el orden de sumas de 0x59C810, sin `assert`, sentido de apertura con NaN;
  - `ProcessImpact`: con ejes distintos de X y Z compara la componente Y de la velocidad anterior;
  - `Open` y `IsFullyOpen` tratan NaN como el EXE.
- `VideoPlayer`:
  - `UpdateWindow` llama a `SetFocus` aunque falle `put_MessageDrain`; devuelve el HRESULT, igual que `OnGraphNotify`;
  - `Play` (sin `FIX_BUGS`) convierte como mucho 255 caracteres en un búfer sin inicializar.

**Método:**
- Ficheros, CdStream, operator new/delete y `CMemoryMgr` simulados con un área fija; nunca se abre ningún fichero real.
- Hardware de audio, gestor de sonidos, `CAESound` y `CAudioEngine` simulados y trazados; las voces y entidades son falsas.
- El plugin de animación, RW (jerarquías, matrices, atomics, skin), sombras, luces y `ProcessVerticalLine` se sustituyen por trazas sobre clumps falsos.
- VideoPlayer corre con objetos COM falsos y con `GetClientRect`, `SetFocus`, `UpdateWindow`, `CoInitialize` y `CoCreateInstance` sustituidos solo durante cada llamada.
- Se guardan y restauran el reloj, la cámara, los estáticos de `ServiceAmbientGunFire`, las voces globales, la región y la niebla, el estado del juego, los punteros de DirectShow, las claves de vehículos de cinemática, el alfa de los modelos y la lista de matrices.
- Fuera de los fixtures o de la comparación:
  - los bytes del constructor de `CAEGlobalWeaponAudioEntity` que el EXE no escribe (la DLL inicializa las bases `CAEAudioEntity`/`CAESound`/`CAEWeaponAudioEntity`);
  - las vtables (se compara el tipo);
  - las direcciones fuera de 0..5 en `ProcessImpact` (el EXE usa basura de la pila);
  - `Play` con `FIX_BUGS` (búfer dinámico) solo se compara por el texto convertido.

Repetir: `python reversing/AnimAudio6/build_probe.py` y `python reversing/run.py --case animaudio6`. Evidencia: [validation-summary.json](validation-summary.json).
