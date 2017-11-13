//**************************************************************************//
// The DirectX9 Thing3D now ported to DirextX10.  This module is based on	//
// Microsoft’s “Simple Sample”, but I have done quite a lot of poggeling.   //
// Apart from using my own Thing3D.											//
//																			//
// Bouncing ball; most of the action takes place in the class Ball.			//
//																			//
// This (poggled) code is copyright of Dr Nigel Barlow, lecturer in			//
// computing, University of Plymouth, UK.  email: nigel@soc.plymouth.ac.uk.	//
//																			//
// You may use, modify and distribute this (rather cack-handed in places)	//
// code subject to the following conditions:								//
//																			//
//	1:	You may not use it, or sell it, or use it in any adapted form for	//
//		financial gain, without my written premission.						//
//																			//
//	2:	You must not remove the copyright messages.							//
//																			//
//	3:	You should correct at least 10% of the typig abd spekking errirs.   //
//																			//
// Poggeling ©Nigel Barlow nigel@soc.plymouth.ac.uk.						//
// "Simple Sample" ©Microsoft Corporation. All rights reserved.				//
//**************************************************************************//





//--------------------------------------------------------------------------------------
// File: Basic Thing3D.cpp
//
// Starting point for new Direct3D 11 samples.  For a more basic starting point, 
// use the EmptyProject11 sample instead.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTmisc.h"
#include "DXUTCamera.h"
#include "DXUTSettingsDlg.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "resource.h"
#include "Nig.h"
#include "Thing3D.h"
#include <Ball.h>
#include "Thing3DVS_PS_Pair.h"
#include "Tile.h"
#include <vector>		//Standard lib, no ".h?


//#define DEBUG_VS   // Uncomment this line to debug D3D9 vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug D3D9 pixel shaders 

//**************************************************************************//
// Global Variables.  There are many global variables here (we aren't OO	//
// yet.  I doubt  Roy Tucker (Comp Sci students will know him) will			//
// approve pf this either.  Sorry, Roy.										//
//**************************************************************************//
CModelViewerCamera          g_Camera;				// Not used by Nigel.
CDXUTDirectionWidget        g_LightControl;			// Not used by Nigel.
CDXUTDialogResourceManager  g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg             g_SettingsDlg;          // Device settings dialog
CDXUTTextHelper*            g_pTxtHelper = NULL;
CDXUTDialog                 g_HUD;                  // dialog for standard controls
CDXUTDialog                 g_SampleUI;             // dialog for sample specific controls

// Direct3D 9 resources
extern ID3DXFont*           g_pFont9;
extern ID3DXSprite*         g_pSprite9;






//**************************************************************************//
// Nigel added global variables.											//
//**************************************************************************//
Thing3D         *grass;				//Easier but slower than using a FlatThing3D
Thing3D         *arrow;
XMMATRIX		g_MatProjection;
XMMATRIX		g_matView;
int				g_width  = 800;
int				g_height = 600;;

bool			g_b_LeftArrowDown      = false;	//Status of keyboard.  Thess are set
bool			g_b_RightArrowDown     = false;	//in the callback KeyboardProc(), and 
bool			g_b_UpArrowDown	       = false; //are used in onFrameMove().
bool			g_b_DownArrowDown	   = false;
bool            g_b_WkeyDown           = false;
bool            g_b_SkeyDown           = false;

bool			g_b_MakeNewBall        = false;


float			g_f_newBallSpeedX = 1;
float			g_f_newBallSpeedY = 20;
float			g_f_newBallSpeedZ = 1;




//**********************************************//
// Attributes of the arrow we use to shoot.     //
//**********************************************//
float arrowRx = 0;
float arrowRy = 0;
float arrowRz = 0;
float power = 1;
 



//**************************************************************************//
// A horrible syntax to create a list (vector) of pointers to balls, with   //
// an initial size of 0.													//
//																			//
// As it is a list of Thing3DAbstract, then we can put any child of			//
// Thing3DAbstract into it.													//
//**************************************************************************//
std::vector <Thing3DAbstract *> balls(0);		




