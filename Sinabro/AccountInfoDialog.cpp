// AccountInfoDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Sinabro.h"
#include "SinabroApp.h"
#include "SinabroDialog.h"
#include "AccountInfoDialog.h"
#include "afxdialogex.h"


// AccountInfoDialog ��ȭ �����Դϴ�.

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


// AccountInfoDialog �޽��� ó�����Դϴ�.


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
