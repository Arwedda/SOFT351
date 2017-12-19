#include "DXUT.h"
#include "Thing3D.h";

Thing3D::Thing3D() {
	x = 0.0;
	y = 0.0;
	z = 0.0;
	rx = -2.0;
	ry = 0.0;
	rz = 0.0;
	sx = 10.0;
	sy = 10.0;
	sz = 10.0;
	//frameTime = 0;
	initialDirection = XMVectorSet(0, 0, -2, 0);
	currentDir = XMVectorSet(0, 0, -2, 0);
	matRotations = XMMatrixRotationRollPitchYaw(0, 0, 0);
	matScale = XMMatrixScaling(sx, sy, sz);
	matTranslate = XMMatrixTranslation(x, y, z);
	textureNumber = 0;
}

Thing3D::Thing3D(float setX, float setY, float setZ) {
	x = setX;
	y = setY;
	z = setZ;
	rx = -2.0;
	ry = 0.0;
	rz = 0.0;
	sx = 10.0;
	sy = 10.0;
	sz = 10.0;
	//frameTime = 0;
	initialDirection = XMVectorSet(0, 0, -2, 0);
	currentDir = XMVectorSet(0, 0, -2, 0);
	matRotations = XMMatrixRotationRollPitchYaw(0, 0, 0);
	matScale = XMMatrixScaling(sx, sy, sz);
	matTranslate = XMMatrixTranslation(x, y, z);
	textureNumber = 0;
}

Thing3D::~Thing3D() {
	if (pcbVSPerObject)
	SAFE_DELETE(pcbVSPerObject);
	if (pPixelShader)
	SAFE_DELETE(pPixelShader);
	if (pVertexLayout11)
	SAFE_DELETE(pVertexLayout11);
	if (pVertexShader)
	SAFE_DELETE(pVertexShader);
	if (mesh)
	SAFE_DELETE(mesh);
}

void Thing3D::initialise(ID3D11Buffer *g_pcbVSPerObject, ID3D11PixelShader *g_pPixelShader, CDXUTSDKMesh *g_mesh,
	ID3D11InputLayout *g_pVertexLayout11, ID3D11VertexShader *g_pVertexShader) {
	*pcbVSPerObject = *g_pcbVSPerObject;
	*pPixelShader = *g_pPixelShader;
	*mesh = *g_mesh;
	*pVertexLayout11 = *g_pVertexLayout11;
	*pVertexShader = *g_pVertexShader;
}

float Thing3D::getX() {
	return x;
}

void Thing3D::setX(float newX) {
	x = newX;
}

float Thing3D::getY() {
	return y;
}

void Thing3D::setY(float newY) {
	y = newY;
}

float Thing3D::getZ() {
	return z;
}

void Thing3D::setZ(float newZ) {
	z = newZ;
}

float Thing3D::getRX() {
	return rx;
}

void Thing3D::setRX(float newRX) {
	rx = newRX;
}

float Thing3D::getRY() {
	return ry;
}

void Thing3D::setRY(float newRY) {
	ry = newRY;
}

float Thing3D::getRZ() {
	return rz;
}

void Thing3D::setRZ(float newRZ) {
	rz = newRZ;
}

float Thing3D::getSX() {
	return sx;
}

float Thing3D::getSY() {
	return sy;
}

float Thing3D::getSZ() {
	return sz;
}

XMVECTOR Thing3D::getInitialDirection() {
	return initialDirection;
}

void Thing3D::prepareRender(ID3D11DeviceContext *pd3dImmediateContext, const XMMATRIX &matView, const XMMATRIX &matProjection) {
	XMMATRIX matWorld = matRotations * matTranslate * matScale;
	XMMATRIX matWorldViewProjection = matWorld * matView * matProjection;

	CB_VS_PER_OBJECT CBMatrices;
	CBMatrices.matWorld = XMMatrixTranspose(matWorld);
	CBMatrices.matWorldViewProjection = XMMatrixTranspose(matWorldViewProjection);
	pd3dImmediateContext->UpdateSubresource(pcbVSPerObject, 0, NULL, &CBMatrices, 0, 0);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, &pcbVSPerObject);
	pd3dImmediateContext->PSSetShader(pPixelShader, NULL, 0);
	renderMesh(pd3dImmediateContext);
}

void Thing3D::renderMesh(ID3D11DeviceContext *pContext) {
	//Get the mesh
	//IA setup
	pContext->IASetInputLayout(pVertexLayout11);
	UINT Strides[1];
	UINT Offsets[1];
	ID3D11Buffer* pVB[1];
	pVB[0] = mesh->GetVB11(0, 0);
	Strides[0] = (UINT)mesh->GetVertexStride(0, 0);
	Offsets[0] = 0;
	pContext->IASetVertexBuffers(0, 1, pVB, Strides, Offsets);
	pContext->IASetIndexBuffer(mesh->GetIB11(0), mesh->GetIBFormat11(0), 0);

	// Set the shaders
	pContext->VSSetShader(pVertexShader, NULL, 0);

	for (UINT subset = 0; subset < mesh->GetNumSubsets(0); ++subset)
	{
		//Render
		SDKMESH_SUBSET* pSubset = NULL;
		D3D11_PRIMITIVE_TOPOLOGY PrimType;

		// Get the subset
		pSubset = mesh->GetSubset(0, subset);

		PrimType = CDXUTSDKMesh::GetPrimitiveType11((SDKMESH_PRIMITIVE_TYPE)pSubset->PrimitiveType);
		pContext->IASetPrimitiveTopology(PrimType);

		//**************************************************************************//
		// At the moment we load a texture into video memory every frame, which is	//
		// HORRIBLE, we need to create more Texture2D variables.					//
		//**************************************************************************//
		ID3D11ShaderResourceView* pDiffuseRV = mesh->GetMaterial(pSubset->MaterialID)->pDiffuseRV11;
		pContext->PSSetShaderResources(0, 1, &pDiffuseRV);

		pContext->DrawIndexed((UINT)pSubset->IndexCount, 0, (UINT)pSubset->VertexStart);
	}
}