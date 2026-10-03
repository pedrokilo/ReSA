/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CBulletTraces
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
#include "BulletTraces.h"

void CBulletTraces::InjectHooks()
{
    RH_ScopedClass(CBulletTraces);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x721D50);
    RH_ScopedOverloadedInstall(AddTrace, "", 0x723750, void(*)(const CVector&, const CVector&, float, uint32, uint8));
    RH_ScopedOverloadedInstall(AddTrace, "Wrapper", 0x726AF0, void(*)(const CVector&, const CVector&, eWeaponType, CEntity*));
    RH_ScopedInstall(Render, 0x723C10);
    RH_ScopedInstall(Update, 0x723FB0);
}

// 0x721D50
void CBulletTraces::Init()
{
    for (CBulletTrace& trace : aTraces) {
        trace.m_bExists = false;
    }
}

// 0x723FB0
void CBulletTraces::Update()
{
    for (CBulletTrace& trace : aTraces) {
        trace.Update();
    }
}

CBulletTrace* CBulletTraces::GetFree() {
    for (CBulletTrace& trace : aTraces) {
        if (!trace.m_bExists) {
            return &trace;
        }
    }
    return nullptr;
}

// Como el original (inline en 0x7238F7): coordenadas en la matriz de camara (0xB6F99C) relativas a la posicion de
// TheCamera, con el orden de sumas del EXE; el volumen es log10 * 20 con fyl2x.
void PlayFrontEndSoundForTrace(const CVector& fromWorldSpace, const CVector& toWorldSpace) {
    const auto& camMat = StaticRef<CMatrix>(0xB6F99C);
    const CVector& right = camMat.GetRight();
    const CVector& fwd = camMat.GetForward();
    const CVector& up = camMat.GetUp();
    const CVector camPos = TheCamera.GetPosition();

    const CVector f{ fromWorldSpace.x - camPos.x, fromWorldSpace.y - camPos.y, fromWorldSpace.z - camPos.z };
    const CVector t{ toWorldSpace.x - camPos.x, toWorldSpace.y - camPos.y, toWorldSpace.z - camPos.z };
    const float fromX = f.z * right.z + f.y * right.y + f.x * right.x;
    const float fromZ = up.x * f.x + up.z * f.z + up.y * f.y;
    const float fromY = f.x * fwd.x + fwd.z * f.z + fwd.y * f.y;
    const float toX   = t.y * right.y + t.z * right.z + t.x * right.x;
    const float toZ   = up.z * t.z + up.y * t.y + up.x * t.x;
    const float toY   = fwd.z * t.z + fwd.y * t.y + fwd.x * t.x;

    if (!(toY * fromY < 0.f)) {
        return; // Ambos puntos delante o detras de la camara
    }
    // Como el original: valor absoluto con fchs condicional (el -0 se conserva).
    const float absFrom = fromY < 0.f ? -fromY : fromY;
    const float absTo   = toY < 0.f ? -toY : toY;
    const float interp  = absFrom / (absTo + absFrom);
    const float pointX  = (toX - fromX) * interp + fromX;
    const float pointZ  = (toZ - fromZ) * interp + fromZ;
    const float camToLineDist = std::sqrt(pointZ * pointZ + pointX * pointX);
    if (!(camToLineDist < 2.f)) {
        return;
    }

    const float factor = 1.f - camToLineDist * 0.5f;
    float volume = -100.f;
    if (factor != 0.f) {
        const float decibels = 20.f;
        __asm {
            fld factor
            fldlg2
            fxch st(1)
            fyl2x
            fmul decibels
            fstp volume
        }
    }
    eAudioEvents event;
    if (!(pointX < 0.f)) {
        event = fromY > 0.f ? AE_FRONTEND_BULLET_PASS_LEFT_FRONT : AE_FRONTEND_BULLET_PASS_LEFT_REAR;
    } else {
        event = fromY > 0.f ? AE_FRONTEND_BULLET_PASS_RIGHT_FRONT : AE_FRONTEND_BULLET_PASS_RIGHT_REAR;
    }
    AudioEngine.ReportFrontendAudioEvent(event, volume, 1.0f);
}

