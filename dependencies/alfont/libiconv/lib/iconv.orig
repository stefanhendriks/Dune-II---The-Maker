/*
 * Copyright (C) 1999-2001 Free Software Foundation, Inc.
 * This file is part of the GNU LIBICONV Library.
 *
 * The GNU LIBICONV Library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * The GNU LIBICONV Library is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the GNU LIBICONV Library; see the file COPYING.LIB.
 * If not, write to the Free Software Foundation, Inc., 59 Temple Place -
 * Suite 330, Boston, MA 02111-1307, USA.
 */

#include <iconv.h>

#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "libcharset.h"

/*
 * Consider those system dependent encodings that are needed for the
 * current system.
 */
#ifdef _AIX
#define USE_AIX
#endif
#ifdef __osf__
#define USE_OSF1
#endif
#ifdef __DJGPP__
#define USE_DOS
#endif

/*
 * Data type for general conversion loop.
 */
struct loop_funcs {
  size_t (*loop_convert) (iconv_t icd,
                          const char* * inbuf, size_t *inbytesleft,
                          char* * outbuf, size_t *outbytesleft);
  size_t (*loop_reset) (iconv_t icd,
                        char* * outbuf, size_t *outbytesleft);
};

/*
 * Converters.
 */
#include "converters.h"

/*
 * Transliteration tables.
 */
#include "cjk_variants.h"
#include "translit.h"

/*
 * Table of all supported encodings.
 */
