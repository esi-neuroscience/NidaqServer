================================================================================
    MICROSOFT FOUNDATION CLASS LIBRARY : nidaqServer Project Overview
===============================================================================

The application wizard has created this nidaqServer application for
you.  This application not only demonstrates the basics of using the Microsoft
Foundation Classes but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your nidaqServer application.

nidaqServer.vcxproj
    This is the main project file for VC++ projects generated using an application wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    application wizard.

nidaqServer.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

nidaqServer.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CnidaqServerApp application class.

nidaqServer.cpp
    This is the main application source file that contains the application
    class CnidaqServerApp.

nidaqServer.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
    Visual C++. Your project resources are in 1033.

res\nidaqServer.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file nidaqServer.rc.

res\nidaqServer.rc2
    This file contains resources that are not edited by Microsoft
    Visual C++. You should place all resources not editable by
    the resource editor in this file.

/////////////////////////////////////////////////////////////////////////////

For the main frame window:
    The project includes a standard MFC interface.

MainFrm.h, MainFrm.cpp
    These files contain the frame class CMainFrame, which is derived from
    CFrameWnd and controls all SDI frame features.

/////////////////////////////////////////////////////////////////////////////

The application wizard creates one document type and one view:

nidaqServerDoc.h, nidaqServerDoc.cpp - the document
    These files contain your CnidaqServerDoc class.  Edit these files to
    add your special document data and to implement file saving and loading
    (via CnidaqServerDoc::Serialize).

nidaqServerView.h, nidaqServerView.cpp - the view of the document
    These files contain your CnidaqServerView class.
    CnidaqServerView objects are used to view CnidaqServerDoc objects.





/////////////////////////////////////////////////////////////////////////////

Other Features:

ActiveX Controls
    The application includes support to use ActiveX controls.

/////////////////////////////////////////////////////////////////////////////

Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named nidaqServer.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

nidaqServer.manifest
	Application manifest files are used by Windows XP to describe an applications
	dependency on specific versions of Side-by-Side assemblies. The loader uses this
	information to load the appropriate assembly from the assembly cache or private
	from the application. The Application manifest  maybe included for redistribution
	as an external .manifest file that is installed in the same folder as the application
	executable or it may be included in the executable in the form of a resource.
/////////////////////////////////////////////////////////////////////////////

Other notes:

The application wizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, you will need
to redistribute the MFC DLLs. If your application is in a language
other than the operating system's locale, you will also have to
redistribute the corresponding localized resources MFC100XXX.DLL.
For more information on both of these topics, please see the section on
redistributing Visual C++ applications in MSDN documentation.

/////////////////////////////////////////////////////////////////////////////
1.0.1.0	Mst	27-Jun-2017	pulse line support
1.0.2.0 MSt 30-Jun-2017 M-Series: arbitrary line selection
1.0.3.0 MSt  3-Jul-2017 reward support
1.0.4.0 MSt  7-Jul-2017 RewardDone event; reward cleanup
1.0.5.0 MSt 11-Jul-2017 event marker support
1.0.5.1 MSt 11-Jul-2017 strobe is line0 (not 1) on 6503
1.0.6.0 MSt 24-Jul-2017 various code cleanups; ESI icons; error handling
1.0.7.0 MSt 28-Sep-2017 optional event markers for manual reward
1.0.8.0 MSt 25-Oct-2017 support for USB-6353
		MSt  2-Nov-2017 strobes 41�s
1.0.8.1 MSt  2-Nov-2017 check for strobe low time at beginning of marker
1.0.8.2 MSt 10-Nov-2017 USB-6353: use port 0 only for event markers
1.0.9.0 MSt  5-Apr-2018 optional reward informational messages
1.0.10.0 MSt 7-Jun-2018 query total reward time
1.0.10.1 MSt 16-Aug-2018 support for PCIe-6251
1.0.10.2 MSt 11-Dec-2018 support for PCIe-6321
1.0.10.3 MSt 07-Feb-2019 added DaqServerDone event
1.0.10.4 MSt 16-Mar-2019 realized that PCIe-6321 is X-Series device
1.0.10.5 MSt 20-Mar-2019 set DaqServerDone event when starting change detection
1.0.10.6 MSt  7-May-2019 support for PCIe-6323
1.0.10.7 MSt 14-Nov-2019 support for PCIe-6351
1.0.11.0 MSt 30-Oct-2020 support for output pulse lines