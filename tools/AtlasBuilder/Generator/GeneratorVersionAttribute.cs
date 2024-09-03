namespace AtlasBuilder.Generator;

/// <summary>
/// Version attribute for code generator.
/// </summary>
/// <param name="version"></param>
[AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct)]
public class GeneratorVersionAttribute(string version) : Attribute
{
    public Version Version { get; } = Version.Parse(version);
}