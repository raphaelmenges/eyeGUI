//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Internal shaders used for rendering.

#ifndef SHADERS_H_
#define SHADERS_H_

namespace eyegui
{
    namespace shaders
    {
        // Uniforms:
        // mat4 matrix
        static const char* pStaticVertexShader =
            "#version 330 core\n"
            "in vec3 posAttribute;\n"
            "in vec2 uvAttribute;\n"
            "out vec2 uv;\n"
            "uniform mat4 matrix = mat4(1.0);\n"
            "void main() {\n"
            "   uv = uvAttribute;\n"
            "   gl_Position = matrix * vec4(posAttribute, 1);\n"
            "}\n";

        // Uniforms:
        // vec4 color
        // float alpha
        static const char* pColorFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "uniform vec4 color = vec4(1,1,1,1);\n"
            "uniform float alpha = 1;\n"
            "void main() {\n"
            "   fragColor = vec4(color.rgb, color.a * alpha);\n"
            "}\n";

        // Uniforms:
        // vec4 color
        // float alpha
        static const char* pCircleFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform vec4 color = vec4(1,1,1,1);\n"
            "uniform float alpha = 1;\n"
            "void main() {\n"
            "   float gradient = length(2*uv-1);\n" // Simple gradient as base
            "   float circle = (1-gradient) * 75;\n" // Extend gradient to unclamped circle
            "   fragColor = vec4(color.rgb, color.a * alpha * min(circle, 1.0));\n"
            "}\n";

        // Uniforms:
        // vec4 separatorColor
        // vec4 dimColor
        // vec4 markColor
        // float alpha
        // float mark
        static const char* pSeparatorFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "uniform vec4 separatorColor;\n"
            "uniform vec4 dimColor;\n"
            "uniform vec4 markColor;\n"
            "uniform float alpha;\n"
            "uniform float dim;\n"
            "uniform float mark;\n"
            "void main() {\n"
            "   vec4 col = separatorColor;\n"
            "	col.rgb = (1.0 - (mark * markColor.a)) * col.rgb + (mark * markColor.a * markColor.rgb);\n" // Marking
            "	col.rgba *= (1.0 - dim) + (dim * dimColor);\n" // Dimming
            "   fragColor = vec4(col.rgb, col.a * alpha);\n"
            "}\n";

        // Uniforms:
        // sampler2D atlas
        // vec4 color
        // vec4 dimColor
        // vec4 markColor
        // float alpha
        // float activity
        // float dim
        // float mark
        static const char* pTextFlowFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform sampler2D atlas;\n"
            "uniform vec4 color;\n"
            "uniform vec4 dimColor;\n"
            "uniform vec4 markColor;\n"
            "uniform float alpha;\n"
            "uniform float activity;\n"
            "uniform float dim;\n"
            "uniform float mark;\n"
            "void main() {\n"
            "   float character = texture(atlas, uv).r;\n"
            "   vec4 col = color;\n"
            "   col.rgb = mix(vec3(0.3,0.3,0.3), col.rgb, max(0.2, activity));\n" // Activity
            "	col.rgb = (1.0 - (mark * markColor.a)) * col.rgb + (mark * markColor.a * markColor.rgb);\n" // Marking
            "	col.rgba *= (1.0 - dim) + (dim * dimColor);\n" // Dimming
            "   fragColor = vec4(col.rgb, col.a * alpha * character);\n"
            "}\n";

        // Uniforms:
        // vec4 backgroundColor
        // vec4 dimColor
        // vec4 markColor
        // float alpha
        // float activity
        // float dim
        // float mark
        static const char* pBlockFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "uniform vec4 backgroundColor;\n"
            "uniform vec4 dimColor;\n"
            "uniform vec4 markColor;\n"
            "uniform float alpha;\n"
            "uniform float activity;\n"
            "uniform float dim;\n"
            "uniform float mark;\n"
            "void main() {\n"
            "   vec4 col = backgroundColor;\n"
            "   col.rgb = mix(vec3(0.3,0.3,0.3), col.rgb, max(0.2, activity));\n" // Activity
            "	col.rgb = (1.0 - (mark * markColor.a)) * col.rgb + (mark * markColor.a * markColor.rgb);\n" // Marking
            "	col.rgba *= (1.0 - dim) + (dim * dimColor);\n" // Dimming
            "   fragColor = vec4(col.rgb, col.a * alpha);\n"
            "}\n";

