/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CWaterLevel
 * Reconstrucción: COMPLETA — 33/33 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 33/33 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "WaterLevel.h"
#include "PostEffects.h"
#include "X87Math.h"
#include <sstream>

#define TRIANGLE_ARGS_OUT X1, Y1, P1, X2, Y2, P2, X3, Y3, P3

void CWaterLevel::InjectHooks() {
    RH_ScopedClass(CWaterLevel);
    RH_ScopedCategoryGlobal();
    RH_ScopedInstall(CalculateWavesOnlyForCoordinate2Native, 0x6E7210);

    RH_ScopedGlobalInstall(WaterLevelInitialise, 0x6EAE80);
    RH_ScopedGlobalInstall(Shutdown, 0x6E59E0);
    RH_ScopedGlobalInstall(RenderWaterTriangle, 0x6EE240);
    RH_ScopedGlobalInstall(RenderFlatWaterTriangle_OneLayer, 0x6E8ED0);
    RH_ScopedGlobalInstall(RenderFlatWaterTriangle, 0x6EE080);
    RH_ScopedGlobalInstall(SplitWaterTriangleAlongXLine, 0x6ECF00);
    RH_ScopedGlobalInstall(SplitWaterTriangleAlongYLine, 0x6EE5A0);

    RH_ScopedGlobalInstall(RenderWaterRectangle, 0x6EC5D0);
    RH_ScopedGlobalInstall(RenderFlatWaterRectangle_OneLayer, 0x6E9940);
    RH_ScopedGlobalInstall(RenderFlatWaterRectangle, 0x6EBEC0);

    RH_ScopedGlobalInstall(SplitWaterRectangleAlongXLine, 0x6E73A0);
    RH_ScopedGlobalInstall(SplitWaterRectangleAlongYLine, 0x6ED6D0);

    RH_ScopedGlobalInstall(PreRenderWater, 0x6EB710);
    RH_ScopedGlobalInstall(MarkQuadsAndPolysToBeRendered, 0x6E5810);
    RH_ScopedGlobalInstall(ScanThroughBlocks, 0x6E6D10);
    RH_ScopedGlobalInstall(BlockHit, 0x6E6CA0);

    // This one doesn't seem to work properly for whatever reason
    // It works for some quads, not for others... But then it works for that one too if only it's loaded from the file (eg.: you delete all others)
    // no clue what is the issue
    // one quad that doesn't load can be seen from -1610, 168
    // it's at water.dat:252
    RH_ScopedGlobalInstall(AddWaterLevelQuad, 0x6E7EF0);
    RH_ScopedGlobalInstall(AddWaterLevelTriangle, 0x6E7D40);
    RH_ScopedGlobalInstall(AddWaterLevelVertex, 0x6E5A40);

    RH_ScopedGlobalInstall(RenderBoatWakes, 0x6ED9A0);
    RH_ScopedGlobalInstall(RenderWakeSegment, 0x6EA260);

    RH_ScopedOverloadedInstall(GetWaterLevel, "", 0x6EB690, bool(*)(float, float, float, float&, uint8, CVector*));
    RH_ScopedGlobalInstall(SetUpWaterFog, 0x6EA9F0);
    RH_ScopedGlobalInstall(FindNearestWaterAndItsFlow, 0x6E9D70);
    RH_ScopedGlobalInstall(GetWaterLevelNoWaves, 0x6E8580);
    RH_ScopedInstall(GetWaterLevelNoWavesForQuad, 0x6E5BB0);
    RH_ScopedInstall(GetWaterLevelNoWavesForTriangle, 0x6E5E90);
    RH_ScopedGlobalInstall(RenderWaterFog, 0x6E7760);
    RH_ScopedGlobalInstall(CalculateWavesOnlyForCoordinate, 0x6E6EF0);
    RH_ScopedGlobalInstall(RenderWater, 0x6EF650);
    RH_ScopedGlobalInstall(AddWaveToResult, 0x6E81E0);
    RH_ScopedGlobalInstall(SetCameraRange, 0x6E9C80);
}

// NOTSA (codigo de 0x6EAEB9..0x6EB5E9). Transcripcion de la lectura de water.dat del EXE, que usa el sscanf del CRT del EXE (0x8220AD).
bool CWaterLevel::LoadDataFile() {
    // Como el original (0x6EAEB9..0x6EAF38): ruta "DATA\\water.dat" (config 0, 15 bytes de 0x872148) o "DATA\\water1.dat" (config 1, 16 bytes de 0x872158);
    // con cualquier otro valor el EXE usa el buffer de pila sin inicializar.
    char path[16];
    if (m_nWaterConfiguration == 0) {
        std::memcpy(path, "DATA\\water.dat", 15);
    } else if (m_nWaterConfiguration == 1) {
        std::memcpy(path, "DATA\\water1.dat", 16);
    }

    // Como el original (0x6EAF40..0x6EAF59): OpenFile(ruta, "rb") (0x859C6C) se repite mientras el valor devuelto sea negativo (jl).
    FILESTREAM file;
    do {
        file = CFileMgr::OpenFile(path, "rb");
    } while (int32(reinterpret_cast<intptr_t>(file)) < 0);

    // Como el original (0x6EAF5C..0x6EAF68): los contadores se ponen a cero otra vez tras abrir
    NumWaterTriangles = 0;
    NumWaterQuads     = 0;
    NumWaterVertices  = 0;

    // Como el original: 4 vertices de 7 floats (x y z flowX flowY bigWaves smallWaves) en locales de pila que se conservan entre lineas
    struct { float x, y, z, flowX, flowY, bigWaves, smallWaves; } v[4];
    const auto Sscanf = reinterpret_cast<int(__cdecl*)(const char*, const char*, ...)>(0x8220AD); // sscanf del CRT del EXE
    // Como el original: float -> int con _ftol2 (truncacion de 64 bits, se queda con el dword bajo); el byte de flujo es el byte bajo de (flow * 64.0f, 0x859A44)
    const auto Ftol  = [](float f) { return int32(int64(f)); };
    const auto Flow  = [&](float f) { return int8(Ftol(f * 64.0f)); };
    const auto Par   = [&](int i, int8 flowY) { return CRenPar{ v[i].z, v[i].bigWaves, v[i].smallWaves, Flow(v[i].flowX), flowY }; };

    #define WATER_V(i) &v[i].x, &v[i].y, &v[i].z, &v[i].flowX, &v[i].flowY, &v[i].bigWaves, &v[i].smallWaves
    #define WATER_FMT7 "%f %f %f %f %f %f %f"
    for (char* line = CFileLoader::LoadLine(file); line; line = CFileLoader::LoadLine(file)) {
        // Como el original (0x6EAF80..0x6EAF9C): se ignoran las lineas que empiezan por ';', fin de cadena, '*' o 'p'
        const char c = *line;
        if (c == ';' || c == '\0' || c == '*' || c == 'p') {
            continue;
        }

        // Como el original (0x6EB069): flags = 1 por defecto, antes del primer sscanf de cada linea
        int32 flags = 1;
        int   nRead = Sscanf(line, WATER_FMT7 " " WATER_FMT7 " " WATER_FMT7 " " WATER_FMT7 " %d", WATER_V(0), WATER_V(1), WATER_V(2), WATER_V(3), &flags); // 0x6EB074, formato 0x8720F0
        if (nRead == 29 || (nRead = Sscanf(line, WATER_FMT7 " " WATER_FMT7 " " WATER_FMT7 " " WATER_FMT7, WATER_V(0), WATER_V(1), WATER_V(2), WATER_V(3)), nRead == 28)) { // 0x6EB07C / 0x6EB14C, formato 0x872098
            // Como el original (0x6EB39F..0x6EB5CE): cuadrilatero. Bug del EXE: el flowY del primer vertice es el del SEGUNDO (el byte alto del
            // CRenPar del vertice 1 se rellena con el resultado de _ftol2 de v[1].flowY que quedo en BL: 0x6EB491 mov bl,al / 0x6EB4F1 mov [esp+0F5h],bl).
            const int8 flowY1 = Flow(v[1].flowY);
            AddWaterLevelQuad(
                Ftol(v[0].x), Ftol(v[0].y), Par(0, flowY1),
                Ftol(v[1].x), Ftol(v[1].y), Par(1, Flow(v[1].flowY)),
                Ftol(v[2].x), Ftol(v[2].y), Par(2, Flow(v[2].flowY)),
                Ftol(v[3].x), Ftol(v[3].y), Par(3, Flow(v[3].flowY)),
                uint32(flags)
            );
        } else {
            // Como el original (0x6EB155..0x6EB397): triangulo, formato 0x872050 (3 vertices + flags); sin comprobar el resultado
            Sscanf(line, WATER_FMT7 " " WATER_FMT7 " " WATER_FMT7 " %d", WATER_V(0), WATER_V(1), WATER_V(2), &flags);
            AddWaterLevelTriangle(
                Ftol(v[0].x), Ftol(v[0].y), Par(0, Flow(v[0].flowY)),
                Ftol(v[1].x), Ftol(v[1].y), Par(1, Flow(v[1].flowY)),
                Ftol(v[2].x), Ftol(v[2].y), Par(2, Flow(v[2].flowY)),
                uint32(flags)
            );
        }
    }
    #undef WATER_V
    #undef WATER_FMT7

    CFileMgr::CloseFile(file); // 0x6EB5EA
    return true;
}

// NOTSA: Code @ 0x6EB5F4
void CWaterLevel::LoadTextures() {
    CTxdStore::PushCurrentTxd();    
    CTxdStore::SetCurrentTxd(CTxdStore::FindTxdSlot("particle"));

    const auto DoTex = [](auto& inOutTex, auto& outRaster, const char* name) {
        if (!inOutTex) {
            inOutTex = RwTextureRead(name, nullptr);
        }
        outRaster = RwTextureGetRaster(inOutTex);
    };
    DoTex(texWaterclear256, waterclear256Raster, "waterclear256");
    DoTex(texSeabd32,       seabd32Raster,       "seabd32"      );
    DoTex(texWaterwake,     waterwakeRaster,     "waterwake"    );

    CTxdStore::PopCurrentTxd();
}

// 0x6EAE80
void CWaterLevel::WaterLevelInitialise() {
    // Como el original (0x6EAE8E: rep stos, 0x48 dwords en 0xC21B70): se pone a cero m_BlockPolyInfo (12x12 uint16).
    std::memset(&m_BlockPolyInfo, 0, sizeof(m_BlockPolyInfo));
    NumWaterTriangles = 0;
    NumWaterQuads = 0;
    NumWaterVertices = 0;
    NumWaterZonePolys = 0;

    (void)LoadDataFile();           // 0x6EAEB9..0x6EB5EA
    FillQuadsAndTrianglesList();    // 0x6EB5EF call 0x6E7B30

    LoadTextures();                 // 0x6EB5F4..0x6EB681
}

