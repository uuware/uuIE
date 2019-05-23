#include "MainForm.h"
using namespace cc;
using namespace cc::win;

#ifdef _UNICODE
	#define REG_REGKEY _T("SOFTWARE\\uuware\\uuIE")
#else
	#define REG_REGKEY _T("SOFTWARE\\uuware\\uuIE_ansi")
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//when add manifest.xml for xp theme, also need set controls to have default teansparent
	//so while some controls need backcolor, need remove  (OuterStyle::TransparentParentBrush|OuterStyle::TransparentParentErase)
	cc::win::App::SetDefaultTransparent(true);
	cc::win::App::SetInstance(hInstance);
	//should set this before cc::Config::GetLang(),and SetIsAutoSave is before LoadLang
	cc::win::App::GetConfig()->SetIsAutoSave(true, true, REG_REGKEY);
	cc::win::App::GetConfig()->LoadLang(CC_CONFIG_LANGENG);
	//the name is used for CreateMutex"s flages,cannot change to other lang
	//cc::win::App::SetNameVersion(_T("uuIE[uuware.com]"), _T("Ver 1.02 build 20080912"));
	cc::win::App::SetNameVersion(_T(APPNAME), _T(APPVERSION));

	MainForm* form = new MainForm();
	cc::win::App::Run(form);
	form->Dispose();
	delete form;
	return 0;
}

