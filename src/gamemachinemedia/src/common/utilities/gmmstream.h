﻿#ifndef __GMMSTREAM_H__
#define __GMMSTREAM_H__

#include <gmmcommon.h>
#include <gmobject.h>
#include <gmtools.h>
#include <atomic>

BEGIN_MEDIA_NS

GM_PRIVATE_CLASS(GMMStream);
class GMMStream : public gm::GMObject
{
	GM_DECLARE_PRIVATE(GMMStream)

public:
	GMMStream();
	~GMMStream();

public:
	void resize(gm::GMsize_t sz);
	void beginWrite();
	void endWrite();
	void rewind();
	bool isFull();
	bool isWriting();
	void fill(gm::GMbyte byte);

public:
	GMMStream& operator <<(gm::GMbyte byte);
	bool read(gm::GMbyte* buffer);
};

END_MEDIA_NS

#endif //__GMMAUDIOPLAYER_H__
