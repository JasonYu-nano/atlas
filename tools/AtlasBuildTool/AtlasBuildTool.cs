// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

using ToolCore.Utils;
using AtlasBuildTool.BuildTarget;
using AtlasBuildTool.Generator;

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
            buildTargetAssembly.initialize(DirectoryUtils.EngineRootDirectory);
            var generator = new DefinitionsGenerator(buildTargetAssembly);
            var task = generator.GenerateAsync();
            Task.WhenAny(task);
        }
        catch (BuildTargetException e)
        {
            Console.WriteLine(e);
        }
        catch (GeneratorException e)
        {
            Console.WriteLine(e);
        }
        catch (Exception e)
        {
            Console.WriteLine(e);
        }
        finally
        {
            buildTargetAssembly.deinitialize();
            var endTime = DateTime.Now;
            var timeSpan = endTime - startTime;
            Console.WriteLine($"atlas build tool finished running in {timeSpan.TotalSeconds} seconds");
        }

        return 0;
    }
}

