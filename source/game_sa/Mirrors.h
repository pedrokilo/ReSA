/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
#include "DatosExe/DataBssG1.h"
#include "DatosExe/DataBssE4.h"
#include "DatosExe/DataBssE3.h"
#include "DatosExe/DataBssE2.h"
#include "DatosExe/DataBssE1.h"

#include "Matrix.h"
#include "Vector.h"

enum eMirrorType : int32 { // int32 en el original (TypeOfMirror se lee y escribe como palabra completa)
    MIRROR_TYPE_NONE = 0,
    MIRROR_TYPE_WALL = 1,
    MIRROR_TYPE_FLOOR = 2,
    MIRROR_TYPE_SPHERE_MAP = 3
};

class CMirrors {
public:
    static inline auto& pBuffer = DatosExe::Bss::bC7C71C;
    static inline auto& pZBuffer = DatosExe::Bss::bC7C720;
    static inline auto& bRenderingReflection = DatosExe::Bss::bC7C728;
    static inline auto& d3dRestored = DatosExe::Bss::bC7C729;
    static inline auto& TypeOfMirror = StaticRef<eMirrorType>(0xC7C724);
    static inline auto& MirrorFlags = DatosExe::Bss::bC7C618; // int32 con signo en el original (movsx de los flags de la zona)
    static inline auto& MirrorV = DatosExe::Bss::bC7C61C;
    static inline auto& MirrorNormal = StaticRef<CVector>(0xC803D8);

public:
    static void InjectHooks();

    static void Init();
    static void ShutDown();

    static void BeforeConstructRenderList();
    static void BeforeMainRender();

    static void BuildCamMatrix(CMatrix& camMatrix, CVector source, CVector dest);
    static void BuildCameraMatrixForScreens(CMatrix& mat);

    static void CreateBuffer();

    static void RenderMirrorBuffer();
    static void RenderReflBuffer(bool);
    static void RenderReflections();

    static bool ShouldRenderPeds();
};