// 0x6E59E0
void CWaterLevel::Shutdown() {
    // Unload Textures
    for (auto tex : { &texWaterclear256, &texSeabd32, &texWaterwake }) {
        if (*tex) {
            RwTextureDestroy(*tex);
            *tex = nullptr;
        }
    }
}

// 0x6E81E0
void CWaterLevel::AddWaveToResult(float x, float y, float* pfWaterLevel, float fUnkn1, float fUnkn2, CVector* pVecNormal)
{
    const float halfX = x * 0.5f, halfY = y * 0.5f;
    const double floorX = std::floor(double(halfX)), floorY = std::floor(double(halfY));
    const float fractionX = float(double(halfX) - floorX), fractionY = float(double(halfY) - floorY);
    const auto ix = int32(floorX * 2.0), iy = int32(floorY * 2.0);
    float base = 0.0f, alongX = 0.0f, alongY = 0.0f;
    // Como el original (0x6E8269..0x6E8287): fractionY + fractionX se suma en la x87 (24 bits) y se compara con 1.0; la mitad inferior solo si la suma redondeada es < 1.0.
    const bool lower = WaterLevelWaveLowerCpp(fractionX, fractionY);
    if (lower) {
        CalculateWavesOnlyForCoordinate2(ix, iy, &base, fUnkn1, fUnkn2);
        CalculateWavesOnlyForCoordinate2(ix + 2, iy, &alongX, fUnkn1, fUnkn2);
        CalculateWavesOnlyForCoordinate2(ix, iy + 2, &alongY, fUnkn1, fUnkn2);
    } else {
        CalculateWavesOnlyForCoordinate2(ix + 2, iy + 2, &base, fUnkn1, fUnkn2);
        CalculateWavesOnlyForCoordinate2(ix, iy + 2, &alongX, fUnkn1, fUnkn2);
        CalculateWavesOnlyForCoordinate2(ix + 2, iy, &alongY, fUnkn1, fUnkn2);
    }
    // 0x6E82D3..0x6E8546 (X87Math): interpolacion del nivel y, con normal, producto vectorial de 0x59C730 sin normalizar; luego CVector::Normalise (0x59C910).
    WaterLevelWaveInterpolateCpp(base, alongX, alongY, fractionX, fractionY, lower, pfWaterLevel, pVecNormal ? &pVecNormal->x : nullptr);
    if (pVecNormal) {
        pVecNormal->Normalise();
    }
}

// 0x6EE240
void CWaterLevel::RenderWaterTriangle(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3) {
    const auto [minX, maxX] = std::make_pair(X1, X2); // Assumes: Starting in top left vertex with clockwise order
    const auto [minY, maxY] = std::minmax(Y1, Y3);
    if (minX >= CameraRangeMaxX || maxX <= CameraRangeMinX || minY >= CameraRangeMaxY || maxY <= CameraRangeMinY) { // Lies outside (of camera) fully
        RenderFlatWaterTriangle(TRIANGLE_ARGS_OUT);
    } else if (minX < CameraRangeMinX || maxX > CameraRangeMaxX) { // Lies inside on X 
        SplitWaterTriangleAlongXLine(maxX > CameraRangeMaxX ? CameraRangeMaxX : CameraRangeMinX, TRIANGLE_ARGS_OUT); // Como el original (0x6EE29E..0x6EE2AC): maxX > rango maximo -> maximo; si no, minimo
    } else if (minY < CameraRangeMinY || maxY > CameraRangeMaxY) { // Lies inside of Y
        SplitWaterTriangleAlongYLine(maxY > CameraRangeMaxY ? CameraRangeMaxY : CameraRangeMinY, TRIANGLE_ARGS_OUT); // Como el original (0x6EE331 / 0x6EE3AE): maxY > rango maximo -> maximo; si no, minimo
    } else { // Lies inside of camera fully
        RenderHighDetailWaterTriangle(TRIANGLE_ARGS_OUT);
    }
}

// NOTSA
auto CWaterLevel::GetWaterLayerTexInfo(int32 layer) -> WaterLayerTexInfo {
    switch (layer) {
    // Como el original (0x6E9974 / 0x6E8F00): capa 0 = escala 0.08 (1/12.5) con desplazamiento "Second" (0xC21184/0xC21180);
    // capa 1 = escala 0.04 (1/25) con desplazamiento "First" (0xC2117C/0xC21178).
    case 0: return { { TextureShiftSecondU, TextureShiftSecondV }, 12.5f };
    case 1: return { { TextureShiftFirstU,  TextureShiftFirstV  }, 25.0f };
    default: NOTSA_UNREACHABLE();
    }
}

// NOTSA
CRGBA CWaterLevel::GetWaterColorForRendering(CRGBA real, DebugWaterColor debug, int32 WaterLayer) {
    if (debug.active) {
        return debug.color;
    } else {
        real *= 0.577f; // AKA 1/sqrt3 OR E_CONST OR neither, but just a coincidence?
        real.a = WaterLayerAlpha[WaterLayer];
        return real;
    }
}

// notsa
auto CWaterLevel::GetTextureUV(int32 X1, int32 Y1, int32 Y3, int32 WaterLayer) -> TexUV {
    const auto txinfo = GetWaterLayerTexInfo(WaterLayer);
    const auto posUV  = CVector2D{ (float)X1, (float)Y1 } / txinfo.size + txinfo.shift;

    const auto CalcShift = [](float p, bool dir) {
        return p - std::floor(p) + (dir ? 7.f : -7.f);
    };

    return {
        .size      = txinfo.size,
        .pos       = posUV,
        .baseShift = CVector2D{
            CalcShift(posUV.x, false),
            CalcShift(posUV.y, Y3 - Y1 <= 0)
        }
    };
}

namespace {
struct WaterLayerParams { float scale, shiftU, shiftV; uint8 alpha; };
// Como el original (0x6E8F00..0x6E8FF3 / 0x6E9974..): constantes float 0.08f/0.04f (multiplicacion, no division) y alfa de capa.
WaterLayerParams GetWaterLayerParams(int32 layer) {
    if (layer == 0) {
        return { StaticRef<float>(0x859018), StaticRef<float>(0xC21184), StaticRef<float>(0xC21180), StaticRef<uint8>(0x8D3808) };
    }
    return { StaticRef<float>(0x858CEC), StaticRef<float>(0xC2117C), StaticRef<float>(0xC21178), StaticRef<uint8>(0x8D380C) };
}
// Como el original: fild + fmul 0.577f (0x872004) + _ftol2, solo el byte bajo; alfa de la capa en el byte alto.
uint32 WaterVertexColor(const CRGBA& c, uint8 alpha) {
    const auto Scale = [](uint8 v) { return uint32(uint8(int32(float(v) * 0.577f))); };
    return (uint32(alpha) << 24) | (Scale(c.r) << 16) | (Scale(c.g) << 8) | Scale(c.b);
}
// Como el original: textura base = (fracc(p) - 7) o (+7 segun el signo de dY); p = X * escala + desplazamiento.
void WaterBaseUV(int32 x, int32 y, int32 dy, const WaterLayerParams& l, float& baseU, float& baseV) {
    const float pu = float(x) * l.scale + l.shiftU;
    const float pv = float(y) * l.scale + l.shiftV;
    const float fracU = float(double(pu) - std::floor(double(pu)));
    const float fracV = float(double(pv) - std::floor(double(pv)));
    baseU = fracU - 7.0f;
    baseV = dy > 0 ? fracV - 7.0f : fracV + 7.0f;
}
void WaterSetVertex(int32 idx, float x, float y, float z, uint32 color, float u, float v) {
    auto* vtx = &TempBufferVertices.m_3d[idx];
    vtx->objVertex.x = x; vtx->objVertex.y = y; vtx->objVertex.z = z;
    std::memcpy(&vtx->color, &color, sizeof(color));
    vtx->u = u; vtx->v = v;
}
uint32 WaterDebugColor(const CRGBA& c) { return (uint32(c.a) << 24) | (uint32(c.r) << 16) | (uint32(c.g) << 8) | uint32(c.b); }
}

// 0x6E8ED0
void CWaterLevel::RenderFlatWaterTriangle_OneLayer(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3, int32 WaterLayer) {
    auto& nVtx = StaticRef<int32>(0xC4B950);
    auto& nIdx = StaticRef<int32>(0xC4B954);
    // Como el original: umbrales 0xFFD / 0x7FD y vaciado con la funcion 0x6E7680 (con LittleTest).
    if (nIdx >= 0xFFD || nVtx >= 0x7FD) {
        plugin::Call<0x6E7680>();
    }
    const int32 base = nVtx;
    const auto layer = GetWaterLayerParams(WaterLayer);
    float baseU, baseV;
    WaterBaseUV(X1, Y1, Y3 - Y1, layer, baseU, baseV);
    const auto& debug = DebugWaterColors[DebugWaterColor::TRI];
    const uint32 color = debug.active ? WaterDebugColor(debug.color) : WaterVertexColor(WaterColorTriangle, layer.alpha);
    WaterSetVertex(base + 0, float(X1), float(Y1), P1.z + 0.0f, color, baseU, baseV);
    WaterSetVertex(base + 1, float(X2), float(Y2), P2.z + 0.0f, color, float(X2 - X1) * layer.scale + baseU, float(Y2 - Y1) * layer.scale + baseV);
    WaterSetVertex(base + 2, float(X3), float(Y3), P3.z + 0.0f, color, float(X3 - X1) * layer.scale + baseU, float(Y3 - Y1) * layer.scale + baseV);
    aTempBufferIndices[nIdx + 0] = RxVertexIndex(base + 0);
    aTempBufferIndices[nIdx + 1] = RxVertexIndex(base + 1);
    aTempBufferIndices[nIdx + 2] = RxVertexIndex(base + 2);
    nVtx = base + 3;
    nIdx += 3;
}

// 0x6EE080
void CWaterLevel::RenderFlatWaterTriangle(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3) {
    if (bSplitBigPolys && X2 - X1 > BigPolySize) {
        SplitWaterTriangleAlongXLine((X1 + X2) / 2, X1, Y1, P1, X2, Y2, P2, X3, Y3, P3);
    } else {
        RenderFlatWaterTriangle_OneLayer(X1, Y1, P1, X2, Y2, P2, X3, Y3, P3, 0);
        RenderFlatWaterTriangle_OneLayer(X1, Y1, P1, X2, Y2, P2, X3, Y3, P3, 1);
    }
}

