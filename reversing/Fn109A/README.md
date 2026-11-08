<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 07/11/2026 (Ciudad de México):** 865 grupos / 9350 funciones reconstruidas; 9350 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Fn109A: dieciseis funciones de SetPiece, camara, vectores, escaneres, pool de rutas y ropa (lote 109A)

Estado: **APROBADO dentro del juego** (06/11/2026, worktree `wv-1` sobre main b00bfe4d9 + los cambios del lote). Evidencia: `E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn109a/20261106-113215-086576` (archivada en `reversing/CppTwelveLevel6Final/runs/Fn109A-20261106-113215-086576`). Banco par a par, 16 x 16384 = 262144 pares EXE/DLL, cero diferencias, compuertas de ABI/ruta/SEH/restauracion en ambos lados, x87 igual (pila vacia) y cada lado coincide con el oraculo Python independiente (`fixture_oracle.py`, que reconstruye ademas las arenas para predecir los bytes cambiados).

| Funcion | Direccion | Bytes | Fuente |
|---|---|---|---|
| `CSetPiece::GetSpawnCoord1` | 0x4997A0 | 50 | `SetPiece.cpp` (accesor en linea de la cabecera, ahora enganchado) |
| `CSetPiece::GetSpawnCoord2` | 0x4997E0 | 50 | idem |
| `CSetPiece::GetTargetCoord1` | 0x499820 | 50 | idem |
| `CSetPiece::GetTargetCoord2` | 0x499860 | 50 | idem |
| `CCam::ClipBeta` | 0x509C50 | 70 | `Cam.cpp` |
| `CCam::GetWeaponFirstPersonOn` | 0x509DC0 | 42 | `Cam.cpp` (ranura activa leida con signo, `movsx`) |
| `IsLampPost` (listado: CAudioZones) | 0x509A30 | 175 | `Cam.cpp` (14 palabras leidas SIN signo, `movzx`) |
| `VectorsDiffer` (listado: CAudioZones) | 0x509760 | 72 | `common.cpp` |
| `VectorsAreEqual` (listado: CAEDataStream) | 0x4DC670 | 54 | `common.cpp` |
| `CEventDamage::HasKilledPed` (listado: CEventSource) | 0x4ABCA0 | 23 | `Events/EventDamage.cpp` |
| `CVehicleScanner::GetClosestVehicleInRange` | 0x5FFEE0 | 4 | `VehicleScanner.cpp` |
| `CPedScanner::GetClosestPedInRange` | 0x5FFF20 | 4 | `PedScanner.cpp` |
| `CPointRoute::operator new` (listado: CNodeRoutePool) | 0x41B7C0 | 11 | `PedGeometryAnalyser.cpp` |
| `CPointRoute::operator delete` | 0x41B7D0 | 49 | `PedGeometryAnalyser.cpp` |
| `CPedClothesDesc::SetTextureAndModel` (hash) | 0x5A8050 | 38 | `PedClothesDesc.cpp` |
| `CPedClothesDesc::SetTextureAndModel` (nombre) | 0x5A8080 | 78 | `PedClothesDesc.cpp` |

Ninguna supera 1200 bytes ni depende de mas de una funcion sin hook. Se saltaron por clase dudosa o plantilla: los `CPedAttractor*` (plantillas), 0x48E180, 0x5FE9C0, 0x43A6A0, 0x572DB0 (clase no identificable), CMatrix (otros lotes) y las vtables de eventos sin clase propia.

