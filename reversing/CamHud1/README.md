<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 28/10/2026 (Ciudad de México):** 820 grupos / 8972 funciones reconstruidas; 8972 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CamHud1: banco por pares de CHud (24 funciones) y CCam (12 funciones)

Banco nuevo y simple (flujo Small2/PadMouse1/GeoFinal1) para 36 funciones de `CHud` y `CCam` (EXE GTA SA 1.0 US). Sin compilar ni ejecutar todavía.

Uso: `bash reversing/_handoff/bench.sh CamHud1 3`. Filtro opcional: `camhud1-only.txt` en la carpeta del juego con `Clase::funcion` (por ejemplo `CHud::DrawAmmo`).

## Corpus: 18688 pares

| Función | Pares | Función | Pares |
|---|---:|---|---:|
| CHud::HelpMessageDisplayed | 64 | CHud::DrawAmmo | 1024 |
| CHud::GetYPosBasedOnHealth | 512 | CHud::DrawBustedWastedMessage | 768 |
| CHud::SetMessage | 320 | CHud::DrawAreaName | 1024 |
| CHud::SetBigMessage | 1024 | CHud::DrawVehicleName | 1024 |
| CHud::SetVehicleName | 64 | CHud::DrawMissionTitle | 768 |
| CHud::SetZoneName | 256 | CHud::DrawOddJobMessage | 768 |
| CHud::ResetWastedText | 64 | CHud::DrawScriptText | 512 |
| CHud::GetRidOfAllHudMessages | 256 | CHud::DrawTripSkip | 128 |
| CHud::ReInitialise | 128 | CCam::WellBufferMe | 1024 |
| CHud::SetHelpMessage | 768 | CCam::DoCamBump | 128 |
| CHud::SetHelpMessageStatUpdate | 512 | CCam::GetVectorsReadyForRW | 128 |
| CHud::SetHelpMessageWithNumber | 512 | CCam::Constructor | 64 |
| CHud::DrawFadeState | 1024 | CCam::Init | 64 |
| CHud::RenderArmorBar | 512 | CCam::CacheLastSettingsDWCineyCam | 128 |
| CHud::RenderBreathBar | 512 | CCam::Finalise_DW_CineyCams | 512 |
| CHud::RenderHealthBar | 512 | CCam::GetCoreDataForDWCineyCamMode | 256 |
| CCam::Get_TwoPlayer_AimVector | 768 | CCam::Process_Cam_TwoPlayer_Separate_Cars_TopDown | 768 |
| CCam::Process_Editor | 1024 | CCam::Process_Fixed | 768 |

Cada par es: original (hook desactivado) frente a candidato (hook activo) con el mismo fixture determinista (semilla = índice de caso). Se comprueba que el conmutador funciona (`JumpsToDll` tras `State()`), el control x87 (`fp`, precisión a 24 bits como el juego) y la traza de colaboradores.

## Qué se compara

- **CHud**: las estáticas de CHud (0xBAA388..0xBAB1E4, más posY/guarda 0xBAB220..0xBAB22C y 0x8D0934/38) como hexadecimal entero (incluye los búferes de mensaje, el orden de bytes escritos fuera de rango como `m_ZoneToPrint` en `SetMessage` y la fila siguiente en `SetBigMessage`), `gString` y `gGxtString`, los valores de retorno (float por ST0, bool por AL) y los búferes falsos de entrada.
- **CCam**: el objeto `CCam` falso completo (0x238 bytes más guardas de 16 bytes, relleno 0xCD: detecta campos escritos de más o de menos), las salidas por referencia, `gIdleCam`, el pad 0, los globales de `Init`/`CacheLast`/`Finalise` (`DWCineyCam*`, `gHandShaker[0]`, 0x8CCB9C), el borrado de `TheCamera` (0xB6F0A0..0xB6F0D0) y el valor de `CWorld::pIgnoreEntity`.

## Intercepción (EntryPatch en EXE, símbolo de la DLL y su cuerpo; Body() se calcula ANTES de parchear)

Solo traza el hilo del banco (`owner`/`Mine()`); los hilos ajenos no tocan el registro.

