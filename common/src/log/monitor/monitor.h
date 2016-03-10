// monitor.h




int  Log_StartUI(int nCmdShow);
int  Log_StopUI();
int  Log_PrintLog(const char* channel,char* pstrstream,LoggingSeverity__ sev);
int  Log_ShowLog(const char* channel,BOOL bShow);