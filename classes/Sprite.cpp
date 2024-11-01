#include "../Application.h"
#include "Sprite.h"
#include "../imgui/imgui_impl_opengl3_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// texture cache
std::map<std::string, SpriteCacheObject> Sprite::_textureCache;

// Simple helper function to load an image into a OpenGL texture with common settings
bool Sprite::LoadTextureFromFile(const char *filename)
{
#if defined(UCI_INTERFACE)
    return false;
#endif
    // check the cache
    auto it = _textureCache.find(filename);
    if (it != _textureCache.end())
    {
        _texture = it->second.texture;
        _size = it->second.size;
        return true;
    }
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char *image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
    {
        // try up one directory
        char newFilename[1024]; // hmmmm, this is a bit of a hack
        snprintf(newFilename, sizeof(newFilename), "../%s", filename);
        image_data = stbi_load(newFilename, &image_width, &image_height, NULL, 4);
        if (image_data == NULL)
        {
            // try up one more directory
            snprintf(newFilename, sizeof(newFilename), "../../%s", filename);
            image_data = stbi_load(newFilename, &image_width, &image_height, NULL, 4);
            if (image_data == NULL)
            {
                _size = ImVec2(0, 0);
                return false;
            }
        }
    }

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    _texture = reinterpret_cast<ImTextureID>(image_texture);
    _size = ImVec2((float)image_width, (float)image_height);

    // cache it
    SpriteCacheObject cacheObject;
    cacheObject.texture = _texture;
    cacheObject.size = _size;
    _textureCache[filename] = cacheObject;

    return true;
}

void Sprite::setHighlighted(bool highlighted)
{
    if (highlighted != _highlighted)
    {
        _highlighted = highlighted;
    }
}

bool Sprite::highlighted()
{
    return _highlighted;
}
