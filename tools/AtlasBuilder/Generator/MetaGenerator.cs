using System.Diagnostics;
using System.Text;
using System.Text.RegularExpressions;
using AtlasBuilder.BuildTarget;
using CppAst;
using ToolCore.Utils;

namespace AtlasBuilder.Generator;

using MetaUnitStorage = Dictionary<string, CppTypeDeclaration>;

public class MetaTypeVerifyException(CppType type, CppDeclaration declaration) : Exception
{
    public CppType Type { get; } = type;
    public CppDeclaration Declaration { get; } = declaration;

    public override string ToString()
    {
        return $"Type '{Type.FullName}' in declaration '{Declaration}' is not a meta type";
    }
}

public class MetaGenerator(BuildTargetAssembly buildTargetAssembly)
{
    private readonly BuildTargetAssembly _buildTargetAssembly = buildTargetAssembly;
    
    private MetaUnitStorage _metaTypeStorage = new();

    public async Task Generate()
    {
        var cc = await Parse();
        if (cc == null)
        {
            return;
        }

        if (cc.HasErrors)
        {
            throw new Exception("MetaGenerator: Errors occurred while parse meta.");
        }

        CollectMetaTypes(cc);
        VerifyMetaType();
        await GenerateMetaCode();
    }

    private async Task<CppCompilation?> Parse()
    {
        List<Task<IEnumerable<string>>> tasks = new();
        foreach (var buildTarget in _buildTargetAssembly.NameToBuildTargets.Values)
        {
            tasks.Add(Task.Run(()=>GetPendingParseHeader(buildTarget)));
        }

        var result = await Task.WhenAll(tasks.ToArray());
        List <string> headers = new();
        foreach (var v in result)
        {
            headers.AddRange(v);
        }

        if (headers.Count > 0)
        {
            Console.WriteLine("Parse header:");
            foreach (var header in headers)
            {
                Console.WriteLine($"{header}");
            }
            var options = MakeCppParserOptions();
            var cc = CppParser.ParseFiles(headers, options);
            return cc;
        }
        
        return null;
    }

    private CppParserOptions MakeCppParserOptions()
    {
        var options = new CppParserOptions 
        {
            ParseSystemIncludes = false,
            ParseUserAnnotateOnly = true,
        };
            
        options.AdditionalArguments.AddRange([
            $"-include{Path.Combine(DirectoryUtils.BuildTargetIntermediateDirectory, "no_export_definitions.hpp")}", 
            "-std=c++23",
            "-DATLAS_BUILDER",
            "-Wno-microsoft-include"
        ]);
            
        foreach (var nameToBuildTarget in _buildTargetAssembly.NameToBuildTargets)
        {
            options.IncludeFolders.Add(Path.Combine(nameToBuildTarget.Value.RootDirectory, "include"));
            options.IncludeFolders.Add(Path.Combine(DirectoryUtils.BuildTargetIntermediateDirectory, nameToBuildTarget.Value.TargetName));
        }

        foreach (var nameToPackage in _buildTargetAssembly.NameToThirdPartyPackages)
        {
            options.IncludeFolders.AddRange(nameToPackage.Value.IncludeDirs);
            options.AdditionalArguments.AddRange(nameToPackage.Value.Definitions);
        }

        return options;
    }

    private IEnumerable<string> GetPendingParseHeader(BuildTargetBase buildTarget)
    {
        var headers = Directory.GetFiles(buildTarget.RootDirectory, "*.hpp", SearchOption.AllDirectories);
        return headers.Where(header =>
        {
            using var reader = new StreamReader(header);
            var file = reader.ReadToEnd();
            return Regex.Count(file, @"(?<!#define\s)META\([^)]*\)") > 0;
        });
    }

    private void CollectMetaTypes(CppCompilation compilation)
    {
        foreach (var cls in compilation.Classes)
        {
            if (HasMetaAnnotate(cls))
            {
                _metaTypeStorage.Add(cls.Name, cls);
            }
        }
        
        foreach (var e in compilation.Enums)
        {
            if (HasMetaAnnotate(e))
            {
                _metaTypeStorage.Add(e.Name, e);
            }
        }

        foreach (var ns in compilation.Namespaces)
        {
            CollectMetaTypes(ns);
        }
    }
    
    private void CollectMetaTypes(CppNamespace ns)
    {
        foreach (var cls in ns.Classes)
        {
            if (HasMetaAnnotate(cls))
            {
                _metaTypeStorage.Add(cls.FullName, cls);
            }
        }
        
        foreach (var e in ns.Enums)
        {
            if (HasMetaAnnotate(e))
            {
                _metaTypeStorage.Add(e.FullName, e);
            }
        }

        foreach (var cns in ns.Namespaces)
        {
            CollectMetaTypes(cns);
        }
    }
    
