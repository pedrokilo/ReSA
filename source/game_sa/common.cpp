/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: Common
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"
#include "X87Math.h"

#include "Garages.h"
#include "common.h"
#include "GxtChar.h"
#include "UIRenderer.h"
#include "CarCtrl.h"
#include "UserDisplay.h"
#include "PostEffects.h"
#include "SpecialFX.h"

#include "Hud.h"
#include "app.h"

void InjectCommonHooks() {
    RH_ScopedNamespaceName("Common");
    RH_ScopedCategory("Common");
    RH_ScopedGlobalInstall(TransformPoint, 0x54ECE0);
    RH_ScopedGlobalInstall(TransformPoints, 0x54EEF0);

    RH_ScopedGlobalOverloadedInstall(MakeUpperCase, "", 0x7186E0, char* (*)(char*, const char*));
    RH_ScopedGlobalOverloadedInstall(MakeUpperCase, "in", 0x718710, char* (*)(char*));
    RH_ScopedGlobalInstall(AsciiToGxtChar, 0x718600);
    RH_ScopedGlobalInstall(WriteRaster, 0x005A4150);
    RH_ScopedGlobalOverloadedInstall(CalcScreenCoors, "VVff", 0x71DA00, bool(*)(const CVector&, CVector&, float&, float&), { .reversed = true });
    RH_ScopedGlobalOverloadedInstall(CalcScreenCoors, "VV", 0x71DAB0, bool(*)(const CVector&, CVector&), { .reversed = true });
    RH_ScopedGlobalInstall(LittleTest, 0x541330);
    RH_ScopedGlobalInstall(DoesInfiniteLineTouchScreen, 0x71DB80);
    RH_ScopedGlobalInstall(IsPointInsideLine, 0x71E050);
}

// 0x54ECE0
void TransformPoint(RwV3d& point, const CSimpleTransform& placement, const RwV3d& vecPos) {
    // Como el original: x se escribe antes de releer in.x/in.y para y (la salida puede ser la entrada);
    // coseno y seno quedan sin redondear en la x87 dentro del ayudante.
    CommonTransformPointCpp(&point.x, &placement.m_vPosn.x, &vecPos.x);
}

// 0x54EEA0
void TransformVectors(RwV3d* vecsOut, int32 numVectors, const CMatrix& matrix, const RwV3d* vecsIn) {
    matrix.CopyToRwMatrix(CGame::m_pWorkingMatrix1);
    RwMatrixUpdate(CGame::m_pWorkingMatrix1);

    for (auto i = 0; i < numVectors; i++) {
        RwV3dTransformVector(vecsOut++, vecsIn++, CGame::m_pWorkingMatrix1);
    }
}

// 0x54EE30
void TransformVectors(RwV3d* vecsOut, int32 numVectors, const CSimpleTransform& transform, const RwV3d* vecsIn) {
    // Como el original (0x54EE30): solo rota (sin sumar la posicion) y copia la z; coseno y seno se redondean a float antes de usarlos.
    const float cosHeading = TrigSimpleCosCpp(transform.m_fHeading);
    const float sinHeading = TrigSimpleSinCpp(transform.m_fHeading);

    // El bucle va en X87Math (x87 como el EXE): x se escribe antes de releer in.x para y, z se copia como dword y el contador
    // decrece hasta cero (dec/jne; el dominio válido es numVectors>=0). Con SSE los NaN de x/y se propagarían distinto.
    CommonTransformVectorsCpp(&vecsOut->x, numVectors, cosHeading, sinHeading, &vecsIn->x);
}

// 0x54EEF0
void TransformPoints(RwV3d* pointOut, int count, const CMatrix& transformMatrix, RwV3d* pointIn) {
    // Como el original 0x59B8B0: copia doce dwords, sin tocar flags ni padding.
    const auto* source = reinterpret_cast<const uint8*>(&transformMatrix);
    auto* destination = reinterpret_cast<uint8*>(CGame::m_pWorkingMatrix1);
    for (unsigned row : {0u, 0x10u, 0x20u, 0x30u}) {
        for (unsigned component = 0; component < 3; ++component) {
            uint32 value;
            std::memcpy(&value, source + row + component * 4, 4);
            std::memcpy(destination + row + component * 4, &value, 4);
        }
    }
    RwMatrixUpdate(CGame::m_pWorkingMatrix1); // 0x59B90D: dentro del colaborador original
    RwMatrixUpdate(CGame::m_pWorkingMatrix1); // 0x54EF07: llamada adicional del entry

    // Como el original: el contador de máquina decrece hasta cero; el dominio válido es count>=0.
    for (auto remaining = std::bit_cast<uint32>(count); remaining != 0; --remaining) {
        RwV3dTransformPoint(pointOut, pointIn, CGame::m_pWorkingMatrix1);
        ++pointOut;
        ++pointIn;
    }
}

