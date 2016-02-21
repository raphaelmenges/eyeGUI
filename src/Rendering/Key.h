//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Keys for keyboard element. Created by asset manager.

#ifndef KEY_H_
#define KEY_H_

#include "Font/Font.h"
#include "Shader.h"
#include "RenderItem.h"
#include "LerpValue.h"
#include "externals/OpenGLLoader/gl_core_3_3.h"

namespace eyegui
{
    // Forward declaration
    class Layout;
    class AssetManager;

    // Abstract superclass of key
    class Key
    {
    public:

        // Constructor
        Key(Layout const * pLayout, AssetManager* pAssetManager);

		// Copy constructor
		Key(const Key& rOtherKey);

        // Destructor
        virtual ~Key() = 0;

		// Simple transform and size to fit layout size
		virtual void transformAndSize();

        // Set position of center and size of key
        virtual void transformAndSize(int x, int y, int size);

        // Update
        virtual void update(float tpf);

        // Draw (stencil values in eyeGUI coordinate system)
        virtual void draw(
            int stencilX,
            int stencilY,
            int stencilWidth,
            int stencilHeight,
            glm::vec4 color,
			glm::vec4 selectionColor,
            glm::vec4 iconColor,
            float alpha) const = 0;

        // Reset
        virtual void reset();

        // Set focus
        void setFocus(bool doFocus);

        // Get focus
        bool isFocused() const;

		// Get value of focus
		float getFocusValue() const;

		// Select
		void setSelect(bool doSelect);

		// Get select
		bool isSelected() const;

        // Get position on GUI
        glm::vec2 getPosition() const;

		// Get size in pixels
		int getSize() const;

        // Get value
        virtual std::u16string getValue() const = 0;

    protected:

        // Draw circle (stencil values already in OpenGL coordinates)
        void drawCircle(
            int oglStencilX,
            int oglSencilY,
            int oglStencilWidth,
            int oglStencilHeight,
            glm::vec4 color,
			glm::vec4 selectionColor,
            float alpha) const;

        // Members
        int mX;
        int mY;
        int mSize;
        Layout const * mpLayout;
        AssetManager* mpAssetManager;
        RenderItem const * mpCirlceRenderItem;
        bool mFocused;
        LerpValue mFocus;
		bool mSelected;
		LerpValue mSelection;
        glm::mat4 mCircleMatrix;
    };

    // Key with character
    class CharacterKey : public Key
    {
    public:

        // Constructor
        CharacterKey(
            Layout const * pLayout,
            AssetManager* pAssetManager,
            Font const * pFont,
            char16_t character);

		// Copy constructor
		CharacterKey(const CharacterKey& rOtherKey);

        // Destructor
        virtual ~CharacterKey();

		// Simple transform and size to fit layout size
		virtual void transformAndSize();

        // Draw (stencil values in eyeGUI coordinate system)
        virtual void draw(
            int stencilX,
            int stencilY,
            int stencilWidth,
            int stencilHeight,
            glm::vec4 color,
			glm::vec4 selectionColor,
            glm::vec4 iconColor,
            float alpha) const;

        // Get value
        virtual std::u16string getValue() const;

    private:

		// Prepare quad for displaying the character
		void prepareQuad();

        // Members
        Font const * mpFont;
        char16_t mCharacter;
        Glyph const * mpGlyph;
        Shader const * mpQuadShader;
        GLuint mQuadVertexBuffer;
        GLuint mQuadTextureCoordinateBuffer;
        GLuint mQuadVertexArrayObject;
        glm::mat4 mQuadMatrix;
    };

    // Key with graphic (not used nor implemented)
    class GraphicKey : public Key
    {
    public:

        // Constructor
        GraphicKey(
            Layout const * pLayout,
            AssetManager* pAssetManager);

        // Destructor
        virtual ~GraphicKey();

        // Could share the quad stuff with character key...
    };
}

#endif // KEY_H_
