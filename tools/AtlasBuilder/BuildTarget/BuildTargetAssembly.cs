// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

using System.Diagnostics;
using System.Reflection;
using AtlasBuilder.CMake;
using ToolCore.Utils;

namespace AtlasBuilder.BuildTarget;

public class BuildTargetAssembly
{
    public Dictionary<string, BuildTargetBase> NameToBuildTargets { get; } = new();
    public Dictionary<string, ThirdPartyPackage> NameToThirdPartyPackages { get; } = new();
    private List<MethodInfo> _packageFinders = [];
    
    public void Initialize(string topLevelDirectory)
    {
        if (!Directory.Exists(topLevelDirectory))
        {
            throw new BuildTargetException($"no such directory: {topLevelDirectory}");
        }

        InitializeBuildTargetsInDirectory(topLevelDirectory);

        BuildDependency();
    }

    public void Deinitialize()
    {
        NameToBuildTargets.Clear();
    }
    
    public BuildTargetBase? GetBuildTarget(string targetName)
    {
        return NameToBuildTargets[targetName];
    }

    private void BuildDependency()
    {
        List<string> packageNames = new();
        
        foreach (var nameToBuildTarget in NameToBuildTargets)
        {
            BuildTargetBase buildTarget = nameToBuildTarget.Value;
            if (buildTarget.MultiValueArgs.TryGetValue(CMakeTypes.PublicLinkLib, out var publicLinkNames))
            {
                foreach (var linkName in publicLinkNames)
                {
                    if (NameToBuildTargets.TryGetValue(linkName, out var target))
                    {
                        if (!buildTarget.PublicLinkBuildTargets.Contains(target))
                        {
                            buildTarget.PublicLinkBuildTargets.Add(target);
                        }
                    }
                    else
                    {
                        packageNames.AddUnique(linkName);
                    }
                }
            }
            
            if (buildTarget.MultiValueArgs.TryGetValue(CMakeTypes.PrivateLinkLib, out var privateLinkNames))
            {
                foreach (var linkName in privateLinkNames)
                {
                    if (NameToBuildTargets.TryGetValue(linkName, out var target))
                    {
                        if (!buildTarget.PrivateLinkBuildTargets.Contains(target))
                        {
                            buildTarget.PrivateLinkBuildTargets.Add(target);
                        }
                    }
                    else
                    {
                        packageNames.AddUnique(linkName);
                    }
                }
            }
        }

        ResolveThirdPartyPackages(packageNames);
    }

    private void InitializeBuildTargetsInDirectory(in string directory)
    {
        if (!Directory.Exists(directory))
        {
            throw new BuildTargetException($"no such directory: {directory}");
        }
        
        var cmakeListsFile = $"{directory}/CMakeLists.txt";
        if (!File.Exists(cmakeListsFile))
        {
            return;
        }
        
        var buildTarget = CreateBuildTarget(directory, cmakeListsFile);
        if (buildTarget != null)
        {
            return;
        }
        
        foreach (var subDirectory in Directory.EnumerateDirectories(directory))
        {
            InitializeBuildTargetsInDirectory(subDirectory);
        }
    }

    private void ResolveThirdPartyPackages(List<string> packageNames)
    {
        if (_packageFinders.Count <= 0)
        {
            var interfaceType = typeof(IPackageFinder);
            var assembly = Assembly.GetExecutingAssembly();
            var implementingTypes = assembly.GetTypes()
                .Where(t => interfaceType.IsAssignableFrom(t) && t is { IsClass: true, IsAbstract: false });
            
            foreach (var type in implementingTypes)
            {
                var finder = type.GetMethod("FindPackage", BindingFlags.Public | BindingFlags.Static);
                Debug.Assert(finder != null);
                _packageFinders.Add(finder);
            }
        }

        int idx = 0;
        while (packageNames.Count > idx)
        {
            var packageName = packageNames[idx];
            foreach (var finder in _packageFinders)
            {
                var package = finder.Invoke(null, [packageName]);
                if (package is ThirdPartyPackage thirdPartyPackage)
                {
                    NameToThirdPartyPackages.Add(packageName, thirdPartyPackage);
                    foreach (var dep in thirdPartyPackage.Dependencies)
                    {
                        packageNames.AddUnique(dep);
                    }
                    break;
                }
            }
            ++idx;
        }
    }

    private BuildTargetBase? CreateBuildTarget(in string rootDirectory, in string cmakeListsFile)
    {
        string cmakeContent;
        
        try
        {
            using StreamReader reader = new StreamReader(cmakeListsFile);
            cmakeContent = reader.ReadToEnd();
        }
        catch (FileNotFoundException e)
        {
            throw new BuildTargetException($"no such file: {e}");
        }
        catch (DirectoryNotFoundException e)
        {
            throw new BuildTargetException($"no such file: {e}");
        }
        catch (IOException e)
        {
            throw new BuildTargetException($"read file: {e} failed");
        }
        
        string args = GetBuildTargetArgumentsFromCMakeList(cmakeContent, out bool isExecutable);
        if (args == string.Empty)
        {
            return null;
        }

        BuildTargetBase? buildTarget = null;
        CMakeParser.ParserCMakeArgs(args, out var options,
            out var oneValueArgs, out var multiValueArgs);

        if (oneValueArgs.TryGetValue("TARGET", out var targetName))
        {
            if (isExecutable)
            {
                buildTarget = new ExecutableBuildTarget(targetName, rootDirectory, options, oneValueArgs, multiValueArgs);
            }
            else
            {
                buildTarget = new LibraryBuildTarget(targetName, rootDirectory, options, oneValueArgs, multiValueArgs);
            }

            NameToBuildTargets.Add(targetName, buildTarget);
        }
        else
        {
            throw new BuildTargetException(
                $"can't find TARGET in build target arguments. file path: {cmakeListsFile}");
        }

        return buildTarget;
    }
    
    private string GetBuildTargetArgumentsFromCMakeList(in string content, out bool isExecutable)
    {
        isExecutable = false;

        int funStartIndex = 0;
        if ((funStartIndex = content.IndexOf(CMakeTypes.AddExecutableFunction, StringComparison.Ordinal)) != -1)
        {
            isExecutable = true;
        }
        else if ((funStartIndex = content.IndexOf(CMakeTypes.AddLibraryFunction, StringComparison.Ordinal)) != -1)
        {
            isExecutable = false;
        }
        else if ((funStartIndex = content.IndexOf(CMakeTypes.AddProjectFunction, StringComparison.Ordinal)) != -1)
        {
            isExecutable = false;
        }
        else
        {
            return string.Empty;
        }

        int leftParenthesisIndex = content.IndexOf('(', funStartIndex);
        int rightParenthesisIndex = content.IndexOf(')', funStartIndex);
        if (leftParenthesisIndex == -1 || rightParenthesisIndex == -1 || rightParenthesisIndex - leftParenthesisIndex <= 1)
        {
            return string.Empty;
        }

        string args =
            content.Substring(leftParenthesisIndex + 1, rightParenthesisIndex - leftParenthesisIndex - 1);

        return args;
    }
}