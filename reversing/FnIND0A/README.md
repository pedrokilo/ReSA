# FnIND0A: mapa de dependencias del código propio con el EXE original (18/11/2026)

Informe: [docs/ExeIndependiente/README.md](../../docs/ExeIndependiente/README.md). Todo es estático; no se ejecuta el juego.

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
