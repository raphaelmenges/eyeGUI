//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Key implementation with single character on it.

#ifndef CHARACTER_KEY_H_
#define CHARACTER_KEY_H_

#include "Key.h"

namespace eyegui
{
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
            glm::vec4 color,
            glm::vec4 pickColor,
            glm::vec4 iconColor,
			glm::vec4 thresholdColor,
			float threshold,
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
        float mCharacterSize;
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

#endif // CHARACTER_KEY_H_
