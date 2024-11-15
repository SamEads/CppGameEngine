#include "atlas.h"
#include <iostream>
#include <regex>
#include <map>
#include <raylib.h>

bool Atlas::placeIcon(std::vector<std::vector<bool>>& atlas, Atlas::Icon& icon) const
{
    for (int y = 0; y <= m_atlasHeight - icon.size.y; ++y)
    {
        for (int x = 0; x <= m_atlasWidth - icon.size.x; ++x)
        {
            bool foundFit = true;

            for (int i = 0; foundFit && (i < icon.size.x); ++i)
            {
                for (int j = 0; foundFit && (j < icon.size.y); ++j)
                {
                    // this is an overlap. it does not fit here!
                    if (atlas[y + j][x + i])
                    {
                        foundFit = false;
                    }
                }
            }

            if (foundFit)
            {
                icon.position.x = x;
                icon.position.y = y;
                for (int i = 0; i < icon.size.x; ++i)
                {
                    for (int j = 0; j < icon.size.y; ++j)
                    {
                        atlas[y + j][x + i] = true;
                    }
                }
                return true;
            }
        }
    }
    return false;
}

void Atlas::getIcons()
{
    std::unordered_map<std::string, std::string> files;

    for (const auto& entry : FileSystem::RecursiveDirectoryIterator(m_path))
    {
        if (entry.is_regular_file())
        {
            std::string filePath = entry.path().string();
            std::string fileName = entry.path().filename().string();

            if (files.find(fileName) == files.end())
            {
                files.insert({ fileName, filePath });
            }
        }
    }

    for (auto& pair : m_icons)
    {
        auto& icon = pair.second;
        if (icon.name.empty())
        {
            continue;
        }
        auto it = files.find(icon.name);
        if (it != files.end())
        {
            Image image = LoadImage(it->second.c_str());
            icon.image = { image.data, { image.width, image.height }, image.format };
            icon.size.x = icon.image.size.x;
            icon.size.y = icon.image.size.y;

            float sumR = 0.0f, sumG = 0.0f, sumB = 0.0f;
            int pixelCount = 0;

            for (int y = 0; y < image.height; ++y)
            {
                for (int x = 0; x < image.width; ++x)
                {
                    int index = (y * image.width + x) * 4;
                    unsigned char r = ((unsigned char*)image.data)[index];
                    unsigned char g = ((unsigned char*)image.data)[index + 1];
                    unsigned char b = ((unsigned char*)image.data)[index + 2];
                    unsigned char a = ((unsigned char*)image.data)[index + 3];

                    if (a < 20) continue;

                    sumR += r / 255.0f;
                    sumG += g / 255.0f;
                    sumB += b / 255.0f;
                    pixelCount++;
                }
            }

            if (pixelCount > 0)
            {
                icon.avgColor.r = sumR / pixelCount;
                icon.avgColor.g = sumG / pixelCount;
                icon.avgColor.b = sumB / pixelCount;
                icon.avgColor.a = 1.0f;
            }
            else
            {
                icon.avgColor = { 0.0f, 0.0f, 0.0f, 0.0f };
            }
        }
    }
}

Atlas::Atlas(FileSystem::Path path)
{
    m_path = "assets/textures" / path;
}

Atlas::Icon* Atlas::registerIcon(const std::string& name)
{
    auto it = m_icons.find(name);
    if (it == m_icons.end())
    {
        if (!FileSystem::exists(m_path / (name + ".png")))
        {
            std::cout << "[WARNING] " << name << ".png does not exist" << std::endl;
        }
        m_icons[name] = { name + ".png" };
        m_iconsVec.push_back(&m_icons[name]);
        return &m_icons[name];
    }
    return &it->second;
}

Atlas::Icon* Atlas::createIcon(const std::string& name, int w, int h)
{
    auto it = m_icons.find(name);
    if (it == m_icons.end())
    {
        m_icons.insert({ name, Atlas::Icon() });
        m_icons[name].size.x = w;
        m_icons[name].size.y = h;
        m_iconsVec.push_back(&m_icons[name]);
        return &m_icons[name];
    }
    return &it->second;
}

// Function to convert RGB to HSV
static void RGBtoHSV(const Col4<float>& color, float& h, float& s, float& v)
{
    float r = color.r;
    float g = color.g;
    float b = color.b;

    double cmax = std::max(r, std::max(g, b));
    double cmin = std::min(r, std::min(g, b));

    double delta = cmax - cmin;

    v = cmax * 100;

    if (delta > 0)
    {
        if (r >= cmax)
        {
            h = 60 * (fmod(((g - b) / delta), 6));
        }
        else if (g >= cmax)
        {
            h = 60 * (((b - r) / delta) + 2.0);
        }
        else if (b >= cmax)
        {
            h = 60 * (((r - g) / delta) + 4.0);
        }

        h = (h < 0.0 ? h + 360 : h);

        s = (cmax > 0.0 ? (delta / cmax) * 100 : 0);
    }
    else
    {
        h = 0;
        s = 0;
    }
}