//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3


//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext );
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );

extern bool CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
                                             bool bWindowed, void* pUserContext );
extern HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice,
                                            const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
extern HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                           void* pUserContext );
extern void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime,
                                        void* pUserContext );
extern void CALLBACK OnD3D9LostDevice( void* pUserContext );
extern void CALLBACK OnD3D9DestroyDevice( void* pUserContext );

bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext );
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                         const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D11DestroyDevice( void* pUserContext );
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
                                 float fElapsedTime, void* pUserContext );

void InitApp();
void RenderText();
Ball    *NewBall( ID3D11Device *pRenderingDevice, ID3D11DeviceContext *pImmediateContext);
Thing3D *NewGrass(ID3D11Device *pRenderingDevice, ID3D11DeviceContext *pImmediateContext);
Thing3D *NewArrow(ID3D11Device *pRenderingDevice, ID3D11DeviceContext *pImmediateContext);




//**************************************************************************//
// A Windows program always kicks off in WinMain.							//
// Initializes everything and goes into a message processing				//
// loop.																	//
//																			//
// This version uses DXUT, and is much more complicated than previous		//
// versions you have seen.  This allows you to sync the frame rate to your  //
// monitor's vertical sync event.											//
//**************************************************************************//
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, 
					 LPWSTR    lpCmdLine, int        nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT will create and use the best device (either D3D9 or D3D11) 
    // that is available on the system depending on which D3D callbacks are set below

	//**************************************************************************//
	// Set DXUT callbacks.														//
    //**************************************************************************//
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );

    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
    DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );

    DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
    DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
    DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
    DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
    DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );
    DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );

    InitApp();
    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true );
    DXUTCreateWindow( L"Bouncing Ball" );

    // Only require 10-level hardware, change to D3D_FEATURE_LEVEL_11_0 to require 11-class hardware
    // Switch to D3D_FEATURE_LEVEL_9_x for 10level9 hardware
    DXUTCreateDevice( D3D_FEATURE_LEVEL_10_0, true, 640, 480 );

    DXUTMainLoop(); // Enter into the DXUT render loop

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    g_SettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SampleUI.Init( &g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent );
    int iY = 30;
    int iYo = 26;
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 0, iY, 170, 22 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 0, iY += iYo, 170, 22, VK_F3 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 0, iY += iYo, 170, 22, VK_F2 );

    g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;
}



//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
    g_pTxtHelper->Begin();
    g_pTxtHelper->SetInsertionPos( 5, 5 );
    g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    g_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
    g_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );
    g_pTxtHelper->End();
}


//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice( ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext )
{
    HRESULT hr;

    ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
    V_RETURN( g_DialogResourceManager.OnD3D11CreateDevice( pd3dDevice, pd3dImmediateContext ) );
    V_RETURN( g_SettingsDlg.OnD3D11CreateDevice( pd3dDevice ) );
    g_pTxtHelper = new CDXUTTextHelper( pd3dDevice, pd3dImmediateContext, &g_DialogResourceManager, 15 );



	grass = NewGrass(pd3dDevice, pd3dImmediateContext);
	arrow = NewArrow(pd3dDevice, pd3dImmediateContext);
	

	//**************************************************************************//
	// Initialize the projection matrix.  Generally you will only want to create//
	// this matrix once and then forget it.	I think we should rebuild this		//
	// matrix if we re-size the swap chain, though.								//
	//**************************************************************************//
	g_MatProjection = XMMatrixPerspectiveFovLH( XM_PIDIV2,				 // Field of view (pi / 2 radians, or 90 degrees
											 g_width / (FLOAT) g_height, // Aspect ratio.
											 0.01f,						 // Near clipping plane.
											 100.0f );					 // Far clipping plane.

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                         const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );
    V_RETURN( g_SettingsDlg.OnD3D11ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );
	g_width  = pBackBufferSurfaceDesc->Width;
	g_height = pBackBufferSurfaceDesc->Height;

    // Setup the camera's projection parameters
    float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
 
    g_HUD.SetLocation( pBackBufferSurfaceDesc->Width - 170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 300 );
    g_SampleUI.SetSize( 170, 300 );

    return S_OK;
}




