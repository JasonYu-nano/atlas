// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "asset/serialize_sample.hpp"

#include <fstream>

#include "engine.hpp"
#include "serialize/json_archive.hpp"

namespace atlas
{

struct SampleStruct
{
    int8 i8 = 0;
    bool is_class = false;
    String name = "SampleStruct";

    friend void serialize(WriteStream& ws, const SampleStruct& v)
    {
        ws << v.i8 << v.is_class << v.name;
    }

    friend void deserialize(ReadStream& rs, SampleStruct& v)
    {
        rs >> v.i8;
        rs >> v.is_class;
        rs >> v.name;
    }
};

void SerializeSample::initialize()
{
    JsonArchiveWriter writer;
    SampleStruct s{125};
    serialize(writer, s);

    auto save_dir = Directory::get_engine_directory() / "examples" / "empty_project"/ "temp";
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

    SampleStruct ss{};
    deserialize(reader, ss);
    CHECK(ss.i8 == 125, "Deserialize SampleStruct failed.");
}

void SerializeSample::deinitialize()
{

}

}// namespace atlas