## Revision contra el EXE
- **Accesores de SetPiece**: thiscall, `CVector2D` por puntero oculto en la pila (`ret 4`, puntero en EAX); dos palabras con signo `movsx` en +0xC/+0xE (+0x10, +0x14, +0x18), `fild` y `* 0.25` (0x858C84, exacto).
- **ClipBeta**: `beta > pi` (0x858CB8) resta 2pi (0x858CBC); si no, `beta < -pi` (0x858CC0) suma 2pi; un NaN no cambia nada.
- **GetWeaponFirstPersonOn**: objetivo +0x21C nulo o de tipo distinto de ped (byte +0x36 & 7 != 3) devuelve 0; si no, el byte `[ped + 0x5B4 + 0x1C * movsx(byte +0x718)]` tal cual (el C++ devuelve `bool`; los llamadores solo prueban AL, y el banco compara AL como verdad). La version anterior indexaba con `uint8` (con ranura 255 leeria 7 KB mas alla).
- **IsLampPost**: el argumento de 32 bits se compara con 14 PALABRAS sin signo; con el argumento -1 y un indice sin cargar (0xFFFF) el EXE no coincide (la version anterior, con `ModelIndex` con signo, si).
- **VectorsAreEqual / VectorsDiffer**: tres `fcomp` por orden x, y, z (cdecl, dos punteros); cero con signo igual a cero, un NaN nunca igual; el segundo devuelve `!iguales`.
- **CPointRoute::operator new** = `mov ecx,[0xB744B0]; jmp 0x41B5B0` (corre `CPointRoutePool::New` real en ambos lados, hook de 0x41B5B0 apagado en el EXE); **operator delete** = indice por division con signo entre 100, estado `|= 0x80`, cursor = min con signo. Los llamadores del EXE empujan un solo argumento (`add esp,4`).
- **SetTextureAndModel**: `GetTextureDependency` (0x5A7EA0) real en ambos lados; las claves se escriben sin comprobar el rango de la parte (se quito el `assert`; el hash por nombre calcula primero el de la textura y el del modelo, cada uno solo si el nombre no es nulo, y el del modelo aunque no haya dependencia).
- Todos los llamadores del EXE de las 16 direcciones se revisaron (ventana de instrucciones siguientes): ninguno lee ECX/EDX tras la llamada; los de SetPiece usan EAX (el puntero de salida) y los demas solo AL/EAX devueltos.
- Opciones de compilacion: ninguna especial (las funciones no usan x87 mas alla de comparaciones y un `* 0.25` exacto, que el SSE reproduce bit a bit).

## Banco
16 funciones x 16384 casos, 16 familias de 1024 (ver `Fixtures.hpp`). Memoria real tocada y restaurada: las 14 palabras de IsLampPost y el puntero del pool 0xB744B0 (apuntado a un pool propio de arena). `CKeyGen::GetUppercaseKey` 0x53CF30 va como falso identico (EXE y cuerpo de la DLL). Fuera de los fixtures: punteros no validos, ranuras de arma fuera de -8..14, bytes de bandera mayores que 1 (solo se compara AL como verdad), borrado doble (el cuerpo de la DLL tiene un `assert`) y partes fuera de -8..30.

## Pruebas negativas (`mutate.py`; copias buenas en `artifacts/good/`, restauradas por SHA256)
- Ronda 1 (cableado de cada accesor al vecino, signo de `ClipBeta`, ranura sin signo, indice con signo, comparaciones sin z / con z invertida): pares con diferencias 16384 x4 / 4294 / 1869 / 5424 / 3242 / 7581 (`20261106-120055-581150`; las funciones 9..15 no se ejercieron porque el cableado de `HasKilledPed` a un virtual hizo caer el juego).
- Ronda 2 (cableados cruzados, `>=` en el umbral, bandera negada, palabra repetida, comparaciones recortadas): 16384 x4 / 282 / 13849 / 1239 / 1216 / 3246 (`20261106-121027-566196`).
- Ronda 3 (funciones 9..15: `HasKilledPed` ligada a `GetAnimAdded`, escaneres y `new` devuelven nulo, `delete` no hace nada, claves intercambiadas): 8147 / 15360 / 15360 / 15781 / 16384 / 16324 / 14926 (`20261106-122233-366392`).
- Ronda 4 (escaneres y `new` con +4: 16384 / 16384 / 16384 sobre `20261106-124627-499955`; el cambio de `delete` hizo caer el juego) y ronda 5 (`delete` reserva antes de borrar, modelo con la clave de la textura, hash del modelo con el nombre de la textura): 14818 / 7512 / 10143 (`20261106-131407-155239`).
- Las 16 funciones fallan en al menos dos rondas.

## Bancos repetidos con el codigo final (citan las direcciones del lote)
Aprobados sin cambios en el banco: CamAim355, CamFirstPerson355, CamFollowCar355, CamFollowPed355, CamM16355, CamMouse355, CamControl355, CamBirdy355, CamLamp355, CamMan355, CppIntegerWorld10, EvHA, Veh83B, LeaveCarAbort70, CTasks14a, CheatFinal3, CppTrigCompound6Swim, CppTrigSwim, Mix11, ClothesRules355, GameLogic27, Shop30 (algunos hubo que relanzarlos tras quedarse E: sin espacio: WinError 112).
Adaptado y aprobado: `Helpers65D` (daba por sin hook `CPointRoute::operator new` 0x41B7C0; ahora el falso se declara con hook, `hooked=True` en `LiveComparison.hpp` y `run_local.py`).
No aprobados / no repetidos: `Mix12` (falla `CGrassRenderer::DrawTriPlants`, 839/1024: el EXE llama a un `geometry_for_all_materials` que la DLL no emite; no cita ninguna direccion del lote, ajeno) y `RequestedGeometryWrappersFive` (su `build_probe.py` esta congelado por SHA256 y la construccion aborta; pendiente de reabrir el banco).
