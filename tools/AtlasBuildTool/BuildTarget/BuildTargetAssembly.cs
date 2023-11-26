using AtlasBuildTool.CMake;

namespace AtlasBuildTool.BuildTarget;

public class BuildTargetAssembly
{
    public Dictionary<string, BuildTargetBase> NameToBuildTargets { get; } = new Dictionary<string, BuildTargetBase>();
    
    public void Initialize(string topLevelDirectory)
    {
        if (!Directory.Exists(topLevelDirectory))
        {
            throw new BuildTargetException($"no such directory: {topLevelDirectory}");
        }
        
        foreach (var subDirectory in Directory.EnumerateDirectories(topLevelDirectory))
        {
            var cmakeListsFile = $"{subDirectory}/CMakeLists.txt";
            if (File.Exists(cmakeListsFile))
            {
                if (Directory.Exists($"{subDirectory}/include") || Directory.Exists($"{subDirectory}/src"))
                {
                    CreateBuildTarget(subDirectory, cmakeListsFile);
                }
            }
        }

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
        foreach (var nameToBuildTarget in NameToBuildTargets)
        {
            BuildTargetBase buildTarget = nameToBuildTarget.Value;
            if (buildTarget.MultiValueArgs.TryGetValue(CMakeTypes.DependencyTargets, out var targetNames))
            {
                foreach (var targetName in targetNames)
                {
                    if (NameToBuildTargets.TryGetValue(targetName, out var target))
                    {
                        if (!buildTarget.Dependencies.Contains(target))
                        {
                            buildTarget.Dependencies.Add(target);
                        }
                    }
                }
            }
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