// 0x723750
void CBulletTraces::AddTrace(const CVector& from, const CVector& to, float radius, uint32 disappearTime, uint8 alpha)
{
    // Como el original: la vida depende de cuantas trazas existen ya (no del indice del hueco libre).
    int32 numExisting = 0;
    for (CBulletTrace& trace : aTraces) {
        if (trace.m_bExists) {
            ++numExisting;
        }
    }
    uint32 lifeTime;
    if (numExisting >= 10) {
        lifeTime = (uint32)(int64)((float)disappearTime * 0.25f);
    } else if (numExisting >= 5) {
        lifeTime = (uint32)(int64)((float)disappearTime * 0.5f);
    } else {
        lifeTime = disappearTime;
    }

    if (CBulletTrace* pTrace = GetFree()) {
        pTrace->m_vecStart = from;
        pTrace->m_vecEnd = to;
        pTrace->m_nCreationTime = CTimer::GetTimeInMS();
        pTrace->m_nTransparency = alpha;
        pTrace->m_bExists = true;
        pTrace->m_fRadius = radius;
        pTrace->m_nLifeTime = lifeTime;
    }
    PlayFrontEndSoundForTrace(from, to);
}

// 0x726AF0
void CBulletTraces::AddTrace(const CVector& posMuzzle, const CVector& posBulletHit, eWeaponType weaponType, CEntity* fromEntity)
{
    if (FindPlayerPed() == fromEntity || FindPlayerVehicle() && FindPlayerVehicle() == fromEntity) {
        switch (CCamera::GetActiveCamera().m_nMode) {
        case MODE_M16_1STPERSON:
        case MODE_SNIPER:
        case MODE_CAMERA:
        case MODE_ROCKETLAUNCHER:
        case MODE_ROCKETLAUNCHER_HS:
        case MODE_M16_1STPERSON_RUNABOUT:
        case MODE_SNIPER_RUNABOUT:
        case MODE_ROCKETLAUNCHER_RUNABOUT:
        case MODE_ROCKETLAUNCHER_RUNABOUT_HS:
        case MODE_HELICANNON_1STPERSON: {
            CPhysical* const player = FindPlayerVehicle() ? static_cast<CPhysical*>(FindPlayerVehicle()) : static_cast<CPhysical*>(FindPlayerPed());
            const CVector& speed = player->m_vecMoveSpeed;
            // Como el original: suma z + y + x
            if (std::sqrt(speed.z * speed.z + speed.y * speed.y + speed.x * speed.x) < 0.05f) {
                return;
            }
            break;
        }
        default:
            break;
        }
    }

    CVector dir{ posBulletHit.x - posMuzzle.x, posBulletHit.y - posMuzzle.y, posBulletHit.z - posMuzzle.z };
    const float traceLengthOriginal = std::sqrt(dir.z * dir.z + dir.y * dir.y + dir.x * dir.x);
    dir.Normalise();

    // Como el original: rand() * (1 / 32767) en linea; std::min evaluado como macro (segundo rand si gana el aleatorio).
    const float recip = std::bit_cast<float>(0x38000100u);
    const float traceLengthNew = (float)CGeneral::GetRandomNumber() * recip * traceLengthOriginal;
    const CVector from{ dir.x * traceLengthNew + posMuzzle.x, dir.y * traceLengthNew + posMuzzle.y, dir.z * traceLengthNew + posMuzzle.z };
    const float remaining = traceLengthOriginal - traceLengthNew;
    float fRadius = (float)CGeneral::GetRandomNumber() * recip * 3.f + 2.f;
    if (fRadius < remaining) {
        fRadius = (float)CGeneral::GetRandomNumber() * recip * 3.f + 2.f;
    } else {
        fRadius = remaining;
    }
    const CVector to{ dir.x * fRadius + from.x, dir.y * fRadius + from.y, dir.z * fRadius + from.z };

    AddTrace(
        from,
        to,
        0.01f,
        300,
        70
    );
}

