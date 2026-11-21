#include "StdInc.h"

#include "Lines.h"
#include "X87Math.h"

void CLines::InjectHooks() {
    RH_ScopedClass(CLines);
    RH_ScopedCategoryGlobal();

    RH_ScopedOverloadedInstall(RenderLineWithClipping, "floats", 0x6FF4F0, void(*)(float, float, float, float, float, float, uint32, uint32)); // lote S2A (sin validar)
}

// 0x6FF460
void CLines::RenderLineNoClipping(float startX, float startY, float startZ, float endX, float endY, float endZ, uint32 startColor, uint32 endColor) {
    RxObjSpace3DVertex vertices[] = {
        { .objVertex = { startX, startY, startZ }, .color = startColor >> 8 | startColor << 24 },
        { .objVertex = { endX,   endY,   endZ   }, .color =   endColor >> 8 | endColor   << 24 }
    };

    LittleTest();
    if (RwIm3DTransform(vertices, 2u, nullptr, 0)) {
        RwIm3DRenderLine(0, 1);
        RwIm3DEnd();
    }
}

// 0x6FF4F0 (lote S2A, sin validar; cdecl, ocho argumentos de pila, 669 bytes): dibuja una linea 3D troceada en segmentos. Colores RGBA empaquetados (byte 3 = R, 2 = G, 1 = B, 0 = A).
// Aritmetica de la x87 a 24 bits con la pila sin guardar (Veh83D*): longitud = sqrt((dz*dz + dy*dy) + dx*dx) con d = inicio - fin; iters = longitud * 0.4 + 1.0, limitado a 7.0 (7.0 < iters);
// n = (int16)_ftol2(iters) (solo los 16 bits bajos). Con n > 0 (con signo de 16 bits) crea n pares de vertices en el buffer temporal (0xC4D958, 0x24 bytes por vertice): el par i interpola
// entre t0 = i / n y t1 = (i + 1) / n; posicion = float(delta * t + inicio) con delta = float(fin - inicio) por componente (x, y, z); cada canal de color = _ftol2(delta_canal * t + inicio_canal)
// (los canales y las diferencias son enteros exactos); el color del vertice (ARGB) = (c0 << 24) | (c3 << 16) | (c2 << 8) | c1 con cK = canal del byte K del color RGBA, es decir A, R, G, B.
// Despues LittleTest() (0x541330), RwIm3DTransform(buffer, 2 * n, NULL, 0) y, si tiene exito, RwIm3DRenderIndexedPrimitive(LINELIST, indices, 2 * n) y RwIm3DEnd(), con 2 * n = 2 * (int16)n aunque sea <= 0.
void CLines::RenderLineWithClipping(float startX, float startY, float startZ, float endX, float endY, float endZ, uint32 startColor, uint32 endColor) {
    const double dx = Veh83DSub(startX, endX);
    const double dy = Veh83DSub(startY, endY);
    const double dz = Veh83DSub(startZ, endZ);
    const double length = Veh83DSqrt(Veh83DAdd(Veh83DAdd(Veh83DMul(dz, dz), Veh83DMul(dy, dy)), Veh83DMul(dx, dx)));
    double iters = Veh83DAdd(Veh83DMul(length, 0.4f), 1.0);
    if (7.0 < iters) {
        iters = 7.0;
    }
    const int16 numVerts = static_cast<int16>(Heli83GFtol(iters));

    if (numVerts > 0) {
        const auto byteOf = [](uint32 color, int index) -> int32 { return static_cast<int32>((color >> (8 * index)) & 0xFFu); };
        const double count = numVerts;
        const double s0 = byteOf(startColor, 0), s1 = byteOf(startColor, 1), s2 = byteOf(startColor, 2), s3 = byteOf(startColor, 3);
        const double d0 = byteOf(endColor, 0) - byteOf(startColor, 0);
        const double d1 = byteOf(endColor, 1) - byteOf(startColor, 1);
        const double d2 = byteOf(endColor, 2) - byteOf(startColor, 2);
        const double d3 = byteOf(endColor, 3) - byteOf(startColor, 3);
        const float deltaX = Veh83DStore(Veh83DSub(endX, startX));
        const float deltaY = Veh83DStore(Veh83DSub(endY, startY));
        const float deltaZ = Veh83DStore(Veh83DSub(endZ, startZ));

        const auto writeVertex = [&](RxObjSpace3DVertex& vertex, double t) {
            const auto channel = [&](double delta, double start) { return static_cast<uint32>(Heli83GFtol(Veh83DAdd(Veh83DMul(delta, t), start))) & 0xFFu; };
            const uint32 c0 = channel(d0, s0);
            const uint32 c3 = channel(d3, s3);
            const uint32 c2 = channel(d2, s2);
            const uint32 c1 = channel(d1, s1);
            vertex.color = (c0 << 24) | (c3 << 16) | (c2 << 8) | c1;
            vertex.objVertex.x = Veh83DStore(Veh83DAdd(Veh83DMul(deltaX, t), startX));
            vertex.objVertex.y = Veh83DStore(Veh83DAdd(Veh83DMul(deltaY, t), startY));
            vertex.objVertex.z = Veh83DStore(Veh83DAdd(Veh83DMul(deltaZ, t), startZ));
        };
        for (int32 i = 0; i < numVerts; ++i) {
            const double t0 = Veh83DDiv(i, count);
            const double t1 = Veh83DDiv(i + 1, count);
            writeVertex(TempBufferVertices.m_3d[2 * i], t0);
            writeVertex(TempBufferVertices.m_3d[2 * i + 1], t1);
        }
    }

    static RwImVertexIndex indices[] = { // 0x8D503C
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
        12, 13, 14, 15, 16, 17, 18, 19, 20, 0
    };
    LittleTest();
    const int32 numRenderVerts = 2 * static_cast<int32>(numVerts);
    if (RwIm3DTransform(TempBufferVertices.m_3d, static_cast<RwUInt32>(numRenderVerts), nullptr, 0)) {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, numRenderVerts);
        RwIm3DEnd();
    }
}

// 0x6FF790
void CLines::ImmediateLine2D(int32 startX, int32 startY, int32 endX, int32 endY, uint8 startR, uint8 startG, uint8 startB, uint8 startA, uint8 endR, uint8 endG, uint8 endB, uint8 endA) {
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));

    RwIm2DVertex vertices[] = {
        { .x = float(startX), .y = float(startY), .emissiveColor = CRGBA(startR, startG, startB, startA).ToIntARGB() },
        { .x = float(endX),   .y = float(endY),   .emissiveColor = CRGBA(endR, endG, endB, endA).ToIntARGB() }
    };
    RwIm2DRenderLine(vertices, 2, 0, 1);

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
}
