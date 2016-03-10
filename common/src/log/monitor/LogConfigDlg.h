
// LogConfig.h : interface of the CLogConfig class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CLogConfigDlg : public CDialogImpl<CLogConfigDlg>
{
public:

    LPCHANNELINFO pchinfo;

    enum { IDD = IDD_CONFIGDLG };

    BEGIN_MSG_MAP(CLogConfigDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
    END_MSG_MAP()

    // Handler prototypes (uncomment arguments if needed):
    //	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    //	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    //	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
    {
        CenterWindow(GetParent());

         pchinfo=(LPCHANNELINFO)lParam;

         CComboBox combox=GetDlgItem(IDC_COMBO1);

         combox.InsertString(0,_T("Sensitive"));
         combox.InsertString(1,_T("Verbose"));
         combox.InsertString(2,_T("Info"));
         combox.InsertString(3,_T("Warning"));
         combox.InsertString(4,_T("Error"));
         combox.InsertString(5,_T("Fatal"));
         

         combox.SetCurSel((int)pchinfo->sev);



         SetDlgItemInt(IDC_EDIT1,pchinfo->total/1024/1024);
        

         if(pchinfo->flag & OUTFILE_ENABLE)
         {
            CButton checkbut=GetDlgItem(IDC_CHECK1);
            checkbut.SetCheck(BST_CHECKED);
         }
         if(pchinfo->flag & OUTMONITOR_ENABLE)
         {
             CButton checkbut=GetDlgItem(IDC_CHECK2);
             checkbut.SetCheck(BST_CHECKED);

         }



        return TRUE;
    }

    LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        if(wID==IDOK)
        {
            pchinfo->total=GetDlgItemInt(IDC_EDIT1)*1024*1024;
            CComboBox combox=GetDlgItem(IDC_COMBO1);
            pchinfo->sev=(LoggingSeverity__)combox.GetCurSel();
            
            CButton checkbut1=GetDlgItem(IDC_CHECK1);
            if(checkbut1.GetCheck())
            {
                pchinfo->flag|=OUTFILE_ENABLE;
            }
            else
            {
                pchinfo->flag&=~OUTFILE_ENABLE;

            }
            CButton checkbut2=GetDlgItem(IDC_CHECK2);
            if(checkbut2.GetCheck())
            {
                pchinfo->flag|=OUTMONITOR_ENABLE;

            }
            else
            {
                pchinfo->flag&=~OUTMONITOR_ENABLE;

            }

        }
     
        ::PostMessage(GetParent(),WM_FREECFGDLG,(WPARAM)this,0);

        return 0;
    }
};