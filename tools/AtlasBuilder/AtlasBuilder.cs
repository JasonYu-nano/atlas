// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

using System.CommandLine;
using ToolCore.Utils;
using AtlasBuilder.BuildTarget;
using AtlasBuilder.Generator;

namespace AtlasBuilder;

static class AtlasBuilder
{
    public static DateTime StartTime { get; private set; }
    public static async Task<int> Main(string[] args)
    {
        var buildDefinitionOpt = new Option<bool>(
            name: "--build_definition",
            description: "Whether generate definition files",
            getDefaultValue: () => true);
        
        var buildMetaOpt = new Option<bool>(
            name: "--build_meta",
            description: "Whether generate meta files",
            getDefaultValue: () => true);
        
        var rootCommand = new RootCommand("");
        rootCommand.AddOption(buildDefinitionOpt);
        rootCommand.AddOption(buildMetaOpt);
        
        rootCommand.SetHandler(async context =>
        {
            bool buildDefinition = context.ParseResult.GetValueForOption(buildDefinitionOpt);
            bool buildMeta = context.ParseResult.GetValueForOption(buildMetaOpt);
            context.ExitCode = await GenerateImpl(buildDefinition, buildMeta);
        });

        return rootCommand.InvokeAsync(args).Result;
    }

    private static async Task<int> GenerateImpl(bool buildDefinition, bool buildMeta)
    {
        var errorCode = 0;
        StartTime = DateTime.Now;
        Console.WriteLine($"-----------------{StartTime:yyyy-MM-dd HH:mm:ss}-----------------");
        Console.WriteLine("Start running atlas builder");
        BuildTargetAssembly buildTargetAssembly = new BuildTargetAssembly();
        try
        {
            buildTargetAssembly.Initialize(DirectoryUtils.EngineRootDirectory);

            if (buildDefinition)
            {
                var defGen = new DefinitionsGenerator(buildTargetAssembly);
                await defGen.Generate();
                
                var endTime = DateTime.Now;
                var timeSpan = endTime - StartTime;
                Console.WriteLine($"DefinitionsGenerator finished in {timeSpan.TotalSeconds} seconds");
            }

            if (buildMeta)
            {
                var metaGen = new MetaGenerator(buildTargetAssembly);
                await metaGen.Generate();
            }
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
            errorCode = 1;
        }
        finally
        {
            buildTargetAssembly.Deinitialize();
            var endTime = DateTime.Now;
            var timeSpan = endTime - StartTime;
            Console.WriteLine($"Atlas builder finished running in {timeSpan.TotalSeconds} seconds");
        }

        return errorCode;
    }
}

