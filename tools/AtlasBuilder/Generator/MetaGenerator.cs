using System.Diagnostics;
using System.IO.MemoryMappedFiles;
using System.Text;
using System.Text.RegularExpressions;
using AtlasBuilder.BuildTarget;
using CppAst;
using Newtonsoft.Json;
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

[GeneratorVersion("0.0.1")]
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
            foreach (var msg in cc.Diagnostics.Messages)
            {
                Console.WriteLine(msg);
            }
            throw new Exception("MetaGenerator: Errors occurred while parse meta.");
        }
        
        var endTime = DateTime.Now;
        var timeSpan = endTime - AtlasBuilder.StartTime;
        Console.WriteLine($"Meta parse finished in {timeSpan.TotalSeconds} seconds");

        CollectMetaTypes(cc);
        VerifyMetaType();
        await GenerateMetaCode();
        RecordGeneration();
    }

    private async Task<CppCompilation?> Parse()
    {
        List<Task<IEnumerable<string>>> tasks = new();
        foreach (var buildTarget in _buildTargetAssembly.NameToBuildTargets.Values)
        {
            async Task<IEnumerable<string>> Fn()
            {
                var headers = GetPendingParseHeader(buildTarget);
                await CreateTemporaryHeader(buildTarget, headers);
                return headers;
            }

            tasks.Add(Fn());
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

    public static Version GetGeneratorVersion()
    {
        foreach (var attribute in Attribute.GetCustomAttributes(typeof(MetaGenerator)))
        {
            if (attribute is GeneratorVersionAttribute versionAttribute)
            {
                return versionAttribute.Version;
            }
        }
        return Version.Parse("0.0.0");
    }

    private CodeGenRecord? GetLastGenRecord()
    {
        var recordFile = Path.Combine(DirectoryUtils.BuildTargetIntermediateDirectory, "record.json");
        return Path.Exists(recordFile) ? JsonConvert.DeserializeObject<CodeGenRecord>(File.ReadAllText(recordFile)) : null;
    }

    private void RecordGeneration()
    {
        var record = new CodeGenRecord(GetGeneratorVersion(), DateTime.Now);
        string json = JsonConvert.SerializeObject(record, Formatting.Indented);
        var recordFile = Path.Combine(DirectoryUtils.BuildTargetIntermediateDirectory, "record.json");
        File.WriteAllText(recordFile, json);
    }

    private IEnumerable<string> GetPendingParseHeader(BuildTargetBase buildTarget)
    {
        // If there is an update to the current version of the generator, force all files to be regenerated！
        var forceReparse = true;
        var record = GetLastGenRecord();
        if (record != null && record.GeneratorVersion == GetGeneratorVersion())
        {
            forceReparse = false;
        }
        
        var headers = Directory.GetFiles(buildTarget.RootDirectory, "*.hpp", SearchOption.AllDirectories);
        return headers.Where(header =>
        {
            // If the content of the file has not been modified, just skip the parse state.
            if (!forceReparse && record != null)
            {
                if (File.GetLastWriteTime(header) <= record.LastGenTime)
                {
                    return false;
                }
            }
            
            using var reader = new StreamReader(header);
            var file = reader.ReadToEnd();
            return Regex.Count(file, @"(?<!#define\s)META\([^)]*\)") > 0;
        });
    }

    private async Task CreateTemporaryHeader(BuildTargetBase buildTarget, IEnumerable<string> headers)
    {
        foreach (var header in headers)
        {
            var fileName = Path.GetFileNameWithoutExtension(header);
            var genFile = Path.Combine(DirectoryUtils.BuildTargetIntermediateDirectory, buildTarget.TargetName, $"{fileName}.gen.hpp");
            if (!Path.Exists(genFile))
            {
                string content = """
                              // THIS FILE IS GENERATED BY ATLAS BUILD TOOL.
                              // DON'T PLAY WITH IT IF YOU DON'T KNOW WHAT YOU ARE DOING!

                              #pragma once

                              #include "meta/class.hpp"

                              """;
                await using StreamWriter sourceStream = new StreamWriter(genFile, false, Encoding.UTF8);
                await sourceStream.WriteAsync(content.ToCharArray(), 0, content.Length);
            }
        }
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
        sb.AppendLine($"""
                      // THIS FILE IS GENERATED BY ATLAS BUILD TOOL.
                      // DON'T PLAY WITH IT IF YOU DON'T KNOW WHAT YOU ARE DOING!

                      #pragma once
                      
                      #include "meta/class.hpp"
                      
                      #ifdef FILE_PATH
                      #undef FILE_PATH
                      #endif
                      #define FILE_PATH {CodeGenUtils.MakeUnderlineStylePath(file)}

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

internal record CodeGenRecord(Version GeneratorVersion, DateTime LastGenTime);

public static class CppClassExtension
{
    public static void GenerateHeaderCode(this CppClass cppClass, StringBuilder sb)
    {
        if (cppClass.Parent is CppNamespace ns)
        {
            var keywords = cppClass.ClassKind == CppClassKind.Struct ? "struct" : "class";
            var file = cppClass.Span.Start.File;
            
            sb.AppendLine($$"""
                             namespace {{ns.FullParentName}}::{{ns.Name}}{ {{keywords}} {{cppClass.Name}}; }
                             template<> atlas::MetaClass* meta_class_of<{{cppClass.FullName}}>();
                             
                             #define CLASS_BODY_{{CodeGenUtils.MakeUnderlineStylePath(file)}}_{{cppClass.Name}}() \
                             public: \
                             NODISCARD MetaClass* meta_class() const { return meta_class_of<{{cppClass.FullName}}>(); } \
                             """);

            foreach (var fn in cppClass.Functions)
            {
                fn.GenerateHeaderCode(sb);
            }
        }
    }
    
    public static void GenerateSourceCode(this CppClass cppClass, StringBuilder sb)
    {
        sb.AppendLine($$"""
                   static MetaClass* private_get_meta_class_{{cppClass.Name}}()
                   {
                       return Registration::ClassReg<{{cppClass.FullName}}>("{{cppClass.Name}}")
                   """);

        CodeGenUtils.AddMetaData<MetaClassFlag>(sb, 1, cppClass.MetaAttributes);

        foreach (var field in cppClass.Fields)
        {
            field.GenerateSourceCode(sb, 1);
        }
        
        foreach (var fn in cppClass.Functions)
        {
            fn.GenerateSourceCode(sb, 1);
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
                      
                      static Registration auto_register_{{cppClass.Name}}([]{
                          meta_class_of<{{cppClass.FullName}}>();
                      });
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
    public static void GenerateSourceCode(this CppField cppField, StringBuilder sb, int numTabs)
    {
        sb.AppendTabs(numTabs);
        sb.AppendLine($".add_property(Registration::PropertyReg<{cppField.Type.GetPrettyName()}>(\"{cppField.Name}\", OFFSET_OF({((CppClass)cppField.Parent).FullName}, {cppField.Name}))");
        
        List<string> flags = new();
        switch (cppField.Visibility)
        {
            case CppVisibility.Private:
                flags.Add("Private");
                break;
            case CppVisibility.Public:
                flags.Add("Public");
                break;
            case CppVisibility.Protected:
                flags.Add("Protected");
                break;
            default:
                throw new UnreachableException();
        }
        
        CodeGenUtils.AddMetaData<PropertyFlag>(sb, numTabs + 1, cppField.MetaAttributes, flags);
        sb.AppendTabs(numTabs + 1);
        sb.AppendLine(".get())");
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
    public static void GenerateHeaderCode(this CppFunction cppFunction, StringBuilder sb)
    {
        //TODO: Move implement to cpp
        sb.AppendLine($$"""
                       static void {{cppFunction.Name}}_meta_gen_proxy(void* instance, ParamPack& packed_params, void* result) { \
                       """);

        for (int i = 0; i < cppFunction.Parameters.Count; i++)
        {
            var parameter = cppFunction.Parameters[i];
            sb.AppendLine($"""
                           {parameter.Type.GetPrettyName()}& {parameter.Name} = *reinterpret_cast<{parameter.Type.GetPrettyName()}*>(packed_params[{i}]); \
                           """);
        }

        if (cppFunction.ReturnType.FullName != "void")
        {
            if (cppFunction.IsStatic)
            {
                sb.Append($"""
                           *static_cast<{cppFunction.ReturnType.GetPrettyName()}*>(result) = {cppFunction.Name}(
                           """);
            }
            else
            {
                sb.Append($"""
                           *static_cast<{cppFunction.ReturnType.GetPrettyName()}*>(result) = static_cast<{((CppClass)cppFunction.Parent).FullName}*>(instance)->{cppFunction.Name}(
                           """);
            }

            var first = true;
            foreach (var parameter in cppFunction.Parameters)
            {
                if (first)
                {
                    sb.Append($"{parameter.Name}");
                    first = false;
                }
                else
                {
                    sb.Append($", {parameter.Name}");
                }
            }
        }

        sb.AppendLine(@"); } \");
    }
    public static void GenerateSourceCode(this CppFunction cppFunction, StringBuilder sb, int numTabs)
    {
        var childNumTabs = numTabs + 1;
        sb.AppendTabs(numTabs);
        sb.AppendLine($".add_method(Registration::MethodReg(&{cppFunction.GetProxyFunctionFullName()}, \"{cppFunction.Name}\")");
        
        List<string> flags = new();
        if (cppFunction.IsStatic)
        {
            flags.Add("Static");
        }
        if (cppFunction.IsConst)
        {
            flags.Add("Const");
        }

        switch (cppFunction.Visibility)
        {
            case CppVisibility.Private:
                flags.Add("Private");
                break;
            case CppVisibility.Public:
                flags.Add("Public");
                break;
            case CppVisibility.Protected:
                flags.Add("Protected");
                break;
            default:
                throw new UnreachableException();
        }
        CodeGenUtils.AddMetaData<MethodFlag>(sb, childNumTabs, cppFunction.MetaAttributes, flags);

        foreach (var parameter in cppFunction.Parameters)
        {
            parameter.GenerateSourceCode(sb, childNumTabs);
        }

        if (cppFunction.ReturnType.FullName != "void")
        {
            sb.AppendTabs(childNumTabs);
            sb.AppendLine($""".add_ret_type(Registration::PropertyReg<{cppFunction.ReturnType.GetPrettyName()}>("", 0).get())""");
        }
        
        sb.AppendTabs(childNumTabs);
        sb.AppendLine(".get())");
    }
    
    public static void GenerateProxyMethodImpl(this CppFunction cppFunction, StringBuilder sb)
    {
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

    private static string GetProxyFunctionName(this CppFunction cppFunction)
    {
        return $"{cppFunction.Name}_meta_gen_proxy";
    }
    
    private static string GetProxyFunctionFullName(this CppFunction cppFunction)
    {
        return $"{((CppClass)cppFunction.Parent).FullName}::{cppFunction.Name}_meta_gen_proxy";
    }
}

public static class CppParameterExtension
{
    public static void GenerateSourceCode(this CppParameter cppParameter, StringBuilder sb, int numTabs)
    {
        sb.AppendTabs(numTabs);
        sb.AppendLine($".add_parameter(Registration::PropertyReg<{cppParameter.Type.GetPrettyName()}>(\"{cppParameter.Name}\", 0).get())");
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