/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#include "pch.h"
#include "CIndirectLighting.h"
#include "TextureManager/CTextureManager.h"

#include "Entity/CEntityHandler.h"
#include "GameObject/CGameObject.h"
#include "RenderMesh/CRenderMesh.h"
#include "Entity/CEntityManager.h"

namespace Skylicht
{
	ACTIVATOR_REGISTER(CIndirectLighting);

	CATEGORY_COMPONENT(CIndirectLighting, "Indirect Lighting", "Indirect Lighting");

	CIndirectLighting::CIndirectLighting() :
		m_type(SH9),
		m_autoSH(true),
		m_internalLM(false)
	{

	}

	CIndirectLighting::~CIndirectLighting()
	{
		if (m_internalLM && m_lightmap)
		{
			CTextureManager::getInstance()->removeTexture(m_lightmap);
		}
	}

	void CIndirectLighting::initComponent()
	{
		m_data.clear();

		CRenderMesh* renderMesh = m_gameObject->getComponent<CRenderMesh>();
		if (renderMesh != NULL)
		{
			CEntityManager* entityMgr = m_gameObject->getEntityManager();
			std::vector<CRenderMeshData*>& renderData = renderMesh->getRenderers();
			for (CRenderMeshData* render : renderData)
			{
				// get entity that have render mesh data
				CEntity* entity = entityMgr->getEntity(render->EntityIndex);
				addLightingData(entity);
			}
		}
		else
		{
			CEntityHandler* entityHandler = m_gameObject->getComponent<CEntityHandler>();
			if (entityHandler != NULL)
			{
				std::vector<CEntity*>& entities = entityHandler->getEntities();
				for (CEntity* entity : entities)
					addLightingData(entity);
			}
			else
			{
				// get entity that have render mesh data
				CEntity* entity = m_gameObject->getEntity();
				addLightingData(entity);
			}
		}
	}

	void CIndirectLighting::startComponent()
	{
		initComponent();
	}

	void CIndirectLighting::addLightingData(CEntity* entity)
	{
		// add indirect data info
		CIndirectLightingData* data = entity->addData<CIndirectLightingData>();

		if (m_type == LightmapArray)
		{
			data->Type = CIndirectLightingData::LightmapArray;
		}
		else
		{
			data->Type = CIndirectLightingData::VertexColor;
		}

		data->SH = m_sh;
		data->AutoSH = &m_autoSH;

		m_data.push_back(data);
	}

	void CIndirectLighting::updateComponent()
	{

	}

	CObjectSerializable* CIndirectLighting::createSerializable()
	{
		CObjectSerializable* object = CComponentSystem::createSerializable();

		CEnumProperty<EIndirectType>* enumType = new CEnumProperty<EIndirectType>(object, "type", m_type);
		enumType->addEnumString("Lightmap", EIndirectType::LightmapArray);
		enumType->addEnumString("Vertex Color", EIndirectType::VertexColor);
		enumType->addEnumString("SH9", EIndirectType::SH9);
		object->autoRelease(enumType);

		CArrayTypeSerializable<CFilePathProperty>* textureArray = new CArrayTypeSerializable<CFilePathProperty>("LMTextures", object);
		textureArray->OnCreateElement = [](CValueProperty* element)
		{
			CFilePathProperty* fileProperty = dynamic_cast<CFilePathProperty*>(element);
			fileProperty->Exts.push_back("tga");
			fileProperty->Exts.push_back("png");
		};
		object->autoRelease(textureArray);

		// lightmap path
		textureArray->resize((int)m_lightmapPaths.size());
		for (u32 i = 0, n = (u32)m_lightmapPaths.size(); i < n; i++)
		{
			CFilePathProperty* fileProperty = dynamic_cast<CFilePathProperty*>(textureArray->getElement(i));
			fileProperty->set(m_lightmapPaths[i]);
		}

		return object;
	}

	void CIndirectLighting::loadSerializable(CObjectSerializable* object)
	{
		CComponentSystem::loadSerializable(object);

		bool lightmapChanged = false;

		EIndirectType type = object->get<EIndirectType>("type", EIndirectType::SH9);

		CArraySerializable* textureArray = (CArraySerializable*)object->getProperty("LMTextures");
		if (textureArray != NULL)
		{
			std::vector<std::string> old = m_lightmapPaths;
			m_lightmapPaths.clear();

			int count = textureArray->getElementCount();
			for (int i = 0; i < count; i++)
			{
				std::string path = textureArray->getElementValue<std::string>(i, std::string());
				m_lightmapPaths.push_back(path);
			}

			if (!isLightmapEmpty())
				lightmapChanged = isLightmapChanged(old);
		}

		setIndirectLightingType(type, lightmapChanged);
	}

	bool CIndirectLighting::isLightmapEmpty()
	{
		for (std::string& s : m_lightmapPaths)
		{
			if (s.empty())
				return true;
		}
		return false;
	}

	bool CIndirectLighting::isLightmapChanged(const std::vector<std::string>& paths)
	{
		if (paths.size() != m_lightmapPaths.size())
			return true;

		for (int i = 0, n = (int)paths.size(); i < n; i++)
		{
			if (paths[i] != m_lightmapPaths[i])
			{
				return true;
			}
		}

		return false;
	}

	void CIndirectLighting::setSH(core::vector3df* sh)
	{
		m_autoSH = false;
		for (int i = 0; i < 9; i++)
			m_sh[i] = sh[i];
	}

	void CIndirectLighting::setLightmap(ITexture* texture)
	{
		if (m_internalLM && m_lightmap)
			CTextureManager::getInstance()->removeTexture(m_lightmap);

		m_lightmap = texture;
		m_internalLM = false;
	}

	void CIndirectLighting::setAutoSH(bool b)
	{
		m_autoSH = b;

		for (CIndirectLightingData* data : m_data)
		{
			data->AutoSH = &m_autoSH;
			data->Init = true;
		}
	}

	void CIndirectLighting::setIndirectLightingType(EIndirectType type, bool loadLightmap)
	{
		m_type = type;

		for (CIndirectLightingData* data : m_data)
		{
			if (m_type == LightmapArray)
			{
				// Load lightmap texture array
				if (m_lightmapPaths.size() > 0 && loadLightmap)
				{
					if (m_internalLM && m_lightmap)
						CTextureManager::getInstance()->removeTexture(m_lightmap);

					m_lightmap = CTextureManager::getInstance()->getTextureArray(m_lightmapPaths);
					m_internalLM = true;
				}

				data->Type = CIndirectLightingData::LightmapArray;
				data->LightmapTexture = m_lightmap;
			}
			else if (m_type == SH9)
			{
				data->Type = CIndirectLightingData::SH9;
				data->SH = m_sh;
			}
			else
			{
				data->Type = CIndirectLightingData::VertexColor;
			}
		}
	}
}