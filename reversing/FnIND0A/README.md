<!-- RESA-CURRENT-STATUS:BEGIN -->
> **Estado al 21/11/2026 (Ciudad de México):** 1004 grupos / 13311 funciones reconstruidas; 11801 con evidencia histórica y 1510 pendientes de validación en 39 grupos. CAEVehicleAudioEntity completa: 102 funciones, 20024 pares, cero diferencias (corte 453). Vigencia acreditada: 0; la evidencia histórica no acredita dependencias ni independencia del EXE. Win::NOTSA_WinMain está excluida como adaptación NOTSA; conserva validated=0 en hooks_notsa.csv. El 100% describe sólo las entradas catalogadas, no todos los métodos de cada clase ni todo el EXE: 9223 entradas binarias sin nombre catalogado; 819 destinos originales explícitos fuera del catálogo, 395 referenciados desde entradas catalogadas. Estos conjuntos incluyen bibliotecas y funciones compartidas; no equivalen a funciones pendientes. [Cobertura fuera del catálogo](../../docs/BinaryCoverageAudit.md). Cohorte inicial de 499 dependencias: 131 aprobadas dentro del juego; 124 entradas del juego pendientes, 244 de bibliotecas y 0 inciertas. [Estado actual y alcance](../../docs/CurrentStatus.md). Los resultados y cifras del cuerpo conservan su fecha y alcance histórico.
<!-- RESA-CURRENT-STATUS:END -->

# FnIND0A: mapa de dependencias del código propio con el EXE original (18/11/2026)

Informe: [docs/ExeIndependiente/README.md](../../docs/ExeIndependiente/README.md). Todo es estático; no se ejecuta el juego. **Actualización IND1A (19/11/2026):** el cargador se depuró y se ejecutó con el juego; ver [../FnIND1A/README.md](../FnIND1A/README.md) (el `cargador_imagen.cpp` de esta carpeta es ya la versión final; entrada `IndEntrada`, callback TLS y CRT original primero).

## Reproducir

```
python -I pe_info.py --exe "<gta_sa.exe>" --out datos                 # secciones, imports, recursos, resumen del PE
python -I scan_obj.py --obj-dir ..\..\build\source\gta_reversed.dir\Debug --out datos\refs_codigo.csv   # 11 s; 70.825 referencias
python -I analizar.py --exe "<gta_sa.exe>"                            # dependencias_*.csv, cierre, initterm, tablas, resumen.json
prototipo\enlazar_exe_prueba.cmd <raiz> <salida>                      # enlaza gta_reversed_exe.exe (tomar antes D:\Original\compile_lock)
python -I verificar_exe.py <salida>                                   # comprobacion estatica del layout
```

`datos/refs_codigo.csv` (7,5 MB) no se versiona: se regenera con `scan_obj.py` a partir de los `.obj`.

## Datos (`datos/`)

| Archivo | Contenido |
|---|---|
| `dependencias_codigo.csv` | 890 destinos de código sin hook: región, categoría, confianza A/B/C, módulo, formas de uso, quién los llama |
| `cierre_codigo.csv` | 1.572 funciones del EXE alcanzables desde las raíces (bytes, vía raíz/llamadas/vtable) |
| `dependencias_datos.csv` | 4.949 direcciones de datos: sección, tipo, tamaño conocido, nombradas en C++, quién |
| `tablas_de_punteros.csv` | punteros a código en `.rdata/.data` fuera de vtables (3.881 sin hook) |
| `initterm.csv` | 1.673 inicializadores estáticos (`_cinit`) |
| `excluidas_estado.csv`, `audit819_estado.csv` | estado actual de las 118 excluidas y de los 819 destinos de BinaryCoverageAudit |
| `pe_*.csv`, `pe_resumen.json` | secciones, imports (estáticos y diferidos), recursos y cabecera del PE original |
| `resumen.json` | todas las cifras del informe |