struct encoding {
  struct mbtowc_funcs ifuncs; /* conversion multibyte -> unicode */
  struct wctomb_funcs ofuncs; /* conversion unicode -> multibyte */
  int oflags;                 /* flags for unicode -> multibyte conversion */
};
enum {
#define DEFENCODING(xxx_names,xxx,xxx_ifuncs1,xxx_ifuncs2,xxx_ofuncs1,xxx_ofuncs2) \
  ei_##xxx ,
#include "encodings.def"
#ifdef USE_AIX
#include "encodings_aix.def"
#endif
#ifdef USE_OSF1
#include "encodings_osf1.def"
#endif
#ifdef USE_DOS
#include "encodings_dos.def"
#endif
#include "encodings_local.def"
#undef DEFENCODING
ei_for_broken_compilers_that_dont_like_trailing_commas
};
#include "flags.h"
static struct encoding const all_encodings[] = {
#define DEFENCODING(xxx_names,xxx,xxx_ifuncs1,xxx_ifuncs2,xxx_ofuncs1,xxx_ofuncs2) \
  { xxx_ifuncs1,xxx_ifuncs2, xxx_ofuncs1,xxx_ofuncs2, ei_##xxx##_oflags },
#include "encodings.def"
#ifdef USE_AIX
#include "encodings_aix.def"
#endif
#ifdef USE_OSF1
#include "encodings_osf1.def"
#endif
#ifdef USE_DOS
#include "encodings_dos.def"
#endif
#undef DEFENCODING
#define DEFENCODING(xxx_names,xxx,xxx_ifuncs1,xxx_ifuncs2,xxx_ofuncs1,xxx_ofuncs2) \
  { xxx_ifuncs1,xxx_ifuncs2, xxx_ofuncs1,xxx_ofuncs2, 0 },
#include "encodings_local.def"
#undef DEFENCODING
};

/*
 * Conversion loops.
 */
#include "loops.h"

/*
 * Alias lookup function.
 * Defines
 *   struct alias { const char* name; unsigned int encoding_index; };
 *   const struct alias * aliases_lookup (const char *str, unsigned int len);
 *   #define MAX_WORD_LENGTH ...
 */
#include "aliases.h"

/*
 * System dependent alias lookup function.
 * Defines
 *   const struct alias * aliases2_lookup (const char *str);
 */
#if defined(USE_AIX) || defined(USE_OSF1) || defined(USE_DOS) /* || ... */
static struct alias sysdep_aliases[] = {
#ifdef USE_AIX
#include "aliases_aix.h"
#endif
#ifdef USE_OSF1
#include "aliases_osf1.h"
#endif
#ifdef USE_DOS
#include "aliases_dos.h"
#endif
};
#ifdef __GNUC__
__inline
#endif
const struct alias *
aliases2_lookup (register const char *str)
{
  struct alias * ptr;
  unsigned int count;
  for (ptr = sysdep_aliases, count = sizeof(sysdep_aliases)/sizeof(sysdep_aliases[0]); count > 0; ptr++, count--)
    if (!strcmp(str,ptr->name))
      return ptr;
  return NULL;
}
#else
#define aliases2_lookup(str)  NULL
#endif

#if 0
/* Like !strcasecmp, except that the both strings can be assumed to be ASCII
   and the first string can be assumed to be in uppercase. */
static int strequal (const char* str1, const char* str2)
{
  unsigned char c1;
  unsigned char c2;
  for (;;) {
    c1 = * (unsigned char *) str1++;
    c2 = * (unsigned char *) str2++;
    if (c1 == 0)
      break;
    if (c2 >= 'a' && c2 <= 'z')
      c2 -= 'a'-'A';
    if (c1 != c2)
      break;
  }
  return (c1 == c2);
}
#endif

iconv_t iconv_open (const char* tocode, const char* fromcode)
{
  struct conv_struct * cd;
  char buf[MAX_WORD_LENGTH+10+1];
  const char* cp;
  char* bp;
  const struct alias * ap;
  unsigned int count;
  unsigned int from_index;
  int from_wchar;
  unsigned int to_index;
  int to_wchar;
  int transliterate = 0;

  /* Before calling aliases_lookup, convert the input string to upper case,
   * and check whether it's entirely ASCII (we call gperf with option "-7"
   * to achieve a smaller table) and non-empty. If it's not entirely ASCII,
   * or if it's too long, it is not a valid encoding name.
   */
  for (to_wchar = 0;;) {
    /* Search tocode in the table. */
    for (cp = tocode, bp = buf, count = MAX_WORD_LENGTH+10+1; ; cp++, bp++) {
      unsigned char c = * (unsigned char *) cp;
      if (c >= 0x80)
        goto invalid;
      if (c >= 'a' && c <= 'z')
        c -= 'a'-'A';
      *bp = c;
      if (c == '\0')
        break;
      if (--count == 0)
        goto invalid;
    }
    if (bp-buf > 10 && memcmp(bp-10,"//TRANSLIT",10)==0) {
      bp -= 10;
      *bp = '\0';
      transliterate = 1;
    }
    ap = aliases_lookup(buf,bp-buf);
    if (ap == NULL) {
      ap = aliases2_lookup(buf);
      if (ap == NULL)
        goto invalid;
    }
    if (ap->encoding_index == ei_local_char) {
      tocode = locale_charset();
      continue;
    }
    if (ap->encoding_index == ei_local_wchar_t) {
#if __STDC_ISO_10646__
      if (sizeof(wchar_t) == 4) {
        to_index = ei_ucs4internal;
        break;
      }
      if (sizeof(wchar_t) == 2) {
        to_index = ei_ucs2internal;
        break;
      }
      if (sizeof(wchar_t) == 1) {
        to_index = ei_iso8859_1;
        break;
      }
#endif
#if HAVE_MBRTOWC
      to_wchar = 1;
      tocode = locale_charset();
      continue;
#endif
      goto invalid;
    }
    to_index = ap->encoding_index;
    break;
  }
  for (from_wchar = 0;;) {
    /* Search fromcode in the table. */
    for (cp = fromcode, bp = buf, count = MAX_WORD_LENGTH+10+1; ; cp++, bp++) {
      unsigned char c = * (unsigned char *) cp;
      if (c >= 0x80)
        goto invalid;
      if (c >= 'a' && c <= 'z')
        c -= 'a'-'A';
      *bp = c;
      if (c == '\0')
        break;
      if (--count == 0)
        goto invalid;
    }
    if (bp-buf > 10 && memcmp(bp-10,"//TRANSLIT",10)==0) {
      bp -= 10;
      *bp = '\0';
    }
    ap = aliases_lookup(buf,bp-buf);
    if (ap == NULL) {
      ap = aliases2_lookup(buf);
      if (ap == NULL)
        goto invalid;
    }
    if (ap->encoding_index == ei_local_char) {
      fromcode = locale_charset();
      continue;
    }
    if (ap->encoding_index == ei_local_wchar_t) {
#if __STDC_ISO_10646__
      if (sizeof(wchar_t) == 4) {
        from_index = ei_ucs4internal;
        break;
      }
      if (sizeof(wchar_t) == 2) {
        from_index = ei_ucs2internal;
        break;
      }
      if (sizeof(wchar_t) == 1) {
        from_index = ei_iso8859_1;
        break;
      }
#endif
#if HAVE_WCRTOMB
      from_wchar = 1;
      fromcode = locale_charset();
      continue;
#endif
      goto invalid;
    }
    from_index = ap->encoding_index;
    break;
  }
  cd = (struct conv_struct *) malloc(from_wchar != to_wchar
                                     ? sizeof(struct wchar_conv_struct)
                                     : sizeof(struct conv_struct));
  if (cd == NULL) {
    errno = ENOMEM;
    return (iconv_t)(-1);
  }
  cd->iindex = from_index;
  cd->ifuncs = all_encodings[from_index].ifuncs;
  cd->oindex = to_index;
  cd->ofuncs = all_encodings[to_index].ofuncs;
  cd->oflags = all_encodings[to_index].oflags;
  /* Initialize the loop functions. */
#if HAVE_MBRTOWC
  if (to_wchar) {
#if HAVE_WCRTOMB
    if (from_wchar) {
      cd->lfuncs.loop_convert = wchar_id_loop_convert;
      cd->lfuncs.loop_reset = wchar_id_loop_reset;
    } else
#endif
    {
      cd->lfuncs.loop_convert = wchar_to_loop_convert;
      cd->lfuncs.loop_reset = wchar_to_loop_reset;
    }
  } else
#endif
  {
#if HAVE_WCRTOMB
    if (from_wchar) {
      cd->lfuncs.loop_convert = wchar_from_loop_convert;
      cd->lfuncs.loop_reset = wchar_from_loop_reset;
    } else
#endif
    {
      cd->lfuncs.loop_convert = unicode_loop_convert;
      cd->lfuncs.loop_reset = unicode_loop_reset;
    }
  }
  /* Initialize the states. */
  memset(&cd->istate,'\0',sizeof(state_t));
  memset(&cd->ostate,'\0',sizeof(state_t));
  /* Initialize the operation flags. */
  cd->transliterate = transliterate;
  /* Initialize additional fields. */
  if (from_wchar != to_wchar) {
    struct wchar_conv_struct * wcd = (struct wchar_conv_struct *) cd;
    memset(&wcd->state,'\0',sizeof(mbstate_t));
  }
  /* Done. */
  return (iconv_t)cd;
invalid:
  errno = EINVAL;
  return (iconv_t)(-1);
}

size_t iconv (iconv_t icd,
              ICONV_CONST char* * inbuf, size_t *inbytesleft,
              char* * outbuf, size_t *outbytesleft)
{
  conv_t cd = (conv_t) icd;
  if (inbuf == NULL || *inbuf == NULL)
    return cd->lfuncs.loop_reset(icd,outbuf,outbytesleft);
  else
    return cd->lfuncs.loop_convert(icd,
                                   (const char* *)inbuf,inbytesleft,
                                   outbuf,outbytesleft);
}

int iconv_close (iconv_t icd)
{
  conv_t cd = (conv_t) icd;
  free(cd);
  return 0;
}

#ifndef LIBICONV_PLUG

int iconvctl (iconv_t icd, int request, void* argument)
{
  conv_t cd = (conv_t) icd;
  switch (request) {
    case ICONV_TRIVIALP:
      *(int *)argument =
        ((cd->lfuncs.loop_convert == unicode_loop_convert
          && cd->iindex == cd->oindex)
         || cd->lfuncs.loop_convert == wchar_id_loop_convert
         ? 1 : 0);
      return 0;
    case ICONV_GET_TRANSLITERATE:
      *(int *)argument = cd->transliterate;
      return 0;
    case ICONV_SET_TRANSLITERATE:
      cd->transliterate = (*(const int *)argument ? 1 : 0);
      return 0;
    default:
      errno = EINVAL;
      return -1;
  }
}

int _libiconv_version = _LIBICONV_VERSION;

#endif
