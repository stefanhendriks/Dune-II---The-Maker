# Microsoft Developer Studio Project File - Name="alfont_static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=alfont_static - Win32 MD
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "alfont_static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "alfont_static.mak" CFG="alfont_static - Win32 MD"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "alfont_static - Win32 MD" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "MD"
# PROP BASE Intermediate_Dir "MD"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj\msvc\static"
# PROP Intermediate_Dir "obj\msvc\static"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "include" /I "freetype\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x40a /d "NDEBUG"
# ADD RSC /l 0x40a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\msvc\alfont.lib"
# Begin Target

# Name "alfont_static - Win32 MD"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\alfont.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\autohint\autohint.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\bdf\bdf.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\cff\cff.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\base\ftbase.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\cache\ftcache.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\base\ftdebug.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\base\ftglyph.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\gzip\ftgzip.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\base\ftinit.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\base\ftmm.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\base\ftsystem.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\pcf\pcf.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\pfr\pfr.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\psaux\psaux.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\pshinter\pshinter.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\psnames\psmodule.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\raster\raster.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\sfnt\sfnt.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\smooth\smooth.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\truetype\truetype.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\type1\type1.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\cid\type1cid.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\type42\type42.c
# End Source File
# Begin Source File

SOURCE=.\freetype\src\winfonts\winfnt.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\alfont.h
# End Source File
# Begin Source File

SOURCE=.\include\alfontdll.h
# End Source File
# Begin Source File

SOURCE=".\freetype\builds\win32\visualc\include\freetype\freetype.h"
# End Source File
# Begin Source File

SOURCE=".\freetype\builds\win32\visualc\include\freetype\config\ftconfig.h"
# End Source File
# Begin Source File

SOURCE=".\freetype\builds\win32\visualc\include\freetype\fterrors.h"
# End Source File
# Begin Source File

SOURCE=".\freetype\builds\win32\visualc\include\freetype\config\ftoption.h"
# End Source File
# Begin Source File

SOURCE=".\freetype\builds\win32\visualc\include\freetype\fttypes.h"
# End Source File
# End Group
# End Target
# End Project
