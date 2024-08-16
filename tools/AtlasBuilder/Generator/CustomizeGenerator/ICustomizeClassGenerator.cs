using System.Text;

namespace AtlasBuilder.Generator.CustomizeGenerator;

public interface ICustomizeClassGenerator
{
    public static abstract void GenerateBodyMarco(StringBuilder sb);
    
    public static abstract void GenerateRegistrationCode(StringBuilder sb, int numTabs);
}