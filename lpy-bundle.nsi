# LPy NSIS installer script.
#
# This file is part of LPy.
# 
# This copy of LPy is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2, or (at your option) any later
# version.
# 
# LPy is supplied in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
# 
# You should have received a copy of the GNU General Public License along with
# LPy; see the file LICENSE.  If not, write to the Free Software Foundation,
# Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


# These will change with different releases.
!define LPY_VERSION        "2.0.0"
!define LPY_VERSION_EXTRA  "Alpha"
!define LPY_VERSION_MEXTRA "a"
!define LPY_LICENSE        "GPL"
!define LPY_LICENSE_LC     "gpl"

# These are all derived from the above.
!define LPY_BASE_NAME      "Lpy"
!define LPY_SHORT_NAME     "${LPY_BASE_NAME} ${LPY_LICENSE} v${LPY_VERSION}${LPY_VERSION_MEXTRA}"
!define LPY_LONG_NAME      "${LPY_BASE_NAME} ${LPY_LICENSE} v${LPY_VERSION} ${LPY_VERSION_EXTRA}"

# Tweak some of the standard pages.
!define MUI_WELCOMEPAGE_TEXT \
"This wizard will guide you through the installation of ${LPY_LONG_NAME}.\r\n\
\r\n\
Any code you write must be released under a license that is compatible with \
the GPL.\r\n\
\r\n\
Click Next to continue."

!define MUI_FINISHPAGE_RUN "$LPY_INSTDIR\bin\lpy.exe"
#!define MUI_FINISHPAGE_RUN_TEXT "Run L-Py"
!define MUI_FINISHPAGE_LINK "Get the latest news of L-Py here"
!define MUI_FINISHPAGE_LINK_LOCATION "http://openalea.gforge.inria.fr/dokuwiki/doku.php?id=packages:vplants:lpy:main"


# Include the tools we use.
!include MUI.nsh
!include LogicLib.nsh


# Define the product name and installer executable.
Name "L-Py"
Caption "${LPY_LONG_NAME} Setup"
OutFile "LPy-${LPY_VERSION}${LPY_VERSION_MEXTRA}-win32-Bundle.exe"


# Set the install directory, from the registry if possible.
#InstallDir "${LPY_INSTALLDIR}"

# The different installation types.  "Full" is everything.  "Minimal" is the
# runtime environment.
InstType "Full"
InstType "Minimal"


# Maximum compression.
SetCompressor /SOLID lzma


# We want the user to confirm they want to cancel.
!define MUI_ABORTWARNING

Var LPY_INSTDIR

Function .onInit
   
    StrCpy $LPY_INSTDIR "$PROGRAMFILES\L-Py"
    
FunctionEnd


# Define the different pages.
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "doc/LICENSE.txt"
!insertmacro MUI_PAGE_COMPONENTS

!define MUI_DIRECTORYPAGE_TEXT_DESTINATION "L-Py repository"
!define MUI_DIRECTORYPAGE_VARIABLE $LPY_INSTDIR
!insertmacro MUI_PAGE_DIRECTORY

!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
  
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

  
# Other settings.
!insertmacro MUI_LANGUAGE "English"


# Installer sections.

Section "Binaries" SecModules
    SectionIn 1 2 RO

    # Make sure this is clean and tidy.
    RMDir /r $LPY_INSTDIR
    CreateDirectory $LPY_INSTDIR

    SetOverwrite on

    # We have to take the SIP files from where they should have been installed.
    SetOutPath $LPY_INSTDIR\bin
    File /r .\dist\*
    
SectionEnd

Section "Documentation" SecDocumentation
    SectionIn 1

    SetOverwrite on

    SetOutPath $LPY_INSTDIR\doc
    File .\doc\*
SectionEnd

Section "Examples and tutorial" SecExamples
    SectionIn 1

    SetOverwrite on

    IfFileExists "$LPY_INSTDIR\examples" 0 +2
        CreateDirectory $LPY_INSTDIR\examples

    SetOutPath $LPY_INSTDIR\examples
    File /r .\share\*
SectionEnd

Section "Start Menu shortcuts" SecShortcuts
    SectionIn 1

    # Make sure this is clean and tidy.
    RMDir /r "$SMPROGRAMS\${LPY_BASE_NAME}"
    CreateDirectory "$SMPROGRAMS\${LPY_BASE_NAME}"
    
    CreateShortCut "$SMPROGRAMS\${LPY_BASE_NAME}\LPy.lnk" "$LPY_INSTDIR\bin\lpy.exe"

    IfFileExists "$LPY_INSTDIR\doc" 0 +2
        CreateShortCut "$SMPROGRAMS\${LPY_BASE_NAME}\Web Site.lnk" "http://openalea.gforge.inria.fr/dokuwiki/doku.php?id=packages:vplants:lpy:main"

    IfFileExists "$LPY_INSTDIR\examples" 0 +2
        CreateShortCut "$SMPROGRAMS\${LPY_BASE_NAME}\Examples Source.lnk" "$LPY_INSTDIR\examples"

    CreateShortCut "$SMPROGRAMS\${LPY_BASE_NAME}\Uninstall LPy.lnk" "$LPY_INSTDIR\Uninstall.exe"
SectionEnd

Section -post
    # Tell Windows about the package.
    WriteRegExpandStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LPy" "UninstallString" '"$LPY_INSTDIR\Uninstall.exe"'
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LPy" "DisplayName" "${LPY_BASE_NAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LPy" "DisplayVersion" "${LPY_VERSION} ${LPY_VERSION_MEXTRA}"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LPy" "NoModify" "1"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LPy" "NoRepair" "1"

    # Save the installation directory for the uninstaller.
    WriteRegStr HKLM "Software\LPy" "Install Path" $LPY_INSTDIR
    
    # Create the uninstaller.
    WriteUninstaller "$LPY_INSTDIR\Uninstall.exe"
SectionEnd


# Section description text.
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SecModules} \
"The L-Py binaries."
!insertmacro MUI_DESCRIPTION_TEXT ${SecDocumentation} \
"The L-Py documentation."
!insertmacro MUI_DESCRIPTION_TEXT ${SecExamples} \
"Some first L-Py examples."
!insertmacro MUI_DESCRIPTION_TEXT ${SecShortcuts} \
"This adds shortcuts to your Start Menu."
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Section "Uninstall"
    # Get the install directory.
    ReadRegStr $LPY_INSTDIR HKLM "Software\LPy" "Install Path"

    # The shortcuts section.
    RMDir /r "$SMPROGRAMS\${LPY_BASE_NAME}"

    # The examples section and the installer itself.
    RMDir /r "$LPY_INSTDIR"

    # Clean the registry.
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\LPy"
    DeleteRegKey HKLM "Software\LPy"
SectionEnd
