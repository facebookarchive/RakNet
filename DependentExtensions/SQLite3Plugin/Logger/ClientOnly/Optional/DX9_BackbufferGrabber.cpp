#include "DX9_BackbufferGrabber.h"

DX9_BackbufferGrabber::DX9_BackbufferGrabber()
{
	pDestSurface=0;
	pRenderTargetSurface=0;
	deviceUsedToInit=0;
	width=0;
	height=0;
	needsUnlock=false;
}
DX9_BackbufferGrabber::~DX9_BackbufferGrabber()
{
	if (pDestSurface)
		pDestSurface->Release();
	if (pRenderTargetSurface)
		pRenderTargetSurface->Release();
}
void DX9_BackbufferGrabber::InitBackbufferGrabber(LPDIRECT3DDEVICE9 pd3dDevice, int _width, int _height)
{
	if (width==_width && height==_height && pDestSurface && pRenderTargetSurface)
		return;

	if (pDestSurface)
		pDestSurface->Release();
	if (pRenderTargetSurface)
		pRenderTargetSurface->Release();

	width=_width;
	height=_height;
	deviceUsedToInit=pd3dDevice;

	HRESULT hr;
	// KevinJ: Surface to copy to in system memory
	hr = deviceUsedToInit->CreateOffscreenPlainSurface(width, height, D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM, &pDestSurface,NULL);
	if (hr!=S_OK) return;
	// Surface to downsize to
	hr = deviceUsedToInit->CreateRenderTarget(
		width,
		height,
		D3DFMT_A8R8G8B8,
		D3DMULTISAMPLE_NONE,
		0,
		false,
		&pRenderTargetSurface,
		NULL
		);
}
void DX9_BackbufferGrabber::LockBackbufferCopy(RakNet::RGBImageBlob *blob)
{
	LPDIRECT3DDEVICE9 pd3dDevice;
	pd3dDevice=deviceUsedToInit;

	IDirect3DSurface9 * pBackBuffer;
	HRESULT hr;
	hr = deviceUsedToInit->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	hr = deviceUsedToInit->StretchRect(pBackBuffer, NULL, pRenderTargetSurface, NULL, D3DTEXF_NONE);
	hr = deviceUsedToInit->GetRenderTargetData(pRenderTargetSurface,pDestSurface);

	//	RakNet::TimeMS t1 = RakNet::GetTimeMS();
	D3DLOCKED_RECT lockedRect;
	unsigned int videoMemoryDestOffset=0;
	unsigned int pDataOffset=0;
	hr = pDestSurface->LockRect(&lockedRect,0,D3DLOCK_DONOTWAIT|D3DLOCK_READONLY|D3DLOCK_NOSYSLOCK);
	if (hr==D3D_OK)
	{
		blob->data=(unsigned char*)(lockedRect.pBits);
		blob->imageHeight=height;
		blob->imageWidth=width;
		blob->input_components=4;
		blob->linePitch=lockedRect.Pitch;
		needsUnlock=true;
	}
}
void DX9_BackbufferGrabber::ReleaseBackbufferCopy(void)
{
	if (needsUnlock)
	{
		pDestSurface->UnlockRect();
		needsUnlock=false;
	}
}
