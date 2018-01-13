//**************************************************************************//
// This is a modified version of the Microsoft sample code and loads a mesh.//
// it uses the helper class CDXUTSDKMesh, as there is no longer any built in//
// support for meshes in DirectX 11.										//
//																			//
// The CDXUTSDKMesh is NOT DorectX, not is the file format it uses, the		//
// .sdkmesh, a standard file format. You will hnot find the .sdkmesh format	//
// outside the MS sample code.  Both these things are provided as an entry	//
// point only.																//
//																			//
// Look for the Nigel style comments, like these, for the bits you need to  //
// look at.																	//
//																			//
// You may notice that this sample tries to create a DirectX11 rendering	//
// device, and if it can't do that creates a DirectX 9 device.  I'm not		//
// using DirectX9.															//
//**************************************************************************//


//**************************************************************************//
// Modifications to the MS sample code is copyright of Dr Nigel Barlow,		//
// lecturer in computing, University of Plymouth, UK.						//
// email: nigel@soc.plymouth.ac.uk.											//
//																			//
// Sdkmesh added to MS sample Tutorial09.									//
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
//**************************************************************************//


//--------------------------------------------------------------------------------------
// File: Tutorial 09 - Meshes Using DXUT Helper Classes.cpp
//
// This sample shows a simple example of the Microsoft Direct3D's High-Level 
// Shader Language (HLSL) using the Effect interface. 
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "DXUTcamera.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "SDKmisc.h"
#include "SDKMesh.h"
#include "resource.h"
#include "Thing3D.h"
#include "Bear.h"
#include "Boid.h"
#include <vector>
#include <ctime>
#include <random>

//**************************************************************************//
// Global Variables.  There are many global variables here (we aren't OO	//
// yet.  I doubt  Roy Tucker (Comp Sci students will know him) will			//
// approve pf this either.  Sorry, Roy.										//
//**************************************************************************//
CDXUTDialogResourceManager  g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg             g_D3DSettingsDlg;       // Device settings dialog
CDXUTDialog                 g_HUD;                  // manages the 3D   
CDXUTDialog                 g_SampleUI;             // dialog for sample specific controls
CModelViewerCamera          g_Camera;				// Not used by Nigel.
CDXUTDirectionWidget        g_LightControl;			// Not used by Nigel.

float                       g_fLightScale;
int                         g_nNumActiveLights;
bool                        g_bShowHelp = false;    // If true, it renders the UI control text
int							g_width = 800;
int							g_height = 600;;

//**************************************************************************//
// Meshes here.																//
//**************************************************************************//
CDXUTSDKMesh                meshBear;			// Wot, not a pointer type?
CDXUTSDKMesh				meshWing;
CDXUTSDKMesh				meshFloor;
CDXUTSDKMesh				meshSky;
CDXUTSDKMesh				meshBoid;

XMMATRIX					matProjection;

ID3D11InputLayout          *g_pVertexLayout11 = NULL;
ID3D11Buffer               *g_pVertexBuffer = NULL;
ID3D11Buffer               *g_pIndexBuffer = NULL;
ID3D11VertexShader         *g_pVertexShader = NULL;
ID3D11PixelShader          *g_pPixelShader = NULL;
ID3D11SamplerState         *g_pSamLinear = NULL;

ID3D11PixelShader			*pDiffuseShader = NULL;

bool		isLeftArrowDown		= false;	//Status of keyboard.  Thess are set
bool		isRightArrowDown	= false;	//in the callback KeyboardProc(), and 
bool		isUpArrowDown		= false;	//are used in onFrameMove().
bool		isDownArrowDown		= false;
bool		isWKeyDown			= false;
bool		isSKeyDown			= false;
bool		isSpaceDown			= false;
bool		isBearView			= false;

float		worldSpinRate		= 0.00001;
float		horizontalRY		= 0.0;
float		horizontalRZ		= 0.0;
float		ground				= 0.0;
float		gravityFallSpeed	= 9.8 / 500000;		//Gravity fall speed with scaling (since 9.8 m/frame/frame seems too fast)
float		airDensity			= 1.2;				// Air at 1 atm pressure, at around 20 degrees Celsius
float		cameraYZoom			= 2.0;
float		cameraStabiliser	= 0.0;
Bear*		bear				= new Bear();
Boid*		flock[100];
int			flockSize			= sizeof(flock) / sizeof(*flock);
float		neighbourRange		= 5.0;
float		neighbourMultiplier = 1.0;
float		minProximity		= 1.0;
float		proximityMultiplier = 1.0;
float		leashLength			= 50.0;
float		bearDistance		= 5.0;
float		leashStrength		= 1.0;
float		leashMultiplier		= 1.0;
float		cohesionStrength	= 1.0;
float		alignmentStrength	= 1.0;
float		separationStrength	= 1.0;
bool		leashOn				= true;
std::mt19937 spawnGen;
std::uniform_real_distribution<float> spawnX(-leashLength, leashLength);
std::uniform_real_distribution<float> spawnRX(0, 2 * XM_PI);


//**************************************************************************//
// This is M$ code, but is usuig old D3DX from DirectX9.  I'm glad to see   //
// that M$ are having issues updating their sample code, same as me - Nigel.//
//**************************************************************************//
CDXUTTextHelper*            g_pTxtHelper = NULL;


//**************************************************************************//
// This is a structure we pass to the vertex shader.  						//
// Note we do it properly here and pass the WVP matrix, rather than world,	//
// view and projection matrices separately.									//
//**************************************************************************//
struct CB_VS_PER_OBJECT
{
	XMMATRIX matWorldViewProj;
	XMMATRIX matWorld;				// needed to transform the normals.
};


//**************************************************************************//
// These are structures we pass to the pixel shader.  						//
// Note we do it properly here and pass the WVP matrix, rather than world,	//
// view and projection matrices separately.									//
//																			//
// These structures must be identical to those defined in the shader that	//
// you use.  So much for encapsulation; Roy	Tucker (Comp Sci students will  //
// know him) will not approve.												//
//**************************************************************************//
struct CB_PS_PER_OBJECT
{
	XMFLOAT4 m_vObjectColor;
};
UINT                        g_iCBPSPerObjectBind = 0;


