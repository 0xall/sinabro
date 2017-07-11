
#include "stdafx.h"

#include "SinabroStock.h"

//////////////////////////////////////////////////////////////////////////////////////////

SinabroStock::SinabroStock() : m_pKHController(NULL)
{
}

SinabroStock::SinabroStock(const SinabroStock& stock)
{
	m_pKHController = stock.m_pKHController;
	m_account = stock.m_account;

	m_stockCode = stock.m_stockCode;
	m_stockName = stock.m_stockName;

	m_openingPrice = stock.m_openingPrice;
	m_currentPrice = stock.m_currentPrice;
	for (int i = 0; i < SNBR_MAX_TICK; ++i) m_tickPrice[i] = stock.m_tickPrice[i];

	m_bidPrice = stock.m_bidPrice;
	m_offerPrice = stock.m_offerPrice;

	m_boughtPrice = stock.m_boughtPrice;
	m_soldPrice = stock.m_soldPrice;

	m_commissionPrice = stock.m_commissionPrice;

	m_boughtTime = stock.m_boughtTime;
	m_soldTime = stock.m_soldTime;
	m_analysisFirstTime = stock.m_analysisFirstTime;
	m_analysisLastTime = stock.m_analysisLastTime;

	m_currentVolume = stock.m_currentVolume;
	for (int i = 0; i < SNBR_MAX_TICK; ++i) m_tickVolume[i] = stock.m_tickVolume[i];

	m_boughtVolume = stock.m_boughtVolume;
	m_soldVolume = stock.m_soldVolume;
	m_keepingVolume = stock.m_keepingVolume;

	for (int i = 0; i < SNBR_MAX_TICK; ++i) m_tickTime[i] = stock.m_tickTime[i];

	
	m_analysisCnt = stock.m_analysisCnt;
	m_satisfiedCnt = stock.m_satisfiedCnt;
	m_analyzedTickCnt = stock.m_analyzedTickCnt;

	m_status = stock.m_status;
}

//////////////////////////////////////////////////////////////////////////////////////////

BOOL SinabroStock::Initialize(CKhopenapictrl1* pKHController) 
{
	// set controller
	m_pKHController = pKHController;

	// set stock variables
	m_stockCode = _T("");
	m_stockName = _T("");

	// set price variables
	m_openingPrice = -1;
	m_currentPrice = -1;

	m_bidPrice = -1;
	m_offerPrice = -1;

	m_boughtPrice = -1;
	m_soldPrice = -1;

	m_commissionPrice = -1;

	// set volume variable
	m_currentVolume = -1;

	m_boughtVolume = -1;
	m_soldVolume = -1;
	m_keepingVolume = -1;

	// set count variables
	m_analysisCnt = 0;
	m_satisfiedCnt = 0;
	m_analyzedTickCnt = 0;

	// set status variable
	m_status = SNBR_STOCK_NOT_ANALYZED;

	return TRUE;
}

