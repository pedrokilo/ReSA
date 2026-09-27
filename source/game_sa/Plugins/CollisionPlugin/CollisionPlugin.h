#pragma once

#include <rwplcore.h>

class CClumpModelInfo;

/**
 * collision plugin unique rwID
 */
#define rwID_COLLISIONPLUGIN  MAKECHUNKID(rwVENDORID_DEVELOPER, 0xFA)

/*
* Collision plugin static offset
*/
extern RwInt32& gCollisionPluginOffset;

/**
 * Adaptador RenderWare para cargar un modelo de colisión embebido en un RpClump.
 *
 * ms_currentModel actúa como contexto temporal de destino durante el callback de
 * streaming. El lector real sigue delegado al ejecutable original.
 */
class CCollisionPlugin {
public:
    static inline auto& ms_currentModel = StaticRef<CClumpModelInfo*>(0x9689E0);

public:
    static void InjectHooks();

    static bool PluginAttach();
    static void SetModelInfo(CClumpModelInfo* modelInfo);
};
