; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CMetis3View
LastTemplate=CEditView
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Metis3.h"
LastPage=0

ClassCount=13
Class1=CMetis3App
Class2=CMetis3Doc
Class3=CMetis3View
Class4=CMainFrame

ResourceCount=11
Resource1=IDR_LIST
Resource2=IDR_USER
Resource3=IDR_LISTRMENU
Class5=CAboutDlg
Class6=CConnectionDlg
Class7=CMyEdit
Resource4=IDD_CHANNELLIST
Class8=CRenameDlg
Class9=CMyListCtrl
Resource5=IDR_CHANNEL
Resource6=IDR_MAINFRAME
Class10=CChannelView
Class11=CChannelDoc
Resource7=IDD_METIS3_FORM
Resource8=IDD_ABOUTBOX
Resource9=IDD_CONNECT
Resource10=IDD_RENAME
Class12=CServerDoc
Class13=CServerView
Resource11=IDD_SETTINGS

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
Filter=W
BaseClass=CFormView
VirtualFilter=VWC
LastObject=ID_USERLIST_CUSTOMIZETHISMENU


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=W
BaseClass=CMDIFrameWnd
VirtualFilter=fWC
LastObject=ID_APP_EXIT




[CLS:CAboutDlg]
Type=0
HeaderFile=Metis3.cpp
ImplementationFile=Metis3.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=11
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308481
Control3=IDC_STATIC,static,1342308353
Control4=IDOK,button,1342373889
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,button,1342177287
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_MYFILE_NEW
Command2=ID_CHANNEL_CHANNELLIST
Command3=IDR_START_NODESERVER
Command4=ID_RENAME
Command5=ID_FILE_MRU_FILE1
Command6=ID_APP_EXIT
Command7=ID_VIEW_TOOLBAR
Command8=ID_VIEW_STATUS_BAR
Command9=ID_VIEW_OPTIONS
Command10=ID_APP_ABOUT
CommandCount=10

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
ControlCount=14
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
Control13=IDC_STATIC_USERSETUP,button,1342177287
Control14=IDC_USENODE,button,1342242819

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
Command3=IDR_START_NODESERVER
Command4=ID_CHANNEL_CHANNELLIST
Command5=ID_RENAME
Command6=ID_APP_EXIT
Command7=ID_METIS_START
Command8=ID_METIS_STOP
Command9=ID_METIS_EDIT_FILELIST
Command10=ID_METIS_RELOADCONFIGURATION
Command11=ID_VIEW_TOOLBAR
Command12=ID_VIEW_STATUS_BAR
Command13=ID_VIEW_OPTIONS
Command14=ID_WINDOW_CASCADE
Command15=ID_WINDOW_TILE_HORZ
Command16=ID_WINDOW_ARRANGE
Command17=ID_APP_ABOUT
CommandCount=17

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=IDR_START_NODESERVER
Command2=ID_MYFILE_NEW
Command3=ID_FILE_CLOSE
Command4=ID_CHANNEL_CHANNELLIST
Command5=ID_RENAME
Command6=ID_METIS_START
Command7=ID_METIS_STOP
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
Command3=IDR_START_NODESERVER
Command4=ID_RENAME
Command5=ID_APP_EXIT
Command6=ID_VIEW_TOOLBAR
Command7=ID_VIEW_STATUS_BAR
Command8=ID_VIEW_OPTIONS
Command9=ID_WINDOW_CASCADE
Command10=ID_WINDOW_TILE_HORZ
Command11=ID_WINDOW_ARRANGE
Command12=ID_APP_ABOUT
CommandCount=12

[MNU:IDR_LISTRMENU]
Type=1
Class=?
Command1=ID_MYFILE_NEW
CommandCount=1

[MNU:IDR_USER]
Type=1
Class=CMetis3View
Command1=ID_USERLIST_SENDMESSAGE
Command2=ID_USERLIST_COPYUSERNAME
Command3=ID_USERLIST_KICK
Command4=ID_USERLIST_BAN
Command5=ID_USERLIST_KICKBAN
Command6=ID_USERLIST_UNBAN
Command7=ID_USERLIST_REDIRECT
Command8=ID_USERLIST_ADDADMIN
Command9=ID_USERLIST_REMOVEADMIN
Command10=ID_USERLIST_PRINTUSERSTAT
Command11=ID_USERLIST_PRINTIP
Command12=ID_USERLIST_READUSERMESSAGE
Command13=ID_USERLIST_CUSTOMIZETHISMENU
CommandCount=13

[DLG:IDD_SETTINGS]
Type=1
Class=?
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816

[CLS:CServerDoc]
Type=0
HeaderFile=ServerDoc.h
ImplementationFile=ServerDoc.cpp
BaseClass=CDocument
Filter=N

[CLS:CServerView]
Type=0
HeaderFile=ServerView.h
ImplementationFile=ServerView.cpp
BaseClass=CEditView
Filter=C
LastObject=ID_USERLIST_SENDMESSAGE

