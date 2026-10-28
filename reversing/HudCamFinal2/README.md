<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8969 funciones reconstruidas; 8969 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# HudCamFinal2: banco por pares de las 9 funciones pendientes de CHud, CCam y CPickups

Banco nuevo (flujo Small2/PadMouse1/CamHud1/PickWeap1) para las funciones que `CamHud1` y `PickWeap1` dejaron fuera por sus colaboradores del motor
(EXE GTA SA 1.0 US). Sin compilar ni ejecutar todavía.

Uso: `bash reversing/_handoff/bench.sh HudCamFinal2 3`. Filtro opcional: `hudcamfinal2-only.txt` en la carpeta del juego con `Clase::funcion`
(por ejemplo `CHud::DrawRadar`).

## Corpus: 6528 pares

| Función | Dirección | Pares |
|---|---|---:|
| CHud::Draw | 0x58FAE0 | 1024 |
| CHud::DrawAfterFade | 0x58D490 | 512 |
| CHud::DrawRadar | 0x58A330 | 1024 |
| CHud::DrawWeaponIcon | 0x58D7D0 | 768 |
| CHud::Initialise | 0x5BA850 | 64 |
| CHud::Shutdown | 0x588850 | 64 |
| CCam::Process_1stPerson | 0x517EA0 | 1024 |
| CCam::Process_Rocket | 0x511B50 | 1024 |
| CPickups::Update | 0x458DE0 | 1024 |

Cada par es: original (hook desactivado = código del EXE) frente a candidato (hook activo) con el mismo fixture determinista (semilla = índice de caso).
Se comprueba que el conmutador funciona (`JumpsToDll` después de `State()`), el control x87 (`fp`, precisión a 24 bits como el juego) y la traza de
colaboradores (nombre, orden y argumentos de cada llamada saliente).

## Archivos

- `Common.hpp`: trazas sin reservas (`Push`/`Recs`), zonas globales guardadas/restauradas, registro de colaboradores (`AddSite`/`Use`), `Body()` que no entra en el EXE.
- `HudCases.hpp`: dobles y casos de CHud (motor RW falso, dibujo, TXD y sprites).
- `CamCases.hpp`: dobles y casos de `Process_1stPerson` y `Process_Rocket`.
- `PickupCases.hpp`: dobles y caso de `CPickups::Update`.
- `LiveComparison.hpp`: tabla `fns`, comprobación de disposición (evento `layout`), bucle principal.

## Cómo se doblan los colaboradores

- **EntryPatch por función probada**: cada colaborador se registra con la dirección del EXE, el símbolo de la DLL y su cuerpo (`Body()`, calculado ANTES de
  crear ningún EntryPatch y sin entrar en el EXE). Los parches solo existen mientras se prueba la función que los usa (se destruyen antes de la siguiente),
  de modo que un colaborador que es a la vez función probada (p. ej. `CHud::DrawRadar` dentro de `CHud::Draw`) nunca se parchea mientras se prueba a sí mismo.
- **Hilos ajenos**: todos los dobles filtran por `Mine()` (hilo del banco). `FindPlayerPed/FindPlayerVehicle` devuelven a los hilos ajenos los valores reales
  capturados al empezar.
- **Pila**: cada doble respeta el `ret N` del original (desensamblado con `dis.sh`): `CSprite2d::Draw(x1..y4,color)` `ret 0x24`, `Draw(rect,color)` `ret 8`,
  `GetBonePosition` `ret 0xC`, `Process_WheelCam` `ret 0x10`, `CCamera::SetMotionBlur` `ret 0x14`, `CPickup::Update` `ret 0xC`, etc.
- **Estados RW**: el EXE y la DLL llaman `RwRenderStateSet` por `RwEngineInstance->dOpenDevice.fpRenderStateSet` (`call dword ptr [eax+20h]`). El banco cambia
  temporalmente el puntero `0xC97B24` por una tabla falsa cuyo +0x20 es un doble con traza (estado, valor); se restaura tras cada ejecución y el evento
  `layout` comprueba que `fpRenderStateSet` está en +0x20 en la DLL.
- **TXD/sprites (Initialise/Shutdown)**: `CTxdStore::AddTxdSlot/LoadTxd/AddRef/PushCurrentTxd/SetCurrentTxd/PopCurrentTxd/FindTxdSlot/RemoveTxdSlot`,
  `CSprite2d::SetTexture/Delete` están doblados con traza por contenido (huella del texto): nunca se carga `HUD.TXD`, no se crea ni se destruye ninguna textura y no se toca el
  pool de TXD. `ReInitialise` (tail call del EXE) es real y valida el estado de HUD completo.