// 0x7186E0
// Como el original: sólo 'a'..'z' (comparación sin signo, sin locale) y devuelve el inicio de `dest`.
char* MakeUpperCase(char* dest, const char* src) {
    char* out = dest;
    for (; *src; src++, out++) {
        const auto c = (uint8)*src;
        *out = (char)(c >= 'a' && c <= 'z' ? c - 0x20 : c);
    }
    *out = '\0';
    return dest;
}

// 0x718710
// in-place version
// Como el original: sólo 'a'..'z' y sólo escribe los caracteres que cambia (el EXE no fija el valor devuelto).
char* MakeUpperCase(char* dest) {
    for (char* p = dest; *p; p++) {
        const auto c = (uint8)*p;
        if (c >= 'a' && c <= 'z') {
            *p = (char)(c - 0x20);
        }
    }
    return dest;
}

// NOTSA
bool EndsWith(const char* str, const char* with, bool caseSensitive) {
    const auto strsz = strlen(str), withsz = strlen(with);
    assert(strsz >= withsz);
    return (caseSensitive ? strncmp : _strnicmp)(str + strsz - withsz, with, withsz) == 0;
}

// 0x70F9B0
bool GraphicsHighQuality() {
    if (g_fx.GetFxQuality() < FX_QUALITY_MEDIUM)
        return false;
    if (RwRasterGetDepth(RwCameraGetRaster(Scene.m_pRwCamera)) < 32)
        return false;
    return true;
}

// 0x5A4150
void WriteRaster(RwRaster* raster, const char* filename) {
    assert(raster);
    assert(filename && filename[0]);

    RwImage* img = RwImageCreate(RwRasterGetWidth(raster), RwRasterGetHeight(raster), RwRasterGetDepth(raster));
    RwImageAllocatePixels(img);
    RwImageSetFromRaster(img, raster);
    RtPNGImageWrite(img, filename);
    RwImageDestroy(img);
}

// Como el original: MultiplyMatrixWithVector (0x59C890) con el orden de sumas del EXE
static CVector MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x + m.GetPosition().x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetPosition().y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetPosition().z
    };
}

// 0x71DA00
// Como el original: repite la proyección (no llama a la versión corta); escala = (ancho · 1/z) / FOV · 70.
bool CalcScreenCoors(const CVector& in, CVector& out, float& scaleX, float& scaleY) {
    out = MatrixTimesPoint59C890(TheCamera.GetViewMatrix(), in);
    if (!(out.z > 1.0f)) {
        return false;
    }

    const auto recpZ = 1.0f / out.z;
    out.x = SCREEN_WIDTH * recpZ * out.x;
    out.y = SCREEN_HEIGHT * recpZ * out.y;
    scaleX = SCREEN_WIDTH * recpZ;
    scaleY = SCREEN_HEIGHT * recpZ;
    scaleX = scaleX / CDraw::ms_fFOV * 70.0f;
    scaleY = scaleY / CDraw::ms_fFOV * 70.0f;

    return true;
}

/*!
* @0x71DAB0
* @brief Calculate a 3D position on the screen
* @param in  in  The 3D to get the screen position of
* @param out out The 2D screen position (Also includes the depth in the `z` component)
* @returns False if the depth was <= 1 (in which case the `x, y` positions are not not calculated, but `z` is)
*/
bool CalcScreenCoors(const CVector& in, CVector& out) {
    out = MatrixTimesPoint59C890(TheCamera.GetViewMatrix(), in); // Como el original: orden de sumas de 0x59C890
    if (!(out.z > 1.0f)) { // Como el original: z NaN también devuelve false
        return false;
    }

    const auto recpZ = 1.0f / out.z;
    out.x = SCREEN_WIDTH * recpZ * out.x;
    out.y = SCREEN_HEIGHT * recpZ * out.y;

    return true;
}

