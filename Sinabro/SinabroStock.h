#pragma once

#if !defined _SINABRO_STOCK_H_
#define _SINABRO_STOCK_H_

#include "khopenapictrl1.h"

//////////////////////////////////////////////////////////////////////////////////////////
/*** DEFINE STOCK INFORMATION ****/

#define SNBR_STOCK_CODE_LENGTH		6						// the length of code

//////////////////////////////////////////////////////////////////////////////////////////
/*** DEFINE ANALYSIS INFORMATION ****/

#define SNBR_MAX_TICK				35						// max analyzable tick

enum SNBR_STOCK_STATUS {
	SNBR_STOCK_UNPROFITABLE, SNBR_STOCK_NOT_ANALYZED, SNBR_STOCK_BOUGHT, SNBR_STOCK_ALL_BOUGHT,
	SNBR_STOCK_SOLD, SNBR_STOCK_PROFITABLE, SNBR_STOCK_FAIL_TO_BUY, SNBR_STOCK_ALL_SOLD};

const CString SNBR_STOCK_STATUS_STR[] = {
	_T("Unprofitable"), _T("Not Analyzed"), _T("Bought"), _T("All Bought"), _T("Sold"), _T("Profitable"), 
	_T("Fail to be bought"), _T("All Sold")
};

#define SNBR_ANALYSIS_3STEP_PURCHASE_STRENGTH		500.0f	// purchase strength has to be up to 150%
#define SNBR_ANALYSIS_3STEP_BOUGHT_CNT				20		// bought count has to be up to 20 in 30 ticks
#define SNBR_ANALYSIS_3STEP_TICK_GAP				60		// 30 tick time has to be down to 60 seconds
#define SNBR_ANALYSIS_3STEP_RATE_MIN				-20.0f	// minimum price rate
#define SNBR_ANALYSIS_3STEP_RATE_MAX				10.0f	// maximum price rate

#define SNBR_ANALYSIS_4STEP_DEPOSIT_EVALUATION		30.0f	// max bought price (Percent by deposit)
#define SNBR_ANALYSIS_4STEP_TICK_BOUGHT				10.0f	// max sold count in 30 ticks (Percent by sold in 30 ticks)

#define SNBR_ANALYSIS_5STEP_PURCHASE_STRENGTH		150.0f	// purchase strength has to be down to 100%
#define SNBR_ANALYSIS_5STEP_BOUGHT_CNT				10		// bought count has to be down to 10 in 30 ticks

#define SNBR_PROFITABLE_CONCLUSION_TIME				10		// the needing time for stock to be profitable
#define SNBR_SOLD_CONCLUSION_TIME					12		// the needing time for stock to be sold

//////////////////////////////////////////////////////////////////////////////////////////

class SinabroStock {

//////////////////////////////////////////////////////////////////////////////////////////
/*** MEMBER VARIABLES *****/

protected :

	/// variables for control ////
	CKhopenapictrl1*	m_pKHController;					// Kiwoom open API controller

	/// variables for account ////
	CString				m_account;							// account

	/// variables for stock information ////
	CString				m_stockCode;						// stock code
	CString				m_stockName;						// stock name

	/// variables for stock price ////
	INT64				m_openingPrice;						// opening price of the stock
	INT64				m_currentPrice;						// current price of the stock
	INT64				m_tickPrice[SNBR_MAX_TICK];			// tick price

	INT64				m_bidPrice;							// current price for buying
	INT64				m_offerPrice;						// current price for selling

	INT64				m_boughtPrice;						// buying price
	INT64				m_soldPrice;						// selling price

	INT64				m_commissionPrice;					// commission price

#define SNBR_PRICE_NOT_DEFINED		-1						// price is not defined (not analyzed)

	/// variables for time ////
	CTime				m_boughtTime;						// time when buying
	CTime				m_soldTime;							// time when selling
	CTime				m_analysisFirstTime;				// time when starting analysis
	CTime				m_analysisLastTime;					// last analysis time

	/// variables for stock trading volume ////
	INT64				m_currentVolume;					// current trading volume
	INT64				m_tickVolume[SNBR_MAX_TICK];		// tick volume

	INT					m_boughtVolume;						// buying volume
	INT					m_soldVolume;						// selling volume
	INT					m_keepingVolume;					// current volume

	/// variables for time ////
	CTime				m_tickTime[SNBR_MAX_TICK];			// tick time

	/// variables for stock analysis ////
	INT					m_analysisCnt;						// the number of being analyzed
	INT					m_satisfiedCnt;						// the number of being satisfied when analyzed
	INT					m_analyzedTickCnt;					// the number of tick being analyzed

	/// variables for status ////
	INT					m_status;							// status

#define SNBR_VOLUME_NOT_DEFINED		-1						// volume is not defined (not analyzed)

//////////////////////////////////////////////////////////////////////////////////////////
/*** METHODS *****/

public :

