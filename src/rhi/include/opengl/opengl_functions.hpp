#pragma once

#include <type_traits>

#include "opengl_types.hpp"

namespace atlas
{

#define DECLARE_GLFN_TYPE(ret, name, param, arg) ret (OPENGL_API name)param;
#define GET_GLFN_COUNT(ret, name, param, arg) +1

#define DECLARE_GLFN_FIELD(FUNCTIONS) \
public: \
struct Functions \
{ \
FUNCTIONS(DECLARE_GLFN_TYPE) \
}; \
private: \
union \
{ \
void* functions[FUNCTIONS(GET_GLFN_COUNT)]; \
Functions f; \
};

#pragma region foreach_gl_functions

#define GLES_3_0 1

#if GLES_3_0
#define GLES_3_0_FUNCTIONS(...) __VA_ARGS__
#else
#define GLES_3_0_FUNCTIONS(...)
#endif

#define FOREACH_GL_FUNCTIONS(F) \
F(void, BindTexture, (GLenum target, GLuint texture), (target, texture)) \
F(void, BlendFunc, (GLenum sfactor, GLenum dfactor), (sfactor, dfactor)) \
F(void, Clear, (GLbitfield mask), (mask)) \
F(void, ClearColor, (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha), (red, green, blue, alpha)) \
F(void, ClearDepthf, (GLclampf depth), (depth)) \
F(void, ClearStencil, (GLint s), (s)) \
F(void, ColorMask, (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha), (red, green, blue, alpha)) \
F(void, CopyTexImage2D, (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border), (target, level, internalformat, x, y, width, height, border)) \
F(void, CopyTexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height), (target, level, xoffset, yoffset, x, y, width, height)) \
F(void, CullFace, (GLenum mode), (mode)) \
F(void, DeleteTextures, (GLsizei n, const GLuint* textures), (n, textures)) \
F(void, DepthFunc, (GLenum func), (func)) \
F(void, DepthMask, (GLboolean flag), (flag)) \
F(void, DepthRangef, (GLclampf nearVal, GLclampf farVal), (nearVal, farVal)) \
F(void, Disable, (GLenum cap), (cap)) \
F(void, DrawArrays, (GLenum mode, GLint first, GLsizei count), (mode, first, count)) \
F(void, DrawElements, (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices), (mode, count, type, indices)) \
F(void, Enable, (GLenum cap), (cap)) \
F(void, Finish, (), ()) \
F(void, Flush, (), ()) \
F(void, FrontFace, (GLenum mode), (mode)) \
F(void, GenTextures, (GLsizei n, GLuint* textures), (n, textures)) \
F(void, GetBooleanv, (GLenum pname, GLboolean* params), (pname, params)) \
F(GLenum, GetError, (), ()) \
F(void, GetFloatv, (GLenum pname, GLfloat* params), (pname, params)) \
F(void, GetIntegerv, (GLenum pname, GLint* params), (pname, params)) \
F(const GLubyte *, GetString, (GLenum name), (name)) \
F(void, GetTexParameterfv, (GLenum target, GLenum pname, GLfloat* params), (target, pname, params)) \
F(void, GetTexParameteriv, (GLenum target, GLenum pname, GLint* params), (target, pname, params)) \
F(void, Hint, (GLenum target, GLenum mode), (target, mode)) \
F(GLboolean, IsEnabled, (GLenum cap), (cap)) \
F(GLboolean, IsTexture, (GLuint texture), (texture)) \
F(void, LineWidth, (GLfloat width), (width)) \
F(void, PixelStorei, (GLenum pname, GLint param), (pname, param)) \
F(void, PolygonOffset, (GLfloat factor, GLfloat units), (factor, units)) \
F(void, ReadPixels, (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels), (x, y, width, height, format, type, pixels)) \
F(void, Scissor, (GLint x, GLint y, GLsizei width, GLsizei height), (x, y, width, height)) \
F(void, StencilFunc, (GLenum func, GLint ref, GLuint mask), (func, ref, mask)) \
F(void, StencilMask, (GLuint mask), (mask)) \
F(void, StencilOp, (GLenum fail, GLenum zfail, GLenum zpass), (fail, zfail, zpass)) \
F(void, TexImage2D, (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels), (target, level, internalformat, width, height, border, format, type, pixels)) \
F(void, TexParameterf, (GLenum target, GLenum pname, GLfloat param), (target, pname, param)) \
F(void, TexParameterfv, (GLenum target, GLenum pname, const GLfloat* params), (target, pname, params)) \
F(void, TexParameteri, (GLenum target, GLenum pname, GLint param), (target, pname, param)) \
F(void, TexParameteriv, (GLenum target, GLenum pname, const GLint* params), (target, pname, params)) \
F(void, TexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels), (target, level, xoffset, yoffset, width, height, format, type, pixels)) \
F(void, Viewport, (GLint x, GLint y, GLsizei width, GLsizei height), (x, y, width, height)) \
F(void, ActiveTexture, (GLenum texture), (texture)) \
F(void, AttachShader, (GLuint program, GLuint shader), (program, shader)) \
F(void, BindAttribLocation, (GLuint program, GLuint index, const char* name), (program, index, name)) \
F(void, BindBuffer, (GLenum target, GLuint buffer), (target, buffer)) \
F(void, BindFramebuffer, (GLenum target, GLuint framebuffer), (target, framebuffer)) \
F(void, BindRenderbuffer, (GLenum target, GLuint renderbuffer), (target, renderbuffer)) \
F(void, BlendColor, (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha), (red, green, blue, alpha)) \
F(void, BlendEquation, (GLenum mode), (mode)) \
F(void, BlendEquationSeparate, (GLenum modeRGB, GLenum modeAlpha), (modeRGB, modeAlpha)) \
F(void, BlendFuncSeparate, (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha), (srcRGB, dstRGB, srcAlpha, dstAlpha)) \
F(void, BufferData, (GLenum target, OPGLsizeiptr size, const void* data, GLenum usage), (target, size, data, usage)) \
F(void, BufferSubData, (GLenum target, OPGLintptr offset, OPGLsizeiptr size, const void* data), (target, offset, size, data)) \
F(GLenum, CheckFramebufferStatus, (GLenum target), (target)) \
F(void, CompileShader, (GLuint shader), (shader)) \
F(void, CompressedTexImage2D, (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data), (target, level, internalformat, width, height, border, imageSize, data)) \
F(void, CompressedTexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data), (target, level, xoffset, yoffset, width, height, format, imageSize, data)) \
F(GLuint, CreateProgram, (), ()) \
F(GLuint, CreateShader, (GLenum type), (type)) \
F(void, DeleteBuffers, (GLsizei n, const GLuint* buffers), (n, buffers)) \
F(void, DeleteFramebuffers, (GLsizei n, const GLuint* framebuffers), (n, framebuffers)) \
F(void, DeleteProgram, (GLuint program), (program)) \
F(void, DeleteRenderbuffers, (GLsizei n, const GLuint* renderbuffers), (n, renderbuffers)) \
F(void, DeleteShader, (GLuint shader), (shader)) \
F(void, DetachShader, (GLuint program, GLuint shader), (program, shader)) \
F(void, DisableVertexAttribArray, (GLuint index), (index)) \
F(void, EnableVertexAttribArray, (GLuint index), (index)) \
F(void, FramebufferRenderbuffer, (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer), (target, attachment, renderbuffertarget, renderbuffer)) \
F(void, FramebufferTexture2D, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level), (target, attachment, textarget, texture, level)) \
F(void, GenBuffers, (GLsizei n, GLuint* buffers), (n, buffers)) \
F(void, GenerateMipmap, (GLenum target), (target)) \
F(void, GenFramebuffers, (GLsizei n, GLuint* framebuffers), (n, framebuffers)) \
F(void, GenRenderbuffers, (GLsizei n, GLuint* renderbuffers), (n, renderbuffers)) \
F(void, GetActiveAttrib, (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name), (program, index, bufsize, length, size, type, name)) \
F(void, GetActiveUniform, (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name), (program, index, bufsize, length, size, type, name)) \
F(void, GetAttachedShaders, (GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders), (program, maxcount, count, shaders)) \
F(GLint, GetAttribLocation, (GLuint program, const char* name), (program, name)) \
F(void, GetBufferParameteriv, (GLenum target, GLenum pname, GLint* params), (target, pname, params)) \
F(void, GetFramebufferAttachmentParameteriv, (GLenum target, GLenum attachment, GLenum pname, GLint* params), (target, attachment, pname, params)) \
F(void, GetProgramiv, (GLuint program, GLenum pname, GLint* params), (program, pname, params)) \
F(void, GetProgramInfoLog, (GLuint program, GLsizei bufsize, GLsizei* length, char* infolog), (program, bufsize, length, infolog)) \
F(void, GetRenderbufferParameteriv, (GLenum target, GLenum pname, GLint* params), (target, pname, params)) \
F(void, GetShaderiv, (GLuint shader, GLenum pname, GLint* params), (shader, pname, params)) \
F(void, GetShaderInfoLog, (GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog), (shader, bufsize, length, infolog)) \
F(void, GetShaderPrecisionFormat, (GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision), (shadertype, precisiontype, range, precision)) \
F(void, GetShaderSource, (GLuint shader, GLsizei bufsize, GLsizei* length, char* source), (shader, bufsize, length, source)) \
F(void, GetUniformfv, (GLuint program, GLint location, GLfloat* params), (program, location, params)) \
F(void, GetUniformiv, (GLuint program, GLint location, GLint* params), (program, location, params)) \
F(GLint, GetUniformLocation, (GLuint program, const char* name), (program, name)) \
F(void, GetVertexAttribfv, (GLuint index, GLenum pname, GLfloat* params), (index, pname, params)) \
F(void, GetVertexAttribiv, (GLuint index, GLenum pname, GLint* params), (index, pname, params)) \
F(void, GetVertexAttribPointerv, (GLuint index, GLenum pname, void** pointer), (index, pname, pointer)) \
F(GLboolean, IsBuffer, (GLuint buffer), (buffer)) \
F(GLboolean, IsFramebuffer, (GLuint framebuffer), (framebuffer)) \
F(GLboolean, IsProgram, (GLuint program), (program)) \
F(GLboolean, IsRenderbuffer, (GLuint renderbuffer), (renderbuffer)) \
F(GLboolean, IsShader, (GLuint shader), (shader)) \
F(void, LinkProgram, (GLuint program), (program)) \
F(void, ReleaseShaderCompiler, (), ()) \
F(void, RenderbufferStorage, (GLenum target, GLenum internalformat, GLsizei width, GLsizei height), (target, internalformat, width, height)) \
F(void, SampleCoverage, (GLclampf value, GLboolean invert), (value, invert)) \
F(void, ShaderBinary, (GLint n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLint length), (n, shaders, binaryformat, binary, length)) \
F(void, ShaderSource, (GLuint shader, GLsizei count, const char** string, const GLint* length), (shader, count, string, length)) \
F(void, StencilFuncSeparate, (GLenum face, GLenum func, GLint ref, GLuint mask), (face, func, ref, mask)) \
F(void, StencilMaskSeparate, (GLenum face, GLuint mask), (face, mask)) \
F(void, StencilOpSeparate, (GLenum face, GLenum fail, GLenum zfail, GLenum zpass), (face, fail, zfail, zpass)) \
F(void, Uniform1f, (GLint location, GLfloat x), (location, x)) \
F(void, Uniform1fv, (GLint location, GLsizei count, const GLfloat* v), (location, count, v)) \
F(void, Uniform1i, (GLint location, GLint x), (location, x)) \
F(void, Uniform1iv, (GLint location, GLsizei count, const GLint* v), (location, count, v)) \
F(void, Uniform2f, (GLint location, GLfloat x, GLfloat y), (location, x, y)) \
F(void, Uniform2fv, (GLint location, GLsizei count, const GLfloat* v), (location, count, v)) \
F(void, Uniform2i, (GLint location, GLint x, GLint y), (location, x, y)) \
F(void, Uniform2iv, (GLint location, GLsizei count, const GLint* v), (location, count, v)) \
F(void, Uniform3f, (GLint location, GLfloat x, GLfloat y, GLfloat z), (location, x, y, z)) \
F(void, Uniform3fv, (GLint location, GLsizei count, const GLfloat* v), (location, count, v)) \
F(void, Uniform3i, (GLint location, GLint x, GLint y, GLint z), (location, x, y, z)) \
F(void, Uniform3iv, (GLint location, GLsizei count, const GLint* v), (location, count, v)) \
F(void, Uniform4f, (GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w), (location, x, y, z, w)) \
F(void, Uniform4fv, (GLint location, GLsizei count, const GLfloat* v), (location, count, v)) \
F(void, Uniform4i, (GLint location, GLint x, GLint y, GLint z, GLint w), (location, x, y, z, w)) \
F(void, Uniform4iv, (GLint location, GLsizei count, const GLint* v), (location, count, v)) \
F(void, UniformMatrix2fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value), (location, count, transpose, value)) \
F(void, UniformMatrix3fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value), (location, count, transpose, value)) \
F(void, UniformMatrix4fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value), (location, count, transpose, value)) \
F(void, UseProgram, (GLuint program), (program)) \
F(void, ValidateProgram, (GLuint program), (program)) \
F(void, VertexAttrib1f, (GLuint indx, GLfloat x), (indx, x)) \
F(void, VertexAttrib1fv, (GLuint indx, const GLfloat* values), (indx, values)) \
F(void, VertexAttrib2f, (GLuint indx, GLfloat x, GLfloat y), (indx, x, y)) \
F(void, VertexAttrib2fv, (GLuint indx, const GLfloat* values), (indx, values)) \
F(void, VertexAttrib3f, (GLuint indx, GLfloat x, GLfloat y, GLfloat z), (indx, x, y, z)) \
F(void, VertexAttrib3fv, (GLuint indx, const GLfloat* values), (indx, values)) \
F(void, VertexAttrib4f, (GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w), (indx, x, y, z, w)) \
F(void, VertexAttrib4fv, (GLuint indx, const GLfloat* values), (indx, values)) \
F(void, VertexAttribPointer, (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr), (indx, size, type, normalized, stride, ptr)) \
F(void, ClearDepth, (GLdouble depth), (depth)) \
F(void, DepthRange, (GLdouble zNear, GLdouble zFar), (zNear, zFar)) \
GLES_3_0_FUNCTIONS( \
F(void, BindVertexArray, (GLuint array), (array)) \
F(void, DeleteVertexArrays, (GLsizei n, const GLuint *arrays), (n, arrays)) \
F(void, GenVertexArrays, (GLsizei n, GLuint *arrays), (n, arrays)) \
F(void, PolygonMode, (GLenum face, GLenum mode), (face, mode)) \
) \

