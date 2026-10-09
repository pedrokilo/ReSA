<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 9 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 7163 con evidencia histórica, 0 con vigencia acreditada y 1283 pendientes en 29 grupos. Validaciones detenidas por solicitud del usuario. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# Constructores de CAEWeaponAudioEntity y CAEPedWeaponAudioEntity

Validadas el 03/10/2026 con **1.024 pares original/candidato dentro de GTA**, todos exactos: 2 funciones de 2 clases con `validated=1`.

| Clase | Funciones |
|---|---|
| `CAEWeaponAudioEntity` | Constructor (0x507560) |
| `CAEPedWeaponAudioEntity` | Constructor (0x5DE990) |

**Diferencias reales corregidas:**
- Catálogo de hooks (`docs/hooks.csv` y registros en la fuente):
  - 0x507560 es el constructor de `CAEWeaponAudioEntity` (vtable 0x862E68), no un destructor: la fila `CAEWeaponAudioEntity,Constructor` pasa de 0x5DE990 a 0x507560 y se desbloquea;
  - 0x5DE990 es el constructor de `CAEPedWeaponAudioEntity` (vtable 0x86C2AC, escribe también +0xA0/+0xA4): nueva fila `CAEPedWeaponAudioEntity,Constructor`, registrada en su propia clase;
  - se elimina la fila `CAEWeaponAudioEntity,Destructor`: el EXE no tiene destructor propio de esta clase (está en línea).
- Los envoltorios de la DLL construyen en el sitio como el EXE: solo `m_tempSound.m_PhysicalEntity` (+0x10), la vtable del EXE y los campos de `Clear()` (más `m_Ped` y el byte `m_bInitialised` en la clase de peatón); antes llamaban al constructor C++, que inicializaba todo el objeto.
- Constructor C++ de `CAEWeaponAudioEntity`: sin inicializadores de miembros; solo llama a `Clear()` y deja `m_MiniGunState`/`m_ChainsawState` sin escribir, como el EXE (antes los ponía a 0). La parte `CAEAudioEntity` de la DLL sigue inicializando `m_Entity` y todo `m_tempSound` (clases base compartidas, sin cambios).

**Método:**
- Cada constructor se llama por su entrada del EXE sobre un búfer relleno con 0xCD (la mitad de los casos) o con bytes pseudoaleatorios (para ver qué bytes no se escriben), con el objeto desplazado 0, 1, 2, 4 u 8 bytes y guardas de 16 bytes antes y después.
- Se compara el búfer completo, el valor devuelto (`this`) y la vptr por tipo (vtable del EXE 0x862E68/0x86C2AC o la exportada por la DLL).
- Los constructores no llaman a ninguna función: no hay falsos. Se vigilan las vtables del EXE y las entradas 0x507560/0x5DE990, y el estado de los hooks se restaura al final.

Repetir: `python reversing/WeapAudio/build_probe.py` y `python reversing/run.py --case weapaudio`. Evidencia: [validation-summary.json](validation-summary.json).
