//////////////////////////////////////////////////////////////////////////////////////////
/*
 * SinabroAccountFile.h
 * Created by Seung Won Kang
 *
 * SinabroAccountFile class 
 *
**/

#pragma once

#if !defined _SINABRO_FILE_H_
#define _SINABRO_FILE_H_

//////////////////////////////////////////////////////////////////////////////////////////

/// defining for file header ////
#define SNBR_ACCDAT_FILE_EXT				_T("dat")			// file extension

#define SNBR_ACCDAT_FILE_SIG_LENGTH			6
#define SNBR_ACCDAT_FILE_ACCOUNT_LENGTH		10

#define SNBR_ACCDAT_FILE_SIGNATURE			"SNBRAC"			// file signature
#define SNBR_ACCDAT_FILE_VERSION			100					// file version


/// defining for file data ////
#define SNBR_ACCDAT_FILE_DATE_LENGTH		8

//////////////////////////////////////////////////////////////////////////////////////////
/*** Sinabro Account File Structure *****/

typedef struct SNBR_ACCDAT_FILE_HEADER {
	CHAR		m_signature[SNBR_ACCDAT_FILE_SIG_LENGTH];		// file signature. It must be SNBRAC
	INT			m_version;										// file version
	__time64_t	m_time;											// time that file was saved
	WCHAR		m_account[SNBR_ACCDAT_FILE_ACCOUNT_LENGTH];		// account code
	INT			m_dataCnt;										// data count
} SNBR_ACCDAT_FILE_HEADER;

typedef struct SNBR_ACCDAT_FILE_DATA {
	WCHAR		m_date[SNBR_ACCDAT_FILE_DATE_LENGTH + 1];		// date
	INT64		m_openingAsset;									// opening asset
	INT64		m_endingAsset;									// ending asset
} SNBR_ACCDAT_FILE_DATA;

#include "Sinabro.h"

//////////////////////////////////////////////////////////////////////////////////////////

class SinabroAccountFile
{

//////////////////////////////////////////////////////////////////////////////////////////
/*** MEMBER VARIABLES *****/

	/// variables for file ////
protected :
	CString							m_sPath;
	CString							m_sAccount;

	/// variables for data ////
	SNBR_ACCDAT_FILE_HEADER			m_fileHeader;
	vector<SNBR_ACCDAT_FILE_DATA>	m_fileData;

//////////////////////////////////////////////////////////////////////////////////////////
/*** CONSTRUCTOR METHODS *****/

public :
	SinabroAccountFile();
	~SinabroAccountFile(void);

//////////////////////////////////////////////////////////////////////////////////////////
/*** STATIC METHODS *****/

	static SNBR_ACCDAT_FILE_HEADER GetInitialHeader(void);			// get initial header

//////////////////////////////////////////////////////////////////////////////////////////
/*** METHODS *****/

	/// methods for processing file
	BOOL			SetAccount(LPCTSTR sAccount);			// set account
	BOOL			Load(CString path);						// load the file
	BOOL			Save(void);								// Save the file
	BOOL			Update(INT64 currentAsset);				// update file

	/// methods for checking account data file
	BOOL			CheckAccountDataFile(void);				// check whether the file is account data file

//////////////////////////////////////////////////////////////////////////////////////////
/*** GETTER METHODS *****/

	SNBR_ACCDAT_FILE_HEADER		GetHeader(void) const;			// get header
	SNBR_ACCDAT_FILE_DATA		GetData(SIZE_T nIndex) const;	// get data
	SIZE_T						GetDataCount(void) const;		// get the number of data

};

//////////////////////////////////////////////////////////////////////////////////////////

#endif	/* end of _SINABRO_FILE_H_ */

//////////////////////////////////////////////////////////////////////////////////////////