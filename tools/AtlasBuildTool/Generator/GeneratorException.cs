// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

namespace AtlasBuildTool.Generator;

public class GeneratorException : Exception
{
    public GeneratorException(string message) : base(message)
    {
    }
}