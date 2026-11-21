/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
#include "DatosExe/DataBssE1.h"

#include "RenderWare.h"
#include <Enums/eAreaCodes.h>

class CColModel;
class CEntity;

class CGame {
public:
    //! Current number of area
    static inline auto& aDatFile = StaticRef<char[32]>(0xB728EC);
    static inline auto& currLevel = StaticRef<int32>(0xB7290C);
    static inline auto& bMissionPackGame = StaticRef<uint8>(0xB72910);
    static inline auto& currArea = StaticRef<eAreaCodesS32>(0xB72914);
    static inline auto& m_pWorkingMatrix1 = StaticRef<RwMatrix*>(0xB72920);
    static inline auto& m_pWorkingMatrix2 = StaticRef<RwMatrix*>(0xB72924);


public:
    static void InjectHooks();

    static bool CanSeeOutSideFromCurrArea();
    static bool CanSeeWaterFromCurrArea();

    static void GenerateTempPedAtStartOfNetworkGame();

    static bool Init1(const char* datFile);
    static bool Init2(const char* datFile);
    static bool Init3(const char* datFile);
    static void Initialise(char const *datFile);
    static bool InitialiseCoreDataAfterRW();
    static bool InitialiseEssentialsAfterRW();
    static void InitialiseOnceBeforeRW();
    static bool InitialiseRenderWare();
    static void InitialiseWhenRestarting();
    static void D3DRestoreCallback(); // 0x5BA120 (lote S2A, sin validar): callback de restauración del dispositivo D3D9 que instala Init2
    static void Process();
    static void ReInitGameObjectVariables();
    static void ReloadIPLs();

    static void ShutDownForRestart();
    static bool Shutdown();
    static void ShutdownRenderWare();
    static void DrasticTidyUpMemory(bool a1);
    static void FinalShutdown();
    static void ActivateMenuOnSetFocus(); // 0x53BC60 (lote 182A, sin validar)
    static void TidyUpMemory(bool a1, bool clearD3Dmem);
    static eAreaCodes GetCurrentAreaCode() { return currArea; }
    static eAreaCodes GetPlayerOrCurrentAreaCode();
};

static inline auto& gameTxdSlot = StaticRef<int32>(0xB728E8);
static inline auto& gbLARiots = DatosExe::Bss::bB72958;
static inline auto& gbLARiots_NoPoliceCars = DatosExe::Bss::bB72959;
static inline auto& col1 = StaticRef<std::array<CColModel, 2>>(0xC17824);

bool MoveMem(void **pMem);
bool MoveColModelMemory(CColModel &colModel, bool a2);
RpGeometry *MoveGeometryMemory(RpGeometry *geometry);
bool TidyUpModelInfo2(CEntity *entity, bool a2);
void ValidateVersion();
void D3DDeviceRestoreCallback();
bool DoHaspChecks();
