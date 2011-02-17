# Microsoft Developer Studio Project File - Name="almp3_static" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=almp3_static - Win32 MD lib
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "almp3_static.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "almp3_static.mak" CFG="almp3_static - Win32 MD lib"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "almp3_static - Win32 MD lib" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "almp3_static___Win32_MD_lib"
# PROP BASE Intermediate_Dir "almp3_static___Win32_MD_lib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "obj\msvc\static"
# PROP Intermediate_Dir "obj\msvc\static"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /I "include" /I "decoder/src" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_M_IX_86" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "include" /I "decoder" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FD /c
# SUBTRACT CPP /Z<none> /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"lib/almp3.lib"
# ADD LIB32 /nologo /out:"lib\msvc\almp3.lib"
# Begin Target

# Name "almp3_static - Win32 MD lib"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\almp3.c
# End Source File
# Begin Source File

SOURCE=.\decoder\common.c
# End Source File
# Begin Source File

SOURCE=.\decoder\dct64_i386.c
# End Source File
# Begin Source File

SOURCE=.\decoder\decode_i386.c
# End Source File
# Begin Source File

SOURCE=.\decoder\interface.c
# End Source File
# Begin Source File

SOURCE=.\decoder\layer2.c
# End Source File
# Begin Source File

SOURCE=.\decoder\layer3.c
# End Source File
# Begin Source File

SOURCE=.\decoder\tabinit.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\almp3.h
# End Source File
# Begin Source File

SOURCE=..\alm\include\almp3dll.h
# End Source File
# Begin Source File

SOURCE=.\decoder\huffman.h
# End Source File
# Begin Source File

SOURCE=.\decoder\l2tables.h
# End Source File
# Begin Source File

SOURCE=.\decoder\mpg123.h
# End Source File
# Begin Source File

SOURCE=.\decoder\mpglib.h
# End Source File
# End Group
# End Target
# End Project
