namespace AtlasBuilder.BuildTarget;

public interface IPackageFinder
{
    public static abstract ThirdPartyPackage? FindPackage(string name);
}