// 0x541330
void LittleTest() {
    ++g_nNumIm3dDrawCalls;
}

// @notsa
// Similar to `CCollision::DistAlongLine2D`, but instead of calcualting the dot product
// on the line itself, it's actually calculated on a perpendicular line on the right
float DistAlongPerpRightLine2D(CVector2D origin, CVector2D dir, CVector2D pt) {
    // return (pt.x - origin.x) * dir.y - (pt.y - origin.y) * dir.x
    return (pt - origin).Dot(dir.GetPerpRight());
}

// @unk
bool DoesInfiniteLineCrossFiniteLine(
    CVector2D finiteA,
    CVector2D finiteB,
    CVector2D origin,
    CVector2D dir
) {
    //return (((finiteA.x - origin.x) * dir.y) - ((finiteA.y - origin.y) * dir.x))
    //    * (((finiteB.x - origin.x) * dir.y) - ((finiteB.y - origin.y) * dir.x)) < 0.0;
    return DistAlongPerpRightLine2D(origin, dir, finiteA) * DistAlongPerpRightLine2D(origin, dir, finiteB) < 0.f;
}

// used only in COccluder::ProcessLineSegment
// 0x71DB80
bool DoesInfiniteLineTouchScreen(
    //float oX, float oY, float dX, float dY
    CVector2D origin,
    CVector2D dir
) {
    // Point is on screen
    // Como el original: desigualdades estrictas (un punto en el borde pasa a las pruebas de las líneas)
    if (origin.x > 0.f && origin.y > 0.f && SCREEN_WIDTH > origin.x && SCREEN_HEIGHT > origin.y) {
        return true;
    }

    // Or line touches any of the screen's bezzles (?)
    return DoesInfiniteLineCrossFiniteLine({0.f, 0.f},           {SCREEN_WIDTH, 0.f},           origin, dir)
        || DoesInfiniteLineCrossFiniteLine({0.f, 0.f},           {0.f, SCREEN_HEIGHT},          origin, dir)
        || DoesInfiniteLineCrossFiniteLine({SCREEN_WIDTH, 0.f},  {SCREEN_WIDTH, SCREEN_HEIGHT}, origin, dir)
        || DoesInfiniteLineCrossFiniteLine({0.f, SCREEN_HEIGHT}, {SCREEN_WIDTH, SCREEN_HEIGHT}, origin, dir);
}

// Used only in COcclusion, COccluder, CActiveOccluder
// 0x71E050
bool IsPointInsideLine(
    CVector2D origin,
    CVector2D dir,
    CVector2D pt,
    float     radius
) {
    return !(DistAlongPerpRightLine2D(origin, dir, pt) < radius); // Como el original: NaN devuelve true
}

// Convert UTF-8 string to Windows Unicode. Free pointer using delete[]
// NOTSA
std::wstring UTF8ToUnicode(const std::string &str)
{
    std::wstring out;

    int32 size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.data(), str.length(), nullptr, 0);
    if (size) {
        std::vector<wchar_t> temp;
        temp.resize(size, L'\0');

        if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.data(), str.length(), temp.data(), size)) {
            out.resize(size);
            std::copy(temp.begin(), temp.end(), out.begin());
        }
    }

    return out;
}

// Convert Windows Unicode to UTF-8. Free pointer using delete[]
// NOTSA
std::string UnicodeToUTF8(const std::wstring& str) {
    std::string out;

    int32 size = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, str.data(), str.length(), nullptr, 0, nullptr, nullptr);
    if (size) {
        std::vector<char> temp;
        temp.resize(size, 0);

        if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, str.data(), str.length(), temp.data(), size, nullptr, nullptr)) {
            out.resize(size);
            std::copy(temp.begin(), temp.end(), out.begin());
        }
    }

    return out;
}

/*!
* @notsa
* @return The anim first found from `ids`
*/
CAnimBlendAssociation* RpAnimBlendClumpGetAssociation(RpClump* clump, std::initializer_list<AnimationId> ids) {
    for (const auto id : ids) {
        if (const auto anim = RpAnimBlendClumpGetAssociation(clump, (int32)id)) {
            return anim;
        }
    }
    return nullptr;
}