    /// <summary>
    /// Verify that all meta-types are legitimate.
    /// </summary>
    /// <exception cref="T:AtlasBuilder.Generator.MetaTypeVerifyException"><see cref="_metaTypeStorage"/> contains illegal meta-types. </exception>
    private void VerifyMetaType()
    {
        foreach (var mt in _metaTypeStorage.Values)
        {
            if (mt is CppClass cppClass)
            {
                cppClass.Verify(_metaTypeStorage);
            }
            else if (mt is CppEnum cppEnum)
            {
                cppEnum.Verify(_metaTypeStorage);
            }
        }
    }

    private async Task GenerateMetaCode()
    {
        Dictionary<string, List<CppTypeDeclaration>> fileToTypes = new();
        foreach (var mt in _metaTypeStorage.Values)
        {
            var sourceFile = mt.Span.Start.File;
            if (fileToTypes.TryGetValue(sourceFile, out var types))
            {
                types.Add(mt);
            }
            else
            {
                fileToTypes.Add(sourceFile, [mt]);
            }
        }

        foreach (var types in fileToTypes.Values)
        {
            types.Sort((x, y) => x.Span.Start.Line - y.Span.Start.Line);
        }
        
        List<Task> tasks = new();
        
        foreach (var pair in fileToTypes)
        {
            var fileName = Path.GetFileNameWithoutExtension(pair.Key);
            BuildTargetBase? ownerTarget = null;
            foreach (var target in _buildTargetAssembly.NameToBuildTargets.Values)
            {
                if (pair.Key.StartsWith(target.RootDirectory))
                {
                    ownerTarget = target;
                    break;
                }
            }

            if (ownerTarget == null)
            {
                throw new Exception($"Could not find owner target for {pair.Key}.");
            }
            
            {
                var content = GenerateHeaderCode(pair.Key, pair.Value);
                var outFile = Path.Combine(DirectoryUtils.BuildTargetIntermediateDirectory, ownerTarget.TargetName, $"{fileName}.gen.hpp");
                await using StreamWriter sourceStream = new StreamWriter(outFile, false, Encoding.UTF8);
                tasks.Add(sourceStream.WriteAsync(content.ToCharArray(), 0, content.Length));
            }
            
            {
                var content = GenerateSourceCode(pair.Key, pair.Value);
                var outFile = Path.Combine(DirectoryUtils.BuildTargetIntermediateDirectory, ownerTarget.TargetName, $"{fileName}.gen.cpp");
                await using StreamWriter sourceStream = new StreamWriter(outFile, false, Encoding.UTF8);
                tasks.Add(sourceStream.WriteAsync(content.ToCharArray(), 0, content.Length));
            }
        }
        
        Task.WaitAll(tasks.ToArray());
    }

    private string GenerateHeaderCode(string file, List<CppTypeDeclaration> types)
    {
        StringBuilder sb = new();
        sb.AppendLine("""
                      // THIS FILE IS GENERATED BY ATLAS BUILD TOOL.
                      // DON'T PLAY WITH IT IF YOU DON'T KNOW WHAT YOU ARE DOING!

                      #pragma once
                      
                      #include "meta/class.hpp"

                      """);

        foreach (var type in types)
        {
            if (type is CppClass cppClass)
            {
                cppClass.GenerateHeaderCode(sb);
                sb.AppendLine();
            }
            else if (type is CppEnum cppEnum)
            {
            }
        }
        
        return sb.ToString();
    }

    private string GenerateSourceCode(string file, List<CppTypeDeclaration> types)
    {
        StringBuilder sb = new();
        var fileWithoutExtension = Path.GetFileNameWithoutExtension(file);
        var normalizedFile = file.Replace('\\', '/');
        sb.AppendLine($"""
                      // THIS FILE IS GENERATED BY ATLAS BUILD TOOL.
                      // DON'T PLAY WITH IT IF YOU DON'T KNOW WHAT YOU ARE DOING!

                      #include "{normalizedFile}"
                      #include "{fileWithoutExtension}.gen.hpp"
                      #include "meta/registration.hpp"
                      
                      using namespace atlas;
                      """);
        
        foreach (var type in types)
        {
            if (type is CppClass cppClass)
            {
                cppClass.GenerateSourceCode(sb);
                sb.AppendLine();
            }
            else if (type is CppEnum cppEnum)
            {
                cppEnum.GenerateSourceCode();
                sb.AppendLine();
            }
        }
        
        return sb.ToString();
    }
    
    private bool HasMetaAnnotate(ICppAttributeContainer container)
    {
        foreach (var attribute in container.Attributes)
        {
            if (attribute.Kind == AttributeKind.AnnotateAttribute)
            {
                return true;
            }
        }

        return false;
    }
}

public static class CppClassExtension
{
    public static void GenerateHeaderCode(this CppClass cppClass, StringBuilder sb)
    {
        if (cppClass.Parent is CppNamespace ns)
        {
            var keywords = cppClass.ClassKind == CppClassKind.Struct ? "struct" : "class";
            sb.AppendLine($$"""
                            namespace {{ns.FullParentName}}::{{ns.Name}}{ {{keywords}} {{cppClass.Name}}; }
                            template<> atlas::MetaClass* meta_class_of<{{cppClass.FullName}}>();
                            """);
        }
    }
    
