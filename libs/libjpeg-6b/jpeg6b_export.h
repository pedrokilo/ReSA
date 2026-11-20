/* Tabla de funciones de libjpeg 6b para sustituir las del EXE (lote LIB3A). Cada archivo .c de la biblioteca se compila a traves de un
 * envoltorio wrap/w_<archivo>.c que lo incluye entero (sin modificar la fuente oficial) y exporta aqui las direcciones de TODAS sus funciones,
 * tambien las `static` (METHODDEF/LOCAL), que son las que el EXE enlaza por puntero. */
#ifndef JPEG6B_EXPORT_H
#define JPEG6B_EXPORT_H
#ifdef __cplusplus
extern "C" {
#endif
typedef struct { const char* name; void* fn; } jpeg6b_export;
/* Busca "archivo", "funcion" (p. ej. "jccolor", "rgb_ycc_convert"); NULL si no existe. */
void* jpeg6b_find(const char* file, const char* name);
#ifdef __cplusplus
}
#endif
#endif
