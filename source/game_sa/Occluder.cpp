/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: COccluder
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "ActiveOccluder.h"
#include "Occluder.h"
#include "Occlusion.h"

void COccluder::InjectHooks() {
    RH_ScopedClass(COccluder);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ProcessOneOccluder, 0x71E5D0);
    RH_ScopedInstall(ProcessLineSegment, 0x71E130);
    RH_ScopedInstall(NearCamera, 0x71F960);
}

namespace {
// Como el original: MultiplyMatrixWithVector (0x59C890), con el orden de sumas del EXE
CVector MultiplyMatrixWithVector_59C890(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x + m.GetPosition().x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetPosition().y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetPosition().z,
    };
}

// Como el original: llama a la variante de 4 argumentos (0x71DA00) y descarta la escala
bool CalcScreenCoorsWithScale(const CVector& in, CVector& out) {
    float scaleX, scaleY;
    return CalcScreenCoors(in, out, scaleX, scaleY);
}

// Como el original: 0x71E130 calcula el mismo punto de corte con el plano cercano para cualquiera de los
// dos extremos: t = |z(to)| / (|z(from)| + |z(to)|) y punto = t * from + (1 - t) * to
bool CalcClippedScreenCoors(int32 idxFrom, int32 idxTo, CVector& out) {
    const auto& from = COccluder::OccluderCoors[idxFrom];
    const auto& to   = COccluder::OccluderCoors[idxTo];
    const auto& view = TheCamera.m_mViewMatrix;
    const float zA   = MultiplyMatrixWithVector_59C890(view, from).z - 1.1f;
    const float zB   = MultiplyMatrixWithVector_59C890(view, to).z - 1.1f;
    const float t    = std::fabs(zB) / (std::fabs(zA) + std::fabs(zB));
    const float u    = 1.f - t;
    const CVector pt{ t * from.x + u * to.x, t * from.y + u * to.y, t * from.z + u * to.z };
    return CalcScreenCoorsWithScale(pt, out);
}

// Como el original: devuelve el byte crudo de los angulos (movzx, sin signo)
uint8 RawAngle(const COccluder* o, size_t offset) {
    return reinterpret_cast<const uint8*>(o)[offset];
}
} // namespace

