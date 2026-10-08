<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6848 con evidencia histórica, 0 con vigencia acreditada y 1598 pendientes en 60 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CTaskComplexGangLeader::GetTaskType

[Continuidad](../MEJORAS_DEL_PROCESO.md#continuidad-vigente) · [Comparación](validation-summary.json) · [Instalación e integración](installation-summary.json)

## Cambio y causa del bloqueo

Única función autorizada: `GetTaskType`. La clase ya figuraba completa, 13/13. El registro la mantenía **deshabilitada y bloqueada** junto con otras diez funciones por una advertencia colectiva sobre cierres (comentario de PR#449). Ese comentario no identifica esta función como causa ni obliga a mantenerla activa. No se investigaron ni desbloquearon Clone, el resto de GangLeader, Game, CCollision o CFileMgr.

El original verificado tiene SHA-256 `72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b`. En VA `0x65DF20`, offset de archivo `0x25D320`, contiene `B8 B1 04 00 00 C3`: `mov eax,1201; ret`. No consulta el objeto ni llama a otras funciones. La tabla de `0x86F8FC` referencia esta dirección en el índice **4**. El C++ existente devuelve `Type`, cuyo valor es `TASK_COMPLEX_GANG_LEADER = 1201`; no fue necesario cambiar el cuerpo.

Se retiran `.enabled = false, .locked = true` únicamente de este registro. Los otros diez bloqueos de la clase permanecen. El CSV conserva `reversed=1`, dirección virtual catalogada cero y tipo V; solo cambia `locked=1` a `0`. Totales: 619 locks, 85 aplazados de las cuatro clases completas.

> **Nota de vigencia:** esos 619 locks y 85 aplazados corresponden al cierre histórico de esta entrega. El catálogo vigente se amplió después de la auditoría de hooks; consultar [ReversedClasses.md](../../docs/ReversedClasses.md) para los totales actuales.

## Evidencia dentro del motor

Banco aprobado `20260924-203415-111032`: **24 pares exactos**, ocho valores del timer y tres rutas: entrada del EXE, entrada de la tabla GTA y llamada virtual C++ mediante CTask. Utiliza dos objetos locales independientes, sin peatones ni grupos activos. Se ejecuta en FrontendIdle, desde el menú del motor; no es una sesión jugable.

En cada par se desactiva y activa el hook, se verifican los seis bytes originales y las entradas de ambas tablas, y se exige que el contador del candidato aumente **0** al ejecutar el original y **1** al ejecutar el candidato. Retorno 1201 en ambos, objetos sin cambios, demás hooks intactos y estado inicial restaurado. No hay exclusiones de hooks por argumentos. El estado inicial de esta sonda era deshabilitado/bloqueado; la integración normal verifica por lectura de memoria que la entrega está habilitada.

Se omiten temporalmente los vídeos con FastLoader=true y **SaveGameToLoad=-2**. El ejecutor compartido restaura por hash ambos INI y ASI/PDB anteriores y comprueba las partidas. No se intenta cargar la ranura incompatible ni se escribe en la instalación personal. La copia exclusiva de pruebas es `D:/JUEGOS/GTA San Andreas - CVehicle Test`.

Intentos descartados: una compilación falló por una cabecera faltante del banco. La primera ejecución (`20260924-202907-905162`) usaba por error el índice virtual 3, correspondiente a IsSimple: las rutas directas coincidieron pero el banco falló, restaurando todo. Se corrigió tras leer la tabla del EXE. Sus binarios y registros se conservan; no cuentan como aprobación. El estado 4 observado durante ese arranque correspondía al vídeo de introducción, no a un fallo atribuido a GetTaskType.

## Entrega aislada

Git real de partida: ReSA, `aef6bcbf051d0370cc6ed68e8d9809e0b7bb8d51`, con un cambio local previo en TaskSimpleClimb.cpp. El ASI instalado era `d8b39a395abfed3acae54a480df9db7368dbdbbaff94caeed817dcc121f903ec`; bin/Debug contenía el candidato de cierre `a81bb5fb1cb3cd3586c57a349193b2f5715390768e9fe40ec437943ec0891153`. No eran el HEAD ni los binarios del traspaso antiguo.

`build_probe.py` compila HEAD con estas sustituciones temporales documentadas por hash:

- CustomBuildingRenderer.cpp y CustomBuildingDNPipeline.cpp: versiones de `11fd6912f759d3bc93208a9def001b6dd751dbdf`, anteriores al trabajo aplazado.
- Game.cpp, Cover.cpp y Cover.h: versiones de `aef6bcb^`, anteriores al candidato de cierre también aplazado.
- TaskSimpleClimb.cpp: versión de `aef6bcb`, excluyendo el cambio local preexistente.

Las fuentes completas actuales se conservan en respaldos y se restauran byte a byte mediante `finally`, igual que los binarios previos de bin/Debug. No se modifica el índice Git. La sonda añade solamente un contador y el punto de ejecución del banco; `--normal` no los añade. Los manifiestos distinguen fuentes del workspace, fuentes compiladas y binarios.

`install_and_check.py` respalda ASI/PDB de la copia de pruebas y de bin/Debug, instala la compilación normal aislada y reutiliza InicioNormal. Exige diez segundos en menú, sin entrar a una partida, y verifica en memoria que el salto de `0x65DF20` coincide con el destino de la entrada virtual 4. Restaura los INI normales; si falla, restaura también los binarios. Consultar el resumen enlazado para resultado, hashes y respaldo exactos. Esta comprobación termina solo su proceso; no valida el cierre normal, que sigue aplazado.

**Instalación aprobada el 2026-09-24:** menú alcanzado a los 2,25 segundos y mantenido diez segundos, hook activo confirmado, ninguna carga automática de partida. ASI instalado y en bin/Debug: `fa25398636579a405ab50c2067c28077c5c9cb2e4f7bf7a0d6dd855cf6d78558`; PDB: `cd39cc7bbe38478e6787c0dd4798dece473dcda6d9d2f933d1b539e0e92e3fb9`. Respaldo `_baseline/before-gang-leader-20260924-203727`. Fuentes aplazadas y cambio local de escalada restaurados; ambos INI normales y partidas conservados por hash. La integración omite vídeos temporalmente; no acredita el recorrido completo de introducción con FastLoader=false.

**Una compilación habitual del árbol completo volvería a incluir las fuentes aplazadas.** Para reproducir esta entrega aislada se usa el script documentado, no se copian indiscriminadamente otros binarios a la instalación.

## Repetición y límites

```powershell
python reversing/GangLeader/build_probe.py
python reversing/run.py --case gang-leader-type
python reversing/GangLeader/build_probe.py --normal
python reversing/GangLeader/install_and_check.py
```

Los scripts deben ejecutarse sin otra compilación simultánea ni GTA abierto. Hay un único PROBAR.cmd. Las sondas anteriores se archivan por hash al repetir. La evidencia se vincula como histórica en el registro; no se otorga una nueva acreditación vigente sin revisión de dependencias y versión. No se repitió toda la suite ni se validaron comportamiento completo de bandas, estabilidad general o independencia del EXE. Sin commit, push ni apagado.
