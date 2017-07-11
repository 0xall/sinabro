
// SinabroDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SinabroApp.h"
#include "SinabroDialog.h"
#include "AccountInfoDialog.h"
#include "afxdialogex.h"
#include "afxwin.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_editAbout;
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	HICON hIcon;
	hIcon = ::ExtractIcon(NULL, _T("graph.ico"), 0);

	::SendDlgItemMessage(this->GetSafeHwnd(), IDC_ABOUTICON, STM_SETICON, reinterpret_cast<WPARAM>(hIcon), 0);

	m_editAbout.SetWindowTextW(_T(
		" Sinabro is a bot program that analyzes stocks and trade them. "
		"Sinabro means \"little by little\" in Korean. It has a meaning that "
		"I hope you earn money little by little by this program. "
		"It uses Kiwoom Open API for getting stock information and trading. "
		"(you need a Kiwoom security account for using this program) "
		"It only trades stocks of KRX(KOSPI, KOSDAQ).\r\n\r\n"
		"Ver 1.00 [2016.07.31]\r\n"
		"Initial Release."));

	return TRUE;
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ABOUTUS, m_editAbout);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSinabroDialog dialog
static UINT BASED_CODE indicators[] = {
	ID_INDICATOR_DEPOSIT,
	ID_INDICATOR_LOG
};


CSinabroDialog::CSinabroDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SINABRO_DIALOG, pParent), m_accountIndex(0), m_viewIndex(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_ICON_GRAPH);
}

void CSinabroDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_KHOPENAPICTRL, sinabro.GetControl());
	DDX_GridControl(pDX, IDC_GRID_CONTENTS, m_gridContents);
}

BEGIN_MESSAGE_MAP(CSinabroDialog, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_CONNECT_CONNECT, &CSinabroDialog::OnMenuConnect)
	ON_COMMAND(ID_DISCONNECT_EXIT, &CSinabroDialog::OnMenuExit)
	ON_COMMAND_RANGE(USER_ID, USER_ID+20, &CSinabroDialog::OnAccountSelectMenu)
	ON_WM_CLOSE()
	ON_COMMAND(ID_HELP_ABOUTUS, &CSinabroDialog::OnMenuAboutus)
	ON_COMMAND(ID_VIEW_ANALYSIS, &CSinabroDialog::OnMenuViewAnalysis)
	ON_COMMAND(ID_VIEW_TRADING, &CSinabroDialog::OnMenuViewTrading)
	ON_COMMAND(ID_CONTROL_ANALYSIS, &CSinabroDialog::OnMenuStartAnalysis)
	ON_COMMAND(ID_CONTROL_TRADING, &CSinabroDialog::OnControlTrading)
	ON_COMMAND(ID_ACCOUNT_INFORMATION, &CSinabroDialog::OnMenuAccountInfo)
	ON_COMMAND(ID_VIEW_DAYPROFIT, &CSinabroDialog::OnViewDayProfit)
END_MESSAGE_MAP()


// CSinabroDialog message handlers

BOOL CSinabroDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	ModifyStyle(NULL, WS_THICKFRAME);
	ShowWindow(SW_SHOWMAXIMIZED);

	// add status bar to the dialog
	m_statusBar.Create(this);
	m_statusBar.SetIndicators(indicators, 2);
	
	CRect clientRect;
	GetClientRect(&clientRect);
	m_statusBar.SetPaneInfo(0, ID_INDICATOR_DEPOSIT, SBPS_NORMAL, 300);
	m_statusBar.SetPaneInfo(1, ID_INDICATOR_LOG, SBPS_STRETCH, 0);
	m_statusBar.SetPaneText(0, _T("Asset : 0(0)(0%)"));

	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_INDICATOR_LOG);
	m_statusBar.GetStatusBarCtrl().SetBkColor(RGB(18, 18, 18));

	if (m_gridContents.GetSafeHwnd())
	{
		CRect clientRect, statusRect;
		GetClientRect(&clientRect);

		if (m_statusBar.GetSafeHwnd())
		{
			m_statusBar.GetItemRect(0, &statusRect);
			clientRect.bottom -= statusRect.Height();
		}

		m_gridContents.MoveWindow(&clientRect);
		ResizeGridColumn(&clientRect);
	}

	m_accountIndex = -1;
	m_viewIndex = -1;
	
	return sinabro.Initialize();
}

