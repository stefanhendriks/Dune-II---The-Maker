/***************************************************************************/
/*                                                                         */
/*  ttpload.c                                                              */
/*                                                                         */
/*    TrueType glyph data/program tables loader (body).                    */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2004, 2005 by                               */
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
#include FT_INTERNAL_OBJECTS_H
#include FT_INTERNAL_STREAM_H
#include FT_TRUETYPE_TAGS_H

#include "ttpload.h"

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
#include "ttgxvar.h"
#endif

#include "tterrors.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttpload


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_loca                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Loads the locations table.                                         */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    stream :: The input stream.                                        */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
#ifdef FT_OPTIMIZE_MEMORY

  FT_LOCAL_DEF( FT_Error )
  tt_face_load_loca( TT_Face    face,
                     FT_Stream  stream )
  {
    FT_Error  error;
    FT_ULong  table_len;


    /* we need the size of the `glyf' table for malformed `loca' tables */
    error = face->goto_table( face, TTAG_glyf, stream, &face->glyf_len );
    if ( error )
      goto Exit;

    FT_TRACE2(( "Locations " ));
    error = face->goto_table( face, TTAG_loca, stream, &table_len );
    if ( error )
    {
      error = TT_Err_Locations_Missing;
      goto Exit;
    }

    if ( face->header.Index_To_Loc_Format != 0 )
    {
      if ( table_len >= 0x40000 )
      {
        FT_TRACE2(( "table too large!\n" ));
        error = TT_Err_Invalid_Table;
        goto Exit;
      }
      face->num_locations = (FT_UInt)( table_len >> 2 );
    }
    else
    {
      if ( table_len >= 0x20000 )
      {
        FT_TRACE2(( "table too large!\n" ));
        error = TT_Err_Invalid_Table;
        goto Exit;
      }
      face->num_locations = (FT_UInt)( table_len >> 1 );
    }

    /*
     * Extract the frame.  We don't need to decompress it since
     * we are able to parse it directly.
     */
    if ( FT_FRAME_EXTRACT( table_len, face->glyph_locations ) )
      goto Exit;

    FT_TRACE2(( "loaded\n" ));

  Exit:
    return error;
  }


  FT_LOCAL_DEF( FT_ULong )
  tt_face_get_location( TT_Face   face,
                        FT_UInt   gindex,
                        FT_UInt  *asize )
  {
    FT_ULong  pos1, pos2;
    FT_Byte*  p;
    FT_Byte*  p_limit;


    pos1 = pos2 = 0;

    if ( gindex < face->num_locations )
    {
      if ( face->header.Index_To_Loc_Format != 0 )
      {
        p       = face->glyph_locations + gindex * 4;
        p_limit = face->glyph_locations + face->num_locations * 4;

        pos1 = FT_NEXT_ULONG( p );
        pos2 = pos1;

        if ( p + 4 <= p_limit )
          pos2 = FT_NEXT_ULONG( p );
      }
      else
      {
        p       = face->glyph_locations + gindex * 2;
        p_limit = face->glyph_locations + face->num_locations * 2;

        pos1 = FT_NEXT_USHORT( p );
        pos2 = pos1;

        if ( p + 2 <= p_limit )
          pos2 = FT_NEXT_USHORT( p );

        pos1 <<= 1;
        pos2 <<= 1;
      }
    }

    /* It isn't mentioned explicitly that the `loca' table must be  */
    /* ordered, but implicitly it refers to the length of an entry  */
    /* as the difference between the current and the next position. */
    /* Anyway, there do exist (malformed) fonts which don't obey    */
    /* this rule, so we are only able to provide an upper bound for */
    /* the size.                                                    */
    if ( pos2 >= pos1 )
      *asize = (FT_UInt)( pos2 - pos1 );
    else
      *asize = (FT_UInt)( face->glyf_len - pos1 );

    return pos1;
  }


  FT_LOCAL_DEF( void )
  tt_face_done_loca( TT_Face  face )
  {
    FT_Stream  stream = face->root.stream;


    FT_FRAME_RELEASE( face->glyph_locations );
    face->num_locations = 0;
  }


