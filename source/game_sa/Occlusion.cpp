/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: COcclusion
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Occlusion.h"
#include "Occluder.h"
#include "ActiveOccluder.h"

void COcclusion::InjectHooks() {
    RH_ScopedClass(COcclusion);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x71DCA0);
    RH_ScopedInstall(AddOne, 0x71DCD0);
    RH_ScopedInstall(IsPositionOccluded, 0x7200B0);
    RH_ScopedInstall(OccluderHidesBehind, 0x71E080);
    RH_ScopedInstall(ProcessBeforeRendering, 0x7201C0);
}

// 0x71DCA0
void COcclusion::Init() {
    ZoneScoped;

    NumOccludersOnMap         = 0;
    NumInteriorOccludersOnMap = 0;
    FarAwayList               = -1;
    NearbyList                = -1;
    ListWalkThroughFA         = -1;
    PreviousListWalkThroughFA = -1;
}

// 0x71DCD0
// Yes, rotation is z, y, x
void COcclusion::AddOne(float centerX, float centerY, float centerZ, float width, float length, float height, float rotZ, float rotY, float rotX, uint32 flags, bool isInterior) {
    // Como el original: tamaños truncados con _ftol2 y se descarta si al menos dos son cero.
    const auto Trunc = [](float v) { return (int32)(int64)v; };
    const int32 l = Trunc(std::fabs(length));
    const int32 w = Trunc(std::fabs(width));
    const int32 h = Trunc(std::fabs(height));
    if ((l == 0) + (w == 0) + (h == 0) > 1) {
        return;
    }

    // Como el original: ángulo llevado a [0, 360] (sin sumar 180), a radianes con 0x3C8EFA35 y a byte con 0x4222F983.
    const auto Rad = [](float a) {
        while (a < 0.0f) {
            a += 360.0f;
        }
        while (a > 360.0f) {
            a -= 360.0f;
        }
        return a * std::bit_cast<float>(0x3C8EFA35u);
    };
    const float rz = Rad(rotZ);
    const float ry = Rad(rotY);
    const float rx = Rad(rotX);

    // Como el original: tablas acotadas (1000 / 40) con contadores con signo.
    int32 index;
    COccluder* occl;
    if (isInterior) {
        index = (int32)NumInteriorOccludersOnMap;
        if (index >= MAX_INTERIOR_OCCLUDERS) {
            return;
        }
        occl = &InteriorOccluders.data()[index];
    } else {
        index = (int32)NumOccludersOnMap;
        if (index >= MAX_MAP_OCCLUDERS) {
            return;
        }
        occl = &Occluders.data()[index];
    }

    const float scale = std::bit_cast<float>(0x4222F983u);
    auto* packed = reinterpret_cast<int16*>(occl);
    packed[0] = (int16)Trunc(centerX * 4.0f);
    packed[1] = (int16)Trunc(centerY * 4.0f);
    packed[2] = (int16)Trunc(centerZ * 4.0f);
    packed[3] = (int16)Trunc((float)l * 4.0f);
    packed[4] = (int16)Trunc((float)w * 4.0f);
    packed[5] = (int16)Trunc((float)h * 4.0f);
    auto* rot = reinterpret_cast<int8*>(occl) + 0xC;
    rot[2] = (int8)Trunc(rx * scale);
    rot[1] = (int8)Trunc(ry * scale);
    rot[0] = (int8)Trunc(rz * scale);

    if (isInterior) {
        NumInteriorOccludersOnMap = index + 1;
    } else {
        occl->m_DontStream = flags != 0;
        occl->m_NextIndex  = FarAwayList;
        FarAwayList        = (int16)index;
        NumOccludersOnMap  = index + 1;
    }
}

// 0x71E080
bool COcclusion::OccluderHidesBehind(CActiveOccluder* first, CActiveOccluder* second) {
    // Como el original: contadores de líneas con signo (movsx) y prueba "no por debajo de 0" en línea (NaN pasa).
    if (!first->m_LinesUsed) {
        return second->m_LinesUsed == 0;
    }
    for (int32 a = 0; a < (int32)(int8)first->m_LinesUsed; ++a) {
        const auto& lineA = first->m_Lines.data()[a];
        for (int32 b = 0; b < (int32)(int8)second->m_LinesUsed; ++b) {
            const auto& lineB = second->m_Lines.data()[b];

            if ((lineA.Origin.x - lineB.Origin.x) * lineB.Dir.y - (lineA.Origin.y - lineB.Origin.y) * lineB.Dir.x < 0.0f) {
                return false;
            }
            const float endX = lineA.Dir.x * lineA.Length + lineA.Origin.x;
            const float endY = lineA.Dir.y * lineA.Length + lineA.Origin.y;
            if ((endX - lineB.Origin.x) * lineB.Dir.y - (endY - lineB.Origin.y) * lineB.Dir.x < 0.0f) {
                return false;
            }
        }
    }
    return true;
}

