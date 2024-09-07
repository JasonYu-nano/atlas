using System.Diagnostics;
using System.Text;
using CppAst;

namespace AtlasBuilder.Generator;

public static class CodeGenUtils
{
    public static void AppendTabs(this StringBuilder sb, int numTabs)
    {
        for (int i = 0; i < numTabs; i++)
        {
            sb.Append('\t');
        }
    }
    
    public static string MakeUnderlineStylePath(string path)
    {
        StringBuilder sb = new();
        var underlineAppend = false;
        foreach (var ch in path)
        {
            if (char.IsLetterOrDigit(ch))
            {
                sb.Append(ch);
                underlineAppend = false;
            }
            else if (!underlineAppend)
            {
                underlineAppend = true;
                sb.Append('_');
            }
        }
        
        return sb.ToString();
    }
    
    public static string MakeFlagSequence(string enumName, List<string> flags)
    {
        if (flags.Count > 0)
        {
            StringBuilder sb = new();
            var first = true;
            foreach (var flag in flags)
            {
                if (first)
                {
                    sb.Append($"{enumName}::{flag}");
                    first = false;
                }
                else
                {
                    sb.Append($"|{enumName}::{flag}");
                }
            }

            return sb.ToString();
        }

        return $"{enumName}::None";
    }

    public static void AddMetaData<TE>(StringBuilder sb, int numTabs, MetaAttributeMap metaAttributeMap) where TE : Enum
    {
        AddMetaData<TE>(sb, numTabs, metaAttributeMap, []);
    }

    public static void AddMetaData<TE>(StringBuilder sb, int numTabs, MetaAttributeMap metaAttributeMap, List<string> additionalFlags) where TE : Enum
    {
        foreach (var attribute in metaAttributeMap.MetaList)
        {
            foreach (var pair in attribute.ArgumentMap)
            {
                if (pair.Value is bool)
                {
                    additionalFlags.Add(pair.Key);
                }
                else if (pair.Value is string)
                {
                    sb.AppendTabs(numTabs);
                    sb.AppendLine($""".set_meta("{pair.Key}", "{pair.Value}")""");
                }
                else if (pair.Value is int)
                {
                    sb.AppendTabs(numTabs);
                    sb.AppendLine($""".set_meta("{pair.Key}", {pair.Value})""");
                }
                else
                {
                    throw new UnreachableException($"Metadata value only supports string or int types: {pair.Key}.");
                }
            }
        }

        if (additionalFlags.Count > 0)
        {
            var type = typeof(TE);
            var enumNames = type.GetEnumNames();
            var invalidFlags = additionalFlags.TakeWhile(f => !enumNames.Contains(f));
            if (invalidFlags.Any())
            {
                throw new Exception($"META() declaration contains invalid flag: {string.Join(",",invalidFlags)}");
            }
            sb.AppendTabs(numTabs);
            sb.AppendLine($".set_flags({MakeFlagSequence($"E{type.Name}", additionalFlags)})");
        }
    }
}