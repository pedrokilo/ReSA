
// =========================================================================================================================================
// Ruta 0 (flightModel == 0, 0x6D8715..0x6D8B18). El juego nunca la usa. Tres fuerzas de giro proporcionales a la deriva lateral y a la velocidad al cuadrado,
// dos de empuje (ApplyMoveForce) y el amortiguamiento del giro sobre Y.
// =========================================================================================================================================
constexpr double kFlcD001      = 0.001;       // 0x871FB0
constexpr double kFlcD003      = 0.003;       // 0x871FA8
constexpr double kFlcD05       = 0.5;         // 0x859060
constexpr double kFlcD005      = 0.05;        // 0x859068
constexpr double kFlcD35       = 3.5;         // 0x871F98
constexpr double kFlcD0072     = 0.0072000003419816494;   // 0x871F90 (0x3F7D7DBF60000000)
constexpr double kFlcD09f      = 0.89999997615814209;     // 0x86C308 (0.9f como double)
constexpr float  kFlcMinus4    = -4.0f;       // 0x859C3C
constexpr float  kFlcMinus128th = -0.0078125f; // 0x871FA0
constexpr float  kFlcF001      = 0.001f;      // 0x858CDC
constexpr float  kFlcF002      = 0.002f;      // 0x858F44
constexpr float  kFlcF008      = 0.008f;      // 0x863984
constexpr float  kFlcTwo       = 2.0f;        // (inmediato 0x40000000)

