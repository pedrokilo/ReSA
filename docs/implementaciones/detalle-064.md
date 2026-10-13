<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 12/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Detalle de implementaciones

[Resumen y metodología](../ImplementacionesDelJuego.md). Validación histórica no demuestra independencia ni vigencia con todas las fuentes actuales.

<a id="grupo-1261"></a>

## Globales y helpers: source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxInterpInfoU255_c | [source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.h:5](../../source/game_sa/Fx/Blueprints/Interp/FxInterpInfoU255.h#L5) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1262"></a>

## Globales y helpers: source/game_sa/Fx/Fx.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CreateFxWithinCameraRange(const char* name, const CVector& pos, float range) | [source/game_sa/Fx/Fx.cpp:141](../../source/game_sa/Fx/Fx.cpp#L141) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo TheCamera: source/game_sa/Camera.cpp:39 (0x00B6F028); Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |
| RenderAddTri(CVector pos1, CVector pos2, CVector pos3, RwTexCoords coord1, RwTexCoords coord2, RwTexCoords coord3, const CRGBA& color1, const CRGBA& color2, const CRGBA& color3) | [source/game_sa/Fx/Fx.cpp:456](../../source/game_sa/Fx/Fx.cpp#L456) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:32 (0x00A9AE00) |

<a id="grupo-1263"></a>

## Globales y helpers: source/game_sa/Fx/FxEmitter.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxEmitter_c | [source/game_sa/Fx/FxEmitter.h:7](../../source/game_sa/Fx/FxEmitter.h#L7) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1264"></a>

## Globales y helpers: source/game_sa/Fx/FxEmitterBP.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RenderEmitterParticles355(FxEmitterBP_c& emitter, RwCamera* camera, float& brightness, bool heat) | [source/game_sa/Fx/FxEmitterBP.cpp:58](../../source/game_sa/Fx/FxEmitterBP.cpp#L58) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Conversión de dirección fija: 0x00B6FA14; Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:32 (0x00A9AE00); Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80); Posible dato fijo m_Color: source/game_sa/Font.h:99 (0x00C71A60); Posible dato fijo ms_fTimeStep: source/game_sa/Timer.h:33 (0x00B7CB5C) |

<a id="grupo-1265"></a>

## Globales y helpers: source/game_sa/Fx/FxEmitterBP.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxEmitterBP_c | [source/game_sa/Fx/FxEmitterBP.h:7](../../source/game_sa/Fx/FxEmitterBP.h#L7) | C++ · Cuerpo fuente con errores de análisis | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1266"></a>

## Globales y helpers: source/game_sa/Fx/FxInfoManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NewFxInfoInPool() | [source/game_sa/Fx/FxInfoManager.cpp:83](../../source/game_sa/Fx/FxInfoManager.cpp#L83) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fxMan: source/game_sa/Fx/FxManager.cpp:37 (0x00A9AE80) |

<a id="grupo-1267"></a>

## Globales y helpers: source/game_sa/Fx/FxManager.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * NewFxEmitterArray() | [source/game_sa/Fx/FxManager.cpp:92](../../source/game_sa/Fx/FxManager.cpp#L92) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1268"></a>

## Globales y helpers: source/game_sa/Fx/FxPrim.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| FxPrim_c | [source/game_sa/Fx/FxPrim.h:6](../../source/game_sa/Fx/FxPrim.h#L6) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1269"></a>

## Globales y helpers: source/game_sa/Fx/FxSystem.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| CanAddParticle() | [source/game_sa/Fx/FxSystem.cpp:201](../../source/game_sa/Fx/FxSystem.cpp#L201) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo g_fx: source/game_sa/Fx/Fx.cpp:32 (0x00A9AE00) |

<a id="grupo-1270"></a>

## Globales y helpers: source/game_sa/Fx/FxSystemBP.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| RH_ScopedInstall(operator new, 0x4AA100); RH_ScopedInstall(Constructor, 0x4AA0D0); RH_ScopedInstall(Destructor, 0x4AA0F0); RH_ScopedInstall(Load, 0x5C05F0); RH_ScopedInstall(Unload, 0x4AA120); RH_ScopedInstall(Update, 0x4AA130); RH_ScopedInstall(Render, 0x4AA160); RH_ScopedInstall(FreePrtFromSystem, 0x4AA1B0); RH_ScopedInstall(GetBoundingSphere, 0x4AA1F0); RH_ScopedInstall(SetBoundingSphere, 0x4AA200); } // 0x4AA100 // Los blueprints se almacenan en la arena FX porque viven durante todo el proyecto cargado. void* FxSystemBP_c::operator new(unsigned size) { return g_fxMan.GetMemPool().GetMem(size); } // NOTSA void FxSystemBP_c::operator delete(void* data) { CMemoryMgr::Free(data); } // 0x4AA0D0 FxSystemBP_c::FxSystemBP_c() : ListItem_c() { m_BoundingSphere = nullptr; } FxSystemBP_c* FxSystemBP_c::Constructor() { this-&gt;FxSystemBP_c::FxSystemBP_c(); return this; } FxSystemBP_c* FxSystemBP_c::Destructor() { this-&gt;FxSystemBP_c::~FxSystemBP_c(); return this; } // 0x5C05F0 /** * Parsea una definición de sistema desde effects.fxp y crea sus primitivos emitter. * * Primero recoge nombres de textura y configuración; las texturas se cargan al final * una vez conocidas todas las primitivas del sistema. */ bool FxSystemBP_c::Load(Const char* filename, FILESTREAM file, int32 version) { // Como el original: cada campo se lee con ReadLine + sscanf sin comprobar el resultado ni el nombre // del campo, y los valores en coma flotante se escriben directamente en los miembros. char line\[256\], field\[128\], text\[128\]; int32 intValue; float floatValue; ReadLine(file, line, sizeof(line)); if (version &gt; 100) { ReadLine(file, line, sizeof(line)); RET_IGNORED(sscanf(line, "%s %s", field, text)); // FILENAME: } ReadLine(file, line, sizeof(line)); RET_IGNORED(sscanf(line, "%s %s", field, text)); // NAME: m_nNameKey = CKeyGen::GetUppercaseKey(text); ReadLine(file, line, sizeof(line)); RET_IGNORED(sscanf(line, "%s %f", field, &m_fLength)); if (version &lt; 106) { m_fLoopIntervalMin = 0.0f; m_fLoopLength = 0.0f; } else { ReadLine(file, line, sizeof(line)); RET_IGNORED(sscanf(line, "%s %f", field, &m_fLoopIntervalMin)); ReadLine(file, line, sizeof(line)); RET_IGNORED(sscanf(line, "%s %f", field, &m_fLoopLength)); } ReadLine(file, line, sizeof(line)); RET_IGNORED(sscanf(line, "%s %d", field, &intValue)); // PLAYMODE: m_nPlayMode = (uint8)intValue; ReadLine(file, line, sizeof(line)); RET_IGNORED(sscanf(line, "%s %f", field, &floatValue)); // CULLDIST: m_nCullDist = (uint16)(int64)(floatValue * 256.0f); // Como el original: _ftol2 if (version &gt; 103) { CVector vecCenter; float fRadius; ReadLine(file, line, sizeof(line)); RET_IGNORED(sscanf(line, "%s %f %f %f %f", field, &vecCenter.x, &vecCenter.y, &vecCenter.z, &fRadius)); m_BoundingSphere = nullptr; SetBoundingSphere(vecCenter, fRadius); } ReadLine(file, line, sizeof(line)); RET_IGNORED(sscanf(line, "%s %d", field, &intValue)); // NUM_PRIMS: m_nNumPrims = (uint8)intValue; // Como el original: el contador se trata con signo (movsx) y la reserva se hace en la arena FX m_Prims = static_cast&lt;FxPrimBP_c**&gt;(g_fxMan.GetMemPool().GetMem((int32)(int8)m_nNumPrims * (int32)sizeof(FxPrimBP_c*), 4)); FxName32_t aTextureNames\[8\]\[4\]; for (auto i = 0; i &lt; (int8)m_nNumPrims; i++) { ReadLine(file, line, sizeof(line)); RET_IGNORED(sscanf(line, "%s", field)); if (strcmp(field, "FX_PRIM_EMITTER_DATA:") != 0) continue; // Como el original: el emisor se reserva en la arena FX (0x4A1900) con el tamano del EXE (0x40; el // constructor no escribe los campos NOTSA de FxEmitterBP_c a partir de 0x40), no en el heap auto* const mem = g_fxMan.GetMemPool().GetMem(0x40, 4); auto* const emitter = mem ? new (mem) FxEmitterBP_c() : nullptr; m_Prims\[i\] = emitter; // Como el original: los indicadores son los de FxInfoManager_c (+0x3C/+0x3D), no m_bHasInfo*Data (+0x44/+0x45) auto& infos = emitter-&gt;m_FxInfoManager; infos.m_bHasFlatParticleEmitter = false; infos.m_bHasHeatHazeParticleEmitter = false; ReadLine(file, line, sizeof(line)); emitter-&gt;Load(file, version, aTextureNames\[i\]); infos.m_bHasFlatParticleEmitter = emitter-&gt;IsFxInfoPresent(FX_INFO_FLAT_DATA); infos.m_bHasHeatHazeParticleEmitter = emitter-&gt;IsFxInfoPresent(FX_INFO_HEATHAZE_DATA); } if (version &gt;= 108) { ReadLine(file, line, sizeof(line)); RET_IGNORED(sscanf(line, "%s %d", field, &intValue)); // OMITTEXTURES: (sin uso) } if (version &gt;= 109) | [source/game_sa/Fx/FxSystemBP.cpp:35](../../source/game_sa/Fx/FxSystemBP.cpp#L35) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1271"></a>

## Globales y helpers: source/game_sa/Fx/FxTools.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ReadLine(FILESTREAM file, char* buffer, int32 bufferSize) | [source/game_sa/Fx/FxTools.cpp:13](../../source/game_sa/Fx/FxTools.cpp#L13) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1272"></a>

## Globales y helpers: source/game_sa/Fx/FxTools.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ReadFieldImpl(FILESTREAM file, Type& refValue, const char* fieldName = nullptr) | [source/game_sa/Fx/FxTools.h:46](../../source/game_sa/Fx/FxTools.h#L46) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:60](../../source/game_sa/Fx/FxTools.h#L60) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;void&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:67](../../source/game_sa/Fx/FxTools.h#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;int8&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:74](../../source/game_sa/Fx/FxTools.h#L74) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;uint8&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:79](../../source/game_sa/Fx/FxTools.h#L79) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| ReadField&lt;bool&gt;(FILESTREAM file, const char* fieldName) | [source/game_sa/Fx/FxTools.h:84](../../source/game_sa/Fx/FxTools.h#L84) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1273"></a>

## Globales y helpers: source/game_sa/Game.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| ValidateVersion() | [source/game_sa/Game.cpp:114](../../source/game_sa/Game.cpp#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Histórica del catálogo: CGame::CameraDestroy 0x0072FD90 | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1274"></a>

## Globales y helpers: source/game_sa/GameLogic.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GetHeadingMinusHalfPi(const CPlaceable& placeable) | [source/game_sa/GameLogic.cpp:67](../../source/game_sa/GameLogic.cpp#L67) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LeaveVehicleForResurrection() | [source/game_sa/GameLogic.cpp:647](../../source/game_sa/GameLogic.cpp#L647) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| GetRestartSearchPosition() | [source/game_sa/GameLogic.cpp:669](../../source/game_sa/GameLogic.cpp#L669) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |
| UseTwoPlayerRestartPoint(CVector& restartPoint, float& restartAngle) | [source/game_sa/GameLogic.cpp:679](../../source/game_sa/GameLogic.cpp#L679) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo f2PlayerStartHeading: source/game_sa/GameLogic.h:50 (0x0096A840); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0); Posible dato fijo vec2PlayerStartLocation: source/game_sa/GameLogic.h:51 (0x0096A9AC) |
| FinishResurrection() | [source/game_sa/GameLogic.cpp:688](../../source/game_sa/GameLogic.cpp#L688) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo CountDownToCarsAtStart: source/game_sa/CarCtrl.h:39 (0x009690C0); Posible dato fijo PlayerInFocus: source/game_sa/World.h:56 (0x00B7CD74); Posible dato fijo Players: source/game_sa/World.h:54 (0x00B7CD98); Posible dato fijo TheCamera: source/game_sa/Camera.cpp:39 (0x00B6F028); Posible dato fijo m_CountDownToPedsAtStart: source/game_sa/Population.h:45 (0x00C0EC23); Posible dato fijo m_pPed: source/game_sa/Conversations/PedToPlayerConversations.h:26 (0x009691C0) |

<a id="grupo-1275"></a>

## Globales y helpers: source/game_sa/GangWars.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GangWaveRandomOffset(float base, float scale, float subtract) | [source/game_sa/GangWars.cpp:257](../../source/game_sa/GangWars.cpp#L257) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GangWaveInteger(float value, float scale, float base, int32 multiple) | [source/game_sa/GangWars.cpp:271](../../source/game_sa/GangWars.cpp#L271) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GangWaveCommand(CPed* ped, CTask* task) | [source/game_sa/GangWars.cpp:289](../../source/game_sa/GangWars.cpp#L289) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004AB420: CEventGroup::Add-; ruta efectiva sin resolver; Colaborador por dirección 0x004B0A00: CEventScriptCommand::Constructor; ruta efectiva sin resolver; Colaborador por dirección 0x004B0A50: CEventScriptCommand::Destructor; ruta efectiva sin resolver |
| GangWaveBlip(CPed* ped, eGangID gang, int32 scale) | [source/game_sa/GangWars.cpp:296](../../source/game_sa/GangWars.cpp#L296) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Colaborador por dirección 0x004442D0: CPedPool::GetRef; ruta efectiva sin resolver; Conversión de dirección fija: 0x008D1344, 0x008D1350, 0x008D135C, 0x00B74490 |
| GangDefenderOffset(int32 pedIndex, int32 pedTotal, int32 random1, int32 random2, float baseX, float baseY, float& outAngle, float& outX, float& outY) | [source/game_sa/GangWars.cpp:448](../../source/game_sa/GangWars.cpp#L448) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GangPickupOffset(int32 random, float base) | [source/game_sa/GangWars.cpp:483](../../source/game_sa/GangWars.cpp#L483) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| GangWarTimeDelta(bool negative) | [source/game_sa/GangWars.cpp:1059](../../source/game_sa/GangWars.cpp#L1059) | Mixta (C++ + ASM) · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 1 / 0 | ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1276"></a>

## Globales y helpers: source/game_sa/Garage.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GarageSphere15_4487D0() | [source/game_sa/Garage.cpp:111](../../source/game_sa/Garage.cpp#L111) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x004487D0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GarageSphere15_535300() | [source/game_sa/Garage.cpp:115](../../source/game_sa/Garage.cpp#L115) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x00535300; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GarageSphere15_54F1B0() | [source/game_sa/Garage.cpp:119](../../source/game_sa/Garage.cpp#L119) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x0054F1B0; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GarageSphere15_54F560() | [source/game_sa/Garage.cpp:123](../../source/game_sa/Garage.cpp#L123) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x0054F560; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |
| GarageSphere15_59C890() | [source/game_sa/Garage.cpp:127](../../source/game_sa/Garage.cpp#L127) | Ensamblador · Envoltura al EXE | Sin registro inequívoco | 2 / 1 | Trampolín ASM a 0x0059C890; cuerpo delegado; ASM: flujo por registros y direcciones requiere revisión; no se resuelve indirectamente |

<a id="grupo-1277"></a>

## Globales y helpers: source/game_sa/GenericGameStorage.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| make_hex_string(TInputIter first, TInputIter last, bool use_uppercase = true, bool insert_spaces = false) | [source/game_sa/GenericGameStorage.cpp:40](../../source/game_sa/GenericGameStorage.cpp#L40) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LOG_HEX_SPAN(uint8* start, size_t size) | [source/game_sa/GenericGameStorage.cpp:56](../../source/game_sa/GenericGameStorage.cpp#L56) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| LOG_HEX_SPAN(uint8* start, size_t size) | [source/game_sa/GenericGameStorage.cpp:65](../../source/game_sa/GenericGameStorage.cpp#L65) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| SaveVersionName(uint32 version) | [source/game_sa/GenericGameStorage.cpp:237](../../source/game_sa/GenericGameStorage.cpp#L237) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1278"></a>

## Globales y helpers: source/game_sa/GenericGameStorage.h

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| * GetNameOfSavedGame(int32 slot) | [source/game_sa/GenericGameStorage.h:114](../../source/game_sa/GenericGameStorage.h#L114) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_SlotFileName: source/game_sa/GenericGameStorage.h:73 (0x00C16368) |
| GetSavedGameState(int32 slot) | [source/game_sa/GenericGameStorage.h:120](../../source/game_sa/GenericGameStorage.h#L120) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Posible dato fijo ms_Slots: source/game_sa/GenericGameStorage.h:74 (0x00C16EBC) |

<a id="grupo-1279"></a>

## Globales y helpers: source/game_sa/Glass.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| Multiply3x3_59C790(const CMatrix& m, const CVector& v) | [source/game_sa/Glass.cpp:52](../../source/game_sa/Glass.cpp#L52) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| MultiplyPoint_59C890(const CMatrix& m, const CVector& v) | [source/game_sa/Glass.cpp:61](../../source/game_sa/Glass.cpp#L61) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindMinMaxZOfVertices(const CVector (&v)\[4\], float& minZ, float& maxZ) | [source/game_sa/Glass.cpp:70](../../source/game_sa/Glass.cpp#L70) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| FindVertexFurthestFromV0(const CVector (&v)\[4\]) | [source/game_sa/Glass.cpp:81](../../source/game_sa/Glass.cpp#L81) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
| IsGlassObjectWithCol(CEntity* entity) | [source/game_sa/Glass.cpp:258](../../source/game_sa/Glass.cpp#L258) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |

<a id="grupo-1280"></a>

## Globales y helpers: source/game_sa/GrassRenderer.cpp

| Función / firma | Ubicación | Implementación | Validación | ASM / naked | Dependencias del EXE |
| --- | --- | --- | --- | --- | --- |
| GrassDistance(const CVector& a, const CVector& b) | [source/game_sa/GrassRenderer.cpp:69](../../source/game_sa/GrassRenderer.cpp#L69) | C++ · Cuerpo fuente; delegación indirecta no descartada | Sin registro inequívoco | 0 / 0 | Sin indicio directo en patrones examinados; dependencias transitivas sin determinar |
