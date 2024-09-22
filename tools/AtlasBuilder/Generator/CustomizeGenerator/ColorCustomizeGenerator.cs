using System.Text;

namespace AtlasBuilder.Generator.CustomizeGenerator;

public class ColorCustomizeGenerator : ICustomizeClassGenerator
{
    public static void GenerateBodyMarco(StringBuilder sb)
    {
        
    }

    public static void GenerateRegistrationCode(StringBuilder sb, int numTabs)
    {
        var append = """
                     .add_property(Registration::PropertyReg<uint8>("r", OFFSET_OF(atlas::Color, r))
                         .set_flags(EPropertyFlag::Public)
                         .get())
                     .add_property(Registration::PropertyReg<uint8>("g", OFFSET_OF(atlas::Color, g))
                         .set_flags(EPropertyFlag::Public)
                         .get())
                     .add_property(Registration::PropertyReg<uint8>("b", OFFSET_OF(atlas::Color, b))
                         .set_flags(EPropertyFlag::Public)
                         .get())
                     .add_property(Registration::PropertyReg<uint8>("a", OFFSET_OF(atlas::Color, a))
                         .set_flags(EPropertyFlag::Public)
                         .get()) 
                     """;
        string tabString = new string('\t', numTabs);
        string[] lines = append.Split([ "\r\n", "\r", "\n" ], StringSplitOptions.None);
        for (int i = 0; i < lines.Length; i++)
        {
            lines[i] = tabString + lines[i];
        }
        
        sb.AppendJoin(Environment.NewLine, lines);
        sb.AppendLine();
    }
}