//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
                                 float fElapsedTime, void* pUserContext )
{
    // If the settings dialog is being shown, then render it instead of rendering the app's scene
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.OnRender( fElapsedTime );
        return;
    }       

    float ClearColor[4] = { 0.176f, 0.196f, 0.667f, 0.0f };
    ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
    pd3dImmediateContext->ClearRenderTargetView( pRTV, ClearColor );

    // Clear the depth stencil
    ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
    pd3dImmediateContext->ClearDepthStencilView( pDSV, D3D11_CLEAR_DEPTH, 1.0, 0 );

  
	//**************************************************************************//
    // Initialize the view matrix.  What you do to the viewer matrix moves the  //
	// viewer, or course.														//
	//																			//
	// The viewer matrix is created every frame here, which looks silly as the	//
	// viewer never moves.  However in general your viewer does move.			//
	//**************************************************************************//
	XMVECTOR Eye = XMVectorSet( 0.0f, 2.0f, -5.0f, 0.0f );
	XMVECTOR At  = XMVectorSet( 0.0f, 3.0f, 5.0f, 0.0f );
	XMVECTOR Up  = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	
	g_matView= XMMatrixLookAtLH( Eye,	// The eye, or viewer's position.
								 At,	// The look at point.
								 Up );	// Which way is up.



	//**********************************************************************//
	// Make a new bakk.  I don't thin the renert finction is a good place to//
	// put this, and maning a ball dynamically is a memory burn, of course.	//
	//**********************************************************************//
	if (g_b_MakeNewBall)
	{
		Ball *b = NewBall(pd3dDevice, pd3dImmediateContext);
		balls.push_back(b);
		g_b_MakeNewBall = false;
	}


	//**********************************************************************//
	// Render the Balls.													//
	// Note, the Thing3D creates its own world matrix.  But it needs to know//
	// the other matrices to pass to its own shader.						//
	//**********************************************************************//
	int numBalls = balls.size();
	for (int i = 0; i < numBalls; i++)
	{
		Ball *ball = (Ball *) balls[i];
		ball->matView       = g_matView;
		ball->matProjection = g_MatProjection;
		ball->frameTime      = fElapsedTime;
		ball->Render(pd3dDevice, pd3dImmediateContext);
	}


	//**********************************************************************//
	// Render the grass, same as the ball.									//
	//**********************************************************************//
	grass->matView       = g_matView;
	grass->matProjection = g_MatProjection;
	grass->Render(pd3dDevice, pd3dImmediateContext);



	//**********************************************************************//
	// Render the streach and rotate it to indicate power					//
	//**********************************************************************//
	arrow->SetScale(1, power, 1);
	arrow->RotateTo(0, XMConvertToRadians(180), 0);
	arrow->RotateBy(arrowRx, arrowRy, arrowRz);
	arrow->matView       = g_matView;
	arrow->matProjection = g_MatProjection;
	arrow->Render(pd3dDevice, pd3dImmediateContext);





	//****************************************************************************//
	// And finally, render the DXUT buttons and the thing which grandly calls	  //
	// itself a HUD (Head Up Display).											  //
	//****************************************************************************//
    DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
    g_HUD.OnRender( fElapsedTime );
    g_SampleUI.OnRender( fElapsedTime );
    RenderText();
    DXUT_EndPerfEvent();

    static DWORD dwTimefirst = GetTickCount();
    if ( GetTickCount() - dwTimefirst > 5000 )
    {    
        OutputDebugString( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
        OutputDebugString( L"\n" );
        dwTimefirst = GetTickCount();
    }
}