- **DrawWeaponIcon**: usa modelos de arma y ranuras de TXD reales (solo lectura; solo ranuras en uso, el EXE desreferencia la dirección 0 con una ranura libre,
  0x58D825 `xor ecx,ecx` + 0x58D831 `mov esi,[ecx]`). El `m_pRwDictionary` de la ranura se sustituye temporalmente por un diccionario falso o nulo (la zona
  de 12 bytes de cada `TxdDef` usado se guarda y restaura) y `RwTexDictionaryFindHashNamedTexture` (0x734E50) devuelve una textura falsa o nula;
  `CSprite::RenderOneXLUSprite` (0x70D000, 13 dwords) y `CSprite2d::Draw(rect,color)` están doblados.
- **Huesos, colisión y mundo (Process_1stPerson/Rocket)**: `CPed::GetBonePosition` (0x5E4280), `CWorld::GetIsLineOfSightClear` (0x56A490, secuencia de resultados
  por caso), `CWaterLevel::GetWaterLevel` (0x6EB690), `CCamera::SetMotionBlur` (0x50BF40), `CCam::Process_WheelCam` (0x512110), `CBike::CalculateLeanMatrix` (0x6B7150),
  `CPlaceable::AllocateMatrix` (0x54F560; el doble enlaza una matriz preparada por el fixture), `CSimpleTransform::UpdateMatrix/UpdateRwMatrix`, `CMatrix::UpdateRwMatrix`,
  `CEntity::UpdateRwFrame/UpdateRpHAnim`, `CPad::LookAroundLeftRight/UpDown`, `CWeapon::PickTargetForHeatSeekingMissile` (0x73F910), `CWeaponEffects::MarkTarget` (0x742BF0)
  y `RwCameraSetNearClipPlane` (0x7EE1D0).
- **CPickups::Update**: `CPickup::IsVisible`, `GiveUsAPickUpObject`, `CPickup::Update` (puede anular el tipo o la visibilidad del pickup), `CWorld::Add/Remove`,
  `CPad::CollectPickupJustDown/GetTarget`, `CPedIntelligence::FindTaskByType` (la traza distingue la inteligencia de Players[0] y Players[1]).
- **Reales (no se doblan)**: `CPad::GetPad`, `CText::Get`, `CKeyGen::AppendStringToKey`, `CVehicle::IsUpsideDown`, `CVector::Normalise`/`CrossProduct`, `CGeneral::GetATanOfXY`,
  `CCam::ClipBeta`, `GetVectorsReadyForRW`, `CHud::ReInitialise`, `AddToCollectedPickupsArray`.

## Objetos falsos y fixtures

- Pickups: el array REAL (0x9788C0, 620 x 0x20) y los globales contiguos 0x978610..0x97D690 se rellenan con datos deterministas y se restauran; los objetos
  son 64 objetos falsos con tabla virtual falsa cuya ranura 0 (destructor con borrado, `push 1; call [eax]`) se traza. El contador de fotogramas recorre los 32 sub-rangos
  del primer bucle y los 6 del segundo; `Players[1].m_pPed` existe en 2/3 de los casos (ambos jugadores), con jugadores falsos cuya `m_pIntelligence` (+0x47C) es distinta.
- CCam: objeto falso de 0x238 bytes relleno con 0xCD y comparado entero (campos escritos de más o de menos). Entidades falsas: ped (tipo 3, rotación +0x558, objetivo +0x71C),
  vehículo (tipo 2, subtipo +0x594, tipo +0x590, byte +0x614, conductor +0x460, matriz de inclinación +0x5CC, velocidad +0x44) con una ranura de ModelInfo sin usar (id 19995,
  guardada/restaurada) que apunta a un ModelInfo falso (+0x3C tipo, +0x5C estructura con la tabla de dummies). Objetivo de misil, jugador (`m_pPlayerData` +0x480 con +0x94/+0x98/+0xA0),
  cámara RW y ped de TheCamera (0xB6F980) también falsos.
- HUD: la zona de estáticas de CHud (0xBAA388.., 0xBAB1E4..0xBAB214 con los sprites, 0xBAB220..), jugador/vehículo/ped falsos por el doble de `FindPlayerPed/Vehicle`.
  `m_bDraw3dMarkers` toma 0, 1, 2 y 0x80 (el EXE compara el byte con 1, 0x58FCA6).
- Floats moderados (sin Inf, sin NaN de señalización ni NaN silenciosos salvo donde el asm los trata de forma específica); tiempo en ms con valores >= 2^31 y > 2^24 para ejercitar
  la suma `fild + 2^32` de `Process_1stPerson` (0x517F58).
- Dominios coherentes con el juego: ranuras de arma 0..12, tipos de arma 0..46, `CollectedPickUpIndex` < 20, ranuras de TXD en uso, vehículos que dibujan el anillo del avión
  siempre con matriz (el EXE desreferencia `[eax+14h]` sin comprobar, 0x58A3ED).

## Enmascarado y ruido documentado

- El relleno de las estructuras no se compara fuera de lo que escribe el EXE: los objetos falsos se rellenan con 0xCD (CCam) o con ceros y se comparan enteros, de modo que una
  escritura de más o de menos aparece como diferencia real.
