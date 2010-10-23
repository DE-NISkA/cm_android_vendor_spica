#ifndef __OGL2FUNC_H
#define __OGL2FUNC_H


#ifdef __cplusplus
extern "C" {
#endif

 void         OGL2_glActiveTexture (GLenum texture);
 void         OGL2_glAttachShader (GLuint program, GLuint shader);
 void         OGL2_glBindAttribLocation (GLuint program, GLuint index, const char* name);
 void         OGL2_glBindBuffer (GLenum target, GLuint buffer);
 void         OGL2_glBindFramebuffer (GLenum target, GLuint framebuffer);
 void         OGL2_glBindRenderbuffer (GLenum target, GLuint renderbuffer);
 void         OGL2_glBindTexture (GLenum target, GLuint texture);
 void         OGL2_glBlendColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
 void         OGL2_glBlendEquation ( GLenum mode );
 void         OGL2_glBlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha);
 void         OGL2_glBlendFunc (GLenum sfactor, GLenum dfactor);
 void         OGL2_glBlendFuncSeparate (GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
 void         OGL2_glBufferData (GLenum target, GLsizeiptr size, const void* data, GLenum usage);
 void         OGL2_glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
 GLenum       OGL2_glCheckFramebufferStatus (GLenum target);
 void         OGL2_glClear (GLbitfield mask);
 void         OGL2_glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
 void         OGL2_glClearDepthf (GLclampf depth);
 void         OGL2_glClearStencil (GLint s);
 void         OGL2_glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
 void         OGL2_glCompileShader (GLuint shader);
 void         OGL2_glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data);
 void         OGL2_glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data);
 void         OGL2_glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
 void         OGL2_glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
 GLuint       OGL2_glCreateProgram (void);
 GLuint       OGL2_glCreateShader (GLenum type);
 void         OGL2_glCullFace (GLenum mode);
 void         OGL2_glDeleteBuffers (GLsizei n, const GLuint* buffers);
 void         OGL2_glDeleteFramebuffers (GLsizei n, const GLuint* framebuffers);
 void         OGL2_glDeleteProgram (GLuint program);
 void         OGL2_glDeleteRenderbuffers (GLsizei n, const GLuint* renderbuffers);
 void         OGL2_glDeleteShader (GLuint shader);
 void         OGL2_glDeleteTextures (GLsizei n, const GLuint* textures);
 void         OGL2_glDepthFunc (GLenum func);
 void         OGL2_glDepthMask (GLboolean flag);
 void         OGL2_glDepthRangef (GLclampf zNear, GLclampf zFar);
 void         OGL2_glDetachShader (GLuint program, GLuint shader);
 void         OGL2_glDisable (GLenum cap);
 void         OGL2_glDisableVertexAttribArray (GLuint index);
 void         OGL2_glDrawArrays (GLenum mode, GLint first, GLsizei count);
 void         OGL2_glDrawElements (GLenum mode, GLsizei count, GLenum type, const void* indices);
 void         OGL2_glEnable (GLenum cap);
 void         OGL2_glEnableVertexAttribArray (GLuint index);
 void         OGL2_glFinish (void);
 void         OGL2_glFlush (void);
 void         OGL2_glFramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
 void         OGL2_glFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
 void         OGL2_glFrontFace (GLenum mode);
 void         OGL2_glGenBuffers (GLsizei n, GLuint* buffers);
 void         OGL2_glGenerateMipmap (GLenum target);
 void         OGL2_glGenFramebuffers (GLsizei n, GLuint* framebuffers);
 void         OGL2_glGenRenderbuffers (GLsizei n, GLuint* renderbuffers);
 void         OGL2_glGenTextures (GLsizei n, GLuint* textures);
 void         OGL2_glGetActiveAttrib (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);
 void         OGL2_glGetActiveUniform (GLuint program, GLuint index, GLsizei bufsize, GLsizei* length, GLint* size, GLenum* type, char* name);
 void         OGL2_glGetAttachedShaders (GLuint program, GLsizei maxcount, GLsizei* count, GLuint* shaders);
 int          OGL2_glGetAttribLocation (GLuint program, const char* name);
 void         OGL2_glGetBooleanv (GLenum pname, GLboolean* params);
 void         OGL2_glGetBufferParameteriv (GLenum target, GLenum pname, GLint* params);
 GLenum       OGL2_glGetError (void);
 void         OGL2_glGetFloatv (GLenum pname, GLfloat* params);
 void         OGL2_glGetFramebufferAttachmentParameteriv (GLenum target, GLenum attachment, GLenum pname, GLint* params);
 void         OGL2_glGetIntegerv (GLenum pname, GLint* params);
 void         OGL2_glGetProgramiv (GLuint program, GLenum pname, GLint* params);
 void         OGL2_glGetProgramInfoLog (GLuint program, GLsizei bufsize, GLsizei* length, char* infolog);
 void         OGL2_glGetRenderbufferParameteriv (GLenum target, GLenum pname, GLint* params);
 void         OGL2_glGetShaderiv (GLuint shader, GLenum pname, GLint* params);
 void         OGL2_glGetShaderInfoLog (GLuint shader, GLsizei bufsize, GLsizei* length, char* infolog);
 void         OGL2_glGetShaderPrecisionFormat (GLenum shadertype, GLenum precisiontype, GLint* range, GLint* precision);
 void         OGL2_glGetShaderSource (GLuint shader, GLsizei bufsize, GLsizei* length, char* source);
 const GLubyte* OGL2_glGetString (GLenum name);
 void         OGL2_glGetTexParameterfv (GLenum target, GLenum pname, GLfloat* params);
 void         OGL2_glGetTexParameteriv (GLenum target, GLenum pname, GLint* params);
 void         OGL2_glGetUniformfv (GLuint program, GLint location, GLfloat* params);
 void         OGL2_glGetUniformiv (GLuint program, GLint location, GLint* params);
 int          OGL2_glGetUniformLocation (GLuint program, const char* name);
 void         OGL2_glGetVertexAttribfv (GLuint index, GLenum pname, GLfloat* params);
 void         OGL2_glGetVertexAttribiv (GLuint index, GLenum pname, GLint* params);
 void         OGL2_glGetVertexAttribPointerv (GLuint index, GLenum pname, void** pointer);
 void         OGL2_glHint (GLenum target, GLenum mode);
 GLboolean    OGL2_glIsBuffer (GLuint buffer);
 GLboolean    OGL2_glIsEnabled (GLenum cap);
 GLboolean    OGL2_glIsFramebuffer (GLuint framebuffer);
 GLboolean    OGL2_glIsProgram (GLuint program);
 GLboolean    OGL2_glIsRenderbuffer (GLuint renderbuffer);
 GLboolean    OGL2_glIsShader (GLuint shader);
 GLboolean    OGL2_glIsTexture (GLuint texture);
 void         OGL2_glLineWidth (GLfloat width);
 void         OGL2_glLinkProgram (GLuint program);
 void         OGL2_glPixelStorei (GLenum pname, GLint param);
 void         OGL2_glPolygonOffset (GLfloat factor, GLfloat units);
 void         OGL2_glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels);
 void         OGL2_glReleaseShaderCompiler (void);
 void         OGL2_glRenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
 void         OGL2_glSampleCoverage (GLclampf value, GLboolean invert);
 void         OGL2_glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
 void         OGL2_glShaderBinary (GLsizei n, const GLuint* shaders, GLenum binaryformat, const void* binary, GLsizei length);
 void         OGL2_glShaderSource (GLuint shader, GLsizei count, const char** string, const GLint* length);
 void         OGL2_glStencilFunc (GLenum func, GLint ref, GLuint mask);
 void         OGL2_glStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask);
 void         OGL2_glStencilMask (GLuint mask);
 void         OGL2_glStencilMaskSeparate (GLenum face, GLuint mask);
 void         OGL2_glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
 void         OGL2_glStencilOpSeparate (GLenum face, GLenum fail, GLenum zfail, GLenum zpass);
 void         OGL2_glTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
 void         OGL2_glTexParameterf (GLenum target, GLenum pname, GLfloat param);
 void         OGL2_glTexParameterfv (GLenum target, GLenum pname, const GLfloat* params);
 void         OGL2_glTexParameteri (GLenum target, GLenum pname, GLint param);
 void         OGL2_glTexParameteriv (GLenum target, GLenum pname, const GLint* params);
 void         OGL2_glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
 void         OGL2_glUniform1f (GLint location, GLfloat x);
 void         OGL2_glUniform1fv (GLint location, GLsizei count, const GLfloat* v);
 void         OGL2_glUniform1i (GLint location, GLint x);
 void         OGL2_glUniform1iv (GLint location, GLsizei count, const GLint* v);
 void         OGL2_glUniform2f (GLint location, GLfloat x, GLfloat y);
 void         OGL2_glUniform2fv (GLint location, GLsizei count, const GLfloat* v);
 void         OGL2_glUniform2i (GLint location, GLint x, GLint y);
 void         OGL2_glUniform2iv (GLint location, GLsizei count, const GLint* v);
 void         OGL2_glUniform3f (GLint location, GLfloat x, GLfloat y, GLfloat z);
 void         OGL2_glUniform3fv (GLint location, GLsizei count, const GLfloat* v);
 void         OGL2_glUniform3i (GLint location, GLint x, GLint y, GLint z);
 void         OGL2_glUniform3iv (GLint location, GLsizei count, const GLint* v);
 void         OGL2_glUniform4f (GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
 void         OGL2_glUniform4fv (GLint location, GLsizei count, const GLfloat* v);
 void         OGL2_glUniform4i (GLint location, GLint x, GLint y, GLint z, GLint w);
 void         OGL2_glUniform4iv (GLint location, GLsizei count, const GLint* v);
 void         OGL2_glUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
 void         OGL2_glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
 void         OGL2_glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
 void         OGL2_glUseProgram (GLuint program);
 void         OGL2_glValidateProgram (GLuint program);
 void         OGL2_glVertexAttrib1f (GLuint indx, GLfloat x);
 void         OGL2_glVertexAttrib1fv (GLuint indx, const GLfloat* values);
 void         OGL2_glVertexAttrib2f (GLuint indx, GLfloat x, GLfloat y);
 void         OGL2_glVertexAttrib2fv (GLuint indx, const GLfloat* values);
 void         OGL2_glVertexAttrib3f (GLuint indx, GLfloat x, GLfloat y, GLfloat z);
 void         OGL2_glVertexAttrib3fv (GLuint indx, const GLfloat* values);
 void         OGL2_glVertexAttrib4f (GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
 void         OGL2_glVertexAttrib4fv (GLuint indx, const GLfloat* values);
 void         OGL2_glVertexAttribPointer (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* ptr);
 void         OGL2_glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
 void*        OGL2_glMapBuffer (GLenum target, GLenum access);
 GLboolean    OGL2_glUnmapBuffer (GLenum target);

 void         OGL2_glLogicOpEXP(GLenum op);
 void         OGL2_glVaryingInterpolationEXP(const char* name, GLenum mode);
 void         OGL2_glAlphaFuncEXP (GLenum func, GLclampf ref);

 void*        OGL2_glMapTexture(GLenum target, GLint level, GLenum access);
 GLboolean    OGL2_glUnmapTexture(GLenum target);

 //Texture OES API - 2009.05.20
void         OGL2_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint *params);
void         OGL2_glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat *params);

#ifdef __cplusplus
}
#endif


#endif
