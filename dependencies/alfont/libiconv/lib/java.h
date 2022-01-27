/*
 * Copyright (C) 1999-2000 Free Software Foundation, Inc.
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

/*
 * JAVA
 * This is ISO 8859-1 with \uXXXX escape sequences, denoting Unicode characters.
 */

static int
java_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  unsigned char c;
  ucs4_t wc;
  int i;

  c = s[0];
  if (c != '\\') {
    *pwc = c;
    return 1;
  }
  if (n < 2)
    return RET_TOOFEW(0);
  if (s[1] != 'u')
    goto simply_backslash;
  wc = 0;
  for (i = 2; i < 6; i++) {
    if (n <= i)
      return RET_TOOFEW(0);
    c = s[i];
    if (c >= '0' && c <= '9')
      c -= '0';
    else if (c >= 'A' && c <= 'Z')
      c -= 'A'-10;
    else if (c >= 'a' && c <= 'z')
      c -= 'a'-10;
    else
      goto simply_backslash;
    wc |= (ucs4_t) c << (4 * (5-i));
  }
  *pwc = wc;
  return 6;
simply_backslash:
  *pwc = '\\';
  return 1;
}

static int
java_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  if (wc < 0x80) {
    *r = wc;
    return 1;
  } else {
    if (n >= 6) {
      unsigned int i;
      r[0] = '\\';
      r[1] = 'u';
      i = (wc >> 12) & 0x0f; r[2] = (i < 10 ? '0'+i : 'a'-10+i);
      i = (wc >> 8) & 0x0f;  r[3] = (i < 10 ? '0'+i : 'a'-10+i);
      i = (wc >> 4) & 0x0f;  r[4] = (i < 10 ? '0'+i : 'a'-10+i);
      i = wc & 0x0f;         r[5] = (i < 10 ? '0'+i : 'a'-10+i);
      return 6;
    } else
      return RET_TOOSMALL;
  }
}
