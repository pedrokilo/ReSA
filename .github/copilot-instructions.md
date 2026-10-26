<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 818 grupos / 8908 funciones reconstruidas; 8908 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
