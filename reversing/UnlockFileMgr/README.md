<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 17/10/2026 (Ciudad de México):** 787 grupos / 8581 funciones reconstruidas; 8581 con evidencia histórica y 0 pendientes de validación en 0 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 125 aprobadas dentro del juego; 130 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# CFileMgr: trabajo aplazado

> Los resultados siguientes corresponden al alcance de este informe. El ASI instalado, los diagnósticos posteriores y el trabajo aplazado figuran en la [continuidad vigente](../MEJORAS_DEL_PROCESO.md#continuidad-vigente).

**No validado; trece candados conservados.** Este banco no forma parte del catálogo de pruebas aprobadas.

El borrador y sus adaptaciones se conservan en el [parche de la entrega parcial](../UnlockBatch2/deferred-filemgr-game.patch), con [manifiesto y copias locales](../UnlockBatch2/deferred-manifest.json). Las fuentes compilables vuelven al estado anterior. No reconstruir esta sonda sin recuperar y revisar primero el borrador.

Última ejecución fallida: resultados-unlock-file-mgr/20260923-131422-691585. LoadFile, caso 1: el original devuelve 0 y el candidato 256. La causa sigue pendiente. El relleno del buffer de SetDirMyDocuments se corrigió antes de esa pasada. No se modificaron los flags del CSV.

[Estado y alcance autorizado](../UnlockBatch2/README.md).

## Interoperabilidad de hooks (limitación documentada)

El `FILE*` de la CRT estática del EXE (0x8232D8) y el de la UCRT de la DLL no son intercambiables. Abrir con una CRT y leer con la otra cierra el juego, así que los 13 hooks de archivos son un grupo todo-o-nada. El banco comprueba solo los dos modos viables (máscara 0 = todo EXE y 63 = todo DLL), no las 64 mezclas. Además, el origen del `Seek` se limita a SEEK_SET/CUR/END: un origen inválido provoca *fail-fast* (0xC0000409) en la UCRT y no es comportamiento del juego.

La comprobación `extensions.bounded_load` esperaba que `LoadFile` respetara `size`; el EXE (0x538890) lo ignora (lee bloques de 0x4000 hasta uno incompleto y termina con 0) y la DLL ya lo reproduce, comprobado, ahora de verdad, en los 128 pares de la función 1 (antes devolvían 0 siempre; véase el diagnóstico más abajo). La comprobación la comprobación exige ese comportamiento: devuelve el tamaño entero del fichero, terminador 0 justo después y nada escrito más allá.

## Diagnóstico de LoadFile (17/10/2026)

Firma real de 0x538890 (desensamblado del EXE): `size_t __cdecl LoadFile(const char* path, uint8* buf, size_t size, const char* mode)`; abre con el `fopen` de la CRT del EXE (0x8232D8), llama en bucle a `fread(buf+total, 1, 0x4000, f)` (0x823521) hasta un bloque incompleto, escribe `buf[total]=0`, hace `fclose` y devuelve `total`; `fopen` fallido devuelve -1. `size` (3.er argumento, `[esp+0xC]`) no se lee en ningún punto. La firma de la DLL (`source/game_sa/FileMgr.cpp`) y la llamada del banco coinciden con ella: no hay defecto de firma ni de la DLL.

Defectos del banco corregidos: (1) el destino era un `std::array<uint8,1024>` de pila (y 300 bytes en `bounded_load`), demasiado corto para lo que pide la CRT (0x4000 por bloque, más el 0 final) y sensible a la distancia al tope de la pila: todos los pares devolvían 0 sin leer nada, así que no validaban nada; ahora el destino es un `std::vector` de monton de n+0x4000+64 bytes relleno de 0xA5; (2) el caso «ausente» no lo era: las aperturas en escritura de pasadas anteriores dejaban `fixture.txt.missing` creado (vacío, LoadFile devolvía 0 en vez de -1); ahora se borra antes de cada ejecución y se comprueba que no exista; (3) los pares de la función 1 no registraban el fichero y solo comparaban EXE contra DLL: ahora cada par se contrasta además con un modelo independiente (contenido, retorno, terminador, intacto más allá) y el analizador exige `expect_ok` en ambos lados, retorno > 0 con ficheros no vacíos y -1 con el ausente. Cobertura de la función 1: ficheros de 0, 1, 256, 0x3FFF, 0x4000, 0x4001, 0x8000 y 0xC001 bytes × modos `rb`/`r` × 7 valores del argumento `size` + 16 casos con fichero ausente. `bounded_load` se ejecuta con los ganchos activados y apagados y con ficheros de 0, 256, 0x3FFF, 0x4000, 0x4001 y 0x8000 bytes más uno ausente.
