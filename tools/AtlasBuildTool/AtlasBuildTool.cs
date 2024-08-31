// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

using System.Text;
using ToolCore.Utils;
using AtlasBuildTool.BuildTarget;
using AtlasBuildTool.Generator;
using ClangSharp.Interop;
using CppAst;

namespace AtlasBuildTool;

static class AtlasBuildTool
{
    public static int Main(string[] args)
    {
        var startTime = DateTime.Now;
        Console.WriteLine($"-----------------{startTime:yyyy-MM-dd HH:mm:ss}-----------------");
        Console.WriteLine("start running atlas build tool");
        BuildTargetAssembly buildTargetAssembly = new BuildTargetAssembly();
        try
        {
            buildTargetAssembly.Initialize(DirectoryUtils.EngineRootDirectory);
            var generator = new DefinitionsGenerator(buildTargetAssembly);
            var task = generator.GenerateAsync();
            task.Wait();
            
            var endTime = DateTime.Now;
            var timeSpan = endTime - startTime;
            Console.WriteLine($"DefinitionsGenerator finished running in {timeSpan.TotalSeconds} seconds");

            var metaGen = new MetaGenerator(buildTargetAssembly);
            var result = metaGen.Generate();
            result.Wait();
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
        }
        finally
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
            
            foreach (var nameToBuildTarget in buildTargetAssembly.NameToBuildTargets)
            {
                options.IncludeFolders.Add(Path.Combine(nameToBuildTarget.Value.RootDirectory, "include"));
            }

            foreach (var nameToPackage in buildTargetAssembly.NameToThirdPartyPackages)
            {
                options.IncludeFolders.AddRange(nameToPackage.Value.IncludeDirs);
                options.AdditionalArguments.AddRange(nameToPackage.Value.Definitions);
            }

            var target = buildTargetAssembly.NameToBuildTargets["core"];
            string p = Path.Combine(target.RootDirectory, "include");
            var files = Directory.GetFiles(p, "*.hpp", SearchOption.AllDirectories);
            var nfiles = files.Where((a) => !a.Contains("platform\\mac"));
            
            var cc = CppParser.ParseFiles([..nfiles, "C:/Code/atlas/test/test_core/src/test_types.hpp"], options);
            
            
            
            if (!cc.HasErrors)
            {
                foreach(var cppStruct in cc.Classes)
                {
                    // Skip non struct
                    if (cppStruct.ClassKind != CppClassKind.Struct) continue;
                    Console.WriteLine($"struct {cppStruct.Name}");
            
                    // Print all fields
                    foreach(var cppField in cppStruct.Fields)
                        Console.WriteLine($"   {cppField}");
            
                    Console.WriteLine("}");
                }
            }
            
            buildTargetAssembly.Deinitialize();
            var endTime = DateTime.Now;
            var timeSpan = endTime - startTime;
            Console.WriteLine($"atlas build tool finished running in {timeSpan.TotalSeconds} seconds");
        }

        return 0;
    }
}

