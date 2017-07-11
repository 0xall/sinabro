//////////////////////////////////////////////////////////////////////////////////////////
/* 
 * Sinabro.h
 * Created By : Seung Won Kang
 * 
 *
 * Sinabro is a bot that analyzes stocks, buys stocks when they would rise in price, 
 * and sells when you make profits from stocks. 
 *
 * Sinabro only analyze Korean stock items. (stock items of KOSPI and KOSDAQ)
 *
 * There are 5 steps in Sinabro's Algorithm.
 *
 *   1 Step : checking conditions
 *			 check conditions (current time)
 *
 *   2 Step : primary observation
 *           Search the top 20 high trading volume stock items
 *
 *   3 Step : stock verification
 *			 select profit-making stocks from the 20 stock items.
 *
 *   4 Step : Buying stocks
 *           buy proper volume of stocks.
 *
 *   5 Step : Selling stocks
 *			 Sell all stocks. If all be sold, go back to 1 step.
 *
 * There are some conditions for going to next step. You can change the conditions
 * by modifying define preprocessor.
 *
**/
//////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#if !defined _SINABRO_H_
#define _SINABRO_H_

#include <vector>
#include <map>

#include "khopenapictrl1.h"
#include "SinabroStock.h"

using namespace std;

#define SNBR_MAX_ANALYSIS_STOCK			12					// max analysis stock count

const CString SNBR_STOCK_EXCEPTION_WORD[] = {
	_T("레버리지"), _T("인버스"), _T("선물"), _T("KODEX"), _T("KOSEF"), _T("TIGER"),
	_T("ARIRANG"), _T("GIANT"), _T("KINDEX")
};

//////////////////////////////////////////////////////////////////////////////////////////

class Sinabro {

//////////////////////////////////////////////////////////////////////////////////////////
/*** MEMBER VARIABLES *****/

protected :

	/// variables for control ////
	CKhopenapictrl1		m_khOpenCtrl;						// KH Open API Controller
	CWinThread*			m_pAnalysisThread;					// thread for analysis

	/// variables for accounts ////
	vector<CString>		m_accounts;							// accounts list
	CString				m_currentAccount;					// current account
	INT64				m_deposit;							// current deposit
	INT64				m_asset;							// current asset
	INT64				m_beginningAsset;					// the asset when selecting an account

	/// variables for stocks ////
	vector<SinabroStock> m_stocks;							// analyzed stocks
public :
	vector<SinabroStock> m_tradingStocks;					// trading stocks
protected :
	CString				m_analyzedStockCode;				// the code of current analyzed stock
	
	/// variables for status ////
	BOOL				m_bThreading;						// whether threading or not
	BOOL				m_bAnalysis;						// whether bot analyzing or not
	BOOL				m_bTrading;							// whether bot trading stocks or not
	BOOL				m_bConnection;						// whether connecting kiwoom or not
	BOOL				m_bThreadAnalysis;					// whether thread analyzing a stock

//////////////////////////////////////////////////////////////////////////////////////////
/*** METHODS *****/

public :

	/// method for constructor ////
	Sinabro(void);											// Constructor

	/// methods for initializing and releasing ////
	BOOL				Initialize(void);					// initialize bot
	BOOL				Release(void);						// release bot

	/// methods for connection ////
	INT					Connect(void);						// login kiwoom
	BOOL				ProcessEventConnect(LONG nErrCode);	// set login information

	/// methods for translating KH message ////
	BOOL				TranslateMessage(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sMsg);
	BOOL				TranslateTrMessage(LPCTSTR sScrNo, LPCTSTR sRQName, LPCTSTR sTrCode, LPCTSTR sRecordName,
							LPCTSTR sPrevNext, LONG nDataLength, LPCTSTR sErrorCode,
							LPCTSTR sMessage, LPCTSTR sSplmMsg);	// translate message
	BOOL				TranslateChejan(LPCTSTR sGubun, LONG nItemCnt, LPCTSTR sFIdList);

	/// static function for analyzing ////
	static UINT			ProcessAnalysis(LPVOID pSinabro);	// analyzing function
	BOOL				Analyze(BOOL bAnalyze = TRUE);		// start analyzing

protected :
	BOOL				LoopAnalysis(INT nIndex);			// thread loop
	BOOL				AnalyzeStock(INT nIndex);			// analyzing a stock

public :

	BOOL				LockThread(BOOL bLock = TRUE);		// lock thread or not
	
	/// methods for trading ////
	BOOL				Trade(BOOL bTrading = TRUE);		// start trading

	/// methods for sorting ////
	BOOL				SortAnalyzedStocks(void);			// sort analyzed stocks
	BOOL				SortTradingStocks(void);			// sort trading stocks

//////////////////////////////////////////////////////////////////////////////////////////
/*** SETTER METHODS *****/

	/// methods for setting account ////
	BOOL				SetAccount(void);					// set account information (deposit, assets..)
	BOOL				SetAccount(INT nIndex);				// set account
	BOOL				SetAccount(CString account);		// set account

//////////////////////////////////////////////////////////////////////////////////////////
/*** GETTER METHODS *****/

public :

	/// method for getting controller ////
	CKhopenapictrl1&	GetControl(void);					// get KH Open API Control

	/// method for getting accounts ////
	CString				GetAccount(INT no = 0) const;		// get account
	INT					GetAccountsCount(void) const;		// get accounts acount
	CString				GetCurrentAccount(void) const;		// get current account

	INT64				GetDeposit(void) const;				// get deposit
	INT64				GetBeginningAsset(void) const;	// get beginning deposit
	INT64				GetAsset(void) const;				// get asset

	/// methods for getting stocks ////
	SinabroStock*		GetStock(CString stockCode);		// get stock from list
	SinabroStock*		GetStock(INT nIndex);				// get stock from list
	INT					GetStockCount(void) const;			// get the number of stocks
	INT					GetStockIndex(CString stockCode);	// get index by stock code
	SinabroStock*		GetAnalyzedStock(void);				// get current analyzed stock
	CString				GetAnalyzedStockByCode(void) const;	// get current analyzed stock by code

	INT					GetIdealPurchaseCnt(SinabroStock*);	// get profitable purchase count
	INT					GetTradingStockCount(void) const;	// get trading stock count
	SinabroStock*		GetTradingStock(INT nIndex);		// get trading stock

	/// methods for getting status ////
	BOOL				GetThreadStatus(void) const;		// get thread status
	BOOL				GetAnalysisStatus(void) const;		// get analysis status
	BOOL				GetTradingStatus(void) const;		// get trading status
	BOOL				GetConnectionStatus(void) const;	// get connection(login) status

	/// methods for getting thread information ///
	BOOL				GetThreadLockStatus(void) const;	// get thread lock status

//////////////////////////////////////////////////////////////////////////////////////////

};

//////////////////////////////////////////////////////////////////////////////////////////

#endif	/* end of _SINABRO_H_ */