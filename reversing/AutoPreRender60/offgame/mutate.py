"""Prueba de sensibilidad del banco fuera del juego: aplica mutaciones deliberadas a la fuente candidata (Automobile.cpp, con copia de seguridad que se restaura siempre) y
comprueba que el banco las detecta (fallos > 0). Una mutacion que sobrevive es un hueco del banco (o una mutacion equivalente).

  python -B reversing/AutoPreRender60/offgame/mutate.py [--cases N] [--out DIR] [--only i,j]
"""
import argparse, shutil, subprocess, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
HERE = Path(__file__).resolve().parent
SRC = ROOT / "source/game_sa/Entity/Vehicle/Automobile.cpp"

MUTATIONS = [
    ("rueda LB -> 0 (puntos de contacto)", "{ CAR_WHEEL_LB, 1 }", "{ CAR_WHEEL_LB, 0 }"),
    ("direccion de las ruedas", "wheel < 2 ? -1.f : 1.f", "wheel < 1 ? -1.f : 1.f"),
    ("ranuras de las ruedas intermedias", "{ { 1, 5 }, { 3, 6 } }", "{ { 1, 6 }, { 3, 5 } }"),
    ("umbral de lluvia 20 -> 10", "fabsf(speedRatio) < 20.f", "fabsf(speedRatio) < 10.f"),
    ("umbral de escape 130 -> 100", "speedRatio < 130.f", "speedRatio < 100.f"),
    ("posicion de la luz del taxi", "CVector{ 0.f, -0.4f, 0.95f }", "CVector{ 0.f, -0.4f, 0.96f }"),
    ("tamano de la corona del taxi", "intensity, intensity, 0, 255, local, 0.8f", "intensity, intensity, 0, 255, local, 0.9f"),
    ("barra del 599", "case 599: a = CVector{ 0.7f, -0.1f, 1.2f }", "case 599: a = CVector{ 0.7f, -0.1f, 1.3f }"),
    ("color c1 de la sirena", "c1 = extra / 6;", "c1 = extra / 5;"),
    ("selector de la sirena", "selector == 2", "selector == 1"),
    ("bandera de direccion trasera", "(car.m_nHandlingFlagsIntValue & 0x40) && car.m_f2ndSteerAngle != 0.f", "(car.m_nHandlingFlagsIntValue & 0x80) && car.m_f2ndSteerAngle != 0.f"),
    ("direccion delantera/trasera de la rueda", "isFront ? front : rear", "isFront ? rear : front"),
    ("salto del Rhino", "if (car.m_nModelIndex == 432) { // el Rhino salta", "if (car.m_nModelIndex == 433) { // el Rhino salta"),
    ("orden de UpdateWheelMatrix", "car.UpdateWheelMatrix(4, wheelFlags);\n    car.UpdateWheelMatrix(7, wheelFlags);", "car.UpdateWheelMatrix(7, wheelFlags);\n    car.UpdateWheelMatrix(4, wheelFlags);"),
    ("puerta del maletero", "car.ProcessSwingingDoor(CAR_BOOT, DOOR_BOOT);", "car.ProcessSwingingDoor(CAR_BOOT, DOOR_BONNET);"),
    ("indice de panel > -1", "static_cast<int16>(bouncing.m_nFrameId) > -1", "static_cast<int16>(bouncing.m_nFrameId) > 0"),
    ("rotacion del panel 0", "AutoPreAddCpp(frameMatrix.GetUp().y, car.m_panels[0].m_vecRotation.x)", "AutoPreAddCpp(frameMatrix.GetUp().y, car.m_panels[0].m_vecRotation.y)"),
    ("eje de la hormigonera", "car.SetComponentRotation(nodes[CAR_MISC_B], AXIS_Y, car.m_fPropRotate, true);", "car.SetComponentRotation(nodes[CAR_MISC_B], AXIS_X, car.m_fPropRotate, true);"),
    ("hidraulica", "(car.m_nHandlingFlagsIntValue & 0x10000) && (car.m_nHandlingFlagsIntValue & 0x20000)", "(car.m_nHandlingFlagsIntValue & 0x10000) && (car.m_nHandlingFlagsIntValue & 0x40000)"),
    ("montacargas", "AutoPreForkliftZCpp(car.m_wMiscComponentAngle, 0.0006f,", "AutoPreForkliftZCpp(car.m_wMiscComponentAngle, 0.0005f,"),
    ("excavadora", "misc * 0.0002f", "misc * 0.0003f"),
    ("BF Injection", "AutoPreMulCpp(angle, 1.5f)", "AutoPreMulCpp(angle, 1.4f)"),
    ("cosechadora: banderas de entrada", "car.m_nGettingInFlags == 0 && car.m_nGettingOutFlags == 0", "car.m_nGettingInFlags == 0"),
    ("llamada a GetColModel del BF", "(void)car.GetColModel(); // 0x535300 (el EXE no usa el resultado)\n    static constexpr int32 nodes[]", "static constexpr int32 nodes[]"),
    ("banderas de rueda del Rhino", "car.m_nModelIndex == 432 ? 7 : 0", "car.m_nModelIndex == 432 ? 6 : 0"),
    ("sombra RC", "(vehicleFlags.bIsRCVehicle && m_nVehicleSubType != VEHICLE_TYPE_AUTOMOBILE) ? VEH_SHD_RC : VEH_SHD_CAR", "(vehicleFlags.bIsRCVehicle && m_nVehicleSubType != VEHICLE_TYPE_BIKE) ? VEH_SHD_RC : VEH_SHD_CAR"),
    ("suspension: mtruck", "m_nVehicleSubType != VEHICLE_TYPE_MTRUCK", "m_nVehicleSubType != VEHICLE_TYPE_QUAD"),
    ("rain: area", "StaticRef<int32>(0xB72914) == 0 // CGame::currArea", "StaticRef<int32>(0xB72914) == 1 // CGame::currArea"),
    ("exhaust: drowning", "!car.vehicleFlags.bIsDrowning", "car.vehicleFlags.bIsDrowning"),
]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--cases", type=int, default=4096)
    ap.add_argument("--out", default=str(HERE / "build_mut"))
    ap.add_argument("--only", default="")
    a = ap.parse_args()
    backup = SRC.with_suffix(".cpp.mutbak")
    shutil.copyfile(SRC, backup)
    original = SRC.read_text(encoding="utf-8")
    only = {int(x) for x in a.only.split(",") if x}
    survivors = []
    try:
        for i, (name, old, new) in enumerate(MUTATIONS):
            if only and i not in only:
                continue
            if original.count(old) < 1:
                print("%2d  NO ENCONTRADA: %s" % (i, name))
                continue
            SRC.write_text(original.replace(old, new), encoding="utf-8", newline="")
            b = subprocess.run([sys.executable, "-B", str(HERE / "build_offgame.py"), "--out", a.out], capture_output=True, text=True)
            if b.returncode:
                print("%2d  NO COMPILA: %s" % (i, name))
                continue
            r = subprocess.run([sys.executable, "-B", str(HERE / "run_offgame.py"), "--exe-harness", str(Path(a.out) / "harness.exe"), str(a.cases), "0"], capture_output=True, text=True)
            line = [l for l in r.stdout.splitlines() if l.startswith("Casos")]
            failures = int(line[0].split("fallos:")[1]) if line else -1
            print("%2d  %s: %s" % (i, "detectada" if failures else "SOBREVIVE", name), "(%d fallos)" % failures)
            if not failures:
                survivors.append(name)
    finally:
        shutil.copyfile(backup, SRC)
        backup.unlink()
    print("\nSobreviven %d mutaciones: %s" % (len(survivors), survivors))


if __name__ == "__main__":
    main()
