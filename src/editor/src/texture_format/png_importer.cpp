// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <cstddef>

#include "texture/png_importer.hpp"
#include "editor_log.hpp"
#include "engine.hpp"
#include "png.h"
#include "texture/texture_2d.hpp"
#include "texture/texture_format_rgb8.hpp"
#include "utility/on_scope_exit.hpp"

namespace atlas
{

Task<> PngImporter::import(const Path& file)
{
    //open file as binary
    FILE* fp = fopen(file.to_string().data(), "rb");
    if (!fp)
    {
        LOG_WARN(editor, "Failed to open file {}", file);
        co_return;
    }

    auto close_file = on_scope_exit([=]() {
        fclose(fp);
    });

    png_byte header[8];
    if (fread(header, 1, 8, fp) != 8)
    {
        LOG_ERROR(editor, "File {} is not a png format", file);
        co_return;
    }

    if (png_sig_cmp(header, 0, 8) != 0)
    {
        LOG_ERROR(editor, "File {} is not a png format", file);
        co_return;
    }

    // create png struct
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
    [](png_structp, png_const_charp msg) {
        LOG_WARN(editor, "Decode png file got error: {}", msg);
    }, [](png_structp, png_const_charp msg) {
        LOG_WARN(editor, "Decode png file got warning: {}", msg);
    });

    if (!png_ptr)
    {
        LOG_ERROR(editor, "Failed to read png file {}", file);
        co_return;
    }

    auto destroy_png_ptr = on_scope_exit([&] {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
    });

    // create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        co_return;
    }

    auto destroy_info_ptr = on_scope_exit([&] {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    });

    // create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info)
    {
        co_return;
    }

    auto destroy_end_ptr = on_scope_exit([&] {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    });

    // init png reading
    png_init_io(png_ptr, fp);

    // already read 8 bit.
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    int32 bit_depth, color_type;
    png_uint_32 twidth, theight;
    png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type, nullptr, nullptr, nullptr);

    const uint32 width = twidth;
    const uint32 height = theight;

    // update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int32 rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // Allocate the image_data as a big block, to be given to opengl
    Array<png_byte> data(rowbytes * height);

    // row_pointers is for pointing to image_data for reading the png with libpng
    Array<png_bytep> row_pointers(height);

    // set the individual row_pointers to point at the correct offsets of image_data
    for (size_t i = 0; i < height; ++i)
    {
        row_pointers[i] = &data[i * rowbytes];
    }

    // read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers.data());

    auto* rgb8 = new TFRGB8(static_cast<uint32>(width), static_cast<uint32>(height));
    Texture2D texture(rgb8);
    if (color_type == PNG_COLOR_TYPE_RGB && bit_depth == 8)
    {
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x<width; x++)
            {
                unsigned char* texel = data.data() + (y * width + x) * 3;
                rgb8->set_color(x, y, Color(texel[0], texel[1], texel[2]));
            }
        }
    }
    else if (color_type == PNG_COLOR_TYPE_RGBA && bit_depth == 8)
    {
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                unsigned char *texel = data.data() + (y * width + x) * 4;
                rgb8->set_color(x, y, Color(texel[0], texel[1], texel[2], texel[3]));
            }
        }
    }
    else
    {
        LOG_WARN(editor, "The color type of PNG is no supported yet.")
    }
}

}// namespace atlas
