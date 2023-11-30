// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

namespace AtlasBuildTool.BuildTarget;

public class ExecutableBuildTarget : BuildTargetBase
{
    public ExecutableBuildTarget(string targetName, string rootDirectory) : base(targetName, rootDirectory)
    {
    }
    
    public ExecutableBuildTarget(string targetName, string rootDirectory, HashSet<string> options, Dictionary<string, string> oneValueArgs, Dictionary<string, List<string>> multiValueArgs)
        : base(targetName, rootDirectory, options, oneValueArgs, multiValueArgs)
    {
    }

    public override bool NeedExportSymbol()
    {
        return false;
    }
}