// 0x6EA260 - Transcripcion del EXE: esquinas = (1-t)*A + t*B / (1-t)*D + t*C, UV con desplazamiento 0xC228D4/0xC228D8,
// color blanco con alfa = alfa * 0x8D390C[], z = fsin completo (x87) * radio de viento.
void CWaterLevel::RenderWakeSegment(
    const CVector2D& vecA, const CVector2D& vecB,
    const CVector2D& vecC, const CVector2D& vecD,
    const float& widthA, const float& widthB,
    const float& alphaA, const float& alphaB,
    const float& wakeZ
) {
    auto& nVtx = StaticRef<int32>(0xC4B950);
    auto& nIdx = StaticRef<int32>(0xC4B954);
    const float angle = float(CTimer::GetTimeInMS() & 0xFFFu) * StaticRef<float>(0x872028);
    const float* mults = &StaticRef<float>(0x8D390C); // 5 floats consecutivos (0x8D390C..0x8D391C)
    const float windRadius = CWeather::WindClipped * 0.4f + 0.2f; // 0x858EE8 / 0x858CC4
    const float uvShiftX = StaticRef<float>(0xC228D4), uvShiftY = StaticRef<float>(0xC228D8);

    for (int32 part = 0; part < 4; ++part) {
        if (nIdx >= 0xFFA || nVtx >= 0x7FC) {
            plugin::Call<0x6E7680>();
        }
        const int32 base = nVtx;
        const float ta = float(part) * 0.25f, tb = float(part + 1) * 0.25f;
        const float oa = 1.0f - ta, ob = 1.0f - tb;
        const float cx[4]{ oa * vecA.x + ta * vecB.x, ob * vecA.x + tb * vecB.x, tb * vecC.x + ob * vecD.x, ta * vecC.x + oa * vecD.x };
        const float cy[4]{ oa * vecA.y + ta * vecB.y, ob * vecA.y + tb * vecB.y, tb * vecC.y + ob * vecD.y, ta * vecC.y + oa * vecD.y };
        float u[4], v[4];
        for (int k = 0; k < 4; ++k) {
            u[k] = cx[k] * 0.25f + uvShiftX;
            v[k] = cy[k] * 0.25f + uvShiftY;
        }
        // Como el original: minimos con comparaciones estrictas en el orden 3/2, 1, 0 (fcomp + test ah,5)
        float minU = (u[2] < u[3]) ? u[2] : u[3];
        if (u[1] < minU) minU = u[1];
        if (u[0] < minU) minU = u[0];
        const float floorU = float(std::floor(double(minU)));
        float minV = (v[2] < v[3]) ? v[2] : v[3];
        if (v[1] < minV) minV = v[1];
        if (v[0] < minV) minV = v[0];
        const double floorV = std::floor(double(minV));

        const float alphas[4]{ alphaA * mults[part], alphaA * mults[part + 1], alphaB * mults[part + 1], alphaB * mults[part] };
        for (int k = 0; k < 4; ++k) {
            const float px = (cx[k] + 3072.0f) * 0.03125f;
            const float fx = float(double(px) - std::floor(double(px)));
            const float py = (cy[k] + 3072.0f) * 0.03125f;
            const float sum = float(double(py) - std::floor(double(py))) + fx;
            float phase = sum * 6.2831855f; // 0x858CBC
            phase = phase + angle;
            // Como el original (0x6EA65A..0x6EA67E): fld phase; fsin; * windRadius; + wakeZ; fstp (el seno entra sin redondear).
            const float z = TrigCompoundSinMulAdd(phase, windRadius, wakeZ);
            auto* vtx = &TempBufferVertices.m_3d[base + k];
            vtx->objVertex.x = cx[k]; vtx->objVertex.y = cy[k]; vtx->objVertex.z = z;
            const uint32 color = (uint32(uint8(int32(alphas[k]))) << 24) | 0xFFFFFFu;
            std::memcpy(&vtx->color, &color, sizeof(color));
            vtx->u = u[k] - floorU;
            vtx->v = float(double(v[k]) - floorV);
        }
        static constexpr int32 IDX[6]{ 0, 2, 1, 0, 3, 2 };
        for (int k = 0; k < 6; ++k) aTempBufferIndices[nIdx + k] = RxVertexIndex(base + IDX[k]);
        nVtx = base + 4;
        nIdx += 6;
    }
}

// 0x6ED9A0 - Transcripcion de la pasada que el EXE tiene inlineada sobre la lista de barcos 0xC27994 (x87 a 24 bits == float):
// las constantes son las del EXE (0.65f 0x858F50, 0.4f 0x858EE8, 0.04f 0x858CEC, 0.5f 0x858B8C, 150.0f 0x8D3940, 0.01f 0x858C58,
// 0.0f 0x858B50, 50.0f 0x858B40, 80.0f 0x858A14, 0.033333335f 0x858F10, 9.0f 0x85EED4, 13.0f 0x858CE4, 160.0f 0x858970,
// 0.33333334f 0x859040, 0.15f 0x858FCC, -0.03f 0xBCF5C28F).
void CWaterLevel::RenderBoatWakes() {
    // 0x6ED9AE..0x6ED9BB: RwRenderStateSet(rwRENDERSTATETEXTURERASTER, [0xC228B8]) y luego CBoat::FillBoatList (0x6F2710)
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, waterwakeRaster);
    CBoat::FillBoatList();

    constexpr float WAKE_Z = std::bit_cast<float>(0xBCF5C28Fu); // [esp+34h]; las referencias widthA y wakeZ apuntan a este mismo float
    for (int16 b = 0; b < 4; ++b) { // 0x6ED9E0: movsx edx,bx / cmp bx,4
        CBoat* const boat = CBoat::apFrameWakeGeneratingBoats.data()[b];
        if (!boat) { // 0x6ED9EA cmp esi,ebp / je 0x6EDD46
            break;
        }

        // 0x6ED9F2..0x6ED9FF: direccion "forward" de la matriz ([m+0x10], [m+0x14]); el EXE no comprueba que la matriz exista aqui
        float prevDirX = boat->m_matrix->GetForward().x;
        float prevDirY = boat->m_matrix->GetForward().y;

        // 0x6EDA07..0x6EDA2B: ancho base = max.x del ColModel * 0.65f (x0.4f si es un Skimmer 0x1CC)
        float baseWidth = boat->GetColModel()->GetBoundingBox().m_vecMax.x * 0.65f;
        if (boat->m_nModelIndex == 0x1CC) {
            baseWidth = baseWidth * 0.4f;
        }

        // 0x6EDA2F..0x6EDA74: anchura del primer punto (velocidad0, contador0) que queda en la pila x87
        const float speed0 = float(boat->m_WakeBoatSpeed.data()[0]);
        float prevWidth = (150.0f - boat->m_WakePtCounters.data()[0]) * ((speed0 * 0.04f + 0.5f) / 150.0f);
        prevWidth = prevWidth * baseWidth;
        prevWidth = prevWidth + baseWidth;
        // 0x6EDA78..0x6EDA88: alfa del primer punto = velocidad0 * 0.01f * 0.0f
        float prevAlpha = speed0 * 0.01f;
        prevAlpha = prevAlpha * 0.0f;

        // 0x6EDA3A..0x6EDAB8: distancia 2D barco-camara (matriz de la camara +0x30 o 0xB6F02C; barco: matriz +0x30 o entidad +4)
        const CVector& camPos  = TheCamera.GetPosition();
        const CVector& boatPos = boat->m_matrix ? boat->m_matrix->GetPosition() : boat->m_placement.m_vPosn;
        const float ddx = boatPos.x - camPos.x;
        const float ddy = boatPos.y - camPos.y;
        const float dist = std::sqrt(ddy * ddy + ddx * ddx);
        if (50.0f < dist) { // 0x6EDAC0 fcomp 50.0 / test ah,41h / jne
            prevAlpha = (80.0f - dist) * 0.033333335f * prevAlpha;
        }

        // 0x6EDAE5: if (N <= 1) saltar el bucle (jle con signo sobre el word sin signo ampliado)
        uint16 count = boat->m_NumWakeCoords;
        if (int32(count) > 1) {
            int16 i = 1;
            do {
                const CVector2D& prev = boat->m_WakeCoords.data()[i - 1]; // [esi+ecx*8+0x640 / 0x644]
                const CVector2D& cur  = boat->m_WakeCoords.data()[i];     // [esi+ecx*8+0x648 / 0x64C]

                // 0x6EDB10..0x6EDB82: direccion hacia el punto anterior, normalizada si |d|^2 > 9; no se dibuja si |d| > 13
                float dirX = prev.x - cur.x;
                float dirY = prev.y - cur.y;
                bool draw = true;
                const float sumSq = dirY * dirY + dirX * dirX;
                if (9.0f < sumSq) { // fcom 9.0 / test ah,41h / jne 0x6EDB88
                    const float len = std::sqrt(sumSq);
                    const float inv = 1.0f / len;
                    dirX = inv * dirX;
                    dirY = inv * dirY;
                    if (13.0f < len) { // fcomp 13.0 / test ah,41h / jne -> xor bl,bl
                        draw = false;
                    }
                }

                // 0x6EDB8A..0x6EDBCF: anchura de este punto
                const float speed = float(boat->m_WakeBoatSpeed.data()[i]);
                float curWidth = (150.0f - boat->m_WakePtCounters.data()[i]) * ((speed * 0.04f + 0.5f) / 150.0f);
                curWidth = curWidth * baseWidth;
                curWidth = curWidth + baseWidth;

                // 0x6EDBD3..0x6EDC4F: esquinas A,B (punto anterior, ancho anterior) y C,D (punto actual, ancho actual)
                const float off1 = prevDirY * prevWidth;
                const float off2 = prevWidth * prevDirX;
                CVector2D A{ prev.x - off1, off2 + prev.y };
                CVector2D B{ off1 + prev.x, prev.y - off2 };
                const float t1 = curWidth * dirY;
                const float t2 = curWidth * dirX;
                CVector2D C{ t1 + cur.x, cur.y - t2 };
                CVector2D D{ cur.x - t1, t2 + cur.y };

                // 0x6EDC55..0x6EDCBA: alfa = (1 - i/N) * 160 (x i x 0.33333334f si i < 3, jge sobre cmp di,3 con signo)
                float alpha = (1.0f - float(i) / float(int32(count))) * 160.0f;
                if (i < 3) {
                    alpha = alpha * float(i);
                    alpha = alpha * 0.33333334f;
                }
                alpha = (speed * 0.01f + 0.15f) * alpha;
                if (50.0f < dist) {
                    alpha = (80.0f - dist) * 0.033333335f * alpha;
                }

                if (draw) { // test bl,bl / je
                    RenderWakeSegment(A, B, C, D, WAKE_Z, curWidth, prevAlpha, alpha, WAKE_Z);
                }

                // 0x6EDCFA..0x6EDD2B
                prevDirX  = dirX;
                prevDirY  = dirY;
                prevAlpha = alpha;
                prevWidth = curWidth;
                ++i;
                count = boat->m_NumWakeCoords;
            } while (int32(i) < int32(count));
        }
    }

    // 0x6EDD46..0x6EDD96: vaciado inline del buffer temporal (LittleTest, RwIm3DTransform con rwIM3D_VERTEXUV, indices, RwIm3DEnd)
    if (uiTempBufferVerticesStored != 0) {
        LittleTest();
        if (RwIm3DTransform(TempBufferVertices.m_3d, uiTempBufferVerticesStored, nullptr, rwIM3D_VERTEXUV)) {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, uiTempBufferIndicesStored);
            RwIm3DEnd();
        }
    }
    uiTempBufferVerticesStored = 0;
    uiTempBufferIndicesStored  = 0;
}

