#pragma once

#include <type_traits>

#include "opengl_types.hpp"

namespace atlas
{
namespace details
{

class GLFunction;

}// namespace details

#define GLFN_DECLARE_FUNCTIONS(ret, name, param, arg) ret (OPENGL_API name)param;
#define GLFN_COUNT_FUNCTIONS(ret, name, param, arg) +1

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

#define AAA() \
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

class OpenGLContext;

#define DECLARE_OPENGL_FUNCTION(ret, name, param, arg) \
ret gl##name param const;

class OpenGLFunctions
{
public:
    OpenGLFunctions(const OpenGLContext& context);
    void initialize();

    GLFN_FUNCTIONS(DECLARE_OPENGL_FUNCTION)

    // // GLES2 + OpenGL1 common subset
    // void glBindTexture(GLenum target, GLuint texture);
    // void glBlendFunc(GLenum sfactor, GLenum dfactor);
    // void glClear(GLbitfield mask);
    // void glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    // void glClearStencil(GLint s);
    // void glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
    // void glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
    // void glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
    // void glCullFace(GLenum mode);
    // void glDeleteTextures(GLsizei n, const GLuint* textures);
    // void glDepthFunc(GLenum func);
    // void glDepthMask(GLboolean flag);
    // void glDisable(GLenum cap);
    // void glDrawArrays(GLenum mode, GLint first, GLsizei count);
    // void glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
    // void glEnable(GLenum cap);
    // void glFinish();
    // void glFlush();
    // void glFrontFace(GLenum mode);
    // void glGenTextures(GLsizei n, GLuint* textures);
    // void glGetBooleanv(GLenum pname, GLboolean* params);
    // GLenum glGetError();
    // void glGetFloatv(GLenum pname, GLfloat* params);
    // void glGetIntegerv(GLenum pname, GLint* params);
    // const GLubyte *glGetString(GLenum name);
    // void glGetTexParameterfv(GLenum target, GLenum pname, GLfloat* params);
    // void glGetTexParameteriv(GLenum target, GLenum pname, GLint* params);
    // void glHint(GLenum target, GLenum mode);
    // GLboolean glIsEnabled(GLenum cap);
    // GLboolean glIsTexture(GLuint texture);
    // void glLineWidth(GLfloat width);
    // void glPixelStorei(GLenum pname, GLint param);
    // void glPolygonOffset(GLfloat factor, GLfloat units);
    // void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels);
    // void glScissor(GLint x, GLint y, GLsizei width, GLsizei height);
    // void glStencilFunc(GLenum func, GLint ref, GLuint mask);
    // void glStencilMask(GLuint mask);
    // void glStencilOp(GLenum fail, GLenum zfail, GLenum zpass);
    // void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
    // void glTexParameterf(GLenum target, GLenum pname, GLfloat param);
    // void glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params);
    // void glTexParameteri(GLenum target, GLenum pname, GLint param);
    // void glTexParameteriv(GLenum target, GLenum pname, const GLint* params);
    // void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels);
    // void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
    //
    // // GL(ES)2
    // void glActiveTexture(GLenum texture);
    // void glAttachShader(GLuint program, GLuint shader);
    // void glBindAttribLocation(GLuint program, GLuint index, const char* name);
    // void glBindBuffer(GLenum target, GLuint buffer);
    // void glBindFramebuffer(GLenum target, GLuint framebuffer);
    // void glBindRenderbuffer(GLenum target, GLuint renderbuffer);
    // void glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    // void glBlendEquation(GLenum mode);
    // void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
    // void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
    // void glBufferData(GLenum target, OPGLsizeiptr size, const void* data, GLenum usage);
    // void glBufferSubData(GLenum target, OPGLintptr offset, OPGLsizeiptr size, const void* data);
    // GLenum glCheckFramebufferStatus(GLenum target);
    // void glClearDepthf(GLclampf depth);
    // void glCompileShader(GLuint shader);
    // void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
    // void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
    // GLuint glCreateProgram();
    // GLuint glCreateShader(GLenum type);
    // void glDeleteBuffers(GLsizei n, const GLuint* buffers);
    // void glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers);
    // void glDeleteProgram(GLuint program);
    // void glDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers);
    // void glDeleteShader(GLuint shader);
    // void glDepthRangef(GLclampf zNear, GLclampf zFar);
    // void glDetachShader(GLuint program, GLuint shader);
    // void glDisableVertexAttribArray(GLuint index);
    // void glEnableVertexAttribArray(GLuint index);
    // void glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
    // void glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
    // void glGenBuffers(GLsizei n, GLuint* buffers);
    // void glGenerateMipmap(GLenum target);
    // void glGenFramebuffers(GLsizei n, GLuint* framebuffers);
    // void glGenRenderbuffers(GLsizei n, GLuint* renderbuffers);
    // void glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);
    // void glGetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);
    // void glGetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);
    // GLint glGetAttribLocation(GLuint program, const char* name);
    // void glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params);
    // void glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params);
    // void glGetProgramiv(GLuint program, GLenum pname, GLint* params);
    // void glGetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog);
    // void glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params);
    // void glGetShaderiv(GLuint shader, GLenum pname, GLint* params);
    // void glGetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog);
    // void glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
    // void glGetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, char* source);
    // void glGetUniformfv(GLuint program, GLint location, GLfloat* params);
    // void glGetUniformiv(GLuint program, GLint location, GLint* params);
    // GLint glGetUniformLocation(GLuint program, const char* name);
    // void glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params);
    // void glGetVertexAttribiv(GLuint index, GLenum pname, GLint* params);
    // void glGetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer);
    // GLboolean glIsBuffer(GLuint buffer);
    // GLboolean glIsFramebuffer(GLuint framebuffer);
    // GLboolean glIsProgram(GLuint program);
    // GLboolean glIsRenderbuffer(GLuint renderbuffer);
    // GLboolean glIsShader(GLuint shader);
    // void glLinkProgram(GLuint program);
    // void glReleaseShaderCompiler();
    // void glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
    // void glSampleCoverage(GLclampf value, GLboolean invert);
    // void glShaderBinary(GLint n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLint length);
    // void glShaderSource(GLuint shader, GLsizei count, const char** string, const GLint* length);
    // void glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask);
    // void glStencilMaskSeparate(GLenum face, GLuint mask);
    // void glStencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
    // void glUniform1f(GLint location, GLfloat x);
    // void glUniform1fv(GLint location, GLsizei count, const GLfloat* v);
    // void glUniform1i(GLint location, GLint x);
    // void glUniform1iv(GLint location, GLsizei count, const GLint* v);
    // void glUniform2f(GLint location, GLfloat x, GLfloat y);
    // void glUniform2fv(GLint location, GLsizei count, const GLfloat* v);
    // void glUniform2i(GLint location, GLint x, GLint y);
    // void glUniform2iv(GLint location, GLsizei count, const GLint* v);
    // void glUniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z);
    // void glUniform3fv(GLint location, GLsizei count, const GLfloat* v);
    // void glUniform3i(GLint location, GLint x, GLint y, GLint z);
    // void glUniform3iv(GLint location, GLsizei count, const GLint* v);
    // void glUniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    // void glUniform4fv(GLint location, GLsizei count, const GLfloat* v);
    // void glUniform4i(GLint location, GLint x, GLint y, GLint z, GLint w);
    // void glUniform4iv(GLint location, GLsizei count, const GLint* v);
    // void glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    // void glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    // void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
    // void glUseProgram(GLuint program);
    // void glValidateProgram(GLuint program);
    // void glVertexAttrib1f(GLuint indx, GLfloat x);
    // void glVertexAttrib1fv(GLuint indx, const GLfloat* values);
    // void glVertexAttrib2f(GLuint indx, GLfloat x, GLfloat y);
    // void glVertexAttrib2fv(GLuint indx, const GLfloat* values);
    // void glVertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z);
    // void glVertexAttrib3fv(GLuint indx, const GLfloat* values);
    // void glVertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    // void glVertexAttrib4fv(GLuint indx, const GLfloat* values);
    // void glVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr);
private:
    GLFN_DECLARE(GLFN_FUNCTIONS);
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

GLFN_FUNCTIONS(DEFINE_OPENGL_FUNCTION)

// inline void OpenGLFunctions::glBindTexture(GLenum target, GLuint texture)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glBindTexture(target, texture);
// #else
//     f.BindTexture(target, texture);
// #endif
// }
//
// inline void OpenGLFunctions::glBlendFunc(GLenum sfactor, GLenum dfactor)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glBlendFunc(sfactor, dfactor);
// #else
//     f.BlendFunc(sfactor, dfactor);
// #endif
// }
//
// inline void OpenGLFunctions::glClear(GLbitfield mask)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glClear(mask);
// #else
//     f.Clear(mask);
// #endif
// }
//
// inline void OpenGLFunctions::glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glClearColor(red, green, blue, alpha);
// #else
//     f.ClearColor(red, green, blue, alpha);
// #endif
// }
//
// inline void OpenGLFunctions::glClearStencil(GLint s)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glClearStencil(s);
// #else
//     f.ClearStencil(s);
// #endif
// }
//
// inline void OpenGLFunctions::glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glColorMask(red, green, blue, alpha);
// #else
//     f.ColorMask(red, green, blue, alpha);
// #endif
// }
//
// inline void OpenGLFunctions::glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glCopyTexImage2D(target, level, internalformat, x, y, width,height, border);
// #else
//     f.CopyTexImage2D(target, level, internalformat, x, y, width,height, border);
// #endif
// }
//
// inline void OpenGLFunctions::glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
// #else
//     f.CopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
// #endif
// }
//
// inline void OpenGLFunctions::glCullFace(GLenum mode)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glCullFace(mode);
// #else
//     f.CullFace(mode);
// #endif
// }
//
// inline void OpenGLFunctions::glDeleteTextures(GLsizei n, const GLuint* textures)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glDeleteTextures(n, textures);
// #else
//     f.DeleteTextures(n, textures);
// #endif
// }
//
// inline void OpenGLFunctions::glDepthFunc(GLenum func)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glDepthFunc(func);
// #else
//     f.DepthFunc(func);
// #endif
// }
//
// inline void OpenGLFunctions::glDepthMask(GLboolean flag)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glDepthMask(flag);
// #else
//     f.DepthMask(flag);
// #endif
// }
//
// inline void OpenGLFunctions::glDisable(GLenum cap)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glDisable(cap);
// #else
//     f.Disable(cap);
// #endif
// }
//
// inline void OpenGLFunctions::glDrawArrays(GLenum mode, GLint first, GLsizei count)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glDrawArrays(mode, first, count);
// #else
//     f.DrawArrays(mode, first, count);
// #endif
// }
//
// inline void OpenGLFunctions::glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glDrawElements(mode, count, type, indices);
// #else
//     f.DrawElements(mode, count, type, indices);
// #endif
// }
//
// inline void OpenGLFunctions::glEnable(GLenum cap)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glEnable(cap);
// #else
//     f.Enable(cap);
// #endif
// }
//
// inline void OpenGLFunctions::glFinish()
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glFinish();
// #else
//     f.Finish();
// #endif
// }
//
// inline void OpenGLFunctions::glFlush()
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glFlush();
// #else
//     f.Flush();
// #endif
// }
//
// inline void OpenGLFunctions::glFrontFace(GLenum mode)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glFrontFace(mode);
// #else
//     f.FrontFace(mode);
// #endif
// }
//
// inline void OpenGLFunctions::glGenTextures(GLsizei n, GLuint* textures)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGenTextures(n, textures);
// #else
//     f.GenTextures(n, textures);
// #endif
// }
//
// inline void OpenGLFunctions::glGetBooleanv(GLenum pname, GLboolean* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetBooleanv(pname, params);
// #else
//     f.GetBooleanv(pname, params);
// #endif
// }
//
// inline GLenum OpenGLFunctions::glGetError()
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     GLenum result = ::glGetError();
// #else
//     GLenum result = f.GetError();
// #endif
//     return result;
// }
//
// inline void OpenGLFunctions::glGetFloatv(GLenum pname, GLfloat* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetFloatv(pname, params);
// #else
//     f.GetFloatv(pname, params);
// #endif
// }
//
// inline void OpenGLFunctions::glGetIntegerv(GLenum pname, GLint* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetIntegerv(pname, params);
// #else
//     f.GetIntegerv(pname, params);
// #endif
// }
//
// inline const GLubyte *OpenGLFunctions::glGetString(GLenum name)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     const GLubyte *result = ::glGetString(name);
// #else
//     const GLubyte *result = f.GetString(name);
// #endif
//     return result;
// }
//
// inline void OpenGLFunctions::glGetTexParameterfv(GLenum target, GLenum pname, GLfloat* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetTexParameterfv(target, pname, params);
// #else
//     f.GetTexParameterfv(target, pname, params);
// #endif
// }
//
// inline void OpenGLFunctions::glGetTexParameteriv(GLenum target, GLenum pname, GLint* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetTexParameteriv(target, pname, params);
// #else
//     f.GetTexParameteriv(target, pname, params);
// #endif
// }
//
// inline void OpenGLFunctions::glHint(GLenum target, GLenum mode)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glHint(target, mode);
// #else
//     f.Hint(target, mode);
// #endif
// }
//
// inline GLboolean OpenGLFunctions::glIsEnabled(GLenum cap)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     GLboolean result = ::glIsEnabled(cap);
// #else
//     GLboolean result = f.IsEnabled(cap);
// #endif
//     return result;
// }
//
// inline GLboolean OpenGLFunctions::glIsTexture(GLuint texture)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     GLboolean result = ::glIsTexture(texture);
// #else
//     GLboolean result = f.IsTexture(texture);
// #endif
//     return result;
// }
//
// inline void OpenGLFunctions::glLineWidth(GLfloat width)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glLineWidth(width);
// #else
//     f.LineWidth(width);
// #endif
// }
//
// inline void OpenGLFunctions::glPixelStorei(GLenum pname, GLint param)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glPixelStorei(pname, param);
// #else
//     f.PixelStorei(pname, param);
// #endif
// }
//
// inline void OpenGLFunctions::glPolygonOffset(GLfloat factor, GLfloat units)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glPolygonOffset(factor, units);
// #else
//     f.PolygonOffset(factor, units);
// #endif
// }
//
// inline void OpenGLFunctions::glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glReadPixels(x, y, width, height, format, type, pixels);
// #else
//     f.ReadPixels(x, y, width, height, format, type, pixels);
// #endif
// }
//
// inline void OpenGLFunctions::glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glScissor(x, y, width, height);
// #else
//     f.Scissor(x, y, width, height);
// #endif
// }
//
// inline void OpenGLFunctions::glStencilFunc(GLenum func, GLint ref, GLuint mask)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glStencilFunc(func, ref, mask);
// #else
//     f.StencilFunc(func, ref, mask);
// #endif
// }
//
// inline void OpenGLFunctions::glStencilMask(GLuint mask)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glStencilMask(mask);
// #else
//     f.StencilMask(mask);
// #endif
// }
//
// inline void OpenGLFunctions::glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glStencilOp(fail, zfail, zpass);
// #else
//     f.StencilOp(fail, zfail, zpass);
// #endif
// }
//
// inline void OpenGLFunctions::glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glTexImage2D(target, level, internalformat, width,height, border, format, type, pixels);
// #else
//     f.TexImage2D(target, level, internalformat, width,height, border, format, type, pixels);
// #endif
// }
//
// inline void OpenGLFunctions::glTexParameterf(GLenum target, GLenum pname, GLfloat param)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glTexParameterf(target, pname, param);
// #else
//     f.TexParameterf(target, pname, param);
// #endif
// }
//
// inline void OpenGLFunctions::glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glTexParameterfv(target, pname, params);
// #else
//     f.TexParameterfv(target, pname, params);
// #endif
// }
//
// inline void OpenGLFunctions::glTexParameteri(GLenum target, GLenum pname, GLint param)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glTexParameteri(target, pname, param);
// #else
//     f.TexParameteri(target, pname, param);
// #endif
// }
//
// inline void OpenGLFunctions::glTexParameteriv(GLenum target, GLenum pname, const GLint* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glTexParameteriv(target, pname, params);
// #else
//     f.TexParameteriv(target, pname, params);
// #endif
// }
//
// inline void OpenGLFunctions::glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
// #else
//     f.TexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
// #endif
// }
//
// inline void OpenGLFunctions::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glViewport(x, y, width, height);
// #else
//     f.Viewport(x, y, width, height);
// #endif
// }
//
// // GL(ES)2
//
// inline void OpenGLFunctions::glActiveTexture(GLenum texture)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glActiveTexture(texture);
// #else
//     f.ActiveTexture(texture);
// #endif
// }
//
// inline void OpenGLFunctions::glAttachShader(GLuint program, GLuint shader)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glAttachShader(program, shader);
// #else
//     f.AttachShader(program, shader);
// #endif
// }
//
// inline void OpenGLFunctions::glBindAttribLocation(GLuint program, GLuint index, const char* name)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glBindAttribLocation(program, index, name);
// #else
//     f.BindAttribLocation(program, index, name);
// #endif
// }
//
// inline void OpenGLFunctions::glBindBuffer(GLenum target, GLuint buffer)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glBindBuffer(target, buffer);
// #else
//     f.BindBuffer(target, buffer);
// #endif
// }
//
// inline void OpenGLFunctions::glBindFramebuffer(GLenum target, GLuint framebuffer)
// {
//     if (framebuffer == 0)
//         //framebuffer = QOpenGLContext::currentContext()->defaultFramebufferObject(); //TODO:
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glBindFramebuffer(target, framebuffer);
// #else
//     f.BindFramebuffer(target, framebuffer);
// #endif
// }
//
// inline void OpenGLFunctions::glBindRenderbuffer(GLenum target, GLuint renderbuffer)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glBindRenderbuffer(target, renderbuffer);
// #else
//     f.BindRenderbuffer(target, renderbuffer);
// #endif
// }
//
// inline void OpenGLFunctions::glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glBlendColor(red, green, blue, alpha);
// #else
//     f.BlendColor(red, green, blue, alpha);
// #endif
// }
//
// inline void OpenGLFunctions::glBlendEquation(GLenum mode)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glBlendEquation(mode);
// #else
//     f.BlendEquation(mode);
// #endif
// }
//
// inline void OpenGLFunctions::glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glBlendEquationSeparate(modeRGB, modeAlpha);
// #else
//     f.BlendEquationSeparate(modeRGB, modeAlpha);
// #endif
// }
//
// inline void OpenGLFunctions::glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glBlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
// #else
//     f.BlendFuncSeparate(srcRGB, dstRGB, srcAlpha, dstAlpha);
// #endif
// }
//
// inline void OpenGLFunctions::glBufferData(GLenum target, OPGLsizeiptr size, const void* data, GLenum usage)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glBufferData(target, size, data, usage);
// #else
//     f.BufferData(target, size, data, usage);
// #endif
// }
//
// inline void OpenGLFunctions::glBufferSubData(GLenum target, OPGLintptr offset, OPGLsizeiptr size, const void* data)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glBufferSubData(target, offset, size, data);
// #else
//     f.BufferSubData(target, offset, size, data);
// #endif
// }
//
// inline GLenum OpenGLFunctions::glCheckFramebufferStatus(GLenum target)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     GLenum result = ::glCheckFramebufferStatus(target);
// #else
//     GLenum result = f.CheckFramebufferStatus(target);
// #endif
//     return result;
// }
//
// inline void OpenGLFunctions::glClearDepthf(GLclampf depth)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glClearDepthf(depth);
// #else
//     f.ClearDepthf(depth);
// #endif
// }
//
// inline void OpenGLFunctions::glCompileShader(GLuint shader)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glCompileShader(shader);
// #else
//     f.CompileShader(shader);
// #endif
// }
//
// inline void OpenGLFunctions::glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
// #else
//     f.CompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
// #endif
// }
//
// inline void OpenGLFunctions::glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
// #else
//     f.CompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
// #endif
// }
//
// inline GLuint OpenGLFunctions::glCreateProgram()
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     GLuint result = ::glCreateProgram();
// #else
//     GLuint result = f.CreateProgram();
// #endif
//     return result;
// }
//
// inline GLuint OpenGLFunctions::glCreateShader(GLenum type)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     GLuint result = ::glCreateShader(type);
// #else
//     GLuint result = f.CreateShader(type);
// #endif
//     return result;
// }
//
// inline void OpenGLFunctions::glDeleteBuffers(GLsizei n, const GLuint* buffers)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glDeleteBuffers(n, buffers);
// #else
//     f.DeleteBuffers(n, buffers);
// #endif
// }
//
// inline void OpenGLFunctions::glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glDeleteFramebuffers(n, framebuffers);
// #else
//     f.DeleteFramebuffers(n, framebuffers);
// #endif
// }
//
// inline void OpenGLFunctions::glDeleteProgram(GLuint program)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glDeleteProgram(program);
// #else
//     f.DeleteProgram(program);
// #endif
// }
//
// inline void OpenGLFunctions::glDeleteRenderbuffers(GLsizei n, const GLuint* renderbuffers)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glDeleteRenderbuffers(n, renderbuffers);
// #else
//     f.DeleteRenderbuffers(n, renderbuffers);
// #endif
// }
//
// inline void OpenGLFunctions::glDeleteShader(GLuint shader)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glDeleteShader(shader);
// #else
//     f.DeleteShader(shader);
// #endif
// }
//
// inline void OpenGLFunctions::glDepthRangef(GLclampf zNear, GLclampf zFar)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glDepthRangef(zNear, zFar);
// #else
//     f.DepthRangef(zNear, zFar);
// #endif
// }
//
// inline void OpenGLFunctions::glDetachShader(GLuint program, GLuint shader)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glDetachShader(program, shader);
// #else
//     f.DetachShader(program, shader);
// #endif
// }
//
// inline void OpenGLFunctions::glDisableVertexAttribArray(GLuint index)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glDisableVertexAttribArray(index);
// #else
//     f.DisableVertexAttribArray(index);
// #endif
// }
//
// inline void OpenGLFunctions::glEnableVertexAttribArray(GLuint index)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glEnableVertexAttribArray(index);
// #else
//     f.EnableVertexAttribArray(index);
// #endif
// }
//
// inline void OpenGLFunctions::glFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
// #else
//     f.FramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
// #endif
// }
//
// inline void OpenGLFunctions::glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glFramebufferTexture2D(target, attachment, textarget, texture, level);
// #else
//     f.FramebufferTexture2D(target, attachment, textarget, texture, level);
// #endif
// }
//
// inline void OpenGLFunctions::glGenBuffers(GLsizei n, GLuint* buffers)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGenBuffers(n, buffers);
// #else
//     f.GenBuffers(n, buffers);
// #endif
// }
//
// inline void OpenGLFunctions::glGenerateMipmap(GLenum target)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGenerateMipmap(target);
// #else
//     f.GenerateMipmap(target);
// #endif
// }
//
// inline void OpenGLFunctions::glGenFramebuffers(GLsizei n, GLuint* framebuffers)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGenFramebuffers(n, framebuffers);
// #else
//     f.GenFramebuffers(n, framebuffers);
// #endif
// }
//
// inline void OpenGLFunctions::glGenRenderbuffers(GLsizei n, GLuint* renderbuffers)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGenRenderbuffers(n, renderbuffers);
// #else
//     f.GenRenderbuffers(n, renderbuffers);
// #endif
// }
//
// inline void OpenGLFunctions::glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetActiveAttrib(program, index, bufsize, length, size, type, name);
// #else
//     f.GetActiveAttrib(program, index, bufsize, length, size, type, name);
// #endif
// }
//
// inline void OpenGLFunctions::glGetActiveUniform(GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetActiveUniform(program, index, bufsize, length, size, type, name);
// #else
//     f.GetActiveUniform(program, index, bufsize, length, size, type, name);
// #endif
// }
//
// inline void OpenGLFunctions::glGetAttachedShaders(GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetAttachedShaders(program, maxcount, count, shaders);
// #else
//     f.GetAttachedShaders(program, maxcount, count, shaders);
// #endif
// }
//
// inline GLint OpenGLFunctions::glGetAttribLocation(GLuint program, const char* name)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     GLint result = ::glGetAttribLocation(program, name);
// #else
//     GLint result = f.GetAttribLocation(program, name);
// #endif
//     return result;
// }
//
// inline void OpenGLFunctions::glGetBufferParameteriv(GLenum target, GLenum pname, GLint* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetBufferParameteriv(target, pname, params);
// #else
//     f.GetBufferParameteriv(target, pname, params);
// #endif
// }
//
// inline void OpenGLFunctions::glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
// #else
//     f.GetFramebufferAttachmentParameteriv(target, attachment, pname, params);
// #endif
// }
//
// inline void OpenGLFunctions::glGetProgramiv(GLuint program, GLenum pname, GLint* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetProgramiv(program, pname, params);
// #else
//     f.GetProgramiv(program, pname, params);
// #endif
// }
//
// inline void OpenGLFunctions::glGetProgramInfoLog(GLuint program, GLsizei bufsize, GLsizei* length, char* infolog)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetProgramInfoLog(program, bufsize, length, infolog);
// #else
//     f.GetProgramInfoLog(program, bufsize, length, infolog);
// #endif
// }
//
// inline void OpenGLFunctions::glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetRenderbufferParameteriv(target, pname, params);
// #else
//     f.GetRenderbufferParameteriv(target, pname, params);
// #endif
// }
//
// inline void OpenGLFunctions::glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetShaderiv(shader, pname, params);
// #else
//     f.GetShaderiv(shader, pname, params);
// #endif
// }
//
// inline void OpenGLFunctions::glGetShaderInfoLog(GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetShaderInfoLog(shader, bufsize, length, infolog);
// #else
//     f.GetShaderInfoLog(shader, bufsize, length, infolog);
// #endif
// }
//
// inline void OpenGLFunctions::glGetShaderPrecisionFormat(GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetShaderPrecisionFormat(shadertype, precisiontype, range, precision);
// #else
//     f.GetShaderPrecisionFormat(shadertype, precisiontype, range, precision);
// #endif
// }
//
// inline void OpenGLFunctions::glGetShaderSource(GLuint shader, GLsizei bufsize, GLsizei* length, char* source)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetShaderSource(shader, bufsize, length, source);
// #else
//     f.GetShaderSource(shader, bufsize, length, source);
// #endif
// }
//
// inline void OpenGLFunctions::glGetUniformfv(GLuint program, GLint location, GLfloat* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetUniformfv(program, location, params);
// #else
//     f.GetUniformfv(program, location, params);
// #endif
// }
//
// inline void OpenGLFunctions::glGetUniformiv(GLuint program, GLint location, GLint* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetUniformiv(program, location, params);
// #else
//     f.GetUniformiv(program, location, params);
// #endif
// }
//
// inline GLint OpenGLFunctions::glGetUniformLocation(GLuint program, const char* name)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     GLint result = ::glGetUniformLocation(program, name);
// #else
//     GLint result = f.GetUniformLocation(program, name);
// #endif
//     return result;
// }
//
// inline void OpenGLFunctions::glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetVertexAttribfv(index, pname, params);
// #else
//     f.GetVertexAttribfv(index, pname, params);
// #endif
// }
//
// inline void OpenGLFunctions::glGetVertexAttribiv(GLuint index, GLenum pname, GLint* params)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetVertexAttribiv(index, pname, params);
// #else
//     f.GetVertexAttribiv(index, pname, params);
// #endif
// }
//
// inline void OpenGLFunctions::glGetVertexAttribPointerv(GLuint index, GLenum pname, void** pointer)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glGetVertexAttribPointerv(index, pname, pointer);
// #else
//     f.GetVertexAttribPointerv(index, pname, pointer);
// #endif
// }
//
// inline GLboolean OpenGLFunctions::glIsBuffer(GLuint buffer)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     GLboolean result = ::glIsBuffer(buffer);
// #else
//     GLboolean result = f.IsBuffer(buffer);
// #endif
//     return result;
// }
//
// inline GLboolean OpenGLFunctions::glIsFramebuffer(GLuint framebuffer)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     GLboolean result = ::glIsFramebuffer(framebuffer);
// #else
//     GLboolean result = f.IsFramebuffer(framebuffer);
// #endif
//     return result;
// }
//
// inline GLboolean OpenGLFunctions::glIsProgram(GLuint program)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     GLboolean result = ::glIsProgram(program);
// #else
//     GLboolean result = f.IsProgram(program);
// #endif
//     return result;
// }
//
// inline GLboolean OpenGLFunctions::glIsRenderbuffer(GLuint renderbuffer)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     GLboolean result = ::glIsRenderbuffer(renderbuffer);
// #else
//     GLboolean result = f.IsRenderbuffer(renderbuffer);
// #endif
//     return result;
// }
//
// inline GLboolean OpenGLFunctions::glIsShader(GLuint shader)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     GLboolean result = ::glIsShader(shader);
// #else
//     GLboolean result = f.IsShader(shader);
// #endif
//     return result;
// }
//
// inline void OpenGLFunctions::glLinkProgram(GLuint program)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glLinkProgram(program);
// #else
//     f.LinkProgram(program);
// #endif
// }
//
// inline void OpenGLFunctions::glReleaseShaderCompiler()
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glReleaseShaderCompiler();
// #else
//     f.ReleaseShaderCompiler();
// #endif
// }
//
// inline void OpenGLFunctions::glRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glRenderbufferStorage(target, internalformat, width, height);
// #else
//     f.RenderbufferStorage(target, internalformat, width, height);
// #endif
// }
//
// inline void OpenGLFunctions::glSampleCoverage(GLclampf value, GLboolean invert)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glSampleCoverage(value, invert);
// #else
//     f.SampleCoverage(value, invert);
// #endif
// }
//
// inline void OpenGLFunctions::glShaderBinary(GLint n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLint length)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glShaderBinary(n, shaders, binaryformat, binary, length);
// #else
//     f.ShaderBinary(n, shaders, binaryformat, binary, length);
// #endif
// }
//
// inline void OpenGLFunctions::glShaderSource(GLuint shader, GLsizei count, const char** string, const GLint* length)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glShaderSource(shader, count, string, length);
// #else
//     f.ShaderSource(shader, count, string, length);
// #endif
// }
//
// inline void OpenGLFunctions::glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glStencilFuncSeparate(face, func, ref, mask);
// #else
//     f.StencilFuncSeparate(face, func, ref, mask);
// #endif
// }
//
// inline void OpenGLFunctions::glStencilMaskSeparate(GLenum face, GLuint mask)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glStencilMaskSeparate(face, mask);
// #else
//     f.StencilMaskSeparate(face, mask);
// #endif
// }
//
// inline void OpenGLFunctions::glStencilOpSeparate(GLenum face, GLenum fail, GLenum zfail, GLenum zpass)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glStencilOpSeparate(face, fail, zfail, zpass);
// #else
//     f.StencilOpSeparate(face, fail, zfail, zpass);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform1f(GLint location, GLfloat x)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform1f(location, x);
// #else
//     f.Uniform1f(location, x);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform1fv(GLint location, GLsizei count, const GLfloat* v)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform1fv(location, count, v);
// #else
//     f.Uniform1fv(location, count, v);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform1i(GLint location, GLint x)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform1i(location, x);
// #else
//     f.Uniform1i(location, x);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform1iv(GLint location, GLsizei count, const GLint* v)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform1iv(location, count, v);
// #else
//     f.Uniform1iv(location, count, v);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform2f(GLint location, GLfloat x, GLfloat y)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform2f(location, x, y);
// #else
//     f.Uniform2f(location, x, y);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform2fv(GLint location, GLsizei count, const GLfloat* v)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform2fv(location, count, v);
// #else
//     f.Uniform2fv(location, count, v);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform2i(GLint location, GLint x, GLint y)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform2i(location, x, y);
// #else
//     f.Uniform2i(location, x, y);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform2iv(GLint location, GLsizei count, const GLint* v)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform2iv(location, count, v);
// #else
//     f.Uniform2iv(location, count, v);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform3f(location, x, y, z);
// #else
//     f.Uniform3f(location, x, y, z);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform3fv(GLint location, GLsizei count, const GLfloat* v)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform3fv(location, count, v);
// #else
//     f.Uniform3fv(location, count, v);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform3i(GLint location, GLint x, GLint y, GLint z)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform3i(location, x, y, z);
// #else
//     f.Uniform3i(location, x, y, z);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform3iv(GLint location, GLsizei count, const GLint* v)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform3iv(location, count, v);
// #else
//     f.Uniform3iv(location, count, v);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform4f(location, x, y, z, w);
// #else
//     f.Uniform4f(location, x, y, z, w);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform4fv(GLint location, GLsizei count, const GLfloat* v)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform4fv(location, count, v);
// #else
//     f.Uniform4fv(location, count, v);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform4i(GLint location, GLint x, GLint y, GLint z, GLint w)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform4i(location, x, y, z, w);
// #else
//     f.Uniform4i(location, x, y, z, w);
// #endif
// }
//
// inline void OpenGLFunctions::glUniform4iv(GLint location, GLsizei count, const GLint* v)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniform4iv(location, count, v);
// #else
//     f.Uniform4iv(location, count, v);
// #endif
// }
//
// inline void OpenGLFunctions::glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniformMatrix2fv(location, count, transpose, value);
// #else
//     f.UniformMatrix2fv(location, count, transpose, value);
// #endif
// }
//
// inline void OpenGLFunctions::glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniformMatrix3fv(location, count, transpose, value);
// #else
//     f.UniformMatrix3fv(location, count, transpose, value);
// #endif
// }
//
// inline void OpenGLFunctions::glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUniformMatrix4fv(location, count, transpose, value);
// #else
//     f.UniformMatrix4fv(location, count, transpose, value);
// #endif
// }
//
// inline void OpenGLFunctions::glUseProgram(GLuint program)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glUseProgram(program);
// #else
//     f.UseProgram(program);
// #endif
// }
//
// inline void OpenGLFunctions::glValidateProgram(GLuint program)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glValidateProgram(program);
// #else
//     f.ValidateProgram(program);
// #endif
// }
//
// inline void OpenGLFunctions::glVertexAttrib1f(GLuint indx, GLfloat x)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glVertexAttrib1f(indx, x);
// #else
//     f.VertexAttrib1f(indx, x);
// #endif
// }
//
// inline void OpenGLFunctions::glVertexAttrib1fv(GLuint indx, const GLfloat* values)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glVertexAttrib1fv(indx, values);
// #else
//     f.VertexAttrib1fv(indx, values);
// #endif
// }
//
// inline void OpenGLFunctions::glVertexAttrib2f(GLuint indx, GLfloat x, GLfloat y)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glVertexAttrib2f(indx, x, y);
// #else
//     f.VertexAttrib2f(indx, x, y);
// #endif
// }
//
// inline void OpenGLFunctions::glVertexAttrib2fv(GLuint indx, const GLfloat* values)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glVertexAttrib2fv(indx, values);
// #else
//     f.VertexAttrib2fv(indx, values);
// #endif
// }
//
// inline void OpenGLFunctions::glVertexAttrib3f(GLuint indx, GLfloat x, GLfloat y, GLfloat z)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glVertexAttrib3f(indx, x, y, z);
// #else
//     f.VertexAttrib3f(indx, x, y, z);
// #endif
// }
//
// inline void OpenGLFunctions::glVertexAttrib3fv(GLuint indx, const GLfloat* values)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glVertexAttrib3fv(indx, values);
// #else
//     f.VertexAttrib3fv(indx, values);
// #endif
// }
//
// inline void OpenGLFunctions::glVertexAttrib4f(GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glVertexAttrib4f(indx, x, y, z, w);
// #else
//     f.VertexAttrib4f(indx, x, y, z, w);
// #endif
// }
//
// inline void OpenGLFunctions::glVertexAttrib4fv(GLuint indx, const GLfloat* values)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glVertexAttrib4fv(indx, values);
// #else
//     f.VertexAttrib4fv(indx, values);
// #endif
// }
//
// inline void OpenGLFunctions::glVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr)
// {
// #if OPENGL_ES2 && PLATFORM_ANDROID
//     ::glVertexAttribPointer(indx, size, type, normalized, stride, ptr);
// #else
//     f.VertexAttribPointer(indx, size, type, normalized, stride, ptr);
// #endif
// }

}// namespace atlas
