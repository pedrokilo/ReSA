import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from probe_case import main
here = Path(__file__).parent
# 168 trozos de valores especiales (28224 pares de double), 4096 trozos de 2^20 exponentes (todos los float) + 1024 trozos de 65536 casos aleatorios por cada función de producción
EXPECTED = {'pow especiales (double)': 168, 'pow(0.96f, todo float)': 4096, 'pow regiones (float)': 512, 'PowScaleVector3Cpp': 1024, 'PowDecaySpeedCpp': 1024}
if __name__ == '__main__': sys.exit(main(here, 'pow-equivalence', 'PowEquivalence', EXPECTED, timeout=9000))
