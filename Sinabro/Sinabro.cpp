
#include "stdafx.h"
#include "Sinabro.h"

#include <algorithm>

//////////////////////////////////////////////////////////////////////////////////////////

Sinabro::Sinabro(void) : m_pAnalysisThread(NULL), m_analyzedStockCode(_T("")),
					m_bAnalysis(FALSE), m_bConnection(FALSE), m_bTrading(FALSE), m_bThreadAnalysis(TRUE),
					m_beginningAsset(0), m_currentAccount(_T(""))
{
}

//////////////////////////////////////////////////////////////////////////////////////////

BOOL Sinabro::Initialize(void) 
{
	m_bThreading = TRUE;
	m_pAnalysisThread = ::AfxBeginThread(ProcessAnalysis, this);
	if (!m_pAnalysisThread) return FALSE;

	return TRUE;
}

BOOL Sinabro::Release(void)
{
	// Release Thread
	if (m_pAnalysisThread)
	{
		m_bThreading = FALSE;
		m_bAnalysis = FALSE;
		CloseHandle(m_pAnalysisThread);
		m_pAnalysisThread = NULL;
		m_bThreadAnalysis = FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////

INT Sinabro::Connect(void)
{
	return m_khOpenCtrl.CommConnect();
}

BOOL Sinabro::ProcessEventConnect(LONG nErrCode)
{
	if (nErrCode)
	{
		return FALSE;
	}

	CString output, account;
	INT indexStart = 0, index, cnt = 0;


	// get accounts count
	output = m_khOpenCtrl.GetLoginInfo(_T("ACCOUNT_CNT")); output.Trim();
	m_accounts.resize(_wtoi(output));
	m_currentAccount = _T("");

	// get accounts
	output = m_khOpenCtrl.GetLoginInfo(_T("ACCNO")); output.Trim();
	while ((index = output.Find(_T(";"), indexStart)) >= 0)
	{
		account = output.Mid(indexStart, index - indexStart);
		m_accounts[cnt++] = account;
		indexStart = index + 1;
	}

	// set connection value to TRUE
	m_bConnection = TRUE;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////

BOOL Sinabro::TranslateMessage(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg)
{
	SinabroStock* currentStock = GetAnalyzedStock();
	if (!currentStock->TranslateMessage(sScrNo, sRQName, sTrCode, sMsg))
	{
		// if failed to buying
		return FALSE;
	}

	return TRUE;
}

BOOL Sinabro::TranslateTrMessage(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sRecordName,
	LPCTSTR sPrevNext, LONG nDataLength, LPCTSTR sErrorCode,
	LPCTSTR sMessage, LPCTSTR sSplmMsg)
{
	CString strRQName = sRQName;
	CString sStockCode, sStockName;
	DOUBLE increment;
	INT i, repeatCnt;
	BOOL bExcept;

	if (strRQName == _T("거래량급증"))
	{
		repeatCnt = m_khOpenCtrl.GetRepeatCnt(sTrCode, sRecordName);
		m_stocks.clear();

		for (i = 0; i < repeatCnt; ++i)
		{
			sStockCode = m_khOpenCtrl.GetCommData(sTrCode, sRecordName, i, _T("종목코드")); sStockCode.Trim();
			sStockName = m_khOpenCtrl.GetCommData(sTrCode, sRecordName, i, _T("종목명")); sStockName.Trim();
			increment = _wtof(m_khOpenCtrl.GetCommData(sTrCode, sRecordName, i, _T("급증률")).Trim());

			bExcept = FALSE;
			for (int j = 0; j < sizeof(SNBR_STOCK_EXCEPTION_WORD) / sizeof(CString); ++j)
			{
				if (sStockName.Find(SNBR_STOCK_EXCEPTION_WORD[j]) != -1) //||
				//	(increment > 1000.0 || increment < 100.0))
				{
					bExcept = TRUE;
					break;
				}
			}
			
			if (bExcept) continue;

			SinabroStock stAdd;
			stAdd.Initialize(&m_khOpenCtrl, sStockCode);
			stAdd.TranslateTrMessage(sScrNo, sRQName, sTrCode, sRecordName, sPrevNext, nDataLength,
				sErrorCode, sMessage, sSplmMsg, &i);
			m_stocks.push_back(stAdd);

			if (m_stocks.size() >= SNBR_MAX_ANALYSIS_STOCK) break;
		}

		if (!m_stocks.size())
		{
			m_stocks.resize(1);
			m_stocks[0].Initialize(&m_khOpenCtrl, _T("023430"));
		}

		m_bAnalysis = TRUE;
	}

	else if (strRQName == _T("체결정보"))
	{
		m_stocks[GetStockIndex(m_analyzedStockCode)].TranslateTrMessage(sScrNo, sRQName, sTrCode,
			sRecordName, sPrevNext, nDataLength, sErrorCode, sMessage, sSplmMsg, NULL);
	}

	else if (strRQName == _T("계좌평가현황"))
	{
		CString sOutput;
		sOutput = m_khOpenCtrl.GetCommData(sTrCode, sRecordName, 0, _T("D+2추정예수금")); sOutput.Trim();
		m_deposit = _wtoi64(sOutput);

		m_khOpenCtrl.SetInputValue(_T("계좌번호"), m_currentAccount);
		m_khOpenCtrl.SetInputValue(_T("비밀번호"), _T(""));
		m_khOpenCtrl.SetInputValue(_T("상장폐지조회구분"), _T("0"));
		if (m_khOpenCtrl.CommRqData(_T("추정자산조회"), _T("OPW00003"), 0, _T("0345"))) return TRUE;
	}

	else if (strRQName == _T("추정자산조회"))
	{
		CString sOutput;
		SinabroStock* stock = GetAnalyzedStock();
		
		if (stock != NULL)
		{
			stock->TranslateTrMessage(sScrNo, sRQName, sTrCode, sRecordName, sPrevNext,
				nDataLength, sErrorCode, sMessage, sSplmMsg, NULL);

			if (stock->GetStatus() == SNBR_STOCK_ALL_SOLD)
			{
				//m_tradingStocks.push_back(*stock);
			}
		}

		sOutput = m_khOpenCtrl.GetCommData(sTrCode, sRecordName, 0, _T("추정예탁자산")); sOutput.Trim();
		m_beginningAsset = _wtoi64(sOutput);
		m_asset = m_beginningAsset;
	}

	else if (strRQName == _T("주식주문"))
	{
		SinabroStock* stock = GetAnalyzedStock();

		if (stock != NULL)
		{
			stock->TranslateTrMessage(sScrNo, sRQName, sTrCode, sRecordName, sPrevNext, nDataLength,
				sErrorCode, sMessage, sSplmMsg, NULL);

			if (stock->GetStatus() == SNBR_STOCK_FAIL_TO_BUY) return TRUE;
		}
	}

	return FALSE;
}

BOOL Sinabro::TranslateChejan(LPCTSTR sGubun, LONG nItemCnt, LPCTSTR sFIdList)
{
	CString stockCode = m_khOpenCtrl.GetChejanData(9001); stockCode.Trim();
	CString data;
	CString sType = sGubun;
	SinabroStock* stock = GetStock(stockCode.Right(6));

	stock->TranslateChejan(sGubun, nItemCnt, sFIdList);

	if (sType == _T("1") && stock->GetBoughtVolume() == stock->GetKeepingVolume() &&
		stock->GetStatus() == SNBR_STOCK_BOUGHT)
	{
		if(!SetAccount()) return TRUE;
	}

	else if (sType == _T("1") && stock->GetKeepingVolume() == stock->GetSoldVolume() &&
		stock->GetStatus() == SNBR_STOCK_SOLD)
	{
		if (!SetAccount()) return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////

UINT Sinabro::ProcessAnalysis(LPVOID pSinabro)
{
	Sinabro* pBot = (Sinabro*)pSinabro;
	INT i, count = 0, flag = 0, timer = 0;

	while (pBot->GetThreadStatus())
	{
		if(pBot->GetAnalysisStatus())
		{
			flag = FALSE;
			for (i = 0; i < pBot->GetStockCount(); ++i)
			{
				if (!pBot->GetAnalysisStatus()) break;
				if (pBot->GetThreadLockStatus())
				{
					i--; continue;
				}

				SinabroStock* stock = pBot->GetStock(i);
				INT status = stock->GetStatus();
				if (status == SNBR_STOCK_ALL_SOLD || status == SNBR_STOCK_UNPROFITABLE
					|| status == SNBR_STOCK_FAIL_TO_BUY || status == SNBR_STOCK_BOUGHT) continue;
				flag = TRUE;

				pBot->m_bThreadAnalysis = TRUE;

				if (!pBot->LoopAnalysis(i))
				{
					i--; continue;
				}

				while (pBot->GetThreadLockStatus());
			}

			if (!flag)
			{	// reanalyze because there is no stock for analyzing
				pBot->m_bThreadAnalysis = TRUE;
				if (!pBot->Analyze()) continue;
				
				
				while (pBot->GetThreadLockStatus());
			}
		}
	}

	
	return 0;
}

BOOL Sinabro::LoopAnalysis(INT nIndex)
{
	LockThread();
	SinabroStock* stock = GetStock(nIndex);
	
	if (stock == nullptr)
	{
		LockThread(FALSE);
		return FALSE;
	}

	INT status = stock->GetStatus();
	m_analyzedStockCode = stock->GetStockCode();

	// buy the stock
	if (status == SNBR_STOCK_PROFITABLE && GetTradingStatus())
	{
		if (!stock->Buy(m_currentAccount, GetIdealPurchaseCnt(stock)))
		{	// if buying is failed 
			LockThread(FALSE);
			return FALSE;
		}
	}

	// sell the stock
	else if (status == SNBR_STOCK_ALL_BOUGHT && GetTradingStatus() && 
		stock->GetAnalysisTickCount() >= SNBR_SOLD_CONCLUSION_TIME)
	{
		if (!stock->Sell(stock->GetKeepingVolume()))
		{	// if selling is failed
			LockThread(FALSE);
			return FALSE;
		}
	}

	// Analyze the stock
	else
	{
		m_analyzedStockCode = stock->GetStockCode();
		if (!AnalyzeStock(nIndex))
		{
			LockThread(FALSE);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL Sinabro::LockThread(BOOL bLock)
{
	if (m_bThreading == FALSE) return FALSE;
	m_bThreadAnalysis = bLock;
	return TRUE;
}

BOOL Sinabro::Analyze(BOOL bAnalysis)
{
	if (!GetConnectionStatus()) return FALSE;

	if (bAnalysis == FALSE)
	{
		//m_analyzedStockCode = _T("");
		m_bAnalysis = FALSE;
		return TRUE;
	}

	LockThread(TRUE);

	CTime currentTime = CTime::GetTickCount();
	
	m_analyzedStockCode = _T("");

	if (currentTime.GetHour() < 9)
	{
		m_khOpenCtrl.SetInputValue(_T("시장구분"), _T("000"));
		m_khOpenCtrl.SetInputValue(_T("정렬구분"), _T("2"));
		m_khOpenCtrl.SetInputValue(_T("시간구분"), _T("2"));
		m_khOpenCtrl.SetInputValue(_T("거래량구분"), _T("10"));
		m_khOpenCtrl.SetInputValue(_T("시간"), _T("1"));
		m_khOpenCtrl.SetInputValue(_T("종목조건"), _T("0"));
		m_khOpenCtrl.SetInputValue(_T("가격구분"), _T("0"));
	}

	else
	{
		m_khOpenCtrl.SetInputValue(_T("시장구분"), _T("000"));
		m_khOpenCtrl.SetInputValue(_T("정렬구분"), _T("2"));
		m_khOpenCtrl.SetInputValue(_T("시간구분"), _T("1"));
		m_khOpenCtrl.SetInputValue(_T("거래량구분"), _T("10"));
		m_khOpenCtrl.SetInputValue(_T("시간"), _T("1"));
		m_khOpenCtrl.SetInputValue(_T("종목조건"), _T("0"));
		m_khOpenCtrl.SetInputValue(_T("가격구분"), _T("0"));
	}
	if (m_khOpenCtrl.CommRqData(_T("거래량급증"), _T("OPT10023"), 0, _T("0168")))
	{
		LockThread(FALSE);
		return FALSE;
	}

	return TRUE;
}

BOOL Sinabro::AnalyzeStock(INT nIndex)
{
	if (!GetConnectionStatus()) return FALSE;
	if (nIndex < 0 || nIndex >= (INT)m_stocks.size()) return FALSE;
	INT stockStatus = m_stocks[nIndex].GetStatus();
	
	if (stockStatus == SNBR_STOCK_UNPROFITABLE ||
		stockStatus == SNBR_STOCK_ALL_SOLD || stockStatus == SNBR_STOCK_FAIL_TO_BUY) return TRUE;

	if (stockStatus == SNBR_STOCK_BOUGHT && m_stocks[nIndex].GetBoughtVolume() ==
		m_stocks[nIndex].GetKeepingVolume())
	{
		SetAccount();
	}

	else if (stockStatus == SNBR_STOCK_SOLD && m_stocks[nIndex].GetKeepingVolume() ==
		m_stocks[nIndex].GetSoldVolume())
	{
		SetAccount();
	}

	else
	{
		m_analyzedStockCode = m_stocks[nIndex].GetStockCode();
		m_khOpenCtrl.SetInputValue(_T("종목코드"), m_analyzedStockCode);
		if (m_khOpenCtrl.CommRqData(_T("체결정보"), _T("OPT10003"), 0, _T("0101"))) return FALSE;
	}
	return TRUE;
}

BOOL Sinabro::Trade(BOOL bTrading)
{
	if (!GetConnectionStatus()) return FALSE;
	m_bTrading = bTrading;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////

BOOL Sinabro::SortAnalyzedStocks(void)
{
	sort(m_stocks.begin(), m_stocks.end());
	return TRUE;
}

BOOL Sinabro::SortTradingStocks(void)
{
	sort(m_tradingStocks.begin(), m_tradingStocks.end());
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////

BOOL Sinabro::SetAccount(void)
{
	m_khOpenCtrl.SetInputValue(_T("계좌번호"), m_currentAccount);
	m_khOpenCtrl.SetInputValue(_T("비밀번호"), _T(""));
	m_khOpenCtrl.SetInputValue(_T("비밀번호입력매체구분"), _T("00"));
	m_khOpenCtrl.SetInputValue(_T("상장폐지조회구분"), _T("0"));
	if(m_khOpenCtrl.CommRqData(_T("계좌평가현황"), _T("OPW00004"), 0, _T("0361"))) return FALSE;
	return TRUE;
}

BOOL Sinabro::SetAccount(INT nIndex)
{
	if (nIndex < 0 || nIndex >= (INT)m_accounts.size()) return FALSE;

	m_khOpenCtrl.SetInputValue(_T("계좌번호"), m_accounts[nIndex]);
	m_khOpenCtrl.SetInputValue(_T("비밀번호"), _T(""));
	m_khOpenCtrl.SetInputValue(_T("비밀번호입력매체구분"), _T("00"));
	m_khOpenCtrl.SetInputValue(_T("상장폐지조회구분"), _T("0"));
	if(m_khOpenCtrl.CommRqData(_T("계좌평가현황"), _T("OPW00004"), 0, _T("0361"))) return FALSE;
	m_currentAccount = m_accounts[nIndex];
	return TRUE;
}

BOOL Sinabro::SetAccount(CString account)
{
	SIZE_T i;
	for (i = 0; i < m_accounts.size(); ++i)
	{
		if (m_accounts[i] == account) break;
	}

	if (i == m_accounts.size()) return FALSE;

	m_khOpenCtrl.SetInputValue(_T("계좌번호"), m_accounts[i]);
	m_khOpenCtrl.SetInputValue(_T("비밀번호"), _T(""));
	m_khOpenCtrl.SetInputValue(_T("비밀번호입력매체구분"), _T("00"));
	m_khOpenCtrl.SetInputValue(_T("조회구분"), _T("2"));
	m_khOpenCtrl.CommRqData(_T("계좌평가현황"), _T("OPW00004"), 0, _T("0361"));
	m_currentAccount = m_accounts[i];
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////

CKhopenapictrl1& Sinabro::GetControl(void)
{
	return m_khOpenCtrl;
}

CString Sinabro::GetAccount(INT no) const
{
	INT accountsCnt = (INT)m_accounts.size();
	if (!accountsCnt || accountsCnt <= no) return _T("NULL");
	return m_accounts[no];
}

INT Sinabro::GetAccountsCount(void) const
{
	return m_accounts.size();
}

CString Sinabro::GetCurrentAccount(void) const
{
	return m_currentAccount;
}

INT64 Sinabro::GetDeposit(void) const
{
	return m_deposit;
}

INT64 Sinabro::GetBeginningAsset(void) const
{
	return m_beginningAsset;
}

INT64	Sinabro::GetAsset(void) const
{
	return m_asset;
}

SinabroStock* Sinabro::GetStock(CString stockCode)
{
	SIZE_T i;
	for (i = 0; i < m_stocks.size(); ++i)
		if (m_stocks[i].GetStockCode() == stockCode) break;
	
	if (i == m_stocks.size()) return NULL;
	return &(m_stocks[i]);
}

SinabroStock* Sinabro::GetStock(INT nIndex)
{
	if (nIndex < 0 || nIndex >= (INT)m_stocks.size()) return NULL;
	return &(m_stocks[nIndex]);
}

INT Sinabro::GetStockCount(void) const
{
	return m_stocks.size();
}

SinabroStock* Sinabro::GetAnalyzedStock(void)
{
	if (m_analyzedStockCode == _T("")) return NULL;
	return GetStock(m_analyzedStockCode);
}

CString Sinabro::GetAnalyzedStockByCode(void) const
{
	if (m_analyzedStockCode == _T("")) return _T("NULL");
	return m_analyzedStockCode;
}

INT Sinabro::GetStockIndex(CString stockCode)
{
	SIZE_T i;
	for (i = 0; i < m_stocks.size(); ++i)
		if (m_stocks[i].GetStockCode() == stockCode) break;

	if (i == m_stocks.size()) return -1;
	return i;
}

INT Sinabro::GetIdealPurchaseCnt(SinabroStock* stock)
{
	INT64 bidPrice = stock->GetBidPrice(), soldCnt = 0;
	INT idealCnt1 = 0, idealCnt2 = 0, minCnt;
	if (bidPrice <= 0 || m_deposit <= 0) return 0;

	soldCnt = stock->GetTickAllBoughtCount();
	idealCnt1 = (INT)((FLOAT)soldCnt * SNBR_ANALYSIS_4STEP_TICK_BOUGHT / 100.f);
	idealCnt2 = ((INT)((FLOAT)m_deposit * SNBR_ANALYSIS_4STEP_DEPOSIT_EVALUATION / 100.f) / (FLOAT)bidPrice);
	minCnt = min(idealCnt1, idealCnt2);
	
	return minCnt;
}

INT	Sinabro::GetTradingStockCount(void) const
{
	return m_tradingStocks.size();
}

SinabroStock* Sinabro::GetTradingStock(INT nIndex)
{
	if (nIndex < 0 || nIndex >= (INT)m_tradingStocks.size()) return NULL;
	return &(m_tradingStocks[nIndex]);
}

BOOL Sinabro::GetThreadStatus(void) const
{
	return m_bThreading;
}

BOOL Sinabro::GetAnalysisStatus(void) const
{
	return m_bAnalysis;
}

BOOL Sinabro::GetTradingStatus(void) const
{
	return m_bTrading;
}

BOOL Sinabro::GetConnectionStatus(void) const
{
	return m_bConnection;
}

BOOL Sinabro::GetThreadLockStatus(void) const
{
	return m_bThreadAnalysis;
}

//////////////////////////////////////////////////////////////////////////////////////////