#pragma once

/*
 * Lote DE05-E4 (docs/DatosExe): variables de .bss del juego como definiciones propias (a cero, como el .bss
 * del EXE). Estado vivo: las lee y escribe solo C++. Generado por contrib/datos_exe_de05.py; definidas en DataBssE4.cpp.
 */

class CAESound;
class CAccidentManager;
class CAcquaintance;
class CDirectory;
class CEntity;
class CEventGlobalGroup;
struct CFontChar;
class CGarage;
class CMultiBuilding;
class CPed;
class CPedAttractorManager;
class CPedClothesDesc;
class CPlantColEntEntry;
class CPlantLocTri;
class CPlayerRelationshipRecorder;
class CPolyBunch;
class CRGBA;
class CReference;
class CStreamingInfo;
struct CStuntJump;
class CVehicle;
class CZone;
class CZoneInfo;
struct RpClump;
struct RpMaterial;
struct RwCamera;
struct RwFrame;
struct RwRaster;
struct RwTexDictionary;
struct RwTexture;
struct RwV3d;
struct RxPipeline;
#include "GxtChar.h"


namespace DatosExe::Bss {
extern CStreamingInfo* b8E4C54; // 0x008E4C54
extern CStreamingInfo* b8E4C58; // 0x008E4C58
extern CStreamingInfo* b8E4C5C; // 0x008E4C5C
extern CStreamingInfo* b8E4C60; // 0x008E4C60
extern CEntity* b9689D8; // 0x009689D8
extern CPed* b9691C0; // 0x009691C0
extern CPed* b9691CC; // 0x009691CC
extern CVehicle* b969A40; // 0x00969A40
extern const GxtChar* b96A6D0; // 0x0096A6D0
extern CVehicle* b96A898; // 0x0096A898
extern CVehicle* b96AB28; // 0x0096AB28
extern CZone* b96AB88; // 0x0096AB88
extern CZoneInfo* b96AB8C; // 0x0096AB8C
extern CGarage* b96BFD8; // 0x0096BFD8
extern CVehicle* b97861C; // 0x0097861C
extern void* b97FAF4; // 0x0097FAF4
extern void* b97FB00; // 0x0097FB00
extern void* b97FB04; // 0x0097FB04
extern void* b97FB08; // 0x0097FB08
extern void* b97FB0C; // 0x0097FB0C
extern CMultiBuilding* bA97294; // 0x00A97294
extern CStuntJump* bA9A88C; // 0x00A9A88C
extern RxPipeline* bA9AD78; // 0x00A9AD78
extern CEventGlobalGroup* bA9AF6C; // 0x00A9AF6C
extern uint8* bA9B76C; // 0x00A9B76C
extern RwTexture* bB4E3EC; // 0x00B4E3EC
extern RwTexDictionary* bB4E688; // 0x00B4E688
extern RwTexture* bB4E68C; // 0x00B4E68C
extern RwTexture* bB4E690; // 0x00B4E690
extern RwFrame* bB4E6B8; // 0x00B4E6B8
extern CDirectory* bB5F848; // 0x00B5F848
extern CAESound* bB612FC; // 0x00B612FC
extern CPed* bB61400; // 0x00B61400
extern CPed* bB6140C; // 0x00B6140C
extern CPed* bB61410; // 0x00B61410
extern CAESound* bB61D00; // 0x00B61D00
extern CPed* bB6B98C; // 0x00B6B98C
extern CPed* bB6B990; // 0x00B6B990
extern CEntity* bB6BC6C; // 0x00B6BC6C
extern CEntity* bB6BC74; // 0x00B6BC74
extern CVehicle* bB745D4; // 0x00B745D4
extern CAccidentManager* bB9B7D0; // 0x00B9B7D0
extern CReference* bBA1768; // 0x00BA1768
extern const GxtChar* bBAA444; // 0x00BAA444
extern const GxtChar* bBAA454; // 0x00BAA454
extern const GxtChar* bBAA458; // 0x00BAA458
extern const GxtChar* bBAB1D0; // 0x00BAB1D0
extern const GxtChar* bBAB1D8; // 0x00BAB1D8
extern const CPedClothesDesc* bBC1288; // 0x00BC1288
extern CRGBA* bBD0160; // 0x00BD0160
extern RxPipeline* bC02C68; // 0x00C02C68
extern RxPipeline* bC02D24; // 0x00C02D24
extern CPlantLocTri* bC03984; // 0x00C03984
extern CPlantColEntEntry* bC03998; // 0x00C03998
extern CPlantColEntEntry* bC0399C; // 0x00C0399C
extern CPedAttractorManager* bC09840; // 0x00C09840
extern CAcquaintance* bC0BBE8; // 0x00C0BBE8
extern CZone* bC0BC64; // 0x00C0BC64
extern CZoneInfo* bC0BC68; // 0x00C0BC68
extern CPlayerRelationshipRecorder* bC17084; // 0x00C17084
extern RpClump* bC1CB58; // 0x00C1CB58
extern RwRaster* bC228A8; // 0x00C228A8
extern RwTexture* bC228AC; // 0x00C228AC
extern RwRaster* bC228B0; // 0x00C228B0
extern RwTexture* bC228B4; // 0x00C228B4
extern RwRaster* bC228B8; // 0x00C228B8
extern RwTexture* bC228BC; // 0x00C228BC
extern RwTexture* bC3EF5C; // 0x00C3EF5C
extern RwUInt8* bC3EF78; // 0x00C3EF78
extern RpMaterial* bC3EF7C; // 0x00C3EF7C
extern RwTexture* bC3EF84; // 0x00C3EF84
extern RwUInt8* bC3EF88; // 0x00C3EF88
extern RpClump* bC3F040; // 0x00C3F040
extern RwRaster* bC402B0; // 0x00C402B0
extern CPolyBunch* bC403D8; // 0x00C403D8
extern RwTexture* bC403E0; // 0x00C403E0
extern RwTexture* bC403E4; // 0x00C403E4
extern RwTexture* bC403E8; // 0x00C403E8
extern RwTexture* bC403EC; // 0x00C403EC
extern RwTexture* bC403F0; // 0x00C403F0
extern RwTexture* bC403F4; // 0x00C403F4
extern RwTexture* bC403F8; // 0x00C403F8
extern RwTexture* bC403FC; // 0x00C403FC
extern RwTexture* bC40400; // 0x00C40400
extern RwTexture* bC40404; // 0x00C40404
extern RwTexture* bC40408; // 0x00C40408
extern RwTexture* bC4040C; // 0x00C4040C
extern RwTexture* bC6AA74; // 0x00C6AA74
extern RwTexture* bC6AA78; // 0x00C6AA78
extern RwTexture* bC6AA7C; // 0x00C6AA7C
extern CFontChar* bC716A8; // 0x00C716A8
extern RwTexture* bC7C718; // 0x00C7C718
extern RwRaster* bC7C71C; // 0x00C7C71C
extern RwRaster* bC7C720; // 0x00C7C720
extern uint8* bC81414; // 0x00C81414
extern int32* bC81418; // 0x00C81418
extern int32* bC8141C; // 0x00C8141C
extern int32* bC81420; // 0x00C81420
extern RwTexDictionary* bC88010; // 0x00C88010
extern RwCamera* bC8804C; // 0x00C8804C
extern RwV3d* bC88050; // 0x00C88050
extern RwTexture* bC8A818; // 0x00C8A818
extern CEntity* bC8A894; // 0x00C8A894
extern char** bC920D0; // 0x00C920D0

} // namespace DatosExe::Bss
