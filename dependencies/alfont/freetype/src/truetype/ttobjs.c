/***************************************************************************/
/*                                                                         */
/*  ttobjs.c                                                               */
/*                                                                         */
/*    Objects manager (body).                                              */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2003, 2004, 2005 by                         */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include <ft2build.h>
#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_CALC_H
#include FT_INTERNAL_STREAM_H
#include FT_TRUETYPE_IDS_H
#include FT_TRUETYPE_TAGS_H
#include FT_INTERNAL_SFNT_H

#include "ttgload.h"
#include "ttpload.h"

#include "tterrors.h"

#ifdef TT_CONFIG_OPTION_BYTECODE_INTERPRETER
#include "ttinterp.h"
#endif

#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING
#include FT_TRUETYPE_UNPATENTED_H
#endif

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
#include "ttgxvar.h"
#endif

  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttobjs


#ifdef TT_CONFIG_OPTION_BYTECODE_INTERPRETER

  /*************************************************************************/
  /*                                                                       */
  /*                       GLYPH ZONE FUNCTIONS                            */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_glyphzone_done                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Deallocates a glyph zone.                                          */
  /*                                                                       */
  /* <Input>                                                               */
  /*    zone :: A pointer to the target glyph zone.                        */
  /*                                                                       */
  FT_LOCAL_DEF( void )
  tt_glyphzone_done( TT_GlyphZone  zone )
  {
    FT_Memory  memory = zone->memory;


    if ( memory )
    {
      FT_FREE( zone->contours );
      FT_FREE( zone->tags );
      FT_FREE( zone->cur );
      FT_FREE( zone->org );

      zone->max_points   = zone->n_points   = 0;
      zone->max_contours = zone->n_contours = 0;
      zone->memory       = NULL;
    }
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_glyphzone_new                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Allocates a new glyph zone.                                        */
  /*                                                                       */
  /* <Input>                                                               */
  /*    memory      :: A handle to the current memory object.              */
  /*                                                                       */
  /*    maxPoints   :: The capacity of glyph zone in points.               */
  /*                                                                       */
  /*    maxContours :: The capacity of glyph zone in contours.             */
  /*                                                                       */
  /* <Output>                                                              */
  /*    zone        :: A pointer to the target glyph zone record.          */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_glyphzone_new( FT_Memory     memory,
                    FT_UShort     maxPoints,
                    FT_Short      maxContours,
                    TT_GlyphZone  zone )
  {
    FT_Error  error;


    if ( maxPoints > 0 )
      maxPoints += 2;

    FT_MEM_ZERO( zone, sizeof ( *zone ) );
    zone->memory = memory;

    if ( FT_NEW_ARRAY( zone->org,      maxPoints * 2 ) ||
         FT_NEW_ARRAY( zone->cur,      maxPoints * 2 ) ||
         FT_NEW_ARRAY( zone->tags,     maxPoints     ) ||
         FT_NEW_ARRAY( zone->contours, maxContours   ) )
    {
      tt_glyphzone_done( zone );
    }

    return error;
  }
#endif /* TT_CONFIG_OPTION_BYTECODE_INTERPRETER */


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_init                                                       */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Initializes a given TrueType face object.                          */
  /*                                                                       */
  /* <Input>                                                               */
  /*    stream     :: The source font stream.                              */
  /*                                                                       */
  /*    face_index :: The index of the font face in the resource.          */
  /*                                                                       */
  /*    num_params :: Number of additional generic parameters.  Ignored.   */
  /*                                                                       */
  /*    params     :: Additional generic parameters.  Ignored.             */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face       :: The newly built face object.                         */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_init( FT_Stream      stream,
                FT_Face        ttface,      /* TT_Face */
                FT_Int         face_index,
                FT_Int         num_params,
                FT_Parameter*  params )
  {
    FT_Error      error;
    FT_Library    library;
    SFNT_Service  sfnt;
    TT_Face       face = (TT_Face)ttface;


    library = face->root.driver->root.library;
    sfnt    = (SFNT_Service)FT_Get_Module_Interface( library, "sfnt" );
    if ( !sfnt )
      goto Bad_Format;

    /* create input stream from resource */
    if ( FT_STREAM_SEEK( 0 ) )
      goto Exit;

    /* check that we have a valid TrueType file */
    error = sfnt->init_face( stream, face, face_index, num_params, params );
    if ( error )
      goto Exit;

    /* We must also be able to accept Mac/GX fonts, as well as OT ones. */
    /* The 0x00020000 tag is completely undocumented; some fonts from   */
    /* Arphic made for Chinese Windows 3.1 have this.                   */
    if ( face->format_tag != 0x00010000L &&    /* MS fonts  */
         face->format_tag != 0x00020000L &&    /* CJK fonts for Win 3.1 */
         face->format_tag != TTAG_true   )     /* Mac fonts */
    {
      FT_TRACE2(( "[not a valid TTF font]\n" ));
      goto Bad_Format;
    }

    /* If we are performing a simple font format check, exit immediately */
    if ( face_index < 0 )
      return TT_Err_Ok;

    /* Load font directory */
    error = sfnt->load_face( stream, face, face_index, num_params, params );
    if ( error )
      goto Exit;

    if ( face->root.face_flags & FT_FACE_FLAG_SCALABLE )
    {

#ifdef FT_CONFIG_OPTION_INCREMENTAL

      if ( !face->root.internal->incremental_interface )
        error = tt_face_load_loca( face, stream );
      if ( !error )
      {
        error = tt_face_load_cvt( face, stream );
        if ( !error )
          error = tt_face_load_fpgm( face, stream );
      }

#else

      if ( !error )
        error = tt_face_load_loca( face, stream ) ||
                tt_face_load_cvt( face, stream )  ||
                tt_face_load_fpgm( face, stream );

#endif

    }

#ifdef TT_CONFIG_OPTION_UNPATENTED_HINTING

    /* Determine whether unpatented hinting is to be used for this face. */
    face->unpatented_hinting = FT_BOOL
       ( library->debug_hooks[ FT_DEBUG_HOOK_UNPATENTED_HINTING ] != NULL );

    {
      int  i;


      for ( i = 0; i < num_params && !face->unpatented_hinting; i++ )
        if ( params[i].tag == FT_PARAM_TAG_UNPATENTED_HINTING )
          face->unpatented_hinting = TRUE;
    }

#endif /* TT_CONFIG_OPTION_UNPATENTED_HINTING */

    /* initialize standard glyph loading routines */
    TT_Init_Glyph_Loading( face );

  Exit:
    return error;

  Bad_Format:
    error = TT_Err_Unknown_File_Format;
    goto Exit;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_done                                                       */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Finalizes a given face object.                                     */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face :: A pointer to the face object to destroy.                   */
  /*                                                                       */
  FT_LOCAL_DEF( void )
  tt_face_done( FT_Face  ttface )           /* TT_Face */
  {
    TT_Face       face   = (TT_Face)ttface;
    FT_Memory     memory = face->root.memory;
    FT_Stream     stream = face->root.stream;

    SFNT_Service  sfnt   = (SFNT_Service)face->sfnt;


    /* for `extended TrueType formats' (i.e. compressed versions) */
    if ( face->extra.finalizer )
      face->extra.finalizer( face->extra.data );

    if ( sfnt )
      sfnt->done_face( face );

    /* freeing the locations table */
    tt_face_done_loca( face );

    /* freeing the CVT */
    FT_FREE( face->cvt );
    face->cvt_size = 0;

    /* freeing the programs */
    FT_FRAME_RELEASE( face->font_program );
    FT_FRAME_RELEASE( face->cvt_program );
    face->font_program_size = 0;
    face->cvt_program_size  = 0;

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
    tt_done_blend( memory, face->blend );
    face->blend = NULL;
#endif
  }


  /*************************************************************************/
  /*                                                                       */
  /*                           SIZE  FUNCTIONS                             */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_size_init                                                       */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Initializes a new TrueType size object.                            */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    size :: A handle to the size object.                               */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_size_init( FT_Size  ttsize )           /* TT_Size */
  {
    TT_Size   size  = (TT_Size)ttsize;
    FT_Error  error = TT_Err_Ok;


#ifdef TT_CONFIG_OPTION_BYTECODE_INTERPRETER

    TT_Face    face   = (TT_Face)size->root.face;
    FT_Memory  memory = face->root.memory;
    FT_Int     i;

    TT_ExecContext  exec;
    FT_UShort       n_twilight;
    TT_MaxProfile*  maxp = &face->max_profile;


    size->ttmetrics.valid = FALSE;

    size->max_function_defs    = maxp->maxFunctionDefs;
    size->max_instruction_defs = maxp->maxInstructionDefs;

    size->num_function_defs    = 0;
    size->num_instruction_defs = 0;

    size->max_func = 0;
    size->max_ins  = 0;

    size->cvt_size     = face->cvt_size;
    size->storage_size = maxp->maxStorage;

    /* Set default metrics */
    {
      FT_Size_Metrics*  metrics  = &size->root.metrics;
      TT_Size_Metrics*  metrics2 = &size->ttmetrics;


      metrics->x_ppem = 0;
      metrics->y_ppem = 0;

      metrics2->rotated   = FALSE;
      metrics2->stretched = FALSE;

      /* set default compensation (all 0) */
      for ( i = 0; i < 4; i++ )
        metrics2->compensations[i] = 0;
    }

    /* allocate function defs, instruction defs, cvt, and storage area */
    if ( FT_NEW_ARRAY( size->function_defs,    size->max_function_defs    ) ||
         FT_NEW_ARRAY( size->instruction_defs, size->max_instruction_defs ) ||
         FT_NEW_ARRAY( size->cvt,              size->cvt_size             ) ||
         FT_NEW_ARRAY( size->storage,          size->storage_size         ) )

      goto Fail_Memory;

    /* reserve twilight zone */
    n_twilight = maxp->maxTwilightPoints;
    error = tt_glyphzone_new( memory, n_twilight, 0, &size->twilight );
    if ( error )
      goto Fail_Memory;

    size->twilight.n_points = n_twilight;

    /* set `face->interpreter' according to the debug hook present */
    {
      FT_Library  library = face->root.driver->root.library;


      face->interpreter = (TT_Interpreter)
                            library->debug_hooks[FT_DEBUG_HOOK_TRUETYPE];
      if ( !face->interpreter )
        face->interpreter = (TT_Interpreter)TT_RunIns;
    }

    /* Fine, now execute the font program! */
    exec = size->context;
    /* size objects used during debugging have their own context */
    if ( !size->debug )
      exec = TT_New_Context( face );

    if ( !exec )
    {
      error = TT_Err_Could_Not_Find_Context;
      goto Fail_Memory;
    }

    size->GS = tt_default_graphics_state;
    TT_Load_Context( exec, face, size );

    exec->callTop   = 0;
    exec->top       = 0;

    exec->period    = 64;
    exec->phase     = 0;
    exec->threshold = 0;

    {
      FT_Size_Metrics*  metrics    = &exec->metrics;
      TT_Size_Metrics*  tt_metrics = &exec->tt_metrics;


      metrics->x_ppem   = 0;
      metrics->y_ppem   = 0;
      metrics->x_scale  = 0;
      metrics->y_scale  = 0;

      tt_metrics->ppem  = 0;
      tt_metrics->scale = 0;
      tt_metrics->ratio = 0x10000L;
    }

    exec->instruction_trap = FALSE;

    exec->cvtSize = size->cvt_size;
    exec->cvt     = size->cvt;

    exec->F_dot_P = 0x10000L;

    /* allow font program execution */
    TT_Set_CodeRange( exec,
                      tt_coderange_font,
                      face->font_program,
                      face->font_program_size );

    /* disable CVT and glyph programs coderange */
    TT_Clear_CodeRange( exec, tt_coderange_cvt );
    TT_Clear_CodeRange( exec, tt_coderange_glyph );

    if ( face->font_program_size > 0 )
    {
      error = TT_Goto_CodeRange( exec, tt_coderange_font, 0 );
      if ( !error )
        error = face->interpreter( exec );

      if ( error )
        goto Fail_Exec;
    }
    else
      error = TT_Err_Ok;

    TT_Save_Context( exec, size );

    if ( !size->debug )
      TT_Done_Context( exec );

#endif /* TT_CONFIG_OPTION_BYTECODE_INTERPRETER */

    size->ttmetrics.valid = FALSE;
    return error;

#ifdef TT_CONFIG_OPTION_BYTECODE_INTERPRETER

  Fail_Exec:
    if ( !size->debug )
      TT_Done_Context( exec );

  Fail_Memory:

    tt_size_done( ttsize );
    return error;

#endif /* TT_CONFIG_OPTION_BYTECODE_INTERPRETER */

  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_size_done                                                       */
  /*                                                                       */
  /* <Description>                                                         */
  /*    The TrueType size object finalizer.                                */
  /*                                                                       */
  /* <Input>                                                               */
  /*    size :: A handle to the target size object.                        */
  /*                                                                       */
  FT_LOCAL_DEF( void )
  tt_size_done( FT_Size  ttsize )           /* TT_Size */
  {
    TT_Size    size = (TT_Size)ttsize;

#ifdef TT_CONFIG_OPTION_BYTECODE_INTERPRETER

    FT_Memory  memory = size->root.face->memory;


    if ( size->debug )
    {
      /* the debug context must be deleted by the debugger itself */
      size->context = NULL;
      size->debug   = FALSE;
    }

    FT_FREE( size->cvt );
    size->cvt_size = 0;

    /* free storage area */
    FT_FREE( size->storage );
    size->storage_size = 0;

    /* twilight zone */
    tt_glyphzone_done( &size->twilight );

    FT_FREE( size->function_defs );
    FT_FREE( size->instruction_defs );

    size->num_function_defs    = 0;
    size->max_function_defs    = 0;
    size->num_instruction_defs = 0;
    size->max_instruction_defs = 0;

    size->max_func = 0;
    size->max_ins  = 0;

#endif

    size->ttmetrics.valid = FALSE;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Reset_Outline_Size                                                 */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Resets a TrueType outline size when resolutions and character      */
  /*    dimensions have been changed.                                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    size :: A handle to the target size object.                        */
  /*                                                                       */
  static FT_Error
  Reset_Outline_Size( TT_Size  size )
  {
    TT_Face           face;
    FT_Error          error = TT_Err_Ok;

    FT_Size_Metrics*  metrics;


    if ( size->ttmetrics.valid )
      return TT_Err_Ok;

    face = (TT_Face)size->root.face;

    metrics = &size->metrics;

    if ( metrics->x_ppem < 1 || metrics->y_ppem < 1 )
      return TT_Err_Invalid_PPem;

    /* compute new transformation */
    if ( metrics->x_ppem >= metrics->y_ppem )
    {
      size->ttmetrics.scale   = metrics->x_scale;
      size->ttmetrics.ppem    = metrics->x_ppem;
      size->ttmetrics.x_ratio = 0x10000L;
      size->ttmetrics.y_ratio = FT_MulDiv( metrics->y_ppem,
                                           0x10000L,
                                           metrics->x_ppem );
    }
    else
    {
      size->ttmetrics.scale   = metrics->y_scale;
      size->ttmetrics.ppem    = metrics->y_ppem;
      size->ttmetrics.x_ratio = FT_MulDiv( metrics->x_ppem,
                                           0x10000L,
                                           metrics->y_ppem );
      size->ttmetrics.y_ratio = 0x10000L;
    }

    /* Compute root ascender, descender, text height, and max_advance */
    metrics->ascender =
      FT_PIX_ROUND( FT_MulFix( face->root.ascender, metrics->y_scale ) );
    metrics->descender =
      FT_PIX_ROUND( FT_MulFix( face->root.descender, metrics->y_scale ) );
    metrics->height =
      FT_PIX_ROUND( FT_MulFix( face->root.height, metrics->y_scale ) );
    metrics->max_advance =
      FT_PIX_ROUND( FT_MulFix( face->root.max_advance_width,
                               metrics->x_scale ) );


#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS
    /* set to `invalid' by default */
    size->strike_index = 0xFFFFU;
#endif

#ifdef TT_CONFIG_OPTION_BYTECODE_INTERPRETER

    {
      TT_ExecContext  exec;
      FT_UInt         i, j;


      /* Scale the cvt values to the new ppem.          */
      /* We use by default the y ppem to scale the CVT. */
      for ( i = 0; i < size->cvt_size; i++ )
        size->cvt[i] = FT_MulFix( face->cvt[i], size->ttmetrics.scale );

      /* All twilight points are originally zero */
      for ( j = 0; j < (FT_UInt)size->twilight.n_points; j++ )
      {
        size->twilight.org[j].x = 0;
        size->twilight.org[j].y = 0;
        size->twilight.cur[j].x = 0;
        size->twilight.cur[j].y = 0;
      }

      /* clear storage area */
      for ( i = 0; i < (FT_UInt)size->storage_size; i++ )
        size->storage[i] = 0;

      size->GS = tt_default_graphics_state;

      /* get execution context and run prep program */
      if ( size->debug )
        exec = size->context;
      else
        exec = TT_New_Context( face );
      /* debugging instances have their own context */

      if ( !exec )
        return TT_Err_Could_Not_Find_Context;

      TT_Load_Context( exec, face, size );

      TT_Set_CodeRange( exec,
                        tt_coderange_cvt,
                        face->cvt_program,
                        face->cvt_program_size );

      TT_Clear_CodeRange( exec, tt_coderange_glyph );

      exec->instruction_trap = FALSE;

      exec->top     = 0;
      exec->callTop = 0;

      if ( face->cvt_program_size > 0 )
      {
        error = TT_Goto_CodeRange( exec, tt_coderange_cvt, 0 );
        if ( error )
          goto End;

        if ( !size->debug )
          error = face->interpreter( exec );
      }
      else
        error = TT_Err_Ok;

      size->GS = exec->GS;
      /* save default graphics state */

    End:
      TT_Save_Context( exec, size );

      if ( !size->debug )
        TT_Done_Context( exec );
      /* debugging instances keep their context */
    }

#endif /* TT_CONFIG_OPTION_BYTECODE_INTERPRETER */

    if ( !error )
      size->ttmetrics.valid = TRUE;

    return error;
  }


#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS

  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    Reset_SBit_Size                                                    */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Resets a TrueType sbit size when resolutions and character         */
  /*    dimensions have been changed.                                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    size :: A handle to the target size object.                        */
  /*                                                                       */
  static FT_Error
  Reset_SBit_Size( TT_Size  size )
  {
    TT_Face           face;
    FT_Error          error = TT_Err_Ok;

    FT_ULong          strike_index;
    FT_Size_Metrics*  metrics;
    FT_Size_Metrics*  sbit_metrics;
    SFNT_Service      sfnt;


    metrics = &size->metrics;

    if ( size->strike_index != 0xFFFFU )
      return TT_Err_Ok;

    face = (TT_Face)size->root.face;
    sfnt = (SFNT_Service)face->sfnt;

    sbit_metrics = &size->strike_metrics;

    error = sfnt->set_sbit_strike( face,
                                   metrics->x_ppem, metrics->y_ppem,
                                   &strike_index );

    if ( !error )
    {
      /* XXX: TODO: move this code to the SFNT module where it belongs */

#ifdef FT_OPTIMIZE_MEMORY
      FT_Byte*    strike = face->sbit_table + 8 + strike_index*48;

      sbit_metrics->ascender  = (FT_Char)strike[16] << 6;  /* hori.ascender  */
      sbit_metrics->descender = (FT_Char)strike[17] << 6;  /* hori.descender */

      /* XXX: Is this correct? */
      sbit_metrics->max_advance = ( (FT_Char)strike[22] + /* min_origin_SB  */
                                             strike[18] + /* max_width      */
                                    (FT_Char)strike[23]   /* min_advance_SB */
                                                        ) << 6;

#else /* !FT_OPTIMIZE_MEMORY */

      TT_SBit_Strike  strike = face->sbit_strikes + strike_index;


      sbit_metrics->ascender  = strike->hori.ascender << 6;
      sbit_metrics->descender = strike->hori.descender << 6;

      /* XXX: Is this correct? */
      sbit_metrics->max_advance = ( strike->hori.min_origin_SB  +
                                    strike->hori.max_width      +
                                    strike->hori.min_advance_SB ) << 6;

#endif /* !FT_OPTIMIZE_MEMORY */

      /* XXX: Is this correct? */
      sbit_metrics->height = sbit_metrics->ascender -
                             sbit_metrics->descender;

      sbit_metrics->x_ppem = metrics->x_ppem;
      sbit_metrics->y_ppem = metrics->y_ppem;
      size->strike_index   = (FT_UInt)strike_index;
    }
    else
    {
      size->strike_index = 0xFFFFU;

      sbit_metrics->x_ppem      = 0;
      sbit_metrics->y_ppem      = 0;
      sbit_metrics->ascender    = 0;
      sbit_metrics->descender   = 0;
      sbit_metrics->height      = 0;
      sbit_metrics->max_advance = 0;
    }

    return error;
  }

#endif /* TT_CONFIG_OPTION_EMBEDDED_BITMAPS */


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_size_reset                                                      */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Resets a TrueType size when resolutions and character dimensions   */
  /*    have been changed.                                                 */
  /*                                                                       */
  /* <Input>                                                               */
  /*    size :: A handle to the target size object.                        */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_size_reset( TT_Size  size )
  {
    FT_Face   face;
    FT_Error  error = TT_Err_Ok;


    face = size->root.face;

    if ( face->face_flags & FT_FACE_FLAG_SCALABLE )
    {
      if ( !size->ttmetrics.valid )
        error = Reset_Outline_Size( size );

      if ( error )
        return error;
    }

#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS

    if ( face->face_flags & FT_FACE_FLAG_FIXED_SIZES )
    {
      if ( size->strike_index == 0xFFFFU )
        error = Reset_SBit_Size( size );

      if ( !error && !( face->face_flags & FT_FACE_FLAG_SCALABLE ) )
        size->root.metrics = size->strike_metrics;
    }

#endif /* TT_CONFIG_OPTION_EMBEDDED_BITMAPS */

    if ( face->face_flags & FT_FACE_FLAG_SCALABLE )
      return TT_Err_Ok;
    else
      return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_driver_init                                                     */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Initializes a given TrueType driver object.                        */
  /*                                                                       */
  /* <Input>                                                               */
  /*    driver :: A handle to the target driver object.                    */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_driver_init( FT_Module  driver )       /* TT_Driver */
  {
    FT_Error  error;


    /* set `extra' in glyph loader */
    error = FT_GlyphLoader_CreateExtra( FT_DRIVER( driver )->glyph_loader );

    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_driver_done                                                     */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Finalizes a given TrueType driver.                                 */
  /*                                                                       */
  /* <Input>                                                               */
  /*    driver :: A handle to the target TrueType driver.                  */
  /*                                                                       */
  FT_LOCAL_DEF( void )
  tt_driver_done( FT_Module  ttdriver )     /* TT_Driver */
  {
#ifdef TT_CONFIG_OPTION_BYTECODE_INTERPRETER
    TT_Driver  driver = (TT_Driver)ttdriver;


    /* destroy the execution context */
    if ( driver->context )
    {
      TT_Destroy_Context( driver->context, driver->root.root.memory );
      driver->context = NULL;
    }
#else
    FT_UNUSED( ttdriver );
#endif

  }


/* END */
