//
// 2018.12.1 Sat
// Created by Weslse
//
// TODO :
// �籸 ť�� �߰�
// Torque ����
//

#include <windows.h>
#include "d3dx9.h"
#include "XFileUtil.h"
#include "PhysicalObj.h"
#include "Camera.h"

//-----------------------------------------------------------------------------
// Function Prototype
VOID SetupViewProjection();

// ���� ���� 
ZCamera*				g_pCamera = NULL;	// Camera Ŭ����
LPDIRECT3D9             g_pD3D = NULL; // Direct3D ��ü 
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // ������ ��ġ (����ī��)

LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL; // ���ؽ� ����
PDIRECT3DVERTEXBUFFER9  g_pVBTexture = NULL; // �ؽ��� ��¿���ؽ� ����
LPDIRECT3DTEXTURE9      g_pTexture = NULL; // �ؽ��� �ε��� ����

LPD3DXEFFECT			pEffect = NULL;	//����Ʈ

CXFileUtil				g_XOrangeBall;
CXFileUtil				g_XWhiteBall;
CXFileUtil				g_XRedBall;

PhysicalObj				g_PhyOrangeBall;
PhysicalObj				g_PhyWhiteBall;
PhysicalObj				g_PhyRedBall;

// Ŀ���� ���ؽ� Ÿ�� ����ü 
struct CUSTOMVERTEX
{
	FLOAT x, y, z;    // 3D ��ǥ��
	DWORD color;      // ���ؽ� ����
};

// Ŀ���� ���ؽ��� ������ ǥ���ϱ� ���� FVF(Flexible Vertex Format) �� 
// D3DFVF_XYZ(3D ���� ��ǥ) ��  D3DFVF_DIFFUSE(���� ����) Ư���� ��������.
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

// �ؽ��� ��ǥ�� ������ ���ؽ� ����ü ����
struct TEXTUREVERTEX
{
	D3DXVECTOR3     position;  // ���ؽ��� ��ġ
	D3DCOLOR        color;     // ���ؽ��� ����
	FLOAT           tu, tv;    // �ؽ��� ��ǥ 
};

// �� ����ü�� ������ ǥ���ϴ� FVF �� ����
#define D3DFVF_TEXTUREVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

D3DXMATRIX matWorld;
D3DXMATRIX matView;
D3DXMATRIX matProj;

//-----------------------------------------------------------------------------
// �̸�: InitD3D()
// ���: Direct3D �ʱ�ȭ 
//-----------------------------------------------------------------------------
HRESULT InitD3D(HWND hWnd)
{
	// Direct3D ��ü ���� 
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	// ��ġ ������ ����Ÿ �غ�

	D3DPRESENT_PARAMETERS d3dpp;         // ��ġ ������ ���� ����ü ���� ����

	ZeroMemory(&d3dpp, sizeof(d3dpp));                  // ����ü Ŭ����
	d3dpp.BackBufferWidth = 1024;               // ���� �ػ� ���� ����
	d3dpp.BackBufferHeight = 768;               // ���� �ػ� ���� ���� 
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;   // ���� ���� ���� 
	d3dpp.BackBufferCount = 1;                 // ����� �� 
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;  // ���� ��� ����
	d3dpp.hDeviceWindow = hWnd;              // ������ �ڵ� ���� 
	d3dpp.Windowed = true;              // ������ ���� ���� �ǵ��� �� 
	d3dpp.EnableAutoDepthStencil = true;              // ���Ľ� ���۸� ����ϵ��� �� 
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;      // ���Ľ� ���� ���� ���� 


	// D3D��ü�� ��ġ ���� �Լ� ȣ�� (����Ʈ ����ī�� ���, HAL ���)

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	// ���� ��ġ�� ���������� �����Ǿ���.

	// zbuffer ����ϵ��� ����
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	return S_OK;
}


