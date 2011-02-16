FBlend
~~~~~~

Copyright (c) Robert J Ohannessian, 2001

Comments, Suggestions, Questions, Bugs etc.:
	voidstar@ifrance.com

Official web site: http://pages.infinit.net/voidstar/

The source code for this package is placed under a zlib-style
license. See license.txt for details.


Disclaimer
----------

#include <std_disclaimer.h>
Use this program at your own risk. This program was not intended to do
anything malicious, but we are not responsible for any damage or loss
that may arise by the use of this program.



What is FBlend?
---------------

Do you think the Allegro blenders are too slow? Do you want to do special
effects in 15/16/32-bpp but can't? Then FBlend is for you! FBlend is
a series of special routines that do color blending -fast-.

FBlend is anywhere from 2 to 14 times faster than Allegro!



Supported platforms:
--------------------

FBlend comes with makefiles for DJGPP, MSVC, Mingw and Unix.
It requires the same setup as Allegro.

Makefiles for BeOS, and QNX are welcomed!



Requirements:
-------------

* Allegro 4.0.0 or later

* GCC (DJGPP, Mingw, Linux's gcc...)
  -> This is required even if you use MSVC! <-
  Make sure you have:
    - Binutils v2.11.2 or later (v2.11.90 is recommened)
    - GNU Make v 3.79.1 or later

* IMPORTANT:
  Note that some people have been experiencing difficulty in
  correctly compiling FBlend using DJGPP's Binutils.
  DJGPP users are strongly recommended to upgrade to the latest
  version of Binutils.



How do I install it?
--------------------

If you are familiar with installing the Allegro, then
it should be no problem. Simply locate and run the fix script
that corresponds to your platform/compiler, then run 'make'
then 'make install'.

Example:
	fix msvc
	make
	make install

Installation under Unix is also like Allegro:
	unzip -a fblend.zip
	./fix.sh
	make
	make install

Remember to use the -a switch in Unix to convert the files from
DOS format to Unix format!


How do I use it the library?
----------------------------

* GCC (DJGPP. Mingw, Linux):

  You should make sure that the library is linked to your program.
  For gcc, the lib name is libfblend.a (or libfblendd.a for the
  debug version), so you should link with the -lfblend option.


* MSVC:

  For MSVC, it's fblend.lib that should be linked with the executable
  (fblendd.lib for the debug version).


API
---

See the fblend_docs pakcage for API details.