static bool compareColors(const Col4<float>& a, const Col4<float>& b)
{
    float ha, sa, va;
    float hb, sb, vb;
    RGBtoHSV(a, ha, sa, va);
    RGBtoHSV(b, hb, sb, vb);
    const float saturationThreshold = 10.0f;

    sa = floor(sa * saturationThreshold) / saturationThreshold;
    sb = floor(sb * saturationThreshold) / saturationThreshold;

    if (sa < saturationThreshold && sb < saturationThreshold)
    {
        return va > vb;
    }

    if (sa < saturationThreshold)
    {
        return false;
    }

    if (sb < saturationThreshold)
    {
        return true;
    }

    return (floor(ha * 10.0f) / 10.0f) < (floor(hb * 10.0f) / 10.0f);
}

bool Atlas::compareHeight(const Atlas::Icon* a, const Atlas::Icon* b)
{
    int areaA = a->size.x * a->size.y;
    int areaB = b->size.x * b->size.y;

    if (areaA == areaB && !a->name.empty())
    {
        return compareColors(a->avgColor, b->avgColor);
    }
    return areaA > areaB;
}

void Atlas::generateAtlas(bool matchHV)
{
    if (m_generated)
    {
        m_texture.reset();
        m_generated = false;
    }

    if (m_iconsVec.empty())
    {
        return;
    }

    // Clear and re-populate icons w/ paths
    getIcons();

    std::sort(m_iconsVec.begin(), m_iconsVec.end(), compareHeight);

    m_atlasWidth = 128;
    m_atlasHeight = 128;
    bool allPlaced = false;

    while (!allPlaced)
    {
        std::vector<std::vector<bool>> atlas(m_atlasHeight, std::vector<bool>(m_atlasWidth, false));
        allPlaced = true;

        for (auto& icon : m_iconsVec)
        {
            if (!placeIcon(atlas, *icon))
            {
                allPlaced = false;
                if (matchHV)
                {
                    m_atlasWidth *= 2;
                    m_atlasHeight *= 2;
                }
                else if (m_atlasWidth <= m_atlasHeight)
                {
                    m_atlasWidth *= 2;
                }
                else
                {
                    m_atlasHeight *= 2;
                }
                break;
            }
        }
    }

    Image img = GenImageColor(m_atlasWidth, m_atlasHeight, BLANK);

    // Draw images to the atlas in their spot
    for (auto& pair : m_icons)
    {
        auto& icon = pair.second;

        if (icon.name.empty())
        {
            icon.name = pair.first;
            continue;
        }

        Image imgCompat = { icon.image.data, icon.image.size.x, icon.image.size.y, 1, icon.image.format };

        ImageDraw(&img, imgCompat,
            { 0, 0, static_cast<float>(icon.size.x), static_cast<float>(icon.size.y) },
            { static_cast<float>(icon.position.x), static_cast<float>(icon.position.y), static_cast<float>(icon.size.x), static_cast<float>(icon.size.y) },
            WHITE);

        icon.atlasSize.x = m_atlasWidth;
        icon.atlasSize.y = m_atlasHeight;

        // Unload image data
        UnloadImage(imgCompat);
        icon.image = { 0 };
    }

    Texture t = LoadTextureFromImage(img);
    m_texture.set(t.id, { t.width, t.height }, t.format);
    m_generated = true;

    UnloadImage(img);

    std::cout << "[INFO] Texture atlas \"" << m_path.string() << "\" generated\n";
}

void Atlas::regenerateAtlas()
{
    for (auto& icon : m_icons)
    {
        auto& i = icon.second;

        Image imgCompat = LoadImage(i.name.c_str());
        i.image = { imgCompat.data, imgCompat.width, imgCompat.height, imgCompat.format };

        i.position.x = i.position.y = i.size.x = i.size.y = 0;
    }

    generateAtlas();
}

void Atlas::exportImage(FileSystem::Path dest)
{
    Texture t = { m_texture.getId(), m_texture.getSize().x, m_texture.getSize().y, 1, m_texture.getFormat() };
    Image i = LoadImageFromTexture(t);
    ExportImage(i, dest.string().c_str());
    UnloadImage(i);
}