struct CB_PS_PER_FRAME
{
	XMVECTOR	vecLight;					//Light vector.
	XMVECTOR	vecViewer;					//Vector pointing at viewer;
	XMFLOAT4	lightDiffuseColour;			//Light intensities.
	XMFLOAT4	lightAmbientColour;
	XMFLOAT4	lightSpecularColour;

	XMFLOAT4	materialPower;				//Only first value used.
};

XMFLOAT4	lDiffuseColour(1.0, 1.0, 1.0, 1.0);			// Alpha unused
XMFLOAT4	lAmbientColour(0.2, 0.2, 0.2, 1.0);			// Alpha unused	
XMFLOAT4	lSpecularColour(1.0, 1.0, 1.0, 1.0);		// Alpha unused
UINT        materialShinyness = 80.0;

struct MexhVertexStructure
{
	XMFLOAT4 pos;
	XMFLOAT3 normal;
	XMFLOAT2 TextureUV;
};

UINT                        g_iCBPSPerFrameBind = 1;



//**************************************************************************//
// Now a global instance of each constant buffer.							//
//**************************************************************************//
ID3D11Buffer               *g_pcbVSPerObject = NULL;
ID3D11Buffer               *g_pcbPSPerObject = NULL;
ID3D11Buffer               *g_pcbPSPerFrame = NULL;



//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           3
#define IDC_CHANGEDEVICE        4





//**************************************************************************//
// If you are not used to "C" you will find that functions (or methods in	//
// "C++" must have templates defined in advance.  It is usual to define the //
// prototypes in a header file, but we'll put them here for now to keep		//
// things simple.															//
//**************************************************************************//
//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
	void* pUserContext);
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext);
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext);

extern bool CALLBACK IsD3D9DeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
	bool bWindowed, void* pUserContext);
extern HRESULT CALLBACK OnD3D9CreateDevice(IDirect3DDevice9* pd3dDevice,
	const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
extern HRESULT CALLBACK OnD3D9ResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext);
extern void CALLBACK OnD3D9FrameRender(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime,
	void* pUserContext);
extern void CALLBACK OnD3D9LostDevice(void* pUserContext);
extern void CALLBACK OnD3D9DestroyDevice(void* pUserContext);

bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
	DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext);
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext);
void CALLBACK OnD3D11DestroyDevice(void* pUserContext);
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
	float fElapsedTime, void* pUserContext);

void InitApp();
void RenderText();
void charStrToWideChar(WCHAR *dest, char *source);
void prepareRender(ID3D11DeviceContext *pd3dImmediateContext, CDXUTSDKMesh *toRender, const XMMATRIX &matWorld, const XMMATRIX &matWorldViewProjection, bool isShadow);
void RenderMesh(ID3D11DeviceContext* pd3dImmediateContext, CDXUTSDKMesh *toRender, bool isShadow);
bool isNotTurning();
void keyboardInput(float fElapsedTime);
float increaseStrength(float toIncrease);
float decreaseStrength(float toDecrease);
void flockInteraction(float fElapsedTime);
void spawnFlock();
void updateFlock(ID3D11DeviceContext *pd3dImmediateContext, const XMMATRIX &matView);

//**************************************************************************//
// A Windows program always kicks off in WinMain.							//
// Initializes everything and goes into a message processing				//
// loop.																	//
//																			//
// This version uses DXUT, and is much more complicated than previous		//
// versions you have seen.  This allows you to sync the frame rate to your  //
// monitor's vertical sync event.											//
//**************************************************************************//
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// DXUT will create and use the best device (either D3D9 or D3D11) 
	// that is available on the system depending on which D3D callbacks are set below


	//**************************************************************************//
	// Set DXUT callbacks.														//
	//**************************************************************************//
	DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);
	DXUTSetCallbackMsgProc(MsgProc);
	DXUTSetCallbackKeyboard(OnKeyboard);
	DXUTSetCallbackFrameMove(OnFrameMove);


	DXUTSetCallbackD3D9DeviceAcceptable(IsD3D9DeviceAcceptable);
	DXUTSetCallbackD3D9DeviceCreated(OnD3D9CreateDevice);
	DXUTSetCallbackD3D9DeviceReset(OnD3D9ResetDevice);
	DXUTSetCallbackD3D9FrameRender(OnD3D9FrameRender);
	DXUTSetCallbackD3D9DeviceLost(OnD3D9LostDevice);
	DXUTSetCallbackD3D9DeviceDestroyed(OnD3D9DestroyDevice);


	DXUTSetCallbackD3D11DeviceAcceptable(IsD3D11DeviceAcceptable);
	DXUTSetCallbackD3D11DeviceCreated(OnD3D11CreateDevice);
	DXUTSetCallbackD3D11SwapChainResized(OnD3D11ResizedSwapChain);
	DXUTSetCallbackD3D11FrameRender(OnD3D11FrameRender);
	DXUTSetCallbackD3D11SwapChainReleasing(OnD3D11ReleasingSwapChain);
	DXUTSetCallbackD3D11DeviceDestroyed(OnD3D11DestroyDevice);

	InitApp();
	DXUTInit(true, true, NULL); // Parse the command line, show msgboxes on error, no extra command line params
	DXUTSetCursorSettings(true, true); // Show the cursor and clip it when in full screen
	DXUTCreateWindow(L"Tutorial 09 - Meshes Using DXUT Helper Classes");
	DXUTCreateDevice(D3D_FEATURE_LEVEL_9_2, true, 800, 600);
	//DXUTCreateDevice(true, 640, 480);
	
	spawnGen.seed(std::time(0));
	spawnFlock();

	DXUTMainLoop(); // Enter into the DXUT render loop

	return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
	// Initialize dialogs
	g_D3DSettingsDlg.Init(&g_DialogResourceManager);
	g_HUD.Init(&g_DialogResourceManager);
	g_SampleUI.Init(&g_DialogResourceManager);

	g_HUD.SetCallback(OnGUIEvent); int iY = 10;
	g_HUD.AddButton(IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 0, iY, 170, 23);
	g_HUD.AddButton(IDC_TOGGLEREF, L"Toggle REF (F3)", 0, iY += 26, 170, 23, VK_F3);
	g_HUD.AddButton(IDC_CHANGEDEVICE, L"Change device (F2)", 0, iY += 26, 170, 23, VK_F2);

	g_SampleUI.SetCallback(OnGUIEvent); iY = 10;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D11 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
	// Uncomment this to get debug information from D3D11
	//pDeviceSettings->d3d11.CreateFlags |= D3D11_CREATE_DEVICE_DEBUG;

	// For the first device created if its a REF device, optionally display a warning dialog box
	static bool s_bFirstTime = true;
	if (s_bFirstTime)
	{
		s_bFirstTime = false;
		if ((DXUT_D3D11_DEVICE == pDeviceSettings->ver &&
			pDeviceSettings->d3d11.DriverType == D3D_DRIVER_TYPE_REFERENCE))
		{
			DXUTDisplaySwitchingToREFWarning(pDeviceSettings->ver);
		}
	}

	return true;
}


