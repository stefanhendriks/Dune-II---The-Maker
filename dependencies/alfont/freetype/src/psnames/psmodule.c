/***************************************************************************/
/*                                                                         */
/*  psmodule.c                                                             */
/*                                                                         */
/*    PSNames module implementation (body).                                */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2003, 2005 by                               */
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
#include FT_INTERNAL_OBJECTS_H
#include FT_SERVICE_POSTSCRIPT_CMAPS_H

#include "psmodule.h"
#include "pstables.h"

#include "psnamerr.h"


#ifndef FT_CONFIG_OPTION_NO_POSTSCRIPT_NAMES


#ifdef FT_CONFIG_OPTION_ADOBE_GLYPH_LIST


  /* Return the Unicode value corresponding to a given glyph.  Note that */
  /* we do deal with glyph variants by detecting a non-initial dot in    */
  /* the name, as in `A.swash' or `e.final'.                             */
  /*                                                                     */
  static FT_UInt32
  ps_unicode_value( const char*  glyph_name )
  {
    /* If the name begins with `uni', then the glyph name may be a */
    /* hard-coded unicode character code.                          */
    if ( glyph_name[0] == 'u' &&
         glyph_name[1] == 'n' &&
         glyph_name[2] == 'i' )
    {
      /* determine whether the next four characters following are */
      /* hexadecimal.                                             */

      /* XXX: Add code to deal with ligatures, i.e. glyph names like */
      /*      `uniXXXXYYYYZZZZ'...                                   */

      FT_Int       count;
      FT_ULong     value = 0;
      const char*  p     = glyph_name + 3;


      for ( count = 4; count > 0; count--, p++ )
      {
        char          c = *p;
        unsigned int  d;


        d = (unsigned char)c - '0';
        if ( d >= 10 )
        {
          d = (unsigned char)c - 'A';
          if ( d >= 6 )
            d = 16;
          else
            d += 10;
        }

        /* exit if a non-uppercase hexadecimal character was found */
        if ( d >= 16 )
          break;

        value = ( value << 4 ) + d;
      }

      /* there must be exactly four hex digits */
      if ( ( *p == '\0' || *p == '.' ) && count == 0 )
        return value;
    }

    /* If the name begins with `u', followed by four to six uppercase */
    /* hexadicimal digits, it is a hard-coded unicode character code. */
    if ( glyph_name[0] == 'u' )
    {
      FT_Int       count;
      FT_ULong     value = 0;
      const char*  p     = glyph_name + 1;


      for ( count = 6; count > 0; count--, p++ )
      {
        char          c = *p;
        unsigned int  d;


        d = (unsigned char)c - '0';
        if ( d >= 10 )
        {
          d = (unsigned char)c - 'A';
          if ( d >= 6 )
            d = 16;
          else
            d += 10;
        }

        if ( d >= 16 )
          break;

        value = ( value << 4 ) + d;
      }

      if ( ( *p == '\0' || *p == '.' ) && count <= 2 )
        return value;
    }

    /* look for a non-initial dot in the glyph name in order to */
    /* sort-out variants like `A.swash', `e.final', etc.        */
    {
      const char*  p   = glyph_name;
      const char*  dot = NULL;


      for ( ; *p; p++ )
      {
        if ( *p == '.' && p > glyph_name && !dot )
          dot = p;
      }

      if ( !dot )
        dot = p;

      /* now, look up the glyph in the Adobe Glyph List */
      return ft_get_adobe_glyph_index( glyph_name, dot );
    }
  }


  /* ft_qsort callback to sort the unicode map */
  FT_CALLBACK_DEF( int )
  compare_uni_maps( const void*  a,
                    const void*  b )
  {
    PS_UniMap*  map1 = (PS_UniMap*)a;
    PS_UniMap*  map2 = (PS_UniMap*)b;


    return ( map1->unicode - map2->unicode );
  }


  /* Builds a table that maps Unicode values to glyph indices */
  static FT_Error
  ps_unicodes_init( FT_Memory     memory,
                    FT_UInt       num_glyphs,
                    const char**  glyph_names,
                    PS_Unicodes*  table )
  {
    FT_Error  error;


    /* we first allocate the table */
    table->num_maps = 0;
    table->maps     = 0;

    if ( !FT_NEW_ARRAY( table->maps, num_glyphs ) )
    {
      FT_UInt     n;
      FT_UInt     count;
      PS_UniMap*  map;
      FT_UInt32   uni_char;


      map = table->maps;

      for ( n = 0; n < num_glyphs; n++ )
      {
        const char*  gname = glyph_names[n];


        if ( gname )
        {
          uni_char = ps_unicode_value( gname );

          if ( uni_char != 0 && uni_char != 0xFFFFL )
          {
            map->unicode     = (FT_UInt)uni_char;
            map->glyph_index = n;
            map++;
          }
        }
      }

      /* now, compress the table a bit */
      count = (FT_UInt)( map - table->maps );

      if ( count > 0 && FT_REALLOC( table->maps,
                                    num_glyphs * sizeof ( PS_UniMap ),
                                    count * sizeof ( PS_UniMap ) ) )
        count = 0;

      if ( count == 0 )
      {
        FT_FREE( table->maps );
        if ( !error )
          error = PSnames_Err_Invalid_Argument;  /* no unicode chars here! */
      }
      else
        /* sort the table in increasing order of unicode values */
        ft_qsort( table->maps, count, sizeof ( PS_UniMap ), compare_uni_maps );

      table->num_maps = count;
    }

    return error;
  }


  static FT_UInt
  ps_unicodes_char_index( PS_Unicodes*  table,
                          FT_ULong      unicode )
  {
    PS_UniMap  *min, *max, *mid;


    /* perform a binary search on the table */

    min = table->maps;
    max = min + table->num_maps - 1;

    while ( min <= max )
    {
      mid = min + ( max - min ) / 2;
      if ( mid->unicode == unicode )
        return mid->glyph_index;

      if ( min == max )
        break;

      if ( mid->unicode < unicode )
        min = mid + 1;
      else
        max = mid - 1;
    }

    return 0xFFFFU;
  }


  static FT_ULong
  ps_unicodes_char_next( PS_Unicodes*  table,
                         FT_ULong      unicode )
  {
    PS_UniMap  *min, *max, *mid;


    unicode++;
    /* perform a binary search on the table */

    min = table->maps;
    max = min + table->num_maps - 1;

    while ( min <= max )
    {
      mid = min + ( max - min ) / 2;
      if ( mid->unicode == unicode )
        return unicode;

      if ( min == max )
        break;

      if ( mid->unicode < unicode )
        min = mid + 1;
      else
        max = mid - 1;
    }

    if ( max < table->maps )
      max = table->maps;

    while ( max < table->maps + table->num_maps )
    {
      if ( unicode < max->unicode )
        return max->unicode;
      max++;
    }

    return 0;
  }


