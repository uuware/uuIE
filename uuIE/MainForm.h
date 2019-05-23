// MainForm.h
// The C# like Form class
// Author: Shu.KK
// 2007-10-10
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef CC_WIN_MAINFORM_H_
#define CC_WIN_MAINFORM_H_

#include "..\cc\cc.All.h"
#include "resource.h"

using namespace cc;
using namespace cc::win;

/*
DISPID_BEFORENAVIGATE   =   100;  
//   this   is   sent   before   navigation   to   give   a   chance   to   abort  
DISPID_NAVIGATECOMPLETE   =   101;  
//   in   async,   this   is   sent   when   we   have   enough   to   show  
DISPID_STATUSTEXTCHANGE   =   102;  
DISPID_QUIT   =   103;  
DISPID_DOWNLOADCOMPLETE   =   104;  
DISPID_COMMANDSTATECHANGE   =   105;  
DISPID_DOWNLOADBEGIN   =   106;  
DISPID_NEWWINDOW   =   107;   //   sent   when   a   new   window   should   be   created  
DISPID_PROGRESSCHANGE   =   108;   //   sent   when   download   progress   is   updated  
DISPID_WINDOWMOVE   =   109;   //   sent   when   main   window   has   been   moved  
DISPID_WINDOWRESIZE   =   110;   //   sent   when   main   window   has   been   sized  
DISPID_WINDOWACTIVATE   =   111;   //   sent   when   main   window   has   been   activated  
DISPID_PROPERTYCHANGE   =   112;   //   sent   when   the   PutProperty   method   is   called  
DISPID_TITLECHANGE   =   113;   //   sent   when   the   document   title   changes  
DISPID_TITLEICONCHANGE   =   114;  
//   sent   when   the   top   level   window   icon   may   have   changed.  
DISPID_FRAMEBEFORENAVIGATE   =   200;  
DISPID_FRAMENAVIGATECOMPLETE   =   201;  
DISPID_FRAMENEWWINDOW   =   204;  
DISPID_BEFORENAVIGATE2   =   250;   //   hyperlink   clicked   on  
DISPID_NEWWINDOW2   =   251;  
DISPID_NAVIGATECOMPLETE2   =   252;   //   UIActivate   new   document  
DISPID_ONQUIT   =   253;  
DISPID_ONVISIBLE   =   254;   //   sent   when   the   window   goes   visible/hidden  
DISPID_ONTOOLBAR   =   255;   //   sent   when   the   toolbar   should   be   shown/hidden  
DISPID_ONMENUBAR   =   256;   //   sent   when   the   menubar   should   be   shown/hidden  
DISPID_ONSTATUSBAR   =   257;   //   sent   when   the   statusbar   should   be   shown/hidden  
DISPID_ONFULLSCREEN   =   258;   //   sent   when   kiosk   mode   should   be   on/off  
DISPID_DOCUMENTCOMPLETE   =   259;   //   new   document   goes   ReadyState_Complete  
DISPID_ONTHEATERMODE   =   260;   //   sent   when   theater   mode   should   be   on/off  
DISPID_ONADDRESSBAR   =   261;   //   sent   when   the   address   bar   should   be   shown/hidden  
DISPID_WINDOWSETRESIZABLE   =   262;  
//   sent   to   set   the   style   of   the   host   window   frame  
DISPID_WINDOWCLOSING   =   263;  
//   sent   before   script   window.close   closes   the   window  
DISPID_WINDOWSETLEFT   =   264;  
//   sent   when   the   put_left   method   is   called   on   the   WebOC  
DISPID_WINDOWSETTOP   =   265;  
//   sent   when   the   put_top   method   is   called   on   the   WebOC  
DISPID_WINDOWSETWIDTH   =   266;  
//   sent   when   the   put_width   method   is   called   on   the   WebOC  
DISPID_WINDOWSETHEIGHT   =   267;  
//   sent   when   the   put_height   method   is   called   on   the   WebOC  
DISPID_CLIENTTOHOSTWINDOW   =   268;  
//   sent   during   window.open   to   request   conversion   of   dimensions  
DISPID_SETSECURELOCKICON   =   269;  
//   sent   to   suggest   the   appropriate   security   icon   to   show  
DISPID_FILEDOWNLOAD   =   270;  
//   Fired   to   indicate   the   File   Download   dialog   is   opening   	
*/

