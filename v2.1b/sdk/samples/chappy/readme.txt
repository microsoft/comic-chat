========================================================================
       MICROSOFT FOUNDATION CLASS LIBRARY : Chappy
========================================================================


AppWizard has created this Chappy application for you.  This application
not only demonstrates the basics of using the Microsoft Foundation classes
but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your Chappy application.

Chappy.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CChappyApp application class.

Chappy.cpp
    This is the main application source file that contains the application
    class CChappyApp.

Chappy.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
	Developer Studio.

res\Chappy.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file Chappy.rc.

res\Chappy.rc2
    This file contains resources that are not edited by Microsoft 
	Developer Studio.  You should place all resources not
	editable by the resource editor in this file.

Chappy.clw
    This file contains information used by ClassWizard to edit existing
    classes or add new classes.  ClassWizard also uses this file to store
    information needed to create and edit message maps and dialog data
    maps and to create prototype member functions.

/////////////////////////////////////////////////////////////////////////////

For the main frame window:

MainFrm.h, MainFrm.cpp
    These files contain the frame class CMainFrame, which is derived from
    CFrameWnd and controls all SDI frame features.

res\Toolbar.bmp
    This bitmap file is used to create tiled images for the toolbar.
    The initial toolbar and status bar are constructed in the
    CMainFrame class.  Edit this toolbar bitmap along with the
    array in MainFrm.cpp to add more toolbar buttons.

/////////////////////////////////////////////////////////////////////////////

AppWizard creates one document type and one view:

ChappyDoc.h, ChappyDoc.cpp - the document
    These files contain your CChappyDoc class.  Edit these files to
    add your special document data and to implement file saving and loading
    (via CChappyDoc::Serialize).

ChappyView.h, ChappyView.cpp - the view of the document
    These files contain your CChappyView class.
    CChappyView objects are used to view CChappyDoc objects.



/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named Chappy.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Developer Studio reads and updates this file.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, and your application is 
in a language other than the operating system's current language, you
will need to copy the corresponding localized resources MFC40XXX.DLL
from the Microsoft Visual C++ CD-ROM onto the system or system32 directory,
and rename it to be MFCLOC.DLL.  ("XXX" stands for the language abbreviation.
For example, MFC40DEU.DLL contains resources translated to German.)  If you
don't do this, some of the UI elements of your application will remain in the
language of the operating system.

/////////////////////////////////////////////////////////////////////////////
The OCX

MFC helps you generate the wrapper classes for the OCX but does not do all the work
for you.

To generate the entry class (CMsChatPr) you need to do the following:
- Select the 'Project - add to project - components and controls' menu
- from the select the registered activeX controls
- select 'MsChatPr class' object
- press 'Insert'
- press 'OK'
- press 'OK' 

This will generate the files MsChatPr.cpp and MsChatPr.h containing the the wrapper 
class for CMSChatPr.  This will also generate the files that contain the wrapper class 
'CChatItems'.  In this project I modified the file name to be ChtItems.cpp and ChtItems.h

This two object are the only instaciatable object contained in the OCX.  To generate wrapper 
classes for the other Interfaces contained in the OCX you need to generate an .TLB file from
the .IDL.  This is simply done from the command line using the MIDL compiler shipped with VC50.
Once you compiled the .TBL use the class wizzard to generate wrapper classes for all the remaining 
interfaces. From the class wizzard:
-Press the 'Add Classes' button and select 'From Type library"
-Select the .TBL file
The wizzard will present you the list of interfaces
-Select them all 
-change the file names not to conflict with the MsChatPr.cpp and MsChatPr.h <default>.  In this project
 MSChatX.cpp and MsChatX.h
-click OK

At this point you should have all the Interfaces and Objects ready to be used from the OCX




