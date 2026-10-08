from pathlib import Path
p=Path('source/game_sa/Fx/FxEmitterBP.cpp');s=p.read_text().replace('#include "FxInfoManager.h"','#include "FxInfoManager.h"\n#include "MovementInfo.h"\n#include "WaterLevel.h"')
start=s.index('// 0x4A21D0');b=s.index('{',start);end=s.index('\n}',b)
body='''
    auto* system = emitter->m_System;
    const auto step = float(system->m_nTimeMult) * .001f * deltaTime;
    emitter->m_fCurrentLife += step;
    if (emitter->m_fCurrentLife >= emitter->m_fTotalLife) { return true; }
    emitter->m_Pos.x = step * emitter->m_Velocity.x + emitter->m_Pos.x;
    emitter->m_Pos.y = step * emitter->m_Velocity.y + emitter->m_Pos.y;
    emitter->m_Pos.z = step * emitter->m_Velocity.z + emitter->m_Pos.z;
    MovementInfo_t movement;
    movement.m_Pos = emitter->m_Pos;
    movement.m_Vel = emitter->m_Velocity;
    m_FxInfoManager.ProcessMovementInfo(system->m_fCurrentTime, emitter->m_fCurrentLife / emitter->m_fTotalLife, step, system->m_SystemBP->m_fLength, false, &movement);
    emitter->m_Pos = movement.m_Pos;
    emitter->m_Velocity = movement.m_Vel;
    if (movement.m_bHasFloatInfo || movement.m_bHasUnderwaterInfo) {
        float water = float(system->m_nTimeMult);
        const auto found = CWaterLevel::GetWaterLevel(emitter->m_Pos.x, emitter->m_Pos.y, emitter->m_Pos.z, water, 1, nullptr);
        if (movement.m_bHasFloatInfo && found && emitter->m_Pos.z < water) { emitter->m_Pos.z = water; }
        if (movement.m_bHasUnderwaterInfo && (!found || water < emitter->m_Pos.z)) { return true; }
    }
    const auto* bytes = reinterpret_cast<const uint8*>(emitter);
    const auto random = float(bytes[0x32]), multiplier = float(bytes[0x31]);
    const auto& rotations = movement.m_Rot;
    const auto positive = rotations[0] > 0.f || rotations[1] > 0.f;
    const auto negative = rotations[2] > 0.f || rotations[3] > 0.f;
    constexpr float byteScale = std::bit_cast<float>(0x3B808081u);
    if (!positive || !negative) {
        if (positive) {
            emitter->m_CurrentRotation = (((rotations[1] - rotations[0]) * random * byteScale + rotations[0]) * multiplier * step * byteScale) + emitter->m_CurrentRotation;
        } else if (negative) {
            const auto amount = ((rotations[3] - rotations[2]) * random * byteScale + rotations[2]) * multiplier;
            emitter->m_CurrentRotation -= amount * step * byteScale;
        }
    } else if (bytes[0x32] < 128) {
        emitter->m_CurrentRotation = (((rotations[1] - rotations[0]) * random * .0078125f + rotations[0]) * multiplier * step * byteScale) + emitter->m_CurrentRotation;
    } else {
        const auto amount = ((rotations[3] - rotations[2]) * (random - 128.f) * .0078125f + rotations[2]) * multiplier;
        emitter->m_CurrentRotation -= amount * step * byteScale;
    }
    return false;
'''
s=s[:b+1]+body+s[end:];p.write_text(s)
