#ifndef __FUNC_DEC_H
#define __FUNC_DEC_H
#ifdef __cplusplus

extern "C"
{
#endif

void OGL2_glVaryingInterpolationEXP(const char* , GLenum );

void OGL2_glGetVaryingInterpolationEXP(const char *, GLint * );

void OGL2_glCompressedTexSubImage2D (GLenum , GLint , GLint , GLint , GLsizei , GLsizei , GLenum , GLsizei , const GLvoid *);

void OGL2_glCompressedTexImage2D (GLenum , GLint , GLenum , GLsizei , GLsizei , GLint , GLsizei , const GLvoid *);

void OGL2_glCopyTexSubImage2D (GLenum , GLint , GLint , GLint , GLint , GLint , GLsizei , GLsizei );

void OGL2_glLogicOpEXP (GLenum );

void OGL2_glAlphaFuncEXP (GLenum , GLclampf );

void OGL2_glActiveTexture (GLenum );

void OGL2_glBlendFunc (GLenum sfactor, GLenum dfactor);

void OGL2_glEnableClientState (GLenum);

void OGL2_glPixelStorei(GLenum , GLint);

void OGL2_glReadPixels (GLint ,GLint ,GLsizei ,GLsizei ,GLenum , GLenum ,GLvoid *);

void OGL2_glTexSubImage2D(GLenum , GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);

void OGL2_glDeleteProgram (GLuint program);

void OGL2_glDeleteShader (GLuint );

GLenum OGL2_glCheckFramebufferStatus(GLenum );

GLuint OGL2_glCreateProgram (void);

GLuint OGL2_glCreateShader (GLenum );

void OGL2_glAttachShader (GLuint , GLuint );

void OGL2_glBindAttribLocation (GLuint , GLuint , const char *);

void OGL2_glShaderBinary(GLint , GLuint *, GLenum , const void *, GLint );

void OGL2_glLinkProgram (GLuint );

void OGL2_glGetProgramInfoLog (GLuint , GLsizei , GLsizei *, char *);

void OGL2_glUseProgram (GLuint );

void OGL2_glShaderSource(GLuint  , GLsizei  , const char **, const GLint *);

void OGL2_glCompileShader(GLuint  );

void OGL2_glUniform1f(GLint ,GLfloat );

void OGL2_glUniform2f(GLint ,GLfloat ,GLfloat );

void OGL2_glUniform3f(GLint location,GLfloat v0,GLfloat v1,GLfloat v2);

void OGL2_glUniform4f(GLint location,GLfloat v0,GLfloat v1,GLfloat v2,GLfloat v3);

void OGL2_glUniform1i(GLint location,GLint v0);

void OGL2_glUniform2i(GLint location,GLint v0,GLint v1);

void OGL2_glUniform3i(GLint location,GLint v0,GLint v1,GLint v2);

void OGL2_glUniform4i(GLint location,GLint v0,GLint v1,GLint v2,GLint v3);

void OGL2_glUniform1fv(GLint location,GLsizei count,const GLfloat *value);

void OGL2_glUniform2fv(GLint location,GLsizei count,const GLfloat *value);

void OGL2_glUniform3fv(GLint location,GLsizei count,const GLfloat *value);

void OGL2_glUniform4fv(GLint location,GLsizei count,const GLfloat *value);

void OGL2_glUniform1iv(GLint location,GLsizei count,const GLint *value);

void OGL2_glUniform2iv(GLint location,GLsizei count,const GLint *value);

void OGL2_glUniform3iv(GLint location,GLsizei count,const GLint *value);

void OGL2_glUniform4iv(GLint location,GLsizei count,const GLint *value);

void OGL2_glUniformMatrix2fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat *value);

void OGL2_glUniformMatrix3fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat *value);

void OGL2_glUniformMatrix4fv(GLint location,GLsizei count,GLboolean transpose,const GLfloat *value);

GLint OGL2_glGetUniformLocation(GLuint program,const char *name);

void OGL2_glDrawArrays(GLenum mode, GLint first, GLsizei count);

void OGL2_glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);

//  TODO convert 2.0 functions to 2.0 ES functions later
void OGL2_glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized,GLsizei stride, const GLvoid *pointer);

void OGL2_glClear(GLbitfield mask);

void OGL2_glClearColor(GLfloat red, GLfloat green, GLfloat blue , GLfloat alpha);

void OGL2_glEnableVertexAttribArray(unsigned int  index);

GLint OGL2_glGetAttribLocation(GLuint program,const char *name);

void OGL2_glEnableVertexAttribArray( GLuint index );

void OGL2_glVertexAttrib4fv(GLuint index,const GLfloat *pointer);