#pragma endregion foreach_gl_functions

class OpenGLContext;

#define DECLARE_OPENGL_FUNCTION(ret, name, param, arg) \
ret gl##name param const;

class OpenGLFunctions
{
public:
    explicit OpenGLFunctions(const OpenGLContext& context);
    void initialize();

    FOREACH_GL_FUNCTIONS(DECLARE_OPENGL_FUNCTION)
private:
    DECLARE_GLFN_FIELD(FOREACH_GL_FUNCTIONS);
};

#if OPENGL_ES2 && PLATFORM_ANDROID
#define DEFINE_OPENGL_FUNCTION(ret, name, param, arg) \
inline ret OpenGLFunctions::gl##name param const \
{ \
    if constexpr (std::is_void_v<ret>) \
        ::gl##name arg; \
    else \
        return ::gl##name arg; \
}
#else
#define DEFINE_OPENGL_FUNCTION(ret, name, param, arg) \
inline ret OpenGLFunctions::gl##name param const \
{ \
    if constexpr (std::is_void_v<ret>) \
        f.name arg; \
    else \
        return f.name arg; \
}
#endif

FOREACH_GL_FUNCTIONS(DEFINE_OPENGL_FUNCTION)

#undef DECLARE_GLFN_TYPE
#undef GET_GLFN_COUNT
#undef DECLARE_GLFN_FIELD
#undef DECLARE_OPENGL_FUNCTION
#undef DEFINE_OPENGL_FUNCTION

}// namespace atlas