- 18 funciones de `CFont` (cdecl, estáticas): SetBackground, SetScale, SetScaleForCurrentLanguage, SetOrientation, SetCentreSize, SetProportional, SetEdge, SetDropColor, SetFontStyle, SetColor, PrintString, PrintStringFromBottom, SetRightJustifyWrap, SetJustify, SetSlant, SetWrapx, SetBackgroundColor, SetDropShadowPosition. Los `bool`/`int8`/`int16` solo cuentan por su ancho real; los textos se trazan por huella del contenido GXT.
- `CSprite2d::DrawBarChart` (0x728640, 10 dwords) y `CSprite2d::Draw(CRect,CRGBA)` (0x728350, miembro: stub fastcall con dos argumentos de pila, `ret 8` igual que el EXE).
- `CPed::GetWeaponSkill()` (0x5E6580), `CMessages::AddToPreviousBriefArray` (0x69DD50, 8 dwords), `CCamera::GetFading` (0x50ADE0), `CCamera::GetScreenFadeStatus` (0x50AE20), `CTheScripts::DrawScriptSpritesAndRectangles` (0x464980).
- `CWaterLevel::GetWaterLevel` (0x6EB690; la sobrecarga del DLL con referencias tiene el mismo diseño de pila), `CCamera::ConeCastCollisionResolve` (0x51A5D0), `RwCameraSetNearClipPlane` (0x7EE1D0, el wrapper de la DLL llama a esa dirección), `CShadows::StoreShadowToBeRendered` (0x707390, 16 dwords), `CWeapon::FindNearestTargetEntityWithScreenCoors` (0x73E240) y el RNG (`rand` 0x821B1E con retorno int y `CGeneral::GetRandomNumber` con uint16: dos stubs con el mismo contenido).
- Reales (no se doblan): `HudColour.GetRGB/GetRGBA`, `CRGBA`, `TheText.Get`, `CMessages::StringCopy/StringCompare/InsertNumberInString/InsertPlayerControlKeysInString/GetStringLength`, `CWeaponInfo::GetWeaponInfo`, `CStats::GetFatAndMuscleModifier`, `CDarkel::FrenzyOnGoing`, `CIdleCam::Reset`, `CHandShaker::Process`, `CEntity::GetBoundCentre`, `FindPlayerVehicle`.

## Objetos falsos y fixtures

- Peds falsos en `CWorld::Players[0/1].m_pPed` (se guardan y restauran): armadura 0x548, salud 0x540, datos de jugador 0x480 (aliento +0x44), vehículo 0x58C, ranura de arma 0x718, armas 0x5A0 (+0x1C por ranura), bandera `bInVehicle` (0x46C bit 8), posición en +4 o matriz falsa en +0x14. Vehículos falsos con tabla virtual falsa (la ranura 14 es `Teleport`, traza `T_TELEPORT`; el resto cae en un stub que traza un error).
- `Scene.m_pRwCamera` (0xC1703C) apunta a una cámara RW falsa solo con el plano cercano en +0x80. `m_pCamTargetEntity` apunta a una entidad falsa (tipo vehículo, modelo del jugador para `GetBoundCentre`).
- Pad 1 (0xB73458+0x134) con `NewState`/`OldState` sintéticos para `Process_Editor`.
- Floats moderados (sin Inf, sin NaN de señalización; NaN silencioso solo donde el EXE lo trata de forma distinta: `BigMessageX`, comprobaciones con `fcomp`). Dominios coherentes con el juego (ranuras de arma 0..12, tipos de arma válidos, estados de HUD 0..4 en `DrawVehicleName` porque el EXE deja el alfa sin inicializar fuera de 1..4).
- Textos GXT con caracteres 0x20..0x7D (nunca `~`, que activaría etiquetas de teclas); `SetHelpMessageWithNumber` inserta `~1~` en la mitad de los casos.

## Enmascarado y ruido documentado

- El relleno de las estructuras no se compara fuera de lo que escribe el EXE: los objetos falsos se rellenan con 0xCD (CCam) o con ceros (peds/entidades) y se comparan enteros, de modo que una escritura de más o de menos aparece como diferencia real.
- `sprintf_s` de la DLL rellena con 0xFE tras el terminador; el EXE no. Las correcciones usan `sprintf`/escritura directa (por eso se compara `gString` entero).
- Los logs NOTSA (spdlog) de `Process_Editor` se eliminaron del código (el EXE solo formatea tres textos de depuración en un búfer local).
- Los hilos ajenos (audio) pueden llamar a `rand` 0x821B1E: el stub solo cuenta llamadas del hilo del banco.

## Fuera del banco (y por qué)

