// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

namespace AtlasBuilder.BuildTarget;

class BuildTargetException : Exception
{
    public BuildTargetException(string message) : base(message)
    {
    }
}

public abstract class BuildTargetBase : IComparable<BuildTargetBase>
{
    public string TargetName { get; }
    
    public string RootDirectory { get; }

    public List<BuildTargetBase> PublicLinkBuildTargets { get; set; } = new List<BuildTargetBase>();
    
    public List<BuildTargetBase> PrivateLinkBuildTargets { get; set; } = new List<BuildTargetBase>();
    
    public HashSet<string> Options { get; } = new HashSet<string>();

    public Dictionary<string, string> OneValueArgs { get; } = new Dictionary<string, string>();

    public Dictionary<string, List<string>> MultiValueArgs { get; } = new Dictionary<string, List<string>>();

    protected BuildTargetBase(string targetName, string rootDirectory)
    {
        TargetName = targetName;
        RootDirectory = rootDirectory;
    }
    
    protected BuildTargetBase(string targetName, string rootDirectory, HashSet<string> options, Dictionary<string, string> oneValueArgs, Dictionary<string, List<string>> multiValueArgs)
    {
        TargetName = targetName;
        RootDirectory = rootDirectory;
        Options = options;
        OneValueArgs = oneValueArgs;
        MultiValueArgs = multiValueArgs;
    }

    public abstract bool NeedExportSymbol();

    public void GetAllDependenciesBuildTargets(SortedSet<BuildTargetBase> dependencies)
    {
        foreach (var linkTarget in PublicLinkBuildTargets)
        {
            linkTarget.GetAllDependenciesBuildTargets(dependencies);
            dependencies.Add(linkTarget);
        }
        foreach (var linkTarget in PrivateLinkBuildTargets)
        {
            linkTarget.GetAllDependenciesBuildTargets(dependencies);
            dependencies.Add(linkTarget);
        }
    }

    public int CompareTo(BuildTargetBase? obj)
    {
        return String.Compare(TargetName, obj?.TargetName, StringComparison.Ordinal);
    }
}