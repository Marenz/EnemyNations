//---------------------------------------------------------------------------
//
//	Copyright (c) 1995, 1996. Windward Studios, Inc.
//	All Rights Reserved.
//
//---------------------------------------------------------------------------


// scanlist.cpp : class for row-by-row scan conversion
//

#include "stdafx.h"
#include "_windwrd.h"
#include "windward/scanline_math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#define new DEBUG_NEW

#include "scanlist.h"

//---------------------------- S c a n L i s t ------------------------------

//--------------------------------------------------------------------------
// ScanList::ScanList
//--------------------------------------------------------------------------
ScanList::ScanList(
	int	iMaxHeight )
	:
		m_iMaxHeight( iMaxHeight ),
		m_piLeft 	( NULL ),
		m_piRight 	( NULL ),
		m_iTopY     ( 0 ),
		m_iBottomY  ( 0 )
{
	ASSERT_STRICT( iMaxHeight > 0 );

	m_piLeft  = new int [ iMaxHeight ];
	m_piRight = new int [ iMaxHeight ];

	m_aiList[ 0 ] = m_piLeft;
	m_aiList[ 1 ] = m_piRight;

	ASSERT_STRICT_VALID( this );
}

//--------------------------------------------------------------------------
// ScanList::~ScanList
//--------------------------------------------------------------------------
ScanList::~ScanList()
{
	delete [] m_piLeft;
	delete [] m_piRight;
}

//--------------------------------------------------------------------------
// ScanList::CheckPoly
//--------------------------------------------------------------------------
BOOL
ScanList::CheckPoly(
	CPoint const	apt[],		// Array of points in clockwise order
	int				iCount )		// # points
{
	for ( int i = 0; i < iCount; ++i )
	{
		int	j1 = i - 1;
		int	j2 = i + 1;

		if ( j1 < 0 )
			j1 = iCount - 1;

		if ( j2 == iCount )
			j2 = 0;

		CPoint	pt1 = CPoint( apt[j1] - apt[i] );
		CPoint	pt2 = CPoint( apt[j2] - apt[i] );

		int	iZ = pt1.y * pt2.x - pt1.x * pt2.y;

		if ( iZ <= 0 )
			return FALSE;
	}

	return TRUE;
}

//--------------------------------------------------------------------------
// ScanList::ScanPoly
//--------------------------------------------------------------------------
void
ScanList::ScanPoly(	
	CPoint const	apt[],		// Array of points in clockwise order
	int				iCount )		// # points
{
	ASSERT_STRICT_VALID( this );
	ASSERT_STRICT( iCount > 2 );
	ASSERT_STRICT( AfxIsValidAddress( apt, iCount * sizeof( CPoint )));

	#ifdef _DEBUG
	CheckPoly( apt, iCount );
	#endif

	m_iTopY   	= apt[0].y;
	m_iBottomY	= apt[0].y;

	for ( int i = 1; i < iCount; ++i )
		if ( apt[i].y < m_iTopY )
			m_iTopY = apt[i].y;
		else if ( apt[i].y > m_iBottomY )
			m_iBottomY = apt[i].y;

	for ( i = 0; i < iCount; ++i )
	{
		int j = i + 1;

		if ( j == iCount )
			j = 0;

		// MakeLine( apt[i], apt[j] );

		int		iSide;
		CPoint	ptTop;
		CPoint	ptBot;

		if ( apt[i].y > apt[j].y )
		{
			iSide = 0;

			ptTop = apt[j];
			ptBot = apt[i];
		}
		else
		{
			iSide = 1;

			ptTop = apt[i];
			ptBot = apt[j];
		}

		int	iDelY	= ptBot.y - ptTop.y;

		if ( 0 == iDelY )
			continue;

		int	iDelX		= ptBot.x - ptTop.x;
		int	iX 		= ptTop.x;
		int * pi 		= m_aiList[ iSide ] + ptTop.y - m_iTopY;
		int	iCount 	= iDelY + 1;

		if ( 0 == iDelX )
		{
			windward::scanlist_detail::FillConstant( pi, iCount, iX );
		}
		else
		{
			windward::scanlist_detail::FillInterpolatedIntegers(
				pi,
				iCount,
				iX,
				iDelX,
				iDelY );
		}
		}
	}

	ASSERT_STRICT_VALID( this );
}

//--------------------------------------------------------------------------
// ScanList::ScanPolyFixed
//--------------------------------------------------------------------------
void
ScanList::ScanPolyFixed(	
	CPoint const	apt[],		// Array of points in clockwise order
	int				iCount )		// # points
{
	ASSERT_STRICT_VALID( this );
	ASSERT_STRICT( iCount > 2 );
	ASSERT_STRICT( AfxIsValidAddress( apt, iCount * sizeof( CPoint )));

	#ifdef _DEBUG
	CheckPoly( apt, iCount );
	#endif

	m_iTopY   	= apt[0].y;
	m_iBottomY	= apt[0].y;

	for ( int i = 1; i < iCount; ++i )
		if ( apt[i].y < m_iTopY )
			m_iTopY = apt[i].y;
		else if ( apt[i].y > m_iBottomY )
			m_iBottomY = apt[i].y;

	for ( i = 0; i < iCount; ++i )
	{
		int j = i + 1;

		if ( j == iCount )
			j = 0;

		// MakeLine( apt[i], apt[j] );

		int		iSide;
		CPoint	ptTop;
		CPoint	ptBot;

		if ( apt[i].y > apt[j].y )
		{
			iSide = 0;

			ptTop = apt[j];
			ptBot = apt[i];
		}
		else
		{
			iSide = 1;

			ptTop = apt[i];
			ptBot = apt[j];
		}

		int	iDelY	= ptBot.y - ptTop.y;

		if ( 0 == iDelY )
			continue;

		int	iDelX		= ptBot.x - ptTop.x;
		int	fixX 		= ( ptTop.x << 16 ) + 0x00008000;
//		int	iX 		= ptTop.x;
		int * pi 		= m_aiList[ iSide ] + ptTop.y - m_iTopY;
		int	iCount 	= iDelY + 1;

		if ( 0 == iDelX )
		{
			windward::scanlist_detail::FillConstant( pi, iCount, fixX );
		}
		else
		{
			windward::scanlist_detail::FillInterpolatedFixed(
				pi,
				iCount,
				fixX,
				iDelX,
				iDelY );
		}
		}
	}

	ASSERT_STRICT_VALID( this );
}

//-------------------------------------------------------------------------
// ScanList::AssertValid
//-------------------------------------------------------------------------
#ifdef _DEBUG
void ScanList::AssertValid() const
{
	CObject::AssertValid();

	ASSERT_STRICT( 0 <= m_iMaxHeight && m_iMaxHeight < 2000 );

	ASSERT_STRICT( AfxIsValidAddress( m_prowends, m_iMaxHeight * sizeof( RowEnds )));

	ASSERT_STRICT( m_iBottomY >= m_iTopY );
	ASSERT_STRICT( m_iBottomY - m_iTopY < m_iMaxHeight );
}

#endif