//**************************************************************************//
// Handle updates to the scene.  This is called regardless of which D3D		//
// API is used (we are only using Dx11).									//
//**************************************************************************//
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	keyboardInput(fElapsedTime);
	flockInteraction(fElapsedTime);
}

void keyboardInput(float fElapsedTime) {
	//Handles turning including a tilt to simulate re-proportioning weight
	//If neither are pressed straightens Z-axis
	if (isLeftArrowDown) {
		bear->turnLeft(fElapsedTime);
		if (bear->inAir(ground) || bear->getSpeed() >= (bear->getMaxForward() * 0.8)) {
			bear->tiltLeft(fElapsedTime);
		}
	}
	if (isRightArrowDown) {
		bear->turnRight(fElapsedTime);
		if (bear->inAir(ground) || bear->getSpeed() >= (bear->getMaxForward() * 0.8)) {
			bear->tiltRight(fElapsedTime);
		}
	}
	if (isNotTurning()) {
		bear->straightenUp(fElapsedTime, horizontalRZ);
	}

	//Handles climbing/falling
	if (isUpArrowDown) {
		bear->tiltUp(fElapsedTime);
		cameraStabiliser = (cameraYZoom / bear->getMaxDescent() * bear->getRY());
	}
	if (isDownArrowDown) {
		if (bear->inAir(ground) || bear->getRY() <= horizontalRY) {
			bear->tiltDown(fElapsedTime);
			cameraStabiliser = (cameraYZoom / bear->getMaxDescent() * bear->getRY());
		}
	}

	//Handles speed adjustments
	//If neither are pressed slows to a halt and falls to y = 0
	if (isWKeyDown) {
		bear->setFallSpeed(0);
		bear->forward(fElapsedTime);
		if (bear->inAir(ground)) {
			bear->wingFlap();
		}
		else {
			bear->restWings();
		}
	}
	if (isSKeyDown) {
		bear->setFallSpeed(0);
		if (bear->inAir(ground) || bear->getRY() >= horizontalRY) {
			bear->reverse(fElapsedTime);
		}
		if (bear->inAir(ground)) {
			bear->wingFlap();
		}
		else {
			bear->restWings();
		}
	}
	if (!isWKeyDown && !isSKeyDown) {
		bear->slowDown(fElapsedTime, airDensity);
		bear->restWings();
		if (bear->inAir(ground)) {
			bear->fall(gravityFallSpeed);
		}
		else {
			bear->slowGround();
		}
	}

	if (!bear->inAir(ground)) {
		bear->setFallSpeed(0);
		bear->levelOut(fElapsedTime, horizontalRY);
	}

	if (isSpaceDown) {
		bear->roar();
	}
}

float increaseStrength(float toIncrease) {
	if (toIncrease < 2.0) {
		toIncrease += 0.1;
	}
	return toIncrease;
}

float decreaseStrength(float toDecrease) {
	if (0.1 < toDecrease) {
		toDecrease -= 0.1;
	}
	return toDecrease;
}

void flockInteraction(float fElapsedTime) {
	XMVECTOR bearPos = XMVectorSet(bear->getX(), bear->getY(), bear->getZ(), 0.0f);

	//For each boid
	for (int i = 0; i < flockSize; i++) {
		std::vector<Boid*> localFlock;
		//Cycle through other boids to determine local flock
		for (int j = 0; j < flockSize; j++) {
			if ((flock[i]->isNear(flock[j], (neighbourRange * neighbourMultiplier))) && (i != j)) {
				localFlock.push_back(flock[j]);
			}
		}
		//If no nearby boids, explore
		if (localFlock.empty()) {
			flock[i]->moveRandomly(fElapsedTime);
		} else { //Otherwise, be a boid
			flock[i]->adjustSpeed(fElapsedTime);
			flock[i]->alignment(localFlock, alignmentStrength);
			flock[i]->cohesion(localFlock, cohesionStrength, fElapsedTime);
			flock[i]->separation(localFlock, separationStrength, (minProximity * proximityMultiplier), fElapsedTime);
		}
		//Run from the bear if it is nearby
		if (flock[i]->isNear(bearPos, (bearDistance * proximityMultiplier))) {
			flock[i]->fleeBear(bearPos, fElapsedTime);
			flock[i]->move(fElapsedTime);
		}
		//Force the boids to stay near the base 3rd-person camera position
		if (leashOn) {
			flock[i]->leash(XMVectorSet(0.0f, 0.0f, -0.1f, 0.0f), leashStrength, (leashLength * leashMultiplier), fElapsedTime);
		}
		flock[i]->move(fElapsedTime);
		//Modulus division required to ensure angles don't go beyond float capacities
		if (2*XM_PI < flock[i]->getRX()) {
			flock[i]->setRX(fmod(flock[i]->getRX(), 2*XM_PI));
		}
	}
}

bool isNotTurning() {
	return (!isLeftArrowDown && !isRightArrowDown);
}

