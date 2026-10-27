"""La emulacion x87 (fsin/fcos, NaN) de Cross74PhaseCpp y Cross74TrailPointCpp se valida contra el hardware en el microtest de PlayerCrosshair74 (mismo bloque «Lote 74» de X87Math.cpp)."""
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "PlayerCrosshair74"))
import nan_rules_hw as shared
run = shared.run

if __name__ == "__main__":
    res = run(sys.argv[1] if len(sys.argv) > 1 else None)
    print(res["output"])
    print("combinaciones totales:", res["total"], "OK" if res["ok"] else "FALLO")
    sys.exit(0 if res["ok"] else 1)
