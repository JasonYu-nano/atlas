#pragma once

#include "opengl_context.hpp"
#include "opengl_types.hpp"

namespace atlas
{

namespace details
{

class GLFunction;

}// namespace details

#define GLFN_DECLARE_FUNCTIONS(ret, name, args) ret (OPENGL_API name)args;
#define GLFN_COUNT_FUNCTIONS(ret, name, args) +1

#define GLFN_DECLARE(FUNCTIONS) \
public: \
struct Functions \
{ \
    FUNCTIONS(GLFN_DECLARE_FUNCTIONS) \
}; \
private: \
union \
{ \
    void* functions[FUNCTIONS(GLFN_COUNT_FUNCTIONS)]; \
    Functions f; \
};

#define GLFN_FUNCTIONS(F) \
    F(void, BindTexture, (GLenum target, GLuint texture)) \
    F(void, BlendFunc, (GLenum sfactor, GLenum dfactor)) \
    F(void, Clear, (GLbitfield mask)) \
    F(void, ClearColor, (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)) \
    F(void, ClearDepthf, (GLclampf depth)) \
    F(void, ClearStencil, (GLint s)) \
    F(void, ColorMask, (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)) \
    F(void, CopyTexImage2D, (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)) \
    F(void, CopyTexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)) \
    F(void, CullFace, (GLenum mode)) \
    F(void, DeleteTextures, (GLsizei n, const GLuint* textures)) \
    F(void, DepthFunc, (GLenum func)) \
    F(void, DepthMask, (GLboolean flag)) \
    F(void, DepthRangef, (GLclampf nearVal, GLclampf farVal)) \
    F(void, Disable, (GLenum cap)) \
    F(void, DrawArrays, (GLenum mode, GLint first, GLsizei count)) \
    F(void, DrawElements, (GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)) \
    F(void, Enable, (GLenum cap)) \
    F(void, Finish, ()) \
    F(void, Flush, ()) \
    F(void, FrontFace, (GLenum mode)) \
    F(void, GenTextures, (GLsizei n, GLuint* textures)) \
    F(void, GetBooleanv, (GLenum pname, GLboolean* params)) \
    F(GLenum, GetError, ()) \
    F(void, GetFloatv, (GLenum pname, GLfloat* params)) \
    F(void, GetIntegerv, (GLenum pname, GLint* params)) \
    F(const GLubyte *, GetString, (GLenum name)) \
    F(void, GetTexParameterfv, (GLenum target, GLenum pname, GLfloat* params)) \
    F(void, GetTexParameteriv, (GLenum target, GLenum pname, GLint* params)) \
    F(void, Hint, (GLenum target, GLenum mode)) \
    F(GLboolean, IsEnabled, (GLenum cap)) \
    F(GLboolean, IsTexture, (GLuint texture)) \
    F(void, LineWidth, (GLfloat width)) \
    F(void, PixelStorei, (GLenum pname, GLint param)) \
    F(void, PolygonOffset, (GLfloat factor, GLfloat units)) \
    F(void, ReadPixels, (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)) \
    F(void, Scissor, (GLint x, GLint y, GLsizei width, GLsizei height)) \
    F(void, StencilFunc, (GLenum func, GLint ref, GLuint mask)) \
    F(void, StencilMask, (GLuint mask)) \
    F(void, StencilOp, (GLenum fail, GLenum zfail, GLenum zpass)) \
    F(void, TexImage2D, (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels)) \
    F(void, TexParameterf, (GLenum target, GLenum pname, GLfloat param)) \
    F(void, TexParameterfv, (GLenum target, GLenum pname, const GLfloat* params)) \
    F(void, TexParameteri, (GLenum target, GLenum pname, GLint param)) \
    F(void, TexParameteriv, (GLenum target, GLenum pname, const GLint* params)) \
    F(void, TexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels)) \
    F(void, Viewport, (GLint x, GLint y, GLsizei width, GLsizei height)) \
    F(void, ActiveTexture, (GLenum texture)) \
    F(void, AttachShader, (GLuint program, GLuint shader)) \
    F(void, BindAttribLocation, (GLuint program, GLuint index, const char* name)) \
    F(void, BindBuffer, (GLenum target, GLuint buffer)) \
    F(void, BindFramebuffer, (GLenum target, GLuint framebuffer)) \
    F(void, BindRenderbuffer, (GLenum target, GLuint renderbuffer)) \
    F(void, BlendColor, (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)) \
    F(void, BlendEquation, (GLenum mode)) \
    F(void, BlendEquationSeparate, (GLenum modeRGB, GLenum modeAlpha)) \
    F(void, BlendFuncSeparate, (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)) \
    F(void, BufferData, (GLenum target, OPGLsizeiptr size, const void* data, GLenum usage)) \
    F(void, BufferSubData, (GLenum target, OPGLintptr offset, OPGLsizeiptr size, const void* data)) \
    F(GLenum, CheckFramebufferStatus, (GLenum target)) \
    F(void, CompileShader, (GLuint shader)) \
    F(void, CompressedTexImage2D, (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)) \
    F(void, CompressedTexSubImage2D, (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)) \
    F(GLuint, CreateProgram, ()) \
    F(GLuint, CreateShader, (GLenum type)) \
    F(void, DeleteBuffers, (GLsizei n, const GLuint* buffers)) \
    F(void, DeleteFramebuffers, (GLsizei n, const GLuint* framebuffers)) \
    F(void, DeleteProgram, (GLuint program)) \
    F(void, DeleteRenderbuffers, (GLsizei n, const GLuint* renderbuffers)) \
    F(void, DeleteShader, (GLuint shader)) \
    F(void, DetachShader, (GLuint program, GLuint shader)) \
    F(void, DisableVertexAttribArray, (GLuint index)) \
    F(void, EnableVertexAttribArray, (GLuint index)) \
    F(void, FramebufferRenderbuffer, (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)) \
    F(void, FramebufferTexture2D, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)) \
    F(void, GenBuffers, (GLsizei n, GLuint* buffers)) \
    F(void, GenerateMipmap, (GLenum target)) \
    F(void, GenFramebuffers, (GLsizei n, GLuint* framebuffers)) \
    F(void, GenRenderbuffers, (GLsizei n, GLuint* renderbuffers)) \
    F(void, GetActiveAttrib, (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)) \
    F(void, GetActiveUniform, (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)) \
    F(void, GetAttachedShaders, (GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)) \
    F(GLint, GetAttribLocation, (GLuint program, const char* name)) \
    F(void, GetBufferParameteriv, (GLenum target, GLenum pname, GLint* params)) \
    F(void, GetFramebufferAttachmentParameteriv, (GLenum target, GLenum attachment, GLenum pname, GLint* params)) \
    F(void, GetProgramiv, (GLuint program, GLenum pname, GLint* params)) \
    F(void, GetProgramInfoLog, (GLuint program, GLsizei bufsize, GLsizei* length, char* infolog)) \
    F(void, GetRenderbufferParameteriv, (GLenum target, GLenum pname, GLint* params)) \
    F(void, GetShaderiv, (GLuint shader, GLenum pname, GLint* params)) \
    F(void, GetShaderInfoLog, (GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog)) \
    F(void, GetShaderPrecisionFormat, (GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)) \
    F(void, GetShaderSource, (GLuint shader, GLsizei bufsize, GLsizei* length, char* source)) \
    F(void, GetUniformfv, (GLuint program, GLint location, GLfloat* params)) \
    F(void, GetUniformiv, (GLuint program, GLint location, GLint* params)) \
    F(GLint, GetUniformLocation, (GLuint program, const char* name)) \
    F(void, GetVertexAttribfv, (GLuint index, GLenum pname, GLfloat* params)) \
    F(void, GetVertexAttribiv, (GLuint index, GLenum pname, GLint* params)) \
    F(void, GetVertexAttribPointerv, (GLuint index, GLenum pname, void** pointer)) \
    F(GLboolean, IsBuffer, (GLuint buffer)) \
    F(GLboolean, IsFramebuffer, (GLuint framebuffer)) \
    F(GLboolean, IsProgram, (GLuint program)) \
    F(GLboolean, IsRenderbuffer, (GLuint renderbuffer)) \
    F(GLboolean, IsShader, (GLuint shader)) \
    F(void, LinkProgram, (GLuint program)) \
    F(void, ReleaseShaderCompiler, ()) \
    F(void, RenderbufferStorage, (GLenum target, GLenum internalformat, GLsizei width, GLsizei height)) \
    F(void, SampleCoverage, (GLclampf value, GLboolean invert)) \
    F(void, ShaderBinary, (GLint n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLint length)) \
    F(void, ShaderSource, (GLuint shader, GLsizei count, const char** string, const GLint* length)) \
    F(void, StencilFuncSeparate, (GLenum face, GLenum func, GLint ref, GLuint mask)) \
    F(void, StencilMaskSeparate, (GLenum face, GLuint mask)) \
    F(void, StencilOpSeparate, (GLenum face, GLenum fail, GLenum zfail, GLenum zpass)) \
    F(void, Uniform1f, (GLint location, GLfloat x)) \
    F(void, Uniform1fv, (GLint location, GLsizei count, const GLfloat* v)) \
    F(void, Uniform1i, (GLint location, GLint x)) \
    F(void, Uniform1iv, (GLint location, GLsizei count, const GLint* v)) \
    F(void, Uniform2f, (GLint location, GLfloat x, GLfloat y)) \
    F(void, Uniform2fv, (GLint location, GLsizei count, const GLfloat* v)) \
    F(void, Uniform2i, (GLint location, GLint x, GLint y)) \
    F(void, Uniform2iv, (GLint location, GLsizei count, const GLint* v)) \
    F(void, Uniform3f, (GLint location, GLfloat x, GLfloat y, GLfloat z)) \
    F(void, Uniform3fv, (GLint location, GLsizei count, const GLfloat* v)) \
    F(void, Uniform3i, (GLint location, GLint x, GLint y, GLint z)) \
    F(void, Uniform3iv, (GLint location, GLsizei count, const GLint* v)) \
    F(void, Uniform4f, (GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)) \
    F(void, Uniform4fv, (GLint location, GLsizei count, const GLfloat* v)) \
    F(void, Uniform4i, (GLint location, GLint x, GLint y, GLint z, GLint w)) \
    F(void, Uniform4iv, (GLint location, GLsizei count, const GLint* v)) \
    F(void, UniformMatrix2fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)) \
    F(void, UniformMatrix3fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)) \
    F(void, UniformMatrix4fv, (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)) \
    F(void, UseProgram, (GLuint program)) \
    F(void, ValidateProgram, (GLuint program)) \
    F(void, VertexAttrib1f, (GLuint indx, GLfloat x)) \
    F(void, VertexAttrib1fv, (GLuint indx, const GLfloat* values)) \
    F(void, VertexAttrib2f, (GLuint indx, GLfloat x, GLfloat y)) \
    F(void, VertexAttrib2fv, (GLuint indx, const GLfloat* values)) \
    F(void, VertexAttrib3f, (GLuint indx, GLfloat x, GLfloat y, GLfloat z)) \
    F(void, VertexAttrib3fv, (GLuint indx, const GLfloat* values)) \
    F(void, VertexAttrib4f, (GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w)) \
    F(void, VertexAttrib4fv, (GLuint indx, const GLfloat* values)) \
    F(void, VertexAttribPointer, (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr)) \
    F(void, ClearDepth, (GLdouble depth)) \
    F(void, DepthRange, (GLdouble zNear, GLdouble zFar)) \

class OpenGLFunctions
{
public:
    OpenGLFunctions(const OpenGLContext& context);
    void initialize();

private:
    GLFN_DECLARE(GLFN_FUNCTIONS);
};

}// namespace atlas
