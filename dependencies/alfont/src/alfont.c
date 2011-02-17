/* AllegroFont - a wrapper for FreeType 2 */
/* to render TTF and other font formats with Allegro */

            
/* FreeType 2 is copyright (c) 1996-2000 */
/* David Turner, Robert Wilhelm, and Werner Lemberg */
/* AllegroFont is copyright (c) 2001, 2002 Javier Gonz lez */

/* See FTL.txt (FreeType License) for license */

// todo (char map null check)

#include <allegro.h>
#include <stdlib.h>
#include <string.h>
#include <alfont.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H


/* structs */

struct ALFONT_FONT {
  FT_Face face;           /* face */
  int face_h;             /* face height */
  int real_face_h;        /* real face height */
  int face_ascender;      /* face ascender */
  char *data;             /* if loaded from memory, the data chunk */
  int data_size;          /* and its size */
  int ch_spacing;         /* extra spacing */
  int num_fixed_sizes;    /* -1 if scalable, >=0 if fixed */
  struct _ALFONT_CACHED_GLYPH {
    char is_cached;
    int width, height, aawidth, aaheight;
    int left, top, aaleft, aatop;
    int advancex, advancey;
    int mono_available, aa_available;
    unsigned char *bmp;
    unsigned char *aabmp;
  } *cached_glyphs;       /* array to know which glyphs have been cached */
  int *fixed_sizes;       /* array with the fixed sizes */
};


/* global vars */

static FT_Library ft_library;
static int alfont_textmode = 0;
static int alfont_inited = 0;


/* helpers */

static void _alfont_reget_fixed_sizes(ALFONT_FONT *f) {
  if (f->num_fixed_sizes < 0) {
    /* scalable font */
    f->fixed_sizes[0] = -1;
  }
  else {
    /* fixed */
    int i;
    for (i = 0; i < f->num_fixed_sizes; i++) {
      f->fixed_sizes[i] = f->face->available_sizes[i].height;
    }
    /* set last one to -1 */
    f->fixed_sizes[f->num_fixed_sizes] = -1;
  }
}


static void _alfont_uncache_glyphs(ALFONT_FONT *f) {
  if (f->cached_glyphs) {
    int i;
    for (i = 0; i < f->face->num_glyphs; i++) {
      if (f->cached_glyphs[i].is_cached) {
        f->cached_glyphs[i].is_cached = 0;
        if (f->cached_glyphs[i].bmp) {
          free(f->cached_glyphs[i].bmp);
          f->cached_glyphs[i].bmp = NULL;
        }
        if (f->cached_glyphs[i].aabmp) {
          free(f->cached_glyphs[i].aabmp);
          f->cached_glyphs[i].aabmp = NULL;
        }
      }
    }
  }
}


static void _alfont_delete_glyphs(ALFONT_FONT *f) {
  _alfont_uncache_glyphs(f);
  if (f->cached_glyphs) {
    free(f->cached_glyphs);
    f->cached_glyphs = NULL;
  }
}