void CSinabroDialog::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSinabroDialog::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSinabroDialog::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CSinabroDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) {
		case VK_ESCAPE :
		case VK_RETURN :
			return TRUE;	// do not exit when input key enter or escape
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CSinabroDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// resize stock grid control
	if (m_gridContents.GetSafeHwnd())
	{
		CRect clientRect, statusRect;
		GetClientRect(&clientRect);

		if (m_statusBar.GetSafeHwnd())
		{	
			m_statusBar.SetPaneInfo(0, ID_INDICATOR_DEPOSIT, SBPS_NORMAL, 300);
			m_statusBar.SetPaneInfo(1, ID_INDICATOR_LOG, SBPS_STRETCH, 0);
			RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, ID_INDICATOR_LOG);

			m_statusBar.GetWindowRect(&statusRect);
			clientRect.bottom -= statusRect.Height();
		}

		m_gridContents.MoveWindow(&clientRect);
		ResizeGridColumn(&clientRect);
	}
}

BOOL CSinabroDialog::ResizeGridColumn(CRect* clientRect)
{
	INT columnCnt = m_gridContents.GetColumnCount();
	INT columnSize;

	if (columnCnt != 0 && clientRect->Width() > 92 * columnCnt)
	{
		columnSize = (clientRect->Width() - 10) / columnCnt;
		for (int i = 0; i < columnCnt; ++i)
			m_gridContents.SetColumnWidth(i, columnSize);
	}

	return TRUE;
}


void CSinabroDialog::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 600;
	lpMMI->ptMinTrackSize.y = 400;

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}


void CSinabroDialog::OnMenuConnect()
{
	if (sinabro.GetConnectionStatus())
	{
		::AfxMessageBox(_T("Already connected."));
		return;
	}

	sinabro.Connect();
}


void CSinabroDialog::OnMenuExit()
{
	SendMessage(WM_CLOSE);
}

