<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 10/10/2026 (Ciudad de México):** 784 grupos / 8550 funciones reconstruidas; 8550 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 105 aprobadas dentro del juego; 130 entradas del juego pendientes, 243 de bibliotecas y 21 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
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