	/// methods for constructor ////
	SinabroStock(void);										// constructor
	SinabroStock(const SinabroStock& stock);				// copy constructor

	/// methods for initialzing ////
	BOOL				Initialize(CKhopenapictrl1*);					// initialize class
	BOOL				Initialize(CKhopenapictrl1*, LPCTSTR stockCode);// initialize class

	/// methods for analysis ////
	BOOL				Analyze(void);						// analyze the stock

	/// methods for purchase and selling ////
	BOOL				Buy(CString account, INT volume);	// buy the stock
	BOOL				Sell(INT volume);					// sell the stock

	/// methods for stock verification ////
	BOOL				IsProfitable(void) const;			// get whether this stock is profitable now

//////////////////////////////////////////////////////////////////////////////////////////

	bool				operator<(SinabroStock&);			// operator <
	bool				operator<=(SinabroStock&);			// operator <=
	bool				operator>(SinabroStock&);			// operator >
	bool				operator>=(SinabroStock&);			// operator >=
	bool				operator==(SinabroStock&);			// operator ==

//////////////////////////////////////////////////////////////////////////////////////////
/*** SETTER METHODS *****/

	/// methods for setting stock
	BOOL				SetStock(LPCTSTR stockCode);		// set stock code
	BOOL				TranslateMessage(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg);
	BOOL				TranslateTrMessage(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sRecordName,
						LPCTSTR sPrevNext, long nDataLength, LPCTSTR sErrorCode,
						LPCTSTR sMessage, LPCTSTR sSplmMsg, INT*pIndex);	// instruct variables by message
	BOOL				TranslateChejan(LPCTSTR sGubun, LONG nItemCnt, LPCTSTR sFIdList);  // translate chejan message
protected :
	BOOL				TranslateVolumeIncreaseMessage(LPCTSTR sTrCode, LPCTSTR sRecordName, INT* pIndex);
	BOOL				TranslateContractInformationMessage(LPCTSTR sTrCode, LPCTSTR sRecordName);


//////////////////////////////////////////////////////////////////////////////////////////
/*** GETTER METHODS *****/

public :

	/// methods for getting controller ////
	CKhopenapictrl1*	GetController(void);				// get controller

	/// methods for getting stock information ////
	CString				GetStockCode(void) const;			// get stock code
	CString				GetStockName(void) const;			// get stock name

	/// methods for getting stock price ////
	INT64				GetOpeningPrice(void) const;		// get opening price
	INT64				GetCurrentPrice(void) const;		// get current price
	INT64				GetTickPrice(INT tickCnt) const;	// get tick price
	FLOAT				GetIncreaseRate(void) const;		// get increase rate of current price

	INT64				GetBidPrice(void) const;			// get bid price
	INT64				GetOfferPrice(void) const;			// get offer price

	INT64				GetBoughtPrice(void) const;			// get bought price
	INT64				GetSoldPrice(void) const;			// get sold price

	INT64				GetCommission(void) const;			// get commission price
	INT64				GetProfitPrice(void) const;			// get profit price

	FLOAT				GetProfitRate(void) const;			// get profit rate
	
	/// methods for getting stock volume ////
	INT64				GetCurrentVolume(void) const;		// get current volume
	INT64				GetTickVolume(INT tickCnt) const;	// get tick volume

#define SNBR_TICK_VOLUME_ERR	0x80000000
// if tick volume is not found or not analyzed return 0x80000000.

	INT					GetBoughtVolume(void) const;		// get volume that bot buys
	INT					GetSoldVolume(void) const;			// get volume that bot sells
	INT					GetKeepingVolume(void) const;		// get volume that bot has

	/// methods for getting time ////
	CTime				GetTickTime(INT tickCnt) const;		// get tick time
	CTime				GetBoughtTime(void) const;			// get buying time
	CTime				GetSoldTime(void) const;			// get sold time
	
	/// methods for getting status ////
	INT					GetStatus(void) const;				// get status

	/// methods for getting stock data ////
	INT					GetAnalysisSatisfiedCount(void) const;	// get satisfied count
	FLOAT				GetTicksPurchaseStrength(void) const;	// get purchase strength in 30 ticks
	INT					GetTicksPurchaseCount(void) const;		// get purchase count in 30 ticks
	INT					GetTicksSecondsGap(void) const;			// get seconds gap in 30 ticks
	INT64				GetTickAllBoughtCount(void) const;		// get the number of bought stock in 30 ticks
	INT64				GetTickAllSoldCount(void) const;		// get the number of sold stock in 30 ticks
	LONGLONG			GetAnalysisTickCount(void) const;		// get analysis tick count

//////////////////////////////////////////////////////////////////////////////////////////

};

//////////////////////////////////////////////////////////////////////////////////////////

#endif	/* end of _SINABRO_STOCK_H_ */