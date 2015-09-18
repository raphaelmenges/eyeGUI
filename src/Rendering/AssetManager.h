//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE 
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Managing all kind of assets like shaders, meshes and graphics. Ensures that
// all assets are unique and loaded only one time into memory.

#ifndef ASSET_MANAGER_H_
#define ASSET_MANAGER_H_

#include "Shader.h"
#include "Mesh.h"
#include "Textures/Texture.h"
#include "RenderItem.h"

#include <memory>
#include <map>

namespace eyegui
{
	// Available assets
	namespace shaders { enum class Type { SEPARATOR, BLOCK, PICTURE, CIRCLE_BUTTON, BOX_BUTTON, SENSOR }; }
	namespace meshes { enum class Type { QUAD }; }
	namespace graphics { enum class Type { CIRCLE, NOT_FOUND }; }

	class AssetManager
	{
	public:

		// Constructor
		AssetManager();

		// Destructor
		virtual ~AssetManager();

		// Fetch render item
		RenderItem* fetchRenderItem(shaders::Type shader, meshes::Type mesh);

		// Fetch texture
		Texture* fetchTexture(std::string filepath);

		// Fetch graphics
		Texture* fetchTexture(graphics::Type graphic);

	private:

		// Fetch shader
		Shader* fetchShader(shaders::Type shader);

		// Fetch mesh
		Mesh* fetchMesh(meshes::Type mesh);

		// Members
		std::map<shaders::Type, std::unique_ptr<Shader> > mShaders;
		std::map<meshes::Type, std::unique_ptr<Mesh> > mMeshes;
		std::map<shaders::Type, std::map<meshes::Type, std::unique_ptr<RenderItem> > > mRenderItems;
		std::map<std::string, std::unique_ptr<Texture> > mTextures;
		std::map<graphics::Type, std::unique_ptr<Texture> > mGraphics;
	};
}

#endif // ASSET_MANAGER_H_
