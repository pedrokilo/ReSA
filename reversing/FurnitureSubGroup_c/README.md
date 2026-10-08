<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 8 de octubre de 2026:** 771 grupos / 8446 funciones reconstruidas; 6761 con evidencia histórica, 0 con vigencia acreditada y 1685 pendientes en 66 grupos. Snapshot de arranque histórico: 694 grupos / 6136 funciones; whitelist sin ampliación automática. REA: 1395 dossiers y 315 fallos de búsqueda sobre 1710 direcciones iniciales resueltas; no equivale a validación. Standalone jugable aún no acreditado. [Estado actual y alcance](../../docs/CurrentStatus.md). Las mediciones anteriores conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FurnitureSubGroup_c - 3/3

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

Reconstruccion de AddFurniture (0x5C00C0), GetFurniture (0x590EE0) y GetRandomId (0x590FD0). Sin delegacion a esas tres funciones originales desde los candidatos.

AddFurniture reserva una entrada en el almacen global de 256 muebles, escribe modelo, ID, dimensiones, riqueza y flags, y la inserta al principio de la lista. Solo lee/escribe los 16 bits bajos del contador global. Si esta lleno devuelve false sin modificarlo. Los muebles robables usan maximo menos minimo de la caja; los demas usan maximo mas 0.5. Cada dimension se trunca y se incrementa si la fraccion es al menos 0.02; se conserva la conversion final a un byte.

GetFurniture con ID no negativo devuelve la primera coincidencia e ignora riqueza. Con cualquier ID negativo selecciona entre los muebles cuyo intervalo incluye la riqueza. GetRandomId aplica el mismo filtro y devuelve el ID con signo o -1 si no hay coincidencia. El original compara la riqueza extendida sin signo con -1: esa rama es inalcanzable, por lo que 255 no funciona como comodin. La seleccion consume exactamente un valor aleatorio incluso con lista vacia o sin coincidencias, usando trunc(random / 32768 * cantidad).

Banco automatico: 20.480 pares dentro del motor (4.096 AddFurniture, 8.192 GetFurniture y 8.192 GetRandomId). Estados y retornos exactos, contadores original/candidato, listas 0-32, IDs negativos/duplicados/ausentes, riqueza 0-255, intervalos invertidos, extremos aleatorios, todos los indices 0-255 y casos de almacen lleno, mitad alta del contador no cero, flags y dimensiones cercanas a umbrales. Se comparan enlaces, entrada escrita, inmutabilidad del resto del almacen y datos de modelo. El jugador, almacen, contador, puntero de modelo y hooks se restauran.

PROBAR.cmd --case furniture, tambien integrado en --all. Se reutilizan el runner y soporte de sondas de reversing, sin otro CMD. El banco termina y cierra su propio GTA automaticamente; restaura ASI/PDB y verifica partidas.

Limites: fixtures de listas y cajas de colision aislados; RNG controlado identico en ambos lados, sin consumir la semilla real. El AddItem llamado por el original conserva el hook compartido existente. No prueba distribucion estadistica del RNG, carga de todos los datos de muebles, colocacion/robo durante juego, representacion ni estabilidad general. Entradas geometricas finitas; no NaN/infinito ni punteros invalidos. Constructor, destructor y Exit no tienen entradas registradas en hooks.csv y no forman parte de estas tres comparaciones. Evidencia final y hashes en validation-summary.json.

Banco final aprobado con los tres hooks habilitados desde el arranque, sin locked. ASI normal Debug x86 instalado solo en copia de pruebas, con respaldo y partidas sin cambios. No se repitio toda la suite historica. Sin commit/push.
