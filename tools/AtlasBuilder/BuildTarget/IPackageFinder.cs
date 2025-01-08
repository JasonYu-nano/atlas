namespace AtlasBuilder.BuildTarget;

public interface IPackageFinder
{
    public static abstract PackageManagerType GetPackageManagerType();

    public static abstract ThirdPartyPackage? FindPackage(string name);
}