//--------------------------------------------------------------------------------------
// Render the help and statistics text
//--------------------------------------------------------------------------------------
void RenderText()
{
	UINT nBackBufferHeight = (DXUTIsAppRenderingWithD3D9()) ? DXUTGetD3D9BackBufferSurfaceDesc()->Height :
		DXUTGetDXGIBackBufferSurfaceDesc()->Height;

	g_pTxtHelper->Begin();
	g_pTxtHelper->SetInsertionPos(2, 0);
	g_pTxtHelper->SetForegroundColor(D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f));
	g_pTxtHelper->DrawTextLine(DXUTGetFrameStats(DXUTIsVsyncEnabled()));
	g_pTxtHelper->DrawTextLine(DXUTGetDeviceStats());

	// Draw help
	if (g_bShowHelp)
	{
		g_pTxtHelper->SetInsertionPos(2, nBackBufferHeight - 20 * 13);
		g_pTxtHelper->SetForegroundColor(D3DXCOLOR(1.0f, 0.75f, 0.0f, 1.0f));
		g_pTxtHelper->DrawTextLine(L"Controls:");

		g_pTxtHelper->SetInsertionPos(20, nBackBufferHeight - 20 * 12);
		g_pTxtHelper->DrawTextLine(L"Leash: On/Off = e. r = Stronger / d = Weaker. t = Longer / f = Shorter.\n");

		g_pTxtHelper->SetInsertionPos(20, nBackBufferHeight - 20 * 11);
		g_pTxtHelper->DrawTextLine(L"Neighbour Range: y = Longer / g = Shorter.\n");

		g_pTxtHelper->SetInsertionPos(20, nBackBufferHeight - 20 * 10);
		g_pTxtHelper->DrawTextLine(L"Cohesion Strength: u = Stronger / h = Weaker\n");

		g_pTxtHelper->SetInsertionPos(20, nBackBufferHeight - 20 * 9);
		g_pTxtHelper->DrawTextLine(L"Alignment Strength: i = Stronger / j = Weaker\n");

		g_pTxtHelper->SetInsertionPos(20, nBackBufferHeight - 20 * 8);
		g_pTxtHelper->DrawTextLine(L"Separation: o = stronger / k = Weaker. p = Longer / l = Shorter.\n");

		g_pTxtHelper->SetInsertionPos(20, nBackBufferHeight - 20 * 7);
		g_pTxtHelper->DrawTextLine(L"Rotate Model: Left / Right Arrows\n");

		g_pTxtHelper->SetInsertionPos(20, nBackBufferHeight - 20 * 6);
		g_pTxtHelper->DrawTextLine(L"Rotate Model: Up / Down Arrows\n");

		g_pTxtHelper->SetInsertionPos(20, nBackBufferHeight - 20 * 5);
		g_pTxtHelper->DrawTextLine(L"Add Momentum to Model: W / S Keys\n");

		g_pTxtHelper->SetInsertionPos(20, nBackBufferHeight - 20 * 4);
		g_pTxtHelper->DrawTextLine(L"Roar: Space Key\n");

		g_pTxtHelper->SetInsertionPos(20, nBackBufferHeight - 20 * 3);
		g_pTxtHelper->DrawTextLine(L"Apply Momentum & Angle the Bear Skyward to Fly\n");

		g_pTxtHelper->SetInsertionPos(20, nBackBufferHeight - 20 * 2);
		g_pTxtHelper->DrawTextLine(L"Switch views: F4 key\n");

		g_pTxtHelper->SetInsertionPos(550, nBackBufferHeight - 20 * 5);
		g_pTxtHelper->DrawTextLine(L"Hide help: F1\n"
			L"Quit: ESC\n");
	}
	else
	{
		g_pTxtHelper->SetForegroundColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		g_pTxtHelper->DrawTextLine(L"Press F1 for help");
	}

	g_pTxtHelper->End();
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
	void* pUserContext)
{
	// Pass messages to dialog resource manager calls so GUI state is updated correctly
	*pbNoFurtherProcessing = g_DialogResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;

	// Pass messages to settings dialog if its active
	if (g_D3DSettingsDlg.IsActive())
	{
		g_D3DSettingsDlg.MsgProc(hWnd, uMsg, wParam, lParam);
		return 0;
	}

	// Give the dialogs a chance to handle the message first
	*pbNoFurtherProcessing = g_HUD.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;
	*pbNoFurtherProcessing = g_SampleUI.MsgProc(hWnd, uMsg, wParam, lParam);
	if (*pbNoFurtherProcessing)
		return 0;


	return 0;
}

//**************************************************************************//
// Handle key presses.														//
//**************************************************************************//
void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
	if (bKeyDown)
	{
		switch (nChar)
		{
		case VK_F1:
			g_bShowHelp = !g_bShowHelp;
			break;
		case VK_F4:
			isBearView = !isBearView;
			break;
		case 82://r
			leashStrength = increaseStrength(leashStrength);
			break;
		case 68://d
			leashStrength = decreaseStrength(leashStrength);
			break;
		case 84://t
			leashMultiplier = increaseStrength(leashMultiplier);
			break;
		case 70://f
			leashMultiplier = decreaseStrength(leashMultiplier);
			break;
		case 89://y
			neighbourMultiplier = increaseStrength(neighbourMultiplier);
			break;
		case 71://g
			neighbourMultiplier = decreaseStrength(neighbourMultiplier);
			break;
		case 85://u
			cohesionStrength = increaseStrength(cohesionStrength);
			break;
		case 72://h
			cohesionStrength = decreaseStrength(cohesionStrength);
			break;
		case 73://i
			alignmentStrength = increaseStrength(alignmentStrength);
			break;
		case 74://j
			alignmentStrength = decreaseStrength(alignmentStrength);
			break;
		case 79://o
			separationStrength = increaseStrength(separationStrength);
			break;
		case 75://k
			separationStrength = decreaseStrength(separationStrength);
			break;
		case 80://p
			proximityMultiplier = increaseStrength(proximityMultiplier);
			break;
		case 76://l
			proximityMultiplier = decreaseStrength(proximityMultiplier);
			break;
		case 69://e
			leashOn = !leashOn;
			break;
		}
	}

	//**************************************************************//
	// Nigel code to rotate the bear.								//
	//**************************************************************//
	switch (nChar)
	{
	case VK_LEFT:
		isLeftArrowDown = bKeyDown;
		break;
	case VK_RIGHT:
		isRightArrowDown = bKeyDown;
		break;
	case VK_UP:
		isUpArrowDown = bKeyDown;
		break;
	case VK_DOWN:
		isDownArrowDown = bKeyDown;
		break;
	case 87:
		isWKeyDown = bKeyDown;
		break;
	case 83:
		isSKeyDown = bKeyDown;
		break;
	case 32:
		isSpaceDown = bKeyDown;
		break;
	}
}

