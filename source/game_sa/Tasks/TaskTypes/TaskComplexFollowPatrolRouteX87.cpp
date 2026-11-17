// Archivo propio de CTaskComplexFollowPatrolRoute::CreateFirstSubTaskBody (0x670E20): aritmetica x87 del EXE (24 bits, intermedios en registros con exponente ampliado, un SSE con DAZ/FTZ aplana los denormales que la x87
// conserva); se compila con /O2 /Ob0 /Oi /arch:IA32 /fp:fast (ver source/CMakeLists.txt) para que el resto de la clase conserve la aritmetica normal. Su hook sigue en TaskComplexFollowPatrolRoute.cpp.
#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)

#include "TaskComplexFollowPatrolRoute.h"
#include "PatrolRoute.h"
#include "Entity/Ped/Ped.h"

#pragma float_control(precise, on, push)
// 0x670E20 (lote 210A, sin validar; thiscall, ped en la pila, `ret 4`, 968 bytes; ranura 9). Como el original: apaga el bit 0 de las banderas (+0x20). Ruta vacia (contador 0) -> CreateSubTask(TASK_NONE) sin mas (un contador
// negativo, con signo, solo se salta los bucles). Busca el nodo siguiente al segmento de la ruta mas cercano al ped (posicion = matriz si hay, si no la posicion simple). Los intermedios son `double`: con la palabra de control
// de 24 bits del juego cada operacion se redondea a 24 bits y el `double` conserva el valor sin redondearlo al rango de un float; solo se guarda como float lo que el EXE guarda en memoria (d, longitud al cuadrado, t, dz*t, qx,
// ex, ey y la distancia al cuadrado del mejor); qy, qz, ez y las distancias de la segunda pasada quedan en registros.
//  1. Por cada segmento i (de B = punto i a A = punto (i+1) % n): d = A - B, longitud al cuadrado = (z*z + x*x) + y*y, d normalizado con CVector::Normalise (0x59C910); r = ped - B; t = (rz*dz + rx*dx) + ry*dy. Solo cuenta si
//     t > 0 y sqrt(longitud2) > t (el t guardado como float; comparaciones ordenadas: NaN descarta). Pie Q = d*t + B y D = (ez*ez + ey*ey) + ex*ex con e = ped - Q. Si D < mejor*mejor: mejor = float(D), candidato = (i+1) % n.
//  2. Sin candidato: por punto, D = (dz*dz + dy*dy) + dx*dx con d = ped - punto; mismo criterio con mejor inicial FLT_MAX (en registro, luego el float guardado de D).
//  3. Nodo actual (+0x12, 16 bits) = candidato. Bit 1 de las banderas: se apaga y CreateSubTask(TASK_COMPLEX_FOLLOW_NODE_ROUTE); si no se apaga y CreateSubTask(GetNextSubTaskType()).
__declspec(noinline) CTask* CTaskComplexFollowPatrolRoute::CreateFirstSubTaskBody(CPed* ped) {
    m_nFlags &= ~1u;
    const int32 nodeCount = static_cast<int32>(m_pRoute->m_nNumNodes);
    if (nodeCount == 0) {
        return CreateSubTask(TASK_NONE);
    }

    const CVector& pedPos = ped->GetPosition();
    const double   px = pedPos.x, py = pedPos.y, pz = pedPos.z;
    int32 closest = -1;
    float best    = FLT_MAX; // 0x7F7FFFFF

    for (int32 i = 0; i < nodeCount; ++i) {
        const int32    next = (i + 1) % nodeCount;
        const CVector& to   = m_pRoute->m_aPoints[next];
        const CVector& from = m_pRoute->m_aPoints[i % nodeCount];

        CVector dir;
        dir.x = static_cast<float>(static_cast<double>(to.x) - static_cast<double>(from.x));
        dir.y = static_cast<float>(static_cast<double>(to.y) - static_cast<double>(from.y));
        const double dzValue = static_cast<double>(to.z) - static_cast<double>(from.z);
        dir.z = static_cast<float>(dzValue);
        double lengthSqValue = dzValue * static_cast<double>(dir.z);
        lengthSqValue        = lengthSqValue + static_cast<double>(dir.x) * static_cast<double>(dir.x);
        lengthSqValue        = lengthSqValue + static_cast<double>(dir.y) * static_cast<double>(dir.y);
        const float lengthSq = static_cast<float>(lengthSqValue);
        dir.Normalise();

        const double rx     = px - static_cast<double>(from.x);
        const double ry     = py - static_cast<double>(from.y);
        const double rz     = pz - static_cast<double>(from.z);
        double       tValue = rz * static_cast<double>(dir.z);
        tValue              = tValue + rx * static_cast<double>(dir.x);
        tValue              = tValue + ry * static_cast<double>(dir.y);
        const float t       = static_cast<float>(tValue);
        if (!(tValue > 0.0)) {
            continue;
        }
        if (!(std::sqrt(static_cast<double>(lengthSq)) > static_cast<double>(t))) {
            continue;
        }

        const double xt     = static_cast<double>(dir.x) * static_cast<double>(t);
        const double yt     = static_cast<double>(dir.y) * static_cast<double>(t);
        const double zt     = static_cast<double>(dir.z) * static_cast<double>(t);
        const float  ztb    = static_cast<float>(zt);
        const double qx     = xt + static_cast<double>(from.x);
        const float  qxb    = static_cast<float>(qx);
        const double qy     = yt + static_cast<double>(from.y);
        const double qz     = static_cast<double>(ztb) + static_cast<double>(from.z);
        const double ex     = px - static_cast<double>(qxb);
        const float  exb    = static_cast<float>(ex);
        const double ey     = py - qy;
        const float  eyb    = static_cast<float>(ey);
        const double ez     = pz - qz;
        double       distSq = ez * ez;
        distSq              = distSq + static_cast<double>(eyb) * static_cast<double>(eyb);
        distSq              = distSq + static_cast<double>(exb) * static_cast<double>(exb);
        const double bestSq = static_cast<double>(best) * static_cast<double>(best);
        if (distSq < bestSq) {
            best    = static_cast<float>(distSq);
            closest = next;
        }
    }

    if (closest == -1) {
        double bestPoint = static_cast<double>(FLT_MAX);
        for (int32 i = 0; i < nodeCount; ++i) {
            const CVector& point  = m_pRoute->m_aPoints[i];
            const double   dx     = px - static_cast<double>(point.x);
            const double   dy     = py - static_cast<double>(point.y);
            const double   dz     = pz - static_cast<double>(point.z);
            double         distSq = dz * dz;
            distSq                = distSq + dy * dy;
            distSq                = distSq + dx * dx;
            const float    stored = static_cast<float>(distSq);
            const double   bestSq = bestPoint * bestPoint;
            if (distSq < bestSq) {
                bestPoint = static_cast<double>(stored);
                closest   = i;
            }
        }
    }

    const uint32 flags = m_nFlags;
    m_nCurrentNode     = static_cast<int16>(closest);
    if (flags & 2u) {
        m_nFlags = flags & ~2u;
        return CreateSubTask(TASK_COMPLEX_FOLLOW_NODE_ROUTE);
    }
    m_nFlags = flags & ~2u;
    return CreateSubTask(GetNextSubTaskType());
}
#pragma float_control(pop)
