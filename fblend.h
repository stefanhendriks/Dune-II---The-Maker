/***************************************************************
 * FBlend Library,
 *  Copyright (c) Robert J Ohannessian, 2001
 *
 * See the accompanying readme.txt and license.txt file for
 * details.
 */

/** \file fblend.h */
 
#ifndef INCLUDE_FBLEND_H_GUARD
#define INCLUDE_FBLEND_H_GUARD

#include <allegro.h>



/** \defgroup version Version information
  * \{ */
/** \name Version Information
  * \{ */

#define FBLEND_VERSION       0          ///< Major version number
#define FBLEND_MINOR_VERSION 5          ///< Minor version number
#define FBLEND_VERSION_STRING "0.5 CVS" ///< Version string

/** \} */
/** \} */


#ifdef __cplusplus
extern "C" {
#endif


/** \defgroup bitmap_blenders Bitmap Blenders  
 *  \{ */
/** \name Bitmap Blenders
 *  \{ */
extern void fblend_add(BITMAP *src, BITMAP *dst, int x, int y, int fact);
extern void fblend_sub(BITMAP *src, BITMAP *dst, int x, int y, int fact);
extern void fblend_trans(BITMAP *src, BITMAP *dst, int x, int y, int fact);

/** \} */
/** \} */

/** \defgroup primitive_blenders Primitive Blenders  
 *  \{ */
/** \name Primitive Blenders
 *  \{ */
void fblend_rect_add(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
void fblend_rect_trans(BITMAP *dst, int x, int y, int w, int h, int color, int fact);
/** \} */
/** \} */


/** \defgroup stretchers Bitmap Stretching
 *  \{ */
/** \name Bitmap Stretching
 *  \{ */
void fblend_2x_stretch(BITMAP *src, BITMAP *dst, int s_x, int s_y, int d_x, int d_y, int w, int h);
/** \} */
/** \} */


/** \defgroup faders Fade-in/Fade-out
 *  \{ */
/** \name Fade-in/Fade-out
 *  \{ */
void fblend_fade_to_color(BITMAP *src, BITMAP *dst, int x, int y, int color, int fact);
/** \} */
/** \} */


#ifdef __cplusplus
}
#endif

//#define USE_ALLEGRO_BLENDER

#endif

