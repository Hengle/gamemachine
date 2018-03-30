﻿#include "stdafx.h"
#include "gmdx11texture.h"
#include <gamemachine.h>
#include "gmdx11graphic_engine.h"
#include "gmdx11helper.h"

namespace
{
	inline DXGI_FORMAT toDxgiFormat(GMImageFormat format)
	{
		switch (format)
		{
		case GMImageFormat::RGBA:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case GMImageFormat::BGRA:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
		default:
			GM_ASSERT(false);
			return DXGI_FORMAT_UNKNOWN;
		}
	}

	D3D11_TEXTURE_ADDRESS_MODE getAddressMode(GMS_Wrap wrapMode)
	{
		switch (wrapMode)
		{
		case GMS_Wrap::CLAMP_TO_BORDER:
			return D3D11_TEXTURE_ADDRESS_BORDER;
		case GMS_Wrap::CLAMP_TO_EDGE:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		case GMS_Wrap::REPEAT:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		case GMS_Wrap::MIRRORED_REPEAT:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		default:
			GM_ASSERT(false);
			return D3D11_TEXTURE_ADDRESS_WRAP;
		}
	}
}

GMDx11Texture::GMDx11Texture(GMImage* image)
{
	D(d);
	d->image = image;
	GM.getGraphicEngine()->getInterface(GameMachineInterfaceID::D3D11Device, (void**)&d->device);
	GM_ASSERT(d->device);

	GM.getGraphicEngine()->getInterface(GameMachineInterfaceID::D3D11DeviceContext, (void**)&d->deviceContext);
	GM_ASSERT(d->deviceContext);

	init();
}

GMDx11Texture::~GMDx11Texture()
{
}

void GMDx11Texture::drawTexture(GMTextureFrames* frames, GMint textureIndex)
{
	D(d);
	if (!d->samplerState)
	{
		// 创建采样器
		D3D11_SAMPLER_DESC desc = GMDx11Helper::GMGetDx11DefaultSamplerDesc();
		desc.Filter = GMDx11Helper::GMGetDx11Filter(frames->getMinFilter(), frames->getMagFilter());
		desc.AddressU = getAddressMode(frames->getWrapS());
		desc.AddressV = getAddressMode(frames->getWrapT());
		GM_DX_HR(d->device->CreateSamplerState(&desc, &d->samplerState));
	}
	
	GM_ASSERT(d->samplerState);
	d->deviceContext->PSSetShaderResources(textureIndex, 1, &d->shaderResourceView);
	d->deviceContext->PSSetSamplers(textureIndex, 1, &d->samplerState);
}

void GMDx11Texture::init()
{
	D(d);
	auto& imageData = d->image->getData();
	if (imageData.target == GMImageTarget::Texture2D ||
		imageData.target == GMImageTarget::CubeMap)
	{
		GMComPtr<ID3D11Texture2D> texture;
		D3D11_SUBRESOURCE_DATA* resourceData = new D3D11_SUBRESOURCE_DATA[imageData.mipLevels * imageData.slices];

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = d->image->getWidth();
		desc.Height = d->image->getHeight();
		desc.MipLevels = imageData.mipLevels;
		desc.ArraySize = imageData.slices;
		desc.Format = toDxgiFormat(imageData.format);
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		GMint index = 0;
		for (GMuint i = 0; i < desc.ArraySize; ++i)
		{
			for (GMuint j = 0; j < desc.MipLevels; ++j, ++index)
			{
				GMuint pitch = d->image->getWidth(j);
				resourceData[index].pSysMem = imageData.mip[j].data;
				resourceData[index].SysMemPitch = pitch * d->image->getData().channels;
			}
		}

		GM_DX_HR(d->device->CreateTexture2D(&desc, resourceData, &texture));
		d->resource = texture;

		GM_delete_array(resourceData);
	}
	else
	{
		GM_ASSERT(false);
		// 目前先只支持2D吧，之后再加
	}

	GM_ASSERT(d->resource);
	GM_DX_HR(d->device->CreateShaderResourceView(
		d->resource,
		NULL,
		&d->shaderResourceView
	));
}