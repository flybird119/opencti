
// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#define WINDOW_MENU_POSITION	3


#define  CHECKCOMMAND           \
    if(wNotifyCode >1 )         \
    { bHandled=FALSE;return 0;}



class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CTabView m_view;
	CCommandBarCtrl m_CmdBar;
    CTabCtrlXbuttonImpl m_tabctrl;

    CTrayIcon           m_trayicon;

    BOOL                m_bclose;

    int                 m_day;

    std::map<std::string,CMonitorView*>  m_channel;


	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

		return m_view.PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateToolBar();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)



        MESSAGE_HANDLER(WM_PRINTLOG,OnPrintLog);
        MESSAGE_HANDLER(WM_SHOWLOG,OnShowLog);

        

        MESSAGE_HANDLER(WM_TIMER,OnTimer);


        MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)

        MESSAGE_HANDLER(WM_CLOSE, OnClose)
        MESSAGE_HANDLER(WM_FREECFGDLG,OnFreeCfgDlg )

        

		COMMAND_ID_HANDLER(ID_TRAYICON_SHOW, OnTrayIconShow)
        COMMAND_ID_HANDLER(ID_LOG_CONFIG, OnLogConfig)
        COMMAND_ID_HANDLER(ID_LOG_CLEAR, OnLogClear)



        COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_WINDOW_CLOSE, OnWindowClose)
		COMMAND_ID_HANDLER(ID_WINDOW_CLOSE_ALL, OnWindowCloseAll)
		COMMAND_RANGE_HANDLER(ID_WINDOW_TABFIRST, ID_WINDOW_TABLAST, OnWindowActivate)

        COMMAND_RANGE_HANDLER(ID_MENU_LOG_BEGIN, ID_MENU_LOG_END, OnShowLogWindow)

		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)



    LRESULT OnPrintLog(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {

    

        struct LOGINFO 
        {
            const char* channel;
            char* pstrstream;       
        } *pLogInfo = (LOGINFO*)wParam;
        LoggingSeverity__ sev = (LoggingSeverity__)lParam;
        

        std::map<std::string,CMonitorView*>::iterator iter=m_channel.find(pLogInfo->channel);

      
        CMonitorView* pViewLog=NULL;

        if(iter==m_channel.end())
        {
          
            pViewLog=CreateLogView(pLogInfo->channel);
            m_channel[pLogInfo->channel]=pViewLog;


                HMENU hSubMenu;
            
                if ((hSubMenu = ::GetSubMenu(m_CmdBar.GetMenu(),5)))
                {
                    ::AppendMenuA(hSubMenu,MF_CHECKED|MF_BYCOMMAND|MF_STRING,ID_MENU_LOG_BEGIN+m_channel.size(),pLogInfo->channel);
                   
                }
        }
        else
        {
            pViewLog=iter->second;
        }


        //如果pstrstream为空是只通知增加视图
        if(pViewLog && pLogInfo->pstrstream)
        {
            COLORREF col=RGB(0,0,0);

            switch(sev)
            {
            case LEVEL_SENSITIVE:
                col=RGB(120,120,120);
                break;
            case LEVEL_VERBOSE:
                col=RGB(60,60,60);
                break;
            case LEVEL_INFO:
                col=RGB(0,0,0);
                break;

            case LEVEL_WARNING:
                col=RGB(255,120,120);
                break;

            case LEVEL_ERROR:
                col=RGB(255,60,60);
                break;

            case LEVEL_FATAL:
                col=RGB(255,0,0);
                break;

            }

            int nOldLines = 0, nNewLines = 0, nScroll = 0;
            CHARFORMAT cf;
            cf.cbSize		= sizeof(CHARFORMAT);
            cf.dwMask		= CFM_COLOR;
            cf.dwEffects	= CFE_DISABLED;	// To disable CFE_AUTOCOLOR;
            cf.crTextColor	= col;
            cf.cbSize		= sizeof(CHARFORMAT);


            //pViewLog->LockWindowUpdate(TRUE);   
            //pViewLog->HideSelection(TRUE,   FALSE);   
  
            //LONG nStartChar, nEndChar;
            //pViewLog->GetSel(nStartChar,nEndChar);

            nOldLines=(int)::SendMessage(pViewLog->m_hWnd, EM_GETLINECOUNT, 0, 0);
            if(nOldLines>1000)
            {

                int linelen=(int)::SendMessageA(pViewLog->m_hWnd, EM_LINEINDEX, nOldLines, 0); 
                ::SendMessage(pViewLog->m_hWnd, EM_SETSEL,0,linelen);
                ::SendMessage(pViewLog->m_hWnd, EM_REPLACESEL, (WPARAM) 0, (LPARAM)_T(""));



            }
            ::SendMessage(pViewLog->m_hWnd, EM_SETSEL,-1,-1);			
            ::SendMessage(pViewLog->m_hWnd, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
            ::SendMessageA(pViewLog->m_hWnd, EM_REPLACESEL, (WPARAM) 0, (LPARAM)pLogInfo->pstrstream);	

        

            //nNewLines=(int)::SendMessage(pViewLog->m_hWnd, EM_GETLINECOUNT, 0, 0);
            //nScroll=nNewLines-nOldLines;
            //::SendMessage(pViewLog->m_hWnd, EM_LINESCROLL, 0, nScroll);
//            pViewLog->SetSel(nStartChar,nEndChar);
//            pViewLog->HideSelection(FALSE,   FALSE);   
//            pViewLog->LockWindowUpdate(FALSE);   


        }


      
        if(pLogInfo->pstrstream)
        {
            free(pLogInfo->pstrstream);
        }


        delete pLogInfo;




        return 1;
    }

    LRESULT OnShowLog(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        char* pchannelname=(char*)wParam;
        BOOL bShow=(BOOL)lParam;
        std::map<std::string,CMonitorView*>::iterator iter=m_channel.find(pchannelname);


        CMonitorView* pViewLog=NULL;


        if(iter==m_channel.end())
        {

            if(bShow)
            {
                pViewLog=CreateLogView(pchannelname);
            }

          

            m_channel[pchannelname]=pViewLog;

           


            HMENU hSubMenu;

            if ((hSubMenu = ::GetSubMenu(m_CmdBar.GetMenu(),5)))
            {
                ::AppendMenuA(hSubMenu,MF_CHECKED|MF_BYCOMMAND|MF_STRING,ID_MENU_LOG_BEGIN+m_channel.size(),pchannelname);

            }
        }
        else if(bShow)
        {
            ShowLogView(pchannelname);           
        }
        else if(!bShow)
        {
            CloseLogView(pchannelname,NULL);
        }


        delete []pchannelname;

        return 1;
    }


    
    LRESULT OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        if(wParam==100)
        {
            SYSTEMTIME st;
            ::GetLocalTime(&st);
            if(st.wDay!=m_day)
            {
                m_day=st.wDay;
                CLogServer::GetInstance()->TimerUpDataFile();
      
            }
        }
        else
        {
            bHandled=FALSE;
        }

        return 0;
    }



	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// create command bar window
		HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
		// attach menu
		m_CmdBar.AttachMenu(GetMenu());
		// load command bar images
		m_CmdBar.LoadImages(IDR_MAINFRAME);
		// remove old menu
		SetMenu(NULL);

		HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

		CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
		AddSimpleReBarBand(hWndCmdBar);
		AddSimpleReBarBand(hWndToolBar, NULL, TRUE);

		CreateSimpleStatusBar();

		m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CLIENTEDGE);

        m_tabctrl.SubclassWindow(m_view.m_tab.m_hWnd);


        

		UIAddToolBar(hWndToolBar);
		UISetCheck(ID_VIEW_TOOLBAR, 1);
		UISetCheck(ID_VIEW_STATUS_BAR, 1);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);

		CMenuHandle menuMain = m_CmdBar.GetMenu();
		m_view.SetWindowMenu(menuMain.GetSubMenu(WINDOW_MENU_POSITION));


        //其它初始化代码

        m_trayicon.Create(m_hWnd,WM_TRAYICON,_T("Log Monitor"),LoadIcon( _Module.GetResourceInstance(),MAKEINTRESOURCE(IDR_MAINFRAME)),IDR_TRAYICON,_Module.GetResourceInstance());


        SetTimer(100,10000);
        SYSTEMTIME st;
        ::GetLocalTime(&st);
        m_day=st.wDay;

        m_bclose=FALSE;

		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{

        m_view.RemoveAllPages();


        for(std::map<std::string,CMonitorView*>::iterator iter=m_channel.begin();iter!=m_channel.end();++iter)
        {
            delete iter->second;
        }

        m_channel.clear();


        

		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		bHandled = FALSE;
		return 0;
	}


    LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        if(!m_bclose)
        {
            g_LogOnOff=FALSE;
            m_trayicon.MinimizeToTray();
        }
        else
        {
            bHandled=FALSE;

        }

        return 0;
    }

    LRESULT OnFreeCfgDlg(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {

        CLogConfigDlg*  pLogCfgDlg=(CLogConfigDlg*)wParam;

        pLogCfgDlg->DestroyWindow();
        delete pLogCfgDlg;

        return 0;
    }

    

    LRESULT OnTrayIconShow(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
    {
        CHECKCOMMAND

        g_LogOnOff=TRUE;
        
        ::ShowWindow(m_hWnd,SW_RESTORE);
        return 0;
    }

    LRESULT OnLogConfig(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
    {
        CHECKCOMMAND

        int ActiveIndex=m_view.GetActivePage();
        if(ActiveIndex>=0)
        {
            HWND ActivehWnd=m_view.GetPageHWND(ActiveIndex);
            if(ActivehWnd)
            {
                for(std::map<std::string,CMonitorView*>::iterator iter=m_channel.begin();iter!=m_channel.end();++iter)
                {
                    if(iter->second && iter->second->m_hWnd==ActivehWnd)
                    {

                        LPCHANNELINFO pchinfo=CLogServer::GetInstance()->GetChannelInfo(iter->first.c_str());
             

                        if(pchinfo)
                        {
                            CLogConfigDlg*  pLogCfgDlg=new CLogConfigDlg;
                            pLogCfgDlg->Create(m_hWnd,(LPARAM)pchinfo);
                            ::SetWindowTextA(pLogCfgDlg->m_hWnd,iter->first.c_str());
                            pLogCfgDlg->ShowWindow(SW_SHOW);

                        }

                        break;
                    }
                }


            }



        }

        return 0;
    }


    LRESULT OnLogClear(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
    {
        CHECKCOMMAND

        int ActiveIndex=m_view.GetActivePage();
        if(ActiveIndex>=0)
        {
            HWND ActivehWnd=m_view.GetPageHWND(ActiveIndex);

            int nOldLines=(int)::SendMessage(ActivehWnd, EM_GETLINECOUNT, 0, 0);
            int linelen=(int)::SendMessageA(ActivehWnd, EM_LINEINDEX, nOldLines, 0); 
            ::SendMessage(ActivehWnd, EM_SETSEL,0,linelen);
            ::SendMessage(ActivehWnd, EM_REPLACESEL, (WPARAM) 0, (LPARAM)_T(""));

        }

        return 0;
    }
    

    

	LRESULT OnFileExit(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
        CHECKCOMMAND

        if(wNotifyCode==1)
            m_bclose=TRUE;

        PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnFileNew(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
        CHECKCOMMAND
       

        return 1;
		CMonitorView* pView = new CMonitorView;
		pView->Create(m_view, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | ES_NOHIDESEL | ES_SAVESEL, 0);
		pView->SetFont(AtlGetDefaultGuiFont());

  
        m_view.AddPage(pView->m_hWnd, _T("Document   "),-1,(LPVOID)(TAB_CLOSEBUTTON|TAB_CONFIGBUTTON|TAB_VERIFY));
      
		// TODO: add code to initialize document


		return 0;
	}

	LRESULT OnViewToolBar(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
        CHECKCOMMAND


		static BOOL bVisible = TRUE;	// initially visible
		bVisible = !bVisible;
		CReBarCtrl rebar = m_hWndToolBar;
		int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
		rebar.ShowBand(nBandIndex, bVisible);
		UISetCheck(ID_VIEW_TOOLBAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnViewStatusBar(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
        CHECKCOMMAND


		BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
		::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
		UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnAppAbout(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
        CHECKCOMMAND


		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}

	LRESULT OnWindowClose(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
	{
        CHECKCOMMAND

		int nActivePage = m_view.GetActivePage();
		if(nActivePage != -1)
        {
            HWND tmphWnd=m_view.GetPageHWND(nActivePage);
			
            CloseLogView(NULL,tmphWnd);

        }
		else
			::MessageBeep((UINT)-1);

		return 0;
	}

	LRESULT OnWindowCloseAll(WORD wNotifyCode, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
    {
        CHECKCOMMAND
    
		m_view.RemoveAllPages();
        for(std::map<std::string,CMonitorView*>::iterator iter=m_channel.begin();iter!=m_channel.end();++iter)
        {
            if(iter->second)
            {
                delete  iter->second;
                iter->second=NULL;
            }
        }

        //清消菜单选中状态
        HMENU hSubMenu;
        if ((hSubMenu = ::GetSubMenu(m_CmdBar.GetMenu(),5)))
        {
            int menucount=::GetMenuItemCount(hSubMenu);
            {
                for(int i=0;i<menucount;++i)
                {
                    ::CheckMenuItem(hSubMenu,i,MF_BYPOSITION|MF_UNCHECKED);   
                }
            }
        }

		return 0;
	}

LRESULT OnWindowActivate(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
{

    CHECKCOMMAND

	int nPage = wID - ID_WINDOW_TABFIRST;
	m_view.SetActivePage(nPage);





	return 0;
}
LRESULT OnShowLogWindow(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
{
   
    CHECKCOMMAND

    if(wID<ID_MENU_LOG_BEGIN || wID>ID_MENU_LOG_END)
    {

        bHandled=FALSE;
        return 0;
    }


    HMENU hSubMenu;
    if ((hSubMenu = ::GetSubMenu(m_CmdBar.GetMenu(),5)))
    {
        char menustr[256]="\0";

        ::GetMenuStringA(hSubMenu,wID,menustr,256,MF_BYCOMMAND);

        ::CheckMenuItem(hSubMenu,wID,MF_BYCOMMAND|MF_CHECKED);   


/*
        //点状显示
        MENUITEMINFO   miInfo;   
        miInfo.cbSize   =   sizeof(MENUITEMINFO);   

        TCHAR   szMenu[64];   
        miInfo.fMask   =   MIIM_TYPE   |   MIIM_STATE;   
        (LPTSTR)   miInfo.dwTypeData   =   szMenu;   
        miInfo.cch   =   sizeof(menustr);   
        GetMenuItemInfo(hSubMenu,   wID,   FALSE,   &miInfo);   
        miInfo.fType   |=   MFT_RADIOCHECK;   
        miInfo.fState   =   MFS_CHECKED;   
        SetMenuItemInfo(hSubMenu,   wID,   FALSE,   &miInfo);
*/
     
    
        ShowLogView(menustr);

    }

    return 0;
}




CMonitorView* CreateLogView(const char* channel)
{
    CMonitorView* pView = new CMonitorView;

    pView->Create(m_view, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE |  ES_SAVESEL | ES_NOHIDESEL | ES_READONLY, 0);
    pView->SetFont(AtlGetDefaultGuiFont());

    CString ChannelName(channel);
    ChannelName+=_T("   ");
    m_view.AddPage(pView->m_hWnd, ChannelName,-1,(LPVOID)(TAB_CLOSEBUTTON|TAB_VERIFY));//TAB_CONFIGBUTTON

    return pView;
}



CMonitorView* ShowLogView(const char* channel)
{
    std::map<std::string,CMonitorView*>::iterator iter=m_channel.find(channel);


    CMonitorView* pViewLog=NULL;

    if(iter!=m_channel.end())
    {
        pViewLog=iter->second;
        if(pViewLog)
        {
            for(int i=0;i<m_view.GetPageCount();++i)
            {
                if(m_view.GetPageHWND(i)==pViewLog->m_hWnd)
                {
                    m_view.SetActivePage(i);
                    break;
                }
            }
        }
        else
        {
            iter->second=CreateLogView(channel);
        }
    }
    else
    {
        ATLASSERT(0);
    }

    return pViewLog;
}



void CloseLogView(const char* channel,HWND hWnd)
{

    std::map<std::string,CMonitorView*>::iterator iter;

    if(channel!=NULL)
    {
        iter=m_channel.find(channel);

    }
    else if(hWnd!=NULL)
    {
        for(iter=m_channel.begin();iter!=m_channel.end();++iter)
        {
            if(iter->second && iter->second->m_hWnd==hWnd)
            {
                break;
            }
        }
    }

    if(iter!=m_channel.end()  && iter->second!=NULL)
    {

        for(int i=0;i<m_view.GetPageCount();++i)
        {
            if(m_view.GetPageHWND(i)==iter->second->m_hWnd)
            {
                m_view.RemovePage(i);

                break;
            }

        }


        delete iter->second;
        iter->second=NULL;

        //清消菜单选中状态
        HMENU hSubMenu;
        if ((hSubMenu = ::GetSubMenu(m_CmdBar.GetMenu(),5)))
        {
            int menucount=::GetMenuItemCount(hSubMenu);
            {
                char menustr[256]="\0";

                for(int i=0;i<menucount;++i)
                {
                    ::GetMenuStringA(hSubMenu,i,menustr,256,MF_BYPOSITION);

                    if(iter->first==menustr)
                    {
                        ::CheckMenuItem(hSubMenu,i,MF_BYPOSITION|MF_UNCHECKED);
                        break;
                    }
                }


            }
        }        
        
    }


    





 
}




};
