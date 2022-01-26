/*  bdfdrivr.c

    FreeType font driver for bdf files

    Copyright (C) 2001, 2002, 2003, 2004, 2005 by
    Francesco Zappa Nardelli

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <ft2build.h>

#include FT_INTERNAL_DEBUG_H
#include FT_INTERNAL_STREAM_H
#include FT_INTERNAL_OBJECTS_H
#include FT_BDF_H

#include FT_SERVICE_BDF_H
#include FT_SERVICE_XFREE86_NAME_H

#include "bdf.h"
#include "bdfdrivr.h"

#include "bdferror.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_bdfdriver


  typedef struct  BDF_CMapRec_
  {
    FT_CMapRec        cmap;
    FT_UInt           num_encodings;
    BDF_encoding_el*  encodings;

  } BDF_CMapRec, *BDF_CMap;


  FT_CALLBACK_DEF( FT_Error )
  bdf_cmap_init( FT_CMap     bdfcmap,
                 FT_Pointer  init_data )
  {
    BDF_CMap  cmap = (BDF_CMap)bdfcmap;
    BDF_Face  face = (BDF_Face)FT_CMAP_FACE( cmap );
    FT_UNUSED( init_data );


    cmap->num_encodings = face->bdffont->glyphs_used;
    cmap->encodings     = face->en_table;

    return BDF_Err_Ok;
  }


  FT_CALLBACK_DEF( void )
  bdf_cmap_done( FT_CMap  bdfcmap )
  {
    BDF_CMap  cmap = (BDF_CMap)bdfcmap;


    cmap->encodings     = NULL;
    cmap->num_encodings = 0;
  }


  FT_CALLBACK_DEF( FT_UInt )
  bdf_cmap_char_index( FT_CMap    bdfcmap,
                       FT_UInt32  charcode )
  {
    BDF_CMap          cmap      = (BDF_CMap)bdfcmap;
    BDF_encoding_el*  encodings = cmap->encodings;
    FT_UInt           min, max, mid;
    FT_UInt           result    = 0;


    min = 0;
    max = cmap->num_encodings;

    while ( min < max )
    {
      FT_UInt32  code;


      mid  = ( min + max ) >> 1;
      code = encodings[mid].enc;

      if ( charcode == code )
      {
        /* increase glyph index by 1 --              */
        /* we reserve slot 0 for the undefined glyph */
        result = encodings[mid].glyph + 1;
        break;
      }

      if ( charcode < code )
        max = mid;
      else
        min = mid + 1;
    }

    return result;
  }


  FT_CALLBACK_DEF( FT_UInt )
  bdf_cmap_char_next( FT_CMap     bdfcmap,
                      FT_UInt32  *acharcode )
  {
    BDF_CMap          cmap      = (BDF_CMap)bdfcmap;
    BDF_encoding_el*  encodings = cmap->encodings;
    FT_UInt           min, max, mid;
    FT_UInt32         charcode = *acharcode + 1;
    FT_UInt           result   = 0;


    min = 0;
    max = cmap->num_encodings;

    while ( min < max )
    {
      FT_UInt32  code;


      mid  = ( min + max ) >> 1;
      code = encodings[mid].enc;

      if ( charcode == code )
      {
        /* increase glyph index by 1 --              */
        /* we reserve slot 0 for the undefined glyph */
        result = encodings[mid].glyph + 1;
        goto Exit;
      }

      if ( charcode < code )
        max = mid;
      else
        min = mid + 1;
    }

    charcode = 0;
    if ( min < cmap->num_encodings )
    {
      charcode = encodings[min].enc;
      result   = encodings[min].glyph + 1;
    }

  Exit:
    *acharcode = charcode;
    return result;
  }


  FT_CALLBACK_TABLE_DEF
  const FT_CMap_ClassRec  bdf_cmap_class =
  {
    sizeof ( BDF_CMapRec ),
    bdf_cmap_init,
    bdf_cmap_done,
    bdf_cmap_char_index,
    bdf_cmap_char_next
  };


  static FT_Error
  bdf_interpret_style( BDF_Face  bdf )
  {
    FT_Error         error  = BDF_Err_Ok;
    FT_Face          face   = FT_FACE( bdf );
    FT_Memory        memory = face->memory;
    bdf_font_t*      font   = bdf->bdffont;
    bdf_property_t*  prop;

    char  *istr = NULL, *bstr = NULL;
    char  *sstr = NULL, *astr = NULL;

    int  parts = 0, len = 0;


    face->style_flags = 0;

    prop = bdf_get_font_property( font, (char *)"SLANT" );
    if ( prop && prop->format == BDF_ATOM                             &&
         prop->value.atom                                             &&
         ( *(prop->value.atom) == 'O' || *(prop->value.atom) == 'o' ||
           *(prop->value.atom) == 'I' || *(prop->value.atom) == 'i' ) )
    {
      face->style_flags |= FT_STYLE_FLAG_ITALIC;
      istr = ( *(prop->value.atom) == 'O' || *(prop->value.atom) == 'o' )
               ? (char *)"Oblique"
               : (char *)"Italic";
      len += ft_strlen( istr );
      parts++;
    }

    prop = bdf_get_font_property( font, (char *)"WEIGHT_NAME" );
    if ( prop && prop->format == BDF_ATOM                             &&
         prop->value.atom                                             &&
         ( *(prop->value.atom) == 'B' || *(prop->value.atom) == 'b' ) )
    {
      face->style_flags |= FT_STYLE_FLAG_BOLD;
      bstr = (char *)"Bold";
      len += ft_strlen( bstr );
      parts++;
    }

    prop = bdf_get_font_property( font, (char *)"SETWIDTH_NAME" );
    if ( prop && prop->format == BDF_ATOM                              &&
         prop->value.atom && *(prop->value.atom)                       &&
         !( *(prop->value.atom) == 'N' || *(prop->value.atom) == 'n' ) )
    {
      sstr = (char *)(prop->value.atom);
      len += ft_strlen( sstr );
      parts++;
    }

    prop = bdf_get_font_property( font, (char *)"ADD_STYLE_NAME" );
    if ( prop && prop->format == BDF_ATOM                              &&
         prop->value.atom && *(prop->value.atom)                       &&
         !( *(prop->value.atom) == 'N' || *(prop->value.atom) == 'n' ) )
    {
      astr = (char *)(prop->value.atom);
      len += ft_strlen( astr );
      parts++;
    }

    if ( !parts || !len )
      face->style_name = (char *)"Regular";
    else
    {
      char          *style, *s;
      unsigned int  i;


      if ( FT_ALLOC( style, len + parts ) )
        return error;

      s = style;

      if ( astr )
      {
        ft_strcpy( s, astr );
        for ( i = 0; i < ft_strlen( astr ); i++, s++ )
          if ( *s == ' ' )
            *s = '-';                     /* replace spaces with dashes */
        *(s++) = ' ';
      }
      if ( bstr )
      {
        ft_strcpy( s, bstr );
        s += ft_strlen( bstr );
        *(s++) = ' ';
      }
      if ( istr )
      {
        ft_strcpy( s, istr );
        s += ft_strlen( istr );
        *(s++) = ' ';
      }
      if ( sstr )
      {
        ft_strcpy( s, sstr );
        for ( i = 0; i < ft_strlen( sstr ); i++, s++ )
          if ( *s == ' ' )
            *s = '-';                     /* replace spaces with dashes */
        *(s++) = ' ';
      }
      *(--s) = '\0';        /* overwrite last ' ', terminate the string */

      face->style_name = style;                     /* allocated string */
    }

    return error;
  }


  FT_CALLBACK_DEF( void )
  BDF_Face_Done( FT_Face  bdfface )         /* BDF_Face */
  {
    BDF_Face   face   = (BDF_Face)bdfface;
    FT_Memory  memory = FT_FACE_MEMORY( face );


    bdf_free_font( face->bdffont );

    FT_FREE( face->en_table );

    FT_FREE( face->charset_encoding );
    FT_FREE( face->charset_registry );
    FT_FREE( bdfface->family_name );

    FT_FREE( bdfface->available_sizes );

    FT_FREE( face->bdffont );

    FT_TRACE4(( "BDF_Face_Done: done face\n" ));
  }


  FT_CALLBACK_DEF( FT_Error )
  BDF_Face_Init( FT_Stream      stream,
                 FT_Face        bdfface,        /* BDF_Face */
                 FT_Int         face_index,
                 FT_Int         num_params,
                 FT_Parameter*  params )
  {
    FT_Error       error  = BDF_Err_Ok;
    BDF_Face       face   = (BDF_Face)bdfface;
    FT_Memory      memory = FT_FACE_MEMORY( face );

    bdf_font_t*    font;
    bdf_options_t  options;

    FT_UNUSED( num_params );
    FT_UNUSED( params );
    FT_UNUSED( face_index );


    if ( FT_STREAM_SEEK( 0 ) )
      goto Exit;

    options.correct_metrics = 1;   /* FZ XXX: options semantics */
    options.keep_unencoded  = 1;
    options.keep_comments   = 0;
    options.font_spacing    = BDF_PROPORTIONAL;

    error = bdf_load_font( stream, memory, &options, &font );
    if ( error == BDF_Err_Missing_Startfont_Field )
    {
      FT_TRACE2(( "[not a valid BDF file]\n" ));
      goto Fail;
    }
    else if ( error )
      goto Exit;

    /* we have a bdf font: let's construct the face object */
    face->bdffont = font;
    {
      bdf_property_t*  prop = NULL;


      FT_TRACE4(( "number of glyphs: %d (%d)\n",
                  font->glyphs_size,
                  font->glyphs_used ));
      FT_TRACE4(( "number of unencoded glyphs: %d (%d)\n",
                  font->unencoded_size,
                  font->unencoded_used ));

      bdfface->num_faces  = 1;
      bdfface->face_index = 0;
      bdfface->face_flags = FT_FACE_FLAG_FIXED_SIZES |
                            FT_FACE_FLAG_HORIZONTAL  |
                            FT_FACE_FLAG_FAST_GLYPHS;

      prop = bdf_get_font_property( font, "SPACING" );
      if ( prop && prop->format == BDF_ATOM                             &&
           prop->value.atom                                             &&
           ( *(prop->value.atom) == 'M' || *(prop->value.atom) == 'm' ||
             *(prop->value.atom) == 'C' || *(prop->value.atom) == 'c' ) )
        bdfface->face_flags |= FT_FACE_FLAG_FIXED_WIDTH;

      /* FZ XXX: TO DO: FT_FACE_FLAGS_VERTICAL   */
      /* FZ XXX: I need a font to implement this */

      prop = bdf_get_font_property( font, "FAMILY_NAME" );
      if ( prop && prop->value.atom )
      {
        int  l = ft_strlen( prop->value.atom ) + 1;


        if ( FT_NEW_ARRAY( bdfface->family_name, l ) )
          goto Exit;
        ft_strcpy( bdfface->family_name, prop->value.atom );
      }
      else
        bdfface->family_name = 0;

      if ( ( error = bdf_interpret_style( face ) ) != 0 )
        goto Exit;

      /* the number of glyphs (with one slot for the undefined glyph */
      /* at position 0 and all unencoded glyphs)                     */
      bdfface->num_glyphs = font->glyphs_size + 1;

      bdfface->num_fixed_sizes = 1;
      if ( FT_NEW_ARRAY( bdfface->available_sizes, 1 ) )
        goto Exit;

      {
        FT_Bitmap_Size*  bsize = bdfface->available_sizes;
        FT_Short         resolution_x = 0, resolution_y = 0;


        FT_MEM_ZERO( bsize, sizeof ( FT_Bitmap_Size ) );

        bsize->height = (FT_Short)( font->font_ascent + font->font_descent );

        prop = bdf_get_font_property( font, "AVERAGE_WIDTH" );
        if ( prop )
          bsize->width = (FT_Short)( ( prop->value.int32 + 5 ) / 10 );
        else
          bsize->width = (FT_Short)( bsize->height * 2/3 );

        prop = bdf_get_font_property( font, "POINT_SIZE" );
        if ( prop )
          /* convert from 722.7 decipoints to 72 points per inch */
          bsize->size =
            (FT_Pos)( ( prop->value.int32 * 64 * 7200 + 36135L ) / 72270L );

        prop = bdf_get_font_property( font, "PIXEL_SIZE" );
        if ( prop )
          bsize->y_ppem = (FT_Short)prop->value.int32 << 6;

        prop = bdf_get_font_property( font, "RESOLUTION_X" );
        if ( prop )
          resolution_x = (FT_Short)prop->value.int32;

        prop = bdf_get_font_property( font, "RESOLUTION_Y" );
        if ( prop )
          resolution_y = (FT_Short)prop->value.int32;

        if ( bsize->y_ppem == 0 )
        {
          bsize->y_ppem = bsize->size;
          if ( resolution_y )
            bsize->y_ppem = bsize->y_ppem * resolution_y / 72;
        }
        if ( resolution_x && resolution_y )
          bsize->x_ppem = bsize->y_ppem * resolution_x / resolution_y;
        else
          bsize->x_ppem = bsize->y_ppem;
      }

      /* encoding table */
      {
        bdf_glyph_t*   cur = font->glyphs;
        unsigned long  n;


        if ( FT_NEW_ARRAY( face->en_table, font->glyphs_size ) )
          goto Exit;

        face->default_glyph = 0;
        for ( n = 0; n < font->glyphs_size; n++ )
        {
          (face->en_table[n]).enc = cur[n].encoding;
          FT_TRACE4(( "idx %d, val 0x%lX\n", n, cur[n].encoding ));
          (face->en_table[n]).glyph = (FT_Short)n;
  
          if ( cur[n].encoding == font->default_char )
            face->default_glyph = n;
        }
      }

      /* charmaps */
      {
        bdf_property_t  *charset_registry = 0, *charset_encoding = 0;
        FT_Bool          unicode_charmap  = 0;


        charset_registry =
          bdf_get_font_property( font, "CHARSET_REGISTRY" );
        charset_encoding =
          bdf_get_font_property( font, "CHARSET_ENCODING" );
        if ( charset_registry && charset_encoding )
        {
          if ( charset_registry->format == BDF_ATOM &&
               charset_encoding->format == BDF_ATOM &&
               charset_registry->value.atom         &&
               charset_encoding->value.atom         )
          {
            const char*  s;


            if ( FT_NEW_ARRAY( face->charset_encoding,
                               ft_strlen( charset_encoding->value.atom ) + 1 ) )
              goto Exit;
            if ( FT_NEW_ARRAY( face->charset_registry,
                               ft_strlen( charset_registry->value.atom ) + 1 ) )
              goto Exit;

            ft_strcpy( face->charset_registry, charset_registry->value.atom );
            ft_strcpy( face->charset_encoding, charset_encoding->value.atom );

            /* Uh, oh, compare first letters manually to avoid dependency
               on locales. */
            s = face->charset_registry;
            if ( ( s[0] == 'i' || s[0] == 'I' ) &&
                 ( s[1] == 's' || s[1] == 'S' ) &&
                 ( s[2] == 'o' || s[2] == 'O' ) )
            {
              s += 3;
              if ( !ft_strcmp( s, "10646" )                      ||
                   ( !ft_strcmp( s, "8859" ) &&
                     !ft_strcmp( face->charset_encoding, "1" ) ) )
              unicode_charmap = 1;
            }

            {
              FT_CharMapRec  charmap;


              charmap.face        = FT_FACE( face );
              charmap.encoding    = FT_ENCODING_NONE;
              charmap.platform_id = 0;
              charmap.encoding_id = 0;

              if ( unicode_charmap )
              {
                charmap.encoding    = FT_ENCODING_UNICODE;
                charmap.platform_id = 3;
                charmap.encoding_id = 1;
              }

              error = FT_CMap_New( &bdf_cmap_class, NULL, &charmap, NULL );

#if 0
              /* Select default charmap */
              if ( bdfface->num_charmaps )
                bdfface->charmap = bdfface->charmaps[0];
#endif
            }

            goto Exit;
          }
        }

        /* otherwise assume Adobe standard encoding */

        {
          FT_CharMapRec  charmap;


          charmap.face        = FT_FACE( face );
          charmap.encoding    = FT_ENCODING_ADOBE_STANDARD;
          charmap.platform_id = 7;
          charmap.encoding_id = 0;

          error = FT_CMap_New( &bdf_cmap_class, NULL, &charmap, NULL );

          /* Select default charmap */
          if ( bdfface->num_charmaps )
            bdfface->charmap = bdfface->charmaps[0];
        }
      }
    }

  Exit:
    return error;

  Fail:
    BDF_Face_Done( bdfface );
    return BDF_Err_Unknown_File_Format;
  }


  FT_CALLBACK_DEF( FT_Error )
  BDF_Set_Pixel_Size( FT_Size  size,
                      FT_UInt  char_width,
                      FT_UInt  char_height )
  {
    BDF_Face  face = (BDF_Face)FT_SIZE_FACE( size );
    FT_Face   root = FT_FACE( face );

    FT_UNUSED( char_width );


    if ( char_height == (FT_UInt)root->available_sizes->height )
    {
      size->metrics.ascender    = face->bdffont->font_ascent << 6;
      size->metrics.descender   = -face->bdffont->font_descent << 6;
      size->metrics.height      = ( face->bdffont->font_ascent +
                                    face->bdffont->font_descent ) << 6;
      size->metrics.max_advance = face->bdffont->bbx.width << 6;

      return BDF_Err_Ok;
    }
    else
      return BDF_Err_Invalid_Pixel_Size;
  }


  FT_CALLBACK_DEF( FT_Error )
  BDF_Set_Point_Size( FT_Size     size,
                      FT_F26Dot6  char_width,
                      FT_F26Dot6  char_height,
                      FT_UInt     horz_resolution,
                      FT_UInt     vert_resolution )
  {
    BDF_Face  face = (BDF_Face)FT_SIZE_FACE( size );
    FT_Face   root = FT_FACE( face );

    FT_UNUSED( char_width );
    FT_UNUSED( char_height );
    FT_UNUSED( horz_resolution );
    FT_UNUSED( vert_resolution );


    FT_TRACE4(( "rec %d - pres %d\n",
                size->metrics.y_ppem, root->available_sizes->y_ppem ));

    if ( size->metrics.y_ppem == root->available_sizes->y_ppem >> 6 )
    {
      size->metrics.ascender    = face->bdffont->font_ascent << 6;
      size->metrics.descender   = -face->bdffont->font_descent << 6;
      size->metrics.height      = ( face->bdffont->font_ascent +
                                    face->bdffont->font_descent ) << 6;
      size->metrics.max_advance = face->bdffont->bbx.width << 6;

      return BDF_Err_Ok;
    }
    else
      return BDF_Err_Invalid_Pixel_Size;
  }


  FT_CALLBACK_DEF( FT_Error )
  BDF_Glyph_Load( FT_GlyphSlot  slot,
                  FT_Size       size,
                  FT_UInt       glyph_index,
                  FT_Int32      load_flags )
  {
    BDF_Face     face   = (BDF_Face)FT_SIZE_FACE( size );
    FT_Error     error  = BDF_Err_Ok;
    FT_Bitmap*   bitmap = &slot->bitmap;
    bdf_glyph_t  glyph;
    int          bpp    = face->bdffont->bpp;

    FT_UNUSED( load_flags );


    if ( !face )
    {
      error = BDF_Err_Invalid_Argument;
      goto Exit;
    }

    /* index 0 is the undefined glyph */
    if ( glyph_index == 0 )
      glyph_index = face->default_glyph;
    else
      glyph_index--;

    /* slot, bitmap => freetype, glyph => bdflib */
    glyph = face->bdffont->glyphs[glyph_index];

    bitmap->rows  = glyph.bbx.height;
    bitmap->width = glyph.bbx.width;
    bitmap->pitch = glyph.bpr;

    /* note: we don't allocate a new array to hold the bitmap; */
    /*       we can simply point to it                         */
    ft_glyphslot_set_bitmap( slot, glyph.bitmap );

    switch ( bpp )
    {
    case 1:
      bitmap->pixel_mode = FT_PIXEL_MODE_MONO;
      break;
    case 2:
      bitmap->pixel_mode = FT_PIXEL_MODE_GRAY2;
      break;
    case 4:
      bitmap->pixel_mode = FT_PIXEL_MODE_GRAY4;
      break;
    case 8:
      bitmap->pixel_mode = FT_PIXEL_MODE_GRAY;
      bitmap->num_grays  = 256;
      break;
    }

    slot->bitmap_left = glyph.bbx.x_offset;
    slot->bitmap_top  = glyph.bbx.ascent;

    /* FZ XXX: TODO: vertical metrics */
    slot->metrics.horiAdvance  = glyph.dwidth << 6;
    slot->metrics.horiBearingX = glyph.bbx.x_offset << 6;
    slot->metrics.horiBearingY = glyph.bbx.ascent << 6;
    slot->metrics.width        = bitmap->width << 6;
    slot->metrics.height       = bitmap->rows << 6;

    slot->linearHoriAdvance = (FT_Fixed)glyph.dwidth << 16;
    slot->format            = FT_GLYPH_FORMAT_BITMAP;

  Exit:
    return error;
  }


 /*
  *
  *  BDF SERVICE
  *
  */

  static FT_Error
  bdf_get_bdf_property( BDF_Face          face,
                        const char*       prop_name,
                        BDF_PropertyRec  *aproperty )
  {
    bdf_property_t*  prop;


    FT_ASSERT( face && face->bdffont );

    prop = bdf_get_font_property( face->bdffont, prop_name );
    if ( prop )
    {
      switch ( prop->format )
      {
      case BDF_ATOM:
        aproperty->type   = BDF_PROPERTY_TYPE_ATOM;
        aproperty->u.atom = prop->value.atom;
        break;

      case BDF_INTEGER:
        aproperty->type      = BDF_PROPERTY_TYPE_INTEGER;
        aproperty->u.integer = prop->value.int32;
        break;

      case BDF_CARDINAL:
        aproperty->type       = BDF_PROPERTY_TYPE_CARDINAL;
        aproperty->u.cardinal = prop->value.card32;
        break;

      default:
        goto Fail;
      }
      return 0;
    }

  Fail:
    return BDF_Err_Invalid_Argument;
  }


  static FT_Error
  bdf_get_charset_id( BDF_Face      face,
                      const char*  *acharset_encoding,
                      const char*  *acharset_registry )
  {
    *acharset_encoding = face->charset_encoding;
    *acharset_registry = face->charset_registry;

    return 0;
  }


  static const FT_Service_BDFRec  bdf_service_bdf =
  {
    (FT_BDF_GetCharsetIdFunc)bdf_get_charset_id,
    (FT_BDF_GetPropertyFunc) bdf_get_bdf_property
  };


 /*
  *
  *  SERVICES LIST
  *
  */

  static const FT_ServiceDescRec  bdf_services[] =
  {
    { FT_SERVICE_ID_BDF,       &bdf_service_bdf },
    { FT_SERVICE_ID_XF86_NAME, FT_XF86_FORMAT_BDF },
    { NULL, NULL }
  };


  FT_CALLBACK_DEF( FT_Module_Interface )
  bdf_driver_requester( FT_Module    module,
                        const char*  name )
  {
    FT_UNUSED( module );

    return ft_service_list_lookup( bdf_services, name );
  }



  FT_CALLBACK_TABLE_DEF
  const FT_Driver_ClassRec  bdf_driver_class =
  {
    {
      FT_MODULE_FONT_DRIVER         |
      FT_MODULE_DRIVER_NO_OUTLINES,
      sizeof ( FT_DriverRec ),

      "bdf",
      0x10000L,
      0x20000L,

      0,

      (FT_Module_Constructor)0,
      (FT_Module_Destructor) 0,
      (FT_Module_Requester)  bdf_driver_requester
    },

    sizeof ( BDF_FaceRec ),
    sizeof ( FT_SizeRec ),
    sizeof ( FT_GlyphSlotRec ),

    BDF_Face_Init,
    BDF_Face_Done,
    0,                          /* FT_Size_InitFunc */
    0,                          /* FT_Size_DoneFunc */
    0,                          /* FT_Slot_InitFunc */
    0,                          /* FT_Slot_DoneFunc */

    BDF_Set_Point_Size,
    BDF_Set_Pixel_Size,

    BDF_Glyph_Load,

    0,                          /* FT_Face_GetKerningFunc   */
    0,                          /* FT_Face_AttachFunc       */
    0,                          /* FT_Face_GetAdvancesFunc  */
  };


/* END */