#endif /* FT_CONFIG_OPTION_ADOBE_GLYPH_LIST */


  static const char*
  ps_get_macintosh_name( FT_UInt  name_index )
  {
    if ( name_index >= FT_NUM_MAC_NAMES )
      name_index = 0;

    return ft_standard_glyph_names + ft_mac_names[name_index];
  }


  static const char*
  ps_get_standard_strings( FT_UInt  sid )
  {
    if ( sid >= FT_NUM_SID_NAMES )
      return 0;

    return ft_standard_glyph_names + ft_sid_names[sid];
  }


  static
  const FT_Service_PsCMapsRec  pscmaps_interface =
  {
#ifdef FT_CONFIG_OPTION_ADOBE_GLYPH_LIST

    (PS_Unicode_ValueFunc)     ps_unicode_value,
    (PS_Unicodes_InitFunc)     ps_unicodes_init,
    (PS_Unicodes_CharIndexFunc)ps_unicodes_char_index,
    (PS_Unicodes_CharNextFunc) ps_unicodes_char_next,

#else

    0,
    0,
    0,
    0,

#endif /* FT_CONFIG_OPTION_ADOBE_GLYPH_LIST */

    (PS_Macintosh_Name_Func)   ps_get_macintosh_name,
    (PS_Adobe_Std_Strings_Func)ps_get_standard_strings,

    t1_standard_encoding,
    t1_expert_encoding
  };


  static const FT_ServiceDescRec  pscmaps_services[] =
  {
    { FT_SERVICE_ID_POSTSCRIPT_CMAPS, &pscmaps_interface },
    { NULL, NULL }
  };


  static FT_Pointer
  psnames_get_service( FT_Module    module,
                       const char*  service_id )
  {
    FT_UNUSED( module );

    return ft_service_list_lookup( pscmaps_services, service_id );
  }

#endif /* !FT_CONFIG_OPTION_NO_POSTSCRIPT_NAMES */



  FT_CALLBACK_TABLE_DEF
  const FT_Module_Class  psnames_module_class =
  {
    0,  /* this is not a font driver, nor a renderer */
    sizeof ( FT_ModuleRec ),

    "psnames",  /* driver name                         */
    0x10000L,   /* driver version                      */
    0x20000L,   /* driver requires FreeType 2 or above */

#ifdef FT_CONFIG_OPTION_NO_POSTSCRIPT_NAMES
    0,
    (FT_Module_Constructor)0,
    (FT_Module_Destructor) 0,
    (FT_Module_Requester)  0
#else
    (void*)&pscmaps_interface,   /* module specific interface */
    (FT_Module_Constructor)0,
    (FT_Module_Destructor) 0,
    (FT_Module_Requester)  psnames_get_service
#endif
  };


/* END */
