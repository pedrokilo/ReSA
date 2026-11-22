#pragma once
#include "DatosExe/DataBssE3.h"
#include "DatosExe/DataInicializadaA.h"

#include "PPTriPlantBuffer.h"

class CVector;

class CGrassRenderer {
public:
    static inline auto& m_windBending = DatosExe::Bss::bC02DB8;
    static inline auto& m_closeDist = DatosExe::Bss::bC02DBC;
    static inline auto& m_vecCameraPos = StaticRef<CVector>(0xC02DDC);
    static inline auto& m_farDist = DatosExe::Data::v8D132C; // 10.0f

public:
    static void InjectHooks();

    CGrassRenderer() = default;
    ~CGrassRenderer() = default;

    static bool Initialise();
    static void Shutdown();

    static void AddTriPlant(PPTriPlant* srcPlant, uint32 plantModelSet);
    static void DrawTriPlants(PPTriPlant* triPlants, int32 numTriPlants, RpAtomic** plantModelsTab);
    static void FlushTriPlantBuffer();

    static RpAtomic** GetPlantModelsTab(uint32 type);
    static bool SetPlantModelsTab(uint32 type, RpAtomic** atomics);

    static void SetCloseFarAlphaDist(float fCloseDist, float fFarDist);
    static void SetCurrentScanCode(uint16 scanCode);
    static void SetGlobalCameraPos(const CVector& cameraPosn);
    static void SetGlobalWindBending(float bending);

    static CVector& GenPointInTriangle(CVector& outPosn, const CVector& v1, const CVector& v2, const CVector& v3, float randA, float randB);
};

extern uint16& g_GrassCurrentScanCode;
extern CPPTriPlantBuffer& gTriPlantBuf;