// 0x71E5D0
bool COccluder::ProcessOneOccluder(CActiveOccluder* out) {
    out->m_LinesUsed = 0;
    const auto center = CVector{m_Center};

    if (!CalcScreenCoorsWithScale(center, CenterOnScreen)) {
        return false;
    }
    if (CenterOnScreen.z < -150.0F || CenterOnScreen.z > 300.0F) {
        return false;
    }
    const auto size = GetSize();

    // Como el original: _ftol2 y se guarda la palabra baja
    out->m_DistToCam = (uint16)(int64)(CenterOnScreen.z - size.Magnitude());

    // Como el original: (RotY * RotX) * RotZ con los bytes sin signo por 2*pi/256 (0x859BBC)
    constexpr auto ANGLE_SCALE = std::bit_cast<float>(0x3CC90FDBu);
    CMatrix rotX, rotY, rotZ;
    rotX.SetRotateX((float)RawAngle(this, 0xE) * ANGLE_SCALE);
    rotY.SetRotateY((float)RawAngle(this, 0xD) * ANGLE_SCALE);
    rotZ.SetRotateZ((float)RawAngle(this, 0xC) * ANGLE_SCALE);
    const CMatrix transform = (rotY * rotX) * rotZ;

    MinXInOccluder = 999999.88F;
    MinYInOccluder = 999999.88F;
    MaxXInOccluder = -999999.88F;
    MaxYInOccluder = -999999.88F;

    if (size.x != 0 && size.y != 0.f && size.z != 0.f) {
        const auto up      = MultiplyMatrixWithVector_59C890(transform, CVector(0.0F, size.y / 2.0F, 0.0F));
        const auto right   = MultiplyMatrixWithVector_59C890(transform, CVector(size.x / 2.0F, 0.0F, 0.0F));
        const auto forward = MultiplyMatrixWithVector_59C890(transform, CVector(0.0F, 0.0F, size.z / 2.0F));

        enum {
            DUP,
            DBOTTOM,

            DRIGHT,
            DLEFT,

            DBACK,
            DFRONT,
        };
        const std::array directions{
            up, -up,
            right, -right,
            forward, -forward
        };

        // Figure out if we see the front or back of a face
        // Como el original: el orden de la suma del producto escalar depende de la cara
        const auto cam = TheCamera.GetPosition();
        const auto IsVertexOnScreen = [&](int32 i) {
            const auto& d = directions[i];
            const CVector delta = center + d - cam;
            float dot;
            switch (i) {
            case DUP:
            case DRIGHT:
                dot = delta.x * d.x + delta.y * d.y + delta.z * d.z;
                break;
            case DBOTTOM:
            case DLEFT:
                dot = delta.x * d.x + delta.z * d.z + delta.y * d.y;
                break;
            default:
                dot = d.z * delta.z + d.y * delta.y + delta.x * d.x;
                break;
            }
            return dot < 0.0F;
        };
        const std::array onScreen{
            IsVertexOnScreen(DUP),
            IsVertexOnScreen(DBOTTOM),

            IsVertexOnScreen(DRIGHT),
            IsVertexOnScreen(DLEFT),

            IsVertexOnScreen(DBACK),
            IsVertexOnScreen(DFRONT),
        };

        OccluderCoors[0] = center + directions[DUP]     + directions[DRIGHT] + directions[DBACK]; // back top right
        OccluderCoors[1] = center + directions[DBOTTOM] + directions[DRIGHT] + directions[DBACK]; // back bottom right
        OccluderCoors[2] = center + directions[DUP]     + directions[DLEFT]  + directions[DBACK]; // back top left
        OccluderCoors[3] = center + directions[DBOTTOM] + directions[DLEFT]  + directions[DBACK]; // back bottom left

        OccluderCoors[4] = center + directions[DUP]     + directions[DRIGHT] + directions[DFRONT]; // front top right
        OccluderCoors[5] = center + directions[DBOTTOM] + directions[DRIGHT] + directions[DFRONT]; // front bottom right
        OccluderCoors[6] = center + directions[DUP]     + directions[DLEFT]  + directions[DFRONT]; // front top left
        OccluderCoors[7] = center + directions[DBOTTOM] + directions[DLEFT]  + directions[DFRONT]; // front bottom left

        for (auto i = 0; i < 8; ++i) {
            OccluderCoorsValid[i] = CalcScreenCoorsWithScale(OccluderCoors[i], OccluderCoorsOnScreen[i]);
        }

        // Between two differently facing sides we see an edge, so process those
        if ((onScreen[DUP] == onScreen[DRIGHT] || !ProcessLineSegment(0, 4, out))
         && (onScreen[DUP] == onScreen[DLEFT] || !ProcessLineSegment(2, 6, out))
         && (onScreen[DUP] == onScreen[DBACK] || !ProcessLineSegment(0, 2, out))
         && (onScreen[DUP] == onScreen[DFRONT] || !ProcessLineSegment(4, 6, out))
         && (onScreen[DBOTTOM] == onScreen[DRIGHT] || !ProcessLineSegment(1, 5, out))
         && (onScreen[DBOTTOM] == onScreen[DLEFT] || !ProcessLineSegment(3, 7, out))
         && (onScreen[DBOTTOM] == onScreen[DBACK] || !ProcessLineSegment(1, 3, out))
         && (onScreen[DBOTTOM] == onScreen[DFRONT] || !ProcessLineSegment(5, 7, out))
         && (onScreen[DRIGHT] == onScreen[DBACK] || !ProcessLineSegment(0, 1, out))
         && (onScreen[DLEFT] == onScreen[DBACK] || !ProcessLineSegment(2, 3, out))
         && (onScreen[DLEFT] == onScreen[DFRONT] || !ProcessLineSegment(6, 7, out))
         && (onScreen[DRIGHT] == onScreen[DFRONT] || !ProcessLineSegment(4, 5, out))
        ) {
            // Como el original: solo se descarta si el minimo supera estrictamente al tamano (NaN pasa)
            if (!(SCREEN_WIDTH * 0.15F > MaxXInOccluder - MinXInOccluder)
             && !(SCREEN_HEIGHT * 0.1F > MaxYInOccluder - MinYInOccluder)
            ) {
                out->m_NumFaces = 0;
                for (auto i = 0; i < 6; ++i) {
                    if (!onScreen[i]) {
                        continue;
                    }
                    // Como el original: el desplazamiento usa center + direccion SIN normalizar
                    const auto& raw = directions[i];
                    const CVector pt{ raw.x + center.x, raw.y + center.y, raw.z + center.z };
                    CVector dir = raw;
                    dir.Normalise();

                    out->m_FaceNormals[out->m_NumFaces] = dir;
                    out->m_FaceOffsets[out->m_NumFaces] = pt.x * dir.x + dir.y * pt.y + dir.z * pt.z;

                    ++out->m_NumFaces;
                }
                return true;
            }
        }

        return false;
    }

    CVector right, up;
    if (size.y == 0.f) {
        right = CVector{size.x, 0.f, 0.f   };
        up    = CVector{0.f,   0.f, size.z };
    } else if (size.x == 0.f) {
        right = CVector{0.f, size.y, 0.f   };
        up    = CVector{0.f, 0.f,   size.z };
    } else if (size.z == 0.f) {
        right = CVector{0.f,    size.y, 0.f};
        up    = CVector{size.x , 0.f,   0.f};
    }
    right = MultiplyMatrixWithVector_59C890(transform, right / 2.f);
    up    = MultiplyMatrixWithVector_59C890(transform, up / 2.f);
    OccluderCoors[0] = center + right + up; // top right -- top right
    OccluderCoors[1] = center - right + up; // top left -- bottom right
    OccluderCoors[2] = center - right - up; // bottom left -- bottom left
    OccluderCoors[3] = center + right - up; // bottom right -- top left

    for (auto i = 0; i < 4; ++i) {
        OccluderCoorsValid[i] = CalcScreenCoorsWithScale(OccluderCoors[i], OccluderCoorsOnScreen[i]);
    }

    if (ProcessLineSegment(0, 1, out)
     || ProcessLineSegment(1, 2, out)
     || ProcessLineSegment(2, 3, out)
     || ProcessLineSegment(3, 0, out)
    )  {
        return false;
    }
    if (MaxXInOccluder - MinXInOccluder < SCREEN_WIDTH * 0.1F
     || MaxYInOccluder - MinYInOccluder < SCREEN_HEIGHT * 0.07F
    ) {
        return false;
    }

    // Como el original: la normal se normaliza (0x59C910) antes de guardarla
    auto normal = right.Cross(up);
    normal.Normalise();
    out->m_FaceNormals[0] = normal;
    out->m_FaceOffsets[0] = normal.x * center.x + normal.y * center.y + normal.z * center.z;
    out->m_NumFaces       = 1;

    return true;
}


