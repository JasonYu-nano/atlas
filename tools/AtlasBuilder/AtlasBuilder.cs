// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

using System.Text;
using ToolCore.Utils;
using AtlasBuilder.BuildTarget;
using AtlasBuilder.Generator;
using ClangSharp.Interop;
using CppAst;

namespace AtlasBuilder;

static class AtlasBuilder
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
            var defGen = new DefinitionsGenerator(buildTargetAssembly);
            var task = defGen.Generate();
            task.Wait();
            
            var endTime = DateTime.Now;
            var timeSpan = endTime - startTime;
            Console.WriteLine($"DefinitionsGenerator finished in {timeSpan.TotalSeconds} seconds");

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
            buildTargetAssembly.Deinitialize();
            var endTime = DateTime.Now;
            var timeSpan = endTime - startTime;
            Console.WriteLine($"atlas build tool finished running in {timeSpan.TotalSeconds} seconds");
        }

        return 0;
    }
}

