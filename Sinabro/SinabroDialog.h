#pragma once

#include "GridCtrl.h"
#include "SinabroAccountFile.h"

#define USER_ID		3000

#define SNBR_ANALYSIS_VIEW_COLUMN_CNT		13

const CString SNBR_ANALYSIS_VIEW_COLUMN[] = {
	_T("Code"),					// stock code
	_T("Name"),					// stock name
	_T("Opening Price"),		// opening price of the stock
	_T("Current Price"),		// current price of the stock
	_T("Increase Rate"),		// price increase rate
	_T("Volume"),				// trading volume
	_T("Buying Price"),			// buying price
	_T("Selling Price"),		// selling price
	_T("Tax"),					// tax + commission price
	_T("Profit Price"),			// profit price
	_T("Buying Cnt"),			// the number of stock bot buys
	_T("Selling Cnt"),			// the number of stock bot sells
	_T("Status")				// status
};

typedef struct SNBR_ANALYSIS_VIEW_ROW {
	GV_ITEM m_column[SNBR_ANALYSIS_VIEW_COLUMN_CNT];
} SNBR_ANALYSIS_VIEW_ROW;

enum SNBRENUM_ANALYSIS_VIEW_COLUMN {
	ANALVIEW_CODE, ANALVIEW_NAME, ANALVIEW_OPENPRICE, ANALVIEW_CURRENTPRICE, ANALVIEW_INCRATE,
	ANALVIEW_VOLUME, ANALVIEW_BUYPRICE, ANALVIEW_SELLPRICE, ANALVIEW_COMMISSION, ANALVIEW_PROFITPRICE, 
	ANALVIEW_BUYCOUNT, ANALVIEW_SELLCOUNT, ANALVIEW_STATUS
};

#define SNBR_TRADING_VIEW_COLUMN_CNT		10

const CString SNBR_TRADING_VIEW_COLUMN[] = {
	_T("Code"),					// stock code
	_T("Name"),					// stock name
	_T("Buying Price"),			// buying price
	_T("Selling Price"),		// selling price
	_T("Tax"),					// commission price
	_T("Earning Rate"),			// earning rate compared with previous estimated assets
	_T("Profit Price"),			// profit price
	_T("Buying Time"),			// buying time
	_T("Selling Time"),			// selling time
	_T("Selling Cnt"),			// the number of stock you sells
};

typedef struct SNBR_TRADING_VIEW_ROW {
	GV_ITEM m_column[SNBR_TRADING_VIEW_COLUMN_CNT];
} SNBR_TRADING_VIEW_ROW;

enum SNBRENUM_TRADING_VIEW_COLUMN {
	TRADVIEW_CODE, TRADVIEW_NAME, TRADVIEW_BUYPRICE, TRADVIEW_SELLPRICE, TRADVIEW_COMMISSION,
	TRADVIEW_EARNRATE, TRADVIEW_PROFITPRICE, TRADVIEW_BUYTIME, TRADVIEW_SELLTIME, TRADVIEW_SELLCOUNT
};

#define SNBR_DAYPROFIT_VIEW_COLUMN_CNT		6

const CString SNBR_DAYPROFIT_VIEW_COLUMN[] = {
	_T("Date"),
	_T("Opening Asset"),
	_T("Ending Asset"),
	_T("Day Earning"),
	_T("Earning Accumulated"),
	_T("Increase Rate")
};

typedef struct SNBR_DAYPROFIT_VIEW_ROW {
	GV_ITEM m_column[SNBR_TRADING_VIEW_COLUMN_CNT];
} SNBR_DAYPROFIT_VIEW_ROW;

enum SNBRENUM_DAYPROFIT_VIEW_COLUMN {
	DAYPROFITVIEW_DATE, DAYPROFITVIEW_OPENASSET, DAYPROFITVIEW_ENDASSET, DAYPROFITVIEW_DAYEARN, 
	DAYPROFITVIEW_ACCEARN, DAYPROFITVIEW_INCRATE
};

class CSinabroDialog : public CDialogEx
{
protected :
	CGridCtrl	m_gridContents;
	CStatusBar	m_statusBar;
	INT			m_accountIndex;
	INT			m_viewIndex;

#define SNBR_VIEW_INDEX_ANALYSIS		0
#define SNBR_VIEW_INDEX_TRADING			1
#define SNBR_VIEW_INDEX_DAYPROFIT		2

public:
	CSinabroDialog(CWnd* pParent = NULL);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SINABRO_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;
	SinabroAccountFile m_accountFile;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:

	CString GetFilePathByAccount(CString sAccount);
	BOOL ShowDepositOnStatusBar();
	BOOL ShowGrid();
	BOOL ResizeGridColumn(CRect*);
	BOOL ShowAnalyzeGrid();
	BOOL ShowTradingGrid();
	BOOL ShowDayProfitGrid();
	BOOL ShowRow(INT nIndex);
	BOOL ChangeRow(INT, INT);

	BOOL LoadAccountData(void);
	BOOL UpdateAccountData(void);
	BOOL SaveAccountData(void);

	static CString GetNumberWithComma(CString);
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnMenuConnect();
	afx_msg void OnMenuExit();
	afx_msg void OnAccountSelectMenu(UINT);
	DECLARE_EVENTSINK_MAP()
	void OnEventConnect(long nErrCode);
	afx_msg void OnClose();
	void OnReceiveTrDataKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg);
	afx_msg void OnMenuAboutus();

	void OnMenuView(INT nIndex);
	afx_msg void OnMenuViewAnalysis();
	afx_msg void OnMenuViewTrading();
	
	afx_msg void OnMenuStartAnalysis();
	afx_msg void OnControlTrading();
	afx_msg void OnMenuAccountInfo();
	void OnReceiveChejanData(LPCTSTR sGubun, long nItemCnt, LPCTSTR sFIdList);
	void OnReceiveMsgKH(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg);
	afx_msg void OnViewDayProfit();
};
