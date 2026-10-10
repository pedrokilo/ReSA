<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7893 con evidencia histórica, 0 con vigencia acreditada y 553 pendientes en 7 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FLOSS: herramienta local de cadenas

Disponible desde el 9 de octubre de 2026: **FLOSS 3.1.1**, ejecutable `C:\Users\pedro\Tools\FLOSS\floss.exe`. Instalado en el PATH del usuario `pedro`, no para todos los usuarios. Codex y Claude pueden usarlo mediante su terminal local; usar la ruta completa si su sesión no ha recogido el PATH nuevo.

Se verificaron `--version` y `--help`. No se ha ejecutado todavía sobre el ejecutable de ReSA ni se atribuyen nuevos hallazgos o validaciones a esta instalación.

Desde la raíz de ReSA:

```powershell
& 'C:\Users\pedro\Tools\FLOSS\floss.exe' --version
New-Item -ItemType Directory -Force 'build/floss' | Out-Null
& 'C:\Users\pedro\Tools\FLOSS\floss.exe' -j --only static -- 'D:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe' > 'build/floss/static.json'
# Pasada complementaria de todos los tipos admitidos:
& 'C:\Users\pedro\Tools\FLOSS\floss.exe' -j -- 'D:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe' > 'build/floss/all.json'
```

Esta ruta de pruebas procede del [README](../README.md); comprobar su disponibilidad y SHA-256 canónico antes del análisis. Registrar hash, versión de FLOSS, comando, código de salida, resultados y limitaciones. Los ejemplos no ejecutan el juego ni cambian hooks.

FLOSS extrae cadenas estáticas, construidas en pila y decodificadas. Contrastar los resultados con Ghidra y el binario. No reconstruye funciones, no demuestra equivalencia y no recupera automáticamente nombres originales a partir de hashes.

ReSA utiliza su sistema propio `ReversibleHooks`, implementado en `source/reversiblehooks/`; documentar FLOSS no incorpora SafetyHook ni cambia ese mecanismo.

[Proyecto oficial](https://github.com/mandiant/flare-floss) · [Release descargada v3.1.1](https://github.com/mandiant/flare-floss/releases/tag/v3.1.1). SHA-256 local del ZIP Windows: `6c71089b8c629c69424b042769f1565f71adc6cd24b2f8d3713c96fa7fdac2fb`; no es una suma publicada por el autor.
