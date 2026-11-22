#pragma once
#include "DatosExe/DataBssE2.h"

#include "RenderWare.h"

class CPlayerSkin {
public:
    static inline auto& m_txdSlot = DatosExe::Bss::bC3F03C;

public:
    static void InjectHooks();

    static void       Initialise();
    static void       Shutdown();
    static void       RenderFrontendSkinEdit();
    static RwTexture* GetSkinTexture(const char* name);
};
