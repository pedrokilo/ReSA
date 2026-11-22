#pragma once

/*
 * Lote DE05-G4 (docs/DatosExe): variables de .bss del juego como definiciones propias (a cero, como el .bss
 * del EXE). Estado vivo: las lee y escribe solo C++. Generado por contrib/datos_exe_de05.py; definidas en DataBssG4.cpp.
 */

class CAnimBlendAssocGroup;
class CAnimBlendClumpData;
class CCamera;
class CClumpModelInfo;
struct CColAccelColBound;
struct CColAccelColEntry;
struct CColAccelIPLEntry;
class CDirectory;
class CEntryExit;
class CMatrix;
class CNodeRoutePool;
class CObject;
class CPatrolRoutePool;
class CPed;
class CPedAttractorPool;
class CPedStat;
class CPtrNodeSingleLinkPool;
class CStencilShadowObject;
class CTaskAllocatorPool;
class CVector;
struct RpLight;
struct RwTexture;
struct RxPipeline;
#include "GxtChar.h"


namespace DatosExe::Bss {
extern CDirectory* b8E48D0; // 0x008E48D0
extern CClumpModelInfo* b9689E0; // 0x009689E0
extern CObject* b96A7BC; // 0x0096A7BC
extern CEntryExit* b96A7C0; // 0x0096A7C0
extern CEntryExit* b96A7D4; // 0x0096A7D4
extern CCamera* b97FAF8; // 0x0097FAF8
extern RwTexture* bB4E47C; // 0x00B4E47C
extern CAnimBlendAssocGroup* bB4EA34; // 0x00B4EA34
extern CPtrNodeSingleLinkPool* bB74484; // 0x00B74484
extern CPatrolRoutePool* bB744B4; // 0x00B744B4
extern CNodeRoutePool* bB744B8; // 0x00B744B8
extern CTaskAllocatorPool* bB744BC; // 0x00B744BC
extern CPedAttractorPool* bB744C4; // 0x00B744C4
extern const GxtChar* bBAB1D4; // 0x00BAB1D4
extern CColAccelColBound* bBC4090; // 0x00BC4090
extern int32* bBC4098; // 0x00BC4098
extern CColAccelColEntry* bBC40A4; // 0x00BC40A4
extern CColAccelIPLEntry* bBC40AC; // 0x00BC40AC
extern int8* bBD00F8; // 0x00BD00F8
extern uint8** bC02B78; // 0x00C02B78
extern RxPipeline* bC02C1C; // 0x00C02C1C
extern RwTexture* bC02DC0; // 0x00C02DC0
extern RwTexture* bC09174; // 0x00C09174
extern CPedStat* bC0BBEC; // 0x00C0BBEC
extern uint8* bC16EE8; // 0x00C16EE8
extern CStencilShadowObject* bC6A168; // 0x00C6A168
extern CStencilShadowObject* bC6A16C; // 0x00C6A16C
extern CVector* bC6A174; // 0x00C6A174
extern CVector* bC6A178; // 0x00C6A178
extern int32* bC87B14; // 0x00C87B14
extern int32* bC87B18; // 0x00C87B18
extern RpLight* bC886E8; // 0x00C886E8
extern RpLight* bC886EC; // 0x00C886EC



} // namespace DatosExe::Bss
