<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 26/10/2026 (Ciudad de México):** 820 grupos / 8926 funciones reconstruidas; 8926 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# DependenciesPedGroups2

Validación aprobada y registrada: [corrida 20261010-182106-246424](runs/20261010-182106-246424/summary.json), cuatro funciones y **13.056 pares, cero diferencias y ningún original/candidato nulo**. La corrida comprueba hooks RH reales OFF/ON, ABI y restauración del estado. Partidas y configuración permanecieron intactas. [Comparaciones](runs/20261010-182106-246424/comparisons.jsonl.gz) · [Auditoría](runs/20261010-182106-246424/approval-audit.json).

SHA-256 ASI ejecutado: `3e117780f16936d9f71fe1917554642a37e70e6f883a15b0513c3b1b5e218bf5`. SHA-256 log: `067e8bf1421e4bd7df713e80295851dda0a0ca4df4ad50d6f0bea001a0f956b4`.

| Función | Dirección | Pares |
| --- | --- | ---: |
| IsGroupLeader | 0x5F7E40 | 4096 |
| GetGroupId | 0x5F7EE0 | 256 |
| IsInPlayersGroup | 0x5F7F10 | 512 |
| AreInSameGroup | 0x5F7F40 | 8192 |
| Total | | 13056 |

La misma entrada EXE se invoca con RH apagado y activo; begin exige que los cuatro hooks estén registrados. Se comprueba destino EXE/DLL, ESP y EBX/ESI/EDI mediante sentinelas cdecl. Los bool se comparan en AL: EAX alto es indeterminado en varias salidas originales y no pertenece al contrato bool. Los resultados originales y candidatos son objetos JSON no nulos. Cada Write bombea mensajes.

Se guarda y restaura la totalidad de ms_activeGroups y ms_groups; cada mitad del par reinstala idénticos fixtures. Se compara que grupos, flags y ped falso no se escriban. Cada HookRoute restaura bytes de entrada, estado activo y bloqueo originales.

El desensamblado acredita:

- IsGroupLeader: 0x5F7E50 prueba flag activo, 0x5F7E5A ped no nulo, 0x5F7E5E compara el líder; avanza 0x2D4 y corta tras ocho grupos. La candidata usa GetLeader existente; no le concede crédito adicional.
- GetGroupId: 0x5F7EF0 compara direcciones exactas; no consulta flags activos; 0x5F7F09 devuelve -1 si no coincide con alguno de ocho grupos. Fixtures incluyen grupos activos/inactivos, NULL y direcciones interiores no coincidentes, nunca dereferenciadas.
- IsInPlayersGroup: 0x5F7F14 lee ped+0x480 (player data) antes de comprobar pertenencia; si es no nulo devuelve false. 0x5F7F1E..0x5F7F37 compara los ocho miembros del grupo cero sin consultar actividad. El banco siempre pasa ped válido con almacenamiento suficiente; NULL está fuera del dominio y no se prueba. No confundir la prueba con m_pIntelligence o con tipo de ped.
- AreInSameGroup: 0x5F7F4A/52 rechazan nulos; recorre grupos activos y los ocho slots de ped1. 0x5F7F85 llama IsMember(0x5F6A10) para ped2; si falla sigue buscando en otros grupos. La candidata conserva ese colaborador, sin crédito nuevo. No sustituir por igualdad de GetPedsGroup: duplicados en grupos diferentes cambian el resultado.

Fixtures: máscaras completas de actividad, flags 1/0x80, posiciones de miembro/líder, ped nulo cuando EXE lo admite, mismos peds, distintos grupos y duplicados. Punteros de membresía sólo se comparan; únicamente IsInPlayersGroup lee el ped de fixture. No se construyen o destruyen grupos/peds ni se acreditan referencias de mundo reales. Init/AddGroup quedan fuera: requieren operaciones de grupos y referencias con un contrato de restauración independiente.

Fuentes anteriores probadas y sus archivos de evidencia permanecen históricos; esta preparación no acredita vigencia de dependencias, integración general o independencia del EXE. runtime_runner no se modifica; la copia de pruebas se selecciona en memoria mediante E:.
