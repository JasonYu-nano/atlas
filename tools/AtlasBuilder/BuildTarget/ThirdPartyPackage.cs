namespace AtlasBuilder.BuildTarget;

public class ThirdPartyPackage
{
    public ThirdPartyPackage(string name, string includeDirs, List<string> definitions, List<string> dependencies)
    {
        Name = name;
        IncludeDirs.Add(includeDirs);
        Definitions.AddRange(definitions);
        Dependencies.AddRange(dependencies);
    }

    public string Name { get; } = "";
    public List<string> IncludeDirs { get; } = new ();
    public List<string> Definitions { get; } = new ();
    
    public List<string> Dependencies { get; } = new ();
}