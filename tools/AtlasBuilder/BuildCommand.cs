﻿using System.CommandLine;
using ToolCore.Utils;

namespace AtlasBuilder;

public static class BuildCommand
{
    public static bool IsBuildDefinition { get; private set; }
    public static bool IsBuildMeta { get; private set; }
    public static TargetPlatform TargetPlatform { get; private set; }
    public static PackageManagerType PackageManager { get; private set; }
    public static BuildType BuildType { get; private set; }
    public static ArchType ArchType { get; private set; }
    public static RootCommand Command { get; private set; }
    private static Func<Task<int>>? _handler;

    static BuildCommand()
    {
        var buildDefinitionOpt = new Option<bool>(
            name: "--build_definition",
            description: "Whether generate definition files",
            getDefaultValue: () => true);
        
        var buildMetaOpt = new Option<bool>(
            name: "--build_meta",
            description: "Whether generate meta files",
            getDefaultValue: () => true);
        
        var targetPlatformOpt = new Option<TargetPlatform>(
            name: "--target_platform",
            description: "Which platform to build",
            getDefaultValue: GetDefaultTargetPlatform);
        
        var pkgManagerOpt = new Option<PackageManagerType>(
            name: "--package_manager",
            description: "Which package manager to use",
            getDefaultValue: () => PackageManagerType.Vcpkg);

        var buildOpt = new Option<BuildType>(
            name: "--build_type",
            description: "Which type to build",
            getDefaultValue: () => BuildType.Debug);
        
        var archOpt = new Option<ArchType>(
            name: "--arch",
            description: "Which architecture to build",
            getDefaultValue: () => ArchType.X64);

        Command = new RootCommand("Build Command");
        Command.AddOption(buildDefinitionOpt);
        Command.AddOption(buildMetaOpt);
        Command.AddOption(targetPlatformOpt);
        Command.AddOption(pkgManagerOpt);
        Command.AddOption(buildOpt);
        Command.AddOption(archOpt);
            
        Command.SetHandler(async context =>
        {
            IsBuildDefinition = context.ParseResult.GetValueForOption(buildDefinitionOpt);
            IsBuildMeta = context.ParseResult.GetValueForOption(buildMetaOpt);
            TargetPlatform = context.ParseResult.GetValueForOption(targetPlatformOpt);
            PackageManager = context.ParseResult.GetValueForOption(pkgManagerOpt);
            BuildType = context.ParseResult.GetValueForOption(buildOpt);
            ArchType = context.ParseResult.GetValueForOption(archOpt);

            context.ExitCode = _handler != null ? await _handler.Invoke() : 1;
        });
    }

    public static void SetHandler(Func<Task<int>> handler)
    {
        _handler ??= handler;
    }

    private static TargetPlatform GetDefaultTargetPlatform()
    {
        if (System.OperatingSystem.IsWindows())
        {
            return TargetPlatform.Windows;
        }
        
        if (System.OperatingSystem.IsMacOS())
        {
            return TargetPlatform.MacOS;
        }
        
        return TargetPlatform.Unknown;
    }
}