//-----------------------------------------------------------------------------
// �̸�: InitMeshData()
// ���: Mesh�� ���õ� ���� �����͸� �ʱ�ȭ �Ѵ�. 
//-----------------------------------------------------------------------------
HRESULT InitMeshData()
{
	g_XOrangeBall.XFileLoad(g_pd3dDevice, "./images/ball.x");
	g_XWhiteBall.XFileLoad(g_pd3dDevice, "./images/ball.x");
	g_XRedBall.XFileLoad(g_pd3dDevice, "./images/ball.x");
	D3DXVECTOR3 min, max;
	D3DXVECTOR3 center;
	float radius;


	BYTE* v = 0;

	g_XOrangeBall.GetMesh()->LockVertexBuffer(0, (void**)&v);
	HRESULT hr = D3DXComputeBoundingBox(
		(D3DXVECTOR3*)v,
		g_XOrangeBall.GetMesh()->GetNumVertices(),
		D3DXGetFVFVertexSize(g_XOrangeBall.GetMesh()->GetFVF()),
		&min,
		&max);
	hr = D3DXComputeBoundingSphere(
		(D3DXVECTOR3*)v,
		g_XOrangeBall.GetMesh()->GetNumVertices(),
		D3DXGetFVFVertexSize(g_XOrangeBall.GetMesh()->GetFVF()),
		&center,
		&radius);

	g_XOrangeBall.GetMesh()->UnlockVertexBuffer();

	g_PhyOrangeBall.SetBoundingBox(min, max);
	g_PhyOrangeBall.SetBoundingSphere(center, radius);
	g_PhyOrangeBall.SetPosition(100, 0, 100);

	// OrangeBall�� ball2�� �����ϹǷ� Bounding volume�� ���� ����� �ʿ�� ����
	g_PhyWhiteBall.SetBoundingBox(min, max);
	g_PhyWhiteBall.SetBoundingSphere(center, radius);
	g_PhyWhiteBall.SetPosition(-100, 0, -100);

	g_PhyRedBall.SetBoundingBox(min, max);
	g_PhyRedBall.SetBoundingSphere(center, radius);
	g_PhyRedBall.SetPosition(-50, 0, 70);

	return S_OK;
}

HRESULT InitShader()
{
	HRESULT hr;
	LPD3DXBUFFER pBufferErrors = NULL;
	hr = D3DXCreateEffectFromFile(g_pd3dDevice, "shader.fx", NULL, NULL,
		D3DXSHADER_USE_LEGACY_D3DX9_31_DLL,
		NULL, &pEffect, &pBufferErrors);

	if (FAILED(hr)) {
		LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
		MessageBox(NULL, (LPCSTR)pCompilErrors, "Fx Compile Error",
			MB_OK | MB_ICONEXCLAMATION);
	}

	return S_OK;
}

//-----------------------------------------------------------------------------
// �̸�: InitGeometry()
// ���: ���ؽ� ���۸� ������ �� ���ؽ��� ä���. 
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
	// ���ؽ� ���ۿ� ���� ���ؽ� �ڷḦ �ӽ÷� �����. 
	CUSTOMVERTEX vertices[] =
	{
		{ -200.0f,  0.0f, 0.0f, 0xff00ff00, }, // x�� ������ ���� ���ؽ� 
	{ 200.0f,  0.0f, 0.0f, 0xff00ff00, },

	{ 0.0f, 0.0f, -200.0f, 0xffffff00, },  // z�� ������ ���� ���ؽ�
	{ 0.0f, 0.0f,  200.0f, 0xffffff00, },

	{ 0.0f, -200.0f, 0.0f, 0xffff0000, },  // y�� ������ ���� ���ؽ�
	{ 0.0f,  200.0f, 0.0f, 0xffff0000, },

	{ 0.0f, 50.0f, 0.0f, 0xffff0000, },  // �ﰢ���� ù ��° ���ؽ� 
	{ -50.0f,  0.0f, 0.0f, 0xffff0000, },  // �ﰢ���� �� ��° ���ؽ� 
	{ 50.0f,  0.0f, 0.0f, 0xffff0000, },  // �ﰡ���� �� ��° ���ؽ� 
	};

	// ���ؽ� ���۸� �����Ѵ�.
	// �� ���ؽ��� ������ D3DFVF_CUSTOMVERTEX ��� �͵� ���� 
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(9 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	// ���ؽ� ���ۿ� ���� �� �� ���ؽ��� �ִ´�. 
	VOID* pVertices;
	if (FAILED(g_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, vertices, sizeof(vertices));
	g_pVB->Unlock();

	InitShader();

	return S_OK;
}



//-----------------------------------------------------------------------------
// �̸�: InitGeometryTexture()
// ���: �ؽ��� ����� ���� ���ؽ� ���۸� ������ �� ���ؽ��� ä���. 
//-----------------------------------------------------------------------------
HRESULT InitGeometryTexture()
{
	// �ؽ��� �ε� 
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, "./Images/ball.png", &g_pTexture)))
	{
		return E_FAIL;
	}

	// ���ؽ� ���� ���� 
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(TEXTUREVERTEX), 0,
		D3DFVF_TEXTUREVERTEX, D3DPOOL_DEFAULT, &g_pVBTexture, NULL)))
	{
		return E_FAIL;
	}

	// ���ؽ� ���� ���� 
	TEXTUREVERTEX* pVertices;
	if (FAILED(g_pVBTexture->Lock(0, 0, (void**)&pVertices, 0)))
		return E_FAIL;
	pVertices[0].position = D3DXVECTOR3(-50, 100, 100);  // ���ؽ� ��ġ 
	pVertices[0].color = 0xffffffff;                  // ���ؽ� ���� �� ���� 
	pVertices[0].tu = 0.0f;                        // ���ؽ� U �ؽ��� ��ǥ 
	pVertices[0].tv = 0.0f;                        // ���ؽ� V �ؽ��� ��ǥ 

	pVertices[1].position = D3DXVECTOR3(50, 100, 100);
	pVertices[1].color = 0xffffffff;
	pVertices[1].tu = 1.0f;
	pVertices[1].tv = 0.0f;

	pVertices[2].position = D3DXVECTOR3(-50, 0, 100);
	pVertices[2].color = 0xffffffff;
	pVertices[2].tu = 0.0f;
	pVertices[2].tv = 1.0f;

	pVertices[3].position = D3DXVECTOR3(50, 0, 100);
	pVertices[3].color = 0xffffffff;
	pVertices[3].tu = 1.0f;
	pVertices[3].tv = 1.0f;

	g_pVBTexture->Unlock();
	return S_OK;
}

