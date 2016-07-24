# Microsoft Developer Studio Generated NMAKE File, Based on DXBomns.dsp
!IF "$(CFG)" == ""
CFG=DXBomns - Win32 Debug
!MESSAGE No configuration specified. Defaulting to DXBomns - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DXBomns - Win32 Release" && "$(CFG)" != "DXBomns - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DXBomns.mak" CFG="DXBomns - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DXBomns - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DXBomns - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DXBomns - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\DXBomns.exe"


CLEAN :
	-@erase "$(INTDIR)\Bomn.obj"
	-@erase "$(INTDIR)\DXBomns.obj"
	-@erase "$(INTDIR)\DXBomns.res"
	-@erase "$(INTDIR)\DXWindow.obj"
	-@erase "$(INTDIR)\Editor.obj"
	-@erase "$(INTDIR)\EditorDialog.obj"
	-@erase "$(INTDIR)\Font.obj"
	-@erase "$(INTDIR)\Game.obj"
	-@erase "$(INTDIR)\Graphics.obj"
	-@erase "$(INTDIR)\Input.obj"
	-@erase "$(INTDIR)\Level.obj"
	-@erase "$(INTDIR)\Opts.obj"
	-@erase "$(INTDIR)\Player.obj"
	-@erase "$(INTDIR)\Sound.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\World.obj"
	-@erase "$(INTDIR)\XNotDirect.obj"
	-@erase "$(OUTDIR)\DXBomns.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\DXBomns.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\DXBomns.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DXBomns.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winmm.lib comdlg32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\DXBomns.pdb" /machine:I386 /out:"$(OUTDIR)\DXBomns.exe" 
LINK32_OBJS= \
	"$(INTDIR)\Bomn.obj" \
	"$(INTDIR)\DXBomns.obj" \
	"$(INTDIR)\DXWindow.obj" \
	"$(INTDIR)\Editor.obj" \
	"$(INTDIR)\EditorDialog.obj" \
	"$(INTDIR)\Font.obj" \
	"$(INTDIR)\Game.obj" \
	"$(INTDIR)\Graphics.obj" \
	"$(INTDIR)\Input.obj" \
	"$(INTDIR)\Level.obj" \
	"$(INTDIR)\Opts.obj" \
	"$(INTDIR)\Player.obj" \
	"$(INTDIR)\Sound.obj" \
	"$(INTDIR)\World.obj" \
	"$(INTDIR)\XNotDirect.obj" \
	"$(INTDIR)\DXBomns.res"

"$(OUTDIR)\DXBomns.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DXBomns - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\DXBomns.exe"


CLEAN :
	-@erase "$(INTDIR)\Bomn.obj"
	-@erase "$(INTDIR)\DXBomns.obj"
	-@erase "$(INTDIR)\DXBomns.res"
	-@erase "$(INTDIR)\DXWindow.obj"
	-@erase "$(INTDIR)\Editor.obj"
	-@erase "$(INTDIR)\EditorDialog.obj"
	-@erase "$(INTDIR)\Font.obj"
	-@erase "$(INTDIR)\Game.obj"
	-@erase "$(INTDIR)\Graphics.obj"
	-@erase "$(INTDIR)\Input.obj"
	-@erase "$(INTDIR)\Level.obj"
	-@erase "$(INTDIR)\Opts.obj"
	-@erase "$(INTDIR)\Player.obj"
	-@erase "$(INTDIR)\Sound.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\World.obj"
	-@erase "$(INTDIR)\XNotDirect.obj"
	-@erase "$(OUTDIR)\DXBomns.exe"
	-@erase "$(OUTDIR)\DXBomns.ilk"
	-@erase "$(OUTDIR)\DXBomns.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\DXBomns.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\DXBomns.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\DXBomns.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winmm.lib comdlg32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\DXBomns.pdb" /debug /machine:I386 /out:"$(OUTDIR)\DXBomns.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\Bomn.obj" \
	"$(INTDIR)\DXBomns.obj" \
	"$(INTDIR)\DXWindow.obj" \
	"$(INTDIR)\Editor.obj" \
	"$(INTDIR)\EditorDialog.obj" \
	"$(INTDIR)\Font.obj" \
	"$(INTDIR)\Game.obj" \
	"$(INTDIR)\Graphics.obj" \
	"$(INTDIR)\Input.obj" \
	"$(INTDIR)\Level.obj" \
	"$(INTDIR)\Opts.obj" \
	"$(INTDIR)\Player.obj" \
	"$(INTDIR)\Sound.obj" \
	"$(INTDIR)\World.obj" \
	"$(INTDIR)\XNotDirect.obj" \
	"$(INTDIR)\DXBomns.res"

"$(OUTDIR)\DXBomns.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("DXBomns.dep")
!INCLUDE "DXBomns.dep"
!ELSE 
!MESSAGE Warning: cannot find "DXBomns.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "DXBomns - Win32 Release" || "$(CFG)" == "DXBomns - Win32 Debug"
SOURCE=.\Bomn.cpp

"$(INTDIR)\Bomn.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DXBomns.cpp

"$(INTDIR)\DXBomns.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DXWindow.cpp

"$(INTDIR)\DXWindow.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Editor.cpp

"$(INTDIR)\Editor.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\EditorDialog.cpp

"$(INTDIR)\EditorDialog.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Font.cpp

"$(INTDIR)\Font.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Game.cpp

"$(INTDIR)\Game.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Graphics.cpp

"$(INTDIR)\Graphics.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Input.cpp

"$(INTDIR)\Input.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Level.cpp

"$(INTDIR)\Level.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Opts.cpp

"$(INTDIR)\Opts.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Player.cpp

"$(INTDIR)\Player.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\Sound.cpp

"$(INTDIR)\Sound.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\World.cpp

"$(INTDIR)\World.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\XNotDirect.cpp

"$(INTDIR)\XNotDirect.obj" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DXBomns.rc

"$(INTDIR)\DXBomns.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