        // Uniforms:
        // sampler2D icon
        // vec2 scale
        // vec4 dimColor
        // vec4 markColor
        // float alpha
        // float activity
        // float dim
        // float mark
        static const char* pImageFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform sampler2D image;\n"
            "uniform vec2 scale;\n"
            "uniform float alpha;\n"
            "uniform vec4 dimColor;\n"
            "uniform vec4 markColor;\n"
            "uniform float activity;\n"
            "uniform float dim;\n"
            "uniform float mark;\n"
            "void main() {\n"
            "   vec4 col = texture(image, ((uv - 0.5) * scale) + 0.5);\n" // Fetch color from image
            "   col.rgb = mix(vec3(0.3,0.3,0.3), col.rgb, max(0.2, activity));\n" // Activity
            "	col.rgb = (1.0 - (mark * markColor.a)) * col.rgb + (mark * markColor.a * markColor.rgb);\n" // Marking
            "	col.rgba *= (1.0 - dim) + (dim * dimColor);\n" // Dimming
            "   fragColor = vec4(col.rgb, col.a * alpha);\n"
            "}\n";

        // Uniforms:
        // sampler2D icon
        // float time
        // vec4 color
        // vec4 highlightColor
        // vec4 selectionColor
        // vec4 iconColor
        // vec4 dimColor
        // vec4 markColor
        // float threshold
        // float pressing
        // float highlight
        // float alpha
        // float activity
        // float dim
        // float mark
        // float selection
        static const char* pCircleButtonFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform sampler2D icon;\n"
            "uniform float time;\n"
            "uniform vec4 color = vec4(1,1,0,1);\n"
            "uniform vec4 highlightColor = vec4(0,1,0,1);\n"
            "uniform vec4 selectionColor = vec4(0,1,1,0.5);\n"
            "uniform vec4 iconColor = vec4(1,1,1,1);\n"
            "uniform vec4 dimColor;\n"
            "uniform vec4 markColor;\n"
            "uniform float threshold = 0;\n" // How much pressing threshold [0..1]
            "uniform float pressing = 0;\n" // How much pressed [0..1]
            "uniform float highlight = 0;\n" // How much highlighted [0..1]
            "uniform float alpha;\n"
            "uniform float activity;\n"
            "uniform float dim;\n"
            "uniform float mark;\n"
            "uniform float selection = 1;\n"
            "const int bodyPressBorder = 20;\n"
            "void main() {\n"
            "   float sinPressing = sin(0.5 * 3.14 * pressing);\n" // Pressing value
            "   float iconScale = mix(1, 1.5, sinPressing);\n" // Icon scale by pressing value
            "   vec4 iconValue = iconColor * texture(icon, ((uv - 0.5) * iconScale) + 0.5).rgba;\n" // Fetch icon
            "   float gradient = length(2*uv-1);\n" // Simple gradient as base
            "   float circle = (1.0-gradient) * 75;\n" // Extend gradient to unclamped circle
            "   float bodyMask = clamp(circle - bodyPressBorder * sinPressing + 0.5, 0, 1);\n" // Body mask (Adding 0.5 to hide background border when not pressing)
            "   float buttonMask = clamp(circle, 0, 1);\n" // Mask of whole button
            "   vec4 button = mix(color, vec4(iconValue.rgb, 1), iconValue.a);\n" // Just body with icon
            "   button = mix(vec4(color.rgb / 3, color.a), button, bodyMask);\n" // Just body with background
            "   float thresholdMask = clamp(100 * clamp((length(2*uv-1)) - (1.025 * threshold - 0.025), 0, 1), 0 ,1);\n" // Inverted threshold mask
            "   button.rgb = mix(1-button.rgb, button.rgb, thresholdMask);\n" // Use threshold to invert inner circle
            "   button.rgb = mix(button.rgb, highlightColor.rgb, 0.5 * (1 + sin(3 * time)) * highlight * highlightColor.a);\n" // Adding highlight
            "   button.rgb = mix(vec3(0.3,0.3,0.3), button.rgb, max(0.2, activity));\n" // Activity
            "	button.rgb = (1.0 - (mark * markColor.a)) * button.rgb + (mark * markColor.a * markColor.rgb);\n" // Marking
            "	button.rgba *= (1.0 - dim) + (dim * dimColor);\n" // Dimming
            "	button.rgb = mix(button.rgb, selectionColor.rgb, pow(gradient,2) * selection * selectionColor.a);\n" // Selection
            "   fragColor = vec4(button.rgb, button.a * buttonMask * alpha);\n" // Composing pixel
            "}\n";