// 0x6ECF00
void CWaterLevel::SplitWaterTriangleAlongXLine(int32 splitAtX, int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3) {
    assert(Y1 == Y2); 

    // Ease of life
    const auto XS = splitAtX;

    const auto splitWidth = XS - X1;
    const auto triWidth   = X2 - X1;

    // Calculate position of split along Y axis
    const auto CalcSplitPosY = [&](int32 fromY, int32 toY) {
        return fromY + (toY - fromY) * splitWidth / triWidth;
    };

    // Interpolation value
    const auto t = (float)splitWidth / (float)triWidth;

    // New interpolations of RenPar's along a few segments
    const auto P12 = lerp(P1, P2, t);
    const auto P13 = lerp(P1, P3, t);
    const auto P23 = lerp(P2, P3, t);

    // Vertex 1 and 2 are always (top left), (top right)
    // Also the triangles always contain a 90deg corner at either the left or right side.

    if (X1 == X3) { // Vertex 3 => (bottom left)
        const auto YS = CalcSplitPosY(Y3, Y1);

        // Como el original (0x6ECF5D..0x6ED2DB): en esta rama el EXE interpola desde P3 (peso 1-t) hacia P1 / P2 (peso t):
        // B = (X1,YS) = (1-t)*P3 + t*P1 ; H = (XS,YS) = (1-t)*P3 + t*P2 (la fuente usaba lerp(P1,P3) / lerp(P2,P3), direccion invertida).
        const auto PB = lerp(P3, P1, t);
        const auto PH = lerp(P3, P2, t);

        // Bottom
        RenderWaterTriangle(
            X1, YS, PB,
            XS, YS, PH,
            X3, Y3, P3
        );

        // Left
        RenderWaterRectangle(
            X1, XS,
            Y1, YS,
            P1, P12, PB, PH
        );

        // Right
        RenderWaterTriangle(
            XS, Y1, P12,
            X2, Y1, P2,
            XS, YS, PH
        );
    } else if (X2 == X3) { // Vertex 3 => (bottom right)
        const auto YS = CalcSplitPosY(Y1, Y3);

        // Como el original (0x6ED094 / 0x6ED5D9 / 0x6ED6B8): orden Bottom, rectangulo, Left
        // Bottom
        RenderWaterTriangle(
            XS, YS, P13,
            X2, YS, P23,
            X3, Y3, P3
        );

        // Right
        RenderWaterRectangle(
            XS, X2,
            Y1, YS,
            P12, P2, P13, P23 // Confirmado en el EXE (0x6ED506-0x6ED5AC): P4=P23, P3=P13, P2=P2, P1=P12
        );

        // Left
        RenderWaterTriangle(
            X1, Y1, P1,
            XS, Y1, P12,
            XS, YS, P13
        );
    } else {
        NOTSA_UNREACHABLE("Triangle has no 90deg corner => Very bad");
    }
}

// 0x6EE5A0
void CWaterLevel::SplitWaterTriangleAlongYLine(int32 splitAtY, int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3) {
    if (DontRenderYSplitTri) { // NOTSA (el EXE no tiene esta comprobacion; false por defecto)
        return;
    }

    // Como el original (0x6EE5B3..0x6EE5D3): dS = splitAtY - Y1 y dH = Y3 - Y1 (enteros con signo) y t = (float)(fild dS / fidiv dH)
    // guardado como float (fstp dword [esp+4Ch]). Las cuatro ramas del EXE usan el mismo t y calculan XS con idiv (truncado a cero).
    const int32 dS = splitAtY - Y1;
    const int32 dH = Y3 - Y1;
    const float t  = (float)dS / (float)dH;

    // Como el original (0x6EE5C1 `cmp esi,edi; jge 0x6EED72`): Y1 < Y3 -> ramas A/B; si no C/D.
    // Segundo salto (0x6EE5DD `cmp ebp,eax; jne 0x6EEA48` / 0x6EED72 `cmp ebp,eax; jne 0x6EF1D7`): X1 == X3 -> A/C; si no B/D.
    if (Y1 < Y3) {
        if (X1 == X3) { // Rama A (0x6EE5E5): XS = (X1 - X2) * dS / dH + X2; hipotenusa V2-V3
            const int32 XS = (X1 - X2) * dS / dH + X2;
            RenderWaterTriangle(XS, Y1, lerp(P2, P1, t), X2, Y2, P2, XS, splitAtY, lerp(P2, P3, t));
            RenderWaterRectangle(X1, XS, Y1, splitAtY, P1, lerp(P2, P1, t), lerp(P1, P3, t), lerp(P2, P3, t));
            RenderWaterTriangle(X1, splitAtY, lerp(P1, P3, t), XS, splitAtY, lerp(P2, P3, t), X3, Y3, P3);
        } else {        // Rama B (0x6EEA48): XS = (X2 - X1) * dS / dH + X1; hipotenusa V1-V3
            const int32 XS = (X2 - X1) * dS / dH + X1;
            RenderWaterTriangle(X1, Y1, P1, XS, Y1, lerp(P1, P2, t), XS, splitAtY, lerp(P1, P3, t));
            RenderWaterRectangle(XS, X2, Y1, splitAtY, lerp(P1, P2, t), P2, lerp(P1, P3, t), lerp(P2, P3, t));
            RenderWaterTriangle(XS, splitAtY, lerp(P1, P3, t), X2, splitAtY, lerp(P2, P3, t), X3, Y3, P3);
        }
    } else {
        if (X1 == X3) { // Rama C (0x6EED7A): como A pero el rectangulo va de splitAtY a Y1 y sus parametros son (P13, P23, P1, P21)
            const int32 XS = (X1 - X2) * dS / dH + X2;
            RenderWaterTriangle(XS, Y1, lerp(P2, P1, t), X2, Y2, P2, XS, splitAtY, lerp(P2, P3, t));
            RenderWaterRectangle(X1, XS, splitAtY, Y1, lerp(P1, P3, t), lerp(P2, P3, t), P1, lerp(P2, P1, t));
            RenderWaterTriangle(X1, splitAtY, lerp(P1, P3, t), XS, splitAtY, lerp(P2, P3, t), X3, Y3, P3);
        } else {        // Rama D (0x6EF1D7): como B pero el rectangulo va de splitAtY a Y1
            const int32 XS = (X2 - X1) * dS / dH + X1;
            RenderWaterTriangle(X1, Y1, P1, XS, Y1, lerp(P1, P2, t), XS, splitAtY, lerp(P1, P3, t));
            // Como el original (0x6EF3E1 y 0x6EF445, bug del EXE): las dos primeras esquinas reciben lerp(P1,P3,t) (la segunda deberia ser lerp(P2,P3,t)).
            RenderWaterRectangle(XS, X2, splitAtY, Y1, lerp(P1, P3, t), lerp(P1, P3, t), lerp(P1, P2, t), P2);
            RenderWaterTriangle(XS, splitAtY, lerp(P1, P3, t), X2, splitAtY, lerp(P2, P3, t), X3, Y3, P3);
        }
    }
}

// 0x6EC5D0
void CWaterLevel::RenderWaterRectangle(int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4) {
    const auto [minY, maxY] = std::minmax(Y1, Y2);
    if (minX >= CameraRangeMaxX || maxX <= CameraRangeMinX) { // Lies outside (of camera) fully on X
        RenderFlatWaterRectangle(minX, maxX, Y1, Y2, P1, P2, P3, P4);
        SetUpWaterFog(minX, minY, maxX, maxY); // Como el original (0x6ECE2C): el EXE ordena Y1/Y2 en sus ranuras (Y1'=max, Y2'=min, 0x6EC5DE-0x6EC5F1) y pasa (minX, Y2', maxX, Y1') = (minX, minY, maxX, maxY)
    } else if (minY >= CameraRangeMaxY || maxY <= CameraRangeMinY) { // Lies outside on Y
        RenderFlatWaterRectangle(minX, maxX, Y1, Y2, P1, P2, P3, P4);
        SetUpWaterFog(minX, minY, maxX, maxY); // Como el original (0x6ECEEF): igual que arriba
    } else if (minX < CameraRangeMinX || maxX > CameraRangeMaxX) { // Lies inside on X
        // Como el original (0x6EC634..0x6EC646): maxX > rango maximo -> maximo; si no, minimo.
        SplitWaterRectangleAlongXLine(maxX > CameraRangeMaxX ? CameraRangeMaxX : CameraRangeMinX, minX, maxX, Y1, Y2, P1, P2, P3, P4);
    } else if (minY < CameraRangeMinY || maxY > CameraRangeMaxY) { // Lies inside of Y
        SplitWaterRectangleAlongYLine(minY < CameraRangeMinY ? CameraRangeMinY : CameraRangeMaxY, minX, maxX, Y1, Y2, P1, P2, P3, P4);
    } else { // Lies inside of camera fully
        RenderHighDetailWaterRectangle(minX, maxX, Y1, Y2, P1, P2, P3, P4);
    }
}

// 0x6EBEC0
void CWaterLevel::RenderFlatWaterRectangle(int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4) {
    // Como el original: las mitades se pintan recursivamente con esta misma funcion (t = 0.5f), no con Split*/RenderWaterRectangle.
    if (bSplitBigPolys) {
        if (maxX - minX > BigPolySize) {
            const int32 mid = (minX + maxX) / 2;
            const CRenPar P12 = lerp(P1, P2, 0.5f), P34 = lerp(P3, P4, 0.5f);
            RenderFlatWaterRectangle(minX, mid, Y1, Y2, P1, P12, P3, P34);
            RenderFlatWaterRectangle(mid, maxX, Y1, Y2, P12, P2, P34, P4);
            return;
        }
        if (Y2 - Y1 > BigPolySize) {
            const int32 mid = (Y1 + Y2) / 2;
            const CRenPar P13 = lerp(P1, P3, 0.5f), P24 = lerp(P2, P4, 0.5f);
            RenderFlatWaterRectangle(minX, maxX, Y1, mid, P1, P2, P13, P24);
            // Como el original (0x6EC3F1, bug del EXE): la mitad inferior recibe P2 (no P3) como tercer CRenPar: (P13, P24, P2, P4)
            RenderFlatWaterRectangle(minX, maxX, mid, Y2, P13, P24, P2, P4);
            return;
        }
    }
    for (int32 lyr = 0; lyr < 2; lyr++) {
        RenderFlatWaterRectangle_OneLayer(minX, maxX, Y1, Y2, P1, P2, P3, P4, lyr);
    }
}