// 0x723C10
void CBulletTraces::Render() {
    ZoneScoped;

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(RwBlendFunction::rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(RwBlendFunction::rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(RwCullMode::rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));

    RxObjSpace3DVertex verts[6];
    for (CBulletTrace& trace : aTraces) {
        if (!trace.m_bExists) {
            continue;
        }

        // Como el original: direcciones normalizadas con 0x59C910 y la traza reconstruida como dir * longitud * t.
        const CVector camPos = TheCamera.GetPosition();
        CVector camToOriginDir{ trace.m_vecStart.x - camPos.x, trace.m_vecStart.y - camPos.y, trace.m_vecStart.z - camPos.z };
        camToOriginDir.Normalise();
        CVector dir{ trace.m_vecEnd.x - trace.m_vecStart.x, trace.m_vecEnd.y - trace.m_vecStart.y, trace.m_vecEnd.z - trace.m_vecStart.z };
        const float length = std::sqrt(dir.z * dir.z + dir.y * dir.y + dir.x * dir.x);
        dir.Normalise();
        CVector up = camToOriginDir.Cross(dir);
        up.Normalise();

        // Time progress 1 => 0 as time passes
        // Como el original: enteros sin signo cargados con fild (+2^32) y una sola division redondeada.
        const uint32 elapsed = trace.GetRemainingLifetime(), lifeTime = trace.m_nLifeTime;
        const float twoPow32 = 4294967296.f, one = 1.f;
        float t;
        __asm {
            mov eax, elapsed
            fild elapsed
            test eax, eax
            jge trace_elapsed_ok
            fadd twoPow32
        trace_elapsed_ok:
            mov ecx, lifeTime
            fild lifeTime
            test ecx, ecx
            jge trace_life_ok
            fadd twoPow32
        trace_life_ok:
            fdivp st(1), st
            fsubr one
            fstp t
        }
        const auto alpha = (uint8)(int32)((float)trace.m_nTransparency * t);

        const float size = t * trace.m_fRadius;
        const CVector sizeVec{ up.x * size, up.y * size, up.z * size };
        const CVector currPosOnTrace{
            trace.m_vecEnd.x - dir.x * length * t,
            trace.m_vecEnd.y - dir.y * length * t,
            trace.m_vecEnd.z - dir.z * length * t,
        };
        const CVector vertPositions[std::size(verts)] = {
            currPosOnTrace,
            CVector{ sizeVec.x + currPosOnTrace.x, sizeVec.y + currPosOnTrace.y, sizeVec.z + currPosOnTrace.z },
            CVector{ currPosOnTrace.x - sizeVec.x, currPosOnTrace.y - sizeVec.y, currPosOnTrace.z - sizeVec.z },
            trace.m_vecEnd,
            CVector{ sizeVec.x + trace.m_vecEnd.x, sizeVec.y + trace.m_vecEnd.y, sizeVec.z + trace.m_vecEnd.z },
            CVector{ trace.m_vecEnd.x - sizeVec.x, trace.m_vecEnd.y - sizeVec.y, trace.m_vecEnd.z - sizeVec.z },
        };
        for (size_t idx = 0; idx < std::size(verts); ++idx) {
            RwV3dAssign(RwIm3DVertexGetPos(&verts[idx]), &vertPositions[idx]);
        }

        // Set colors
        for (auto& vert : verts) {
            RwIm3DVertexSetRGBA(&vert, 255, 255, 128, 0);
        }
        RwIm3DVertexSetRGBA(&verts[3], 255, 255, 128, alpha); // Only vertex 3 has non-zero alpha

        if (RwIm3DTransform(verts, std::size(verts), nullptr, rwIM3D_VERTEXRGBA)) {
            RwImVertexIndex indices[] = {
                // Each row represents a triangle
                4, 1, 3,
                1, 0, 3,
                0, 2, 3,
                3, 2, 5
            };
            RwIm3DRenderIndexedPrimitive(RwPrimitiveType::rwPRIMTYPETRILIST, indices, std::size(indices));
            RwIm3DEnd();
        }
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,     RWRSTATE(RwBlendFunction::rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,    RWRSTATE(RwBlendFunction::rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATECULLMODE,     RWRSTATE(RwCullMode::rwCULLMODECULLBACK));
}