| Función | Motivo (dirección + instrucción) |
|---|---|
| `CHud::Draw` 0x58FAE0 | Doce `RwRenderStateSet` por la tabla del motor (`call dword ptr [eax+20h]`, 0x58FB22..0x58FBAA) y orquesta funciones bloqueadas de dibujo (`call 0058E020`, `0058EAF0`, `0058D9A0`, `0058B180`, `0058C6A0`...). |
| `CHud::DrawAfterFade` 0x58D490 | Tres `RwRenderStateSet` por la tabla del motor (0x58D499, 0x58D4A6, 0x58D4B3) y llamadas a `DrawSubtitles`/`DrawMissionTitle`/`DrawOddJobMessage`. |
| `CHud::DrawRadar` 0x58A330 | `call 00586B00` (CRadar::DrawMap), `jmp 00588050` (DrawBlips, cola) y estados RW (0x58A377, 0x58A384); dibuja con `CSprite2d::Draw(x1..y4)` 0x728520 sobre texturas RW reales. |
| `CHud::DrawWeaponIcon` 0x58D7D0 | Necesita el TXD real del arma: `call 00734E50` (RwTexDictionaryFindHashNamedTexture, 0x58D84B), estados RW por la tabla del motor (0x58D7DD, 0x58D866, 0x58D874) y `call 0070D000` (RenderOneXLUSprite, 0x58D8FD). |
| `CHud::Initialise` 0x5BA850 / `CHud::Shutdown` 0x588850 | E/S de ficheros y texturas reales: `call 007320B0` (CTxdStore::LoadTxd de MODELS\HUD.TXD, 0x5BA865), `call 007272B0` (CSprite2d::SetTexture x6, 0x5BA899), `call 00727240` (Delete x6, 0x588858), `call 00731CD0` (RemoveTxdSlot, 0x588873). |
| `CCam::Process_Rocket` 0x511B50 | `call 005E4280` (GetBonePosition de la cabeza, 0x511C32) exige una jerarquía de animación esquelética real; `call 0073F910` (PickTargetForHeatSeekingMissile, 0x511F2C), `call 0056A490` (GetIsLineOfSightClear, 0x511FD3) y `call 00742BF0` (MarkTarget, 0x512085) trabajan sobre el mundo real. |
| `CCam::Process_1stPerson` 0x517EA0 | `call 00512110` (Process_WheelCam, 0x51801D), `call 006B7150` (CBike::CalculateLeanMatrix, 0x5180D4), `call 005E4280` (hueso del cuello, 0x5181FC) y `call 0056A490` (0x51829F) exigen una moto/vehículo con clump y tabla de dummies reales (`cmp dword ptr [eax+3Ch],5` sobre `CVehicleModelInfo`, 0x5180F6). |

Las correcciones de estas ocho funciones se verificaron solo por lectura del ensamblador.

## Supuestos a revisar al compilar

- `CRect` se construye como `(left, bottom, right, top)` con los miembros en el orden `left, top, right, bottom`: las coordenadas "más grandes" van en +4 (campo `top`) y las menores en +0xC (campo `bottom`), como escribe el EXE (por ejemplo 0x58A1DF en DrawTripSkip).
- `CCam::Get_TwoPlayer_AimVector` ahora devuelve `CEntity*` (el EXE deja la entidad en EAX); ningún llamador del EXE (0x514A2E, 0x6219DC, 0x74049A) la lee.
- Los desplazamientos de `CPlayerPedData` (+0x94, +0x98, +0xA0) y del byte 0x614 del vehículo en `Process_Rocket`/`Process_1stPerson` se escriben en bruto porque el EXE los usa así (campo de 31 bits guardado como booleano).
- Las firmas de pila de los stubs miembro (`__fastcall(self, _, ...)`) coinciden con thiscall en EXE y DLL.

## Aprobación (09/10/2026)

Corrida `20261009-155715-312758`: passed=true, 18688 pares, 36 funciones con 0 fallos (24 de CHud y 12 de CCam), evento complete (state_restored), begin con known=true y 31 sitios correctos, saves y configuraciones intactos. Registrada con `register_camhud1.py` (evidencia en `runs/`, `validation-summary.json`). Solo se registran las funciones con casos en la corrida. Siguen PENDIENTES las ocho funciones fuera del banco (ver la sección «Fuera del banco»): CHud::Draw, DrawAfterFade, DrawRadar, DrawWeaponIcon, Initialise, Shutdown, CCam::Process_Rocket y Process_1stPerson; CHud queda con 6 pendientes y CCam con 2. Límites: textos GXT sin `~`, floats moderados, peds/vehículos/cámara RW falsos y colaboradores doblados (sin crédito).
