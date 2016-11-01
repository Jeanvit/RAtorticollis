#include "Relatorio.h"
// MainView.cpp : implementation of the CMainView class
//

#include "stdafx.h"
#include "PrintDev.h"

#include "Doc.h"
#include "MainView.h"
#include "mainfrm.h"
#include "CPage.h"
#include "PrintSelect.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static	int PRINTWHAT;
/////////////////////////////////////////////////////////////////////////////
// CMainView

IMPLEMENT_DYNCREATE(CMainView, CFormView)

BEGIN_MESSAGE_MAP(CMainView, CFormView)
	//{{AFX_MSG_MAP(CMainView)
	ON_BN_CLICKED(IDC_BUTTONPRT, OnButtonprt)
	ON_BN_CLICKED(IDC_BUTTONPREV, OnButtonprev)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, DoPrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, DoPrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainView construction/destruction

CMainView::CMainView()
	: CFormView(CMainView::IDD)
{
	//{{AFX_DATA_INIT(CMainView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// TODO: add construction code here
	PRINTWHAT=0;
}

CMainView::~CMainView()
{
}

void CMainView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CMainView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CFormView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainView printing

BOOL CMainView::OnPreparePrinting(CPrintInfo* pInfo)
{
	pInfo->SetMaxPage(2);
	return DoPreparePrinting(pInfo);
}

void CMainView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{

}

void CMainView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


void CMainView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	CView::OnPrepareDC(pDC, pInfo);
	CString s= CPage::GetPrinterName(pDC);	
	switch(pInfo->m_nCurPage)	
	{
	case 1:CPage::SetPrinterMode(pDC);break;
	case 2:CPage::SetPrinterMode(pDC);break;
	}
}				

void CMainView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	extern void PrintForm1(CPage* pPage);
	extern void PrintForm2(CPage* pPage);
	extern void PrintForm3(CPage* pPage);
	extern void PrintForm4(CPage* pPage);
	extern void PrintForm5(CPage* pPage);
	extern void PrintForm6(CPage* pPage);

	extern void PrintForm7(CPage* pPage);

	CPage*	ps= new CPage(pInfo->m_rectDraw,pDC,MM_TEXT);
//	CPage*	ps= new CPage(pInfo->m_rectDraw,pDC);
if(pInfo->m_nCurPage==1) PrintForm4(ps);
if(pInfo->m_nCurPage==2) PrintForm1(ps);
if (pInfo->m_nCurPage==3) {
	delete ps;
	return;
}
}

/////////////////////////////////////////////////////////////////////////////
// CMainView diagnostics

#ifdef _DEBUG
void CMainView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMainView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CDoc* CMainView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDoc)));
	return (CDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainView message handlers

void CMainView::DoPrintPreview()
{
	CPrintSelect dlg;
	dlg.m_Form=PRINTWHAT;
	dlg.DoModal();
	PRINTWHAT=dlg.m_Form;
	CFormView::OnFilePrintPreview();
}

void CMainView::DoPrint()
{
	CPrintSelect dlg;
	dlg.m_Form=PRINTWHAT;
	dlg.DoModal();
	PRINTWHAT=dlg.m_Form;
	CFormView::OnFilePrint();
}

void CMainView::OnButtonprt() 
{
	DoPrint();	
}

void CMainView::OnButtonprev() 
{
	DoPrintPreview();	
}

void CMainView::OnInitialUpdate() 
{
	static	BOOL FirstTime=TRUE;
    CMainFrame* pFrame;
    CRect rectFrame, rectView;

    CFormView::OnInitialUpdate();
	UpdateData(FALSE);

	if(FirstTime==TRUE)
	{
		FirstTime=FALSE;
		// Resize parent to fit dialog template exactly    
		// while not resizing beyond size of screen
		VERIFY( pFrame = (CMainFrame*)GetParentFrame() );
		pFrame->GetClientRect( rectFrame );
		GetClientRect( rectView );
		if ( rectFrame.Width() > rectView.Width()
			 || rectFrame.Height() < rectView.Height() )
		{        
			// Resize so can be refit to any template that fits the screen
			pFrame->MoveWindow( 0, 0, GetSystemMetrics(SM_CXSCREEN), 
							GetSystemMetrics(SM_CYSCREEN), FALSE ); 
		}
		pFrame->RecalcLayout();
		ResizeParentToFit(TRUE);    // Shrink to fit template
		pFrame->CenterWindow();
	}	
}

void CMainView::OnPaint() 
{
	CMainFrame* pFrame;
	CPaintDC dc(this); // device context for painting
	VERIFY( pFrame = (CMainFrame*)GetParentFrame() );
	CString s="ISS Printing Library Demo Program";
	pFrame->SetWindowText(s);
}




Relatorio::Relatorio(void)
{
}


Relatorio::~Relatorio(void)
{
}