// 0x6E9940
void CWaterLevel::RenderFlatWaterRectangle_OneLayer(int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4, int32 WaterLayer) {
    auto& nVtx = StaticRef<int32>(0xC4B950);
    auto& nIdx = StaticRef<int32>(0xC4B954);
    // Como el original: umbrales 0xFFA / 0x7FC y vaciado con la funcion 0x6E7680 (con LittleTest).
    if (nIdx >= 0xFFA || nVtx >= 0x7FC) {
        plugin::Call<0x6E7680>();
    }
    const int32 base = nVtx;
    const auto layer = GetWaterLayerParams(WaterLayer);
    float baseU, baseV;
    WaterBaseUV(minX, Y1, Y2 - Y1, layer, baseU, baseV);
    const auto& debug = DebugWaterColors[DebugWaterColor::RECT];
    const uint32 color = debug.active ? WaterDebugColor(debug.color) : WaterVertexColor(WaterColor, layer.alpha);
    // Como el original el orden de los vertices es TL, TR, BL, BR (P1, P2, P3, P4) e indices {0,1,2,3,1,2}.
    const float u1 = float(maxX - minX) * layer.scale + baseU;
    const float v2 = float(Y2 - Y1) * layer.scale + baseV;
    WaterSetVertex(base + 0, float(minX), float(Y1), P1.z + 0.0f, color, baseU, baseV);
    WaterSetVertex(base + 1, float(maxX), float(Y1), P2.z + 0.0f, color, u1,    baseV);
    WaterSetVertex(base + 2, float(minX), float(Y2), P3.z + 0.0f, color, baseU, v2);
    WaterSetVertex(base + 3, float(maxX), float(Y2), P4.z + 0.0f, color, u1,    v2);
    static constexpr int32 IDX[6]{ 0, 1, 2, 3, 1, 2 };
    for (int k = 0; k < 6; ++k) aTempBufferIndices[nIdx + k] = RxVertexIndex(base + IDX[k]);
    nVtx = base + 4;
    nIdx += 6;
}

void CWaterLevel::RenderHighDetailWaterRectangle(int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4) {
    plugin::Call<0x6EB810>(minX, maxX, Y1, Y2, P1, P2, P3, P4);
}

// 0x6E73A0
void CWaterLevel::SplitWaterRectangleAlongXLine(int32 splitAtX, int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4) {
    const auto t = (float)(splitAtX - minX) / (float)(maxX - minX);

    const auto P12 = lerp(P1, P2, t);
    const auto P34 = lerp(P3, P4, t);

    // Left
    RenderWaterRectangle(
        minX, splitAtX,
        Y1, Y2,
        P1, P12, P3, P34
    );

    // Right
    RenderWaterRectangle(
        splitAtX, maxX,
        Y1, Y2,
        P12, P2, P34, P4
    );
}

// 0x6ED6D0 - Though fully inlined into `RenderWaterRectangle`
void CWaterLevel::SplitWaterRectangleAlongYLine(int32 splitAtY, int32 minX, int32 maxX, int32 Y1, int32 Y2, CRenPar P1, CRenPar P2, CRenPar P3, CRenPar P4) {
    // Como el original (0x6ED6D0 / inline 0x6EC713): t = (splitAtY - Y1) / (Y2 - Y1) con Y1/Y2 tal cual (fild + fidiv), sin min/max.
    const auto t = (float)(splitAtY - Y1) / (float)(Y2 - Y1);

    const auto P13 = lerp(P1, P3, t);
    const auto P24 = lerp(P2, P4, t);

    // Top
    RenderWaterRectangle(
        minX, maxX,
        Y1, splitAtY,
        P1, P2, P13, P24
    );

    // Bottom
    RenderWaterRectangle(
        minX,     maxX,
        splitAtY, Y2,
        P13, P24, P3, P4
    );
}

// 0x6EB710
void CWaterLevel::PreRenderWater() {
    ZoneScoped;

    if (CGame::CanSeeWaterFromCurrArea()) {
        ScanThroughBlocks();
        UpdateFlow();
        HandleBeachToysStuff();
    }
}

// NOTSA: From PreRenderWater()
void CWaterLevel::UpdateFlow() {
    if (CTimer::m_FrameCounter % 32 == 29) {
        CWaterLevel::FindNearestWaterAndItsFlow();
    }

    // Como el original (0x6EB733..): el paso es TimeStep * 0.001f (constante float 0x858CDC, no una division entre 1000);
    // |d| < paso -> valor deseado; si no, se resta/suma el paso segun d < 0 (fcom + test ah,5).
    const auto CalculateFlowOnAxis = [
        step = CTimer::GetTimeStep() * 0.001f
    ](float desired, float curr) {
        const float delta = desired - curr;
        const float absDelta = delta < 0.f ? -delta : delta;
        if (absDelta < step) {
            return desired;
        }
        return delta < 0.f ? curr - step : curr + step;
    };

    m_CurrentFlow = {
        CalculateFlowOnAxis(m_CurrentDesiredFlow.x, m_CurrentFlow.x),
        CalculateFlowOnAxis(m_CurrentDesiredFlow.y, m_CurrentFlow.y)
    };
}

// 0x6EB690
bool CWaterLevel::GetWaterLevel(float x, float y, float z, float& pOutWaterLevel, uint8 bTouchingWater, CVector* pVecNormals) {
    float smallWaves, bigWaves;
    if (!GetWaterLevelNoWaves({x, y, z}, &pOutWaterLevel, &smallWaves, &bigWaves)) {
        return false;
    }
     
    if ((pOutWaterLevel - z > 3.0F) && !bTouchingWater) {
        pOutWaterLevel = 0.0F;
        return false;
    }

    AddWaveToResult(x, y, &pOutWaterLevel, smallWaves, bigWaves, pVecNormals);

    return true;
}

// 0x6EA9F0
void CWaterLevel::SetUpWaterFog(int32 minX, int32 minY, int32 maxX, int32 maxY) {
    if (!CWaterLevel::m_bWaterFog || gWaterFogIndex >= 70) {
        return;
    }

    // Como el original (0x6EAA0D-0x6EAA14): FindPlayerCoors se llama ANTES de GetWaterLevelNoWaves (aunque despues se salga).
    const auto plyrPos = FindPlayerCoors();

    // Como el original (0x6EAA58..0x6EAA78): si GetWaterLevelNoWaves acierta y big == 0 && small == 0 la funcion termina sin registrar nada.
    float fogZ = 0.f;
    {
        float waterLvl, bigWaves, smallWaves;
        if (GetWaterLevelNoWaves({(float)minX, (float)minY, 0.f}, &waterLvl, &bigWaves, &smallWaves)) {
            if (bigWaves == 0.f && smallWaves == 0.f) {
                return;
            }
            fogZ = waterLvl;
        }
    }

    // Como el original (0x6EAAF1): solo se pone a true (nunca se borra aqui).
    if (m_fWaterFogHeight + fogZ > plyrPos.z && CRect{ (float)minX, (float)minY, (float)maxX, (float)maxY }.IsPointInside(plyrPos)) {
        gbPlayerIsInsideWaterFog = true;
    }

    const auto idx = gWaterFogIndex++;
    ms_WaterFog.minX[idx] = minX;
    ms_WaterFog.minY[idx] = minY;
    ms_WaterFog.maxX[idx] = maxX;
    ms_WaterFog.maxY[idx] = maxY;
    ms_WaterFog.z[idx]    = fogZ;
}

// 0x6E9D70
void CWaterLevel::FindNearestWaterAndItsFlow() {
    const auto& position = TheCamera.GetPosition();
    const float x = position.x, y = position.y;
    // Como el original (0x6E9DBA..0x6E9E05): fcomp + test ah,41h / test ah,1 solo rechazan x <= -3000 o x >= 3000 ordenados: un NaN sigue adelante.
    if (x <= -3000.0f || x >= 3000.0f || y <= -3000.0f || y >= 3000.0f) {
        TheCamera.m_fDistanceToWater = 0.0f;
        TheCamera.m_fHeightOfNearestWater = 0.0f;
        m_CurrentDesiredFlow.x = 0.0f; m_CurrentDesiredFlow.y = 0.0f;
        return;
    }
    float nearest = 10000000.0f, nearestWaves = 10000000.0f, height = 0.0f;
    for (int32 i = 0; i < int32(NumWaterQuads); ++i) {
        const auto& quad = WaterQuads[i];
        const auto& a = m_aVertices[quad.verts[0]];
        const auto& b = m_aVertices[quad.verts[1]];
        const auto& c = m_aVertices[quad.verts[2]];
        // Distancia con las restas a 24 bits, suma de cuadrados y fsqrt como el EXE (X87Math).
        const float distance = WaterLevelNearestDistanceCpp(x, y, float(a.x), float(b.x), float(a.y), float(c.y));
        bool waves = false;
        for (const auto index : quad.verts) {
            const auto& vertex = m_aVertices[index];
            waves |= vertex.rp.bigWaves != 0.0f || vertex.rp.smallWaves != 0.0f;
        }
        if (distance < nearestWaves && waves) {
            nearestWaves = distance;
            height = a.rp.z;
        }
        if (distance < nearest) {
            nearest = distance;
            float vertexX[4], vertexY[4];
            for (size_t k = 0; k < 4; ++k) {
                const auto& vertex = m_aVertices[quad.verts[k]];
                vertexX[k] = float(vertex.x);
                vertexY[k] = float(vertex.y);
            }
            const auto chosen = size_t(WaterLevelNearestFlowIndexCpp(x, y, vertexX, vertexY));
            const auto& vertex = m_aVertices[quad.verts[chosen]];
            m_CurrentDesiredFlow.x = float(vertex.rp.flowX) * (1.0f / 64.0f);
            m_CurrentDesiredFlow.y = float(vertex.rp.flowY) * (1.0f / 64.0f);
        }
    }
    TheCamera.m_fDistanceToWater = nearestWaves;
    TheCamera.m_fHeightOfNearestWater = height;
}