        // Uniforms:
        // sampler2D icon
        // float time
        // vec4 color
        // vec4 highlightColor
        // vec4 selectionColor
        // vec4 iconColor
        // vec4 dimColor
        // vec4 markColor
        // float threshold
        // float pressing
        // float highlight
        // float alpha
        // float activity
        // float dim
        // float mark
        // float selection
        // vec2 iconUVScale
        // float orientation
        static const char* pBoxButtonFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform sampler2D icon;\n"
            "uniform float time;\n"
            "uniform vec4 color = vec4(1,0,0,1);\n"
            "uniform vec4 highlightColor = vec4(0,1,0,1);\n"
            "uniform vec4 selectionColor = vec4(0,1,1,0.5);\n"
            "uniform vec4 iconColor = vec4(1,1,1,1);\n"
            "uniform vec4 dimColor;\n"
            "uniform vec4 markColor;\n"
            "uniform float threshold = 0;\n" // How much pressing threshold [0..1]
            "uniform float pressing = 0;\n" // How much pressed [0..1]
            "uniform float highlight= 0;\n" // How much highlighted [0..1]
            "uniform float alpha;\n"
            "uniform float activity;\n"
            "uniform float dim;\n"
            "uniform float mark;\n"
            "uniform float selection = 1;\n"
            "uniform vec2 iconUVScale;\n"
            "uniform float orientation;\n"
            "const int bodyPressBorder = 40;\n"
            "void main() {\n"
            "   float sinPressing = sin(0.5 * 3.14 * pressing);\n" // Pressing value
            "   float iconScale = mix(1, 1.5, sinPressing);\n" // Icon scale by pressing value
            "   vec2 iconUV = ((uv - 0.5) * iconScale);\n" // Fetch icon
            "   iconUV = vec2(iconUV.r * iconUVScale.x, iconUV.g * iconUVScale.y) + 0.5;\n" // Icon scale by aspect ratio
            "   vec4 iconValue = iconColor *  texture(icon, iconUV).rgba;\n" // Fetch value of pixel
            "   float iconAlphaMask = clamp(iconUV.r * 1000, 0 , 1) - clamp((iconUV.r - 1) * 1000, 0, 1);" // Create horizontal mask
            "   iconAlphaMask *= clamp(iconUV.g * 1000, 0 , 1) - clamp((iconUV.g - 1) * 1000, 0, 1);" // Add vertical mask
            "   iconValue.a *= iconAlphaMask;\n" // Mask to hide clamping area
            "   float horizontalPressing = 100 * (1 - 1.95*abs(uv.r-0.5));\n"
            "   float verticalPressing = 100 * (1 - 1.95*abs(uv.g-0.5));\n"
            "   float bodyMask = clamp(horizontalPressing - bodyPressBorder * sinPressing, 0, 1) * clamp(verticalPressing - bodyPressBorder * sinPressing, 0, 1);\n" // Body mask
            "   float thresholdMask = mix(uv.r, uv.g, orientation);\n"
            "   thresholdMask =  clamp(100 * clamp(1.9 * abs(thresholdMask - 0.5)  - (0.975 * threshold - 0.025), 0, 1), 0, 1);\n" // Inverted threshold mask
            "   vec4 button = mix(color, vec4(iconValue.rgb, 1), iconValue.a);\n" // Just body with icon
            "   button = mix(vec4(color.rgb / 3, color.a), button, bodyMask);\n" // Just body with background
            "   button.rgb = mix(1-button.rgb, button.rgb, thresholdMask);\n" // Use threshold to invert
            "   button.rgb = mix(button.rgb, highlightColor.rgb, 0.5 * (1 + sin(3 * time)) * highlight * highlightColor.a);\n" // Adding highlight
            "   button.rgb = mix(vec3(0.3,0.3,0.3), button.rgb, max(0.2, activity));\n" // Activity
            "	button.rgb = (1.0 - (mark * markColor.a)) * button.rgb + (mark * markColor.a * markColor.rgb);\n" // Marking
            "	button.rgba *= (1.0 - dim) + (dim * dimColor);\n" // Dimming
            "   float gradient = length(2*uv-1);\n" // Simple gradient as base
            "	button.rgb = mix(button.rgb, selectionColor.rgb, pow(gradient,2) * selection * selectionColor.a);\n" // Selection
            "   fragColor = vec4(button.rgb, button.a * alpha);\n" // Composing pixel
            "}\n";

