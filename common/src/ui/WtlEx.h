

class WtlEx
{
public:
    static UINT GetDlgItemText(HWND hWnd,int nID, CString& strText)
    {
        HWND hItem;

        ATLASSERT(::IsWindow(hWnd));
        hItem = ::GetDlgItem(hWnd,nID);
        if (hItem != NULL)
        {
            int nLength;
            LPTSTR pszText;

            nLength = ::GetWindowTextLength(hItem);
            pszText = strText.GetBuffer(nLength+1);
            nLength = ::GetWindowText(hItem, pszText, nLength+1);
            strText.ReleaseBuffer(nLength);

            return nLength;
        }
        else
        {
            strText.Empty();

            return 0;
        }
    }
};
