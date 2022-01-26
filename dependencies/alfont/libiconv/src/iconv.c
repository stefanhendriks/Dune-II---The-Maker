/* Copyright (C) 2000-2001 Free Software Foundation, Inc.
   This file is part of the GNU LIBICONV Library.

   The GNU LIBICONV Library is free software; you can redistribute it
   and/or modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   The GNU LIBICONV Library is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU LIBICONV Library; see the file COPYING.LIB.
   If not, write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA.  */

#include "config.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
#include <errno.h>
#if HAVE_LOCALE_H
#include <locale.h>
#endif
#include <fcntl.h>

/* For systems that distinguish between text and binary I/O.
   O_BINARY is usually declared in <fcntl.h>. */
#if !defined O_BINARY && defined _O_BINARY
  /* For MSC-compatible compilers.  */
# define O_BINARY _O_BINARY
# define O_TEXT _O_TEXT
#endif
#ifdef __BEOS__
  /* BeOS 5 has O_BINARY and O_TEXT, but they have no effect.  */
# undef O_BINARY
# undef O_TEXT
#endif
#if O_BINARY
# if !(defined(__EMX__) || defined(__DJGPP__))
#  define setmode _setmode
#  define fileno _fileno
# endif
# ifdef __DJGPP__
#  include <io.h> /* declares setmode() */
#  include <unistd.h> /* declares isatty() */
#  /* Avoid putting stdin/stdout in binary mode if it is connected to the
#     console, because that would make it impossible for the user to
#     interrupt the program through Ctrl-C or Ctrl-Break.  */
#  define SET_BINARY(fd) (!isatty(fd) ? (setmode(fd,O_BINARY), 0) : 0)
# else
#  define SET_BINARY(fd) setmode(fd,O_BINARY)
# endif
#endif

#if O_BINARY
  static int force_binary = 0;
#endif

static void usage (int exitcode)
{
  const char* helpstring =
#if O_BINARY
    "Usage: iconv [--binary] [-f fromcode] [-t tocode] [file ...]\n";
#else
    "Usage: iconv [-f fromcode] [-t tocode] [file ...]\n";
#endif
  fprintf(exitcode ? stderr : stdout, helpstring);
  exit(exitcode);
}