// 0x6E8580
bool CWaterLevel::GetWaterLevelNoWaves(CVector pos, float * pOutWaterLevel, float * pOutBigWaves, float * pOutSmallWaves) {
    // The EXE evaluates the multiply/add in x87 before passing a double to floor.
    const auto blockIndex = [](float coord) -> int32 {
        const auto value = std::floor(double(coord) * double(0.002f) + 6.0);
        if (!std::isfinite(value) || value < double(INT32_MIN) || value > double(INT32_MAX)) return INT32_MIN;
        return int32(value);
    };
    const auto x = blockIndex(pos.x), y = blockIndex(pos.y);
    if (x < 0 || x >= 12 || y < 0 || y >= 12) {
        *pOutWaterLevel = 0.0f;
        if (pOutBigWaves) *pOutBigWaves = 1.0f;
        if (pOutSmallWaves) *pOutSmallWaves = 0.0f;
        return true;
    }
    const auto check = [&](const PolyInfo& info) {
        switch (info.Type()) {
        case PolyInfo::PType::SINGLE_QUAD:
            return GetWaterLevelNoWavesForQuad(&WaterQuads[info.Id()], pos.x, pos.y, pos.z, pOutWaterLevel, pOutBigWaves, pOutSmallWaves);
        case PolyInfo::PType::SINGLE_TRI:
            return GetWaterLevelNoWavesForTriangle(&WaterTriangles[info.Id()], pos.x, pos.y, pos.z, pOutWaterLevel, pOutBigWaves, pOutSmallWaves);
        default:
            return false;
        }
    };
    const auto& info = m_BlockPolyInfo[x][y];
    if (info.Type() == PolyInfo::PType::NONE) return false;
    if (info.Type() != PolyInfo::PType::COMBO) return check(info);
    for (uint32 i = info.Id(); m_PolyCombos[i].Type() != PolyInfo::PType::NONE; ++i) {
        if (check(m_PolyCombos[i])) return true;
    }
    return false;
}

bool CWaterLevel::GetWaterDepth(const CVector& vecPos, float* pOutWaterDepth, float* pOutWaterLevel, float* pOutGroundLevel)
{
    return plugin::CallAndReturn<bool, 0x6EA960, const CVector&, float*, float*, float*>
        (vecPos, pOutWaterDepth, pOutWaterLevel, pOutGroundLevel);
}

// 0x6E7760
void CWaterLevel::RenderWaterFog() {
    ZoneScoped;

    if (!m_bWaterFog || !m_bWaterFogScript) return;
    const auto truncate = [](double value) -> int32 {
        if (!std::isfinite(value) || value < -9223372036854775808.0 || value >= 9223372036854775808.0) return 0;
        return int32(uint32(int64(value)));
    };
    const int32 layers = truncate(double(int32(m_WaterFogDensity)) * CWeather::WaterFogFXControl);
    if (!(CWeather::UnderWaterness < CPostEffects::m_fWaterFXStartUnderWaterness) || !layers) {
        gWaterFogIndex = 0;
        return;
    }
    const float step = float(double(m_fWaterFogHeight) / float(layers));
    const int32 count = int32(gWaterFogIndex);
    gWaterFogIndex = 0;
    auto& insideFade = StaticRef<float>(0xC228EC);
    auto& insideHold = StaticRef<float>(0xC228F0);
    const float fadeSpeed = StaticRef<float>(0x8D37F0);
    const float timeStep = CTimer::GetTimeStep();
    if (gbPlayerIsInsideWaterFog) {
        const double fade = double(timeStep) * fadeSpeed + insideFade;
        insideFade = float(fade);
        if (fade > 1.0) insideFade = 1.0f;
        insideHold = 40.0f;
    } else {
        const double hold = double(insideHold) - timeStep;
        insideHold = float(hold);
        if (hold <= 0.0) {
            insideHold = 0.0f;
            const double fade = double(insideFade) - double(timeStep) * fadeSpeed;
            insideFade = float(fade);
            if (fade < 0.0) insideFade = 0.0f;
        }
    }
    gbPlayerIsInsideWaterFog = false;
    const auto insideColor = StaticRef<CRGBA>(0x8D37EC);
    if (insideFade > 0.0f) {
        const int32 alphaBase = truncate(double(insideColor.a) * insideFade);
        const auto alpha = uint8(truncate(double(alphaBase) * CWeather::WaterFogFXControl));
        CPostEffects::ImmediateModeRenderStatesStore();
        CPostEffects::ImmediateModeRenderStatesSet();
        CPostEffects::DrawQuad(0.0f, 0.0f, float(RsGlobal.maximumWidth), float(RsGlobal.maximumHeight), insideColor.r, insideColor.g, insideColor.b, alpha, nullptr);
        CPostEffects::ImmediateModeRenderStatesReStore();
        if (insideFade == 1.0f) return;
    }
    CPostEffects::ImmediateModeRenderStatesStore();
    CPostEffects::ImmediateModeRenderStatesSet();
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, nullptr);
    const int32 visibleLayers = truncate((1.0 - double(insideFade)) * layers);
    const auto color = StaticRef<CRGBA>(0x8D37E8);
    int32 vertices = 0;
    const auto flush = [&] {
        if (vertices && RwIm3DTransform(TempBufferVertices.m_3d, vertices, nullptr, rwIM3D_VERTEXXYZ)) {
            RwIm3DRenderPrimitive(rwPRIMTYPETRILIST);
            RwIm3DEnd();
        }
        vertices = 0;
    };
    for (int32 i = 0; i < count; ++i) {
        const float minX = float(ms_WaterFog.minX[i]), maxX = float(ms_WaterFog.maxX[i]);
        const float minY = float(ms_WaterFog.minY[i]), maxY = float(ms_WaterFog.maxY[i]);
        const float xs[]{minX,maxX,maxX,minX,maxX,minX};
        const float ys[]{minY,minY,maxY,minY,maxY,maxY};
        float z = ms_WaterFog.z[i];
        for (int32 layer = 0; layer < visibleLayers; ++layer) {
            for (int32 k = 0; k < 6; ++k) {
                auto* vertex = &TempBufferVertices.m_3d[vertices++];
                RwIm3DVertexSetPos(vertex, xs[k], ys[k], z);
                RwIm3DVertexSetRGBA(vertex, color.r, color.g, color.b, color.a);
                if (vertices == 2046) flush();
            }
            z = float(double(z) + step);
        }
    }
    flush();
    CPostEffects::ImmediateModeRenderStatesReStore();
}

namespace {
// Preparacion comun de CalculateWavesOnlyForCoordinate (0x6E6EF0..0x6E6F49) y de CalculateWavesOnlyForCoordinate2Native (0x6E7210..0x6E726F): valor absoluto con neg (INT_MIN sigue negativo),
// cdq + sub, sar 1 y and 7 como indices de las tablas de factores (0x8D38C8 / 0x8D38E8) y restos de (tiempo - desplazamiento) entre 5000, 3500 y 3000 (div sin signo, 0x1388 / 0xDAC / 0xBB8).
struct WaterWaveSetup {
    int32 absX, absY;
    float multX, multY;
    uint32 rem1, rem2, rem3;
};
WaterWaveSetup MakeWaterWaveSetup(int32 x, int32 y) {
    WaterWaveSetup setup{};
    setup.absX = x < 0 ? int32(0u - uint32(x)) : x;
    setup.absY = y < 0 ? int32(0u - uint32(y)) : y;
    const int32 foldedX = int32(uint32(setup.absX) - uint32(setup.absX >> 31));
    const int32 foldedY = int32(uint32(setup.absY) - uint32(setup.absY >> 31));
    setup.multX = CWaterLevel::faWaveMultipliersX.data()[(foldedX >> 1) & 7];
    setup.multY = CWaterLevel::faWaveMultipliersY.data()[(foldedY >> 1) & 7];
    const uint32 elapsed = CTimer::m_snTimeInMilliseconds - CWaterLevel::m_nWaterTimeOffset;
    setup.rem1 = elapsed % 5000u;
    setup.rem2 = elapsed % 3500u;
    setup.rem3 = elapsed % 3000u;
    return setup;
}
const float* WaterSinTable() { return StaticRef<std::array<float, 256>>(0xBB3E00).data(); }
}

// 0x6E6EF0 - Aritmetica x87 del EXE en X87Math (tabla de senos 0xBB3E00 indexada por _ftol2 & 0xFF); la normal se normaliza aqui con CVector::Normalise (0x59C910).
void CWaterLevel::CalculateWavesOnlyForCoordinate(
    int32 x, int32 y,
    float bigWavesAmplitude,
    float smallWavesAmplitude,
    float& outWave,
    float& colorMult,
    float& glare,
    CVector& vecNormal
)
{
    const WaterWaveSetup setup = MakeWaterWaveSetup(x, y);
    WaterLevelWavesFullCpp(setup.absX, setup.absY, setup.multX, setup.multY, CWeather::Wavyness, setup.rem1, setup.rem2, setup.rem3,
        bigWavesAmplitude, smallWavesAmplitude, WaterSinTable(), &outWave, &vecNormal.x);
    vecNormal.Normalise();
    WaterLevelWaveColorGlareCpp(&vecNormal.x, StaticRef<float>(0x8D3924), StaticRef<float>(0x8D3920), CWeather::SunGlare, &colorMult, &glare);
}

// 0x6E5810
void CWaterLevel::MarkQuadsAndPolysToBeRendered(int32 blockX, int32 blockY, bool isInInterior) {
    using PType = PolyInfo::PType;

    // Horrible naming, sorry.
    const auto ProcessPoly = [&](PolyInfo data) {
        switch (data.Type()) {
        case PType::SINGLE_QUAD:
            WaterQuads[data.Id()].DoMarkToBeRendered(isInInterior);
            break;
        case PType::SINGLE_TRI:
            WaterTriangles[data.Id()].DoMarkToBeRendered(isInInterior);
            break;
        }
    };

    auto& blockPolyInfo = m_BlockPolyInfo[blockX][blockY];
    switch (blockPolyInfo.Type()) {
    case PType::SINGLE_QUAD:
    case PType::SINGLE_TRI:
        ProcessPoly(blockPolyInfo);
        break;
    case PType::COMBO: {
        for (auto& comboPoly : m_PolyCombos | rng::views::drop(blockPolyInfo.Id())) {
            if (comboPoly.Type() == PType::NONE) {
                break; // End of sequence
            }
            ProcessPoly(comboPoly);
        }
        break;
    }
    }
}