//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
	switch (nControlID)
	{
	case IDC_TOGGLEFULLSCREEN:
		DXUTToggleFullScreen(); break;
	case IDC_TOGGLEREF:
		DXUTToggleREF(); break;
	case IDC_CHANGEDEVICE:
		g_D3DSettingsDlg.SetActive(!g_D3DSettingsDlg.IsActive()); break;
	}

}


//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
	DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
	return true;
}


//**************************************************************************//
// Compile the shader file.  These files aren't pre-compiled (well, not		//
// here, and are compiled on the fly).										//
//**************************************************************************//
HRESULT CompileShaderFromFile(WCHAR* szFileName,		// File Name
	LPCSTR szEntryPoint,		// Namee of shader
	LPCSTR szShaderModel,		// Shader model
	ID3DBlob** ppBlobOut)	// Blob returned
{
	HRESULT hr = S_OK;

	// find the file
	WCHAR str[MAX_PATH];
	V_RETURN(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, szFileName));

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob* pErrorBlob;
	hr = D3DX11CompileFromFile(str, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
	if (FAILED(hr))
	{
		WCHAR errorCharsW[200];
		if (pErrorBlob != NULL)
		{
			charStrToWideChar(errorCharsW, (char *)pErrorBlob->GetBufferPointer());
			MessageBox(0, errorCharsW, L"Error", 0);
		}
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	SAFE_RELEASE(pErrorBlob);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext)
{
	HRESULT hr;
	
	ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
	V_RETURN(g_DialogResourceManager.OnD3D11CreateDevice(pd3dDevice, pd3dImmediateContext));
	V_RETURN(g_D3DSettingsDlg.OnD3D11CreateDevice(pd3dDevice));
	g_pTxtHelper = new CDXUTTextHelper(pd3dDevice, pd3dImmediateContext, &g_DialogResourceManager, 15);


	//**********************************************************************//
	// Compile the pixel and vertex shaders.  If your computer doesn't		//
	// support shader model 5, try shader model 4.  There is nothing we are //
	// using here that needs shader model 5.								//
	//**********************************************************************..
	ID3DBlob* pVertexShaderBuffer = NULL;
	V_RETURN(CompileShaderFromFile(L"VertexShader.hlsl", "VS_DXUTSDKMesh", "vs_4_0", &pVertexShaderBuffer));

	ID3DBlob* pPixelShaderBuffer = NULL;
	V_RETURN(CompileShaderFromFile(L"PixelShader.hlsl", "PS_DXUTSDKMesh", "ps_4_0", &pPixelShaderBuffer));

	ID3DBlob *pDiffuseShaderBuffer = NULL;
	V_RETURN(CompileShaderFromFile(L"DiffuseOnlyPS.hlsl", "PS_DXUTSDKMesh", "ps_4_0", &pDiffuseShaderBuffer));

	//**********************************************************************//
	// Create the pixel and vertex shaders.									//
	//**********************************************************************//
	V_RETURN(pd3dDevice->CreateVertexShader(pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), NULL, &g_pVertexShader));
	DXUT_SetDebugName(g_pVertexShader, "VS_DXUTSDKMesh");
	V_RETURN(pd3dDevice->CreatePixelShader(pPixelShaderBuffer->GetBufferPointer(),
		pPixelShaderBuffer->GetBufferSize(), NULL, &g_pPixelShader));
	DXUT_SetDebugName(g_pPixelShader, "PS_DXUTSDKMesh");

	V_RETURN(pd3dDevice->CreatePixelShader(pDiffuseShaderBuffer->GetBufferPointer(),
		pDiffuseShaderBuffer->GetBufferSize(), NULL, &pDiffuseShader));
	DXUT_SetDebugName(pDiffuseShader, "PS_DXUTSDKMesh");


	//**********************************************************************//
	// Define the input layout.  I won't go too much into this except that  //
	// the vertex defined here MUST be consistent with the vertex shader	//
	// input you use in your shader file and the constand buffer structure  //
	// at the top of this module.											//
	//																		//
	// Normal vectors are used by lighting.									//
	//**********************************************************************//
	const D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	V_RETURN(pd3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), pVertexShaderBuffer->GetBufferPointer(),
		pVertexShaderBuffer->GetBufferSize(), &g_pVertexLayout11));
	DXUT_SetDebugName(g_pVertexLayout11, "Primary");

	SAFE_RELEASE(pVertexShaderBuffer);
	SAFE_RELEASE(pPixelShaderBuffer);


	//**************************************************************************//
	// Initialize the projection matrix.  Generally you will only want to create//
	// this matrix once and then forget it.										//
	//**************************************************************************//
	matProjection = XMMatrixPerspectiveFovLH(XM_PIDIV2,				// Field of view (pi / 2 radians, or 90 degrees
		g_width / (FLOAT)g_height, // Aspect ratio.
		0.01f,						// Near clipping plane.
		10000.0f);					// Far clipping plane.


									//**************************************************************************//
									// Load the mesh.															//
									//**************************************************************************//
	V_RETURN(meshBear.Create(pd3dDevice, L"Media\\Bear\\bear.sdkmesh", true));
	V_RETURN(meshWing.Create(pd3dDevice, L"Media\\Bear\\bearwing.sdkmesh", true));
	V_RETURN(meshFloor.Create(pd3dDevice, L"Media\\Floor\\seafloor.sdkmesh", true));
	V_RETURN(meshSky.Create(pd3dDevice, L"Media\\Cloudbox\\skysphere.sdkmesh", true));
	V_RETURN(meshBoid.Create(pd3dDevice, L"Media\\Parasaur\\parasaur.sdkmesh", true));

	// Create a sampler state
	D3D11_SAMPLER_DESC SamDesc;
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.MipLODBias = 0.0f;
	SamDesc.MaxAnisotropy = 1;
	SamDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SamDesc.BorderColor[0] = SamDesc.BorderColor[1] = SamDesc.BorderColor[2] = SamDesc.BorderColor[3] = 0;
	SamDesc.MinLOD = 0;
	SamDesc.MaxLOD = D3D11_FLOAT32_MAX;
	V_RETURN(pd3dDevice->CreateSamplerState(&SamDesc, &g_pSamLinear));
	DXUT_SetDebugName(g_pSamLinear, "Primary");


	//**************************************************************************//
	// Create the 3 constant bufers, using the same buffer descriptor to create //
	// all three.																//
	//**************************************************************************//
	D3D11_BUFFER_DESC Desc;
	ZeroMemory(&Desc, sizeof(Desc));
	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;

	Desc.ByteWidth = sizeof(CB_VS_PER_OBJECT);
	V_RETURN(pd3dDevice->CreateBuffer(&Desc, NULL, &g_pcbVSPerObject));
	DXUT_SetDebugName(g_pcbVSPerObject, "CB_VS_PER_OBJECT");

	Desc.ByteWidth = sizeof(CB_PS_PER_OBJECT);
	V_RETURN(pd3dDevice->CreateBuffer(&Desc, NULL, &g_pcbPSPerObject));
	DXUT_SetDebugName(g_pcbPSPerObject, "CB_PS_PER_OBJECT");

	Desc.ByteWidth = sizeof(CB_PS_PER_FRAME);
	V_RETURN(pd3dDevice->CreateBuffer(&Desc, NULL, &g_pcbPSPerFrame));
	DXUT_SetDebugName(g_pcbPSPerFrame, "CB_PS_PER_FRAME");

	//bear->initialise(g_pcbVSPerObject, g_pPixelShader, &meshBear, g_pVertexLayout11, g_pVertexShader);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	HRESULT hr;

	V_RETURN(g_DialogResourceManager.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));
	V_RETURN(g_D3DSettingsDlg.OnD3D11ResizedSwapChain(pd3dDevice, pBackBufferSurfaceDesc));

	g_width = pBackBufferSurfaceDesc->Width;
	g_height = pBackBufferSurfaceDesc->Height;

	g_HUD.SetLocation(g_width - 170, 0);
	g_HUD.SetSize(170, 170);
	g_SampleUI.SetLocation(g_width - 170, g_height - 300);
	g_SampleUI.SetSize(170, 300);

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, double fTime,
	float fElapsedTime, void* pUserContext) {
	HRESULT hr;

	// If the settings dialog is being shown, then render it instead of rendering the app's scene
	if (g_D3DSettingsDlg.IsActive())
	{
		g_D3DSettingsDlg.OnRender(fElapsedTime);
		return;
	}

	// Clear the render target and depth stencil
	float ClearColor[4] = { 0.0f, 0.25f, 0.25f, 0.55f };
	ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
	pd3dImmediateContext->ClearRenderTargetView(pRTV, ClearColor);
	ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

	bear->move(fElapsedTime);

	//**************************************************************************//
	// Initialize the view matrix.  What you do to the viewer matrix moves the  //
	// viewer, or course.														//
	//																			//
	// The viewer matrix is created every frame here, which looks silly as the	//
	// viewer never moves.  However in general your viewer does move.			//
	//**************************************************************************//
	XMVECTOR Eye;
	XMVECTOR At;
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	if (!isBearView) { //Base / 3rd "person" view
		Eye = XMVectorSet(0.0f, 100.0f, 0.0f, 0.0f);
		At = XMVectorSet(0.0f, 1.0f, -0.1f, 0.0f);
	}
	else { //Follow the bear
		Eye = XMVectorSet(bear->getX() - XMVectorGetX(bear->vecRear),
			(bear->getY() - XMVectorGetY(bear->vecRear)) + (cameraYZoom - cameraStabiliser),
			bear->getZ() - XMVectorGetZ(bear->vecRear), 0) * 10;
		At = XMVectorSet(bear->getX(), bear->getY(), bear->getZ(), 0.0f) * 10;
	}

	XMMATRIX matView;
	matView = XMMatrixLookAtLH(Eye,	// The eye, or viewer's position.
		At,		// The look at point.
		Up);	// Which way is up.
	

	//******************************************************************//
	// Create the world matrix for the bear: just a rotate around	    //
	// the Y axis of 45 degrees.  DirectX does all angles in radians,	//
	// hence the conversion.  And a translate.							//
	//******************************************************************//
	XMMATRIX matBearTranslate = XMMatrixTranslation(bear->getX(), bear->getY(), bear->getZ());
	XMMATRIX matBearScale = XMMatrixScaling(bear->getSX(), bear->getSY(), bear->getSZ());
	XMMATRIX matBearWorld = bear->matRotations * matBearTranslate * matBearScale;
	XMMATRIX matWorldViewProjection = matBearWorld * matView * matProjection;

	//******************************************************************//    
	// Update shader variables.  We must update these for every mesh	//
	// that we draw (well, actually we need only update the position	//
	// for each mesh, think hard about this - Nigel						//
	//																	//
	// We pass the parameters to it in a constant buffer.  The buffer	//
	// we define in this module MUST match the constant buffer in the	//
	// shader.															//
	//																	//
	// It would seem that the constant buffer we pass to the shader must//
	// be global, well defined on the heap anyway.  Not a local variable//
	// it would seem.													//
	//******************************************************************//

	//******************************************************************//
	// Lighting.  Ambient light and a light direction, above, to the	//
	// left and two paces back, I think.  Then normalise the light		//
	// vector.  It is kind-a-silly doing this every frame unless the	//
	// light moves.														//
	//******************************************************************//
	XMVECTOR vecLightDirection = XMVectorSet(-1, 1, -2, 0);  // 4th value unused.
	vecLightDirection = XMVector3Normalize(vecLightDirection);

	CB_PS_PER_FRAME CBPerFrame;
	CBPerFrame.vecLight = vecLightDirection;
	CBPerFrame.vecViewer = Eye;
	CBPerFrame.lightDiffuseColour = lDiffuseColour;
	CBPerFrame.lightAmbientColour = lAmbientColour;
	CBPerFrame.lightSpecularColour = lSpecularColour;
	CBPerFrame.materialPower.x = materialShinyness;
	pd3dImmediateContext->UpdateSubresource(g_pcbPSPerFrame, 0, NULL, &CBPerFrame, 0, 0);
	pd3dImmediateContext->PSSetConstantBuffers(1, 1, &g_pcbPSPerFrame);

	CB_PS_PER_OBJECT CBPerObject;
	CBPerObject.m_vObjectColor = XMFLOAT4(1, 1, 1, 1);
	pd3dImmediateContext->UpdateSubresource(g_pcbPSPerObject, 0, NULL, &CBPerObject, 0, 0);
	pd3dImmediateContext->PSSetConstantBuffers(0, 1, &g_pcbPSPerObject);

	pd3dImmediateContext->PSSetSamplers(0, 1, &g_pSamLinear);

	//**************************************************************************//
	// Render the mesh.															//
	//**************************************************************************//
	//bear->prepareRender(pd3dImmediateContext, matView, matProjection);

	pd3dImmediateContext->PSSetShader(g_pPixelShader, NULL, 0);
	prepareRender(pd3dImmediateContext, &meshBear, matBearWorld, matWorldViewProjection, false);

	XMMATRIX matWingScale = XMMatrixScaling(0.5, 0.5, 0.5);

	//Right Wing Creation
	XMMATRIX matRightWingTranslate = XMMatrixTranslation(0.2, 1.7, 0.0);
	XMMATRIX matRightWingRotate = XMMatrixRotationZ(bear->getWingPosition());
	XMMATRIX matRightWingWorld = matRightWingRotate * matRightWingTranslate * matWingScale * matBearWorld;
	XMMATRIX matRightWingWorldViewProjection = matRightWingWorld * matView * matProjection;
	prepareRender(pd3dImmediateContext, &meshWing, matRightWingWorld, matRightWingWorldViewProjection, false);

	//Left Wing Creation
	XMMATRIX matLeftWingTranslate = XMMatrixTranslation(-0.2, 1.7, 0.0);
	XMMATRIX matLeftWingRotate = XMMatrixRotationY(3.14159) * XMMatrixRotationZ(bear->getWingPosition()) * XMMatrixRotationX(3.14159);
	XMMATRIX matLeftWingWorld = matLeftWingRotate * matLeftWingTranslate * matWingScale * matBearWorld;
	XMMATRIX matLeftWingWorldViewProjection = matLeftWingWorld * matView * matProjection;
	prepareRender(pd3dImmediateContext, &meshWing, matLeftWingWorld, matLeftWingWorldViewProjection, false);

	//Floor Creation
	XMMATRIX matFloorScale = XMMatrixScaling(10, 1, 10);
	XMMATRIX matFloorTranslate = XMMatrixTranslation(0.0, 1.0, 0.0);
	XMMATRIX matFloorWorld = matFloorScale * matFloorTranslate;
	XMMATRIX matFloorWorldViewProjection = matFloorWorld * matView * matProjection;
	prepareRender(pd3dImmediateContext, &meshFloor, matFloorWorld, matFloorWorldViewProjection, false);

	//Shadow - bear
	XMVECTOR vecFloor = XMVectorSet(0.0, 1.0, 0.0, 0.0);
	XMMATRIX matShadow = XMMatrixShadow(vecFloor, vecLightDirection);
	XMMATRIX matShadowTranslate = XMMatrixTranslation(bear->getX(), bear->getY() - 0.5, bear->getZ());
	XMMATRIX matShadowWorld = matBearWorld * matShadow * matShadowTranslate;
	XMMATRIX matShadowWorldViewProjection = matShadowWorld * matView * matProjection;
	prepareRender(pd3dImmediateContext, &meshBear, matShadowWorld, matShadowWorldViewProjection, true);

	//Shadow - wings
	XMMATRIX matLeftWingShadowWorld = matLeftWingWorld * matShadow *  matShadowTranslate;
	XMMATRIX matLeftWingShadowWorldViewProjection = matLeftWingShadowWorld * matView * matProjection;
	prepareRender(pd3dImmediateContext, &meshWing, matLeftWingShadowWorld, matLeftWingShadowWorldViewProjection, true);

	XMMATRIX matRightWingShadowWorld = matRightWingWorld * matShadow *  matShadowTranslate;
	XMMATRIX matRightWingShadowWorldViewProjection = matRightWingShadowWorld * matView * matProjection;
	prepareRender(pd3dImmediateContext, &meshWing, matRightWingShadowWorld, matRightWingShadowWorldViewProjection, true);

	//Boids
	updateFlock(pd3dImmediateContext, matView);

	//Skybox
	XMMATRIX matSkyTranslate = XMMatrixTranslation(XMVectorGetX(Eye) * 2, XMVectorGetY(Eye) * 2, XMVectorGetZ(Eye) * 2);
	XMMATRIX matSkyRotate = XMMatrixRotationY(timeGetTime() * worldSpinRate);
	XMMATRIX matSkyScale = XMMatrixScaling(0.5, 0.5, 0.5);
	XMMATRIX matSkyWorld = matSkyRotate * matSkyTranslate * matSkyScale;
	XMMATRIX matSkyWorldViewProjection = matSkyWorld * matView * matProjection;
	pd3dImmediateContext->PSSetShader(pDiffuseShader, NULL, 0);
	prepareRender(pd3dImmediateContext, &meshSky, matSkyWorld, matSkyWorldViewProjection, false);

	//**************************************************************************//
	// Render what is rather grandly called the head up display.				//
	//**************************************************************************//
	DXUT_BeginPerfEvent(DXUT_PERFEVENTCOLOR, L"HUD / Stats");
	g_HUD.OnRender(fElapsedTime);
	g_SampleUI.OnRender(fElapsedTime);
	RenderText();
	DXUT_EndPerfEvent();
}