void FlcRouteAirship(FlcFrame& f) {
    CVehicle& v = *f.v;
    FlcX87& fp = f.fp;
    const float* m = reinterpret_cast<const float*>(v.m_matrix);      // derecha m[0..2], adelante m[4..6], arriba m[8..10]
    float lat, c, scaleArg, right[3], fwd[3], t1[3], t2[3], t3[3];
    // deriva lateral normalizada: lat = -(vel . derecha) / |vel|
    fp.Fld(f.vel[2]); fp.Fmul(m[2]); fp.Fld(f.vel[1]); fp.Fmul(m[1]); fp.Arith('+', 1, 1, 0, true); fp.Fld(f.vel[0]); fp.Fmul(m[0]); fp.Arith('+', 1, 1, 0, true);
    fp.Fmul(kFlcMinusOne); fp.Fstp(lat);
    FlcVecMagnitude(fp, f.vel);
    fp.Fdivr(lat);
    memcpy(fwd, &m[4], 12);
    fp.Fstp(lat);
    // c = (|vel|^2 * (adelante . vel)) * (adelante . vel)
    fp.Fld(fwd[1]); fp.Fmul(f.vel[1]); fp.Fld(fwd[0]); fp.Fmul(f.vel[0]); fp.Arith('+', 1, 1, 0, true); fp.Fld(fwd[2]); fp.Fmul(f.vel[2]); fp.Arith('+', 1, 1, 0, true);
    fp.Fstp(c);
    FlcVecMagnitudeSquared(fp, f.vel);
    fp.Fmul(c);
    memcpy(right, &m[0], 12);
    fp.Fmul(c);
    fp.Fstp(c);
    // giro: fuerza = derecha * (((v494 * 0.001 + lat * 0.003) * dt * c) * turnMass) en el punto adelante * -4
    fp.Fld(FlcAt<float>(v, 0x494)); fp.FmulD(kFlcD001); fp.Fld(lat); fp.FmulD(kFlcD003); fp.Arith('+', 1, 1, 0, true);
    fp.Fmul(CTimer::ms_fTimeStep); fp.Fmul(c); fp.Fmul(v.m_fTurnMass); fp.Fstp(scaleArg);
    FlcVecScale(fp, right, scaleArg);                                                 // 0x40FEF0
    fp.Fld(fwd[0]); fp.Fmul(kFlcMinus4); fp.Fstp(fwd[0]);
    fp.Fld(fwd[1]); fp.Fmul(kFlcMinus4); fp.Fstp(fwd[1]);
    fp.Fld(fwd[2]); fp.Fmul(kFlcMinus4); fp.Fstp(fwd[2]);
    v.ApplyTurnForce(CVector{ right[0], right[1], right[2] }, CVector{ fwd[0], fwd[1], fwd[2] });   // 0x6D883E
    // empuje lateral: derecha * ((dt * masa * c * lat) * 0.2)
    fp.Fld(CTimer::ms_fTimeStep); fp.Fmul(v.m_fMass);
    memcpy(right, &m[0], 12);
    fp.Fmul(c); fp.Fmul(lat); fp.Fmul(kFlcTurnClamp); fp.Fstp(scaleArg);
    FlcVecScale(fp, right, scaleArg);
    v.ApplyMoveForce(CVector{ right[0], right[1], right[2] });                        // 0x6D889C
    FlcVecMulLeft(fp, t1, kFlcTwo, &m[8]);                                            // 0x40FE90: 2 * arriba
    v.ApplyTurnForce(CVector{ right[0], right[1], right[2] }, CVector{ t1[0], t1[1], t1[2] });       // 0x6D88DB
    // deriva vertical: lat = -(vel . arriba) / |vel|
    fp.Fld(f.vel[2]); fp.Fmul(m[10]); fp.Fld(f.vel[1]); fp.Fmul(m[9]); fp.Arith('+', 1, 1, 0, true); fp.Fld(f.vel[0]); fp.Fmul(m[8]); fp.Arith('+', 1, 1, 0, true);
    fp.Fmul(kFlcMinusOne); fp.Fstp(lat);
    FlcVecMagnitude(fp, f.vel);
    fp.Fdivr(lat);
    fp.Fstp(lat);
    fp.Fld(kFlcZero);
    if (f.pad) {
        fp.Pop();
        const int steer = f.pad->GetSteeringUpDown();                                 // 0x53FBD0 (int16 con signo)
        fp.FldI(steer); fp.Fmul(kFlcMinus128th);
    }
    fp.Fmul(kFlcF001);
    memcpy(right, &m[8], 12);
    fp.Fld(lat); fp.Fmul(kFlcF002); fp.Arith('+', 1, 1, 0, true);
    fp.Fmul(CTimer::ms_fTimeStep); fp.Fmul(c); fp.Fmul(v.m_fTurnMass); fp.Fstp(scaleArg);
    FlcVecScale(fp, right, scaleArg);
    v.ApplyTurnForce(CVector{ right[0], right[1], right[2] }, CVector{ fwd[0], fwd[1], fwd[2] });   // 0x6D89BE
    // empuje vertical: c = ((lat * 3.5 + 0.5) * masa * dt * c) * 0.05, limitado a dt * masa * 0.0072 si el techo esta por debajo y la fuerza es pequena
    fp.Fld(lat); fp.FmulD(kFlcD35);
    const uint8 statusBits = uint8(v.m_nTypeStatus >> 3);
    fp.FaddD(kFlcD05); fp.Fmul(v.m_fMass); fp.Fmul(CTimer::ms_fTimeStep); fp.Fmul(c); fp.FmulD(kFlcD005); fp.Fstp(c);
    if (statusBits == 0 || statusBits == 8) {
        fp.Fld(CTimer::ms_fTimeStep); fp.Fmul(v.m_fMass); fp.Fmul(kFlcF008); fp.FcomMem(c, true);
        if (!FlcParityEven(fp.Ah() & 5)) {                                            // jp 0x6D8A57
            const float height = v.HeightAboveCeiling(v.GetPosition().z, eFlightModel(0));            // 0x6D8A2F
            fp.FldRet(height); fp.FcomMem(kFlcZero, true);
            if (!(fp.Ah() & 0x41)) { fp.Fld(v.m_fMass); fp.Fmul(CTimer::ms_fTimeStep); fp.FmulD(kFlcD0072); fp.Fstp(c); }
        }
    }
    FlcVecMulLeft(fp, t1, c, &m[8]);                                                  // 0x6D8A6B: c * arriba
    FlcVecMulLeft(fp, t2, kFlcTwo, &m[8]);                                            // 0x6D8A95: 2 * arriba
    FlcVecAdd(fp, t3, t2, f.com);                                                     // 0x6D8AA6: punto = 2 * arriba + centro de masas
    v.ApplyMoveForce(CVector{ t1[0], t1[1], t1[2] });                                 // 0x6D8ACF
    v.ApplyTurnForce(CVector{ t1[0], t1[1], t1[2] }, CVector{ t3[0], t3[1], t3[2] }); // 0x6D8AFC
    fp.FldD(kFlcD09f); fp.Fld(CTimer::ms_fTimeStep); FlcCrtPow(fp);                   // 0.9 ^ dt
    fp.Fmul(v.m_vecTurnSpeed.y); fp.Fstp(v.m_vecTurnSpeed.y);
}