static void _alfont_cache_glyph(ALFONT_FONT *f, int glyph_number) {
  /* if glyph not cached yet */
  if (!f->cached_glyphs[glyph_number].is_cached) {
    FT_Glyph new_glyph;
    /* load the font glyph */
    FT_Load_Glyph(f->face, glyph_number, FT_LOAD_DEFAULT);
    FT_Get_Glyph(f->face->glyph, &new_glyph);

    /* ok, this glyph is now cached */
    f->cached_glyphs[glyph_number].is_cached = 1;
    f->cached_glyphs[glyph_number].mono_available = 0;
    f->cached_glyphs[glyph_number].aa_available = 0;

    /* render the mono bmp */
    {
      FT_Bitmap *ft_bmp;
      FT_Glyph glyph;
      FT_BitmapGlyph bmp_glyph;

      FT_Glyph_Copy(new_glyph, &glyph);

      /* only render glyph if it is not already a bitmap */
      if (glyph->format != ft_glyph_format_bitmap)
        FT_Glyph_To_Bitmap(&glyph, ft_render_mode_mono, NULL, 1);

      /* the FT rendered bitmap */
      bmp_glyph = (FT_BitmapGlyph)glyph;
      ft_bmp = &bmp_glyph->bitmap;

      /* save only if the bitmap is really 1 bit */
      if (ft_bmp->pixel_mode == ft_pixel_mode_mono) {
        int memsize;

        f->cached_glyphs[glyph_number].mono_available = 1;

        /* set width, height, left, top */
        f->cached_glyphs[glyph_number].width = ft_bmp->width;
        f->cached_glyphs[glyph_number].height = ft_bmp->rows;
        f->cached_glyphs[glyph_number].left = bmp_glyph->left;
        f->cached_glyphs[glyph_number].top = bmp_glyph->top;

        /* allocate bitmap */
        memsize = ft_bmp->width * ft_bmp->rows * sizeof(unsigned char);
        if (memsize > 0)
          f->cached_glyphs[glyph_number].bmp = malloc(memsize);
        else
          f->cached_glyphs[glyph_number].bmp = NULL;

        /* monochrome drawing */
        if (memsize > 0) {
          unsigned char *outbmp_p = f->cached_glyphs[glyph_number].bmp;
          unsigned char *bmp_p;
          int bmp_x, bmp_y, bit;

          /* copy the FT character bitmap to ours */
          bmp_p = ft_bmp->buffer;
          for (bmp_y = 0; bmp_y < ft_bmp->rows; bmp_y++) {
            unsigned char *next_bmp_p;
            next_bmp_p = bmp_p + ft_bmp->pitch;
            bit = 7;
            for (bmp_x = 0; bmp_x < ft_bmp->width; bmp_x++) {
              *outbmp_p = *bmp_p & (1 << bit);
              outbmp_p++;

              if (bit == 0) {
                bit = 7;
                bmp_p++;
              }
              else
                bit--;
            }
            bmp_p = next_bmp_p;
          }
        }
      }

      FT_Done_Glyph(glyph);
    }


    /* render the aa bmp */
    {
      FT_Bitmap *ft_bmp;
      FT_Glyph glyph;
      FT_BitmapGlyph bmp_glyph;
    
      FT_Glyph_Copy(new_glyph, &glyph);

      /* only render glyph if it is not already a bitmap */
      if (glyph->format != ft_glyph_format_bitmap)
        FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, NULL, 1);

      /* the FT rendered bitmap */
      bmp_glyph = (FT_BitmapGlyph)glyph;
      ft_bmp = &bmp_glyph->bitmap;

      /* save only if the bitmap is really 8 bit */
      if (ft_bmp->pixel_mode == ft_pixel_mode_grays) {
        int memsize;

        f->cached_glyphs[glyph_number].aa_available = 1;

        /* set width, height, left, top */
        f->cached_glyphs[glyph_number].aawidth = ft_bmp->width;
        f->cached_glyphs[glyph_number].aaheight = ft_bmp->rows;
        f->cached_glyphs[glyph_number].aaleft = bmp_glyph->left;
        f->cached_glyphs[glyph_number].aatop = bmp_glyph->top;

        /* allocate bitmap */
        memsize = ft_bmp->width * ft_bmp->rows * sizeof(unsigned char);
        if (memsize > 0)
          f->cached_glyphs[glyph_number].aabmp = malloc(memsize);
        else
          f->cached_glyphs[glyph_number].aabmp = NULL;

        /* aa drawing */
        if (memsize > 0) {
          unsigned char *outbmp_p = f->cached_glyphs[glyph_number].aabmp;
          unsigned char *bmp_p;
          int bmp_y;
          unsigned char mul = 256 / ft_bmp->num_grays;
          /* we set it to 0 because it is faster to test for false */
          if (mul == 1)
            mul = 0;

          /* copy the FT character bitmap to ours */
          bmp_p = ft_bmp->buffer;
          for (bmp_y = 0; bmp_y < ft_bmp->rows; bmp_y++) {
            unsigned char *next_bmp_p;
            next_bmp_p = bmp_p + ft_bmp->pitch;
            memcpy(outbmp_p, bmp_p, ft_bmp->width * sizeof(unsigned char));

            /* we have to change our pixels if the numgrays is not 256 */
            if (mul) {
              unsigned char *p = outbmp_p;
              unsigned char *p_end = p + ft_bmp->width;
              for (; p < p_end; p++)
                *p *= mul;
            }

            outbmp_p += ft_bmp->width;
            bmp_p = next_bmp_p;
          }
        }
      }

      FT_Done_Glyph(glyph);
    }

    f->cached_glyphs[glyph_number].advancex = f->face->glyph->advance.x >> 6;
    f->cached_glyphs[glyph_number].advancey = f->face->glyph->advance.y >> 6;

    /* delete the glyph */
    FT_Done_Glyph(new_glyph);
  }
}


