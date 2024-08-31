using System.Text.RegularExpressions;
using AtlasBuildTool.BuildTarget;
using CppAst;
using ToolCore.Utils;

namespace AtlasBuildTool.Generator;

public class MetaGenerator(BuildTargetAssembly buildTargetAssembly)
{
    private BuildTargetAssembly _buildTargetAssembly = buildTargetAssembly;

    public async Task Generate()
    {
        var cc = await Parse();
    }

    private async Task<CppCompilation?> Parse()
    {
        List<Task<IEnumerable<string>>> tasks = new();
        foreach (var buildTarget in _buildTargetAssembly.NameToBuildTargets.Values)
        {
            tasks.Add(GetPendingParseHeader(buildTarget));
        }

        var result = await Task.WhenAll(tasks.ToArray());
        List <string> headers = new();
        foreach (var v in result)
        {
            headers.AddRange(v);
        }

        if (headers.Count > 0)
        {
            var options = MakeCppParserOptions();
            var cc = CppParser.ParseFiles(headers, options);
            return cc;
        }
        
        return null;
    }

    private CppParserOptions MakeCppParserOptions()
    {
        var options = new CppParserOptions 
        {
            ParseSystemIncludes = false,
            KeepGoing = true
        };
            
        options.AdditionalArguments.AddRange([
            $"-include{Path.Combine(DirectoryUtils.BuildTargetIntermediateDirectory, "no_export_definitions.hpp")}", 
            "-std=c++23",
            "-DATLAS_BUILDER",
            "-Wno-microsoft-include"
        ]);
            
        foreach (var nameToBuildTarget in _buildTargetAssembly.NameToBuildTargets)
        {
            options.IncludeFolders.Add(Path.Combine(nameToBuildTarget.Value.RootDirectory, "include"));
        }

        foreach (var nameToPackage in _buildTargetAssembly.NameToThirdPartyPackages)
        {
            options.IncludeFolders.AddRange(nameToPackage.Value.IncludeDirs);
            options.AdditionalArguments.AddRange(nameToPackage.Value.Definitions);
        }

        return options;
    }

    private async Task<IEnumerable<string>> GetPendingParseHeader(BuildTargetBase buildTarget)
    {
        var headers = Directory.GetFiles(buildTarget.RootDirectory, "*.hpp", SearchOption.AllDirectories);
        Console.WriteLine($"{headers.Length}");
        return headers.TakeWhile(header =>
        {
            using var reader = new StreamReader(header);
            var file = reader.ReadToEnd();
            return Regex.Count(file, @"(?<!#define\s)META\([^)]*\)") > 0;
        });
    }
}