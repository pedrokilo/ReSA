# CppShadowSqrtStatic: propuesta pendiente

Banco derivado íntegramente de ShadowStatic355; 4096 pares y los mismos fixtures, trazas, comparación exacta y restauración. Sólo cambian namespace, prefijo del ejecutable y hashes de la nueva TU/header. No se copian resúmenes de validación previos.

Requiere aplicar y auditar primero `../CppSqrtSmallStudy/shadow_sqrt_cpp.patch`. No compilado ni ejecutado. El resultado anterior no valida esta propuesta. Antes de admitir exigir original/candidate no nulos, diferencias cero, restauración efectiva y auditoría de callers COFF completos. No añade cobertura de flags/registros x86: esa comprobación sigue pendiente.
