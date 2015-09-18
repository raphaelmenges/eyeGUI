//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE 
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "AssetManager.h"

#include "Textures/PixelTexture.h"
#include "Textures/VectorTexture.h"
#include "Shaders.h"
#include "Meshes.h"
#include "Graphics.h"

#include <algorithm>

namespace eyegui
{
	AssetManager::AssetManager()
	{
		// Nothing to do
	}

	AssetManager::~AssetManager()
	{
		// Nothing to do
	}

	RenderItem* AssetManager::fetchRenderItem(shaders::Type shader, meshes::Type mesh)
	{
		std::unique_ptr<RenderItem> upRenderItem;
		RenderItem* pRenderItem = NULL;

		// Fetch shader and mesh
		Shader* pShader = fetchShader(shader);
		Mesh* pMesh = fetchMesh(mesh);

		// Search in map for render item
		if (mRenderItems.find(shader) != mRenderItems.end())
		{
			if (mRenderItems[shader].find(mesh) != mRenderItems[shader].end())
			{
				// Found it!
				pRenderItem = mRenderItems[shader][mesh].get();
			}
			else
			{
				// Mesh not registered for this shader
				upRenderItem = std::unique_ptr<RenderItem>(new RenderItem(pShader, pMesh));
				pRenderItem = upRenderItem.get();
				mRenderItems[shader][mesh] = std::move(upRenderItem);
			}
		}
		else
		{
			// Shader not registered
			upRenderItem = std::unique_ptr<RenderItem>(new RenderItem(pShader, pMesh));
			pRenderItem = upRenderItem.get();
			std::map<meshes::Type, std::unique_ptr<RenderItem> > innerMap;
			innerMap[mesh] = std::move(upRenderItem);
			mRenderItems.insert(make_pair(shader, std::move(innerMap)));
		}

		return pRenderItem;
	}

	Texture* AssetManager::fetchTexture(std::string filepath)
	{
		// Search in map for texture and create if needed
		std::unique_ptr<Texture>& rupTexture = mTextures[filepath];
		Texture* pTexture = rupTexture.get();

		if (pTexture == NULL)
		{
			// Check for empty string
			if (filepath == "")
			{
				rupTexture = std::unique_ptr<Texture>(new VectorTexture(&graphics::notFoundGraphics, Texture::Filtering::LINEAR, Texture::Wrap::CLAMP));
			}
			else
			{
				// Determine image type
				std::string delimiter = ".";
				std::string input = filepath;
				size_t pos = 0;
				std::string token;
				while ((pos = input.find(delimiter)) != std::string::npos)
				{
					token = input.substr(0, pos);
					input.erase(0, pos + delimiter.length());
				}

				// File format case is not relevant
				std::transform(input.begin(), input.end(), input.begin(), ::tolower);

				// Check token
				if (input.compare("png") == 0)
				{
					rupTexture = std::unique_ptr<Texture>(new PixelTexture(filepath, Texture::Filtering::LINEAR, Texture::Wrap::CLAMP));
				}
				else if (input.compare("svg") == 0)
				{
					rupTexture = std::unique_ptr<Texture>(new VectorTexture(filepath, Texture::Filtering::LINEAR, Texture::Wrap::CLAMP));
				}
				else
				{
					rupTexture = std::unique_ptr<Texture>(new VectorTexture(&graphics::notFoundGraphics, Texture::Filtering::LINEAR, Texture::Wrap::CLAMP));
				}
			}
			pTexture = rupTexture.get();
			mTextures[filepath] = std::move(rupTexture);
		}

		return pTexture;
	}

	Texture* AssetManager::fetchTexture(graphics::Type graphic)
	{
		// Search in map for graphic and create if needed
		std::unique_ptr<Texture>& rupTexture = mGraphics[graphic];
		Texture* pTexture = rupTexture.get();

		if (pTexture == NULL)
		{
			switch (graphic)
			{
			case graphics::Type::CIRCLE:
				rupTexture = std::unique_ptr<Texture>(new VectorTexture(&graphics::circleGraphics, Texture::Filtering::LINEAR, Texture::Wrap::CLAMP));
				break;
			case graphics::Type::NOT_FOUND:
				rupTexture = std::unique_ptr<Texture>(new VectorTexture(&graphics::notFoundGraphics, Texture::Filtering::LINEAR, Texture::Wrap::CLAMP));
				break;
			}
			pTexture = rupTexture.get();
			mGraphics[graphic] = std::move(rupTexture);
		}

		return pTexture;
	}

	Shader* AssetManager::fetchShader(shaders::Type shader)
	{
		// Search in map for shader and create if needed
		std::unique_ptr<Shader>& rupShader = mShaders[shader];
		Shader* pShader = rupShader.get();

		if (pShader == NULL)
		{
			switch (shader)
			{
			case shaders::Type::SEPARATOR:
				rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pSeparatorFragmentShader));
				break;
			case shaders::Type::BLOCK:
				rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pBlockFragmentShader));
				break;
			case shaders::Type::PICTURE:
				rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pPictureFragmentShader));
				break;
			case shaders::Type::CIRCLE_BUTTON:
				rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pCircleButtonFragmentShader));
				break;
			case shaders::Type::BOX_BUTTON:
				rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pBoxButtonFragmentShader));
				break;
			case shaders::Type::SENSOR:
				rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pSensorFragmentShader));
				break;
			}
			pShader = rupShader.get();
			mShaders[shader] = std::move(rupShader);
		}

		return pShader;
	}

	Mesh* AssetManager::fetchMesh(meshes::Type mesh)
	{
		// Search in map for mesh and create if needed
		std::unique_ptr<Mesh>& rupMesh = mMeshes[mesh];
		Mesh* pMesh = rupMesh.get();

		if (pMesh == NULL)
		{
			switch (mesh)
			{
			case meshes::Type::QUAD:
				rupMesh = std::unique_ptr<Mesh>(new Mesh(&meshes::quadVertices, &meshes::quadTextureCoordinates));
				break;
			}
			pMesh = rupMesh.get();
			mMeshes[mesh] = std::move(rupMesh);
		}

		return pMesh;
	}
}