BOOL SinabroStock::Initialize(CKhopenapictrl1* pKHController, LPCTSTR stockCode) 
{
	if (lstrlenW(stockCode) != SNBR_STOCK_CODE_LENGTH) return FALSE;
	// default call initializing function
	Initialize(pKHController);

	// set stock variables
	if (!SetStock(stockCode)) return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////

BOOL SinabroStock::Analyze(void) 
{
	CTime currentTime = CTime::GetTickCount();
	CTimeSpan analysisTimeElapsed;

	if (!m_pKHController->GetConnectState()) return FALSE;

	switch (m_status)
	{
	case SNBR_STOCK_UNPROFITABLE :
	case SNBR_STOCK_SOLD :
	case SNBR_STOCK_ALL_SOLD :
	case SNBR_STOCK_FAIL_TO_BUY :
		break;

	case SNBR_STOCK_PROFITABLE :
		if (!IsProfitable()) m_status = SNBR_STOCK_UNPROFITABLE;
		break;

	case SNBR_STOCK_NOT_ANALYZED :
		if (!IsProfitable())
		{
			m_status = SNBR_STOCK_UNPROFITABLE;
			break;
		}

		if (!m_analysisCnt)
		{
			m_analysisFirstTime = CTime::GetTickCount();
		}

		m_analysisLastTime = CTime::GetTickCount();

		m_analysisCnt++;
		m_satisfiedCnt++;

		analysisTimeElapsed = m_analysisLastTime - m_analysisFirstTime;

		if (analysisTimeElapsed.GetTotalSeconds() >= SNBR_PROFITABLE_CONCLUSION_TIME)
		{
			m_status = SNBR_STOCK_PROFITABLE;
			m_analysisCnt = 0; m_satisfiedCnt = 0;
		}
		break;

	case SNBR_STOCK_ALL_BOUGHT :
		if (GetTicksPurchaseStrength() < SNBR_ANALYSIS_5STEP_PURCHASE_STRENGTH ||
			GetTicksPurchaseCount() < SNBR_ANALYSIS_5STEP_BOUGHT_CNT)
		{
			m_analysisLastTime = CTime::GetTickCount();
		}
		else
		{
			m_analysisFirstTime = CTime::GetTickCount();
			m_analysisLastTime = CTime::GetTickCount();
		}

		break;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////

BOOL SinabroStock::Buy(CString account, INT volume) 
{
	if (volume == 0)
	{
		m_status = SNBR_STOCK_FAIL_TO_BUY;
		return FALSE;
	}
	m_account = account;
	m_analysisCnt = 0;
	m_satisfiedCnt = 0;
	if (m_pKHController->SendOrder(_T("주식주문"), _T("0101"),
		account, 1, m_stockCode, volume, 0, _T("03"), _T("")))
	{
		return FALSE;
	}

	m_boughtTime = CTime::GetTickCount();
	m_boughtVolume = volume;
	m_keepingVolume = 0;
	return TRUE;
}

BOOL SinabroStock::Sell(INT volume) 
{
	m_status = SNBR_STOCK_SOLD;

	m_soldTime = CTime::GetTickCount();
	if (m_pKHController->SendOrder(_T("주식주문"), _T("0101"),
		m_account, 2, m_stockCode, volume, 0, _T("03"), _T("")))
	{
		return FALSE;
	}

	m_soldPrice = 0;
	m_soldVolume = 0;
	return TRUE;
}

BOOL SinabroStock::IsProfitable(void) const
{
	if (GetTicksPurchaseStrength() < SNBR_ANALYSIS_3STEP_PURCHASE_STRENGTH ||
		GetTicksPurchaseCount() < SNBR_ANALYSIS_3STEP_BOUGHT_CNT ||
		GetTicksSecondsGap() > SNBR_ANALYSIS_3STEP_TICK_GAP ||
		GetIncreaseRate() < SNBR_ANALYSIS_3STEP_RATE_MIN ||
		GetIncreaseRate() > SNBR_ANALYSIS_3STEP_RATE_MAX) return FALSE;

	CTime time = CTime::GetTickCount();
	if ((time.GetHour() >= 15 && time.GetHour() <= 18) ||
		(time.GetHour() >= 7 && time.GetHour() < 9)) return FALSE;

	if (time.GetHour() == 9 && time.GetMinute() <= 5) return FALSE;

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////

BOOL SinabroStock::SetStock(LPCTSTR stockCode) 
{
	// The length of stock code is always 6, so if the length of variable stockCode is not 6,
	// return FALSE.
	if (lstrlenW(stockCode) != SNBR_STOCK_CODE_LENGTH) return FALSE;

	// set variables for stock
	m_stockCode = stockCode;
	m_stockName = m_pKHController->GetMasterCodeName(stockCode);

	return TRUE;
}

BOOL SinabroStock::TranslateMessage(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg)
{
	CString strRQName = sRQName, strMsg = sMsg;

	if (strRQName == _T("주식주문"))
	{
		if (strMsg == _T("[505217] 장종료되었습니다") ||
			strMsg == _T("[571489] 장이 열리지않는 날입니다.") ||
			strMsg == _T("[00Z218] 모의투자 장종료 상태입니다"))
		{
			m_status = SNBR_STOCK_FAIL_TO_BUY;
			return FALSE;
		}

		else if (strMsg == _T("[308007] 주문수량을 입력하십시요.") || 
			strMsg == _T("[00Z214] 모의투자 주문수량을 확인하세요") ||
			strMsg == _T("[00Z354] 모의투자 주문가능 금액을 확인하세요"))
		{
			m_status = SNBR_STOCK_UNPROFITABLE;
			return FALSE;
		}
	}

	return TRUE;
}

BOOL SinabroStock::TranslateTrMessage(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sRecordName,
	LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode, LPCTSTR sMessage, LPCTSTR sSplmMsg, INT* pIndex)
{
	// if not login, return FALSE
	if (!m_pKHController->GetConnectState()) return FALSE;

	CString strRQName = sRQName, sOutput;

	if (strRQName == _T("거래량급증")) return TranslateVolumeIncreaseMessage(sTrCode, sRecordName, pIndex);
	else if (strRQName == _T("체결정보")) return TranslateContractInformationMessage(sTrCode, sRecordName);
	else if (strRQName == _T("추정자산조회"))
	{
		if (m_boughtVolume == m_keepingVolume && m_status == SNBR_STOCK_BOUGHT)
		{
			m_status = SNBR_STOCK_ALL_BOUGHT;
			m_analysisFirstTime = CTime::GetTickCount();
			m_analysisLastTime = CTime::GetTickCount();
		}

		else if (m_keepingVolume == m_soldVolume && m_status == SNBR_STOCK_SOLD)
		{
			m_status = SNBR_STOCK_ALL_SOLD;
		}
	}
	else if (strRQName == _T("주식주문"))
	{
		sOutput = m_pKHController->GetCommData(sTrCode, sRecordName, 0, _T("주문번호"));
		if (m_status == SNBR_STOCK_PROFITABLE && sOutput == _T(""))
		{
			m_status = SNBR_STOCK_FAIL_TO_BUY;
		}
	}
	
	return TRUE;
}

BOOL SinabroStock::TranslateChejan(LPCTSTR sGubun, LONG nItemCnt, LPCTSTR sFIdList)
{
	CString data, tradingType, sType = sGubun;
	INT64 allPrice, conclusionPrice;
	INT conclusionVolume;
	
	if (sType == _T("0"))	// if conclusion data
	{
		tradingType = m_pKHController->GetChejanData(905); tradingType.Trim();

		if (tradingType == _T("+매수"))	// if bought
		{
			m_status = SNBR_STOCK_BOUGHT;
			// get stock code
			data = m_pKHController->GetChejanData(9001); data.Trim();
			data = data.Right(6);
			if (data != m_stockCode) return FALSE;

			// get conclusion price
			data = m_pKHController->GetChejanData(910); data.Trim();
			conclusionPrice = _wtoi64(data);

			// get conclusion volume
			data = m_pKHController->GetChejanData(911); data.Trim();
			conclusionVolume = _wtoi(data);

			// get all conclusion price
			allPrice = conclusionPrice * conclusionVolume;
			m_keepingVolume = conclusionVolume;
			if (m_keepingVolume == 0) m_boughtPrice = 0;
			else m_boughtPrice = allPrice / m_keepingVolume;

			m_commissionPrice = (INT64)((FLOAT)m_boughtPrice * m_keepingVolume * 0.00015f +
				(FLOAT)m_bidPrice * m_keepingVolume * 0.00315f);
		}

		else
		{
			m_status = SNBR_STOCK_SOLD;
			// get stock code
			data = m_pKHController->GetChejanData(9001); data.Trim();
			data = data.Right(6);
			if (data != m_stockCode) return FALSE;

			// get conclusion price
			data = m_pKHController->GetChejanData(910); data.Trim();
			conclusionPrice = _wtoi64(data);
			if (conclusionPrice < 0) conclusionPrice = -conclusionPrice;

			// get conclusion volume
			data = m_pKHController->GetChejanData(911); data.Trim();
			conclusionVolume = _wtoi(data);
			if (conclusionVolume < 0) conclusionVolume = -conclusionVolume;

			// get all conclusion price
			allPrice = conclusionPrice * conclusionVolume;
			m_soldVolume = conclusionVolume;
			if (m_soldVolume == 0) m_soldPrice = 0;
			else m_soldPrice = allPrice / m_soldVolume;

			//if (m_boughtVolume == m_soldVolume && m_soldVolume != -1) m_status = SNBR_STOCK_ALL_SOLD;
		}
	}

	return TRUE;
}

BOOL SinabroStock::TranslateContractInformationMessage(LPCTSTR sTrCode, LPCTSTR sRecordName)
{
	CString sOutput;
	INT tickCnt;

	// get tick count
	tickCnt = (INT) m_pKHController->GetRepeatCnt(sTrCode, sRecordName);
	m_analyzedTickCnt = tickCnt;

	// set current price
	sOutput = m_pKHController->GetCommData(sTrCode, sRecordName, 0, _T("현재가")); sOutput.Trim();
	m_currentPrice = _wtoi64(sOutput);
	if (m_currentPrice < 0) m_currentPrice = -m_currentPrice;

	// set current volume
	sOutput = m_pKHController->GetCommData(sTrCode, sRecordName, 0, _T("누적거래량")); sOutput.Trim();
	m_currentVolume = _wtoi64(sOutput);

	// set bid price
	sOutput = m_pKHController->GetCommData(sTrCode, sRecordName, 0, _T("우선매수호가단위")); sOutput.Trim();
	m_bidPrice = _wtoi64(sOutput);
	if (m_bidPrice < 0) m_bidPrice = -m_bidPrice;

	// set offer price
	sOutput = m_pKHController->GetCommData(sTrCode, sRecordName, 0, _T("우선매도호가단위")); sOutput.Trim();
	m_offerPrice = _wtoi64(sOutput);
	if (m_offerPrice < 0) m_offerPrice = -m_offerPrice;

	if (m_status == SNBR_STOCK_ALL_BOUGHT)
	{
		m_commissionPrice = (INT64)((FLOAT)m_boughtPrice * m_keepingVolume * 0.00015f +
			(FLOAT)m_bidPrice * m_keepingVolume * 0.00315f);

		
	}

	for (INT i = 0; i < tickCnt; ++i)
	{
		// set tick price
		sOutput = m_pKHController->GetCommData(sTrCode, sRecordName, i, _T("현재가")); sOutput.Trim();
		m_tickPrice[i] = _wtoi64(sOutput);
		if (m_tickPrice[i] < 0) m_tickPrice[i] = -m_tickPrice[i];

		// set tick volume
		sOutput = m_pKHController->GetCommData(sTrCode, sRecordName, i, _T("체결거래량")); sOutput.Trim();
		m_tickVolume[i] = _wtoi64(sOutput);

		// set tick time
		sOutput = m_pKHController->GetCommData(sTrCode, sRecordName, i, _T("시간")); sOutput.Trim();
		CTime currentTime = ::GetTickCount();
		INT hour = _wtoi(sOutput.Mid(0, 2)), minute = _wtoi(sOutput.Mid(2, 2)), second = _wtoi(sOutput.Mid(4, 2));
		CTime tickTime(currentTime.GetYear(), currentTime.GetMonth(), currentTime.GetDay(), hour, minute, second);
		m_tickTime[i] = tickTime;
	}

	Analyze();

	return TRUE;
}

BOOL SinabroStock::TranslateVolumeIncreaseMessage(LPCTSTR sTrCode, LPCTSTR sRecordName, INT* pIndex)
{
	CString sOutput;
	INT64 currentPrice, risenPrice;

	// check whether this message is fittable
	sOutput = m_pKHController->GetCommData(sTrCode, sRecordName, *pIndex, _T("종목코드")); sOutput.Trim();
	if (sOutput != m_stockCode) return FALSE;

	// set current price
	sOutput = m_pKHController->GetCommData(sTrCode, sRecordName, *pIndex, _T("현재가")); sOutput.Trim();
	currentPrice = _wtoi64(sOutput);
	if (currentPrice < 0) currentPrice = -currentPrice;
	m_currentPrice = currentPrice;

	// set opening price
	sOutput = m_pKHController->GetCommData(sTrCode, sRecordName, *pIndex, _T("전일대비")); sOutput.Trim();
	risenPrice = _wtoi64(sOutput);
	m_openingPrice = m_currentPrice - risenPrice;

	// set current volume
	/*sOutput = m_pKHController->GetCommData(sTrCode, sRecordName, *pIndex, _T("현재거래량")); sOutput.Trim();
	m_currentVolume = _wtoi(sOutput);*/

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool SinabroStock::operator<(SinabroStock& s)
{
	return m_status < s.m_status;
}

bool SinabroStock::operator<=(SinabroStock& s)
{
	return m_status <= s.m_status;
}

bool SinabroStock::operator>(SinabroStock& s)
{
	return m_status > s.m_status;
}

bool SinabroStock::operator>=(SinabroStock& s)
{
	return m_status >= s.m_status;
}

bool SinabroStock::operator==(SinabroStock& s)
{
	return m_status == s.m_status;
}

//////////////////////////////////////////////////////////////////////////////////////////

CKhopenapictrl1* SinabroStock::GetController(void) 
{
	return m_pKHController;
}

CString SinabroStock::GetStockCode(void) const 
{
	return m_stockCode;
}

CString SinabroStock::GetStockName(void) const
{
	return m_stockName;
}

//////////////////////////////////////////////////////////////////////////////////////////

INT64 SinabroStock::GetOpeningPrice(void) const
{
	return m_openingPrice;
}

INT64 SinabroStock::GetCurrentPrice(void) const
{
	return m_currentPrice;
}

INT64 SinabroStock::GetTickPrice(INT tickCnt) const
{
	if (tickCnt < 0 || tickCnt > m_analyzedTickCnt) return -1;
	return m_tickPrice[tickCnt];
}

INT64 SinabroStock::GetBidPrice(void) const
{
	return m_bidPrice;
}

INT64 SinabroStock::GetOfferPrice(void) const
{
	return m_offerPrice;
}

INT64 SinabroStock::GetBoughtPrice(void) const
{
	return m_boughtPrice;
}

INT64 SinabroStock::GetSoldPrice(void) const
{
	return m_soldPrice;
}

INT64 SinabroStock::GetCommission(void) const
{
	if (m_currentVolume <= 0) return -1;
	return m_commissionPrice;
}

INT64 SinabroStock::GetProfitPrice(void) const
{
	if (m_keepingVolume <= 0) return 0x80000000;
	return m_bidPrice * m_keepingVolume - m_boughtPrice * m_keepingVolume - m_commissionPrice;
}

FLOAT SinabroStock::GetProfitRate(void) const
{
	if (m_boughtVolume * m_boughtPrice == 0) return 0.0f;
	return ((FLOAT)(m_bidPrice * m_boughtVolume - m_commissionPrice) / 
		(m_boughtPrice * m_boughtVolume) * 100 -100.0f);
}

FLOAT SinabroStock::GetIncreaseRate(void) const
{
	if (m_openingPrice == 0) return 0.0f;
	return (((FLOAT)m_currentPrice / m_openingPrice - 1.0000f) * 100);
}

INT64 SinabroStock::GetCurrentVolume(void) const
{
	return m_currentVolume;
}

INT64 SinabroStock::GetTickVolume(INT tickCnt) const
{
	if (tickCnt < 0 || tickCnt > m_analyzedTickCnt) return SNBR_TICK_VOLUME_ERR;
	return m_tickVolume[tickCnt];
}

INT SinabroStock::GetBoughtVolume(void) const
{
	return m_boughtVolume;
}

INT SinabroStock::GetSoldVolume(void) const
{
	return m_soldVolume;
}

INT SinabroStock::GetKeepingVolume(void) const
{
	return m_keepingVolume;
}

CTime SinabroStock::GetTickTime(INT tickCnt) const
{
	if (tickCnt < 0 || tickCnt > m_analyzedTickCnt) return CTime();
	return m_tickTime[tickCnt];
}

CTime SinabroStock::GetBoughtTime(void) const
{
	return m_boughtTime;
}

CTime SinabroStock::GetSoldTime(void) const
{
	return m_soldTime;
}

INT SinabroStock::GetStatus(void) const
{
	return m_status;
}

//////////////////////////////////////////////////////////////////////////////////////////

INT SinabroStock::GetAnalysisSatisfiedCount(void) const
{
	return m_satisfiedCnt;
}

FLOAT SinabroStock::GetTicksPurchaseStrength(void) const
{
	INT64 boughtVolume = 0, soldVolume = 0, tickVolume;

	for (INT i = 0; i < m_analyzedTickCnt; ++i)
	{
		tickVolume = m_tickVolume[i];
		if (tickVolume < 0) soldVolume += -tickVolume;
		else boughtVolume += tickVolume;
	}

	if (soldVolume == 0) return 1000.0f;
	return (FLOAT)boughtVolume / soldVolume * 100;
}

INT SinabroStock::GetTicksPurchaseCount(void) const
{
	INT boughtCnt = 0;

	for (INT i = 0; i < m_analyzedTickCnt; ++i) if (m_tickVolume[i] > 0) boughtCnt++;
	return boughtCnt;
}

INT SinabroStock::GetTicksSecondsGap(void) const
{
	CTimeSpan timeGap;
	timeGap = m_tickTime[0].GetTime() - m_tickTime[m_analyzedTickCnt-1].GetTime();
	return (INT) timeGap.GetTotalSeconds();
}

INT64 SinabroStock::GetTickAllBoughtCount(void) const
{
	INT i;
	INT64 boughtCount = 0;
	INT64 max1 = 0, max2 = 0, max3 = 0;

	for (i = 0; i < m_analyzedTickCnt; ++i)
	{
		if (m_tickVolume[i] > 0) boughtCount += m_tickVolume[i];
		else continue;

		if (m_tickVolume[i] >= max1)
		{
			max3 = max2;
			max2 = max1;
			max1 = m_tickVolume[i];
		}

		else if (m_tickVolume[i] >= max2)
		{
			max3 = max2;
			max2 = m_tickVolume[i];
		}

		else if (m_tickVolume[i] > max3)
		{
			max3 = m_tickVolume[i];
		}
	}

	return boughtCount - max1 - max2 - max3;
}

INT64 SinabroStock::GetTickAllSoldCount(void) const
{
	INT i;
	INT64 soldCount = 0;
	for (i = 0; i < m_analyzedTickCnt; ++i)
	{
		if (m_tickVolume[i] < 0) soldCount = m_tickVolume[i];
	}

	return soldCount;
}

LONGLONG SinabroStock::GetAnalysisTickCount(void) const
{
	CTimeSpan timeGap = m_analysisLastTime - m_analysisFirstTime;
	return timeGap.GetTotalSeconds();
}