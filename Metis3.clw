; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMainFrame
LastTemplate=CDocument
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Metis3.h"
LastPage=0

ClassCount=11
Class1=CMetis3App
Class2=CMetis3Doc
Class3=CMetis3View
Class4=CMainFrame

ResourceCount=9
Resource1=IDR_LISTRMENU
Resource2=IDD_RENAME
Resource3=IDD_CONNECT
Class5=CAboutDlg
Class6=CConnectionDlg
Class7=CMyEdit
Resource4=IDR_CHANNEL
Class8=CRenameDlg
Class9=CMyListCtrl
Resource5=IDR_LIST
Resource6=IDR_MAINFRAME
Class10=CChannelView
Class11=CChannelDoc
Resource7=IDD_ABOUTBOX
Resource8=IDD_METIS3_FORM
Resource9=IDD_CHANNELLIST

[CLS:CMetis3App]
Type=0
HeaderFile=Metis3.h
ImplementationFile=Metis3.cpp
Filter=N

[CLS:CMetis3Doc]
Type=0
HeaderFile=Metis3Doc.h
ImplementationFile=Metis3Doc.cpp
Filter=N

[CLS:CMetis3View]
Type=0
HeaderFile=Metis3View.h
ImplementationFile=Metis3View.cpp
Filter=D
BaseClass=CFormView
VirtualFilter=VWC
LastObject=CMetis3View


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=W
BaseClass=CMDIFrameWnd
VirtualFilter=fWC
LastObject=ID_MYFILE_NEW




[CLS:CAboutDlg]
Type=0
HeaderFile=Metis3.cpp
ImplementationFile=Metis3.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=6
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_MYFILE_NEW
Command2=ID_CHANNEL_CHANNELLIST
Command3=ID_RENAME
Command4=ID_APP_EXIT
Command5=ID_VIEW_TOOLBAR
Command6=ID_VIEW_STATUS_BAR
Command7=ID_VIEW_OPTIONS
Command8=ID_APP_ABOUT
CommandCount=8

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_CHANNEL_CHANNELLIST
Command2=ID_MYFILE_NEW
Command3=ID_NEXT_PANE
Command4=ID_PREV_PANE
CommandCount=4

[DLG:IDD_METIS3_FORM]
Type=1
Class=CMetis3View
ControlCount=7
Control1=IDC_INPUT,edit,1350631552
Control2=IDC_STATIC_CHAT,static,1073741831
Control3=IDC_STATIC_SYSOUT,static,1073741831
Control4=IDC_USERLIST,SysListView32,1350631445
Control5=IDC_SPLITTER_1,static,1073741831
Control6=IDC_SPLITTER_2,static,1073741831
Control7=IDC_LEAVE,button,1342242816

[DLG:IDD_CONNECT]
Type=1
Class=CConnectionDlg
ControlCount=12
Control1=IDC_STATIC_ROOM,static,1342308352
Control2=IDC_ROOM,combobox,1344340226
Control3=IDC_STATIC_USERNAME,static,1342308352
Control4=IDC_USERNAME,edit,1350631552
Control5=IDC_STATIC_FILES,static,1342308352
Control6=IDC_FILES,edit,1350639744
Control7=IDC_STATIC_LINE,static,1342308352
Control8=IDC_LINE,combobox,1344339971
Control9=IDOK,button,1342242817
Control10=IDCANCEL,button,1342242816
Control11=IDC_STATIC,static,1342177283
Control12=IDC_STATIC,button,1476395008

[CLS:CConnectionDlg]
Type=0
HeaderFile=ConnectionDlg.h
ImplementationFile=ConnectionDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_ROOM
VirtualFilter=dWC

[CLS:CMyEdit]
Type=0
HeaderFile=MyEdit.h
ImplementationFile=MyEdit.cpp
BaseClass=CEdit
Filter=W
VirtualFilter=WC

[DLG:IDD_RENAME]
Type=1
Class=CRenameDlg
ControlCount=9
Control1=IDC_STATIC_NAME,static,1342308352
Control2=IDC_NAME,edit,1350631552
Control3=IDC_STATIC_FILES,static,1342308352
Control4=IDC_FILES,edit,1350639744
Control5=IDC_STATIC_LINE,static,1342308352
Control6=IDC_LINE,combobox,1344339971
Control7=IDC_STATIC,static,1342177283
Control8=IDOK,button,1342242817
Control9=IDCANCEL,button,1342242816

[CLS:CRenameDlg]
Type=0
HeaderFile=RenameDlg.h
ImplementationFile=RenameDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_NAME
VirtualFilter=dWC

[CLS:CMyListCtrl]
Type=0
HeaderFile=MyListCtrl.h
ImplementationFile=MyListCtrl.cpp
BaseClass=CListCtrl
Filter=W
VirtualFilter=FWC

[MNU:IDR_CHANNEL]
Type=1
Class=?
Command1=ID_MYFILE_NEW
Command2=ID_FILE_CLOSE
Command3=ID_CHANNEL_CHANNELLIST
Command4=ID_RENAME
Command5=ID_APP_EXIT
Command6=ID_METIS_START
Command7=ID_METIS_STOP
Command8=ID_METIS_EDIT_FILELIST
Command9=ID_METIS_RELOADCONFIGURATION
Command10=ID_VIEW_TOOLBAR
Command11=ID_VIEW_STATUS_BAR
Command12=ID_VIEW_OPTIONS
Command13=ID_WINDOW_CASCADE
Command14=ID_WINDOW_TILE_HORZ
Command15=ID_WINDOW_ARRANGE
Command16=ID_APP_ABOUT
CommandCount=16

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_MYFILE_NEW
Command2=ID_FILE_CLOSE
Command3=ID_CHANNEL_CHANNELLIST
Command4=ID_RENAME
Command5=ID_METIS_START
Command6=ID_METIS_STOP
Command7=ID_APP_EXIT
Command8=ID_APP_ABOUT
CommandCount=8

[DLG:IDD_CHANNELLIST]
Type=1
Class=CChannelView
ControlCount=7
Control1=IDC_CHANNELS,SysListView32,1350631453
Control2=IDC_REFRESH,button,1476460544
Control3=IDC_CLEAR_REFRESH,button,1476460544
Control4=IDC_STATIC_SEARCH,static,1342308352
Control5=IDC_SEARCH,edit,1350631552
Control6=IDC_STATIC_MESSAGES,static,1342308352
Control7=IDC_STATIC_ROOMS,static,1342308354

[CLS:CChannelView]
Type=0
HeaderFile=ChannelView.h
ImplementationFile=ChannelView.cpp
BaseClass=CFormView
Filter=M
VirtualFilter=VWC
LastObject=IDC_CHANNELS

[CLS:CChannelDoc]
Type=0
HeaderFile=ChannelDoc.h
ImplementationFile=ChannelDoc.cpp
BaseClass=CDocument
Filter=N
LastObject=IDC_REFRESH

[MNU:IDR_LIST]
Type=1
Class=?
Command1=ID_MYFILE_NEW
Command2=ID_CHANNEL_CHANNELLIST
Command3=ID_RENAME
Command4=ID_APP_EXIT
Command5=ID_VIEW_TOOLBAR
Command6=ID_VIEW_STATUS_BAR
Command7=ID_VIEW_OPTIONS
Command8=ID_WINDOW_CASCADE
Command9=ID_WINDOW_TILE_HORZ
Command10=ID_WINDOW_ARRANGE
Command11=ID_APP_ABOUT
CommandCount=11

[MNU:IDR_LISTRMENU]
Type=1
Class=?
Command1=ID_MYFILE_NEW
CommandCount=1