HRESULT InitMatrix() {
	SetupViewProjection();
	return S_OK;
}

//-----------------------------------------------------------------------------
// �̸�: Cleanup()
// ���: �ʱ�ȭ�Ǿ��� ��� ��ü���� �����Ѵ�. 
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	if (g_pVB != NULL)
		g_pVB->Release();
	if (g_pVBTexture != NULL)
		g_pVBTexture->Release();
	if (g_pTexture != NULL)
		g_pTexture->Release();


	if (g_pd3dDevice != NULL)    // ��ġ ��ü ���� 
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)          // D3D ��ü ���� 
		g_pD3D->Release();
}


//-----------------------------------------------------------------------------
// �̸�: SetupViewProjection()
// ���: �� ��ȯ�� �������� ��ȯ�� �����Ѵ�. 
//-----------------------------------------------------------------------------


VOID SetupViewProjection()
{
	D3DXVECTOR3 vEyePt(0.f, 600.f, 10.f);    // ī�޶��� ��ġ 
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);       // �ٶ󺸴� ���� 
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);          // ������ ���� 
	// �� ��ȯ ���� 
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	// Direct3D ��ġ�� �� ��Ʈ���� ���� 
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	// �������� ��ȯ ���� 
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 1.0f, 1.0f, 1000.0f);
	// Direct3D ��ġ�� �������� ��Ʈ���� ���� 
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	/// ī�޶� �ʱ�ȭ
	g_pCamera->SetView(&vEyePt, &vLookatPt, &vUpVec);
}

// ������ �̸� ���� ������ ���ϴ�.
D3DCOLORVALUE black = { 0, 0, 0, 1 };
D3DCOLORVALUE dark_gray = { 0.2f, 0.2f, 0.2f, 1.0f };
D3DCOLORVALUE gray = { 0.5f, 0.5f, 0.5f, 1.0f };
D3DCOLORVALUE red = { 1.0f, 0.0f, 0.0f, 1.0f };
D3DCOLORVALUE white = { 1.0f, 1.0f, 1.0f, 1.0f };