// 0x71E130
bool COccluder::ProcessLineSegment(int32 idxFrom, int32 idxTo, CActiveOccluder* activeOccluder) {
    if (!OccluderCoorsValid[idxFrom] && !OccluderCoorsValid[idxTo]) {
        return true;
    }

    // Como el original: si un extremo no es valido se usa el corte con el plano cercano, calculado siempre
    // con (idxFrom, idxTo) para ambos extremos
    CVector from, to;
    if (OccluderCoorsValid[idxFrom]) {
        from = OccluderCoorsOnScreen[idxFrom];
    } else if (!CalcClippedScreenCoors(idxFrom, idxTo, from)) {
        return true;
    }
    if (OccluderCoorsValid[idxTo]) {
        to = OccluderCoorsOnScreen[idxTo];
    } else if (!CalcClippedScreenCoors(idxFrom, idxTo, to)) {
        return true;
    }

    // Como el original: min/max por comparaciones explicitas (los empates y NaN se quedan con el nuevo valor)
    const auto Min3 = [](float cur, float a, float b) { const float m = cur < a ? cur : a; return m < b ? m : b; };
    const auto Max3 = [](float cur, float a, float b) { const float m = cur > a ? cur : a; return m > b ? m : b; };
    MinXInOccluder = Min3(MinXInOccluder, from.x, to.x);
    MaxXInOccluder = Max3(MaxXInOccluder, from.x, to.x);
    MinYInOccluder = Min3(MinYInOccluder, from.y, to.y);
    MaxYInOccluder = Max3(MaxYInOccluder, from.y, to.y);

    auto* const l = &activeOccluder->m_Lines[activeOccluder->m_LinesUsed];

    // Calculate line origin and dir
    CVector2D origin{ from.x, from.y };
    CVector2D dir{ to.x - from.x, to.y - from.y };
    // Como el original: IsPointInsideLine en linea; si queda por debajo de 0, origen = from + dir y se invierte dir
    if ((CenterOnScreen.x - from.x) * dir.y - (CenterOnScreen.y - from.y) * dir.x < 0.f) {
        origin = CVector2D{ dir.x + from.x, from.y + dir.y };
        dir    = CVector2D{ -dir.x, -dir.y };
    }
    l->Length = std::sqrt(dir.y * dir.y + dir.x * dir.x);
    const float recip = 1.f / l->Length;
    l->Origin = origin;
    l->Dir    = CVector2D{ recip * dir.x, recip * dir.y };

    if (!DoesInfiniteLineTouchScreen(l->Origin, l->Dir)) {
        // Como el original: devuelve si el centro de la pantalla queda estrictamente por debajo de la linea
        return (SCREEN_WIDTH * 0.5f - origin.x) * l->Dir.y - (SCREEN_HEIGHT * 0.5f - origin.y) * l->Dir.x < 0.f;
    }

    ++activeOccluder->m_LinesUsed;
    return false;
}

// 0x71F960
bool COccluder::NearCamera() const {
    // Como el original: distancia con la suma z*z + y*y + x*x
    const CVector center = CVector{m_Center};
    const auto& cam = TheCamera.GetPosition();
    const CVector d{ cam.x - center.x, cam.y - center.y, cam.z - center.z };
    const float length = m_Length, width = m_Width;
    const float longest = length > width ? length : width;
    return std::sqrt(d.z * d.z + d.y * d.y + d.x * d.x) - longest * 0.5f < 250.f;
}

int16 COccluder::SetNext(int16 next) {
    const auto old = m_NextIndex;
    m_NextIndex    = next;
    return old;
}