// Map(bool isSort, bool bIsDESC = false, bool isDistinct = false, int nMinCapacity = 0, double nIncrement = 1.3);
Map<HWND, Control*>* mapHWND = new Map<HWND, Control*>(true, false, true);
HHOOK gMsgHook = NULL;
MainForm* hMainIEForm = NULL; //save first form,if no others IEForm then close this
int nIECount = 0;
LRESULT CALLBACK WatchMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
	if(code >= 0 && mapHWND != NULL)
	{
		int nIndex;
		MSG* data = (MSG*)lParam;
		if(data->message == WM_LBUTTONDOWN || data->message == WM_KEYDOWN)
		{
			if((nIndex = mapHWND->GetIndex(data->hwnd)) >= 0)
			{
				MainForm* ieForm = (MainForm*)mapHWND->GetAt(nIndex);
				ieForm->UpdateIEEvent();
				::SetWindowPos(ieForm->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
		}
	}
	return ::CallNextHookEx(gMsgHook, code, wParam, lParam);
}
long StartMsgHook(HINSTANCE hInstance)
{
	gMsgHook = SetWindowsHookEx(WH_GETMESSAGE, WatchMsgProc, hInstance, ::GetCurrentThreadId());
	//cc::Log::debug(_T("StartMsgHook:%d"), gMsgHook);
	if(gMsgHook == NULL)
	{
		return -1;
	}
	return 0;
}
long StopMsgHook()
{
	//cc::Log::debug(_T("StopMsgHook:%d"), gMsgHook);
	if(gMsgHook != NULL)
	{
		::UnhookWindowsHookEx(gMsgHook);
		gMsgHook = NULL;
	}
	return 0;
}

void miniIE_IEEvent::NewWindow(LPDISPATCH* ppDisp, VARIANT_BOOL* Cancel)
{
	if(!mainForm->GetIsBlock() || mainForm->IsIEEvent())
	{
		MainForm* ieForm = mainForm->NewMiniIE();
		if(ieForm != NULL)
		{
			IEControl* ie = ieForm->GetIEControl();
			*ppDisp = ie->GetWebBrowser();
		}
	}
	else
	{
		*Cancel = TRUE;
	}
}
void miniIE_IEEvent::BeforeNavigate2(const BSTR url, IDispatch *ppDisp, short *cancel)
{
	if(webBrower == NULL)
	{
		webBrower = mainForm->GetIEControl()->GetWebBrowser();
	}
	if(ppDisp == webBrower)
	{
		cc::Str url2 = cc::Str(cc::LangCode::TCHAR, url);
		if(url2.IndexOf(_T("script:"), true) < 0)
		{
			editURL->SetText((TCHAR*)url2.GetBuf());
		}
	}
}
void miniIE_IEEvent::DocumentComplete(const BSTR url)
{
	mainForm->GetIEHwnd();
}

SelLang::SelLang()
{
	int nTop = 5;
	Label* labelLang = new Label();
	labelLang->SetName(_T("labLang"));
	labelLang->SetLocation(3, nTop + 2);
	labelLang->SetSize(60, 17);
	this->AddControl(labelLang);

	comboLang = new ComboBox();
	comboLang->SetName(_T("comboLang"));
	comboLang->SetComboBoxStyle(ComboBoxStyle::DropDownList);
	comboLang->SetLocation(64, nTop);
	comboLang->SetSize(330, 145);
	comboLang->Anchor = (AnchorStyles::Type)(AnchorStyles::Top | AnchorStyles::Left | AnchorStyles::Right);
	this->AddControl(comboLang);

	nTop += 25;
	CC_APP_GetIni()->Set(_T("Main.VInfo"), (DWORD)0);
	verInfo = new VersionInfo();
	verInfo->SetLocation(1, nTop);
	verInfo->SetSize(398, 198);
	verInfo->Anchor = (AnchorStyles::Type)(AnchorStyles::Top | AnchorStyles::Left | AnchorStyles::Right | AnchorStyles::Bottom);
	this->AddControl(verInfo);

	nTop += 206;
	btnOK = new Button();
	btnOK->SetName(_T("btnOK"));
	btnOK->SetLocation(10, nTop);
	btnOK->SetSize(100, 19);
	btnOK->Anchor = (AnchorStyles::Type)(AnchorStyles::Left | AnchorStyles::Bottom);
	btnOK->Click += EventHandler((Object*)this, (EventFun)&SelLang::Control_Click);
	this->AddControl(btnOK);
	this->SetOKButton(btnOK);

	btnCancel = new Button();
	btnCancel->SetName(_T("btnCancel"));
	btnCancel->SetLocation(112, nTop);
	btnCancel->SetSize(100, 19);
	btnCancel->Anchor = (AnchorStyles::Type)(AnchorStyles::Left | AnchorStyles::Bottom);
	btnCancel->Click += EventHandler((Object*)this, (EventFun)&SelLang::Control_Click);
	this->AddControl(btnCancel);
	this->SetCancelButton(btnCancel);

	this->SetStyle(StyleType::Style, 0, WS_THICKFRAME);
	this->SetSizeBaseOnClient(400, nTop + 19 + 10 );
	this->SetConfigName(_T("selLang"));
	SetLang(*CC_APP_GetLang(), cc::Str(CC_CONFIG_LANGSECTION));
}

void SelLang::OnCreateControl()
{
	Dialog::OnCreateControl();
	//show version this time
	::SetTimer(verInfo->m_hWnd, CC_TIMERID_VERSION, 100, NULL);

	//get all lang(res and files) and set to item
	cc::win::WinUtil::AddLangItems(comboLang, true, true);
}

void SelLang::Control_Click(Object* sender, EventArgs* e)
{
	if(sender == btnOK)
	{
		//get selected lang and set to ini
		cc::win::WinUtil::SetLangToConfig(comboLang);
		EndDialog(DialogResult::OK);
	}
	else if(sender == btnCancel)
	{
		EndDialog(DialogResult::Cancel);
	}
}

MainForm::MainForm()
{
	InitializeComponent();
	//count for new IE,while close last then release main form
	nIECount++;
	if(hMainIEForm == NULL)
	{
		hMainIEForm = this;
	}
}

void MainForm::InitializeComponent()
{
	imageList = new ImageList();
	imageList->SetIconSize(-1, -1);
	imageList->SetIcon(IDI_ICON_MAIN, IDI_ICON_MAIN);
	imageList->SetIcon(IDI_ICON_ATTACH, IDI_ICON_ATTACH);
	imageList->SetIcon(IDI_ICON_EXIT, IDI_ICON_EXIT);
	imageList->SetIcon(IDI_ICON_REFRESH, IDI_ICON_REFRESH);
	imageList->SetIcon(IDI_ICON_PRINT, IDI_ICON_PRINT);
	imageList->SetIcon(IDI_ICON_NEWWIN, IDI_ICON_NEWWIN);
	imageList->SetIcon(IDI_ICON_OPTION, IDI_ICON_OPTION);
	imageList->SetIcon(IDI_ICON_IE, IDI_ICON_IE);
	imageList->SetIcon(IDI_ICON_STOP, IDI_ICON_STOP);
	imageList->SetIcon(IDI_ICON_GO, IDI_ICON_GO);
	imageList->SetIcon(IDI_ICON_FOLDER, IDI_ICON_FOLDER);
	imageList->SetIcon(IDI_ICON_PAGE, IDI_ICON_PAGE);

	isMin = false;
	isModal = false;
	this->isBlock = true;
	hWndIE = NULL;
	memset(tcBuf, 0, MAX_PATH);

	//context menu
	contMenu = new ContextMenu();
	contMenu->SetName(_T("contMenu"));
	contMenu->SetAllOwnerDraw(true);
	contMenu->Popup += EventHandler((Object*)this, (EventFun)&MainForm::menuFavorites_Popup);

	menuClose = new MenuItem();
	menuClose->SetName(_T("menuClose"));
	menuClose->Click += EventHandler((Object*)this, (EventFun)&MainForm::close_Click);
	menuClose->SetIcon(imageList, IDI_ICON_EXIT);
	menuClose->SetShortcut(Shortcut::CtrlQ);
	contMenu->ItemAdd(menuClose);

	menuBreak1 = new MenuItem();
	menuBreak1->SetText(_T("-"));
	contMenu->ItemAdd(menuBreak1);

	menuNewWin = new MenuItem();
	menuNewWin->SetName(_T("menuNewIE"));
	menuNewWin->Click += EventHandler((Object*)this, (EventFun)&MainForm::NewWin_Click);
	menuNewWin->SetIcon(imageList, IDI_ICON_NEWWIN);
	menuNewWin->SetShortcut(Shortcut::CtrlN);
	contMenu->ItemAdd(menuNewWin);

	menuFavorites = new MenuItem();
	menuFavorites->SetName(_T("menuFavorites"));
	//menuFavorites->Popup += EventHandler((Object*)this, (EventFun)&MainForm::menuFavorites_Popup);
	//menuFavorites->SetIcon(imageList, IDI_ICON_REFRESH);
	contMenu->ItemAdd(menuFavorites);

	menuFavAdd = new MenuItem();
	menuFavAdd->SetName(_T("menuFavAdd"));
	menuFavAdd->Click += EventHandler((Object*)this, (EventFun)&MainForm::menuFavorites_Click);
	//menuFavAdd->SetIcon(imageList, IDI_ICON_REFRESH);
	menuFavorites->ItemAdd(menuFavAdd);

	menuFavMan = new MenuItem();
	menuFavMan->SetName(_T("menuFavMan"));
	menuFavMan->Click += EventHandler((Object*)this, (EventFun)&MainForm::menuFavorites_Click);
	//menuFavMan->SetIcon(imageList, IDI_ICON_REFRESH);
	menuFavorites->ItemAdd(menuFavMan);

	menuBreak2 = new MenuItem();
	menuBreak2->SetText(_T("-"));
	menuFavorites->ItemAdd(menuBreak2);

	menuOpenFile = new MenuItem();
	menuOpenFile->SetName(_T("menuOpenFile"));
	menuOpenFile->Click += EventHandler((Object*)this, (EventFun)&MainForm::tool_Click);
	//menuOpenFile->SetIcon(imageList, IDI_ICON_REFRESH);
	contMenu->ItemAdd(menuOpenFile);

	menuReload = new MenuItem();
	menuReload->SetName(_T("menuReload"));
	menuReload->Click += EventHandler((Object*)this, (EventFun)&MainForm::tool_Click);
	menuReload->SetIcon(imageList, IDI_ICON_REFRESH);
	contMenu->ItemAdd(menuReload);

	menuBreak2 = new MenuItem();
	menuBreak2->SetText(_T("-"));
	contMenu->ItemAdd(menuBreak2);

	menuPrint = new MenuItem();
	menuPrint->SetName(_T("menuPrint"));
	menuPrint->Click += EventHandler((Object*)this, (EventFun)&MainForm::print_Click);
	menuPrint->SetIcon(imageList, IDI_ICON_PRINT);
	menuPrint->SetShortcut(Shortcut::CtrlP);
	contMenu->ItemAdd(menuPrint);

	menuSaveAs = new MenuItem();
	menuSaveAs->SetName(_T("menuSaveAs"));
	menuSaveAs->Click += EventHandler((Object*)this, (EventFun)&MainForm::tool_Click);
	//menuSaveAs->SetIcon(imageList, IDI_ICON_OPTION);
	contMenu->ItemAdd(menuSaveAs);

	menuIsBlock = new MenuItem();
	menuIsBlock->SetName(_T("menuIsBlock"));
	menuIsBlock->Click += EventHandler((Object*)this, (EventFun)&MainForm::tool_Click);
	menuIsBlock->SetChecked(this->isBlock);
	//menuIsBlock->SetIcon(imageList, IDI_ICON_OPTION);
	contMenu->ItemAdd(menuIsBlock);

	menuLang = new MenuItem();
	menuLang->SetName(_T("menuLang"));
	menuLang->Click += EventHandler((Object*)this, (EventFun)&MainForm::tool_Click);
	//menuLang->SetIcon(imageList, IDI_ICON_OPTION);
	contMenu->ItemAdd(menuLang);

	menuAbout = new MenuItem();
	menuAbout->SetName(_T("menuAbout"));
	menuAbout->Click += EventHandler((Object*)this, (EventFun)&MainForm::menuAbout_Click);
	contMenu->ItemAdd(menuAbout);

	menuOption = new MenuItem();
	menuOption->SetName(_T("menuOption"));
	menuOption->Click += EventHandler((Object*)this, (EventFun)&MainForm::tool_Click);
	menuOption->SetIcon(imageList, IDI_ICON_OPTION);
	contMenu->ItemAdd(menuOption);

	moveForm = new ToolItem();
	moveForm->SetName(_T("moveForm"));
	//moveForm->SetMoveForm(this);
	moveForm->SetLocation(-1, 0);
	moveForm->SetSize(20, 18);
	moveForm->SetText(NULL);
	moveForm->SetIcon(imageList, IDI_ICON_IE);
	moveForm->SetStyle(StyleType::Outer, 0, OuterStyle::AlwaysNoEdge);
	moveForm->DoubleClick += EventHandler((Object*)this, (EventFun)&MainForm::move_DoubleClick);
	moveForm->SetMouseDownMoveForm(this);
	this->AddControl(moveForm);

	toolsysmenu = new ToolItem();
	toolsysmenu->SetName(_T("toolsysmenu"));
	toolsysmenu->SetContextMenu(contMenu);
	toolsysmenu->SetText(NULL);
	toolsysmenu->SetLocation(20, 2);
	toolsysmenu->SetSize(11, 19);
	this->AddControl(toolsysmenu);

	//toolbar
	toolbar = new ToolBar();
	toolbar->SetName(_T("editTool"));
	toolbar->SetLocation(31, 0);
	toolbar->SetMinSize(0, 23);
	this->AddControl(toolbar);

	toolexit = new ToolItem();
	toolexit->SetName(_T("toolExit"));
	toolexit->SetMargin(2, 1, 2, 1);
	toolexit->SetIcon(imageList, IDI_ICON_EXIT);
	toolexit->Click += EventHandler((Object*)this, (EventFun)&MainForm::close_Click);
	toolbar->AddControl(toolexit);

	toolbreak1 = new ToolItem();
	toolbreak1->SetBreak(true);
	toolbar->AddControl(toolbreak1);

	toolattach = new ToolItem();
	toolattach->SetName(_T("toolAttach"));
	toolattach->SetMargin(2, 1, 2, 1);
	toolattach->SetIcon(imageList, IDI_ICON_ATTACH);
	toolattach->SetIsPushButton(true);
	toolattach->SetPushed(true);
	toolbar->AddControl(toolattach);

	toolrefresh = new ToolItem();
	toolrefresh->SetName(_T("toolrefresh"));
	toolrefresh->SetMargin(2, 1, 2, 1);
	toolrefresh->SetIcon(imageList, IDI_ICON_REFRESH);
	toolrefresh->Click += EventHandler((Object*)this, (EventFun)&MainForm::tool_Click);
	toolbar->AddControl(toolrefresh);

	toolstop = new ToolItem();
	toolstop->SetName(_T("toolstop"));
	toolstop->SetMargin(2, 1, 2, 1);
	toolstop->SetIcon(imageList, IDI_ICON_STOP);
	toolstop->Click += EventHandler((Object*)this, (EventFun)&MainForm::tool_Click);
	toolbar->AddControl(toolstop);

	//126 - 148
	editURL = new TextBox();
	editURL->SetName(_T("editURL"));
	editURL->SetText(_T(""));
	editURL->SetStyle(StyleType::Style, 0, ES_NOHIDESEL|WS_TABSTOP);
	editURL->SetLocation(126, 3);
	editURL->SetSize(600 - 126 - 21, 17);
	editURL->Anchor = (AnchorStyles::Type)(AnchorStyles::Top | AnchorStyles::Left | AnchorStyles::Right);
	editURL->KeyDown += KeyEventHandler((Object*)this, (KeyEventFun)&MainForm::editURL_KeyDown);
	this->AddControl(editURL);

	toolgo = new ToolItem();
	toolgo->SetName(_T("toolgo"));
	toolgo->SetLocation(600 - 20, 2);
	//toolgo->SetMargin(2, 1, 2, 1);
	toolgo->SetSize(16, 18);
	toolgo->SetIcon(imageList, IDI_ICON_GO);
	toolgo->Click += EventHandler((Object*)this, (EventFun)&MainForm::tool_Click);
	toolgo->Anchor = (AnchorStyles::Type)(AnchorStyles::Top | AnchorStyles::Right);
	toolgo->MinPosition.x = 126 + 2;
	this->AddControl(toolgo);

	ie = new IEControl();
	ie->SetName(_T("ie"));
	ie->SetLocation(0, 23);
	ie->SetSize(600, 480 - 23);
	ie->Anchor = (AnchorStyles::Type)(AnchorStyles::Top | AnchorStyles::Left | AnchorStyles::Right | AnchorStyles::Bottom);
	this->AddControl(ie);

	miniIE = new miniIE_IEEvent();
	miniIE->SetParent(this, moveForm, editURL);
	ie->SetEventHandler(miniIE); //be deleted at ie

	this->DisposeItemAdd(contMenu);
	this->DisposeItemAdd(imageList);
	this->SetName(_T("MiniIE"));
	this->SetText(App::GetNameVersion());
	this->SetConfigName(_T(""));
	this->SetContextMenu(contMenu);
	this->SetStyle(StyleType::Style, 0xFFFFFFFF, WS_VISIBLE|WS_CHILD|WS_THICKFRAME);
	this->SetStyle(StyleType::Style, WS_CAPTION, DS_CENTER);
	this->SetStyle(StyleType::ExStyle, 0xFFFFFFFF, WS_EX_WINDOWEDGE|WS_EX_TOOLWINDOW|WS_EX_TOPMOST);
	this->SetReBarStyle(ReBarStyle::Fix);
	this->SetMoveFormOnClick(true);
	this->SetLocation(100, 100);
	this->SetClientSize(600, 480);
	this->SetSize(230, 100);
	this->SetVisible(true);
	this->Closing += CancelEventHandler((Object*)this, (CancelEventFun)&MainForm::Form_Closing);
	this->Closed += EventHandler((Object*)this, (EventFun)&MainForm::Form_Closed);
	this->Deactivate += EventHandler((Object*)this, (EventFun)&MainForm::Form_Deactivate);
	this->Load += EventHandler((Object*)this, (EventFun)&MainForm::Form_Load);
	::CopyRect(&rcNormal, &_Rect);

	SetLang(*CC_APP_GetLang(), cc::Str(CC_CONFIG_LANGSECTION));
}

void MainForm::DoSetLang(KeyArr& keyArr, cc::Str sThisHead)
{
	Form::DoSetLang(keyArr, sThisHead);
	contMenu->SetLang(keyArr, sThisHead);
}

void MainForm::OnResize(EventArgs* e)
{
	Form::OnResize(e);
	toolgo->SetVisible(false);
	toolgo->SetVisible(true);
}

void MainForm::Form_Deactivate(Object* sender, EventArgs* e)
{
	if(!isModal && !toolattach->GetPushed() && !GetIsMin())
	{
		SetIsMin(true);
	}
}

void MainForm::Form_Load(Object* sender, EventArgs* e)
{
	cc::RegistryKey* registryKey = cc::Registry::GetCurrentUser();
	registryKey->OpenSubKey(_T("SOFTWARE\\Microsoft\\Internet Explorer\\Main\\"), true);
	cc::Str val = registryKey->GetValue(_T("Start Page"));
	registryKey->Close();
	delete registryKey;

	editURL->SetText((TCHAR*)val.GetBuf());
	cc::Str url = cc::Str(cc::LangCode::UNICODE16, val);
	ie->Navigate((wchar_t*)url.GetBuf());
	rcNormal.left = CC_APP_GetIni()->Get(_T("Main.Left"), 0);
	rcNormal.top = CC_APP_GetIni()->Get(_T("Main.Top"), 0);
	rcNormal.right = CC_APP_GetIni()->Get(_T("Main.Width"), 400); //width
	rcNormal.bottom = CC_APP_GetIni()->Get(_T("Main.Height"), 150); //height
	cc::Util::AdjustWindowRect((int&)rcNormal.left, (int&)rcNormal.top, (int&)rcNormal.right, (int&)rcNormal.bottom);
	rcNormal.right += rcNormal.left; //now is right
	rcNormal.bottom += rcNormal.top; //now is bottom
	this->SetBoundsNotOriginal(rcNormal);
	CC_APP_GetIni()->Set(_T("Main.Left"), rcNormal.left + 30);
	CC_APP_GetIni()->Set(_T("Main.Top"), rcNormal.top + 30);
}

void MainForm::Form_Closing(Object* sender, CancelEventArgs* e)
{
	nIECount--;
	if(nIECount > 0)
	{
		if(hMainIEForm == this)
		{
			//this is not last one,cannot release main form
			SetVisible(false);
			e->Cancel = true;
		}
	}
	else
	{
		if(hMainIEForm != this)
		{
			::SendMessage(hMainIEForm->m_hWnd, WM_CLOSE, NULL, NULL);
		}
	}
}

void MainForm::Form_Closed(Object* sender, EventArgs* e)
{
	if(mapHWND != NULL && mapHWND->GetSize() == 1)
	{
		StopMsgHook();
	}
	mapHWND->Remove(hWndIE);
	rcNormal = this->GetNormalBounds();
	CC_APP_GetIni()->Set(_T("Main.Left"), rcNormal.left);
	CC_APP_GetIni()->Set(_T("Main.Top"), rcNormal.top);
	CC_APP_GetIni()->Set(_T("Main.Width"), rcNormal.right - rcNormal.left);
	CC_APP_GetIni()->Set(_T("Main.Height"), rcNormal.bottom - rcNormal.top);
	//CC_APP_GetConfig()->SaveIni();
}

void MainForm::SetIsMin(bool isMin)
{
	if(this->isMin != isMin)
	{
		this->isMin = isMin;
		if(!isMin)
		{
			RECT rcTmp = GetBounds();
			this->SetBoundsNotOriginal(rcTmp.left, rcTmp.top, rcNormal.right - rcNormal.left, rcNormal.bottom - rcNormal.top);
			this->SetStyle(StyleType::Style, WS_DLGFRAME, WS_THICKFRAME, true);
			toolsysmenu->SetEnabled(true);
			::SetWindowPos(this->m_hWnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_DRAWFRAME);
			moveForm->SetLocation(-1, 0);
		}
		else
		{
			this->SetStyle(StyleType::Style, WS_THICKFRAME, WS_DLGFRAME, true);
			rcNormal = GetBounds();
			toolsysmenu->SetEnabled(false);
			this->SetBoundsNotOriginal(rcNormal.left, rcNormal.top, 20, 21);
			moveForm->SetLocation(-4, -3);
		}
	}
}

bool MainForm::GetIsMin()
{
	return isMin;
}

void MainForm::SetIsFix(bool isFix)
{
	toolattach->SetPushed(isFix);
}

bool MainForm::GetIsFix()
{
	return toolattach->GetPushed();
}

RECT MainForm::GetNormalBounds()
{
	if(isMin)
	{
		int nW = rcNormal.right - rcNormal.left;
		int nH = rcNormal.bottom - rcNormal.top;
		RECT rc = GetBounds();
		rcNormal.left = rc.left;
		rcNormal.top = rc.top;
		rcNormal.right = rc.left + nW;
		rcNormal.bottom = rc.top + nH;
		return rcNormal;
	}
	return GetBounds();
}

void MainForm::move_DoubleClick(Object* sender, EventArgs* e)
{
	this->SetIsMin(!this->GetIsMin());
}

void MainForm::close_Click(Object* sender, EventArgs* e)
{
	//notice thar wparam is 1 for saveconfig while Form_Closed
	this->SetTips(NULL);
	::SendMessage(m_hWnd, WM_CLOSE, 1, NULL);
}

MainForm* MainForm::NewMiniIE()
{
	MainForm* ieForm = new MainForm();
	ieForm->SetVisible(false);
	ieForm->CreateControl();
	ieForm->SetVisible(true);
	return ieForm;
}

void MainForm::NewWin_Click(Object* sender, EventArgs* e)
{
	NewMiniIE();
}

void MainForm::print_Click(Object* sender, EventArgs* e)
{
	ie->GetWebBrowser()->ExecWB((ie_OLECMDID)7, (ie_OLECMDEXECOPT)1, NULL, NULL);
}

void MainForm::editURL_KeyDown(Object* sender, KeyEventArgs* e)
{
	if(e != NULL && e->baseArgs->WParam == VK_RETURN)
	{
		tool_Click(toolgo, e->baseArgs);
	}
}

void MainForm::Favorites2Menu(MenuItem* item, cc::Str path)
{
	if(!path.EndsWith(_T("\\")))
	{
		path.Append(_T("\\"));
	}
	//load all text file from dir of note
	cc::List<cc::Str> arrFullName;
	cc::Str filefilter = path + _T("*.url");
	path.Append(_T("*.*"));
	int nCnt = cc::Util::getDirs(arrFullName, (TCHAR*)path.GetBuf(), false);
	for(int i = 0; i < nCnt; i++)
	{
		MenuItem* subitem = new MenuItem();
		cc::Str nameonly = cc::Util::getLastSubDirName((const TCHAR*)arrFullName.GetKey(i).GetBuf());
		subitem->SetText((const TCHAR*)nameonly.GetBuf());
		subitem->SetIcon(imageList, IDI_ICON_FOLDER);
		item->ItemAdd(subitem);
		Favorites2Menu(subitem, arrFullName.GetKey(i));
	}
	arrFullName.Clear();
	nCnt = cc::Util::getFiles(arrFullName, (TCHAR*)filefilter.GetBuf(), false);
	for(int i = 0; i < nCnt; i++)
	{
		MenuItem* subitem = new MenuItem();
		cc::Str nameonly = cc::Util::getFileNameNoExt((const TCHAR*)arrFullName.GetKey(i).GetBuf());
		subitem->SetText((const TCHAR*)nameonly.GetBuf());

		memset(tcBuf, 0, MAX_PATH);
		// an .URL file is formatted just like an .INI file, so we can use GetPrivateProfileString() to get the information we want
 		::GetPrivateProfileString(_T("InternetShortcut"), _T("URL"), _T(""), tcBuf, MAX_PATH, (const TCHAR*)arrFullName.GetKey(i).GetBuf());
		subitem->SetTips(tcBuf);

		subitem->Click += EventHandler((Object*)this, (EventFun)&MainForm::menuFavorites_Click);
		subitem->SetIcon(imageList, IDI_ICON_PAGE);
		item->ItemAdd(subitem);
	}
}

void MainForm::menuFavorites_Popup(Object* sender, EventArgs* e)
{
	int nCnt = menuFavorites->ItemCount();
	if(nCnt > 3)
	{
		for(int i = nCnt - 1; i >= 3; i--)
		{
			//delete menuFavorites->ItemGet(i);
			Menu* menu = menuFavorites->ItemRemove(i);
			delete menu;
		}
	}

	memset(tcBuf, 0, MAX_PATH);
	if(::SHGetSpecialFolderPath(m_hWnd, tcBuf, CSIDL_FAVORITES, FALSE))
	{
		Favorites2Menu(menuFavorites, cc::Str(tcBuf));
	}
}
void MainForm::menuFavorites_Click(Object* sender, EventArgs* e)
{
	if(sender == menuFavAdd)
	{
		#define ID_IE_ID_ADDFAV 2261
		GetIEHwnd();
		::SendMessage(hWndIE, WM_COMMAND, MAKEWPARAM(LOWORD(ID_IE_ID_ADDFAV), 0x0), 0);
	}
	else if(sender == menuFavMan)
	{
		memset(tcBuf, 0, MAX_PATH);
		if(!::SHGetSpecialFolderPathA(m_hWnd, (char*)tcBuf, CSIDL_FAVORITES, TRUE))
		{
			MessageBox(m_hWnd, _T("Error SHGetSpecialFolderPath of CSIDL_FAVORITES."), NULL, MB_ICONERROR);
			return;
		}

		typedef UINT (CALLBACK* LPFNORGFAV)(HWND, char*);
		bool bIsLoadDll = false;
		HMODULE hMod = ::GetModuleHandle(_T("shdocvw.dll"));
		if(hMod == NULL)
		{
			hMod = ::LoadLibrary(_T("shdocvw.dll"));
			bIsLoadDll = true;
			if(hMod == NULL)
			{
				MessageBox(m_hWnd, _T("Error load shdocvw.dll."), NULL, MB_ICONERROR);
				return;
			}
		}
		LPFNORGFAV lpfnDoOrganizeFavDlg = (LPFNORGFAV)::GetProcAddress(hMod, "DoOrganizeFavDlg");
		if(lpfnDoOrganizeFavDlg == NULL)
		{
			MessageBox(m_hWnd, _T("Error GetProcAddress of DoOrganizeFavDlg."), NULL, MB_ICONERROR);
			return;
		}

		(*lpfnDoOrganizeFavDlg)(m_hWnd, (char*)tcBuf);
		if(bIsLoadDll)
		{
			::FreeLibrary(hMod);
		}
	}
	else if(sender != NULL)
	{
		Menu* menu = (Menu*)sender;
		const TCHAR* tips = menu->GetTips();
		if(tips != NULL)
		{
			cc::Str url = cc::Str(cc::LangCode::UNICODE16, tips);
			editURL->SetText(tips);
			ie->Navigate((wchar_t*)url.GetBuf());
			ie->Focus();
		}
	}
}

void MainForm::tool_Click(Object* sender, EventArgs* e)
{
	if(sender == toolrefresh || sender == menuReload)
	{
		ie->GetWebBrowser()->ExecWB((ie_OLECMDID)22, (ie_OLECMDEXECOPT)1, NULL, NULL);
	}
	else if(sender == toolstop)
	{
		ie->GetWebBrowser()->ExecWB((ie_OLECMDID)23, (ie_OLECMDEXECOPT)1, NULL, NULL);
	}
	else if(sender == menuOpenFile)
	{
		isModal = true;
		cc::Str path = cc::Util::selectOpenFile(_T(""), _T("HTML Files (*.HTML,*.HTM,*.TXT)\0*.HTML;*.HTM;*.TXT\0\0"), _T(".html"), m_hWnd, _T("select html file:"));
		if(path.GetLength() > 0)
		{
			cc::Str url = cc::Str(cc::LangCode::UNICODE16, path);
			editURL->SetText((TCHAR*)path.GetBuf());
			ie->Navigate((wchar_t*)url.GetBuf());
			ie->Focus();
		}
		isModal = false;
	}
	else if(sender == menuSaveAs)
	{
		isModal = true;
		ie->GetWebBrowser()->ExecWB((ie_OLECMDID)4, (ie_OLECMDEXECOPT)0, NULL, NULL);
		isModal = false;
	}
	else if(sender == menuOption)
	{
		typedef BOOL (WINAPI *LAUNCHCPL) (HWND);
		HMODULE hInetcpl = LoadLibrary(_T("inetcpl.cpl"));
		if(hInetcpl != NULL)
		{
			LAUNCHCPL cpl = (LAUNCHCPL)GetProcAddress(hInetcpl, "LaunchInternetControlPanel");
			if(cpl)
			{
				cpl(m_hWnd);
			}
			FreeLibrary(hInetcpl);
		}
	}
	else if(sender == toolgo)
	{
		editURL->SetReadOnly(true);
		cc::Str url = cc::Str(cc::LangCode::UNICODE16, editURL->GetText());
		if(url.GetLength() > 0)
		{
			ie->Navigate((wchar_t*)url.GetBuf());
			ie->Focus();
		}
		editURL->SetReadOnly(false);
	}
	else if(sender == menuIsBlock)
	{
		this->isBlock = !this->isBlock;
		menuIsBlock->SetChecked(this->isBlock);
	}
	else if(sender == menuLang)
	{
		isModal = true;
		SelLang selLang;
		selLang.SetParent(this, true);
		selLang.SetCenter();
		if(selLang.ShowDialog() == DialogResult::OK)
		{
			cc::win::App::GetConfig()->LoadLang(CC_CONFIG_LANGENG);
			SetLang(*CC_APP_GetLang(), cc::Str(CC_CONFIG_LANGSECTION));
		}
		selLang.Dispose();
		isModal = false;
	}
}

void MainForm::menuAbout_Click(Object* sender, EventArgs* e)
{
	cc::Str sAbout = App::GetNameVersion();
	sAbout.Append(_T(" ")).Append(APPBUILD);
	AboutDlg* input = new AboutDlg(sAbout, imageList, IDI_ICON_MAIN, CC_APP_GetLang()->Get(_T("Lang.U_CopyRightTitle")), 
		420, 36, 36, CC_APP_GetLang()->Get(_T("Lang.U_CopyRight")), true, true);
	input->SetParent(this, true);
	input->SetCenter();
	input->ShowDialog();

	input->Dispose();
	delete input;
}

void MainForm::GetIEHwnd()
{
	if(hWndIE == NULL)
	{
		//start hook for ie
		hWndIE = ie->GetWebBrowserHWND(); //Shell Embedding
		hWndIE = ::GetWindow(hWndIE, GW_CHILD); //Shell DocObject View
		hWndIE = ::GetWindow(hWndIE, GW_CHILD); //Internet Explorer_Server
		if(hWndIE != NULL)
		{
			//cc::Log::debug(_T("ie->GetWebBrowser()->get_HWND():%x"), hWndIE);
			mapHWND->Add(hWndIE, this);
			if(gMsgHook == NULL)
			{
				StartMsgHook(CC_APP_GetInstance());
			}
		}
	}
}