static void _alfont_new_cache_glyph(ALFONT_FONT *f) {
  int i;

  if (!f->cached_glyphs)
    f->cached_glyphs = malloc(f->face->num_glyphs * sizeof(struct _ALFONT_CACHED_GLYPH));

  for (i = 0; i < f->face->num_glyphs; i++) {
    f->cached_glyphs[i].is_cached = 0;
    f->cached_glyphs[i].bmp = NULL;
    f->cached_glyphs[i].aabmp = NULL;
  }
}


/* API */

int alfont_set_font_size(ALFONT_FONT *f, int h) {
  int error, test_h, direction;
  /* check the font doesn't already use that w and h */
  if (h == f->face_h)
    return ALFONT_OK;
  else if (h <= 0)
    return ALFONT_ERROR;

  /* keep changing the size until the real size is not the one */
  /* we want */
  test_h = h;
  direction = 0;
  while (1) {
    int real_height;
    error = FT_Set_Pixel_Sizes(f->face, 0, test_h);
    if (error)
      break;

    /* compare real height with asked height */
    real_height = abs(f->face->size->metrics.ascender >> 6) + abs(f->face->size->metrics.descender >> 6);
    if (real_height == h) {
      /* we found the wanted height */
      break;
    }

    /* check the direction */
    if (direction == 0) {
      /* direction still not set */
      if (real_height > h)
        direction = -1;
      else
        direction = 1;
    }
    
    /* check we didn't overpass it */
    else if ((direction > 0) && (real_height > h)) {
      /* decrease one and found */
      test_h--;
      FT_Set_Pixel_Sizes(f->face, 0, test_h);
      break;
    }

    /* check we didn't surpass it */
    else if ((direction < 0) && (real_height < h)) {
      break;
    }

    test_h += direction;

    /* check we arent at 0 */
    if (test_h <= 0) {
      error = TRUE;
      break;
    }

  }

  if (!error) {
    _alfont_uncache_glyphs(f);
    f->face_h = h;
    f->real_face_h = test_h;
    f->face_ascender = f->face->size->metrics.ascender >> 6;
    return ALFONT_OK;
  }
  else {
    FT_Set_Pixel_Sizes(f->face, 0, f->real_face_h);
    return ALFONT_ERROR;
  }
}


int alfont_get_font_height(ALFONT_FONT *f) {
  return f->face_h;
}


void alfont_exit(void) {
  if (alfont_inited) {
    alfont_inited = 0;
    FT_Done_FreeType(ft_library);
    memset(&ft_library, 0, sizeof(ft_library));
  }
}


int alfont_init(void) {
  if (alfont_inited)
    return 0;
  else {
    int error;
    memset(&ft_library, 0, sizeof(ft_library));
    error = FT_Init_FreeType(&ft_library);

    if (!error)
      alfont_inited = 1;
      
    return error;
  }
}


