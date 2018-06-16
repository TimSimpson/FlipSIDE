#include "Level.hpp"

namespace nnd3d { namespace game {

FSLevelFile load_fs_level(Vb & vb, const std::string & file) {
    FSLevelFile result;

    auto f = vb.OpenForInput(file);
    {
        std::string texture_file;
        int wide;
        int high;
        for (int j = 0; j <= 9; ++ j) {
            if (j > 3 || j < 1) {  // 0, 4 , 5, 6, 7, 8, 9 <-- lol
                f.input(texture_file, wide, high);

                FSLevelFile::TextureInfo tex;
                tex.index = j;
                tex.name = texture_file;
                tex.size = { wide, high };

                result.textures.push_back(tex);
            }
        }
    }
    {
        std::string name;
        double x, y, z;
        int src_x1, src_y1, src_x2, src_y2;
        double size_x, size_y, size_z;
        int texture_index;
        bool visible;
        Kind kind;
        int z_order;

        for (int j = 40; j <= 100; ++j) {
            f.input(name, x, y, z, src_x1, src_y1, src_x2, src_y2,
                    size_x, size_y, size_z, texture_index, visible,
                    kind, z_order);
            FSLevelFile::Sprite sprite;
            sprite.name = name;
            sprite.position = {x, y, z};
            sprite.src_ul = { src_x1, src_y1 };
            sprite.src_br = { src_x2, src_y2 };
            sprite.size = { size_x, size_y, size_z };
            sprite.texture = texture_index;
            sprite.visible = visible;
            sprite.kind = kind;
            sprite.z_order = z_order;

            result.sprites.push_back(sprite);
        }
    }
    return result;
}

}   }
