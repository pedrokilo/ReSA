/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCompressedMatrixNotAligned
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "CompressedMatrixNotAligned.h"

void CCompressedMatrixNotAligned::InjectHooks()
{
    RH_ScopedClass(CCompressedMatrixNotAligned);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(DecompressIntoFullMatrix, 0x59B9F0);
    RH_ScopedInstall(CompressFromFullMatrix, 0x59BAD0);
}

void CCompressedMatrixNotAligned::DecompressIntoFullMatrix(CMatrix& matrix) const
{
    matrix.GetRight() = m_vecRight;
    matrix.GetForward() = m_vecForward;
    matrix.GetUp() = CrossProduct(matrix.GetRight(), matrix.GetForward());
    matrix.GetPosition() = m_vecPos;
    matrix.Reorthogonalise();
}

void CCompressedMatrixNotAligned::CompressFromFullMatrix(const CMatrix& matrix)
{
    m_vecRight = matrix.GetRight();
    m_vecForward = matrix.GetForward();
    m_vecPos = matrix.GetPosition();
}

