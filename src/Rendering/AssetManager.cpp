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
#include "CharacterSets.h"
#include "OperationNotifier.h"
#include "Helper.h"
#include "GUI.h"
#include "Font/AtlasFont.h"
#include "Font/EmptyFont.h"
#include "PathBuilder.h"

#include <algorithm>

namespace eyegui
{
    AssetManager::AssetManager(GUI const * pGUI)
    {
        // Save members
        mpGUI = pGUI;

        // Initialize FreeType Library
        if (FT_Init_FreeType(&mFreeTypeLibrary))
        {
            throwError(
                OperationNotifier::Operation::RUNTIME,
                "Could not initialize FreeType Library");
        }
    }

    AssetManager::~AssetManager()
    {
        // Get rid of fonts first because they have to delete FreeType faces
        mFonts.clear();

        // Close FreeType Library
        FT_Done_FreeType(mFreeTypeLibrary);
    }

    RenderItem const * AssetManager::fetchRenderItem(shaders::Type shader, meshes::Type mesh)
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

    Texture const * AssetManager::fetchTexture(std::string filepath)
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

    Texture const * AssetManager::fetchTexture(graphics::Type graphic)
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

    Font const * AssetManager::fetchFont(std::string filepath)
    {
        // Search for font
        std::unique_ptr<Font>& rupFont = mFonts[filepath];
        Font* pFont = rupFont.get();

        // Create font if not existing
        if (pFont == NULL)
        {
            // Check whether empty font must be created
            if (filepath == EMPTY_STRING)
            {
                // Empty font
                rupFont = std::unique_ptr<Font>(
                    new EmptyFont());

                // Tell the user about this
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "No default font set. Text rendering will not work");
            }
            else
            {
                // Check filepath
                if (!checkFileNameExtension(filepath, "ttf"))
                {
                    throwError(OperationNotifier::Operation::FONT_LOADING, "Font file has unknown format", filepath);
                }

                // Load font to FreeType face
                std::unique_ptr<FT_Face> upFace = std::unique_ptr<FT_Face>(new FT_Face);
                if (FT_New_Face(mFreeTypeLibrary, buildPath(filepath).c_str(), 0, upFace.get()))
                {
                    throwError(OperationNotifier::Operation::FONT_LOADING, "Failed to load font with FreeType Library", filepath);
                }
                else
                {
                    // Decide character set
                    std::set<char16_t> characters = charsets::BASIC;
                    switch (mpGUI->getCharacterSet())
                    {
                    case CharacterSet::GERMANY_GERMAN:
                        characters.insert(charsets::GERMANY_GERMAN.begin(), charsets::GERMANY_GERMAN.end());
                        break;
                    case CharacterSet::US_ENGLISH:
                        // Are there any special characters in us english?
                        break;
                    }

                    // Give face to a font object (it will delete it in the end)
                    rupFont = std::unique_ptr<Font>(
                        new AtlasFont(
                            filepath,
                            std::move(upFace),
                            characters,
                            mpGUI->getWindowHeight()));
                }
            }

            // Get raw pointer to font
            pFont = rupFont.get();

            // Add font to own map
            mFonts[filepath] = std::move(rupFont);
        }

        return pFont;
    }

    void AssetManager::resizeFontAtlases()
    {
        for (auto& rPair : mFonts)
        {
            rPair.second->resizeFontAtlases(mpGUI->getWindowHeight());
        }
    }

    std::unique_ptr<TextFlow> AssetManager::AssetManager::createTextFlow(
        FontSize fontSize,
        TextFlowAlignment alignment,
        TextFlowVerticalAlignment verticalAlignment,
        std::u16string content)
    {
        return std::move(
            std::unique_ptr<TextFlow>(
                new TextFlow(
                    mpGUI,
                    mpGUI->getDefaultFont(),
                    fontSize,
                    alignment,
                    verticalAlignment,
                    fetchShader(shaders::Type::FONT),
                    content)));
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
            case shaders::Type::COLOR:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pColorFragmentShader));
                break;
            case shaders::Type::CIRCLE:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pCircleFragmentShader));
                break;
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
            case shaders::Type::FONT:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pFontFragmentShader));
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
            case meshes::Type::LINE:
                rupMesh = std::unique_ptr<Mesh>(new Mesh(&meshes::lineVertices, &meshes::lineTextureCoordinates));
                break;
            }
            pMesh = rupMesh.get();
            mMeshes[mesh] = std::move(rupMesh);
        }

        return pMesh;
    }
}