void OGL2_glViewport (GLuint x,GLuint y,GLsizei w, GLsizei h);

void OGL2_glMatrixMode (int index);

void OGL2_glLoadIdentity(void);

void OGL2_glOrthof(GLfloat left, GLfloat right, GLfloat top,GLfloat bottom, GLfloat znear, GLfloat zfar);

void OGl2_glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);

void OGL2_glutSolidCube(GLfloat r);

void OGL2_glutSolidSphere(GLfloat, int, int);

void OGL2_glDepthFunc (GLenum func);

void OGL2_glGetFloatv(GLenum pname, GLfloat *params);

void OGL2_glTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels);

void OGL2_glGenTextures (GLsizei n, GLuint *textures);

void OGL2_glBindTexture (GLenum target, GLuint texture);

void OGL2_glTexParameteri (GLenum target, GLenum pname, GLint param);

void OGL2_glTexParameterf (GLenum target, GLenum pname, GLfloat param);

void OGL2_glLineWidth (GLfloat width);

void OGL2_glVertexAttrib4f (GLuint indx, GLfloat x, GLfloat y, GLfloat z, GLfloat w);

void OGL2_glStencilFunc (GLenum func, GLint ref, GLuint mask);

void OGL2_glStencilMask (GLuint mask);

void OGL2_glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);

void OGL2_glClearStencil (GLint s);

void OGL2_glPolygonOffset (GLfloat factor, GLfloat units);

void OGL2_glScissor (GLint x, GLint y, GLsizei width, GLsizei height);

void OGL2_glSampleCoverage (GLclampf value, GLboolean invert);

void OGL2_glVertexAttrib1f (GLuint indx, GLfloat x);

void OGL2_glVertexAttrib3f (GLuint indx, GLfloat x, GLfloat y, GLfloat z);

int  OGL2_glGetAttribLocation (GLuint program, const char *name);

void OGL2_glDisableVertexAttribArray (GLuint index);

void OGL2_glCullFace (GLenum mode);

void OGL2_glTexParameteriv (GLenum target, GLenum pname, const GLint *params);

GLboolean OGL2_glIsTexture (GLuint texture);

GLboolean OGL2_glIsBuffer (GLuint buffer);

void OGL2_glHint (GLenum target, GLenum mode);

void OGL2_glGetTexParameteriv (GLenum target, GLenum pname, GLint *params);

const GLubyte * OGL2_glGetString (GLenum name);

void OGL2_glGetPointerv (GLenum pname, void **params);

void OGL2_glGetIntegerv (GLenum pname, GLint *params);

GLenum OGL2_glGetError (void);

void OGL2_glGenBuffers (GLsizei n, GLuint *buffers);

void OGL2_glGetBufferParameteriv (GLenum target, GLenum pname, GLint *params);

void OGL2_glGetBooleanv (GLenum pname, GLboolean *params);

void OGL2_glFrontFace (GLenum mode);

void OGL2_glFinish (void);

void OGL2_glFlush(void);

void OGL2_glDepthMask (GLboolean flag);

void OGL2_glDeleteBuffers (GLsizei n, const GLuint *buffers);

void OGL2_glDeleteTextures (GLsizei n, const GLuint *textures);

void OGL2_glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);

void OGL2_glBufferData (GLenum target, GLsizeiptr size, const void *data, GLenum usage);

void OGL2_glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const void *data);

void OGL2_glBindBuffer (GLenum target, GLuint buffer);

void OGL2_glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params);

void OGL2_glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params);

void OGL2_glGetFloatv (GLenum pname, GLfloat *params);

void OGL2_glDepthRangef (GLclampf zNear, GLclampf zFar);

void OGL2_glClearDepthf (GLclampf depth);

void OGL2_glGetVertexAttribPointerv (GLuint index, GLenum pname, void **pointer);

void OGL2_glGetVertexAttribiv (GLuint index, GLenum pname, GLint *params);

void OGL2_glGetVertexAttribfv (GLuint index, GLenum pname, GLfloat *params);

void OGL2_glPointSize(GLfloat size);

void OGL2_glEnable(GLenum cap);

void OGL2_glDisable(GLenum cap);

void OGL2_glCopyTexImage2D (GLenum ,GLint ,GLenum, GLint, GLint, GLsizei, GLsizei, GLint);

GLboolean OGL2_glIsEnabled (GLenum cap);

void  OGL2_glGenerateMipmap(GLenum );

//Texture OES API - 2009.05.20
void OGL2_glGetTexLevelParameteriv(GLenum , GLint, GLenum, GLint *);
void OGL2_glGetTexLevelParameterfv(GLenum , GLint, GLenum, GLint *);


#ifdef __cplusplus
}
#endif
#endif