#else /* !FT_OPTIMIZE_MEMORY */


  FT_LOCAL_DEF( FT_Error )
  tt_face_load_loca( TT_Face    face,
                     FT_Stream  stream )
  {
    FT_Error   error;
    FT_Memory  memory = stream->memory;
    FT_Short   LongOffsets;
    FT_ULong   table_len;


    /* we need the size of the `glyf' table for malformed `loca' tables */
    error = face->goto_table( face, TTAG_glyf, stream, &face->glyf_len );
    if ( error )
      goto Exit;

    FT_TRACE2(( "Locations " ));
    LongOffsets = face->header.Index_To_Loc_Format;

    error = face->goto_table( face, TTAG_loca, stream, &table_len );
    if ( error )
    {
      error = TT_Err_Locations_Missing;
      goto Exit;
    }

    if ( LongOffsets != 0 )
    {
      face->num_locations = (FT_UShort)( table_len >> 2 );

      FT_TRACE2(( "(32bit offsets): %12d ", face->num_locations ));

      if ( FT_NEW_ARRAY( face->glyph_locations, face->num_locations ) )
        goto Exit;

      if ( FT_FRAME_ENTER( face->num_locations * 4L ) )
        goto Exit;

      {
        FT_Long*  loc   = face->glyph_locations;
        FT_Long*  limit = loc + face->num_locations;


        for ( ; loc < limit; loc++ )
          *loc = FT_GET_LONG();
      }

      FT_FRAME_EXIT();
    }
    else
    {
      face->num_locations = (FT_UShort)( table_len >> 1 );

      FT_TRACE2(( "(16bit offsets): %12d ", face->num_locations ));

      if ( FT_NEW_ARRAY( face->glyph_locations, face->num_locations ) )
        goto Exit;

      if ( FT_FRAME_ENTER( face->num_locations * 2L ) )
        goto Exit;

      {
        FT_Long*  loc   = face->glyph_locations;
        FT_Long*  limit = loc + face->num_locations;


        for ( ; loc < limit; loc++ )
          *loc = (FT_Long)( (FT_ULong)FT_GET_USHORT() * 2 );
      }

      FT_FRAME_EXIT();
    }

    FT_TRACE2(( "loaded\n" ));

  Exit:
    return error;
  }


  FT_LOCAL_DEF( FT_ULong )
  tt_face_get_location( TT_Face   face,
                        FT_UInt   gindex,
                        FT_UInt  *asize )
  {
    FT_ULong  offset;
    FT_UInt   count;


    offset = face->glyph_locations[gindex];
    count  = 0;

    if ( gindex < (FT_UInt)face->num_locations - 1 )
    {
      FT_ULong  offset1 = face->glyph_locations[gindex + 1];


      /* It isn't mentioned explicitly that the `loca' table must be  */
      /* ordered, but implicitly it refers to the length of an entry  */
      /* as the difference between the current and the next position. */
      /* Anyway, there do exist (malformed) fonts which don't obey    */
      /* this rule, so we are only able to provide an upper bound for */
      /* the size.                                                    */
      if ( offset1 >= offset )
        count = (FT_UInt)( offset1 - offset );
      else
        count = (FT_UInt)( face->glyf_len - offset );
    }

    *asize = count;
    return offset;
  }


  FT_LOCAL_DEF( void )
  tt_face_done_loca( TT_Face  face )
  {
    FT_Memory  memory = face->root.memory;


    FT_FREE( face->glyph_locations );
    face->num_locations = 0;
  }


#endif /* !FT_OPTIMIZE_MEMORY */


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_cvt                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Loads the control value table into a face object.                  */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    stream :: A handle to the input stream.                            */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_cvt( TT_Face    face,
                    FT_Stream  stream )
  {
#ifdef TT_CONFIG_OPTION_BYTECODE_INTERPRETER

    FT_Error   error;
    FT_Memory  memory = stream->memory;
    FT_ULong   table_len;


    FT_TRACE2(( "CVT " ));

    error = face->goto_table( face, TTAG_cvt, stream, &table_len );
    if ( error )
    {
      FT_TRACE2(( "is missing!\n" ));

      face->cvt_size = 0;
      face->cvt      = NULL;
      error          = TT_Err_Ok;

      goto Exit;
    }

    face->cvt_size = table_len / 2;

    if ( FT_NEW_ARRAY( face->cvt, face->cvt_size ) )
      goto Exit;

    if ( FT_FRAME_ENTER( face->cvt_size * 2L ) )
      goto Exit;

    {
      FT_Short*  cur   = face->cvt;
      FT_Short*  limit = cur + face->cvt_size;


      for ( ; cur <  limit; cur++ )
        *cur = FT_GET_SHORT();
    }

    FT_FRAME_EXIT();
    FT_TRACE2(( "loaded\n" ));

#ifdef TT_CONFIG_OPTION_GX_VAR_SUPPORT
    if ( face->doblend )
      error = tt_face_vary_cvt( face, stream );
#endif

  Exit:
    return error;

#else /* !TT_CONFIG_OPTION_BYTECODE_INTERPRETER */

    FT_UNUSED( face   );
    FT_UNUSED( stream );

    return TT_Err_Ok;

#endif
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_fpgm                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Loads the font program and the cvt program.                        */
  /*                                                                       */
  /* <InOut>                                                               */
  /*    face   :: A handle to the target face object.                      */
  /*                                                                       */
  /* <Input>                                                               */
  /*    stream :: A handle to the input stream.                            */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_fpgm( TT_Face    face,
                     FT_Stream  stream )
  {
#ifdef TT_CONFIG_OPTION_BYTECODE_INTERPRETER

    FT_Error   error;
    FT_ULong   table_len;


    FT_TRACE2(( "Font program " ));

    /* The font program is optional */
    error = face->goto_table( face, TTAG_fpgm, stream, &table_len );
    if ( error )
    {
      face->font_program      = NULL;
      face->font_program_size = 0;

      FT_TRACE2(( "is missing!\n" ));
    }
    else
    {
      face->font_program_size = table_len;
      if ( FT_FRAME_EXTRACT( table_len, face->font_program ) )
        goto Exit;

      FT_TRACE2(( "loaded, %12d bytes\n", face->font_program_size ));
    }

    FT_TRACE2(( "Prep program " ));

    error = face->goto_table( face, TTAG_prep, stream, &table_len );
    if ( error )
    {
      face->cvt_program      = NULL;
      face->cvt_program_size = 0;
      error                  = TT_Err_Ok;

      FT_TRACE2(( "is missing!\n" ));
    }
    else
    {
      face->cvt_program_size = table_len;
      if ( FT_FRAME_EXTRACT( table_len, face->cvt_program ) )
        goto Exit;

      FT_TRACE2(( "loaded, %12d bytes\n", face->cvt_program_size ));
    }

  Exit:
    return error;

#else /* !TT_CONFIG_OPTION_BYTECODE_INTERPRETER */

    FT_UNUSED( face   );
    FT_UNUSED( stream );

    return TT_Err_Ok;

#endif
  }


/* END */
