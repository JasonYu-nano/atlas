using System.CommandLine;
using ToolCore.Utils;

namespace AtlasBuilder;

public static class BuildCommand
{
    public static bool IsBuildDefinition { get; private set; }
    public static bool IsBuildMeta { get; private set; }
    public static TargetPlatform TargetPlatform { get; private set; }
    public static PackageManagerType PackageManager { get; private set; }
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

        Command = new RootCommand("Build Command");
        Command.AddOption(buildDefinitionOpt);
        Command.AddOption(buildMetaOpt);
        Command.AddOption(targetPlatformOpt);
        Command.AddOption(pkgManagerOpt);
            
        Command.SetHandler(async context =>
        {
            IsBuildDefinition = context.ParseResult.GetValueForOption(buildDefinitionOpt);
            IsBuildMeta = context.ParseResult.GetValueForOption(buildMetaOpt);
            TargetPlatform = context.ParseResult.GetValueForOption(targetPlatformOpt);
            PackageManager = context.ParseResult.GetValueForOption(pkgManagerOpt);

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