void spawnFlock() {
	Boid* boid;
	float xStart;
	float zStart;
	float rxStart;

	for (int i = 0; i < flockSize; i++) {
		xStart = spawnX(spawnGen);
		std::uniform_real_distribution<float> spawnZ(-leashLength + fabs(xStart), leashLength - fabs(xStart));
		zStart = spawnZ(spawnGen);
		rxStart = spawnRX(spawnGen);
		boid = new Boid(xStart, 0.0, zStart, rxStart, 0.0, 0.0);
		boid->mesh = &meshBoid;
		flock[i] = boid;
	}
}

void updateFlock(ID3D11DeviceContext *pd3dImmediateContext, const XMMATRIX &matView) {
	XMMATRIX matBoidTranslate;
	XMMATRIX matBoidScale;
	XMMATRIX matBoidWorld;
	XMMATRIX matBoidWorldViewProjection;
	Boid* boid;

	for (int i = 0; i < flockSize; i++) {
		boid = flock[i];
		matBoidTranslate = XMMatrixTranslation(boid->getX(), boid->getY(), boid->getZ());
		matBoidScale = XMMatrixScaling(boid->getSX(), boid->getSY(), boid->getSZ());
		matBoidWorld = boid->matRotations * matBoidTranslate * matBoidScale;
		matBoidWorldViewProjection = matBoidWorld * matView * matProjection;
		prepareRender(pd3dImmediateContext, boid->mesh, matBoidWorld, matBoidWorldViewProjection, false);
	}
}

