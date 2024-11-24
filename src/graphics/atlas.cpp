#include "atlas.h"
#include <iostream>
#include <regex>
#include <map>
#include "image.h"
#include <set>
#include <glad/glad.h>

bool Atlas::placeIcon(std::vector<std::vector<bool>>& atlas, Atlas::Icon& icon) const
{
    for (int y = 0; y <= m_atlasHeight - (icon.size.y + 2 * m_padding); ++y)
    {
        for (int x = 0; x <= m_atlasWidth - (icon.size.x + 2 * m_padding); ++x)
        {
            bool foundFit = true;

            for (int i = 0; foundFit && (i < icon.size.x + 2 * m_padding); ++i)
            {
                for (int j = 0; foundFit && (j < icon.size.y + 2 * m_padding); ++j)
                {
                    if (atlas[y + j][x + i])
                    {
                        foundFit = false;
                    }
                }
            }

            if (foundFit)
            {
                icon.position.x = x + m_padding;
                icon.position.y = y + m_padding;

                for (int i = 0; i < icon.size.x + 2 * m_padding; ++i)
                {
                    for (int j = 0; j < icon.size.y + 2 * m_padding; ++j)
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
    // All files in path (recursive)
    // { File Name (with .png), Path To File }
    std::unordered_map<std::string, FileSystem::Path> files;

    for (const auto& entry : FileSystem::RecursiveDirectoryIterator(m_path))
    {
        if (entry.is_regular_file())
        {
            // Files list has already iterated upon this file
            std::string fname = entry.path().filename().string();
            FileSystem::Path fpath = entry.path();

            // only allow for one of same name type.
            // potentially need to look for fix since recursive
            // also check inside of icons
            if (files.find(fname) != files.end() &&
                m_icons.find(fname) != m_icons.end())
            {
                continue;
            }
            files.emplace(fname, fpath);
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
            icon.image.load(it->second.string());
            icon.size.x = icon.image.getWidth();
            icon.size.y = icon.image.getHeight();

            float sumR = 0.0f, sumG = 0.0f, sumB = 0.0f;
            int pixelCount = 0;

            for (int y = 0; y < icon.image.getHeight(); ++y)
            {
                for (int x = 0; x < icon.image.getWidth(); ++x)
                {
                    int index = (y * icon.image.getWidth() + x) * 4;
                    unsigned char r = ((unsigned char*)icon.image.getData())[index];
                    unsigned char g = ((unsigned char*)icon.image.getData())[index + 1];
                    unsigned char b = ((unsigned char*)icon.image.getData())[index + 2];
                    unsigned char a = ((unsigned char*)icon.image.getData())[index + 3];

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

Atlas::Atlas(FileSystem::Path path, int padding) :
    m_path("assets/textures" / path),
    m_padding(padding)
{
    if (!FileSystem::exists(m_path))
    {
        std::cout << "[WARNING] Path " << m_path.string() << "does not exist!\n";
    }
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

    Image img(m_atlasWidth, m_atlasHeight, 0, 0, 0, 0);

    // Draw images to the atlas in their spot
    for (auto& pair : m_icons)
    {
        auto& icon = pair.second;

        if (icon.name.empty())
        {
            icon.name = pair.first;
            continue;
        }

        img.draw(icon.image, 0, 0, icon.size.x, icon.size.y, icon.position.x, icon.position.y, icon.size.x, icon.size.y, m_padding);

        icon.atlasSize.x = m_atlasWidth;
        icon.atlasSize.y = m_atlasHeight;

        icon.image.unload();
    }

    m_texture.load(img);

    m_generated = true;
}

void Atlas::regenerateAtlas()
{
    for (auto& icon : m_icons)
    {
        auto& i = icon.second;

        i.image.load(i.name);

        i.position.x = i.position.y = i.size.x = i.size.y = 0;
    }

    generateAtlas();
}

void Atlas::exportImage(FileSystem::Path dest)
{
    // Bind texture (must be done to export image)
    m_texture.bind(0);
    
    // Create image and save it to the provided destination. Inherits extension but expects a PNG.
    // Maybe look into other file types and comp.
    Image i(&m_texture);
    i.save(dest);

    // Set bound texture state back to none
    m_texture.unbind();
}