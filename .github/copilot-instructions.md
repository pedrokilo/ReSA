# Instrucciones de revisión para Copilot

En las solicitudes de cambios que añadan o modifiquen clases reconstruidas bajo `source/game_sa/**`, revisa siempre `docs/HookRegistration.md` y aplica su lista de comprobación. Revisa con cuidado para evitar errores.

Comprobaciones obligatorias para cada clase reconstruida nueva:

1. La declaración de la clase contiene `static void InjectHooks();`.
2. `source/InjectHooksMain.cpp` contiene la llamada correspondiente a `ClassName::InjectHooks();` dentro de `InjectHooksMain()`.
3. `friend void InjectHooksMain();` aparece únicamente cuando se necesita acceso a adaptadores privados de hooks.