void prepareRender(ID3D11DeviceContext *pd3dImmediateContext, CDXUTSDKMesh *toRender,
	const XMMATRIX &matWorld, const XMMATRIX &matWorldViewProjection, bool isShadow) {
//	XMMATRIX matWorld;
//	XMMATRIX matWorldViewProjection;
	CB_VS_PER_OBJECT CBMatrices;
	CBMatrices.matWorld = XMMatrixTranspose(matWorld);
	CBMatrices.matWorldViewProj = XMMatrixTranspose(matWorldViewProjection);
	pd3dImmediateContext->UpdateSubresource(g_pcbVSPerObject, 0, NULL, &CBMatrices, 0, 0);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &g_pcbVSPerObject);
	RenderMesh(pd3dImmediateContext, toRender, isShadow);
}

//**************************************************************************//
// Render a CDXUTSDKMesh, using the Device Context specified.				//
//**************************************************************************//
void RenderMesh(ID3D11DeviceContext *pContext, CDXUTSDKMesh *toRender, bool isShadow)
{
	//Get the mesh
	//IA setup
	pContext->IASetInputLayout(g_pVertexLayout11);
	UINT Strides[1];
	UINT Offsets[1];
	ID3D11Buffer* pVB[1];
	pVB[0] = toRender->GetVB11(0, 0);
	Strides[0] = (UINT)toRender->GetVertexStride(0, 0);
	Offsets[0] = 0;
	pContext->IASetVertexBuffers(0, 1, pVB, Strides, Offsets);
	pContext->IASetIndexBuffer(toRender->GetIB11(0), toRender->GetIBFormat11(0), 0);

	SDKMESH_SUBSET* pSubset = NULL;
	ID3D11ShaderResourceView* pDiffuseRV = NULL;

	// Set the shaders
	pContext->VSSetShader(g_pVertexShader, NULL, 0);

	for (UINT subset = 0; subset < toRender->GetNumSubsets(0); ++subset)
	{
		//Render
		D3D11_PRIMITIVE_TOPOLOGY PrimType;

		// Get the subset
		pSubset = toRender->GetSubset(0, subset);

		PrimType = CDXUTSDKMesh::GetPrimitiveType11((SDKMESH_PRIMITIVE_TYPE)pSubset->PrimitiveType);
		pContext->IASetPrimitiveTopology(PrimType);

		//**************************************************************************//
		// At the moment we load a texture into video memory every frame, which is	//
		// HORRIBLE, we need to create more Texture2D variables.					//
		//**************************************************************************//
		if (!isShadow) {
			pDiffuseRV = toRender->GetMaterial(pSubset->MaterialID)->pDiffuseRV11;
		}

		pContext->PSSetShaderResources(0, 1, &pDiffuseRV);

		pContext->DrawIndexed((UINT)pSubset->IndexCount, 0, (UINT)pSubset->VertexStart);
	}

}

