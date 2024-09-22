using System.Diagnostics;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using AtlasBuilder.BuildTarget;
using CppAst;
using Newtonsoft.Json;
using ToolCore.Utils;

namespace AtlasBuilder.Generator;

using MetaTypeStorage = Dictionary<string, CppTypeDeclaration>;

public class MetaTypeException(CppType type, CppDeclaration declaration) : Exception
{
    public CppType Type { get; } = type;
    public CppDeclaration Declaration { get; } = declaration;

    public override string ToString()
    {
        return $"Type '{Type.FullName}' in declaration '{Declaration}' is not a meta type";
    }
}

public class MetaDeclarationException(string message) : Exception(message);

[GeneratorVersion("0.0.4")]
public class MetaGenerator(BuildTargetAssembly buildTargetAssembly)
{
    private MetaTypeStorage _metaTypeStorage = new();

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
        foreach (var buildTarget in buildTargetAssembly.NameToBuildTargets.Values)
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
            
        foreach (var nameToBuildTarget in buildTargetAssembly.NameToBuildTargets)
        {
            options.IncludeFolders.Add(Path.Combine(nameToBuildTarget.Value.RootDirectory, "include"));
            options.IncludeFolders.Add(Path.Combine(DirectoryUtils.BuildTargetIntermediateDirectory, nameToBuildTarget.Value.TargetName));
        }

        foreach (var nameToPackage in buildTargetAssembly.NameToThirdPartyPackages)
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
        #if DEBUG
            var forceReparse = true;
            CodeGenRecord? record = null;
        #else
            var forceReparse = true;
            var record = GetLastGenRecord();
            if (record != null && record.GeneratorVersion == GetGeneratorVersion())
            {
                forceReparse = false;
            }
        #endif
        
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
    /// <exception cref="T:AtlasBuilder.Generator.MetaTypeException"><see cref="_metaTypeStorage"/> contains illegal meta-types. </exception>
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

