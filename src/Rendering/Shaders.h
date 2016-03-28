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

        static const char* pColorFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "uniform vec4 color = vec4(1,1,1,1);\n"
            "uniform float alpha = 1;\n"
            "void main() {\n"
            "   fragColor = vec4(color.rgb, color.a * alpha);\n"
            "}\n";

        static const char* pCircleFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform vec4 color = vec4(1,1,1,1);\n"
            "uniform float alpha = 1;\n"
            "void main() {\n"
            "   float gradient = length(2*uv-1);\n" // simple gradient as base
            "   float circle = (1-gradient) * 75;\n" // extend gradient to unclamped circle
            "   fragColor = vec4(color.rgb, color.a * alpha * min(circle, 1.0));\n"
            "}\n";

        static const char* pDimFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform vec4 dimColor = vec4(1,0,0,0.5);\n"
            "uniform float dim = 1;\n"
            "uniform float alpha = 1;\n"
            "uniform sampler2D mask;\n"
            "void main() {\n"
            "   fragColor = vec4(1.0, 1.0, 1.0, texture(mask, uv).r * alpha) * dimColor * dim;\n"
            "}\n";

        static const char* pActivityFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform float activity;\n"
            "uniform float alpha = 1;\n"
            "uniform sampler2D mask;\n"
            "void main() {\n"
            "   fragColor = vec4(0.3, 0.3, 0.3, texture(mask, uv).r * 0.5 * alpha) * (1.0 - activity);\n"
            "}\n";

        static const char* pMarkFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform vec4 markColor = vec4(1,0,0,0.5);\n"
            "uniform float mark = 1;\n"
            "uniform float alpha = 1;\n"
            "uniform sampler2D mask;\n"
            "void main() {\n"
            "   fragColor = vec4(1.0, 1.0, 1.0, texture(mask, uv).r * alpha) * markColor * mark;\n"
            "}\n";

        static const char* pCircleThresholdFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform vec4 thresholdColor = vec4(1,0,0,0.5);\n"
            "uniform float threshold = 1;\n"
            "uniform float alpha = 1;\n"
            "uniform sampler2D mask;\n"
            "void main() {\n"
            "   float gradient = length(2*uv-1);\n" // simple gradient as base
            "   float circle = (1.0-gradient) * 75;\n" // extend gradient to unclamped circle
            "   float thresholdMask = clamp(100 * clamp((length(2*uv-1)) - (1.025 * threshold - 0.025), 0, 1), 0 ,1);\n" // inverted threshold
            "   fragColor = vec4(1.0, 1.0, 1.0, texture(mask, uv).r * sqrt(threshold) * (1.0 - thresholdMask) * alpha) * thresholdColor;\n"
            "}\n";

        static const char* pBoxThresholdFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform vec4 thresholdColor = vec4(1,0,0,0.5);\n"
            "uniform float threshold = 1;\n"
            "uniform float alpha = 1;\n"
            "uniform float orientation;\n"
            "uniform sampler2D mask;\n"
            "void main() {\n"
            "   float thresholdMask = mix(uv.r, uv.g, orientation);\n"
            "   thresholdMask =  clamp(100 * clamp(1.9 * abs(thresholdMask - 0.5)  - (0.975 * threshold - 0.025), 0, 1), 0, 1);\n" // inverted threshold
			"   fragColor = vec4(1.0, 1.0, 1.0, texture(mask, uv).r * sqrt(threshold) * (1.0 - thresholdMask) * alpha) * thresholdColor;\n"
            "}\n";

        static const char* pHighlightFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform vec4 highlightColor = vec4(1,0,0,0.5);\n"
            "uniform float highlight = 1;\n"
            "uniform float time;\n"
            "uniform float alpha = 1;\n"
            "uniform sampler2D mask;\n"
            "void main() {\n"
            "   fragColor = vec4(1.0, 1.0, 1.0, texture(mask, uv).r * alpha * 0.5 * (1 + sin(3 * time))) * highlightColor * highlight;\n"
            "}\n";

        static const char* pSelectionFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform vec4 selectionColor = vec4(1,0,0,0.5);\n"
            "uniform float selection = 1;\n"
            "uniform float alpha = 1;\n"
            "uniform sampler2D mask;\n"
            "void main() {\n"
            "   float gradient = length(2*uv-1);\n" // simple gradient as base
            "	fragColor = vec4(1.0, 1.0, 1.0, texture(mask, uv).r * alpha * pow(gradient,2) * selection) * selectionColor;\n"
            "}\n";

        static const char* pCircleButtonFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform vec4 color = vec4(1,0,0,0.5);\n"
            "uniform vec4 iconColor = vec4(1,1,0,0.5);\n"
            "uniform float pressing;\n"
            "uniform float alpha = 1;\n"
            "uniform sampler2D mask;\n"
            "uniform sampler2D icon;\n"
            "void main() {\n"
            "   float powPressing = pressing * pressing;\n"
            "   vec2 pressUV = ((0.975 + (0.5 * powPressing)) * (uv - 0.5)) + 0.5;\n"
			"   float pressMask = (texture(mask, pressUV)).r;\n"
			"   vec3 col = mix(color.rgb / 3, color.rgb, pressMask);\n"
            "   vec4 iconValue = iconColor * texture(icon, pressUV).rgba;\n"
			"   vec4 final = mix(vec4(col, color.a), vec4(iconValue.rgb, 1), iconValue.a * pressMask);\n"
            "	fragColor = vec4(final.rgb, final.a * texture(mask, uv).r * alpha);\n"
            "}\n";

		static const char* pBoxButtonFragmentShader =
			"#version 330 core\n"
			"out vec4 fragColor;\n"
			"in vec2 uv;\n"
			"uniform vec4 color = vec4(1,0,0,0.5);\n"
			"uniform vec4 iconColor = vec4(1,1,0,0.5);\n"
			"uniform float pressing;\n"
			"uniform vec2 iconUVScale;\n"
			"uniform float alpha = 1;\n"
			"uniform sampler2D mask;\n"
			"uniform sampler2D icon;\n"
			"void main() {\n"
			"   float powPressing = pressing * pressing;\n"
			"   vec2 pressUV = ((1 + (0.5 * powPressing)) * (uv - 0.5)) + 0.5;\n"
			"   float pressMask = float(all(equal(ivec2(1,1), ivec2(pressUV+1))));\n"
            "   vec3 col = mix(color.rgb / 3, color.rgb, pressMask);\n"
            "   vec2 iconUV = ((1 + (0.5 * powPressing)) * iconUVScale * (uv - 0.5)) + 0.5;\n"
            "   vec4 iconValue = iconColor * texture(icon, iconUV).rgba;\n"
			"   vec4 final = mix(vec4(col, color.a), vec4(iconValue.rgb, 1), iconValue.a * pressMask);\n"
            "	fragColor = vec4(final.rgb, final.a * texture(mask, uv).r * alpha);\n"
            "}\n";

    static const char* pSensorFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform vec4 color = vec4(1,0,0,0.5);\n"
            "uniform vec4 iconColor = vec4(1,1,0,0.5);\n"
            "uniform float penetration;\n"
            "uniform vec2 iconUVScale;\n"
            "uniform float alpha = 1;\n"
            "uniform sampler2D mask;\n"
            "uniform sampler2D icon;\n"
            "void main() {\n"
            "   vec2 penetrationUV = ((1 + (0.5 * penetration)) * (uv - 0.5)) + 0.5;\n"
			"   float penetratioMask = float(all(equal(ivec2(1,1), ivec2(penetrationUV+1))));\n"
			"   vec3 col = mix(color.rgb / 3, color.rgb, penetratioMask);\n"
            "   vec2 iconUV = ((1 + (0.5 * penetration)) * iconUVScale * (uv - 0.5)) + 0.5;\n"
            "   vec4 iconValue = iconColor * texture(icon, iconUV).rgba;\n"
			"   vec4 final = mix(vec4(col, color.a), vec4(iconValue.rgb, 1), iconValue.a * penetratioMask);\n"
            "	fragColor = vec4(final.rgb, final.a * texture(mask, uv).r * alpha);\n"
            "}\n";

        static const char* pTextFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform vec4 color;\n"
            "uniform float alpha;\n"
            "uniform sampler2D atlas;\n"
            "void main() {\n"
            "   float character = texture(atlas, uv).r;\n"
            "   vec4 col = color;\n"
            "   fragColor = vec4(col.rgb, col.a * alpha * character);\n"
            "}\n";

        static const char* pImageFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform vec2 scale;\n"
            "uniform float alpha;\n"
            "uniform sampler2D image;\n"
            "void main() {\n"
            "   vec4 col = texture(image, ((uv - 0.5) * scale) + 0.5);\n" // fetch color from image
            "   fragColor = vec4(col.rgb, col.a * alpha);\n"
            "}\n";

        static const char* pKeyFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform float time;\n"
            "uniform vec4 color = vec4(1,0,0,1);\n"
            "uniform vec4 pickColor = vec4(0,1,1,0.5);\n"
            "uniform float pick = 0;\n"
            "const int innerBorder = 10;\n"
            "void main() {\n"
            "   float gradient = length(2*uv-1);\n" // simple gradient as base
            "   float circle = (1.0-gradient) * 75;\n" // extend gradient to unclamped circle
            "   float inner = clamp(circle - (pick * innerBorder), 0, 1);\n" // inner circle for character
            "	float outer = clamp(circle, 0, 1);\n" // outer circle for pick
            "	vec4 col = color;\n" // color
            "   vec4 customPickColor = pickColor;\n"
            "	customPickColor.a *= 0.5;\n" // perpare pick color
            "	col += pick * customPickColor * (1.0-inner);\n" // add custom pick color
            "   fragColor = vec4(col.rgb , col.a * outer);\n" // composing pixel
            "}\n";

        static const char* pCharacterKeyFragmentShader =
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "in vec2 uv;\n"
            "uniform vec4 color = vec4(1,0,0,1);\n"
            "uniform sampler2D atlas;\n"
            "void main() {\n"
            "   float value = texture(atlas, uv).r;\n"
            "   fragColor = vec4(color.rgb, color.a * value);\n"
            "}\n";
    }
}

#endif // SHADERS_H_
