from pathlib import Path
import re
p=Path('source/game_sa/Camera.cpp');s=p.read_text(encoding='utf8');s=s.replace('// 0x50D160\n', '''namespace {
// Como el original: conservar fsin y las operaciones x87 en precisión del juego.
float CameraLinearComponent(float from, float to, float ratio, bool ease) {
    float value;
    if (ease) {
        const float degrees = 180.0f, startAngle = 270.0f;
        const float radians = std::bit_cast<float>(0x3C8EFA35u);
        const float one = 1.0f, half = 0.5f;
        __asm {
            fld ratio
            fmul degrees
            fsubr startAngle
            fmul radians
            fsin
            fadd one
            fmul half
            fld to
            fsub from
            fmulp st(1), st
            fadd from
            fstp value
        }
    } else {
        value = (to - from) * ratio + from;
    }
    return value;
}
float CameraLinearTime() {
    const auto time = CTimer::GetTimeInMS();
    const auto signedTime = int32(time);
    const float value = float(signedTime);
    return signedTime < 0 ? value + 4294967296.0f : value;
}
}

// 0x50D160
''')
replace={
'plugin::CallMethod<0x50D160, CCamera*, CVector*, CVector*, float, bool>(this, to, from, duration, bMoveLinearWithEase);':'''m_fMoveLinearStartTime = CameraLinearTime();
    m_fMoveLinearEndTime = m_fMoveLinearStartTime + duration;
    memcpy(&m_vecMoveLinearPosnStart, from, sizeof(CVector));
    memcpy(&m_vecMoveLinearPosnEnd, to, sizeof(CVector));
    m_bMoveLinearWithEase = bMoveLinearWithEase;''',
'plugin::CallMethod<0x50D1D0, CCamera*, CVector*, CVector*, float, bool>(this, to, from, duration, bEase);':'''m_fTrackLinearStartTime = CameraLinearTime();
    m_fTrackLinearEndTime = m_fTrackLinearStartTime + duration;
    memcpy(&m_vecTrackLinearEndPoint, from, sizeof(CVector));
    memcpy(&m_vecTrackLinearStartPoint, to, sizeof(CVector));
    m_bTrackLinearWithEase = bEase;''',
'plugin::CallMethod<0x516440, CCamera*>(this);':'''const auto now = CameraLinearTime();
    if (now <= m_fTrackLinearEndTime) {
        ProcessVectorTrackLinear((now - m_fTrackLinearStartTime) / (m_fTrackLinearEndTime - m_fTrackLinearStartTime));
    } else if (m_bCameraPersistTrack) {
        m_bVecTrackLinearProcessed = true;
    }''',
'plugin::CallMethod<0x5164A0, CCamera*>(this);':'''const auto now = CameraLinearTime();
    if (now <= m_fMoveLinearEndTime) {
        ProcessVectorMoveLinear((now - m_fMoveLinearStartTime) / (m_fMoveLinearEndTime - m_fMoveLinearStartTime));
    } else if (m_bCameraPersistPosition) {
        m_bVecMoveLinearProcessed = true;
    }''',
'plugin::CallMethod<0x50D350, CCamera*, float>(this, ratio);':'''m_bVecTrackLinearProcessed = true;
    m_vecTrackLinear.x = CameraLinearComponent(m_vecTrackLinearEndPoint.x, m_vecTrackLinearStartPoint.x, ratio, m_bTrackLinearWithEase);
    m_vecTrackLinear.y = CameraLinearComponent(m_vecTrackLinearEndPoint.y, m_vecTrackLinearStartPoint.y, ratio, m_bTrackLinearWithEase);
    m_vecTrackLinear.z = CameraLinearComponent(m_vecTrackLinearEndPoint.z, m_vecTrackLinearStartPoint.z, ratio, m_bTrackLinearWithEase);''',
'plugin::CallMethod<0x50D430, CCamera*, float>(this, ratio);':'''m_bVecMoveLinearProcessed = true;
    m_vecMoveLinear.x = CameraLinearComponent(m_vecMoveLinearPosnStart.x, m_vecMoveLinearPosnEnd.x, ratio, m_bMoveLinearWithEase);
    m_vecMoveLinear.y = CameraLinearComponent(m_vecMoveLinearPosnStart.y, m_vecMoveLinearPosnEnd.y, ratio, m_bMoveLinearWithEase);
    m_vecMoveLinear.z = CameraLinearComponent(m_vecMoveLinearPosnStart.z, m_vecMoveLinearPosnEnd.z, ratio, m_bMoveLinearWithEase);''',
'plugin::CallMethod<0x50D510, CCamera*, float>(this, ratio);':'''m_bFOVLerpProcessed = true;
    m_fFOVNew = CameraLinearComponent(m_fZoomInFactor, m_fZoomOutFactor, ratio, m_nZoomMode != 0);'''}
for a,b in replace.items():assert a in s;s=s.replace(a,b)
for addr in ['50D160','50D1D0','516440','5164A0','50D350','50D430','50D510']:
 s=re.sub(r'(RH_Scoped[^\n]*0x'+addr+r'[^\n]*?),\s*\{\s*\.reversed = false\s*\}',r'\1',s)
p.write_text(s,encoding='utf8')