- `CPad::GetDisplayVitalStats` devuelve AX en el EXE (0x58FC29 `test ax,ax`) y AL en la DLL: el doble devuelve 0 o 1 (ambos bytes coinciden).
- `Process_1stPerson` guarda el byte `0x614` del vehículo (bit 7 = bWheelieForCamera) sea cual sea el tipo: el fixture escribe un byte aleatorio completo.
- `ZoneScoped` (Tracy) y los `assert` de depuración de la DLL no existen en el EXE: los fixtures no entran en esas ramas (índices de pool siempre dentro de rango).

## Correcciones de fuente (evidencia asm)

- `CPickups::Update` (Pickups.cpp): la rama del jugador 2 NO es un `else`. En 0x459033 `test al,al; je 0x459069` el EXE cae a 0x459069 cuando `CPickup::Update` del jugador 1 devuelve
  falso, y allí (0x459069 `test bl,bl` con `bl` = jugador 2 permitido, que implica jugador 1 libre) vuelve a comprobar tipo (0x45906D `cmp byte ptr [esi],0`) y visibilidad (0x459072 `test byte ptr [esi+1],8`)
  y llama a `Update(FindPlayerPed(1), FindPlayerVehicle(1), 1)`. La fuente anterior afirmaba que esa rama no se ejecutaba nunca.
- `CHud::Draw` (Hud.cpp): `m_bDraw3dMarkers` se compara con `cmp byte ptr ds:[0BAA45Ch],1` (0x58FCA6), no con un test de cero; ahora lee el byte y compara con 1.
- `CHud::DrawRadar` (Hud.cpp): el estado 7 (0x58A384 `push 1; push 7`) es `rwRENDERSTATESHADEMODE` con `rwSHADEMODEFLAT`; la fuente usaba `rwFILTERNEAREST` (mismo valor 1, cambio solo de legibilidad).
- `CCam::Process_1stPerson` (Cam.cpp): el tiempo con signo negativo se convierte con una sola redondeada (`fild` + `fadd 2^32`, 0x517F4E..0x517F5E) y no con dos; el modelo del vehículo es
  `movsx eax,word ptr [ecx+22h]` (0x5180EB), con lo que el índice es con signo y la estructura de dummies sale del mismo ModelInfo (`mov eax,[eax+5Ch]`, 0x5180FA).

El resto de las funciones se contrastó instrucción a instrucción sin otras diferencias (ver el informe de la sesión).

## Fuera del banco

Ninguna de las 9 funciones queda fuera. Límites de lo cubierto:

- `CHud::DrawWeaponIcon` con una ranura de TXD libre (el EXE lee la dirección 0, 0x58D831 `mov esi,[ecx]` con `ecx=0`) no se prueba: la fuente devuelve en ese caso, el EXE revienta.
- `CPickups::Update` con `CollectedPickUpIndex >= 20` no se prueba (el EXE escribe fuera de la tabla antes de reiniciar el índice; la DLL aborta en depuración).

## Supuestos a revisar al compilar

- `RwRenderStateSet` es el macro en línea de rwplcore.h (lee `RwEngineInstance` 0xC97B24); si `RWDEBUG` estuviera definido, el cambio de puntero no bastaría.
- `CPlaceable::AllocateMatrix`, `CSimpleTransform::UpdateMatrix`, `CMatrix::UpdateRwMatrix` y `CSimpleTransform::UpdateRwMatrix` no están inlinadas en la DLL de depuración
  (si lo estuvieran, el doble del lado DLL no se vería y la traza diferiría).
- `CWaterLevel::GetWaterLevel(const CVector&, float&, bool, CVector*)` de la DLL llama a la sobrecarga de seis argumentos (la que se dobla); es el mismo supuesto de CamHud1.
- Los nombres `offsetof` del evento `layout` (`CCam`, `CBike::m_mLeanMatrix`, `CWeaponEffects`, `TxdDef`, `CBaseModelInfo`) son miembros públicos.
- `CPickup::Update(CPlayerPed*, CVehicle*, int32)` y `CPedIntelligence::FindTaskByType(eTaskType)` son los únicos símbolos con esos nombres (sin sobrecargas).

## Aprobación (09/10/2026)

Corrida `20261009-180840-287896`: passed=true, 6528 pares, 9 funciones con 0 fallos, evento complete (state_restored, layout_ok), evento layout correcto, evento begin con known=true y todos los sitios correctos, saves y configuraciones intactos. Registrada con `register_hudcamfinal2.py` (evidencia en `runs/`, `validation-summary.json`). Limites: `CHud::DrawWeaponIcon` con ranura TXD libre no se prueba; `CPickups::Update` con `CollectedPickUpIndex>=20` no se prueba y con `CPickup::Update` doblada; dibujo RW con tabla falsa de estados; colaboradores doblados sin credito.
