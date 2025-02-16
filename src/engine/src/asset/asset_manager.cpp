// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "asset/asset_manager.hpp"

#include "asset/package_loader.hpp"
#include "engine.hpp"

namespace atlas
{

AssetManager::AssetManager()
{
    package_loader_ = std::make_unique<PackageLoader>(g_engine->get_llio());
}

}// namespace atlas
