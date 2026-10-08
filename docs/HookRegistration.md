<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6736 con evidencia histórica y 1710 pendientes en 77 grupos. Perfil histórico: 694 grupos / 6136 funciones. REA admite GTA SA x86; standalone jugable aún no acreditado. [Estado actual y alcance](CurrentStatus.md). Las mediciones anteriores de este documento conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Requisitos de registro de hooks para clases reconstruidas

Para cada clase **del juego** reconstruida que se añada bajo `source/game_sa/**`:

1. La declaración de la clase debe incluir `static void InjectHooks();`.
2. `InjectHooksMain()`, en `source/InjectHooksMain.cpp`, debe llamar a `ClassName::InjectHooks();`.

## Lista de comprobación para revisar una solicitud de cambios

- [ ] La clase reconstruida incluye `static void InjectHooks();`.
- [ ] `InjectHooksMain()` contiene la llamada correspondiente a `ClassName::InjectHooks();`.
- [ ] `friend void InjectHooksMain();` aparece únicamente cuando se necesita acceso a adaptadores privados de hooks.
- [ ] Cada función de la clase tiene su registro correspondiente mediante `RH_Install`.
- [ ] Las funciones aún no reconstruidas —por ejemplo, las que usan `plugin::Call` para redirigir al código de GTA— incluyen `.reversed = false` en las opciones de `RH_Install`.


## Auditoría bidireccional del catálogo

Para comprobar la coherencia entre hooks instalados, registros directos o indirectos y `docs/hooks.csv`:

```powershell
python contrib/audit_hook_catalog.py
```

Para utilizarlo como comprobación estricta y obtener código de salida 1 cuando
existan inconsistencias:

```powershell
python contrib/audit_hook_catalog.py --strict
```

Para inspeccionar también owners inactivos, registros indirectos y aliases informativos:

```powershell
python contrib/audit_hook_catalog.py --strict --verbose
```

Esta auditoría complementa a `source_status.py`: revisa también la dirección
**fuente -> catálogo**, que antes no se comprobaba.


### Cómo interpretar el auditor

El auditor elimina comentarios antes de analizar `InjectHooksMain.cpp`, por lo que
una línea como `// CTaskFoo::InjectHooks();` **no cuenta como registro activo**.

También distingue entre:

- **Owner activo con instalaciones ausente del CSV:** inconsistencia real; `--strict` falla.
- **Owner inactivo con instalaciones:** código existente pero no registrado globalmente; se informa para revisión, sin añadirlo automáticamente al catálogo.
- **Llamada activa sin nombre propio en el CSV:** puede ser válida si el método sólo agrupa/encamina registros. Por ejemplo, `RpAnimBlendPlugin::InjectHooks()` registra bajo el namespace `RpAnimBlend`, que es el owner que aparece en `hooks.csv`.
- **Scope sin `RH_Scoped*Install`:** no crea una entrada de función y no debe inflar el catálogo.
- **Llamada `X::InjectHooks()` sin `InjectHooks` propio en `X`:** inconsistencia real; `--strict` falla. Compila igualmente porque resuelve al `InjectHooks` heredado de una base, que entonces se ejecuta dos veces (assert por nombres duplicados en `HookCategory.h`) o registra hooks bajo el owner equivocado. Caso histórico: `CAEPedWeaponAudioEntity` llamaba al de `CAEWeaponAudioEntity`.


### Estado comprobado del corte actual

En `ReSA`, el modo estricto devuelve **OK** con:

- 772 clases/grupos catalogados.
- 8.442 entradas en `hooks.csv`.
- 0 hooks activos ausentes del CSV.
- 0 registros indirectos activos ausentes del CSV.
- 0 entradas del CSV sin scope fuente.
- 0 llamadas directas con instalaciones propias sin owner catalogado.
- 0 owners catalogados cuya función de registro nunca se invoca.
- 0 llamadas `X::InjectHooks()` que resuelven al `InjectHooks` heredado de una base.
- 0 diferencias con el registro real del juego, comprobado con `--runtime "<copia de pruebas>/scripts/runtime-hooks.csv"`. Esa opción compara `hooks.csv` con el `runtime-hooks.csv` que escribe el ASI al arrancar: hooks registrados sin catalogar, entradas catalogadas sin registrar, `reversed` distinto y dirección distinta.

Los owners inactivos se mantienen fuera del catálogo activo mientras no se registren globalmente. No deben añadirse sólo porque exista código de hook en sus archivos.


## Columnas de hooks.csv

| Columna | Significado |
| --- | --- |
| `class` | Owner/clase o namespace del hook. |
| `fn_name` | Nombre estable de la entrada. |
| `address` | Dirección del original; las virtuales pueden usar `0x00000000` en el catálogo. |
| `reversed` | `1`: implementación reconstruida; `0`: sigue pendiente. |
| `locked` | `1`: el hook no debe conmutarse libremente; `0`: sin bloqueo preventivo. |
| `validated` | `1`: existe evidencia aprobada vinculada; `0`: todavía no está vinculada. No implica vigencia actual. |
| `type` | `S` = **Simple hook**; `V` = **Virtual hook** mediante vtable. |

`validated` no se edita manualmente. Sincronizarlo desde el registro:

```powershell
python contrib/sync_validated_flags.py
```

El generador del panel detiene la ejecución si los `validated=1` no coinciden con la evidencia aprobada del registro.