BEGIN_EVENTSINK_MAP(CSinabroDialog, CDialogEx)
	ON_EVENT(CSinabroDialog, IDC_KHOPENAPICTRL, 5, CSinabroDialog::OnEventConnect, VTS_I4)
	ON_EVENT(CSinabroDialog, IDC_KHOPENAPICTRL, 1, CSinabroDialog::OnReceiveTrDataKhopenapictrl, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(CSinabroDialog, IDC_KHOPENAPICTRL, 4, CSinabroDialog::OnReceiveChejanData, VTS_BSTR VTS_I4 VTS_BSTR)
	ON_EVENT(CSinabroDialog, IDC_KHOPENAPICTRL, 3, CSinabroDialog::OnReceiveMsgKH, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
END_EVENTSINK_MAP()

void CSinabroDialog::OnEventConnect(long nErrCode)
{
	if (nErrCode)
	{
		return;
	}

	CMenu* menu = GetMenu();
	CMenu* pPopup = menu->GetSubMenu(0);
	INT i;
	pPopup->CheckMenuItem(ID_CONNECT_CONNECT, MF_CHECKED);

	pPopup = menu->GetSubMenu(1)->GetSubMenu(0);
	sinabro.ProcessEventConnect(nErrCode);

	for (i = 0; i < sinabro.GetAccountsCount(); ++i)
	{
		INT nFlag = MF_BYPOSITION | MF_POPUP | MF_UNCHECKED;
		pPopup->InsertMenuW(i, nFlag, USER_ID + i, sinabro.GetAccount(i));
	}

	pPopup->DeleteMenu(i, MF_BYPOSITION);

	m_statusBar.SetPaneText(1, _T("Login Success!"));
	OnMenuViewAnalysis();
	ShowGrid();
}

CString CSinabroDialog::GetFilePathByAccount(CString sAccount)
{
	return (_T("data\\DAT") + sAccount + _T(".") + SNBR_ACCDAT_FILE_EXT);
}

BOOL CSinabroDialog::ShowDepositOnStatusBar()
{
	if (!m_statusBar.GetSafeHwnd()) return FALSE;

	INT64 asset, beginningAsset;
	FLOAT rate;
	CString sPaneTxt, sAsset, sBeginningAsset;

	asset = sinabro.GetAsset();
	beginningAsset = sinabro.GetBeginningAsset();
	sAsset.Format(_T("%lld"), asset);
	sBeginningAsset.Format(_T("%lld"), beginningAsset);
	rate = ((FLOAT)asset / beginningAsset - 1.0f) * 100.0f;

	sPaneTxt.Format(_T("Asset : %s(%s)(%.02f%%)"), GetNumberWithComma(sAsset),
		GetNumberWithComma(sBeginningAsset), rate);
	m_statusBar.SetPaneText(0, sPaneTxt);

	return TRUE;
}

BOOL CSinabroDialog::ShowGrid()
{
	m_gridContents.SetEditable(FALSE);
	m_gridContents.SetListMode();

	switch (m_viewIndex)
	{
	case SNBR_VIEW_INDEX_ANALYSIS :
		m_gridContents.SetColumnCount(SNBR_ANALYSIS_VIEW_COLUMN_CNT);
		m_gridContents.SetRowCount(SNBR_MAX_ANALYSIS_STOCK + 1);
		m_gridContents.SetFixedRowCount();

		for (int i = 0; i < SNBR_ANALYSIS_VIEW_COLUMN_CNT; ++i)
		{
			GV_ITEM item;
			item.col = i; item.row = 0;
			item.mask = GVIF_FORMAT | GVIF_TEXT;
			item.nFormat = DT_CENTER | DT_VCENTER;
			item.strText = SNBR_ANALYSIS_VIEW_COLUMN[i];
			m_gridContents.SetItem(&item);
		}

		ShowAnalyzeGrid();
		break;

	case SNBR_VIEW_INDEX_TRADING :
		m_gridContents.SetColumnCount(SNBR_TRADING_VIEW_COLUMN_CNT);
		m_gridContents.SetRowCount(1);
		m_gridContents.SetFixedRowCount();

		for (int i = 0; i < SNBR_TRADING_VIEW_COLUMN_CNT; ++i)
		{
			GV_ITEM item;
			item.col = i; item.row = 0;
			item.mask = GVIF_FORMAT | GVIF_TEXT;
			item.nFormat = DT_CENTER | DT_VCENTER;
			item.strText = SNBR_TRADING_VIEW_COLUMN[i];
			m_gridContents.SetItem(&item);
		}
		
		ShowTradingGrid();
		break;

	case SNBR_VIEW_INDEX_DAYPROFIT :
		m_gridContents.SetColumnCount(SNBR_DAYPROFIT_VIEW_COLUMN_CNT);
		m_gridContents.SetRowCount(1);
		m_gridContents.SetFixedRowCount();

		for (int i = 0; i < SNBR_DAYPROFIT_VIEW_COLUMN_CNT; ++i)
		{
			GV_ITEM item;
			item.col = i; item.row = 0;
			item.mask = GVIF_FORMAT | GVIF_TEXT;
			item.nFormat = DT_CENTER | DT_VCENTER;
			item.strText = SNBR_DAYPROFIT_VIEW_COLUMN[i];
			m_gridContents.SetItem(&item);
		}

		ShowDayProfitGrid();
	}

	CRect clientRect;
	GetClientRect(&clientRect);
	ResizeGridColumn(&clientRect);
	m_gridContents.RedrawWindow();
	return TRUE;
}

BOOL CSinabroDialog::ShowAnalyzeGrid()
{
	INT stockCnt = sinabro.GetStockCount(), i;
	m_gridContents.SetRowCount(stockCnt + 1);
	for (i = 0; i < stockCnt; ++i)
	{
		ShowRow(i);
	}

	return TRUE;
}

BOOL CSinabroDialog::ShowTradingGrid()
{
	INT stockCnt = sinabro.GetTradingStockCount(), i;
	m_gridContents.SetRowCount(stockCnt + 1);
	for (i = 0; i < stockCnt; ++i)
	{
		ShowRow(i);
	}

	return TRUE;
}

BOOL CSinabroDialog::ShowDayProfitGrid()
{
	INT stockCnt = m_accountFile.GetDataCount(), i;
	m_gridContents.SetRowCount(stockCnt + 1);
	for (i = 0; i < stockCnt; ++i)
	{
		ShowRow(i);
	}

	return TRUE;
}

CString CSinabroDialog::GetNumberWithComma(CString number)
{
	INT nIndex = number.GetLength() - 1;
	INT count = 0;
	for (; nIndex > 0; --nIndex)
	{
		count++;
		if (count % 3 == 0)
		{
			if (nIndex == 1 && number[0] == '-') continue;
			number = number.Mid(0, nIndex) + _T(",") + number.Right(number.GetLength() - nIndex);
		}
	}

	return number;
}

BOOL CSinabroDialog::ShowRow(INT nIndex)
{
	//if (nIndex < 0 || nIndex >= sinabro.GetStockCount()) return FALSE;
	INT j;
	CString input;

	if (m_viewIndex == SNBR_VIEW_INDEX_ANALYSIS)
	{
		SinabroStock* rowStock = sinabro.GetStock(nIndex);
		SNBR_ANALYSIS_VIEW_ROW rowCell;
		FLOAT increaseRate = rowStock->GetIncreaseRate();
		INT64 profit = rowStock->GetProfitPrice();
		INT boughtVolume, keepingVolume;
		CString sBoughtVolume, sKeepingVolume;
		LOGFONT* pFont = NULL;

		for (j = 0; j < SNBR_ANALYSIS_VIEW_COLUMN_CNT; ++j)
		{
			rowCell.m_column[j].mask = GVIF_TEXT | GVIF_FORMAT;
			rowCell.m_column[j].nFormat = DT_CENTER | DT_VCENTER;
			rowCell.m_column[j].row = nIndex + 1;
			rowCell.m_column[j].col = j;
		}

		rowCell.m_column[ANALVIEW_CODE].strText = rowStock->GetStockCode();
		rowCell.m_column[ANALVIEW_NAME].strText = rowStock->GetStockName();

		input.Format(_T("%lld"), rowStock->GetOpeningPrice());
		rowCell.m_column[ANALVIEW_OPENPRICE].strText = GetNumberWithComma(input);

		input.Format(_T("%lld"), rowStock->GetCurrentPrice());
		rowCell.m_column[ANALVIEW_CURRENTPRICE].strText = GetNumberWithComma(input);

		input.Format(_T("%.02f%%"), increaseRate);
		if (increaseRate > 0.0f) input = _T("+") + input;
		rowCell.m_column[ANALVIEW_INCRATE].strText = input;

		input.Format(_T("%lld"), rowStock->GetCurrentVolume());
		if(input != _T("-1")) rowCell.m_column[ANALVIEW_VOLUME].strText = GetNumberWithComma(input);

		input.Format(_T("%lld"), rowStock->GetBoughtPrice());
		if (input != _T("-1")) rowCell.m_column[ANALVIEW_BUYPRICE].strText = GetNumberWithComma(input);

		input.Format(_T("%lld"), rowStock->GetSoldPrice());
		if (input != _T("-1")) rowCell.m_column[ANALVIEW_SELLPRICE].strText = GetNumberWithComma(input);

		input.Format(_T("%lld"), rowStock->GetCommission());
		if (input != _T("-1")) rowCell.m_column[ANALVIEW_COMMISSION].strText = GetNumberWithComma(input);

		input.Format(_T("%lld"), profit);
		if (profit != 0x80000000) rowCell.m_column[ANALVIEW_PROFITPRICE].strText = GetNumberWithComma(input);

		boughtVolume = rowStock->GetBoughtVolume();
		keepingVolume = rowStock->GetKeepingVolume();
		if (keepingVolume > 0)
		{
			sBoughtVolume.Format(_T("%d"), boughtVolume); sBoughtVolume = GetNumberWithComma(sBoughtVolume);
			sKeepingVolume.Format(_T("%d"), keepingVolume); sKeepingVolume = GetNumberWithComma(sKeepingVolume);
			input.Format(_T("%s / %s"), sKeepingVolume, sBoughtVolume);
			rowCell.m_column[ANALVIEW_BUYCOUNT].strText = input;
		}

		input.Format(_T("%d"), rowStock->GetSoldVolume());
		if (input != _T("-1")) rowCell.m_column[ANALVIEW_SELLCOUNT].strText = GetNumberWithComma(input);

		INT status = rowStock->GetStatus();
		rowCell.m_column[ANALVIEW_STATUS].strText = SNBR_STOCK_STATUS_STR[rowStock->GetStatus()];

		for (j = 0; j < SNBR_ANALYSIS_VIEW_COLUMN_CNT; ++j)
			m_gridContents.SetItem(&rowCell.m_column[j]);

		COLORREF bgColor = RGB(255, 255, 255), fgColor = RGB(0, 0, 0);
		BOOL bBold = FALSE;
		switch (rowStock->GetStatus())
		{
		case SNBR_STOCK_FAIL_TO_BUY:
			bgColor = RGB(255, 189, 189);
			fgColor = RGB(255, 40, 40);
			bBold = TRUE;
			break;

		case SNBR_STOCK_UNPROFITABLE : 
			bgColor = RGB(255, 189, 189);
			fgColor = RGB(255, 40, 40);
			bBold = FALSE;
			break;

		case SNBR_STOCK_NOT_ANALYZED :
			bgColor = RGB(255, 255, 255);
			fgColor = RGB(0, 0, 0);
			bBold = FALSE;
			break;

		case SNBR_STOCK_PROFITABLE :
			bgColor = RGB(184, 200, 222);
			fgColor = RGB(0, 0, 0);
			bBold = TRUE;
			break;

		case SNBR_STOCK_ALL_BOUGHT :
		case SNBR_STOCK_BOUGHT:
			bgColor = RGB(81, 203, 255); 
			fgColor = RGB(40, 40, 255);
			bBold = TRUE;
			break;

		case SNBR_STOCK_SOLD :
			bgColor = RGB(81, 203, 255);
			fgColor = RGB(163, 73, 164);
			bBold = TRUE;
			break;

		case SNBR_STOCK_ALL_SOLD:
			bgColor = RGB(81, 203, 255);
			fgColor = RGB(63, 72, 204);
			bBold = TRUE;
		}

		for (j = 0; j < SNBR_ANALYSIS_VIEW_COLUMN_CNT; ++j)
		{
			m_gridContents.SetItemBkColour(nIndex + 1, j, bgColor);
			m_gridContents.SetItemFgColour(nIndex + 1, j, RGB(0, 0, 0));
		}

		m_gridContents.SetItemFgColour(nIndex + 1, ANALVIEW_STATUS, fgColor);

		if (increaseRate > 0.0f) fgColor = RGB(255, 0, 0);
		else if (increaseRate < 0.0f) fgColor = RGB(0, 0, 255);
		else fgColor = RGB(0, 0, 0);

		pFont = (LOGFONT*)m_gridContents.GetItemFont(nIndex + 1, ANALVIEW_STATUS);

		if (bBold) pFont->lfWeight |= FW_BOLD;
		else pFont->lfWeight &= ~FW_BOLD;

		m_gridContents.SetItemFont(nIndex + 1, ANALVIEW_STATUS, pFont);
		m_gridContents.SetItemFgColour(nIndex + 1, ANALVIEW_INCRATE, fgColor);

		m_gridContents.RedrawRow(nIndex + 1);
	}

	else if (m_viewIndex == SNBR_VIEW_INDEX_TRADING)
	{
		SinabroStock* rowStock = sinabro.GetTradingStock(nIndex);
		FLOAT increaseRate = rowStock->GetProfitRate();
		INT64 profit = rowStock->GetProfitPrice();
		SNBR_TRADING_VIEW_ROW rowCell;
		LOGFONT* pFont = NULL;

		for (j = 0; j < SNBR_TRADING_VIEW_COLUMN_CNT; ++j)
		{
			rowCell.m_column[j].mask = GVIF_TEXT | GVIF_FORMAT;
			rowCell.m_column[j].nFormat = DT_CENTER | DT_VCENTER;
			rowCell.m_column[j].row = nIndex + 1;
			rowCell.m_column[j].col = j;
		}

		rowCell.m_column[TRADVIEW_CODE].strText = rowStock->GetStockCode();
		rowCell.m_column[TRADVIEW_NAME].strText = rowStock->GetStockName();

		input.Format(_T("%lld"), rowStock->GetBoughtPrice());
		rowCell.m_column[TRADVIEW_BUYPRICE].strText = GetNumberWithComma(input);

		input.Format(_T("%lld"), rowStock->GetSoldPrice());
		rowCell.m_column[TRADVIEW_SELLPRICE].strText = GetNumberWithComma(input);

		input.Format(_T("%lld"), rowStock->GetCommission());
		rowCell.m_column[TRADVIEW_COMMISSION].strText = GetNumberWithComma(input);

		input.Format(_T("%.02f"), increaseRate);
		if (increaseRate > 0.0f) input = _T("+") + input;
		rowCell.m_column[TRADVIEW_EARNRATE].strText = input;

		input.Format(_T("%lld"), rowStock->GetProfitPrice());
		if (input != _T("-1")) rowCell.m_column[TRADVIEW_PROFITPRICE].strText = GetNumberWithComma(input);

		CTime boughtTime = rowStock->GetBoughtTime();
		input.Format(_T("%02d:%02d:%02d"), boughtTime.GetHour(), boughtTime.GetMinute(), boughtTime.GetSecond());
		if (input != _T("-1")) rowCell.m_column[TRADVIEW_BUYTIME].strText = input;

		CTime soldTime = rowStock->GetSoldTime();
		input.Format(_T("%02d:%02d:%02d"), soldTime.GetHour(), soldTime.GetMinute(), soldTime.GetSecond());
		if (input != _T("-1")) rowCell.m_column[TRADVIEW_SELLTIME].strText = input;

		input.Format(_T("%d"), rowStock->GetSoldVolume());
		if (input != _T("-1")) rowCell.m_column[TRADVIEW_SELLCOUNT].strText = GetNumberWithComma(input);

		for (j = 0; j < SNBR_TRADING_VIEW_COLUMN_CNT; ++j)
			m_gridContents.SetItem(&rowCell.m_column[j]);

		COLORREF bgColor = RGB(255, 255, 255), fgColor = RGB(0, 0, 0);

		if (increaseRate > 0.0f)
		{
			fgColor = RGB(255, 0, 0);
			bgColor = RGB(184, 200, 222);
		}

		else if (increaseRate < 0.0f)
		{
			fgColor = RGB(0, 0, 255);
			bgColor = RGB(255, 189, 189);
		}

		else
		{
			fgColor = RGB(0, 0, 0);
			bgColor = RGB(255, 255, 255);
		}

		for (j = 0; j < SNBR_TRADING_VIEW_COLUMN_CNT; ++j)
		{
			m_gridContents.SetItemBkColour(nIndex + 1, j, bgColor);
			m_gridContents.SetItemFgColour(nIndex + 1, j, RGB(0, 0, 0));
		}

		m_gridContents.SetItemFgColour(nIndex + 1, TRADVIEW_EARNRATE, fgColor);
		m_gridContents.RedrawRow(nIndex + 1);
	}

	else if (m_viewIndex == SNBR_VIEW_INDEX_DAYPROFIT)
	{
		SNBR_DAYPROFIT_VIEW_ROW rowCell;
		SNBR_ACCDAT_FILE_DATA rawData;

		for (j = 0; j < SNBR_DAYPROFIT_VIEW_COLUMN_CNT; ++j)
		{
			rowCell.m_column[j].mask = GVIF_TEXT | GVIF_FORMAT;
			rowCell.m_column[j].nFormat = DT_CENTER | DT_VCENTER;
			rowCell.m_column[j].row = nIndex + 1;
			rowCell.m_column[j].col = j;
		}

		rawData = m_accountFile.GetData((SIZE_T)nIndex);

		CString sOutput = rawData.m_date;
		FLOAT increaseRate;
		COLORREF bkColor, fgColor;
		sOutput = sOutput.Mid(0, 4) + _T(".") + sOutput.Mid(4, 2) + _T(".") + sOutput.Mid(6, 2);
		
		rowCell.m_column[DAYPROFITVIEW_DATE].strText = sOutput;
		
		sOutput.Format(_T("%lld"), rawData.m_openingAsset); sOutput = GetNumberWithComma(sOutput);
		rowCell.m_column[DAYPROFITVIEW_OPENASSET].strText = sOutput;
		
		sOutput.Format(_T("%lld"), rawData.m_endingAsset); sOutput = GetNumberWithComma(sOutput);
		rowCell.m_column[DAYPROFITVIEW_ENDASSET].strText = sOutput;

		sOutput.Format(_T("%lld"), rawData.m_endingAsset - rawData.m_openingAsset); 
		sOutput = GetNumberWithComma(sOutput);
		rowCell.m_column[DAYPROFITVIEW_DAYEARN].strText = sOutput;

		increaseRate = ((FLOAT)rawData.m_endingAsset / rawData.m_openingAsset - 1.0f) * 100.0f;
		sOutput.Format(_T("%.02f"), increaseRate);
		if (increaseRate > 0.0f)
		{
			sOutput = _T("+") + sOutput;
			fgColor = RGB(255, 0, 0);
		}

		else if (increaseRate < 0.0f)
		{
			fgColor = RGB(0, 0, 255);
		}

		else
		{
			fgColor = RGB(0, 0, 0);
		}

		rowCell.m_column[DAYPROFITVIEW_INCRATE].strText = sOutput;
		

		if (nIndex == 0)
			rowCell.m_column[DAYPROFITVIEW_ACCEARN].strText.Format(_T("%lld"),
				rawData.m_endingAsset - rawData.m_openingAsset);

		else
		{
			INT64 accumulated = _wtoi64(m_gridContents.GetCell(nIndex - 1, DAYPROFITVIEW_ACCEARN)->GetText());
			rowCell.m_column[DAYPROFITVIEW_ACCEARN].strText.Format(_T("%lld"),
				accumulated + rawData.m_endingAsset - rawData.m_openingAsset);
		}

		for (j = 0; j < SNBR_DAYPROFIT_VIEW_COLUMN_CNT; ++j)
			m_gridContents.SetItem(&rowCell.m_column[j]);

		if (increaseRate > 0.0f) bkColor = RGB(184, 200, 222);
		else if (increaseRate < 0.0f) bkColor = RGB(255, 40, 40);
		else bkColor = RGB(255, 255, 255);

		for (j = 0; j < SNBR_DAYPROFIT_VIEW_COLUMN_CNT; ++j)
		{
			m_gridContents.SetItemBkColour(nIndex + 1, j, bkColor);
			m_gridContents.SetItemFgColour(nIndex + 1, j, RGB(0, 0, 0));
		}

		m_gridContents.SetItemFgColour(nIndex + 1, DAYPROFITVIEW_INCRATE, fgColor);

		m_gridContents.RedrawRow(nIndex + 1);
	}

	return TRUE;
}

BOOL CSinabroDialog::ChangeRow(INT from, INT to)
{
	m_gridContents.InsertRow(_T(""), to);
	if (to < from) from++;

	INT rowCnt = m_gridContents.GetRowCount();

	for (int i = 0; i < rowCnt; ++i) {
		GV_ITEM item;
		item.row = to; item.col = i;
		item.nFormat = m_gridContents.GetItemFormat(from, i);
		item.mask = GVIF_TEXT | GVIF_FORMAT;
		item.strText = m_gridContents.GetItemText(from, i);
		m_gridContents.SetItem(&item);
		m_gridContents.SetItemBkColour(to, i, m_gridContents.GetItemBkColour(from, i));
		m_gridContents.SetItemFgColour(to, i, m_gridContents.GetItemFgColour(from, i));
	}

	m_gridContents.DeleteRow(from);
	if (to > from) to--;
	for (int i = ((to > from) ? from : to); i < ((to > from) ? to : from); ++i) {
		m_gridContents.RedrawRow(i);
	}

	return TRUE;
}

void CSinabroDialog::OnClose()
{
	if (sinabro.GetCurrentAccount() != _T("")) SaveAccountData();

	sinabro.Release();
	Sleep(100);
	CDialogEx::OnClose();
}

void CSinabroDialog::OnAccountSelectMenu(UINT id)
{
	if (sinabro.GetAnalysisStatus())
	{
		::AfxMessageBox(_T("You can't change your account when analyzing."));
		return;
	}

	if (sinabro.GetCurrentAccount() != _T(""))
	{
		SaveAccountData();
	}

	CMenu* pMenu = GetMenu();
	CMenu* pPopup = pMenu->GetSubMenu(1)->GetSubMenu(0);

	if(m_accountIndex != -1) pPopup->CheckMenuItem(USER_ID + m_accountIndex, MF_UNCHECKED);
	if (!sinabro.SetAccount(id - USER_ID)) return;
	m_accountIndex = id - USER_ID;
	pPopup->CheckMenuItem(USER_ID + m_accountIndex, MF_CHECKED);

	LoadAccountData();
}

BOOL CSinabroDialog::LoadAccountData(void)
{
	if (!PathIsDirectory(_T("data"))) _wmkdir(_T("data"));
	m_accountFile.Load(GetFilePathByAccount(sinabro.GetCurrentAccount()));
	m_accountFile.SetAccount(sinabro.GetCurrentAccount());

	return TRUE; 
}

BOOL CSinabroDialog::UpdateAccountData(void)
{
	m_accountFile.Update(sinabro.GetAsset());
	return TRUE;
}

BOOL CSinabroDialog::SaveAccountData(void)
{
	m_accountFile.Save();
	return TRUE;
}


void CSinabroDialog::OnMenuAboutus()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CSinabroDialog::OnMenuView(INT nIndex)
{
	if (m_viewIndex == nIndex) return;
	if (!sinabro.GetConnectionStatus())
	{
		::AfxMessageBox(_T("Login first."));
		return;
	}

	CMenu* pPopup = GetMenu()->GetSubMenu(2);

	if (m_viewIndex != -1)
	{
		pPopup->CheckMenuItem(pPopup->GetMenuItemID(m_viewIndex), MF_UNCHECKED);
	}

	pPopup->CheckMenuItem(pPopup->GetMenuItemID(nIndex), MF_CHECKED);

	m_viewIndex = nIndex;
	ShowGrid();
}

void CSinabroDialog::OnMenuViewAnalysis()
{	
	OnMenuView(SNBR_VIEW_INDEX_ANALYSIS);
}


void CSinabroDialog::OnMenuViewTrading()
{
	OnMenuView(SNBR_VIEW_INDEX_TRADING);
}

void CSinabroDialog::OnViewDayProfit()
{
	OnMenuView(SNBR_VIEW_INDEX_DAYPROFIT);
}



void CSinabroDialog::OnMenuStartAnalysis()
{
	if (!sinabro.GetConnectionStatus())
	{
		::AfxMessageBox(_T("Login first."));
		return;
	}

	if (sinabro.GetCurrentAccount() == _T(""))
	{
		::AfxMessageBox(_T("Select a account."));
		return;
	}

	CMenu* menu = GetMenu()->GetSubMenu(3);

	if (!sinabro.GetAnalysisStatus())
	{
		menu->CheckMenuItem(ID_CONTROL_ANALYSIS, MF_CHECKED);
		sinabro.Analyze();
	}

	else
	{
		menu->CheckMenuItem(ID_CONTROL_TRADING, MF_UNCHECKED);
		menu->CheckMenuItem(ID_CONTROL_ANALYSIS, MF_UNCHECKED);
		sinabro.Trade(FALSE);
		sinabro.Analyze(FALSE);
	}

	m_statusBar.SetPaneText(1, _T("Start analyzing."));
}


void CSinabroDialog::OnControlTrading()
{
	if (!sinabro.GetAnalysisStatus())
	{
		::AfxMessageBox(_T("You have to analyze first."));
		return;
	}

	CMenu* menu = GetMenu()->GetSubMenu(3);
	if (!sinabro.GetTradingStatus())
	{
		menu->CheckMenuItem(ID_CONTROL_TRADING, MF_CHECKED);
		sinabro.Trade();
	}

	else
	{
		menu->CheckMenuItem(ID_CONTROL_TRADING, MF_UNCHECKED);
		sinabro.Trade(FALSE);
	}
}


void CSinabroDialog::OnMenuAccountInfo()
{
	if (!sinabro.GetConnectionStatus())
	{
		::AfxMessageBox(_T("Login first."));
		return;
	}

	AccountInfoDialog accountInfoDlg;
	accountInfoDlg.DoModal();
}

void CSinabroDialog::OnReceiveTrDataKhopenapictrl(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode,
	LPCTSTR sRecordName, LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg)
{
	CString strRQName = sRQName;
	BOOL bReturn;

	bReturn = sinabro.TranslateTrMessage(sScrNo, sRQName, sTrCode, sRecordName, sPrevNext, nDataLength, sErrorCode,
		sMessage, sSplmMsg);

	if (strRQName == _T("거래량급증"))
	{
		if (m_viewIndex == SNBR_VIEW_INDEX_ANALYSIS) ShowAnalyzeGrid();
		sinabro.LockThread(FALSE);
	}

	else if (strRQName == _T("체결정보"))
	{
		INT index = sinabro.GetStockIndex(sinabro.GetAnalyzedStockByCode());
		if (m_viewIndex == SNBR_VIEW_INDEX_ANALYSIS) ShowRow(index);
		sinabro.LockThread(FALSE);
	}

	else if (strRQName == _T("추정자산조회"))
	{
		ShowDepositOnStatusBar();
		//if (sinabro.GetTradingStatus() && sinabro.GetThreadLockStatus()) sinabro.LockThread(FALSE);
		UpdateAccountData();

		if (m_viewIndex == SNBR_VIEW_INDEX_DAYPROFIT)
		{
			ShowRow(m_accountFile.GetDataCount() - 1);
		}

		else if (m_viewIndex == SNBR_VIEW_INDEX_TRADING && sinabro.GetTradingStatus())
		{
			//m_gridContents.SetRowCount(m_gridContents.GetRowCount() + 1);
			//ShowRow(sinabro.GetTradingStockCount());
		}

		SinabroStock* stock = sinabro.GetAnalyzedStock();
		if (sinabro.GetTradingStatus() && stock->GetBoughtVolume() == stock->GetKeepingVolume() &&
			stock->GetStatus() == SNBR_STOCK_ALL_BOUGHT)
		{
			ShowRow(sinabro.GetStockIndex(sinabro.GetAnalyzedStockByCode()));
			sinabro.LockThread(FALSE);
		}

		else if (sinabro.GetTradingStatus() && stock->GetKeepingVolume() == stock->GetSoldVolume() &&
			stock->GetStatus() == SNBR_STOCK_ALL_SOLD)
		{
			ShowRow(sinabro.GetStockIndex(sinabro.GetAnalyzedStockByCode()));
			sinabro.LockThread(FALSE);
		}
	}

	else if (strRQName == _T("주식주문"))
	{
		ShowRow(sinabro.GetStockIndex(sinabro.GetAnalyzedStockByCode()));
	}

	if (bReturn) sinabro.LockThread(FALSE);
}

void CSinabroDialog::OnReceiveChejanData(LPCTSTR sGubun, long nItemCnt, LPCTSTR sFIdList)
{
	BOOL bReturn = sinabro.TranslateChejan(sGubun, nItemCnt, sFIdList);

	INT index = sinabro.GetStockIndex(sinabro.GetAnalyzedStockByCode());
	if (m_viewIndex == SNBR_VIEW_INDEX_ANALYSIS) ShowRow(index);

	ShowDepositOnStatusBar();

	if(bReturn) sinabro.LockThread(FALSE);
}


void CSinabroDialog::OnReceiveMsgKH(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg)
{
	/*BOOL bReturn;
	bReturn = sinabro.TranslateMessage(sScrNo, sRQName, sTrCode, sMsg);

	INT index = sinabro.GetStockIndex(sinabro.GetAnalyzedStockByCode());
	if (m_viewIndex == SNBR_VIEW_INDEX_ANALYSIS && index >= 0) ShowRow(index);

	if (!bReturn) sinabro.LockThread(FALSE);*/
}