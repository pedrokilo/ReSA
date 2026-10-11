# Primera conversión C++: secuencia de CarCtrl

Validación nueva de `CCarCtrl::FindSequenceElement` (`0x421770`) tras retirar su ensamblador y su declaración `naked`. C++ usa aritmética modular sin signo, lectura de representación del flag y resto firmado.

El banco conserva 2.048 casos de secuencia, 2.048 de registro de vehículo y 256 de limpieza: 4.352 pares previstos. Es una nueva corrida con la fuente modificada; no altera el banco histórico DependenciesCarCtrl2 ni sus hashes. Cada par exige resultados original/candidate no nulos, igualdad, rutas OFF/ON, memoria y temporizador intactos y ABI comprobada. El ensamblador del instrumento mide registros; no forma parte de la implementación convertida.

Corrida aprobada `20261010-230755-701916`: 4.352 pares totales, de los que 2.048 corresponden a la conversión. Cero diferencias y ningún registro original/candidate nulo; rutas, ABI, memoria y restauración de binarios, configuración y partidas comprobadas. [Auditoría independiente](runs/20261010-230755-701916/approval-audit.json). SHA256 de las comparaciones: `c1af42f0a22109819641b03c915febcadff5b5217a6197653dba37337b97ebd5`.