ALFONT_FONT *alfont_load_font(const char *filepathname) {
  int error;

  /* try to allocate the memory */
  ALFONT_FONT *font = malloc(sizeof(ALFONT_FONT));

  if (font == NULL)
    return NULL;

  /* clear the struct */
  memset(font, 0, sizeof(ALFONT_FONT));
  font->cached_glyphs = NULL;

  /* we are loading from file, no mem buffer needed */
  font->data = NULL;
  font->data_size = 0; 

  /* load the font */
  error = FT_New_Face(ft_library, filepathname, 0, &font->face);

  if (error) {
    free(font);
    return NULL;
  }

  /* get if the font contains only fixed sizes */
  if (!(font->face->face_flags & FT_FACE_FLAG_SCALABLE))
    font->num_fixed_sizes = font->face->num_fixed_sizes;
  else
    font->num_fixed_sizes = -1;

  _alfont_new_cache_glyph(font);

  if (font->num_fixed_sizes < 0) {
    font->fixed_sizes = malloc(sizeof(int));
    _alfont_reget_fixed_sizes(font);

    alfont_set_font_size(font, 8);
  }
  else {
    font->fixed_sizes = malloc(sizeof(int) * (font->num_fixed_sizes + 1));
    _alfont_reget_fixed_sizes(font);

    /* set as current size the first found fixed size */
    alfont_set_font_size(font, font->fixed_sizes[0]);
  }

  alfont_set_char_extra_spacing(font, 0);

  return font;
}


ALFONT_FONT *alfont_load_font_from_mem(const char *data, int data_len) {
  int error;
  char *new_data;

  /* try to allocate the memory */
  ALFONT_FONT *font = malloc(sizeof(ALFONT_FONT));
  new_data = malloc(data_len);

  if ((font == NULL) || (new_data == NULL)) {
    if (font)
      free(font);
    if (new_data)
      free(new_data);
    return NULL;
  }

  /* clear the struct */
  memset(font, 0, sizeof(ALFONT_FONT));
  font->cached_glyphs = NULL;

  /* copy user data to internal buffer */
  font->data = new_data;
  font->data_size = data_len;
  memcpy((void *)font->data, (void *)data, data_len);

  /* load the font */
  error = FT_New_Memory_Face(ft_library, font->data, font->data_size, 0, &font->face);

  if (error) {
    free(font->data);
    free(font);
    return NULL;
  }

  /* get if the font contains only fixed sizes */
  if (!(font->face->face_flags & FT_FACE_FLAG_SCALABLE))
    font->num_fixed_sizes = font->face->num_fixed_sizes;
  else
    font->num_fixed_sizes = -1;

  _alfont_new_cache_glyph(font);

  if (font->num_fixed_sizes < 0) {
    font->fixed_sizes = malloc(sizeof(int));
    _alfont_reget_fixed_sizes(font);

    alfont_set_font_size(font, 8);
  }
  else {
    font->fixed_sizes = malloc(sizeof(int) * (font->num_fixed_sizes + 1));
    _alfont_reget_fixed_sizes(font);

    /* set as current size the first found fixed size */
    alfont_set_font_size(font, font->fixed_sizes[0]);
  }

  alfont_set_char_extra_spacing(font, 0);

  return font;
}


int alfont_text_mode(int mode) {
  int old_mode = alfont_textmode;
  alfont_textmode = mode;
  return old_mode;
}


void alfont_destroy_font(ALFONT_FONT *f) {
  if (f == NULL)
    return;

  /* delete old glyphs */
  _alfont_delete_glyphs(f);

  /* delete the face */
  FT_Done_Face(f->face);

  if (f->fixed_sizes)
    free(f->fixed_sizes);

  /* deallocate the data */
  if (f->data)
    free(f->data);

  free(f);
}


void alfont_textout_aa(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color) {
  alfont_textout_aa_ex(bmp, f, s, x, y, color, alfont_textmode);
}

