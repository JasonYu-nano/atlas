// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

using System.Runtime.InteropServices;

namespace ToolCore.Utils;

public static class DirectoryUtils
{
    public static readonly string EngineRootDirectory = GetEngineRootDirectory();
    
    public static readonly string EngineIntermediateDirectory = $"{EngineRootDirectory}/intermediate";
    
    public static readonly string EngineSourceDirectory = $"{EngineRootDirectory}/src";
    
    public static readonly string BuildTargetIntermediateDirectory = $"{EngineIntermediateDirectory}/build_targets";

    private static string GetEngineRootDirectory()
    {
        string rootName = $"{Path.DirectorySeparatorChar}atlas{Path.DirectorySeparatorChar}";
        var workingDir = AppContext.BaseDirectory;
        var index = workingDir.LastIndexOf(rootName, StringComparison.Ordinal);
        return workingDir.Remove(index + rootName.Length - 1);
    }
}