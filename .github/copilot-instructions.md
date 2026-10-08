<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Instrucciones de revisión para Copilot

En las solicitudes de cambios que añadan o modifiquen clases reconstruidas bajo `source/game_sa/**`, revisa siempre `docs/HookRegistration.md` y aplica su lista de comprobación. Revisa con cuidado para evitar errores.

Comprobaciones obligatorias para cada clase reconstruida nueva:

1. La declaración de la clase contiene `static void InjectHooks();`.
2. `source/InjectHooksMain.cpp` contiene la llamada correspondiente a `ClassName::InjectHooks();` dentro de `InjectHooksMain()`.
3. `friend void InjectHooksMain();` aparece únicamente cuando se necesita acceso a adaptadores privados de hooks.


## Catálogo de hooks

Antes de modificar métricas o documentación de progreso, comprobar la coherencia del registro activo con:

```powershell
python contrib/audit_hook_catalog.py --strict
```

No añadir al CSV owners sólo porque exista código de hook: los registros inactivos o comentados no forman parte del catálogo activo. Usar `--verbose` para investigarlos sin alterar los denominadores.


## Validación en hooks.csv

La columna `validated` se deriva de `reversing/validation-registry.json` y no se edita manualmente.

```powershell
python contrib/sync_validated_flags.py
```

Semántica:
- `validated=1`: existe evidencia aprobada vinculada.
- `validated=0`: no existe evidencia aprobada vinculada todavía.
- La vigencia para el árbol actual se calcula por separado en `ValidationInventory.md`.
- `type=S`: Simple hook.
- `type=V`: Virtual hook mediante vtable.