// 0x7200B0
bool COcclusion::IsPositionOccluded(CVector pos, float radius) {
    if (!NumActiveOccluders) {
        return false;
    }

    CVector scrPos;
    float   scaleX, scaleY;
    if (!CalcScreenCoors(pos, scrPos, scaleX, scaleY)) {
        return false;
    }

    // Como el original: se toma scaleX solo si es estrictamente mayor; distancia a cámara con signo y estricta.
    const float longEdge     = scaleX > scaleY ? scaleX : scaleY;
    const float screenRadius = longEdge * radius;
    const float screenDepth  = scrPos.z - radius;
    for (int32 i = 0; i < (int32)NumActiveOccluders; i++) {
        const auto& o = ActiveOccluders.data()[i];
        if ((float)(int16)o.m_DistToCam < screenDepth
            && o.IsPointWithinOcclusionArea(scrPos, screenRadius)
            && o.IsPointBehindOccluder(pos, radius)) {
            return true;
        }
    }
    return false;
}

// 0x7201C0
void COcclusion::ProcessBeforeRendering() {
    NumActiveOccluders = 0;

    // Como el original: con el área exterior se recorren las listas (siempre primero la lejana y después la cercana)
    // y los interiores solo se procesan cuando no se ve el exterior.
    if (CGame::currArea == AREA_CODE_NORMAL_WORLD) {
        // @ 0x72021A - Update far-away list
        if (ListWalkThroughFA == -1) {
            PreviousListWalkThroughFA = -1;
            ListWalkThroughFA         = FarAwayList;
        }
        for (int32 size = 0; ListWalkThroughFA != -1;) { // We do at most 16 iterations!
            auto& occluder = Occluders.data()[ListWalkThroughFA];

            // If the occluder is now near move it into the (nearby) list
            if (occluder.NearCamera()) { // 0x72022E
                const int16 current = ListWalkThroughFA;
                if (PreviousListWalkThroughFA == -1) {
                    FarAwayList = occluder.GetNext();
                } else {
                    Occluders.data()[PreviousListWalkThroughFA].SetNext(occluder.GetNext());
                }
                const int16 next = occluder.GetNext();
                Occluders.data()[current].SetNext(NearbyList);
                NearbyList        = current;
                ListWalkThroughFA = next;
            } else {
                PreviousListWalkThroughFA = ListWalkThroughFA;
                ListWalkThroughFA         = occluder.GetNext();
            }
            if (++size >= NUM_OCCLUDERS_PROCESSED_PER_FRAME) {
                break;
            }
        }

        // @ 0x720307 - Update nearby list
        for (int16 i = NearbyList, prev = -1; i != -1;) {
            // 0x720323 - Process this occluder
            if ((int32)NumActiveOccluders < MAX_ACTIVE_OCCLUDERS) {
                if (Occluders.data()[i].ProcessOneOccluder(&ActiveOccluders.data()[NumActiveOccluders])) {
                    NumActiveOccluders++;
                }
            }

            auto& occluder = Occluders.data()[i];
            // If the occluder is now far away move it into the faraway list
            if (!occluder.NearCamera()) { // 0x72035A (inverted!)
                const int16 current = i;
                if (prev == -1) {
                    NearbyList = occluder.GetNext();
                } else {
                    Occluders.data()[prev].SetNext(occluder.GetNext());
                }
                i = occluder.GetNext();
                Occluders.data()[current].SetNext(FarAwayList);
                FarAwayList = current;
            } else { // 0x7203D6
                prev = i;
                i    = occluder.GetNext();
            }
        }
    } else {
        // 0x7203FC - Process interior occluders
        for (int32 i = 0; i < (int32)NumInteriorOccludersOnMap; i++) {
            if ((int32)NumActiveOccluders < MAX_ACTIVE_OCCLUDERS) {
                if (InteriorOccluders.data()[i].ProcessOneOccluder(&ActiveOccluders.data()[NumActiveOccluders])) {
                    NumActiveOccluders++;
                }
            }
        }
    }

    // 0x72043A - Remove occluded occluders
    // Como el original: borrado inmediato desplazando el resto (rep movs) y se reexamina el mismo índice;
    // distancias comparadas como int16.
    int32 count = (int32)NumActiveOccluders;
    for (int32 o = 0; o < count; o++) {
        for (int32 i = 0; i < count; i++) {
            auto* const occluder = &ActiveOccluders.data()[o];
            if (o == i) {
                continue;
            }
            auto* const other = &ActiveOccluders.data()[i];
            if ((int16)other->m_DistToCam >= (int16)occluder->m_DistToCam || !OccluderHidesBehind(occluder, other)) {
                continue;
            }
            const int32 last = count - 1;
            if (o < last) {
                std::memmove(occluder, occluder + 1, (size_t)(last - o) * sizeof(CActiveOccluder));
            }
            count              = last;
            NumActiveOccluders = count;
            o--;
            break;
        }
    }
}
