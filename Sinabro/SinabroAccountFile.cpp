
#include "stdafx.h"
#include "SinabroAccountFile.h"

SinabroAccountFile::SinabroAccountFile() : m_sPath(_T(""))
{
}

SinabroAccountFile::~SinabroAccountFile(void)
{

}


SNBR_ACCDAT_FILE_HEADER SinabroAccountFile::GetInitialHeader(void)
{
	SNBR_ACCDAT_FILE_HEADER fHdr;

	memcpy(fHdr.m_signature, SNBR_ACCDAT_FILE_SIGNATURE, SNBR_ACCDAT_FILE_SIG_LENGTH);
	fHdr.m_version = SNBR_ACCDAT_FILE_VERSION;
	fHdr.m_time = CTime::GetTickCount().GetTime();
	for (int i = 0; i < SNBR_ACCDAT_FILE_ACCOUNT_LENGTH; ++i) fHdr.m_account[i] = L'\0';
	fHdr.m_dataCnt = 0;

	return fHdr;
}

BOOL SinabroAccountFile::SetAccount(LPCTSTR sAccount)
{
	INT i;
	for (i = 0; i < SNBR_ACCDAT_FILE_ACCOUNT_LENGTH; ++i) m_fileHeader.m_account[i] = sAccount[i];

	return TRUE;
}

BOOL SinabroAccountFile::Load(CString path)
{
	CFile file;
	CFileException e;
	INT i;
	
	if (!PathFileExists(path))
	{
		m_fileHeader = GetInitialHeader();

		if (!file.Open(path, CFile::modeCreate | CFile::modeWrite, &e))
		{
			TRACE(_T("File could not be opened %d\n"), e.m_cause);
			return FALSE;
		}

		file.Write(&m_fileHeader, sizeof(SNBR_ACCDAT_FILE_HEADER));
		file.Close();

		return TRUE;
	}

	if (!file.Open(path, CFile::modeRead, &e))
	{
		TRACE(_T("File could not be opened %d\n"), e.m_cause);
		return FALSE;
	}

	m_sPath = path;

	file.Read(&m_fileHeader, sizeof(SNBR_ACCDAT_FILE_HEADER));
	
	if (!CheckAccountDataFile())
	{
		return FALSE;
	}

	m_fileData.resize(m_fileHeader.m_dataCnt);
	for (i = 0; i < m_fileHeader.m_dataCnt; ++i)
	{
		file.Read(&m_fileData[i], sizeof(SNBR_ACCDAT_FILE_DATA));
	}

	return TRUE;
}

BOOL SinabroAccountFile::Save(void)
{
	CFile file;
	CFileException e;
	INT i;

	m_fileHeader.m_time = CTime::GetTickCount().GetTime();
	m_fileHeader.m_dataCnt = m_fileData.size();

	if (!file.Open(m_sPath, CFile::modeCreate | CFile::modeWrite, &e))
	{
		TRACE(_T("File could not be opened %d\n"), e.m_cause);
		return FALSE;
	}

	file.Write(&m_fileHeader, sizeof(SNBR_ACCDAT_FILE_HEADER));

	for (i = 0; i < m_fileHeader.m_dataCnt; ++i)
	{
		file.Write(&m_fileData[i], sizeof(SNBR_ACCDAT_FILE_DATA));
	}

	return TRUE;
}

BOOL SinabroAccountFile::Update(INT64 currentAsset)
{
	CTime currentTime = CTime::GetTickCount();
	CString sCurrentDate;
	SIZE_T i;
	
	sCurrentDate.Format(_T("%04d%02d%02d"), currentTime.GetYear(), currentTime.GetMonth(), currentTime.GetDay());

	for (i = 0; i < m_fileData.size(); ++i) if (m_fileData[i].m_date == sCurrentDate) break;

	if (i == m_fileData.size())
	{
		SNBR_ACCDAT_FILE_DATA newData;
		memcpy(newData.m_date, sCurrentDate.GetString(), sizeof(WCHAR) * SNBR_ACCDAT_FILE_DATE_LENGTH);
		newData.m_date[SNBR_ACCDAT_FILE_DATE_LENGTH] = L'\0';
		newData.m_openingAsset = newData.m_endingAsset = currentAsset;
		m_fileData.push_back(newData);
		m_fileHeader.m_dataCnt = m_fileData.size();
	}

	else
	{
		if (m_fileData[i].m_openingAsset == 0) m_fileData[i].m_openingAsset = currentAsset;
		m_fileData[i].m_endingAsset = currentAsset;
	}

	return TRUE;
}

BOOL SinabroAccountFile::CheckAccountDataFile(void)
{
	if (memcmp(m_fileHeader.m_signature, SNBR_ACCDAT_FILE_SIGNATURE, SNBR_ACCDAT_FILE_SIG_LENGTH))
	{
		return FALSE;
	}

	return TRUE;
}

SNBR_ACCDAT_FILE_HEADER	SinabroAccountFile::GetHeader(void) const
{
	return m_fileHeader;
}

SNBR_ACCDAT_FILE_DATA SinabroAccountFile::GetData(SIZE_T nIndex) const
{
	return m_fileData[nIndex];
}

SIZE_T SinabroAccountFile::GetDataCount(void) const
{
	return m_fileData.size();
}