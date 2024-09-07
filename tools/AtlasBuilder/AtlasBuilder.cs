﻿// Copyright(c) 2023-present, Atlas.
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
    public static DateTime StartTime { get; private set; }
    public static int Main(string[] args)
    {
        var errorCode = 0;
        StartTime = DateTime.Now;
        Console.WriteLine($"-----------------{StartTime:yyyy-MM-dd HH:mm:ss}-----------------");
        Console.WriteLine("Start running atlas builder");
        BuildTargetAssembly buildTargetAssembly = new BuildTargetAssembly();
        try
        {
            buildTargetAssembly.Initialize(DirectoryUtils.EngineRootDirectory);
            var defGen = new DefinitionsGenerator(buildTargetAssembly);
            defGen.Generate().Wait();
            
            var endTime = DateTime.Now;
            var timeSpan = endTime - StartTime;
            Console.WriteLine($"DefinitionsGenerator finished in {timeSpan.TotalSeconds} seconds");

            var metaGen = new MetaGenerator(buildTargetAssembly);
            metaGen.Generate().Wait();
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