// 0x6E7210
// Orden de pila del EXE (x, y, grandes, pequenas, altura), distinto del interfaz publico. Como el EXE, sale con EAX = indice de la tercera onda, ECX/EDX = residuo de _ftol2 (0x821B40) de esa onda y los
// argumentos x, y, grandes de la pila reescritos (x = bits de float(|y|), y = resto entre 3000, grandes = resto entre 3500); termina con ResidueRegsCpp (contrib/audit_register_residue.py).
#pragma runtime_checks("", off)
__declspec(safebuffers) void CWaterLevel::CalculateWavesOnlyForCoordinate2Native(int32 x, int32 y, float bigWavesAmpl, float smallWavesAmpl, float* height) {
    const WaterWaveSetup setup = MakeWaterWaveSetup(x, y);
    const Ftol2Regs regs = WaterLevelWavesNativeCpp(setup.absX, setup.absY, setup.multX, setup.multY, CWeather::Wavyness, setup.rem1, setup.rem2, setup.rem3,
        bigWavesAmpl, smallWavesAmpl, WaterSinTable(), height);
    // Los argumentos de la pila quedan como los deja el EXE (el banco DependenciesWorld9 compara los cinco huecos tras la llamada); la escritura volatil impide que se elimine.
    *const_cast<volatile int32*>(&x) = std::bit_cast<int32>(float(setup.absY));
    *const_cast<volatile int32*>(&y) = int32(setup.rem3);
    *const_cast<volatile uint32*>(reinterpret_cast<const uint32*>(&bigWavesAmpl)) = setup.rem2;
    (void)ResidueRegsCpp(regs.ecx, regs.edx, regs.eax);
}
#pragma runtime_checks("", restore)
void CWaterLevel::CalculateWavesOnlyForCoordinate2(int32 x,int32 y,float* height,float bigWavesAmpl,float smallWavesAmpl) {
    CalculateWavesOnlyForCoordinate2Native(x,y,bigWavesAmpl,smallWavesAmpl,height);
}

// 0x6E6CA0
void CWaterLevel::BlockHit(int32 blockX, int32 blockY) {
    if (blockX >= 0 && blockX < NUM_WATER_BLOCKS_ROWCOL && blockY >= 0 && blockY < NUM_WATER_BLOCKS_ROWCOL) {
        MarkQuadsAndPolysToBeRendered(blockX, blockY, CGame::currArea != AREA_CODE_NORMAL_WORLD);
    }

    // Blocks at the edge of the world (index 0 and 11) need to be handled both ways, the quads and polys are to be rendered, but also the general ocean plane needs to be rendered on them
    if (blockX <= 0 || blockX >= (NUM_WATER_BLOCKS_ROWCOL - 1) || blockY <= 0 || blockY >= (NUM_WATER_BLOCKS_ROWCOL - 1)) {
        if (m_NumBlocksOutsideWorldToBeRendered < (uint32)m_MaxNumBlocksOutsideWorldToBeRendered) {
            const auto idx                         = m_NumBlocksOutsideWorldToBeRendered++;
            m_BlocksToBeRenderedOutsideWorldX[idx] = blockX;
            m_BlocksToBeRenderedOutsideWorldY[idx] = blockY;
        }
    }
}

// 0x6E6D10
void CWaterLevel::ScanThroughBlocks() {
    m_NumBlocksOutsideWorldToBeRendered = 0;

    const auto frustumPts = TheCamera.GetFrustumPoints();
    CVector2D scanPts[5]{};
    for (auto i = 0; i < 5; i++) {
        // Como el original (0x6E6DBC..): x87 multiplica por la constante float 0.002f (0x858F44) y suma 6.0f (0x858B44); no divide entre 500.
        scanPts[i] = CVector2D{ frustumPts[i].x * 0.002f + 6.f, frustumPts[i].y * 0.002f + 6.f };
    }
    CWorldScan::ScanWorld(scanPts, 5, BlockHit);
}

void CWaterLevel::RenderHighDetailWaterTriangle(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3) {
    plugin::CallAndReturn<void, 0x6EDDC0, int32, int32, CRenPar, int32, int32, CRenPar, int32, int32, CRenPar>(X1, Y1, P1, X2, Y2, P2, X3, Y3, P3);
}

namespace {
// Como el original (0x6EF8E2..0x6EFAF9, dentro de RenderWater): desplazamientos de textura y ruido del agua. La aritmetica x87 esta en X87Math; el rand de
// la CRT del EXE (0x821B1E) se llama por direccion, en el mismo orden y con la pila x87 vacia.
void WaterRenderShifts() {
    auto* shifts = reinterpret_cast<float*>(0x8D3824); // 0x8D3824, 0x8D3828, 0x8D382C, 0x8D3830
    WaterLevelShiftsAdvanceCpp(CTimer::GetTimeStep(), CWaterLevel::m_CurrentFlow.x, CWaterLevel::m_CurrentFlow.y, StaticRef<float>(0x8D392C), shifts);
    const uint32 time = CTimer::GetTimeInMS();
    const float angleA = IntTimesFloatCpp(int32(time & 0xFFFu), std::bit_cast<float>(0x3AC90FDBu));  // fild * 0x872174
    CWaterLevel::TextureShiftSecondU = WaterLevelSinMulMulAddCpp(angleA, CWeather::Wavyness, StaticRef<float>(0x859018), shifts[0]); // 0xC21184
    CWaterLevel::TextureShiftSecondV = WaterLevelCosMulMulAddCpp(angleA, CWeather::Wavyness, StaticRef<float>(0x859018), shifts[1]); // 0xC21180
    const float angleB = IntTimesFloatCpp(int32(time & 0x1FFFu), std::bit_cast<float>(0x3A490FDBu));  // fild * 0x872170
    StaticRef<uint32>(0xC2117C) = StaticRef<uint32>(0x8D382C);                                         // copia de los bits (mov eax,[0x8D382C]; mov [0xC2117C],eax)
    const float firstNoise = StaticRef<float>(0x8D3928);
    const float cosineSpill = TrigSimpleCosCpp(angleB);
    CWaterLevel::TextureShiftFirstV = TrigCompoundCosMulAdd(angleB, std::bit_cast<float>(0x3CC49BA6u), shifts[3]); // 0xC21178: cos * 0x87216C + 0x8D3830
    const int32 randomA = plugin::CallAndReturn<int32, 0x821B1E>();
    const float noiseScale = std::bit_cast<float>(0x38000100u);                                         // 0x858C7C
    StaticRef<float>(0xC21174) = WaterLevelNoiseFirstCpp(randomA, noiseScale, firstNoise);
    const float secondNoise = StaticRef<float>(0x8D3928);
    const int32 randomB = plugin::CallAndReturn<int32, 0x821B1E>();
    StaticRef<float>(0xC21174) = TrigCompoundSinMulAdd(angleB, StaticRef<float>(0x8D3834), StaticRef<float>(0xC21174));
    StaticRef<float>(0xC21170) = WaterLevelNoiseSecondCpp(randomB, noiseScale, secondNoise, cosineSpill, StaticRef<float>(0x8D3834));
}
}

// 0x6EF650
void CWaterLevel::RenderWater() {
    if (!CGame::CanSeeWaterFromCurrArea()) return;
    SetCameraRange();
    plugin::Call<0x734650>();
    uiTempBufferIndicesStored = 0;
    uiTempBufferVerticesStored = 0;
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, seabd32Raster);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(static_cast<RwRenderState>(30), nullptr);
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    for (int32 i = 0; i < int32(m_NumBlocksOutsideWorldToBeRendered); ++i) {
        const int32 x = m_BlocksToBeRenderedOutsideWorldX[i], y = m_BlocksToBeRenderedOutsideWorldY[i];
        const auto& camera = TheCamera.GetPosition();
        const double dx = double(camera.x) - ((double(x) + 0.5) * 500.0 - 3000.0);
        const double dy = double(camera.y) - ((double(y) + 0.5) * 500.0 - 3000.0);
        const bool distant = std::sqrt(dx * dx + dy * dy) >= StaticRef<float>(0x8D3934);
        const auto segment = [&](float minX, float maxX, float minY, float maxY) {
            if (distant) plugin::Call<0x6E6870, int32, int32, float, float, float, float>(x, y, minX, maxX, minY, maxY);
            else plugin::Call<0x6E6A10, int32, int32, float, float, float, float>(x, y, minX, maxX, minY, maxY);
        };
        if (x < 0 || x > 11 || y < 0 || y > 11) segment(0.0f,1.0f,0.0f,1.0f);
        else {
            if (x == 0) segment(0.0f,0.04f,0.0f,1.0f);
            else if (x == 11) segment(0.96f,1.0f,0.0f,1.0f);
            if (y == 0) segment(0.0f,1.0f,0.0f,0.04f);
            else if (y == 11) segment(0.0f,1.0f,0.96f,1.0f);
        }
    }
    const auto flush = [] {
        if (uiTempBufferVerticesStored) {
            LittleTest();
            if (RwIm3DTransform(TempBufferVertices.m_3d, uiTempBufferVerticesStored, nullptr, rwIM3D_VERTEXUV)) {
                RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, uiTempBufferIndicesStored);
                RwIm3DEnd();
            }
        }
        uiTempBufferVerticesStored = 0; uiTempBufferIndicesStored = 0;
    };
    flush();
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(StaticRef<uint32>(0x8D3930)));
    WaterRenderShifts();
    const auto lowByte = [](float value) {return uint8(int64(value));};
    WaterColor.r = lowByte(StaticRef<float>(0xB7C508));
    WaterColor.g = lowByte(StaticRef<float>(0xB7C50C));
    WaterColor.b = lowByte(StaticRef<float>(0xB7C510));
    WaterColorTriangle = WaterColor;
    WaterLayerAlpha[1] = uint32(int64(double(StaticRef<float>(0xB7C514)) * 0.5));
    const int32 alpha = int32(WaterLayerAlpha[1]);
    const int32 combined = int32(uint32(alpha) << 8) / (256 - alpha);
    WaterLayerAlpha[0] = uint32(std::min(combined, 255));
    uiTempBufferVerticesStored = 0; uiTempBufferIndicesStored = 0;
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, waterclear256Raster);
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
    for (int32 i = 0; i < int32(NumWaterTriangles); ++i) {
        auto& triangle = WaterTriangles[i];
        if (!triangle.bToBeRendered) continue;
        const auto& a = m_aVertices[triangle.verts[0]], b = m_aVertices[triangle.verts[1]], c = m_aVertices[triangle.verts[2]];
        RenderWaterTriangle(a.x,a.y,a.rp,b.x,b.y,b.rp,c.x,c.y,c.rp);
        triangle.bToBeRendered = false;
    }
    for (int32 i = 0; i < int32(NumWaterQuads); ++i) {
        auto& quad = WaterQuads[i];
        if (!quad.bToBeRendered) continue;
        if (StaticRef<bool>(0xC228DC)) {
            WaterColor.r = lowByte(float((i & 15) * 15.9375f));
            WaterColor.g = lowByte(float(((i >> 4) & 15) * 15.9375f));
            WaterColor.b = lowByte(float(((i >> 8) & 15) * 15.9375f));
            WaterColor.a = lowByte(StaticRef<float>(0xB7C514));
            WaterColorTriangle = WaterColor;
        }
        const auto& a = m_aVertices[quad.verts[0]], b = m_aVertices[quad.verts[1]], c = m_aVertices[quad.verts[2]], d = m_aVertices[quad.verts[3]];
        RenderWaterRectangle(a.x,b.x,a.y,c.y,a.rp,b.rp,c.rp,d.rp);
        quad.bToBeRendered = false;
    }
    for (int32 i = 0; i < int32(m_NumBlocksOutsideWorldToBeRendered); ++i) {
        const int32 x = m_BlocksToBeRenderedOutsideWorldX[i], y = m_BlocksToBeRenderedOutsideWorldY[i];
        if (x >= 0 && x <= 11 && y >= 0 && y <= 11) continue;
        const int32 minX = x * 500 - 3000, minY = y * 500 - 3000;
        const CRenPar parameters{0.0f,1.0f,0.0f,0,0};
        RenderWaterRectangle(minX,minX+500,minY,minY+500,parameters,parameters,parameters,parameters);
    }
    flush();
    RenderBoatWakes();
    plugin::Call<0x734650>();
}

