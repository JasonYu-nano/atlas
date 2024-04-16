// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

namespace AtlasBuildTool.CMake;

public static class CMakeTypes
{
    public const string EditorOnly = "EDITOR_ONLY";
    public const string SkipMoc = "SKIP_MOC";
    public const string TestTarget = "TEST_TARGET";
    public const string GameTarget = "GAME_TARGET";
    public const string Target = "TARGET";
    public const string PublicDefinitions = "PUBLIC_DEFINITIONS";
    public const string PrivateDefinitions = "PRIVATE_DEFINITIONS";
    public const string PublicIncludeDirs = "PUBLIC_INCLUDE_DIRS";
    public const string PrivateIncludeDirs = "PRIVATE_INCLUDE_DIRS";
    public const string PublicLinkLib = "PUBLIC_LINK_LIB";
    public const string PrivateLinkLib = "PRIVATE_LINK_LIB";
    public const string DependencyTargets = "DEPENDENCY_TARGETS";
    public const string PlatformCodeDirs = "PLATFORM_CODE_DIRS";

    public static readonly string[] Options = { EditorOnly, SkipMoc, TestTarget, GameTarget };
    public static readonly string[] OneValueArgs = { Target };
    public static readonly string[] MultiValueArgs =
    {
        PublicDefinitions, PrivateDefinitions, PublicIncludeDirs, PrivateIncludeDirs,
        PublicLinkLib, PrivateLinkLib, DependencyTargets, PlatformCodeDirs
    };

    public const string AddLibraryFunction = "add_atlas_library";
    public const string AddExecutableFunction = "add_atlas_executable";
}