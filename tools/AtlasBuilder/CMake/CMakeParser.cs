// Copyright(c) 2023-present, Atlas.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

namespace AtlasBuilder.CMake;

public static class CMakeParser
{
    public static void ParserCMakeArgs(in string statement, out HashSet<string> options, out Dictionary<string, string> argToValue, out Dictionary<string, List<string>> argToMultiValues)
    {
        options = new HashSet<string>();
        argToValue = new Dictionary<string, string>();
        argToMultiValues = new Dictionary<string, List<string>>();
        List<ArgumentFragment> argumentFragments = new List<ArgumentFragment>();

        foreach (var opt in CMakeTypes.Options)
        {
            int idx = statement.IndexOf(opt, StringComparison.Ordinal);
            if (idx != -1)
            {
                options.Add(opt);
            }
        }
        
        foreach (var arg in CMakeTypes.OneValueArgs)
        {
            int idx = statement.IndexOf(arg, StringComparison.Ordinal);
            if (idx != -1)
            {
                argumentFragments.Add(new ArgumentFragment(arg, idx, false));
            }
        }
        
        foreach (var arg in CMakeTypes.MultiValueArgs)
        {
            int idx = statement.IndexOf(arg, StringComparison.Ordinal);
            if (idx != -1)
            {
                argumentFragments.Add(new ArgumentFragment(arg, idx, true));
            }
        }
        
        argumentFragments.Sort();

        int lastStartIndex = statement.Length;
        int fragmentIndex = argumentFragments.Count - 1;
        while (fragmentIndex >= 0)
        {
            var fragment = argumentFragments[fragmentIndex];
            --fragmentIndex;
            
            var subStringStart = fragment.StartIndex + fragment.Keywords.Length;
            var argLine = statement.Substring(subStringStart, lastStartIndex - 1 - subStringStart);
            lastStartIndex = fragment.StartIndex;
            
            NormalizeCMakeFunctionArgument(ref argLine);
            var args = argLine.Split(' ', StringSplitOptions.RemoveEmptyEntries);
            
            if (!fragment.MultiValue && args.Length == 1)
            {
                argToValue.Add(fragment.Keywords, args[0]);
            }
            else if (fragment.MultiValue && args.Length > 0) 
            {
                argToMultiValues.Add(fragment.Keywords, args.ToList());
            }
        }
    }

    private static void NormalizeCMakeFunctionArgument(ref string argument)
    {
        argument = argument.Trim();
        if (argument.EndsWith('\n'))
        {
            argument = argument.Remove(argument.Length - 1);
        }
    }
    
    private struct ArgumentFragment : IComparable<ArgumentFragment>
    {
        public string Keywords;
        public int StartIndex;
        public bool MultiValue;

        public ArgumentFragment(string keywords, int startIndex, bool multiValue)
        {
            Keywords = keywords;
            StartIndex = startIndex;
            MultiValue = multiValue;
        }
        
        public static bool operator < (ArgumentFragment lhs, ArgumentFragment rhs)
        {
            return lhs.StartIndex < rhs.StartIndex;
        }

        public static bool operator >(ArgumentFragment lhs, ArgumentFragment rhs)
        {
            return lhs.StartIndex > rhs.StartIndex;
        }

        public int CompareTo(ArgumentFragment other)
        {
            return StartIndex.CompareTo(other.StartIndex);
        }
    }
}