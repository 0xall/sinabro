// AccountInfoDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Sinabro.h"
#include "SinabroApp.h"
#include "SinabroDialog.h"
#include "AccountInfoDialog.h"
#include "afxdialogex.h"


// AccountInfoDialog 대화 상자입니다.

IMPLEMENT_DYNAMIC(AccountInfoDialog, CDialogEx)

AccountInfoDialog::AccountInfoDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_ACCOUNTINFO_DIALOG, pParent)
{
	
}

AccountInfoDialog::~AccountInfoDialog()
{
}

void AccountInfoDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEPOSIT, m_sDeposit);
	DDX_Control(pDX, IDC_ACCOUNT_NO, m_sAccount);
}


BEGIN_MESSAGE_MAP(AccountInfoDialog, CDialogEx)
END_MESSAGE_MAP()


// AccountInfoDialog 메시지 처리기입니다.


BOOL AccountInfoDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString deposit, asset;
	deposit.Format(_T("%lld"), sinabro.GetDeposit());
	deposit = _T("DEPOSIT : ") + CSinabroDialog::GetNumberWithComma(deposit);

	m_sAccount.SetWindowTextW(sinabro.GetCurrentAccount());
	m_sDeposit.SetWindowTextW(deposit);

	return TRUE;
}
