}  // namespace

// 0x6D85F0
void CVehicle::FlyingControl(eFlightModel flightModel, float leftRightSkid, float steeringUpDown, float steeringLeftRight, float accelerationBreakStatus) {
    FlcFrame f{ this, FlcX87{}, m_pFlyingHandlingData, flightModel, leftRightSkid, steeringUpDown, steeringLeftRight, accelerationBreakStatus, nullptr, {}, {} };
    FlcX87& fp = f.fp;
    if (!f.fh) { fp.Commit(); return; }                            // 0x6D8604: sin datos de vuelo
    fp.Fld(CTimer::ms_fTimeStep); fp.FcomMem(kFlcZero, true);
    if (!FlcParityEven(fp.Ah() & 0x41)) { fp.Commit(); return; }   // 0x6D861B: jnp, sale con tiempo <= 0
    // 0x6D8621: el mando solo se lee con el vehiculo del jugador y un conductor jugador
    if (GetStatus() == STATUS_PLAYER && m_pDriver && m_pDriver->IsPlayer())
        f.pad = m_pDriver->AsPlayer()->GetPadFromPlayer();
    // 0x6D8652: velocidad + deriva del viento (un vehiculo de mision no la tiene)
    float wind[3] = { 0.f, 0.f, 0.f };
    f.vel[1] = 0.f; f.vel[2] = 0.f;
    fp.Fld(kFlcZero);
    if (m_nCreatedBy != 2) {
        fp.Pop();
        fp.Fld(f.fh->m_fWindMult);
        fp.Fld(CWeather::WindDir.x); fp.Arith('*', 0, 0, 1, false); fp.Fstp(wind[0]);
        fp.Fld(CWeather::WindDir.y); fp.Arith('*', 0, 0, 1, false); fp.Fstp(wind[1]);
        fp.Fmul(CWeather::WindDir.z); fp.Fstp(wind[2]);
        fp.Fld(wind[0]); fp.Fchs();
        fp.Fld(wind[1]); fp.Fchs(); fp.Fstp(f.vel[1]);
        fp.Fld(wind[2]); fp.Fchs(); fp.Fstp(f.vel[2]);
    }
    fp.Fadd(m_vecMoveSpeed.x); fp.Fstp(f.vel[0]);
    fp.Fld(f.vel[1]); fp.Fadd(m_vecMoveSpeed.y); fp.Fstp(f.vel[1]);
    fp.Fld(f.vel[2]); fp.Fadd(m_vecMoveSpeed.z); fp.Fstp(f.vel[2]);
    FlcMatMul3x3(fp, f.com, reinterpret_cast<const float*>(m_matrix), &m_vecCentreOfMass.x);   // 0x59C790
    switch (flightModel) {
    case FLIGHT_MODEL_UNK: FlcRouteAirship(f); break;
    case FLIGHT_MODEL_BARON: case FLIGHT_MODEL_PLANE: case FLIGHT_MODEL_UNK2: case FLIGHT_MODEL_BOAT: FlcRouteWing(f); break;
    case FLIGHT_MODEL_RC: case FLIGHT_MODEL_HELI: case eFlightModel(8): FlcRouteRotor(f); break;
    default: break;
    }
    FlcTail(f);
    fp.Commit();
}