//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
    g_DialogResourceManager.OnD3D11ReleasingSwapChain();
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D11DestroyDevice();
    g_SettingsDlg.OnD3D11DestroyDevice();
    DXUTGetGlobalResourceCache().OnDestroyDevice();
    SAFE_DELETE( g_pTxtHelper );

   

    // Delete additional render resources here...

}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D11 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    if( pDeviceSettings->ver == DXUT_D3D9_DEVICE )
    {
        IDirect3D9* pD3D = DXUTGetD3D9Object();
        D3DCAPS9 Caps;
        pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal, pDeviceSettings->d3d9.DeviceType, &Caps );

        // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
        // then switch to SWVP.
        if( ( Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 ||
            Caps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
        {
            pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }

        // Debugging vertex shaders requires either REF or software vertex processing 
        // and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
        if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
        {
            pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
            pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
            pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }
#endif
#ifdef DEBUG_PS
        pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
    }

    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( ( DXUT_D3D9_DEVICE == pDeviceSettings->ver && pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF ) ||
            ( DXUT_D3D11_DEVICE == pDeviceSettings->ver &&
            pDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_REFERENCE ) )
        {
            DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
        }

    }

    return true;
}


//**************************************************************************//
// Handle updates to the scene.  This is called regardless of which D3D		//
// API is used (we are only using Dx11).									//
//**************************************************************************//
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	
	ULONG timeNow = timeGetTime();

	//**********************************************************************//
	// Get each ball to do its thing, as it were.							//
	//																		//
	// also work out which ball, if any, to remove from the list.  They have//
	// a life of 6 seconds.  thay are moved from the list, but NOT from		//
	// memory; ugh, another leak.											//	
	//**********************************************************************//
	int balltoRemove = -1;

	int numBalls = balls.size();
	for (int i = 0; i < numBalls; i++)
	{
		Ball *ball = (Ball *) balls[i];
		ball->frameTime = fElapsedTime;
		ball->Bounce();
		
		if (timeNow > ball->createTime + 1000*6)
		{
			balltoRemove = i;
		}
	}


	//**********************************************************************//
	// What a weird way to remove an item from a list; welcome to C++		//
	//**********************************************************************//
	if (balltoRemove >= 0)
		balls.erase(balls.begin() + balltoRemove, balls.begin() + balltoRemove+1);



	    if (g_b_RightArrowDown) arrowRy -= 0.003f;
        if (g_b_LeftArrowDown)  arrowRy += 0.003f;

        if (g_b_UpArrowDown)   arrowRx += 0.003f;
        if (g_b_DownArrowDown) arrowRx -= 0.003f;

        if (g_b_WkeyDown) power += 0.003f;
        if (g_b_SkeyDown) power -= 0.003f;

}





//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext )
{
    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass messages to settings dialog if its active
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

 
    return 0;
}


//**************************************************************************//
// Handle key presses.														//
//**************************************************************************//
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    switch( nChar )
    {

		//******************************************************************//
		// Nigel code to rotate and move the tiger.							//
		//******************************************************************//
		case VK_LEFT:  g_b_LeftArrowDown  = bKeyDown; break;
		case VK_RIGHT: g_b_RightArrowDown = bKeyDown; break;
		case VK_UP:    g_b_UpArrowDown    = bKeyDown; break;
		case VK_DOWN:  g_b_DownArrowDown  = bKeyDown; break;
		case ' ':	   g_b_MakeNewBall    = bKeyDown; break;
		case 'W':	   g_b_WkeyDown       = bKeyDown; break;
		case 'S':	   g_b_SkeyDown       = bKeyDown; break;
    }
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN:
            DXUTToggleFullScreen();
            break;
        case IDC_TOGGLEREF:
            DXUTToggleREF();
            break;
        case IDC_CHANGEDEVICE:
            g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() );
            break;
    }
}