void alfont_textout_aa_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int backg) {
  int character;
  int alpha_table[256];
  int last_glyph_index;

  /* is it under or over or too far to the right of the clipping rect then
     we can assume the string is clipped */
  if ((y + f->face_h < bmp->ct) || (y > bmp->cb) || (x > bmp->cr))
    return;

  /* if we are doing opaque mode, draw a rect and init our table*/
  if (backg >= 0) {
    int i, r, g, b, br, bg, bb, ir, ig, ib;
    int blendr, blendg, blendb;
    
    rectfill(bmp, x, y, x + alfont_text_length(f, s) - 1, y + f->face_h - 1, backg);

    /* get the color rgb */
    r = getr(color); g = getg(color); b = getb(color);

    /* get the background rgb */   
    br = getr(backg); bg = getg(backg); bb = getb(backg);

    /* get increments */
    ir = (r == br) ? 0 : (r + 1) - br;
    ig = (g == bg) ? 0 : (g + 1) - bg;
    ib = (b == bb) ? 0 : (b + 1) - bb;

    blendr = br << 8;
    blendg = bg << 8;
    blendb = bb << 8;
    
    /* blend both values and make our alpha table */
    for (i = 0; i < 256; i++) {
      alpha_table[i] = makecol(blendr >> 8, blendg >> 8, blendb >> 8);
      blendr += ir;
      blendg += ig;
      blendb += ib;
    }
  }

  /* draw char by char (using allegro unicode funcs) */
  acquire_bitmap(bmp);
  last_glyph_index = 0;
  for (character = ugetxc(&s); character != 0; character = ugetxc(&s)) {
    int real_x, real_y, glyph_index;
    struct _ALFONT_CACHED_GLYPH cglyph;
    
    /* if left side of char farther than right side of clipping, we are done */
    if (x > bmp->cr)
      break;

    /* get the character out of the font */
    if (f->face->charmap)
      glyph_index = FT_Get_Char_Index(f->face, character);
    else
      glyph_index = character;

    /* cache the glyph */
    _alfont_cache_glyph(f, glyph_index);
    cglyph = f->cached_glyphs[glyph_index];

    /* calculate drawing coords */
    real_x = x + cglyph.aaleft;
    real_y = (y - cglyph.aatop) + f->face_ascender;

    /* apply kerning */
    /*if (last_glyph_index) {
      FT_Vector v;
      FT_Get_Kerning(f->face, last_glyph_index, glyph_index, ft_kerning_default, &v);
      real_x += v.x >> 6;
      real_y += v.y >> 6;
    }*/
    last_glyph_index = glyph_index;

    /* draw only if exists */
    if ((cglyph.aa_available) && (cglyph.aabmp)) {
        
      int bmp_x, bmp_y;
      unsigned char *bmp_p = cglyph.aabmp;
      const int max_bmp_x = cglyph.aawidth + real_x;
      const int max_bmp_y = cglyph.aaheight + real_y;
      
      /* if in opaque mode */
      if (backg >= 0) {

        for (bmp_y = real_y; bmp_y < max_bmp_y; bmp_y++) {
          for (bmp_x = real_x; bmp_x < max_bmp_x; bmp_x++) {
            const int alpha = *bmp_p++;
            
            if (alpha)
               putpixel(bmp, bmp_x, bmp_y, alpha_table[alpha]);
          }
        }
      }

      /* if in transparent mode */
      else {
        for (bmp_y = real_y; bmp_y < max_bmp_y; bmp_y++) {
          for (bmp_x = real_x; bmp_x < max_bmp_x; bmp_x++) {
            const int alpha = *bmp_p++;
            
            if (alpha) {
               if (alpha >= 255)
                 solid_mode();
               else {
                 drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
                 set_trans_blender(0, 0, 0, alpha);
               }
               putpixel(bmp, bmp_x, bmp_y, color);
            }
          }
        }
      }
    }

    /* advance */
    if (cglyph.advancex)
      x += cglyph.advancex + f->ch_spacing;
    if (cglyph.advancey)
      y += cglyph.advancey + f->ch_spacing;
  }
  release_bitmap(bmp);

  /* reset blender */
  if (backg < 0)
    solid_mode();
}


void alfont_textout(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color) {
  alfont_textout_ex(bmp, f, s, x, y, color, alfont_textmode);
}


