<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6736 con evidencia histórica y 1710 pendientes en 77 grupos. Perfil histórico: 694 grupos / 6136 funciones. REA admite GTA SA x86; standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores de este documento conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Alcance solicitado el 8 de octubre de 2026

`scope.json` y `inventory.csv` conservan todas las identidades de las 77 clases solicitadas. La selección inicial es `requested_pending=1`, equivalente a `validated=0` al capturar el catálogo: 1.710 funciones de 2.310. Las 600 restantes aportan contexto histórico; no amplían automáticamente el replay solicitado. Todas las cifras orientativas de la petición coinciden con esa selección.

No hay pendientes de reconstrucción en estas clases: todas las identidades están catalogadas con `reversed=1`. Esto no demuestra equivalencia funcional. Los estados históricos, la coincidencia de fuentes y la vigencia se distinguen por columnas. `audit_mode=selection_only_current_unverified` significa que las columnas de vigencia aún no son una auditoría; `full_registry_evaluate` usa el evaluador conservador existente.

El generador sólo escribe los archivos de esta carpeta. No cambia fuentes, hooks ni registro. Para refrescar la auditoría: `python reversing/RequestedValidation20261008/build_inventory.py --full-audit`. La auditoría comprueba hashes registrados y revisión de dependencias, sin ejecutar el juego ni renovar acreditaciones.

Los bancos candidatos se obtienen por nombre exacto `Clase::función` dentro de `pairs_per_function` o `covered_functions` de resúmenes existentes. No se infieren sobrecargas ni aprobación por cantidad de pares. Cero coincidencias no demuestra ausencia de un banco preparado: los arrays de sondas, nombres alias y direcciones deben revisarse por separado.

`CCollision::SphereVsEntity` tiene una exclusión histórica explícita en `reversing/ColWorld59c/README.md`: el cuerpo original lee una esfera local sin inicializar e ignora su argumento, y no se encontraron llamadores. Es un límite por confirmar mediante el EXE canónico; no equivale a aprobación o a bloqueo externo ya demostrado.
