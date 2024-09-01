// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

namespace AtlasBuilder.BuildTarget;

public class LibraryBuildTarget : BuildTargetBase
{
    public LibraryBuildTarget(string targetName, string rootDir) : base(targetName, rootDir)
    {
    }
    
    public LibraryBuildTarget(string targetName, string rootDirectory, HashSet<string> options, Dictionary<string, string> oneValueArgs, Dictionary<string, List<string>> multiValueArgs)
        : base(targetName, rootDirectory, options, oneValueArgs, multiValueArgs)
    {
    }

    public override bool NeedExportSymbol()
    {
        return true;
    }
}