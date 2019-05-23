#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

//define for version
#define FILEVER          1, 1, 2, 0
#define STRFILEVER       "1, 1, 2, 0\0"
#define PRODUCTVER       FILEVER
#define STRPRODUCTVER    STRFILEVER
#define FILENAME         "uuIE"
#define APPNAME          "uuIE[uuware.com]"
#define APPVERSION       "1.12"
//#define APPBUILD         "build 20080915"
#define APPBUILD         "build 20130115"

#define IDS_APP_TITLE			103

#define IDR_MAINFRAME			128

#define IDI_ICON_MAIN           100
#define IDI_ICON_ATTACH         101
#define IDI_ICON_EXIT           103
#define IDI_ICON_REFRESH        104
#define IDI_ICON_PRINT          105
#define IDI_ICON_NEWWIN         106
#define IDI_ICON_OPTION         107
#define IDI_ICON_IE             108
#define IDI_ICON_STOP           109
#define IDI_ICON_GO             110
#define IDI_ICON_FOLDER         111
#define IDI_ICON_PAGE           112

#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS

#define _APS_NO_MFC					130
#define _APS_NEXT_RESOURCE_VALUE	129
#define _APS_NEXT_COMMAND_VALUE		32771
#define _APS_NEXT_CONTROL_VALUE		1000
#define _APS_NEXT_SYMED_VALUE		110
#endif
#endif