void alfont_textout_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int backg) {
  int character, last_glyph_index;

  /* is it under or over or too far to the right of the clipping rect then
     we can assume the string is clipped */
  if ((y + f->face_h < bmp->ct) || (y > bmp->cb) || (x > bmp->cr))
    return;

  /* if we are doing opaque mode, draw a rect */
  if (backg >= 0)
    rectfill(bmp, x, y, x + alfont_text_length(f, s) - 1, y + f->face_h - 1, backg);

  /* draw char by char (using allegro unicode funcs) */
  acquire_bitmap(bmp);
  last_glyph_index = 0;
  for (character = ugetxc(&s); character != 0; character = ugetxc(&s)) {
    int real_x, real_y, glyph_index;
    struct _ALFONT_CACHED_GLYPH cglyph;
    
    /* if left side of char farther than right side of clipping, we are done */
    if (x > bmp->cr)
      break;

    /* get the character out of the font */
    if (f->face->charmap)
      glyph_index = FT_Get_Char_Index(f->face, character);
    else
      glyph_index = character;

    /* cache the glyph */
    _alfont_cache_glyph(f, glyph_index);
    cglyph = f->cached_glyphs[glyph_index];

    /* calculate drawing coords */
    real_x = x + cglyph.left;
    real_y = (y - cglyph.top) + f->face_ascender;

    /* apply kerning */
    /*if (last_glyph_index) {
      FT_Vector v;
      FT_Get_Kerning(f->face, last_glyph_index, glyph_index, ft_kerning_default, &v);
      real_x += v.x >> 6;
      real_y += v.y >> 6;
    }*/
    last_glyph_index = glyph_index;

    /* draw only if exists */
    if ((cglyph.mono_available) && (cglyph.bmp)) {
      unsigned char *bmp_p = cglyph.bmp;
    
      /* monochrome drawing */
      int bmp_x, bmp_y;

      /* copy the character bitmap to our allegro one */
      const int max_bmp_x = cglyph.width + real_x;
      const int max_bmp_y = cglyph.height + real_y;
      for (bmp_y = real_y; bmp_y < max_bmp_y; bmp_y++) {
        for (bmp_x = real_x; bmp_x < max_bmp_x; bmp_x++) {
          if (*bmp_p++)
            putpixel(bmp, bmp_x, bmp_y, color);
        }
      }
    }

    /* advance */
    if (cglyph.advancex)
      x += cglyph.advancex + f->ch_spacing;
    if (cglyph.advancey)
      y += cglyph.advancey + f->ch_spacing;
  }
  release_bitmap(bmp);

}


int alfont_text_height(ALFONT_FONT *f) {
  return f->face_h;
}


int alfont_text_length(ALFONT_FONT *f, const char *str) {
  int total_length = 0, character, last_glyph_index;

  /* virtually draw char by char */
  last_glyph_index = 0;
  for (character = ugetxc(&str); character != 0; character = ugetxc(&str)) {
    /* get the character out of the font */
    int glyph_index;
    if (f->face->charmap)
      glyph_index = FT_Get_Char_Index(f->face, character);
    else
      glyph_index = character;
    
    /* apply kerning */
    /*if (last_glyph_index) {
      FT_Vector v;
      FT_Get_Kerning(f->face, last_glyph_index, glyph_index, ft_kerning_default, &v);
      total_length += v.x >> 6;
    }*/
    last_glyph_index = glyph_index;

    /* cache */
    _alfont_cache_glyph(f, glyph_index);

    /* advance */
    if (f->cached_glyphs[glyph_index].advancex)
      total_length += f->cached_glyphs[glyph_index].advancex + f->ch_spacing;
  }

  return total_length;
}


void alfont_textout_centre_aa(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color) {
  alfont_textout_centre_aa_ex(bmp, f, s, x, y, color, alfont_textmode);
}


void alfont_textout_centre_aa_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int backg) {
  x -= alfont_text_length(f, s) / 2;
  alfont_textout_aa_ex(bmp, f, s, x, y, color, backg);
}


void alfont_textout_centre(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color) {
  alfont_textout_centre_ex(bmp, f, s, x, y, color, alfont_textmode);
}


void alfont_textout_centre_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int backg) {
  x -= alfont_text_length(f, s) / 2;
  alfont_textout_ex(bmp, f, s, x, y, color, backg);
}


void alfont_textout_right_aa(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color) {
  alfont_textout_right_aa_ex(bmp, f, s, x, y, color, alfont_textmode);
}


void alfont_textout_right_aa_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int backg) {
  x -= alfont_text_length(f, s);
  alfont_textout_aa_ex(bmp, f, s, x, y, color, backg);
}


void alfont_textout_right(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color) {
  alfont_textout_right_ex(bmp, f, s, x, y, color, alfont_textmode);
}


