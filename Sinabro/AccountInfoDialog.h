#pragma once

#include "SinabroApp.h"
#include "afxwin.h"

// AccountInfoDialog ��ȭ �����Դϴ�.

class AccountInfoDialog : public CDialogEx
{
	DECLARE_DYNAMIC(AccountInfoDialog)

public:
	AccountInfoDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~AccountInfoDialog();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ACCOUNTINFO_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CStatic m_sDeposit;
	CStatic m_sAccount;
};
