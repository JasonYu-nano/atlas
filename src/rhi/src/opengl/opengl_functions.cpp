#include "opengl_functions.hpp"
#include "opengl_context.hpp"

namespace atlas
{

static void* resolve_symbol(const OpenGLContext& context, StringView fn_name)
{
    void* function = context.get_proc_address(fn_name);

    static StringView extensions[] = {
        { "ARB", 4 },
        { "OES", 4 },
        { "EXT", 4 },
        { "ANGLE", 6 },
        { "NV", 3 },
    };

    if (!function)
    {
        char fn[512];
        size_t size = fn_name.length();
        ASSERT(size < 500);
        std::memcpy(fn, fn_name.data(), size);
        char *ext = fn + size;

        for (const auto &e : extensions)
        {
            std::memcpy(ext, e.data(), e.length());
            function = context.get_proc_address(fn);
            if (function)
            {
                break;
            }
        }
    }

    return function;
}

#define GLFN_FUNCTION_NAMES(ret, name, param, arg) "gl"#name"\0"

OpenGLFunctions::OpenGLFunctions(const OpenGLContext& context)
{
    const char* names = GLFN_FUNCTIONS(GLFN_FUNCTION_NAMES);
    const char* name = names;

    for (int i = 0; i < GLFN_FUNCTIONS(GLFN_COUNT_FUNCTIONS); ++i)
    {
        functions[i] = resolve_symbol(context, name);
        name += strlen(name) + 1;
    }
}

void OpenGLFunctions::initialize()
{
    // auto context = OpenGLContext::current_context();

    const char* names = GLFN_FUNCTIONS(GLFN_FUNCTION_NAMES);
    const char* name = names;

    // for (int i = 0; i < GLFN_FUNCTIONS(GLFN_COUNT_FUNCTIONS); ++i)
    // {
    //     functions[i] = resolve_symbol(*context, name);
    //     name += strlen(name) + 1;
    // }
}

}// namespace atlas