// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <fstream>

#include "asset/serialize_example.hpp"
#include "engine.hpp"
#include "serialize/binary_archive.hpp"
#include "serialize/json_archive.hpp"

namespace atlas
{

void SerializeExample::initialize()
{
    serialize_to_json();
    serialize_to_binary();
}

void SerializeExample::deinitialize()
{

}

void SerializeExample::serialize_to_json()
{
    JsonArchiveWriter writer;
    MyStruct a{};
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
    auto task = launch(g_engine->get_llio().async_write(save_file, writer.get_buffer()));
    task.wait();

    std::ifstream f(save_file.to_os_path());
    JsonArchiveReader reader(Json::parse(f));

    MyStruct b{};
    std::memset(&b, 0, sizeof(MyStruct));
    deserialize(reader, b);
    CHECK(b == a, "Deserialize MyStruct failed.");
}

void SerializeExample::serialize_to_binary()
{
    BinaryArchiveWriter writer;
    MyStruct a{};
    serialize(writer, a);


    BinaryArchiveReader reader(writer.get_buffer());

    MyStruct b{};
    std::memset(&b, 0, sizeof(MyStruct));
    deserialize(reader, b);

    CHECK(b == a, "Deserialize MyStruct failed.");
}

}// namespace atlas