#pragma once

class CStencilShadowObject;

class CStencilShadows {
public:
    static inline auto& m_StencilShadowObjects = StaticRef<std::array<CStencilShadowObject, 64>>(0xC6A198);

    static inline auto& pFirstAvailableStencilShadowObject = StaticRef<CStencilShadowObject*>(0xC6A168);
    static inline auto& pFirstActiveStencilShadowObject    = StaticRef<CStencilShadowObject*>(0xC6A16C);

    static void InjectHooks();

    static bool Init();
    static void Shutdown();
    static bool Process(CVector& cameraPos);

    // NOTSA/FIX_BUGS: It was named GraphicsLowQuality for some reason
    static bool GraphicsHighQuality();

    static void UpdateHierarchy(CStencilShadowObject*& firstAvailable, CStencilShadowObject*& firstActive, CStencilShadowObject* newOne);
    static bool RegisterStencilShadows(CVector& cameraPos, bool doNotCreateNew);

    static bool RenderStencilShadows();
    static bool RenderForVehicle(CStencilShadowObject*);
    static bool RenderForObject(CStencilShadowObject*);
    static bool Render(const CRGBA& color);
    static bool RenderBuffer(const CVector& pos);
    static bool SunSetPositionFromEntity(const CEntity* entity);

    static void sub_710CC0(int32 indices, int32 vertices);
};
