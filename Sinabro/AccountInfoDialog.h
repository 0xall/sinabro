#pragma once

#include "SinabroApp.h"
#include "afxwin.h"

// AccountInfoDialog 대화 상자입니다.

class AccountInfoDialog : public CDialogEx
{
	DECLARE_DYNAMIC(AccountInfoDialog)

public:
	AccountInfoDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~AccountInfoDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ACCOUNTINFO_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic m_sDeposit;
	CStatic m_sAccount;
};
