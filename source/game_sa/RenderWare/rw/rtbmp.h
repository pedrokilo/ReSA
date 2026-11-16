
/***************************************************************************
 *                                                                         *
 * Module  : rtBMP.h                                                       *
 *                                                                         *
 * Purpose : Load BMP format files                                         *
 *                                                                         *
 **************************************************************************/

#ifndef RTBMP_H
#define RTBMP_H

/**
 * \defgroup rtbmp RtBMP
 * \ingroup imageconversiontools
 *
 * BMP Image Format Toolkit for RenderWare.
 *
 * See also http://www.daubnet.com/formats/BMP.html
 */

/****************************************************************************
 Includes
 */

/*--- Include files ---*/
#include "rwcore.h"

//#include "rtbmp.rpe"   /* automatically generated header file */

/****************************************************************************
 Function prototypes
 */

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern RwImage *RtBMPImageWrite(RwImage * image,
                                const RwChar * imageName);
extern RwImage *RtBMPImageRead(const RwChar * imageName);

/* Interno (lote R3A): copia una fila BMP ya leida a la imagen RenderWare segun profundidad de la imagen y bits por pixel del BMP. */
extern RwBool _rtBMPImageSetRow(RwImage * image, RwInt32 row, const RwUInt8 * source,
                                RwInt32 bitCount, RwInt32 flag);


#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

/* RWPUBLICEND */

#endif /* RTBMP_H */
