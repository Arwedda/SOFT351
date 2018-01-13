#include "DXUT.h"
#include "DXUTcamera.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include <d3d11.h>
#include "SDKmisc.h"
#include "SDKMesh.h"
#include <xnamath.h>
#include "resource.h"
#include <xnamath.h>
#include <exception>

#ifndef Thing3DH   //Guards, as usual.
#define Thing3DH

//**************************************************************************//
// Many of these member variables are public and have to be set by writing  //
// to the variable.   OO programmers would have us write many more          //
// setThisandThat(...) methods.                                             //
//**************************************************************************//

class Thing3D
{
private:
	float x, y, z;							//Position.
	float rx, ry, rz;						//Rotation (direction)
	float sx, sy, sz;						//Scales in x, y, z directions.
	//float frameTime;						//Used to calculate elapsed times
	XMVECTOR initialDirection;				//Initial direction
public:
	XMVECTOR  currentDir;					//Current direction
	XMMATRIX  matRotations;
	XMMATRIX  matScale;
	XMMATRIX  matTranslate;
	ID3D11Buffer *pcbVSPerObject = NULL;
	ID3D11PixelShader *pPixelShader = NULL;
	ID3D11InputLayout *pVertexLayout11 = NULL;
	ID3D11VertexShader *pVertexShader = NULL;
	CDXUTSDKMesh *mesh = NULL;

	struct CB_VS_PER_OBJECT
	{
		XMMATRIX matWorldViewProjection;
		XMMATRIX matWorld;
	};

	UINT      textureNumber;
public:
	Thing3D();
	Thing3D(float startX, float startY, float startZ, float startRX, float startRY, float startRZ);
	~Thing3D();
	void initialise(ID3D11Buffer *g_pcbVSPerObject, ID3D11PixelShader *g_pPixelShader, CDXUTSDKMesh *g_mesh, ID3D11InputLayout *g_pVertexLayout11, ID3D11VertexShader *g_pVertexShader);
	float getX();
	void setX(float newX);
	float getY();
	void setY(float newX);
	float getZ();
	void setZ(float newX);
	float getRX();
	void setRX(float newX);
	float getRY();
	void setRY(float newX);
	float getRZ();
	void setRZ(float newX);
	float getSX();
	void setSX(float newSX);
	float getSY();
	void setSY(float newSX);
	float getSZ();
	void setSZ(float newSX);
	XMVECTOR getInitialDirection();
	void setInitRX(float initRX);
	void prepareRender(ID3D11DeviceContext *pd3dImmediateContext, const XMMATRIX &matView, const XMMATRIX &matProjection);
	void renderMesh(ID3D11DeviceContext *pContext);
};
#endif