    private Task GenerateMetaCode()
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
            foreach (var target in buildTargetAssembly.NameToBuildTargets.Values)
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
                tasks.Add(Task.Run(async () =>
                {
                    await using var sourceStream = new StreamWriter(outFile, false, Encoding.UTF8);
                    await sourceStream.WriteAsync(content.ToCharArray(), 0, content.Length);
                }));
            }
            
            {
                var content = GenerateSourceCode(pair.Key, pair.Value);
                var outFile = Path.Combine(DirectoryUtils.BuildTargetIntermediateDirectory, ownerTarget.TargetName, $"{fileName}.gen.cpp");
                tasks.Add(Task.Run(async () =>
                {
                    await using var sourceStream = new StreamWriter(outFile, false, Encoding.UTF8);
                    await sourceStream.WriteAsync(content.ToCharArray(), 0, content.Length);
                }));
            }
        }
        
        return Task.WhenAll(tasks.ToArray());
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
                cppEnum.GenerateHeaderCode(sb);
                sb.AppendLine();
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
                cppClass.GenerateSourceCode(sb, _metaTypeStorage);
                sb.AppendLine();
            }
            else if (type is CppEnum cppEnum)
            {
                cppEnum.GenerateSourceCode(sb);
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


public static class CppNamespaceExtension
{
    public static string FullName(this CppNamespace cppNamespace)
    {
        if (cppNamespace.FullParentName == "")
        {
            return cppNamespace.Name;
        }

        return $"{cppNamespace.FullParentName}::{cppNamespace.Name}";
    }
}

public static class CppClassExtension
{
    public static void GenerateHeaderCode(this CppClass cppClass, StringBuilder sb)
    {
        if (cppClass.Parent is CppNamespace ns)
        {
            var keywords = cppClass.ClassKind == CppClassKind.Struct ? "struct" : "class";
            var file = cppClass.Span.Start.File;
            
            sb.AppendLine($$"""
                             namespace {{ns.FullName()}}{ {{keywords}} {{cppClass.Name}}; }
                             template<> atlas::MetaClass* meta_class_of<{{cppClass.FullName}}>();
                             
                             struct PrivateCodeGen_{{cppClass.Name}}
                             {
                                 static atlas::MetaClass* get_meta_class();
                             };
                             
                             #define CLASS_BODY_{{CodeGenUtils.MakeUnderlineStylePath(file)}}_{{cppClass.Name}}() \
                             public: \
                             friend class PrivateCodeGen_{{cppClass.Name}}; \
                             NODISCARD virtual MetaClass* meta_class() const { return meta_class_of<{{cppClass.FullName}}>(); } \
                             private: \
                             """);

            if (cppClass.HasCustomizeFlag())
            {
                Type? type = Type.GetType($"AtlasBuilder.Generator.CustomizeGenerator.{cppClass.Name}CustomizeGenerator");
                Debug.Assert(type != null);
                type.InvokeMember("GenerateBodyMarco", BindingFlags.Public | BindingFlags.Static | BindingFlags.InvokeMethod, null, null, [sb]);
            }
            else
            {
                foreach (var fn in cppClass.Functions)
                {
                    fn.GenerateHeaderCode(sb);
                }

                if (cppClass.ClassKind == CppClassKind.Struct)
                {
                    sb.AppendLine("public: \\");
                }
            }
        }
    }
    
    public static void GenerateSourceCode(this CppClass cppClass, StringBuilder sb, MetaTypeStorage storage)
    {
        sb.AppendLine($$"""
                   MetaClass* PrivateCodeGen_{{cppClass.Name}}::get_meta_class()
                   {
                       return Registration::ClassReg<{{cppClass.FullName}}>("{{cppClass.Name}}")
                   """);

        List<string> flags = new();
        if (cppClass.IsAbstract)
        {
            flags.Add("Abstract");
        }
        if (cppClass.IsInterface())
        {
            flags.Add("Interface");
        }
        CodeGenUtils.AddMetaData<MetaClassFlag>(sb, 1, cppClass.MetaAttributes, flags);
        
        foreach (var baseType in cppClass.BaseTypes)
        {
            if (baseType.Type is CppClass baseClass)
            {
                if (!baseClass.IsInterface())
                {
                    sb.AppendTabs(1);
                    sb.AppendLine($".set_base(meta_class_of<{baseClass.FullName}>())");
                }
                else if (storage.ContainsKey(baseClass.FullName))
                {
                    sb.AppendTabs(1);
                    sb.AppendLine($".add_interface(meta_class_of<{baseClass.FullName}>())");
                }
            }
        }
        
        if (cppClass.HasCustomizeFlag())
        {
            Type? type = Type.GetType($"AtlasBuilder.Generator.CustomizeGenerator.{cppClass.Name}CustomizeGenerator");
            Debug.Assert(type != null);
            type.InvokeMember("GenerateRegistrationCode", BindingFlags.Public |BindingFlags.Static | BindingFlags.InvokeMethod, null, null, [sb, 1]);
        }
        else
        {
            foreach (var field in cppClass.Fields)
            {
                field.GenerateSourceCode(sb, 1);
            }
        
            foreach (var fn in cppClass.Functions)
            {
                fn.GenerateSourceCode(sb, 1);
            }
        }

        sb.AppendLine($$"""
                          .get();
                      }
                      
                      template<>
                      MetaClass* meta_class_of<{{cppClass.FullName}}>()
                      {
                          static MetaClass* m = PrivateCodeGen_{{cppClass.Name}}::get_meta_class();
                          return m;
                      }
                      
                      static Registration auto_register_{{cppClass.Name}}([]{
                          meta_class_of<{{cppClass.FullName}}>();
                      });
                      """);
    }
    
    public static void Verify(this CppClass cppClass, MetaTypeStorage storage)
    {
        if (cppClass.ClassKind == CppClassKind.Union)
        {
            throw new MetaDeclarationException("Cannot mark union as meta class");
        }
        
        if (cppClass.Parent is not CppNamespace or CppCompilation)
        {
            throw new MetaDeclarationException("Meta classes can only be declared in namespaces (including global namespaces).");
        }

        if (cppClass.HasCustomizeFlag())
        {
            Type? type = Type.GetType($"AtlasBuilder.Generator.CustomizeGenerator.{cppClass.Name}CustomizeGenerator");
            if (type?.GetInterface("ICustomizeClassGenerator") == null)
            {
                throw new MetaDeclarationException($"Can not find customize generator for class {cppClass.Name}.");
            }           
        }

        bool hasBaseClass = false;
        foreach (var baseType in cppClass.BaseTypes)
        {
            if (baseType.Type is not CppClass baseClass)
            {
                throw new MetaDeclarationException("The base class of meta class should also mark as meta class.");
            }

            // Inheritance from interfaces that are not marked as meta is allowed.
            if (!baseClass.IsInterface())
            {
                if (hasBaseClass)
                {
                    throw new MetaDeclarationException("Multiple inheritance is not allowed in meta class.");    
                }

                if (!storage.ContainsKey(baseClass.FullName))
                {
                    throw new MetaDeclarationException("The base class of meta class should also mark as meta class.");
                }

                hasBaseClass = true;
            }
        }
        
        foreach (var field in cppClass.Fields)
        {
            field.Verify(storage);
        }
        
        foreach (var fn in cppClass.Functions)
        {
            fn.Verify(storage);
        }
    }

    public static bool HasCustomizeFlag(this CppClass cppClass)
    {
        foreach (var attribute in cppClass.MetaAttributes.MetaList)
        {
            foreach (var pair in attribute.ArgumentMap)
            {
                if (pair is { Key: "Customize", Value: bool })
                {
                    return true;
                }
            }
        }

        return false;
    }

    public static bool IsInterface(this CppClass cppClass)
    {
        return cppClass.IsAbstract && cppClass.Fields.Count <= 0;
    }
}

public static class CppEnumExtension
{
    public static void GenerateHeaderCode(this CppEnum cppEnum, StringBuilder sb)
    {
        if (cppEnum.Parent is CppNamespace ns)
        {
            if (cppEnum.IsScoped)
            {
                sb.AppendLine($$"""
                                namespace {{ns.FullParentName}}::{{ns.Name}}{ enum class {{cppEnum.Name}} : {{cppEnum.IntegerType.GetPrettyName()}}; }
                                template<> atlas::MetaEnum* meta_enum_of<{{cppEnum.FullName}}>();
                                """);
            }
            else
            {
                sb.AppendLine($$"""
                                namespace {{ns.FullParentName}}::{{ns.Name}}{ enum {{cppEnum.Name}}; }
                                template<> atlas::MetaEnum* meta_enum_of<{{cppEnum.FullName}}>();
                                """);
            }
        }
    }

    public static void GenerateSourceCode(this CppEnum cppEnum, StringBuilder sb)
    {
        sb.AppendLine($$"""
                        static MetaEnum* private_get_meta_enum_{{cppEnum.Name}}()
                        {
                            return Registration::EnumReg<{{cppEnum.FullName}}>("{{cppEnum.Name}}")
                        """);
        
        CodeGenUtils.AddMetaData<MetaClassFlag>(sb, 1, cppEnum.MetaAttributes);

        foreach (var item in cppEnum.Items)
        {
            item.GenerateSourceCode(sb, 1);
        }
        
        sb.AppendLine($$"""
                            .get();
                        }

                        template<>
                        MetaEnum* meta_enum_of<{{cppEnum.FullName}}>()
                        {
                            static MetaEnum* e = private_get_meta_enum_{{cppEnum.Name}}();
                            return e;
                        }

                        static Registration auto_register_{{cppEnum.Name}}([]{
                            meta_enum_of<{{cppEnum.FullName}}>();
                        });
                        """);
    }
    
    public static void Verify(this CppEnum cppEnum, MetaTypeStorage storage)
    {
    }
}

public static class CppFieldExtension
{
    private static void GeneratePropertyReg(StringBuilder sb, CppType cppType)
    {
        if (cppType.IsArrayType())
        {
            CppType tempType = ((CppClass)cppType).TemplateSpecializedArguments[0].ArgAsType;
            sb.Append(@"Registration::ArrayPropertyReg("""", 0, ");
            GeneratePropertyReg(sb, tempType);
            sb.Append(").get()");
        }
        else
        {
            sb.Append($@"Registration::PropertyReg<{cppType.GetPrettyName()}>("""", 0).get()");
        }
    }
    
    public static void GenerateSourceCode(this CppField cppField, StringBuilder sb, int numTabs)
    {
        sb.AppendTabs(numTabs);
        if (cppField.Type.IsArrayType())
        {
            CppType tempType = ((CppClass)cppField.Type).TemplateSpecializedArguments[0].ArgAsType;
            sb.Append($".add_property(Registration::ArrayPropertyReg(\"{cppField.Name}\", OFFSET_OF({((CppClass)cppField.Parent).FullName}, {cppField.Name}), ");
            GeneratePropertyReg(sb, cppField.Type);
            sb.AppendLine(")");
        }
        else
        {
            sb.AppendLine($".add_property(Registration::PropertyReg<{cppField.Type.GetPrettyName()}>(\"{cppField.Name}\", OFFSET_OF({((CppClass)cppField.Parent).FullName}, {cppField.Name}))");
        }
        
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
    
    public static void Verify(this CppField cppField, MetaTypeStorage storage)
    {
        if (cppField.StorageQualifier != CppStorageQualifier.None)
        {
            throw new MetaDeclarationException("Cannot mark static field as property");
        }

        if (cppField.Parent is not CppClass)
        {
            throw new MetaDeclarationException("Property can only be declared in class or struct");
        }

        if (!CppTypeUtils.IsValidType(cppField.Type, storage))
        {
            throw new MetaTypeException(cppField.Type, cppField);
        }
    }
}

public static class CppFunctionExtension
{
    public static void GenerateHeaderCode(this CppFunction cppFunction, StringBuilder sb)
    {
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
                           *static_cast<{cppFunction.ReturnType.RemoveCVRef().GetPrettyName()}*>(result) = 
                           """);
            }
            else
            {
                sb.Append($"""
                           *static_cast<{cppFunction.ReturnType.RemoveCVRef().GetPrettyName()}*>(result) = 
                           """);
            }
        }
        if (cppFunction.IsStatic)
        {
            sb.Append($"""
                       {cppFunction.Name}(
                       """);
        }
        else
        {
            sb.Append($"""
                       static_cast<{((CppClass)cppFunction.Parent).FullName}*>(instance)->{cppFunction.Name}(
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
            sb.AppendLine($""".add_ret_type(Registration::PropertyReg<{cppFunction.ReturnType.GetUnderlyingType().GetPrettyName()}>("", 0).get())""");
        }
        
        sb.AppendTabs(childNumTabs);
        sb.AppendLine(".get())");
    }
    
    public static void Verify(this CppFunction cppFunction, MetaTypeStorage storage)
    {
        if (!CppTypeUtils.IsValidType(cppFunction.ReturnType, storage))
        {
            throw new MetaTypeException(cppFunction.ReturnType, cppFunction);
        }

        foreach (var parameter in cppFunction.Parameters)
        {
            if (!CppTypeUtils.IsValidType(parameter.Type, storage))
            {
                throw new MetaTypeException(parameter.Type, cppFunction);
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

public static class CppEnumItemExtension
{
    public static void GenerateSourceCode(this CppEnumItem cppEnumItem, StringBuilder sb, int numTabs)
    {
        sb.AppendTabs(numTabs);
        sb.AppendLine($".add_field(Registration::EnumFieldReg(\"{cppEnumItem.Name}\", {cppEnumItem.Value})");
        
        CodeGenUtils.AddMetaData<EnumFieldFlag>(sb, numTabs + 1, cppEnumItem.MetaAttributes);
        sb.AppendTabs(numTabs + 1);
        sb.AppendLine(".get())");
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
    public static bool IsValidType(CppType type, MetaTypeStorage storage)
    {
        var underlyingType = type.GetUnderlyingType();
        if (underlyingType is CppTypedef typedef)
        {
            if (BasicTypes.Contains(typedef.Name))
            {
                return true;
            }

            underlyingType = typedef.ElementType;
        }

        if (BasicTypes.Contains(underlyingType.FullName))
        {
            return true;
        }

        if (storage.ContainsKey(underlyingType.FullName))
        {
            return true;
        }
        
        if (underlyingType.IsContainerType(out var templateTypes))
        {
            foreach (var tempType in templateTypes!)
            {
                if (!IsValidType(tempType, storage))
                {
                    return false;
                }
            }

            return true;
        }

        return false;
    }
    
    internal static bool IsContainerType(this CppType type, out List<CppType>? templateTypes)
    {
        if (type.IsArrayType())
        {
            templateTypes = [((CppClass)type).TemplateSpecializedArguments[0].ArgAsType];
            return true;
        }

        templateTypes = null;
        return false;
    }

    internal static bool IsArrayType(this CppType type)
    {
        return type is CppClass { Name: "Array" };
    }

    internal static string GetPrettyName(this CppType type)
    {
        if (type is CppTypedef typedef)
        {
            return typedef.Name;
        }

        if (type is CppClass cppClass)
        {
            return cppClass.FullName;
        }
        
        return type.FullName;
    }

    /// <summary>
    /// Removes const,volatile,reference from type.
    /// </summary>
    /// <param name="type"></param>
    /// <returns></returns>
    internal static CppType GetUnderlyingType(this CppType type)
    {
        while (true)
        {
            switch (type.TypeKind)
            {
                case CppTypeKind.Pointer:
                case CppTypeKind.Qualified:
                case CppTypeKind.Reference:
                {
                    type = ((CppTypeWithElementType)type).ElementType;
                    continue;
                }

                default:
                    return type;
            }
        }
    }
    
    /// <summary>
    /// Removes const,volatile,reference from type.
    /// </summary>
    /// <param name="type"></param>
    /// <returns></returns>
    public static CppType RemoveCVRef(this CppType type)
    {
        while (true)
        {
            switch (type.TypeKind)
            {
                case CppTypeKind.Qualified:
                case CppTypeKind.Reference:
                {
                    type = ((CppTypeWithElementType)type).ElementType;
                    continue;
                }

                default:
                    return type;
            }
        }
    }
}