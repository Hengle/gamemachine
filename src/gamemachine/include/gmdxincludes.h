﻿#ifndef __GMDXINCLUDES_H__
#define __GMDXINCLUDES_H__
#	if GM_USE_DX11
#		pragma warning(push)
#		pragma warning(disable:4005)
#		include <d3d11.h>
#		include <D3DX11async.h>
#		include <d3dx10math.h>
#		pragma warning(pop)
#	endif
#endif