//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext)
{
	g_DialogResourceManager.OnD3D11ReleasingSwapChain();
}

//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice(void* pUserContext)
{
	g_DialogResourceManager.OnD3D11DestroyDevice();
	g_D3DSettingsDlg.OnD3D11DestroyDevice();
	DXUTGetGlobalResourceCache().OnDestroyDevice();

	SAFE_DELETE(g_pTxtHelper);
	SAFE_DELETE(bear);

	meshBear.Destroy();
	meshWing.Destroy();
	meshFloor.Destroy();
	meshSky.Destroy();
	meshBoid.Destroy();

	SAFE_RELEASE(g_pVertexLayout11);
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pIndexBuffer);
	SAFE_RELEASE(g_pVertexShader);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pSamLinear);
	SAFE_RELEASE(pDiffuseShader);
	SAFE_RELEASE(g_pcbVSPerObject);
	SAFE_RELEASE(g_pcbPSPerObject);
	SAFE_RELEASE(g_pcbPSPerFrame);
}

//**************************************************************************//
// Convert an old chracter (char *) string to a WCHAR * string.  There must //
// be something built into Visual Studio to do this for me, but I can't		//
// find it - Nigel.															//
//**************************************************************************//
void charStrToWideChar(WCHAR *dest, char *source)
{
	int length = strlen(source);
	for (int i = 0; i <= length; i++)
		dest[i] = (WCHAR)source[i];
}
