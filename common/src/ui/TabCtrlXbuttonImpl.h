/********************************************************************
	created:	2008/12/06
	created:	6:12:2008   21:06
	filename: 	f:\xugood.comm\src\wtlextend\TabCtrlXbuttonImpl.h
	file path:	f:\xugood.comm\src\wtlextend
	file base:	TabCtrlXbuttonImpl
	file ext:	h
	author:		xugood@live.cn
	
	purpose:	
*********************************************************************/

#pragma once

#include <vector>

#include "./resource.h"

#define  TAB_VERIFY                 0x18000001
#define  TAB_FLOATBUTTON            0x2
#define  TAB_CLOSEBUTTON            0x4
#define  TAB_CONFIGBUTTON           0x8


class CTabCtrlXbuttonImpl :
    public CWindowImpl<CTabCtrlXbuttonImpl,CTabCtrl>
{

private:

    struct TABVIEWPAGE
    {
        HWND hWnd;
        LPTSTR lpstrTitle;
        LPVOID pData;
    };


    struct TCITEMEXTRA
    {
        TCITEMHEADER tciheader;
        TABVIEWPAGE tvpage;
    };

    class CButton
    {
    public:

        CButton(int index_)
        {
            index=index_;

            down=FALSE;
            highlight=FALSE;

        }

        const static int     rleft  = 5;      //和right 的偏移
        const static int     top    = 5;
        const static int     height = 12;
        const static int     width  = 12;
        UINT    cmdid;
        TCHAR   text;
        TCHAR   tip[256]; 
        BOOL    down;      //鼠标按下
        BOOL    highlight; //高亮显示
        int     index;     //索引

        RECT    rect;


        void SetRect(RECT&ItemRect)
        {
            rect.left=ItemRect.right-rleft-(index)*(width+2)-width;
            rect.top=top;
            rect.right=rect.left+width;
            rect.bottom=rect.top+height;

             

           
        }

    };

    class CloseButton : public CButton
    {
    public:
        CloseButton(int index_)
            :CButton(index_)
        {
            text='r';
            _tcscpy(tip,_T("关闭"));
            cmdid=ID_WINDOW_CLOSE;


        }
    };

    class FloatButton : public CButton
    {
    public:
        FloatButton(int index_)
            :CButton(index_)
        {
            text='1';
            _tcscpy(tip,_T("浮动"));

        }

    };

    class ConfigButton : public CButton
    {
    public:
        ConfigButton(int index_)
            :CButton(index_)
        {
            text='n';
            _tcscpy(tip,_T("配置"));
            cmdid=ID_WINDOW_CONFIG;

        }

    };





    HWND                  origcapturehwnd;
    BOOL                  bcapture;
    int                   highlightnItem;



public:
    BEGIN_MSG_MAP(CTabCtrlXbuttonImpl)
        MESSAGE_HANDLER(WM_MOUSEMOVE,OnMouseMove)
        MESSAGE_HANDLER(WM_LBUTTONUP,OnLButtonUp)
        MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDwon)
        NOTIFY_CODE_HANDLER(TTN_GETDISPINFO, OnTabGetDispInfo)
        MESSAGE_HANDLER(TCM_INSERTITEM,OnInsertItem);
        MESSAGE_HANDLER(TCM_DELETEITEM,OnDeleteItem);
        MESSAGE_HANDLER(TCM_DELETEALLITEMS,OnDeleteAllItems);
        MESSAGE_HANDLER(WM_PAINT,OnPaint)
        
    END_MSG_MAP()


    BOOL SubclassWindow(HWND hWnd)
    {       

        ATLASSERT(::IsWindow(hWnd));


        origcapturehwnd=NULL;
        bcapture=FALSE;
        highlightnItem=-1;
      
  
        return CWindowImpl<CTabCtrlXbuttonImpl,CTabCtrl>::SubclassWindow(hWnd);

    }




    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled=FALSE;
        RECT PaintRc={0};
        if(GetUpdateRect(&PaintRc) && !::IsRectEmpty(&PaintRc))
        {
            int count=CTabCtrl::GetItemCount();
            for(int i=0;i<count;++i)
            {
                RECT ItemRect;
                if(CTabCtrl::GetItemRect(i,&ItemRect))
                {
                    
                    RECT lntersectRect;

                    if(::IntersectRect(&lntersectRect,&ItemRect,&PaintRc) || !::IsRectEmpty(&lntersectRect))
                    {

                        std::vector<CButton>* pbuttons=GetButtons(i);
                        if(pbuttons)
                        {
                           
                            for(std::vector<CButton>::iterator buttoniter=pbuttons->begin();buttoniter!=pbuttons->end();++buttoniter)
                            {
                                (*buttoniter).SetRect(ItemRect);

                                DrawButton(*buttoniter);
                            }
                        }



                    }


                }
            }
        }
        return 0;
    }

    LRESULT OnDeleteAllItems(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled=FALSE;

        for(int i=0;i<GetItemCount();++i)
        {
            TCITEMEXTRA tcix = { 0 };
            tcix.tciheader.mask = TCIF_PARAM;
            GetItem(i,(LPTCITEM)&tcix);
            if(tcix.tvpage.pData)
            {
                std::vector<CButton>*  pbuttons = (std::vector<CButton>*)tcix.tvpage.pData;

                delete pbuttons;
            }
        }

        return 0;

    }
    

    LRESULT OnDeleteItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled=FALSE;

        TCITEMEXTRA tcix = { 0 };
        tcix.tciheader.mask = TCIF_PARAM;
        GetItem(wParam,(LPTCITEM)&tcix);
        if(tcix.tvpage.pData)
        {
            std::vector<CButton>*  pbuttons = (std::vector<CButton>*)tcix.tvpage.pData;

            delete pbuttons;
        }

        return 0;
    }



    LRESULT OnInsertItem(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {   
        bHandled=FALSE;
        


        TCITEMEXTRA* ptcitemextra=(TCITEMEXTRA*)lParam;
        if(ptcitemextra->tciheader.mask&TCIF_PARAM)
        {
            if(ptcitemextra->tvpage.pData)
            {
                if((DWORD)ptcitemextra->tvpage.pData&TAB_VERIFY)
                {
                    std::vector<CButton>*  pbuttons=new  std::vector<CButton>;


                    int e=0;
                    if((DWORD)ptcitemextra->tvpage.pData&TAB_CLOSEBUTTON)
                    {
                        pbuttons->push_back(CloseButton(e));
                        ++e;
                    }

                    if((DWORD)ptcitemextra->tvpage.pData&TAB_FLOATBUTTON)
                    {
                        pbuttons->push_back(FloatButton(e));
                        ++e;
                    }

         
                    if((DWORD)ptcitemextra->tvpage.pData&TAB_CONFIGBUTTON)
                    {
                        pbuttons->push_back(ConfigButton(e));
                        ++e;
                    }




                    ptcitemextra->tvpage.pData=pbuttons;
                }
                else
                {
                    
                    for(int i=0;i<GetItemCount();++i)
                    {
                        TCITEMEXTRA tcix = { 0 };
                        tcix.tciheader.mask = TCIF_PARAM;
                        GetItem(i,(LPTCITEM)&tcix);
                        if(tcix.tvpage.pData==ptcitemextra->tvpage.pData)
                        {

                            tcix.tvpage.pData=NULL;
                            SetItem(i,(LPTCITEM)&tcix);
                            break;
                        }
                    }

                }
            }
        }


        return 0;
    }
  
    LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        bHandled = FALSE;
     
        if(wParam&MK_LBUTTON)
        {
            return 0;
        }

        
    
        


        int xPos = GET_X_LPARAM(lParam); 
        int yPos = GET_Y_LPARAM(lParam); 


     

        CButton* pbutton=NULL;
        int index=GetPtInButton(xPos,yPos,&pbutton);
       
        //处理鼠标 Capture
        if(index!=-1)
        {
            if(!bcapture)
            {
                origcapturehwnd=SetCapture();
                bcapture=TRUE;
            }
        }
        else
        {
            if(bcapture)
            {
                ReleaseCapture();

                bcapture=FALSE;

                if(origcapturehwnd)
                {
                    ::SetCapture(origcapturehwnd);
                }
            }
        }


        

        //处理高亮显示
        if(index!=-1)
        {
            if(!pbutton->highlight)
            {
                DrawButton(*pbutton);
              

                pbutton->highlight=TRUE;
              

                CToolTipCtrl toolctrl=GetTooltips();
                toolctrl.Activate(FALSE);
                toolctrl.Activate(TRUE);     
            }

        }
  




        
           



        //清除高亮显示

        std::vector<CButton>* pbuttons=NULL;
        if(highlightnItem!=-1 && (pbuttons=GetButtons(highlightnItem)))
        {

            for(std::vector<CButton>::iterator buttoniter=pbuttons->begin();buttoniter!=pbuttons->end();++buttoniter)
            {
                CButton& button=*buttoniter;



                if(&button==pbutton)
                {
                    continue;
                }

                if(button.highlight)
                {
                    button.highlight=FALSE;
                    DrawButton(button);
                   

                }

            }

        }



        


         //鼠标在button外恢复 工具提示
        if(index==-1 && highlightnItem!=-1)
        {

            CToolTipCtrl toolctrl=GetTooltips();
            toolctrl.Activate(FALSE);
            toolctrl.Activate(TRUE);

          
        }
 
        highlightnItem=index;

        

        
        return 0;
    }

    LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {





        bHandled = FALSE;

        int xPos = GET_X_LPARAM(lParam); 
        int yPos = GET_Y_LPARAM(lParam); 
  
        CButton* pbutton=NULL;
        int index=GetPtInButton(xPos,yPos,&pbutton);

        if(index!=-1)
        {
            if(pbutton->down)
            {
                pbutton->down=FALSE;

                SendMessage(GetParent(),WM_COMMAND,MAKEWPARAM(pbutton->cmdid/*ID_WINDOW_CLOSE*/,0),(LPARAM)m_hWnd);

                return 0;
            }
            else
            {
                if(!pbutton->highlight)
                {
                    DrawButton(*pbutton);


                    pbutton->highlight=TRUE;
 
                }
            }
           
        }

       

 
        std::vector<CButton>* pbuttons=NULL;
        if(highlightnItem!=-1 && (pbuttons=GetButtons(highlightnItem)))
        {

            for(std::vector<CButton>::iterator buttoniter=pbuttons->begin();buttoniter!=pbuttons->end();++buttoniter)
            {
                CButton& button=*buttoniter;



                if(&button==pbutton)
                {
                    continue;
                }



                if(button.highlight || button.down)
                {
                    button.highlight=FALSE;
                    button.down=FALSE;
                    DrawButton(button);
                }


            }

        }

        



        highlightnItem=index;

        return 0;
    }

    
    LRESULT OnLButtonDwon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {

       

        bHandled = FALSE;

        int xPos = GET_X_LPARAM(lParam); 
        int yPos = GET_Y_LPARAM(lParam); 

      
        CButton* pbutton=NULL;
        int index=GetPtInButton(xPos,yPos,&pbutton);
        if(index!=-1)
        {
           pbutton->down=TRUE;
        }

        return 0;
    }



    LRESULT OnTabGetDispInfo(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
    {
        LPNMTTDISPINFO pTTDI = (LPNMTTDISPINFO)pnmh;
        if(pTTDI->hdr.hwndFrom == GetTooltips())
        {
            POINT ps={0};
            GetCursorPos(&ps);
            ScreenToClient(&ps);

            CButton* pbutton=NULL;
            if(GetPtInButton(ps.x,ps.y,&pbutton)==pTTDI->hdr.idFrom)
            {
                pTTDI->lpszText = pbutton->tip;
            }
            else
            {
                TCHAR buffer[256] = {0};
                TCITEM TabCtrlItem={0};
                TabCtrlItem.mask=TCIF_TEXT;
                TabCtrlItem.pszText = buffer;
                TabCtrlItem.cchTextMax = 256;



                if(GetItem(pTTDI->hdr.idFrom,&TabCtrlItem))
                {
                    int len=_tcslen(buffer);
                    for(int i=len-1;i>0;--i)
                    {
                        if(buffer[i]!=_T(' '))
                            _tcsncpy(pTTDI->szText,buffer,sizeof(pTTDI->szText)/sizeof(TCHAR)-1);
                        else
                            buffer[i]=0;
                       
                    }

                }

               
            }

        }
        else
        {
            bHandled = FALSE;
        }

        return 0;
    }


private:

    std::vector<CButton>* GetButtons(int nItem)
    {
        TCITEMEXTRA tcix = { 0 };
        tcix.tciheader.mask = TCIF_PARAM;
        GetItem(nItem,(LPTCITEM)&tcix);
        if(tcix.tvpage.pData)
        {
            return  (std::vector<CButton>*)tcix.tvpage.pData;
        }

        return NULL;
    }

    void DrawButton(CButton& button)
    {

        RECT& buttonrect=button.rect;
       

        POINT ps={0};
        GetCursorPos(&ps);
        ScreenToClient(&ps);

        BOOL bhighlight=PtInRect(&buttonrect,ps);

       
        CWindowDC windc(m_hWnd);

        CBrush backbrush;

        COLORREF backcolor=RGB(206,219,241);
        if(bhighlight || button.down)
        {
            backcolor=RGB(172,194,227);
        }


        backbrush.CreateSolidBrush(backcolor);
/*
        HRGN hrgn=::CreateRoundRectRgn(buttonrect.left,buttonrect.top,buttonrect.right,buttonrect.bottom,1,1);
        windc.FillRgn(hrgn,backbrush);
        ValidateRgn(hrgn);
        ::DeleteObject(hrgn);
*/
        windc.FillRect(&buttonrect,backbrush);
        ValidateRect(&buttonrect);


        int oldbkmode=windc.GetBkMode();
        windc.SetBkMode(TRANSPARENT);

        int ppi=windc.GetDeviceCaps(LOGPIXELSX);
        int pointsize= MulDiv(75, 96, ppi);
        CFont font;
        font.CreatePointFont(pointsize,_T("Marlett"));
        HFONT oldfont=windc.SelectFont(font);
        windc.DrawText(&button.text,1,(LPRECT)&buttonrect,DT_SINGLELINE|DT_VCENTER|DT_CENTER);;
        windc.SelectFont(oldfont);
        windc.SetBkMode(oldbkmode);



       
       
    }




    int  GetPtInButton(int xPos,int yPos,CButton** lplpbutton)
    {        

        TCHITTESTINFO  tchittestinfo;
        tchittestinfo.pt.x=xPos;
        tchittestinfo.pt.y=yPos;
        tchittestinfo.flags=TCHT_NOWHERE;
        int index=HitTest(&tchittestinfo);
        if(index!=-1)
        {

            if(lplpbutton)
            {
                RECT ItemRect;

                if(CTabCtrl::GetItemRect(index,&ItemRect))
                {


                    std::vector<CButton>* pbuttons=GetButtons(index);
                    if(pbuttons)
                    {
                        
                        for(std::vector<CButton>::iterator buttoniter=pbuttons->begin();buttoniter!=pbuttons->end();++buttoniter)
                        {
                            CButton& button=*buttoniter;
                            button.SetRect(ItemRect);

                            if(PtInRect(&button.rect,tchittestinfo.pt))
                            {
                                *lplpbutton=&button;
                                return index;
                            }
                        }
                    }


    
                }
            }
            else
            {
                return index;
            }


        }


        return -1;
    }





};