//-----------------------------------------------------------------------------
// �̸�: Render()
// ���: ȭ���� �׸���.
//-----------------------------------------------------------------------------
VOID Render()
{
	if (NULL == g_pd3dDevice)  // ��ġ ��ü�� �������� �ʾ����� ���� 
		return;

	

	// �ﰢ���� ��/�� ���� ��� �������ϵ��� �ø� ����� ����.
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ������ ����. (���� ���� ������ �̳���, ���ؽ� ��ü�� ������ ����ϵ���)
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ����۸� ������ �������� �����.
	// ����۸� Ŭ����
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	// ȭ�� �׸��� ���� 
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		///// ���ؽ� ��� 
		// ���ؽ� ���� ���� 
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX); // ���ؽ� ���� ���� 
		D3DXMatrixIdentity(&matWorld);

		g_PhyOrangeBall.setModelMatrix(matWorld);
		g_PhyWhiteBall.setModelMatrix(matWorld);
		g_PhyRedBall.setModelMatrix(matWorld);

		
		pEffect->SetMatrix("gWorldMatrix", &matWorld);
		pEffect->SetMatrix("gViewMatrix", &matView);
		pEffect->SetMatrix("gProjectionMatrix", &matProj);

		D3DXVECTOR4 lightPosition(0.f, 500.f, 0.f, 1.f);
		D3DXVECTOR4 cameraPosition = D3DXVECTOR4(*g_pCamera->GetEye(), 1.f);
		pEffect->SetVector("gWorldLightPosition", &lightPosition);
		pEffect->SetVector("gWorldCameraPosition", &cameraPosition);
		pEffect->SetTechnique("Shader");

		//D3DXMATRIXA16 matTransWorld;  // ���� ��ȯ�� ��Ʈ���� ���� 

		for (float x = -200; x <= 200; x += 40) {  // z �࿡ ������ ������ ���� �� �׸��� 
			D3DXMatrixTranslation(&matWorld, x, 0.0, 0.0);  // x�࿡ ���� ��ġ �̵� ��Ʈ����   
			g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld); // ��ȯ��Ʈ���� ���� 
			g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 2, 1);  // z�� ���� �׸��� 
		}

		for (float z = -200; z <= 200; z += 80) {  // x �࿡ ������ ������ ���� �� �׸��� 
			D3DXMatrixTranslation(&matWorld, 0.0, 0.0, z);  // z �࿡ ���� ��ġ �̵� ��Ʈ����
			g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);  // ��ȯ��Ʈ���� ���� 
			g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 0, 1);   // x�� ���� �׸��� 
		}

		D3DXMatrixIdentity(&matWorld);   // ��Ʈ������ ���� ��ķ� ���� 
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);  // ��ȯ ��Ʈ���� ���� 
		g_pd3dDevice->DrawPrimitive(D3DPT_LINELIST, 4, 1);   // y �� �׸��� 

		g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

		// ���׸���

		// ball1
		// setting
		matWorld = g_PhyOrangeBall.getWorldMatrix();
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		pEffect->SetMatrix("gWorldMatrix", &matWorld);
		auto lightColorOrangeBall = D3DXVECTOR4(0.7f, 0.4f, 0.0f, 1.0f);
		pEffect->SetVector("gLightColor", &lightColorOrangeBall);
		// rendering
		pEffect->Begin(NULL, NULL);
		pEffect->BeginPass(0);

		g_XOrangeBall.XFileDisplay(g_pd3dDevice);
		
		pEffect->EndPass();
		pEffect->End();

		// ball2
		// setting
		matWorld = g_PhyWhiteBall.getWorldMatrix();
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		pEffect->SetMatrix("gWorldMatrix", &matWorld);
		auto lightColorWhiteBall = D3DXVECTOR4(0.7f, 0.7f, 0.70f, 1.0f);
		pEffect->SetVector("gLightColor", &lightColorWhiteBall);
		
		//rendering
		pEffect->Begin(NULL, NULL);
		pEffect->BeginPass(0);

		g_XWhiteBall.XFileDisplay(g_pd3dDevice);

		pEffect->EndPass();
		pEffect->End();

		// ball3
		// setting
		matWorld = g_PhyRedBall.getWorldMatrix();
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
		pEffect->SetMatrix("gWorldMatrix", &matWorld);
		auto lightColorRedBall = D3DXVECTOR4(1.f, 0.f, 0.0f, 1.0f);
		pEffect->SetVector("gLightColor", &lightColorRedBall);
		
		//rendering
		pEffect->Begin(NULL, NULL);
		pEffect->BeginPass(0);

		g_XRedBall.XFileDisplay(g_pd3dDevice);

		pEffect->EndPass();
		pEffect->End();

		// ȭ�� �׸��� �� 
		g_pd3dDevice->EndScene();
	}

	// ������� ������ ȭ������ ������. 
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}



