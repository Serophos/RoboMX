#pragma once


// CMyHeaderCtrl

class CMyHeaderCtrl : public CHeaderCtrl
{
	DECLARE_DYNAMIC(CMyHeaderCtrl)

public:
	CMyHeaderCtrl();
	virtual ~CMyHeaderCtrl();

	virtual void Serialize(CArchive& ar);
	void SetSortArrow(const int nColumn, const BOOL bAscending);

	int		m_nSortColumn;
	BOOL	m_bSortAscending;

protected:
	void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	DECLARE_MESSAGE_MAP()
};


