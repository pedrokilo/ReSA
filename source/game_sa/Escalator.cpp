/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEscalator
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

#include "Escalator.h"

void CEscalator::InjectHooks() {
    RH_ScopedClass(CEscalator);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x717110);
    RH_ScopedInstall(Destructor, 0x717130);

    RH_ScopedInstall(SwitchOff, 0x717860);
    RH_ScopedInstall(AddThisOne, 0x717970);
    RH_ScopedInstall(Update, 0x717D30);
}

// 0x717860
void CEscalator::SwitchOff() {
    // Como el original: bandera de depuración del EXE alrededor de cada borrado.
    auto& deletingEscalator = StaticRef<bool>(0xC6E98C);

    if (!m_StepObjectsCreated) {
        return;
    }

    // Como el original: total con signo releído en cada vuelta; solo los peldaños no nulos se quitan y se anulan.
    for (int32 i = 0; i < (int32)(m_NumIntermediatePlanes + m_NumBottomPlanes + m_NumTopPlanes); i++) {
        auto*& object = m_StepObjects.data()[i];
        if (!object) {
            continue;
        }

        CWorld::Remove(object);
        deletingEscalator = true;
        delete object;
        deletingEscalator = false;
        object = nullptr;
    }
    m_StepObjectsCreated = 0;
}

// 0x717970
void CEscalator::AddThisOne(const CVector& vecStart, const CVector& vecBottom, const CVector& vecTop, const CVector& vecEnd, bool moveDown, CEntity* entity) {
    m_Exist = true;

    m_Start  = vecStart;
    m_Bottom = vecBottom;
    m_Top    = vecTop;
    m_End    = vecEnd;

    // Como el original: solo se rebaja la z con la altura máxima de la caja del peldaño.
    const float stepHeight = CModelInfo::GetModelInfo(ModelIndices::MI_ESCALATORSTEP)->GetColModel()->GetBoundingBox().m_vecMax.z;
    m_Start.z  -= stepHeight;
    m_Bottom.z -= stepHeight;
    m_Top.z    -= stepHeight;
    m_End.z    -= stepHeight;

    // 0x717A14 - Como el original: (int)(d * 2.5 + 1.0) y 1 - (int)(d * -0.3125), con _ftol2 y sumas z, y, x.
    const CVector startBottom = m_Start - m_Bottom;
    const CVector bottomTop   = m_Bottom - m_Top;
    const CVector topEnd      = m_Top - m_End;
    const float distBottomTop = std::sqrt(bottomTop.z * bottomTop.z + bottomTop.y * bottomTop.y + bottomTop.x * bottomTop.x);
    m_NumIntermediatePlanes   = (int32)(int64)(distBottomTop * 2.5f + 1.0f);
    const float distStart     = std::sqrt(startBottom.z * startBottom.z + startBottom.y * startBottom.y + startBottom.x * startBottom.x);
    m_NumBottomPlanes         = 1 - (int32)(int64)(distStart * -0.3125f);
    const float distEnd       = std::sqrt(topEnd.z * topEnd.z + topEnd.y * topEnd.y + topEnd.x * topEnd.x);
    m_NumTopPlanes            = 1 - (int32)(int64)(distEnd * -0.3125f);

    // Como el original: dirección con los argumentos sin rebajar, CVector{dx, dy, 0} y CVector::Normalise.
    CVector dir{ vecStart.x - vecBottom.x, vecStart.y - vecBottom.y, 0.0f };
    dir.Normalise();
    m_Rotation.GetRight().Set(dir.y, -dir.x, 0.0f);
    m_Rotation.GetForward().Set(dir.x, dir.y, 0.0f);
    m_Rotation.GetUp().Set(0.0f, 0.0f, 1.0f);
    m_Rotation.GetPosition().Set(0.0f, 0.0f, 0.0f);
    m_MoveDown = moveDown;

    m_Bounding.m_vecCenter = CVector{ (m_End.x + m_Start.x) * 0.5f, (m_End.y + m_Start.y) * 0.5f, (m_End.z + m_Start.z) * 0.5f };
    // Como el original: puntero crudo y RegisterReference sin limpiar la referencia anterior ni comprobar nulo.
    m_Entity = entity;
    const CVector d = m_Start - m_Bounding.m_vecCenter;
    m_Bounding.m_fRadius = std::sqrt(d.z * d.z + d.y * d.y + d.x * d.x);
    entity->RegisterReference(&m_Entity);
}