static void print_version (void)
{
  printf("iconv (GNU libiconv %d.%d)\n",
         _libiconv_version >> 8, _libiconv_version & 0xff);
  printf("\
Copyright (C) 2000-2001 Free Software Foundation, Inc.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
Written by Bruno Haible.\n");
  exit(0);
}

static int convert (iconv_t cd, FILE* infile, const char* infilename)
{
  char inbuf[4096+4096];
  size_t inbufrest = 0;
  char outbuf[4096];

#if O_BINARY
  if (force_binary)
    SET_BINARY(fileno(infile));
#endif
  iconv(cd,NULL,NULL,NULL,NULL);
  for (;;) {
    size_t inbufsize = fread(inbuf+4096,1,4096,infile);
    if (inbufsize == 0) {
      if (inbufrest == 0)
        break;
      else {
        fprintf(stderr,"iconv: %s: incomplete character or shift sequence\n",infilename);
        return 1;
      }
    } else {
      const char* inptr = inbuf+4096-inbufrest;
      size_t insize = inbufrest+inbufsize;
      inbufrest = 0;
      while (insize > 0) {
        char* outptr = outbuf;
        size_t outsize = sizeof(outbuf);
        size_t res = iconv(cd,(ICONV_CONST char**)&inptr,&insize,&outptr,&outsize);
        if (outptr != outbuf) {
          int saved_errno = errno;
          if (fwrite(outbuf,1,outptr-outbuf,stdout) < outptr-outbuf)
            return 1;
          errno = saved_errno;
        }
        if (res == (size_t)(-1)) {
          if (errno == EILSEQ) {
            fprintf(stderr,"iconv: %s: cannot convert\n",infilename);
            return 1;
          } else if (errno == EINVAL) {
            if (inbufsize == 0 || insize > 4096) {
              fprintf(stderr,"iconv: %s: incomplete character or shift sequence\n",infilename);
              return 1;
            } else {
              inbufrest = insize;
              if (insize > 0) {
                /* Like memcpy(inbuf+4096-insize,inptr,insize), except that
                   we cannot use memcpy here, because source and destination
                   regions may overlap. */
                char* restptr = inbuf+4096-insize;
                do { *restptr++ = *inptr++; } while (--insize > 0);
              }
              break;
            }
          } else if (errno != E2BIG) {
            int saved_errno = errno;
            fprintf(stderr,"iconv: %s: ",infilename);
            errno = saved_errno;
            perror("");
            return 1;
          }
        }
      }
    }
  }
  {
    char* outptr = outbuf;
    size_t outsize = sizeof(outbuf);
    size_t res = iconv(cd,NULL,NULL,&outptr,&outsize);
    if (outptr != outbuf) {
      int saved_errno = errno;
      if (fwrite(outbuf,1,outptr-outbuf,stdout) < outptr-outbuf)
        return 1;
      errno = saved_errno;
    }
    if (res == (size_t)(-1)) {
      if (errno == EILSEQ) {
        fprintf(stderr,"iconv: %s: cannot convert\n",infilename);
        return 1;
      } else if (errno == EINVAL) {
        fprintf(stderr,"iconv: %s: incomplete character or shift sequence\n",infilename);
        return 1;
      } else {
        int saved_errno = errno;
        fprintf(stderr,"iconv: %s: ",infilename);
        errno = saved_errno;
        perror("");
        return 1;
      }
    }
  }
  if (ferror(infile)) {
    fprintf(stderr,"iconv: %s: I/O error\n",infilename);
    return 1;
  }
  return 0;
}

int main (int argc, char* argv[])
{
  const char* fromcode = NULL;
  const char* tocode = NULL;
  iconv_t cd;
  int i;
  int status;

#if HAVE_SETLOCALE
  /* Needed for the locale dependent encodings, "char" and "wchar_t". */
  setlocale(LC_CTYPE,"");
#endif
  for (i = 1; i < argc;) {
    if (!strcmp(argv[i],"-f")) {
      if (i == argc-1) usage(1);
      if (fromcode != NULL) usage(1);
      fromcode = argv[i+1];
      i += 2;
      continue;
    }
    if (!strcmp(argv[i],"-t")) {
      if (i == argc-1) usage(1);
      if (tocode != NULL) usage(1);
      tocode = argv[i+1];
      i += 2;
      continue;
    }
    if (!strcmp(argv[i],"--help")) {
      usage(0);
    }
    if (!strcmp(argv[i],"--version")) {
      print_version();
    }
#if O_BINARY
    if (!strcmp(argv[i],"--binary")) {
      force_binary = 1;
      i++;
      continue;
    }
#endif
    if (argv[i][0] == '-')
      usage(1);
    break;
  }
#if O_BINARY
  if (force_binary)
    SET_BINARY(fileno(stdout));
#endif
  if (fromcode == NULL)
    fromcode = "char";
  if (tocode == NULL)
    tocode = "char";
  cd = iconv_open(tocode,fromcode);
  if (cd == (iconv_t)(-1)) {
    if (iconv_open("UCS-4",fromcode) == (iconv_t)(-1))
      fprintf(stderr,"iconv: conversion from %s unsupported\n",fromcode);
    else if (iconv_open(tocode,"UCS-4") == (iconv_t)(-1))
      fprintf(stderr,"iconv: conversion to %s unsupported\n",tocode);
    else
      fprintf(stderr,"iconv: conversion from %s to %s unsupported\n",fromcode,tocode);
    exit(1);
  }
  if (i == argc)
    status = convert(cd,stdin,"(stdin)");
  else {
    status = 0;
    for (; i < argc; i++) {
      const char* infilename = argv[i];
      FILE* infile = fopen(infilename,"r");
      if (infile == NULL) {
        int saved_errno = errno;
        fprintf(stderr,"iconv: %s: ",infilename);
        errno = saved_errno;
        perror("");
        status = 1;
      } else {
        status |= convert(cd,infile,infilename);
        fclose(infile);
      }
    }
  }
  iconv_close(cd);
  if (ferror(stdout)) {
    fprintf(stderr,"iconv: I/O error\n");
    status = 1;
  }
  exit(status);
}