class MainForm;
class miniIE_IEEvent : public ie_EventHandler
{
protected:
	MainForm* mainForm;
	ToolItem* moveCtl;
	TextBox* editURL;
	ie_IWebBrowser2* webBrower;

public:
	miniIE_IEEvent()
	{
		moveCtl = NULL;
	}
	void SetParent(MainForm* mainForm, ToolItem* ctl, TextBox* editURL)
	{
		this->mainForm = mainForm;
		moveCtl = ctl;
		this->editURL = editURL;
		webBrower = NULL;
	};
	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(
		/* [in] */ REFIID riid,
		/* [size_is][in] */ LPOLESTR *rgszNames,
		/* [in] */ UINT cNames,
		/* [in] */ LCID lcid,
		/* [size_is][out] */ DISPID *rgDispId)
	{
		return S_OK;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(
		/* [in] */ DISPID dispIdMember,
		/* [in] */ REFIID riid,
		/* [in] */ LCID lcid,
		/* [in] */ WORD wFlags,
		/* [out][in] */ DISPPARAMS *pDispParams,
		/* [out] */ VARIANT *pVarResult,
		/* [out] */ EXCEPINFO *pExcepInfo,
		/* [out] */ UINT *puArgErr)
	{
		//cc::Log::debug(_T("Invoke:%d"), dispIdMember);
		switch (dispIdMember)
		{
			case 250: //DISPID_BEFORENAVIGATE2
				if(pDispParams->cArgs >= 6 && pDispParams->rgvarg[6].vt == VT_DISPATCH)
				{
					BeforeNavigate2(pDispParams->rgvarg[5].pvarVal->bstrVal, pDispParams->rgvarg[6].pdispVal, pDispParams->rgvarg[0].pboolVal);
				}
				break;
			case 259: //DISPID_DOCUMENTCOMPLETE
				DocumentComplete(pDispParams->rgvarg[0].pvarVal->bstrVal);
				break;
			case 113: //DISPID_TITLECHANGE
				TitleChange(pDispParams->rgvarg[0].bstrVal);
				break;
			case 107: //DISPID_NEWWINDOW
			case 251: //DISPID_NEWWINDOW2
				NewWindow(pDispParams->rgvarg[1].ppdispVal, pDispParams->rgvarg[0].pboolVal);
				break;
		}
		return S_OK;
	}
	void BeforeNavigate2(const BSTR url, IDispatch *ppDisp, short *cancel);
	void DocumentComplete(const BSTR url);
	void TitleChange(const BSTR title)
	{
		cc::Str str = cc::Str(cc::LangCode::TCHAR, title);
		moveCtl->SetTips((TCHAR*)str.GetBuf());
	}
	void NewWindow(LPDISPATCH* ppDisp, VARIANT_BOOL* Cancel);
};

class SelLang : public Dialog
{
public:
	SelLang();

protected:
	Button* btnOK;
	Button* btnCancel;
	ComboBox* comboLang;
	VersionInfo* verInfo;

	virtual void OnCreateControl();
	void Control_Click(Object* sender, EventArgs* e);
};

class MainForm : public Form
{
public:
	MainForm();

	MainForm* NewMiniIE();
	virtual void SetIsMin(bool isMin);
	virtual bool GetIsMin();
	virtual void SetIsFix(bool isMin);
	virtual bool GetIsFix();
	virtual RECT GetNormalBounds();

	virtual void SetIsBlock(bool isBlock){this->isBlock = isBlock;};
	virtual bool GetIsBlock(){return this->isBlock;};
	virtual IEControl* GetIEControl(){return this->ie;};
	virtual void UpdateIEEvent(){dIEEventTime = ::clock();};
	virtual bool IsIEEvent(){return (((double)(::clock() - dIEEventTime))/(double)CLOCKS_PER_SEC < 0.5);};
	virtual void GetIEHwnd();

protected:
	bool isModal;
	bool isMin;
	bool isBlock;
	RECT rcNormal;
	MainForm* mainForm;
	ImageList* imageList;
	clock_t dIEEventTime;
	HWND hWndIE; //get ie's server's HWND,while document complete
	TCHAR tcBuf[MAX_PATH + 1];

	ToolBar* toolbar;
	ToolItem* toolbreak1;
	ToolItem* moveForm;
	ToolItem* toolsysmenu;
	ToolItem* toolexit;
	ToolItem* toolattach;
	ToolItem* toolrefresh;
	ToolItem* toolstop;
	TextBox* editURL;
	ToolItem* toolgo;
	IEControl* ie;
	miniIE_IEEvent* miniIE;

	ContextMenu* contMenu;
	MenuItem* menuClose;
	MenuItem* menuNewWin;
	MenuItem* menuFavorites;
	MenuItem* menuFavAdd;
	MenuItem* menuFavMan;
	MenuItem* menuOpenFile;
	MenuItem* menuReload;
	MenuItem* menuPrint;
	MenuItem* menuSaveAs;
	MenuItem* menuIsBlock;
	MenuItem* menuLang;
	MenuItem* menuAbout;
	MenuItem* menuOption;
	MenuItem* menuBreak1;
	MenuItem* menuBreak2;

	void InitializeComponent();
	//keyArr is param address,but sThisHead is by value
	virtual void DoSetLang(KeyArr& keyArr, cc::Str sThisHead);
	void OnResize(EventArgs* e);
	void Form_Deactivate(Object* sender, EventArgs* e);
	void Form_Load(Object* sender, EventArgs* e);
	void Form_Closing(Object* sender, CancelEventArgs* e);
	void Form_Closed(Object* sender, EventArgs* e);
	void move_DoubleClick(Object* sender, EventArgs* e);
	void close_Click(Object* sender, EventArgs* e);
	void NewWin_Click(Object* sender, EventArgs* e);
	void print_Click(Object* sender, EventArgs* e);
	void editURL_KeyDown(Object* sender, KeyEventArgs* e);
	void menuFavorites_Popup(Object* sender, EventArgs* e);
	void menuFavorites_Click(Object* sender, EventArgs* e);
	void Favorites2Menu(MenuItem* item, cc::Str path);
	void menuAbout_Click(Object* sender, EventArgs* e);

	void tool_Click(Object* sender, EventArgs* e);
};

#endif //#ifndef CC_WIN_MAINFORM_H_