        // Uniforms:
        // sampler2D icon
        // float time
        // vec4 color
        // vec4 highlightColor
        // vec4 selectionColor
        // vec4 iconColor
        // vec4 dimColor
        // vec4 markColor
        // float penetration
        // float highlight
        // float alpha
        // float activity
        // float dim
        // float mark
        // float selection
        // vec2 iconUVScale
        static const char* pSensorFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform sampler2D icon;\n"
            "uniform float time;\n"
            "uniform vec4 color = vec4(1,0,0,1);\n"
            "uniform vec4 highlightColor = vec4(0,1,0,1);\n"
            "uniform vec4 selectionColor = vec4(0,1,1,0.5);\n"
            "uniform vec4 iconColor = vec4(1,1,1,1);\n"
            "uniform vec4 dimColor;\n"
            "uniform vec4 markColor;\n"
            "uniform float penetration = 0;\n" // How much penentration [0..1]
            "uniform float highlight= 1.0;\n" // How much highlighted [0..1]
            "uniform float alpha;\n"
            "uniform float activity;\n"
            "uniform float dim;\n"
            "uniform float mark;\n"
            "uniform float selection = 1;\n"
            "uniform vec2 iconUVScale;\n"
            "const int bodyPressBorder = 40;\n"
            "void main() {\n"
            "   float sinPressing = sin(0.5 * 3.14 * penetration);\n" // Pressing value
            "   float iconScale = mix(1, 1.5, sinPressing);\n" // Icon scale by pressing value
            "   vec2 iconUV = ((uv - 0.5) * iconScale);\n" // Fetch icon
            "   iconUV = vec2(iconUV.r * iconUVScale.x, iconUV.g * iconUVScale.y) + 0.5;\n" // Icon scale by aspect ratio
            "   vec4 iconValue = iconColor *  texture(icon, iconUV).rgba;\n" // Fetch value of pixel
            "   float iconAlphaMask = clamp(iconUV.r * 1000, 0 , 1) - clamp((iconUV.r - 1) * 1000, 0, 1);" // Create horizontal mask
            "   iconAlphaMask *= clamp(iconUV.g * 1000, 0 , 1) - clamp((iconUV.g - 1) * 1000, 0, 1);" // Add vertical mask
            "   iconValue.a *= iconAlphaMask;\n" // Mask to hide clamping area
            "   float horizontalPressing = 100 * (1 - 1.95*abs(uv.r-0.5));\n"
            "   float verticalPressing = 100 * (1 - 1.95*abs(uv.g-0.5));\n"
            "   float bodyMask = clamp(horizontalPressing - bodyPressBorder * sinPressing, 0, 1) * clamp(verticalPressing - bodyPressBorder * sinPressing, 0, 1);\n" // Body mask
            "   vec4 sensor = mix(color, vec4(iconValue.rgb, 1), iconValue.a);\n" // Just body with icon
            "   sensor = mix(vec4(color.rgb / 3, color.a), sensor, bodyMask);\n" // Just body with background
            "   sensor.rgb = mix(sensor.rgb, highlightColor.rgb, 0.5 * (1 + sin(3 * time)) * highlight * highlightColor.a);\n" // Adding highlight
            "   sensor.rgb = mix(vec3(0.3,0.3,0.3), sensor.rgb, max(0.2, activity));\n" // Activity
            "	sensor.rgb = (1.0 - (mark * markColor.a)) * sensor.rgb + (mark * markColor.a * markColor.rgb);\n" // Marking
            "	sensor.rgba *= (1.0 - dim) + (dim * dimColor);\n" // Dimming
            "   float gradient = length(2*uv-1);\n" // Simple gradient as base
            "	sensor.rgb = mix(sensor.rgb, selectionColor.rgb, pow(gradient,2) * selection * selectionColor.a);\n" // Selection
            "   fragColor = vec4(sensor.rgb, sensor.a * alpha);\n" // Composing pixel
            "}\n";