// 0x717D30
void CEscalator::Update() {
    ZoneScoped;

    if (!m_StepObjectsCreated) {
        // Como el original: distancia a la cámara con suma (dx*dx + dy*dy) + dz*dz y radio + 20 estricto.
        const CVector& camPos = TheCamera.GetPosition();
        const float dz = camPos.z - m_Bounding.m_vecCenter.z;
        const float dy = camPos.y - m_Bounding.m_vecCenter.y;
        const float dx = camPos.x - m_Bounding.m_vecCenter.x;
        const float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
        if (m_Bounding.m_fRadius + 20.0f > dist) {
            // Como el original: huecos libres = tamaño - usados (bit 0x80 del estado), comparación con signo.
            auto* pool = reinterpret_cast<uint8*>(GetObjectPool());
            const auto* states = *reinterpret_cast<int8* const*>(pool + 4);
            const auto size = *reinterpret_cast<const int32*>(pool + 8);
            int32 used = 0;
            for (int32 i = 0; i < size; i++) {
                if (states[i] >= 0) {
                    used++;
                }
            }
            if (size - used > (int32)(m_NumBottomPlanes + m_NumIntermediatePlanes + m_NumTopPlanes) + 10) {
                m_StepObjectsCreated = true;
                m_CurrentPosition    = 0.0f;

                const auto CreateStep = [&](int32 index, ModelIndex model) {
                    auto*& obj = m_StepObjects.data()[index];
                    obj = new CObject(model, true);
                    if (obj) {
                        obj->SetPosn(m_Bottom);
                        CWorld::Add(obj);
                        obj->m_nObjectType = OBJECT_TYPE_DECORATION;
                    }
                };
                for (int32 i = 0; i < (int32)m_NumIntermediatePlanes; i++) {
                    CreateStep(i, ModelIndices::MI_ESCALATORSTEP);
                }
                for (int32 i = 0; i < (int32)(m_NumBottomPlanes + m_NumTopPlanes); i++) {
                    CreateStep((int32)m_NumIntermediatePlanes + i, ModelIndices::MI_ESCALATORSTEP8);
                }
            } else {
                NOTSA_LOG_WARN("Too many shit in the object pool now we can't create escalators!");
            }
        }
    }

    if (!m_StepObjectsCreated) {
        // still not created after all that effort!
        return; // 0x717F70 - Invert
    }

    // Como el original: paso = ts * 0.04 y la fracción se obtiene restando el valor truncado (_ftol2), no floor.
    const float step    = CTimer::GetTimeStep() * 0.04f;
    const float posStep = m_MoveDown ? m_CurrentPosition - step + 1.0f : step + m_CurrentPosition;
    m_CurrentPosition   = posStep - (float)(int32)(int64)posStep;

    // Como el original: matriz copiada sobre m_matrix (sin asignarla), posición y velocidad escritas directamente.
    const auto PlaceStep = [&](CObject* obj, const CVector& pos, const CVector& speed) {
        *static_cast<CMatrix*>(obj->m_matrix) = m_Rotation;
        obj->m_matrix->GetPosition() = pos;
        obj->GetMoveSpeed() = speed;
        obj->UpdateRwMatrix();
        obj->UpdateRwFrame();
        obj->RemoveAndAdd();
    };
    const auto StepSpeed = [&](const CVector& dir) {
        CVector speed{ dir.x * 0.016f, dir.y * 0.016f, dir.z * 0.016f };
        if (m_MoveDown) {
            speed = CVector{ -speed.x, -speed.y, -speed.z };
        }
        return speed;
    };

    // Intermedios: pos = top - dir * ((i - cur + 1) * 0.4), dir = normalizada(top - bottom).
    for (int32 i = 0; i < (int32)m_NumIntermediatePlanes; i++) {
        CVector dir = m_Top - m_Bottom;
        dir.Normalise();
        const CVector speed = StepSpeed(dir);
        if (auto* obj = m_StepObjects.data()[i]) {
            const float t = ((float)i - m_CurrentPosition + 1.0f) * 0.4f;
            PlaceStep(obj, CVector{ m_Top.x - dir.x * t, m_Top.y - dir.y * t, m_Top.z - dir.z * t }, speed);
        }
    }

    // Inferiores: pos = dir * ((8j + 4 + cur) * 0.4) + start, dir = normalizada(bottom - start).
    for (int32 j = 0, k = 4; j < (int32)m_NumBottomPlanes; j++, k += 8) {
        CVector dir = m_Bottom - m_Start;
        dir.Normalise();
        const CVector speed = StepSpeed(dir);
        if (auto* obj = m_StepObjects.data()[(int32)m_NumIntermediatePlanes + j]) {
            const float t = ((float)k + m_CurrentPosition) * 0.4f;
            PlaceStep(obj, CVector{ dir.x * t + m_Start.x, dir.y * t + m_Start.y, dir.z * t + m_Start.z }, speed);
        }
    }

    // Superiores: pos = dir * ((8j + 4 + cur) * 0.4) + top, dir = normalizada(end - top).
    for (int32 j = 0, k = 4; j < (int32)m_NumTopPlanes; j++, k += 8) {
        CVector dir = m_End - m_Top;
        dir.Normalise();
        const CVector speed = StepSpeed(dir);
        if (auto* obj = m_StepObjects.data()[(int32)(m_NumIntermediatePlanes + m_NumBottomPlanes) + j]) {
            const float t = ((float)k + m_CurrentPosition) * 0.4f;
            PlaceStep(obj, CVector{ dir.x * t + m_Top.x, dir.y * t + m_Top.y, dir.z * t + m_Top.z }, speed);
        }
    }

    // Out of sight - Como el original: (dz*dz + dy*dy) + dx*dx y radio + 23 estricto.
    const CVector& camPos = TheCamera.GetPosition();
    const float dx = camPos.x - m_Bounding.m_vecCenter.x;
    const float dy = camPos.y - m_Bounding.m_vecCenter.y;
    const float dz = camPos.z - m_Bounding.m_vecCenter.z;
    if (m_Bounding.m_fRadius + 23.0f < std::sqrt(dz * dz + dy * dy + dx * dx)) {
        SwitchOff();
    }
}
