// XFileUtil.cpp: implementation of the CXFileUtil class.
//
//////////////////////////////////////////////////////////////////////
#include "d3dx9.h"
#include "XFileUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXFileUtil::CXFileUtil()
{
    g_pMesh = NULL;                // �޽� ��ü 
	g_pMeshMaterials = NULL;       // �Ž��� ���� ���� 
	g_pMeshTextures = NULL;        // �Ž��� ���� �ؽ��� 
	g_dwNumMaterials = NULL;       // �Ž� ������ ���� 
}


CXFileUtil::~CXFileUtil()
{
	g_pMesh->Release();
}

int CXFileUtil::XFileLoad(LPDIRECT3DDEVICE9 pD3DDevice, char *xFileName)
{
    LPD3DXBUFFER pD3DXMtrlBuffer;

    // x������ �ε��Ѵ�.
    if( FAILED( D3DXLoadMeshFromX( xFileName, D3DXMESH_SYSTEMMEM, 
                                   pD3DDevice, NULL, 
                                   &pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, 
                                   &g_pMesh ) ) )
    {
            MessageBox(NULL, "X���� �ε� ����", "�޽��ε� ����", MB_OK);
            return E_FAIL;

    }
	
	if (!(g_pMesh->GetFVF() & D3DFVF_NORMAL)) {

		ID3DXMesh* pTempMesh;
		g_pMesh->CloneMeshFVF(g_pMesh->GetOptions(),
			g_pMesh->GetFVF() | D3DFVF_NORMAL,
			pD3DDevice, &pTempMesh);
		D3DXComputeNormals(pTempMesh, NULL);
		g_pMesh->Release();
		g_pMesh = pTempMesh; // ��ü
	}
	
	// �ؽ��� ������ �ٸ� ������ ���� ��츦 ���Ͽ� �ؽ��� �н� ��ġ ��� 
	char texturePath[256];
	if (strchr(xFileName, '/')==NULL) // ���� ������ ��� 
		wsprintf(texturePath, "./");
	else {                            // ��Ÿ ������ ���
		char temp[256], *pChar;
		strcpy(temp, xFileName);
		_strrev(temp);
		pChar = strchr(temp, '/');
		strcpy(texturePath, pChar);
		_strrev(texturePath);
	}
	

    /// x  ���� �ε� �ڵ�
    D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
    g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
    g_pMeshTextures  = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];

    for( DWORD i=0; i<g_dwNumMaterials; i++ )
    {
        // ���� ���� 
        g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;
		// ���� ���� ����
		//g_pMeshMaterials[i].Diffuse.r = g_pMeshMaterials[i].Diffuse.g = 0.3;

        // ������ ���� �ں��Ʈ ���� ����  (D3DX �� ������ �����Ƿ�)
        g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;

        g_pMeshTextures[i] = NULL;
		// �ؽ��� ������ �����ϴ� ��� 
        if( d3dxMaterials[i].pTextureFilename != NULL &&   
            lstrlen(d3dxMaterials[i].pTextureFilename) > 0 )
        {
            // ��� + �ؽ��� ���� �̸� �����
			char tempFile[256];
			wsprintf(tempFile, "%s%s", texturePath, d3dxMaterials[i].pTextureFilename);
			// �ؽ��� ���� 
            if( FAILED( D3DXCreateTextureFromFile( pD3DDevice, 
                                                tempFile, 
                                                &g_pMeshTextures[i] ) ) )
            {
                    //MessageBox(NULL, "Could not find texture map", "Meshes.exe", MB_OK);
				    //return E_FAIL;
				    g_pMeshTextures[i] = NULL;
            }
        }
    }

    // ���� ���� ��볡 & ���� 
    pD3DXMtrlBuffer->Release();
    return S_OK;
}


int CXFileUtil::XFileDisplay(LPDIRECT3DDEVICE9 pD3DDevice)
{
	// �޽� ���
	for( DWORD i=0; i<g_dwNumMaterials; i++ )
	{
		// ���� Sub Set �� ���� ���� ���� 
		pD3DDevice->SetMaterial( &g_pMeshMaterials[i] );
		// ���� Sub Set �� ���� �ؽ��� ����
		pD3DDevice->SetTexture( 0, g_pMeshTextures[i] );

		// Draw the mesh subset
		g_pMesh->DrawSubset( i );
	}

	pD3DDevice->SetTexture( 0, NULL );

	return 0;
}
