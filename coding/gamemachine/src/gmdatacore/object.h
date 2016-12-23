﻿#ifndef __OBJSTRUCT_H__
#define __OBJSTRUCT_H__
#include "common.h"
#include <vector>
#include "image.h"
#include "utilities/assert.h"
#include "utilities/autoptr.h"

BEGIN_NS

class Object;
class GMGLShaders;
class ObjectPainter
{
public:
	ObjectPainter(Object* obj);

public:
	virtual void init() = 0;
	virtual void draw() = 0;
	virtual void dispose() = 0;

protected:
	Object* getObject();

private:
	Object* m_object;
};

struct Material
{
	GMfloat Ka[3];
	GMfloat Kd[3];
	GMfloat Ks[3];
	GMfloat shininess;
};

class Component
{
	friend Object;

public:
	enum
	{
		DefaultEdgesCount = 3,
	};

	Component();
	~Component();

	void setEdgeCountPerPolygon(GMuint edgeCountPerPolygon)
	{
		if (edgeCountPerPolygon > m_edgeCountPerPolygon)
			m_edgeCountPerPolygon = edgeCountPerPolygon;
	}

	GMuint getEdgeCountPerPolygon()
	{
		return m_edgeCountPerPolygon;
	}

	Material& getMaterial()
	{
		return m_material;
	}

	void setOffset(GMuint offset)
	{
		m_offset = offset;
	}

	GMuint getOffset()
	{
		return m_offset;
	}

	GMuint getCount()
	{
		return m_count;
	}

	GMint* getFirstPtr()
	{
		return m_firstPtr;
	}

	GMint* getCountPtr()
	{
		return m_countPtr;
	}

	GMuint getPolygonCount()
	{
		return m_count / m_edgeCountPerPolygon;
	}

	void generatePolygonProperties();

private:
	GMuint m_count;
	GMuint m_offset;
	Material m_material;
	GMuint m_edgeCountPerPolygon;

// 多边形属性
	GMint* m_firstPtr;
	GMint* m_countPtr;
};

class Object
{
public:
	Object();
	~Object();

	void disposeMemory();

	void setPainter(ObjectPainter* painter)
	{
		m_painter.reset(painter);
	}

	ObjectPainter* getPainter()
	{
		ASSERT(m_painter);
		return m_painter;
	}

	void appendComponent(Component* component, GMuint count);

	std::vector<Component*>& getComponents()
	{
		return m_components;
	}

	void setVertices(std::vector<GMfloat>& vertices)
	{
		m_vertices = vertices;
	}

	std::vector<GMfloat>& vao()
	{
		return m_vertices;
	}

	void setNormals(std::vector<GMfloat>& normals)
	{
		m_normals = normals;
	}

	std::vector<GMfloat>& normals()
	{
		return m_normals;
	}

	GMuint getBufferId() { return m_bufferId; }
	GMuint getArrayId() { return m_arrayId; }
	void setBufferId(GMuint id) { m_bufferId = id; }
	void setArrayId(GMuint id) { m_arrayId = id; }

private:
	std::vector<GMfloat> m_vertices;
	std::vector<GMfloat> m_normals;
	GMuint m_arrayId;
	GMuint m_bufferId;
	AutoPtr<ObjectPainter> m_painter;
	std::vector<Component*> m_components;
};

END_NS
#endif