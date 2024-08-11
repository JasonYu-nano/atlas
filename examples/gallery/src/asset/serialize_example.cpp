// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <fstream>

#include "asset/serialize_example.hpp"
#include "engine.hpp"

namespace atlas
{

void SerializeExample::initialize()
{
    JsonArchiveWriter writer;
    MyStruct a{125};
    serialize(writer, a);

    auto save_dir = Directory::get_engine_directory() / "examples" / "gallery"/ "temp";
    if (!save_dir.exists())
    {
        Directory::make_dir_tree(save_dir);
    }
    ASSERT(save_dir.exists());
    auto save_file = save_dir / "sample_struct_data.json";
    if (!save_file.exists())
    {
        std::ofstream file(save_file.to_std_path());
        file.close();
    }

    ASSERT(g_engine);
    auto task = launch(g_engine->get_llio().async_write(save_file, writer.get_stream_buffer()));
    task.wait();

    std::ifstream f(save_file.to_os_path());
    JsonArchiveReader reader(Json::parse(f));

    MyStruct b{};
    deserialize(reader, b);
    CHECK(b.i8 == 125, "Deserialize SampleStruct failed.");
}

void SerializeExample::deinitialize()
{

}

}// namespace atlas