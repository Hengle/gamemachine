﻿#include "stdafx.h"
#include "bsp_model_loader.h"
#include "foundation/vector.h"
#include "gamepackage.h"
#include "gmengine/gmbspgameworld.h"
#include "foundation/utilities/tinyxml/tinyxml.h"
#include "shader.h"
#include "model.h"
#include "foundation/utilities/utilities.h"

BSPModelLoader::BSPModelLoader()
	: m_world(nullptr)
{
}

BSPModelLoader::~BSPModelLoader()
{
	for (auto iter = m_modelDocs.begin(); iter != m_modelDocs.end(); iter++)
	{
		delete *iter;
	}

	for (auto iter = m_items.begin(); iter != m_items.end(); iter++)
	{
		if ((*iter).second)
			delete (*iter).second;
	}
}

void BSPModelLoader::init(const char* directory, GMBSPGameWorld* world)
{
	m_directory = directory;
	m_world = world;
}

void BSPModelLoader::load()
{
	GamePackage* pk = m_world->getGamePackage();
	AlignedVector<std::string> files = pk->getAllFiles(m_directory.c_str());

	for (auto iter = files.begin(); iter != files.end(); iter++)
	{
		GamePackageBuffer buf;
		pk->readFileFromPath((*iter).c_str(), &buf);
		buf.convertToStringBuffer();
		parse((const char*)buf.buffer);
	}
}

Model* BSPModelLoader::find(const std::string& classname)
{
	ModelMap::iterator iter;
	if ((iter = m_items.find(classname)) == m_items.end())
		return nullptr;

	return ((*iter).second);
}

void BSPModelLoader::parse(const char* data)
{
	TiXmlDocument* doc = new TiXmlDocument();
	if (doc->Parse(data) != 0)
	{
		gm_error("xml load error at %d: %s", doc->ErrorRow(), doc->ErrorDesc());
		delete doc;
		return;
	}

	if (doc->Error())
		return;

	m_modelDocs.push_back(doc);
	TiXmlElement* root = doc->RootElement();
	TiXmlElement* it = root->FirstChildElement();
	for (; it; it = it->NextSiblingElement())
	{
		TiXmlElement* elem = it;
		if (!strEqual(elem->Value(), "item"))
			gm_warning("First node must be 'item'.");

		const char* name = elem->Attribute("name");
		for (TiXmlElement* it = root->FirstChildElement(); it; it = it->NextSiblingElement())
		{
			parseItem(it);
		}
	}
}

void BSPModelLoader::parseItem(TiXmlElement* ti)
{
	const char* classname = ti->Attribute("classname");
	if (!classname)
	{
		gm_error("model file 'classname' attribute missing:");
		return;
	}

	Model* m = new Model;
	m->model[0] = 0;
	strcpy_s(m->classname, classname);

	const char* b;
	if ( (b = ti->Attribute("create")) != nullptr )
	{
		Scanner s(b);
		GMint value;
		s.nextInt(&value);
		m->create = value != 0;
	}

	if ((b = ti->Attribute("extents")) != nullptr)
	{
		Scanner s(b);
		for (GMint i = 0; i < 3; i++)
		{
			s.nextFloat(&m->extents[i]);
		}
	}

	if ((b = ti->Attribute("model")) != nullptr)
	{
		strcpy_s(m->model, b);
	}

	m_items[m->classname] = m;
}