void alfont_textout_right_ex(BITMAP *bmp, ALFONT_FONT *f, const char *s, int x, int y, int color, int backg) {
  x -= alfont_text_length(f, s);
  alfont_textout_ex(bmp, f, s, x, y, color, backg);
}


void alfont_textprintf(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...) {
  char buf[512];

  va_list ap;
  va_start(ap, format);
  uvszprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  alfont_textout(bmp, f, buf, x, y, color);
}


void alfont_textprintf_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int backg, const char *format, ...) {
  char buf[512];

  va_list ap;
  va_start(ap, format);
  uvszprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  alfont_textout_ex(bmp, f, buf, x, y, color, backg);
}


void alfont_textprintf_aa(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...) {
  char buf[512];

  va_list ap;
  va_start(ap, format);
  uvszprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  alfont_textout_aa(bmp, f, buf, x, y, color);
}


void alfont_textprintf_aa_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int backg, const char *format, ...) {
  char buf[512];

  va_list ap;
  va_start(ap, format);
  uvszprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  alfont_textout_aa_ex(bmp, f, buf, x, y, color, backg);
}


void alfont_textprintf_centre(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...) {
  char buf[512];

  va_list ap;
  va_start(ap, format);
  uvszprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  alfont_textout_centre(bmp, f, buf, x, y, color);
}


void alfont_textprintf_centre_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int backg, const char *format, ...) {
  char buf[512];

  va_list ap;
  va_start(ap, format);
  uvszprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  alfont_textout_centre_ex(bmp, f, buf, x, y, color, backg);
}


void alfont_textprintf_centre_aa(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...) {
  char buf[512];

  va_list ap;
  va_start(ap, format);
  uvszprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  alfont_textout_centre_aa(bmp, f, buf, x, y, color);
}


void alfont_textprintf_centre_aa_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int backg, const char *format, ...) {
  char buf[512];

  va_list ap;
  va_start(ap, format);
  uvszprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  alfont_textout_centre_aa_ex(bmp, f, buf, x, y, color, backg);
}


void alfont_textprintf_right(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...) {
  char buf[512];

  va_list ap;
  va_start(ap, format);
  uvszprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  alfont_textout_right(bmp, f, buf, x, y, color);
}


void alfont_textprintf_right_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int backg, const char *format, ...) {
  char buf[512];

  va_list ap;
  va_start(ap, format);
  uvszprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  alfont_textout_right_ex(bmp, f, buf, x, y, color, backg);
}


void alfont_textprintf_right_aa(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, const char *format, ...) {
  char buf[512];

  va_list ap;
  va_start(ap, format);
  uvszprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  alfont_textout_right_aa(bmp, f, buf, x, y, color);
}


void alfont_textprintf_right_aa_ex(BITMAP *bmp, ALFONT_FONT *f, int x, int y, int color, int backg, const char *format, ...) {
  char buf[512];

  va_list ap;
  va_start(ap, format);
  uvszprintf(buf, sizeof(buf), format, ap);
  va_end(ap);

  alfont_textout_right_aa_ex(bmp, f, buf, x, y, color, backg);
}


int alfont_is_fixed_font(ALFONT_FONT *f) {
  if (f->num_fixed_sizes < 0)
    return FALSE;
  else
    return TRUE;
}


int alfont_is_scalable_font(ALFONT_FONT *f) {
  if (f->num_fixed_sizes < 0)
    return TRUE;
  else
    return FALSE;
}


const int *alfont_get_available_fixed_sizes(ALFONT_FONT *f) {
  /* we reget them just to make sure the contents where not changed */
  _alfont_reget_fixed_sizes(f);
  return f->fixed_sizes;
}


int alfont_get_nof_available_fixed_sizes(ALFONT_FONT *f) {
  return f->num_fixed_sizes;
}


int alfont_get_char_extra_spacing(ALFONT_FONT *f) {
  return f->ch_spacing;
}


void alfont_set_char_extra_spacing(ALFONT_FONT *f, int spacing) {
  if (spacing  < 0)
    f->ch_spacing = 0;
  else if (spacing > 4096)
    f->ch_spacing = 4096;
  else
    f->ch_spacing = spacing;
}