    public static void GenerateSourceCode(this CppClass cppClass, StringBuilder sb)
    {
        sb.AppendLine($$"""
                   static MetaClass* private_get_meta_class_{{cppClass.Name}}()
                   {
                       return Registration::ClassReg<{{cppClass.FullName}}>("{{cppClass.Name}}")
                   """);
        
        List<string> flags = new();
        foreach (var attribute in cppClass.MetaAttributes.MetaList)
        {
            foreach (var pair in attribute.ArgumentMap)
            {
                if (pair.Value is bool)
                {
                    flags.Add(pair.Key);
                }
                else if (pair.Value is string)
                {
                    sb.AppendLine($"""    .set_meta("{pair.Key}", "{pair.Value}")""");
                }
                else if (pair.Value is int)
                {
                    sb.AppendLine($"""    .set_meta("{pair.Key}", {pair.Value})""");
                }
                else
                {
                    throw new UnreachableException($"Metadata value only supports string or int types: {pair.Key}.");
                }
            }
        }

        if (flags.Count > 0)
        {
            sb.Append("\t.set_flags(");
            bool first = true;
            foreach (var flag in flags)
            {
                if (first)
                {
                    first = false;
                    sb.Append($"EMetaFlag::{flag}");
                }
                else
                {
                    sb.Append($"EMetaFlag::{flag}|");
                }
            }
            sb.AppendLine(")");
        }

        foreach (var field in cppClass.Fields)
        {
            sb.Append("\t");
            field.GenerateMetaCode(sb);
        }

        sb.AppendLine($$"""
                          .get();
                      }
                      
                      template<>
                      MetaClass* meta_class_of<{{cppClass.FullName}}>()
                      {
                          static MetaClass* m = private_get_meta_class_{{cppClass.Name}}();
                          return m;
                      }
                      """);
    }
    
    public static void Verify(this CppClass cppClass, MetaUnitStorage storage)
    {
        foreach (var field in cppClass.Fields)
        {
            field.Verify(storage);
        }
        
        foreach (var fn in cppClass.Functions)
        {
            fn.Verify(storage);
        }
    }
}

public static class CppEnumExtension
{
    public static string GenerateSourceCode(this CppEnum cppClass)
    {
        return "";
    }
    
    public static void Verify(this CppEnum cppEnum, MetaUnitStorage storage)
    {
    }
}

public static class CppFieldExtension
{
    public static void GenerateMetaCode(this CppField cppField, StringBuilder sb)
    {
        sb.AppendLine($".add_property(Registration::PropertyReg<{cppField.Type.GetPrettyName()}>(\"{cppField.Name}\", OFFSET_OF({((CppClass)cppField.Parent).FullName}, {cppField.Name})).get())");
    }
    
    public static void Verify(this CppField cppField, MetaUnitStorage storage)
    {
        if (!CppTypeUtils.IsValidType(cppField.Type, storage))
        {
            throw new MetaTypeVerifyException(cppField.Type, cppField);
        }
    }
}

public static class CppFunctionExtension
{
    public static string GenerateMetaCode(this CppFunction cppFunction)
    {
        return "";
    }
    
    public static void Verify(this CppFunction cppFunction, MetaUnitStorage storage)
    {
        if (!CppTypeUtils.IsValidType(cppFunction.ReturnType, storage))
        {
            throw new MetaTypeVerifyException(cppFunction.ReturnType, cppFunction);
        }

        foreach (var parameter in cppFunction.Parameters)
        {
            if (!CppTypeUtils.IsValidType(parameter.Type, storage))
            {
                throw new MetaTypeVerifyException(parameter.Type, cppFunction);
            }
        }
    }
}

static class CppTypeUtils
{
    public static HashSet<string> BasicTypes { get; } = [
        "int8",
        "int16",
        "int32",
        "int64",
        "uint8",
        "uint16",
        "uint32",
        "uint64",
        "size_t",
        "float",
        "double",
        "real_t",
        "void",
        "bool",
        "atlas::String",
        "atlas::StringName"
    ];
    
    /// <summary>
    /// 
    /// </summary>
    /// <param name="type"></param>
    /// <param name="storage"></param>
    /// <returns></returns>
    public static bool IsValidType(CppType type, MetaUnitStorage storage)
    {
        if (type is CppTypedef typedef)
        {
            if (BasicTypes.Contains(typedef.Name))
            {
                return true;
            }
            
            return storage.ContainsKey(typedef.ElementType.FullName);
        }
        return BasicTypes.Contains(type.FullName) || storage.ContainsKey(type.FullName);
    }

    public static string GetPrettyName(this CppType type)
    {
        if (type is CppTypedef typedef)
        {
            return typedef.Name;
        }

        if (type is CppClass cppClass)
        {
            return cppClass.Name;
        }
        
        return type.FullName;
    }
}