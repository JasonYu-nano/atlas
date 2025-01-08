using System.Text.RegularExpressions;
using ToolCore.Utils;

namespace AtlasBuilder.BuildTarget;

public class VcpkgFinder : IPackageFinder
{
    private const string VcpkgInstallDirName = "vcpkg_installed";
    
    public static PackageManagerType GetPackageManagerType() => PackageManagerType.Vcpkg;

    public static ThirdPartyPackage? FindPackage(string name)
    {
        var installDir = Path.Combine(DirectoryUtils.EngineRootDirectory, VcpkgInstallDirName);
        installDir = Path.Combine(installDir, GetTargetTripletDir(BuildCommand.TargetPlatform));
        if (!Path.Exists(installDir))
        {
            return null;
        }

        var includeDirs = Path.Combine(installDir, "include");
        
        return new ThirdPartyPackage(name, includeDirs, [], []);
    }

    private static string GetTargetTripletDir(TargetPlatform platform)
    {
        switch (platform)
        {
            case TargetPlatform.Windows:
                return "x64-windows";
            case TargetPlatform.MacOS:
            default:
                return "";
        }
    }
}