//**************************************************************************//
// Create a new ball and plug the shader effect and various other stuff		//
// into it.																	//
//**************************************************************************//
Ball *NewBall(ID3D11Device        *pRenderingDevice,
		      ID3D11DeviceContext *pImmediateContext)
{
	//**********************************************************************//
	// Create a new shader effect and plug it into our Thing.  Many things  //
	// can share the same shader.											//
	//																		//
	// I should be checking HRESULTS here....								//
	//**********************************************************************//
	Thing3DVS_PS_Pair *pShader = new Thing3DVS_PS_Pair();
	pShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_VS");

	pShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "Thing3D_PS");

 
	pShader->CreateConstandBuffersAndSampler(pRenderingDevice);


	Ball *newBall = new Ball(pRenderingDevice, pImmediateContext);
	newBall->SetVS_PS_ShaderPair(pShader);

	newBall->CreateMesh(pRenderingDevice, pImmediateContext, L"Media\\Ball\\ball.sdkmesh", 0);
	newBall->SetScale(1, 1, 1);
	newBall->MoveTo(0, 1, 10);
	newBall->createTime = timeGetTime();	//Used to remove ball from balls.


	XMMATRIX matRotations          = XMMatrixRotationRollPitchYaw(-arrowRx, arrowRy, arrowRz);
	XMVECTOR vecInitArrowDirection = XMVectorSet(0, 0, 1, 0);
	XMVECTOR vecArrowDirection     = XMVector3TransformCoord(vecInitArrowDirection, matRotations);

	vecArrowDirection = XMVector3Normalize(vecArrowDirection) * power * 20;

	newBall->setXYZspeed(XMVectorGetX( vecArrowDirection),
		                 XMVectorGetY( vecArrowDirection),
						 XMVectorGetZ( vecArrowDirection));

	return newBall;
}




//**************************************************************************//
// Create new grass and plug the shader effect and various other stuff		//
// into it.																	//
//**************************************************************************//
Thing3D *NewGrass(ID3D11Device        *pRenderingDevice,
		          ID3D11DeviceContext *pImmediateContext)
{
	//**********************************************************************//
	// Create a new shader effect and plug it into our Thing.  Many things  //
	// can share the same shader.											//
	//																		//
	// I should be checking HRESULTS here....								//
	//**********************************************************************//
	Thing3DVS_PS_Pair *pShader = new Thing3DVS_PS_Pair();
	pShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_VS");

	pShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "Thing3D_PS");

 
	pShader->CreateConstandBuffersAndSampler(pRenderingDevice);


	Thing3D *newGrass = new Thing3D(pRenderingDevice, pImmediateContext);
	newGrass->SetVS_PS_ShaderPair(pShader);

	newGrass->CreateMesh(pRenderingDevice, pImmediateContext, L"Media\\Grass\\grass.sdkmesh", 0);
	newGrass->SetScale(1, 1, 1);
	newGrass->MoveTo(0, 0, 0);

	return newGrass;
}



//**************************************************************************//
// Create new arrow and plug the shader effect and various other stuff		//
// into it.																	//
//**************************************************************************//
Thing3D *NewArrow(ID3D11Device        *pRenderingDevice,
		          ID3D11DeviceContext *pImmediateContext)
{
	//**********************************************************************//
	// Create a new shader effect and plug it into our Thing.  Many things  //
	// can share the same shader.											//
	//																		//
	// I should be checking HRESULTS here....								//
	//**********************************************************************//
	Thing3DVS_PS_Pair *pShader = new Thing3DVS_PS_Pair();
	pShader->CreatVertexShader(pRenderingDevice, 
		                       L"Thing3D_VS.hlsl", 
							   "Thing3D_VS");

	pShader->CreatPixelShader(pRenderingDevice, 
		                       L"Thing3D_PS.hlsl", 
							   "Thing3DNoTexture");

 
	pShader->CreateConstandBuffersAndSampler(pRenderingDevice);


	Thing3D *newArrow = new Thing3D(pRenderingDevice, pImmediateContext);
	newArrow->SetVS_PS_ShaderPair(pShader);

	newArrow->CreateMesh(pRenderingDevice, pImmediateContext, L"Media\\Arrow\\arrow.sdkmesh", 0);
	newArrow->SetScale(1, 1, 1);
	newArrow->MoveTo(0, 5, 0);

	return newArrow;
}
