//-----------------------------------------------------------------------------
// �̸� : MsgProc()
// ��� : ������ �޽��� �ڵ鷯 
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		Cleanup();   // ���α׷� ����� ��ü ������ ���Ͽ� ȣ���� 
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		Render();    // ȭ�� ����� ����ϴ� ������ �Լ� ȣ�� 
		ValidateRect(hWnd, NULL);
		return 0;

	case WM_CHAR:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void moveCamera(void)
{
	// ī�޶��� �������� ���� ����
	if (GetAsyncKeyState(VK_UP)) 
		g_pCamera->MoveLocalZ(1.5f);	// ī�޶� ����!
	if (GetAsyncKeyState(VK_DOWN)) 
		g_pCamera->MoveLocalZ(-1.5f);	// ī�޶� ����!
	if (GetAsyncKeyState(VK_LEFT)) 
		g_pCamera->MoveLocalX(-1.5f);	// ī�޶� ����
	if (GetAsyncKeyState(VK_RIGHT)) 
		g_pCamera->MoveLocalX(1.5f);	// ī�޶� ������

	if (GetAsyncKeyState('A')) 
		g_pCamera->RotateLocalY(-.02f);
	if (GetAsyncKeyState('D')) 
		g_pCamera->RotateLocalY(.02f);
	if (GetAsyncKeyState('W')) 
		g_pCamera->RotateLocalX(-.02f);
	if (GetAsyncKeyState('S')) 
		g_pCamera->RotateLocalX(.02f);

	matView = *g_pCamera->GetViewMatrix();		// ī�޶� ����� ��´�.
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);			// ī�޶� ��� ����
}

void moveBall() {
	if (GetAsyncKeyState('M')) 
		g_PhyOrangeBall.AddVelocity(0.f, 2.f, 0.f);
	if (GetAsyncKeyState('L')) 
		g_PhyOrangeBall.AddVelocity(-0.7f, 0.f, 0.f);
	if (GetAsyncKeyState('J')) 
		g_PhyOrangeBall.AddVelocity(0.7f, 0.f, 0.f);
	if (GetAsyncKeyState('I')) 
		g_PhyOrangeBall.AddVelocity(0.f, 0.f, -0.7f);
	if (GetAsyncKeyState('K')) 
		g_PhyOrangeBall.AddVelocity(0.f, 0.f, 0.7f);
}

void Action()
{
	static float dt = 1.f;
	
	g_PhyOrangeBall.collide(g_PhyWhiteBall);
	g_PhyOrangeBall.collide(g_PhyRedBall);
	g_PhyWhiteBall.collide(g_PhyRedBall);

	g_PhyOrangeBall.update(dt);
	g_PhyWhiteBall.update(dt);
	g_PhyRedBall.update(dt);

	moveCamera();
	moveBall();
}


//-----------------------------------------------------------------------------
// �̸�: WinMain()
// ���: ���α׷��� ������ 
//-----------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	// ������ Ŭ���� ���� ���� �� ���� 
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		"D3D Game", NULL };
	// ������ Ŭ���� ��� 
	RegisterClassEx(&wc);

	// ������ ���� 
	HWND hWnd = CreateWindow("D3D Game", "D3D Game Program",
		WS_OVERLAPPEDWINDOW, 0, 0, 1024, 768,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	g_pCamera = new ZCamera;

	// Direct3D �ʱ�ȭ�� �����ϸ� �����ϰ�, �����ϸ� �����Ѵ�.
	if (SUCCEEDED(InitD3D(hWnd)) &&       // Direct3D�� �ʱ�ȭ�� �����ϰ� 
		SUCCEEDED(InitGeometry()) &&         // ���ؽ� ���� ������ �����ϰ�
		SUCCEEDED(InitGeometryTexture()) &&  // �ؽ��� ���ؽ� ���� ������ �����ϰ� 
		SUCCEEDED(InitMeshData()) &&
		SUCCEEDED(InitMatrix()))          // ��Ÿ ���� ����Ÿ �ε� 
	{
		// ������ ��� 
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);

		// �޽��� ���� �����ϱ�
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (msg.message != WM_QUIT)
		{
			// �޽��ڰ� ������ ���� �´�. 
			if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				Action();
				InvalidateRect(hWnd, NULL, TRUE);
			}
		}
	}

	delete g_pCamera;
	UnregisterClass("D3D Game", wc.hInstance);
	return 0;
}
