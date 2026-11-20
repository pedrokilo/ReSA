/* Generado: indice de los envoltorios wrap/w_*.c. */
#include <string.h>
#include "jpeg6b_export.h"
extern const jpeg6b_export jpeg6b_exports_jcapimin[];
extern const jpeg6b_export jpeg6b_exports_jcapistd[];
extern const jpeg6b_export jpeg6b_exports_jccoefct[];
extern const jpeg6b_export jpeg6b_exports_jccolor[];
extern const jpeg6b_export jpeg6b_exports_jcdctmgr[];
extern const jpeg6b_export jpeg6b_exports_jchuff[];
extern const jpeg6b_export jpeg6b_exports_jcinit[];
extern const jpeg6b_export jpeg6b_exports_jcmainct[];
extern const jpeg6b_export jpeg6b_exports_jcmarker[];
extern const jpeg6b_export jpeg6b_exports_jcmaster[];
extern const jpeg6b_export jpeg6b_exports_jcomapi[];
extern const jpeg6b_export jpeg6b_exports_jcparam[];
extern const jpeg6b_export jpeg6b_exports_jcphuff[];
extern const jpeg6b_export jpeg6b_exports_jcprepct[];
extern const jpeg6b_export jpeg6b_exports_jcsample[];
extern const jpeg6b_export jpeg6b_exports_jctrans[];
extern const jpeg6b_export jpeg6b_exports_jdapimin[];
extern const jpeg6b_export jpeg6b_exports_jdapistd[];
extern const jpeg6b_export jpeg6b_exports_jdatadst[];
extern const jpeg6b_export jpeg6b_exports_jdatasrc[];
extern const jpeg6b_export jpeg6b_exports_jdcoefct[];
extern const jpeg6b_export jpeg6b_exports_jdcolor[];
extern const jpeg6b_export jpeg6b_exports_jddctmgr[];
extern const jpeg6b_export jpeg6b_exports_jdhuff[];
extern const jpeg6b_export jpeg6b_exports_jdinput[];
extern const jpeg6b_export jpeg6b_exports_jdmainct[];
extern const jpeg6b_export jpeg6b_exports_jdmarker[];
extern const jpeg6b_export jpeg6b_exports_jdmaster[];
extern const jpeg6b_export jpeg6b_exports_jdmerge[];
extern const jpeg6b_export jpeg6b_exports_jdphuff[];
extern const jpeg6b_export jpeg6b_exports_jdpostct[];
extern const jpeg6b_export jpeg6b_exports_jdsample[];
extern const jpeg6b_export jpeg6b_exports_jdtrans[];
extern const jpeg6b_export jpeg6b_exports_jerror[];
extern const jpeg6b_export jpeg6b_exports_jfdctflt[];
extern const jpeg6b_export jpeg6b_exports_jfdctfst[];
extern const jpeg6b_export jpeg6b_exports_jfdctint[];
extern const jpeg6b_export jpeg6b_exports_jidctflt[];
extern const jpeg6b_export jpeg6b_exports_jidctfst[];
extern const jpeg6b_export jpeg6b_exports_jidctint[];
extern const jpeg6b_export jpeg6b_exports_jidctred[];
extern const jpeg6b_export jpeg6b_exports_jmemmgr[];
extern const jpeg6b_export jpeg6b_exports_jmemnobs[];
extern const jpeg6b_export jpeg6b_exports_jquant1[];
extern const jpeg6b_export jpeg6b_exports_jquant2[];
extern const jpeg6b_export jpeg6b_exports_jutils[];
static const struct { const char* file; const jpeg6b_export* t; } all[] = {
    {"jcapimin", jpeg6b_exports_jcapimin},
    {"jcapistd", jpeg6b_exports_jcapistd},
    {"jccoefct", jpeg6b_exports_jccoefct},
    {"jccolor", jpeg6b_exports_jccolor},
    {"jcdctmgr", jpeg6b_exports_jcdctmgr},
    {"jchuff", jpeg6b_exports_jchuff},
    {"jcinit", jpeg6b_exports_jcinit},
    {"jcmainct", jpeg6b_exports_jcmainct},
    {"jcmarker", jpeg6b_exports_jcmarker},
    {"jcmaster", jpeg6b_exports_jcmaster},
    {"jcomapi", jpeg6b_exports_jcomapi},
    {"jcparam", jpeg6b_exports_jcparam},
    {"jcphuff", jpeg6b_exports_jcphuff},
    {"jcprepct", jpeg6b_exports_jcprepct},
    {"jcsample", jpeg6b_exports_jcsample},
    {"jctrans", jpeg6b_exports_jctrans},
    {"jdapimin", jpeg6b_exports_jdapimin},
    {"jdapistd", jpeg6b_exports_jdapistd},
    {"jdatadst", jpeg6b_exports_jdatadst},
    {"jdatasrc", jpeg6b_exports_jdatasrc},
    {"jdcoefct", jpeg6b_exports_jdcoefct},
    {"jdcolor", jpeg6b_exports_jdcolor},
    {"jddctmgr", jpeg6b_exports_jddctmgr},
    {"jdhuff", jpeg6b_exports_jdhuff},
    {"jdinput", jpeg6b_exports_jdinput},
    {"jdmainct", jpeg6b_exports_jdmainct},
    {"jdmarker", jpeg6b_exports_jdmarker},
    {"jdmaster", jpeg6b_exports_jdmaster},
    {"jdmerge", jpeg6b_exports_jdmerge},
    {"jdphuff", jpeg6b_exports_jdphuff},
    {"jdpostct", jpeg6b_exports_jdpostct},
    {"jdsample", jpeg6b_exports_jdsample},
    {"jdtrans", jpeg6b_exports_jdtrans},
    {"jerror", jpeg6b_exports_jerror},
    {"jfdctflt", jpeg6b_exports_jfdctflt},
    {"jfdctfst", jpeg6b_exports_jfdctfst},
    {"jfdctint", jpeg6b_exports_jfdctint},
    {"jidctflt", jpeg6b_exports_jidctflt},
    {"jidctfst", jpeg6b_exports_jidctfst},
    {"jidctint", jpeg6b_exports_jidctint},
    {"jidctred", jpeg6b_exports_jidctred},
    {"jmemmgr", jpeg6b_exports_jmemmgr},
    {"jmemnobs", jpeg6b_exports_jmemnobs},
    {"jquant1", jpeg6b_exports_jquant1},
    {"jquant2", jpeg6b_exports_jquant2},
    {"jutils", jpeg6b_exports_jutils},
    {0, 0}
};
void* jpeg6b_find(const char* file, const char* name) {
    int i, j;
    for (i = 0; all[i].file; i++) if (!strcmp(all[i].file, file)) for (j = 0; all[i].t[j].name; j++) if (!strcmp(all[i].t[j].name, name)) return all[i].t[j].fn;
    return 0;
}
