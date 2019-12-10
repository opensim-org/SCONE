;NSIS Modern User Interface
;Start Menu Folder Selection Example Script
;Written by Joost Verburg

!define SCONE_VERSION "1.3.2"
!define MSVC_VERSION "vc2019-x64"
!define BIN_FOLDER "..\bin\${MSVC_VERSION}\Release"
!define OSG_PLUGINS_FOLDER "osgPlugins-3.4.1"
!define SCONE_DOCUMENTS_FOLDER "$DOCUMENTS\SCONE"
!define VCREDIST_FILE "vc_redist.x64.exe"

;--------------------------------
;Include Modern UI
!include "MUI2.nsh"

;--------------------------------
;General

;Name and file
Name "SCONE"
OutFile "SCONE-${SCONE_VERSION}.exe"
SetCompressor /SOLID lzma

;Default installation folder
InstallDir "$PROGRAMFILES64\SCONE"

;Get installation folder from registry if available
InstallDirRegKey HKCU "Software\SCONE\INSTDIR" ""

;Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Variables
Var StartMenuFolder

;--------------------------------
;Interface Settings

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "install_header.bmp" ; optional
!define MUI_ABORTWARNING

;--------------------------------
;Pages

;!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
  
;Start Menu Folder Page Configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\SCONE" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

;--------------------------------
;Languages
 
!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Program Files" SecMain
	; root folder
	SetOverwrite on
	SetOutPath "$INSTDIR"
	File "..\.version"
	
	; bin files
	SetOutPath "$INSTDIR\bin"
	File "${BIN_FOLDER}\*.exe"
	File "${BIN_FOLDER}\*.dll"
	File "${VCREDIST_FILE}"
	SetOutPath "$INSTDIR\bin\${OSG_PLUGINS_FOLDER}"
	File "${BIN_FOLDER}\${OSG_PLUGINS_FOLDER}\*.dll"
	SetOutPath "$INSTDIR\bin\platforms"
	File "${BIN_FOLDER}\platforms\*.dll"
	
	; resource files
	SetOutPath "$INSTDIR\resources"
	File /r "..\resources\*.*"

	; Store installation folder
	WriteRegStr HKCU "Software\SCONE\INSTDIR" "" $INSTDIR
	WriteRegStr HKCU "Software\SCONE\DOCDIR" "" ${SCONE_DOCUMENTS_FOLDER}

	; Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"

	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application

	; Create shortcuts
	CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
	CreateShortCut "$SMPROGRAMS\$StartMenuFolder\SCONE.lnk" "$INSTDIR\bin\sconestudio.exe"
	CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall SCONE.lnk" "$INSTDIR\Uninstall.exe"
	
	; Install VC Redistributables
	ExecWait `"$INSTDIR\bin\${VCREDIST_FILE}" /quiet`

	!insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "Examples" SecTutorials
	; scenarios
	SetOutPath "${SCONE_DOCUMENTS_FOLDER}\Tutorials"
	File /r "..\scenarios\Tutorials\*.*"
SectionEnd

; Set section to read-only
Function .onInit
  IntOp $0 ${SF_SELECTED} | ${SF_RO}
  SectionSetFlags ${SecMain} $0
FunctionEnd

;--------------------------------
;Descriptions

	;Language strings
	LangString DESC_SecMain ${LANG_ENGLISH} "Files needed to run SCONE"
	;LangString DESC_SecModels ${LANG_ENGLISH} "Example OpenSim models"
	LangString DESC_SecTutorials ${LANG_ENGLISH} "SCONE tutorials and examples"

	;Assign language strings to sections
	!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
		!insertmacro MUI_DESCRIPTION_TEXT ${SecMain} $(DESC_SecMain)
		;!insertmacro MUI_DESCRIPTION_TEXT ${SecModels} $(DESC_SecModels)
		!insertmacro MUI_DESCRIPTION_TEXT ${SecTutorials} $(DESC_SecTutorials)
	!insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...
  Delete "$INSTDIR\Uninstall.exe"
  RMDir "$INSTDIR"
  
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\SCONE.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  
  DeleteRegKey /ifempty HKCU "Software\SCONE\INSTDIR"
  DeleteRegKey /ifempty HKCU "Software\SCONE\DOCDIR"

SectionEnd