void CWaterLevel::SyncWater() {
    m_nWaterTimeOffset = CTimer::GetTimeInMS();
}

// NOTSA
bool CWaterLevel::IsPointUnderwaterNoWaves(const CVector& point) {
    float level{};
    if (GetWaterLevelNoWaves(point, &level, nullptr, nullptr))
        return level > point.z;
    return false;
}

bool CWaterLevel::GetWaterLevel(const CVector& pos, float& outWaterLevel, bool touchingWater, CVector* normals) {
    return GetWaterLevel(pos.x, pos.y, pos.z, outWaterLevel, touchingWater, normals);
}

// 0x6E5A40
uint32 CWaterLevel::AddWaterLevelVertex(int32 X, int32 Y, CRenPar P) {
    // Como el original (0x6E5A40): cada coordenada <= -3000 o >= 3000 (igualdad incluida) se fija en el limite y los
    // parametros pasan a {z=0, bigWaves=1.0f, smallWaves=0, flow=0} (mov [esp+1Ch],3F800000h tras fcomp 0x859A90/0x859A94).
    constexpr CRenPar CLAMPED{ 0.0f, 1.0f, 0.0f, 0, 0 };
    if (X <= -3000) { X = -3000; P = CLAMPED; }
    if (X >= 3000)  { X = 3000;  P = CLAMPED; }
    if (Y <= -3000) { Y = -3000; P = CLAMPED; }
    if (Y >= 3000)  { Y = 3000;  P = CLAMPED; }

    // Try finding a vertex with the same coords, and use that
    for (auto&& [id, vtx] : rngv::enumerate(m_aVertices | rng::views::take(NumWaterVertices))) {
        if (vtx.x == X && vtx.y == Y && vtx.rp.z == P.z) {
            return id;
        }
    }

    const auto idx = NumWaterVertices++;
    m_aVertices[idx] = { (int16)X, (int16)Y, P };
    return idx;
}

// 0x6E7EF0
void CWaterLevel::AddWaterLevelQuad(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3, int32 X4, int32 Y4, CRenPar P4, uint32 Flags) {
    if ((X1 == X2 && X1 == X3 && X1 == X4) || (Y1 == Y2 && Y1 == Y3 && Y1 == Y4)) {
        return;
    }

    // Como el original: los vertices se anaden en orden 1..4 y no hay asserts (el EXE tolera cuadrilateros no alineados).
    const uint32 v[4]{
        AddWaterLevelVertex(X1, Y1, P1),
        AddWaterLevelVertex(X2, Y2, P2),
        AddWaterLevelVertex(X3, Y3, P3),
        AddWaterLevelVertex(X4, Y4, P4),
    };

    // Como el original (0x6E7F... - 0x6E8165): los vertices con la Y minima van a la lista A y el resto a la B (en la pila
    // B sigue a A: lst[0..1] / lst[2..3]); cada pareja se ordena por X ascendente (cmp/jle con int16 con signo).
    const auto Y = [&](uint32 idx) { return m_aVertices[idx].y; };
    const auto X = [&](uint32 idx) { return m_aVertices[idx].x; };
    const int16 minY = std::min<int16>(std::min<int16>(Y(v[0]), Y(v[1])), std::min<int16>(Y(v[2]), Y(v[3])));
    uint32 lst[8]{};
    int ia = 0, ib = 2;
    for (const auto idx : v) {
        if (Y(idx) == minY) lst[ia++] = idx;
        else                lst[ib++] = idx;
    }
    if (X(lst[0]) > X(lst[1])) std::swap(lst[0], lst[1]);
    if (X(lst[2]) > X(lst[3])) std::swap(lst[2], lst[3]);

    // Como el original: solo se escriben los 4 indices y los 3 bits de flags (el resto de la palabra se conserva).
    auto& quad = WaterQuads[NumWaterQuads];
    for (int k = 0; k < 4; ++k) quad.verts[k] = uint16(lst[k]);
    quad.bToBeRendered = false;
    quad.bInvisible    = (Flags & 1) == 0;
    quad.bLimitedDepth = (Flags & 2) != 0;
    NumWaterQuads++;
}

// 0x6E7D40
void CWaterLevel::AddWaterLevelTriangle(int32 X1, int32 Y1, CRenPar P1, int32 X2, int32 Y2, CRenPar P2, int32 X3, int32 Y3, CRenPar P3, uint32 Flags) {
    if ((X1 == X2 && X1 == X3) || (Y1 == Y2 && Y1 == Y3)) {
        return;
    }

    // Como el original: sin asserts; los vertices se anaden en orden 1..3 y las coordenadas se leen del array.
    const uint32 v0 = AddWaterLevelVertex(X1, Y1, P1);
    const uint32 v1 = AddWaterLevelVertex(X2, Y2, P2);
    const uint32 v2 = AddWaterLevelVertex(X3, Y3, P3);
    const auto& a = m_aVertices[v0]; const auto& b = m_aVertices[v1]; const auto& c = m_aVertices[v2];

    uint32 i0, i1, i2;
    if (a.y == b.y) {
        if (a.x < b.x) { i0 = v0; i1 = v1; } else { i0 = v1; i1 = v0; }
        i2 = v2;
    } else if (a.y == c.y) {
        if (a.x < c.x) { i0 = v0; i1 = v2; } else { i0 = v2; i1 = v0; }
        i2 = v1;
    } else {
        if (b.x < c.x) { i0 = v1; i1 = v2; } else { i0 = v2; i1 = v1; }
        i2 = v0;
    }

    auto& tri = WaterTriangles[NumWaterTriangles];
    tri.verts[0] = uint16(i0); tri.verts[1] = uint16(i1); tri.verts[2] = uint16(i2);
    tri.bToBeRendered = false;
    tri.bInvisible    = (Flags & 1) == 0;
    tri.bLimitedDepth = (Flags & 2) != 0;
    NumWaterTriangles++;
}

void CWaterLevel::FillQuadsAndTrianglesList() {
    plugin::Call<0x6E7B30>();
}

// 0x6E9C80
void CWaterLevel::SetCameraRange() {
    if (DontUpdateCameraRange) {
        return;
    }

    const auto& cmpos = TheCamera.GetPosition();

    const auto CalcMin = [](float p) { return 2 * (int32)std::floor((p - (float)DETAILEDWATERDIST) / 2.f); };
    const auto CalcMax = [](float p) { return 2 * (int32)std::ceil((p + (float)DETAILEDWATERDIST) / 2.f); };

    CameraRangeMinX = CalcMin(cmpos.x);
    CameraRangeMaxX = CalcMax(cmpos.x);

    CameraRangeMinY = CalcMin(cmpos.y);
    CameraRangeMaxY = CalcMax(cmpos.y);
}

// 0x6EAB50
void CWaterLevel::HandleBeachToysStuff() {
    /* nothing special (10 lines), but it uses 3 static variables, and they aren't used anywhere else, so I won't bother */
}

template<size_t NumVerts>
CWaterVertex CWaterPolygon<NumVerts>::GetVertex(uint16 idx) const {
    return CWaterLevel::m_aVertices[verts[idx]];
}

// External collaborator; reconstruction is outside the current pending scope.
bool CWaterLevel::TestLineAgainstWater(CVector from, CVector to, CVector* intersection) {
    return plugin::CallAndReturn<bool, 0x6E61B0, CVector, CVector, CVector*>(from, to, intersection);
}

namespace {
// El EXE indexa m_aVertices con el indice de 16 bits extendido con signo (movsx; lea; shl 2) y sin comprobar el limite.
const WaterLevelVertexCpp* WaterVertexAt(uint16 index) {
    static_assert(sizeof(CWaterVertex) == sizeof(WaterLevelVertexCpp));
    return reinterpret_cast<const WaterLevelVertexCpp*>(CWaterLevel::m_aVertices.data()) + int16(index);
}
}

// 0x6e5bb0: lee el atlas real cargado; sin colaboradores. La aritmetica x87 (limites, u, v, mitad, interpolacion y prueba de profundidad) esta en X87Math (WaterLevelQuadCpp).
bool CWaterLevel::GetWaterLevelNoWavesForQuad(const CWaterQuad* quad, float x, float y, float z, float* outLevel, float* outBigWaves, float* outSmallWaves) {
    const WaterLevelVertexCpp* vertex[4]{ WaterVertexAt(quad->verts[0]), WaterVertexAt(quad->verts[1]), WaterVertexAt(quad->verts[2]), WaterVertexAt(quad->verts[3]) };
    return WaterLevelQuadCpp(vertex, quad->bLimitedDepth, x, y, z, outLevel, outBigWaves, outSmallWaves);
}

// 0x6e5e90: igual para triangulos (WaterLevelTriangleCpp).
bool CWaterLevel::GetWaterLevelNoWavesForTriangle(const CWaterTriangle* triangle, float x, float y, float z, float* outLevel, float* outBigWaves, float* outSmallWaves) {
    const WaterLevelVertexCpp* vertex[3]{ WaterVertexAt(triangle->verts[0]), WaterVertexAt(triangle->verts[1]), WaterVertexAt(triangle->verts[2]) };
    return WaterLevelTriangleCpp(vertex, triangle->bLimitedDepth, x, y, z, outLevel, outBigWaves, outSmallWaves);
}