        // Uniforms:
        // vec4 color
        // float focus
		// float select
        // vec4 stencil
		static const char* pKeyFragmentShader =
			"#version 330 core\n"
			"out vec4 fragColor;\n"
			"in vec2 uv;\n"
			"uniform vec4 color = vec4(1,0,0,1);\n"
			"uniform vec4 selectionColor = vec4(0,1,1,0.5);\n"
			"uniform float focus = 0;\n"
			"uniform float selection = 0;\n"
			"uniform vec4 stencil;\n"
			"const int innerBorder = 10;\n"
			"void main() {\n"
			"   if(gl_FragCoord.x < stencil.x || gl_FragCoord.y < stencil.y || gl_FragCoord.x >= stencil.x+stencil.z || gl_FragCoord.y >= stencil.y+stencil.w)\n"
			"   {\n"
			"       discard;\n"
			"   }\n"
			"   float gradient = length(2*uv-1);\n" // Simple gradient as base
			"   float circle = (1.0-gradient) * 75;\n" // Extend gradient to unclamped circle
			"   float inner = clamp(circle - (selection * innerBorder), 0, 1);\n" // Inner circle for character
			"	float outer = clamp(circle, 0, 1);\n" // Outer circle for selection
			"	vec4 col = vec4((1.0-focus) * color.rgb +  focus * (1.0-color.rgb), color.a);\n" // Color and focus
			"   vec4 customSelectionColor = selectionColor;\n"
			"	customSelectionColor.a *= 0.5;\n" // Perpare selection color
			"	col += selection * customSelectionColor * (1.0-inner);\n" // Add custom selection color
            "   fragColor = vec4(col.rgb , col.a * outer);\n" // Composing pixel
            "}\n";


        // Uniforms:
        // sampler2D atlas
        // vec4 color
        // vec4 stencil
        static const char* pCharacterKeyFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform sampler2D atlas;\n"
            "uniform vec4 color = vec4(1,0,0,1);\n"
            "uniform vec4 stencil;\n"
            "void main() {\n"
            "   if(gl_FragCoord.x < stencil.x || gl_FragCoord.y < stencil.y || gl_FragCoord.x >= stencil.x+stencil.z || gl_FragCoord.y >= stencil.y+stencil.w)\n"
            "   {"
            "       discard;\n"
            "   }"
            "   float value = texture(atlas, uv).r;\n"
            "   fragColor = vec4(color.rgb, color.a * value);\n" // Composing pixel
            "}\n";
    }
}

#endif // SHADERS_H:
