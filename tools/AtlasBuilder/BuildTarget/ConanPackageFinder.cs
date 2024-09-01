using System.Text.RegularExpressions;
using ToolCore.Utils;

namespace AtlasBuilder.BuildTarget;

public class ConanPackageFinder : IPackageFinder
{
    public static ThirdPartyPackage? FindPackage(string name)
    {
        var buildDir = DirectoryUtils.GetEngineBuildDirectory(BuildType.Debug);
        var conanDir = Path.Combine(buildDir, "conan", "build", "Debug", "generators");
        if (!Path.Exists(conanDir))
        {
            return null;
        }

        var packageName = name;
        if (name.Contains("::"))
        {
            packageName = name.Split("::")[0];
        }
        
        var files = Directory.GetFiles(conanDir, $"{packageName}*-data.cmake", SearchOption.TopDirectoryOnly);
        if (files.Length <= 0)
        {
            return null;
        }
        
        var dependencyPattern = @"set\(.+_FIND_DEPENDENCY_NAMES";
        var dependencyPattern2 = @"list\(APPEND .+_FIND_DEPENDENCY_NAMES";
        var packagePattern =  @"set\(.+_PACKAGE_FOLDER";
        var definitionPattern =  @"set\(.+_DEFINITIONS";
        
        List<string>? packageFolder = null;
        List<string>? dependency = null;
        List<string>? definitions = null;

        using var reader = new StreamReader(files[0]);
        var line = reader.ReadLine();
        while (line != null)
        {
            if (dependency == null && Regex.Count(line, dependencyPattern) > 0)
            {
                dependency = new();
            }
            
            if (dependency == null && Regex.Count(line, dependencyPattern2) > 0)
            {
                dependency = ParseListAppendExpressionArgument(line);
            }
            
            if (packageFolder == null && Regex.Count(line, packagePattern) > 0)
            {
                packageFolder = ParseSetExpressionArgument(line);
            }

            if (definitions == null && Regex.Count(line, definitionPattern) > 0)
            {
                definitions = ParseSetExpressionArgument(line);
            }
            
            line = reader.ReadLine();
        }

        if (packageFolder == null || dependency == null || definitions == null)
        {
            throw new Exception($"ConanPackageFinder: unrecognized package: {name}");
        }
        
        if (packageFolder.Count <= 0)
        {
            throw new Exception("ConanPackageFinder: package folder not found");
        }

        if (packageFolder.Count > 1)
        {
            throw new Exception("ConanPackageFinder: More than one package folder found");
        }

        var includeDirs = $"{packageFolder[0]}/include";
        
        return new ThirdPartyPackage(name, includeDirs, definitions, dependency);
    }


    private static List<string> ParseSetExpressionArgument(string stmt)
    {
        List<string> args = new();
        int startIndex = -1;
        for (int i = 0; i < stmt.Length; i++)
        {
            if (stmt[i] == '"')
            {
                if (startIndex != -1)
                {
                    args.Add(stmt.Substring(startIndex, i - startIndex));
                    startIndex = -1;
                }
                else
                {
                    startIndex = i + 1;
                }
            }
        }

        return args;
    }
    
    private static List<string> ParseListAppendExpressionArgument(string stmt)
    {
        List<string> args = new();
        var startIndex = stmt.IndexOf('(');
        var endIndex = stmt.IndexOf(')');
        if (startIndex != -1 && endIndex != -1)
        {
            var sub = stmt.Substring(startIndex + 1, endIndex - startIndex - 1);
            var tokens = sub.Split(" ");
            if (tokens.Length > 1)
            {
                args.AddRange(tokens.Skip(2));
            }
        }

        return args;
    }
}