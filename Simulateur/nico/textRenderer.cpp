#include "textRenderer.h"
#include "config.hpp"

#include FT_FREETYPE_H

nico::TextRenderer::TextRenderer(const char* font) : projectionMatrix(glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f)),
    shader(NICO_SHADERS_PATH"textRenderer.vert",NICO_SHADERS_PATH"textRenderer.frag")
{
    height = 0U;
    width = 0U;
    offset = 2U;
    windowsHeight = 1920U;
    windowsWidth = 1080U;
    maxHeight = 0U;
    // FreeType
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        exit(-1);
    }

    // find path to font
    /*std::string font_name = FileSystem::getPath("resources/fonts/Antonio-Bold.ttf");
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        exit(-1);
    }*/
    
    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font, 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        exit(-1);
    }
   

    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph 
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            //get the max height in pixel
            maxHeight = std::max(face->glyph->bitmap.rows, maxHeight);

            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    
    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // this number allow to transform relative scale in pixel scale
    scaleByPixel = 1.0f / (float)maxHeight;
}

void nico::TextRenderer::updateDisplay(nico::Window* win)
{
    if (!(win->getHeight() && win->getWidth()))
        return;//check if window size is > 0

    windowsWidth = win->getWidth(); windowsHeight = win->getHeight();
    projectionMatrix = glm::ortho<float>(0, windowsWidth, 0, windowsHeight);

    //enable transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

unsigned int nico::TextRenderer::getLastWidth() const
{
    return width;
}

unsigned int nico::TextRenderer::getLastHeight() const
{
    return height;
}


void nico::TextRenderer::print(std::string text, float x, float y, float scale, glm::vec4 color, bool background)
{
    fillWidthAndHeight(&text, scale);
    renderText(text, x, y, scale, color, background);
}

void nico::TextRenderer::printLeftTop(std::string text, float x, float y, float scale, glm::vec4 color, bool background)
{
    fillWidthAndHeight(&text, scale);
    renderText(text, x, windowsHeight - y - height, scale, color, background);
}

void nico::TextRenderer::printRightTop(std::string text, float x, float y, float scale, glm::vec4 color, bool background)
{
    this->print(text, windowsWidth - x, windowsHeight - y, scale, color, background);
}

void nico::TextRenderer::printLeftBottom(std::string text, float x, float y, float scale, glm::vec4 color, bool background)
{
    this->print(text, windowsWidth - x, y, scale, color, background);
}

glm::vec2 nico::TextRenderer::calculateTextRenderingSize(std::string text, float scale)
{
    fillWidthAndHeight(&text, scale);
    return glm::vec2(width, height);
}

float nico::TextRenderer::getAdvance(char asciiCHaracter, float scale)
{
    scale *= scaleByPixel;

    //check that the character exist
    std::map<char, Character>::const_iterator it = Characters.find(asciiCHaracter);
    if (it == Characters.end())
        return 0;

    return (float(Characters[asciiCHaracter].Advance >> 6) * scale);
}

void nico::TextRenderer::fillWidthAndHeight(std::string* text, float scale)
{
    width = 0;
    height = 0;

    scale *= scaleByPixel;

    //determine width and height of the text to render
    std::string::const_iterator c;
    for (c = text->begin(); c != text->end(); c++)
    {
        Character ch = Characters[*c];

        float h = ch.Size.y * scale;//determine the height of the current letter

        width += (ch.Advance >> 6) * scale;//add width of the letter space 
        height = std::max(h, height);//assign the biggest height of all characters
    }
}

void nico::TextRenderer::renderText(std::string text, float x, float y, float scale, glm::vec4 color, bool background)
{
    glDepthFunc(GL_ALWAYS);

    //check that transparency is not null
    if (!color.a)
        color.a = 1;
    
    //set scale in pixel
    scale *= scaleByPixel;

    //print background behind text
    if (background) {
        shader.set("background", true);

        VertexArray VAO2;

        std::vector<float> vertices = {
            x - offset, y - offset,
            x + width + offset, y - offset,
            x - offset, y + height + offset,

            x - offset, y + height + offset,
            x + width + offset, y - offset,
            x + width + offset, y + height + offset
        };


        VertexBuffer<float> VBO2(&vertices);
        VBO2.addLayout(0, 2);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    // activate corresponding render state	
    shader.use();
    shader.set("textColor", color);
    shader.set("projection", projectionMatrix);
    shader.set("background", false);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

std::string nico::TextRenderer::textToPrint = "";