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
