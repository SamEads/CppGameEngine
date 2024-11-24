#pragma once
#include "../io/filesystem.h"
#include "texture.h"
#include <unordered_map>
#include "../utility/col.h"
#include "../utility/vec.h"

class Atlas
{
public:
    struct Icon
    {
        std::string name;
        Vec2<int> position;
        Vec2<int> size;
        Vec2<int> atlasSize;
        Image image;
        Col4<float> avgColor;

        inline double getU(double v) const
        {
            return (static_cast<double>(position.x) + v * size.x) / atlasSize.x;
        }

        inline double getUMin(double minX = 0.0) const
        {
            return (static_cast<double>(position.x) + minX * size.x) / atlasSize.x;
        }

        inline double getUMax(double maxX = 1.0) const
        {
            return (static_cast<double>(position.x) + maxX * size.x) / atlasSize.x;
        }

        inline double getV(double v) const
        {
            return (static_cast<double>(position.y) + v * size.y) / atlasSize.y;
        }

        inline double getVMin(double minZ = 0.0) const
        {
            return (static_cast<double>(position.y) + minZ * size.y) / atlasSize.y;
        }

        inline double getVMax(double maxZ = 1.0) const
        {
            return (static_cast<double>(position.y) + maxZ * size.y) / atlasSize.y;
        }

        inline double offsetU(double offset) const
        {
            return (offset * size.x) / atlasSize.x;
        }

        inline double offsetV(double offset) const
        {
            return (offset * size.y) / atlasSize.y;
        }

        inline double getFlippedX1() const
        {
            return getUMax();
        }

        inline double getFlippedX2() const
        {
            return getUMin();
        }

        inline double getFlippedY1() const
        {
            return getVMax();
        }

        inline double getFlippedY2() const
        {
            return getVMin();
        }
    };

private:
    // Path the texture atlas will search to add textures.
    FileSystem::Path m_path;

    // Texture for atlas.
    // Once an atlas is generated, this is heap memory that needs to be deleted!
    // The destructor takes care of this, so no reassigning.
    Texture m_texture;

    // List of references to icons currently being used in this atlas.
    std::unordered_map<std::string, Icon> m_icons;
    std::vector<Icon*> m_iconsVec;

    int m_atlasWidth = 0;
    int m_atlasHeight = 0;
    bool m_generated = false;

    bool placeIcon(std::vector<std::vector<bool>>& atlas, Icon& icon) const;

    void getIcons();

    static bool compareHeight(const Icon* a, const Icon* b);

    int m_padding = 0;

public:
    Atlas(FileSystem::Path path, int m_padding = 0);

    const FileSystem::Path& getPath() const { return m_path; }

    Icon* registerIcon(const std::string& name);

    Icon* createIcon(const std::string& name, int x, int y);

    // Generates or regenerates an atlas.
    // Takes care of loading and unloading resources.
    void generateAtlas(bool matchHV = true);

    void regenerateAtlas();

    int getAtlasWidth() const
    {
        return m_atlasWidth;
    }

    int getAtlasHeight() const
    {
        return m_atlasHeight;
    }

    Texture& getTexture()
    {
        return m_texture;
    }

    void exportImage(FileSystem::Path dest);
};