using System.Runtime.InteropServices;
using System.Text.RegularExpressions;
using ToolCore.Utils;

namespace AtlasBuilder.BuildTarget;

public class VcpkgFinder : IPackageFinder
{
    private const string VcpkgInstallDirName = "vcpkg_installed";
    
    public static PackageManagerType GetPackageManagerType() => PackageManagerType.Vcpkg;

    public static ThirdPartyPackage? FindPackage(string name)
    {
        string installDir = Path.Combine(DirectoryUtils.GetEngineBuildDirectory(BuildCommand.BuildType),
            VcpkgInstallDirName, GetTargetTripletDir());
        if (!Path.Exists(installDir))
        {
            return null;
        }

        var includeDirs = Path.Combine(installDir, "include");
        
        return new ThirdPartyPackage(name, includeDirs, [], []);
    }

    private static string GetTargetTripletDir()
    {
        string ret;
        switch (BuildCommand.ArchType)
        {
            case ArchType.X86:
                ret = "x86";
                break;
            case ArchType.X64:
                ret = "x64";
                break;
            case ArchType.Arm:
                ret = "arm";
                break;
            case ArchType.Arm64:
                ret = "arm64";
                break;
            default:
                throw new ArgumentOutOfRangeException();
        }

        switch (BuildCommand.TargetPlatform)
        {
            case TargetPlatform.Windows:
                ret += "-windows";
                break;
            case TargetPlatform.MacOS:
                ret += "-osx";
                break;
            default:
                throw new ArgumentOutOfRangeException();
        }

        return ret;
    }
}