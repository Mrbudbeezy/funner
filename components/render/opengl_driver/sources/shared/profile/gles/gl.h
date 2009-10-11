#ifndef RENDER_GL_DRIVER_GLES_PROFILE_HEADER
#define RENDER_GL_DRIVER_GLES_PROFILE_HEADER

#include <shared/profile/gles/gl_types.h>
#include <shared/profile/gles/glext.h>
#include <shared/profile/gles/entries.h>

/*
    ��८�।������ ��� ��饭�� �����㥬���
*/

typedef PFNGLBLENDEQUATIONOESPROC                       PFNGLBLENDEQUATIONPROC;
typedef PFNGLBLENDEQUATIONSEPARATEOESPROC               PFNGLBLENDEQUATIONSEPARATEPROC;
typedef PFNGLBLENDFUNCSEPARATEOESPROC                   PFNGLBLENDFUNCSEPARATEPROC;
typedef PFNGLISRENDERBUFFEROESPROC                      PFNGLISRENDERBUFFERPROC;
typedef PFNGLBINDRENDERBUFFEROESPROC                    PFNGLBINDRENDERBUFFERPROC;
typedef PFNGLDELETERENDERBUFFERSOESPROC                 PFNGLDELETERENDERBUFFERSPROC;
typedef PFNGLGENRENDERBUFFERSOESPROC                    PFNGLGENRENDERBUFFERSPROC;
typedef PFNGLRENDERBUFFERSTORAGEOESPROC                 PFNGLRENDERBUFFERSTORAGEPROC;
typedef PFNGLGETRENDERBUFFERPARAMETERIVOESPROC          PFNGLGETRENDERBUFFERPARAMETERIVPROC;
typedef PFNGLISFRAMEBUFFEROESPROC                       PFNGLISFRAMEBUFFERPROC;
typedef PFNGLBINDFRAMEBUFFEROESPROC                     PFNGLBINDFRAMEBUFFERPROC;
typedef PFNGLDELETEFRAMEBUFFERSOESPROC                  PFNGLDELETEFRAMEBUFFERSPROC;
typedef PFNGLGENFRAMEBUFFERSOESPROC                     PFNGLGENFRAMEBUFFERSPROC;
typedef PFNGLCHECKFRAMEBUFFERSTATUSOESPROC              PFNGLCHECKFRAMEBUFFERSTATUSPROC;
typedef PFNGLFRAMEBUFFERRENDERBUFFEROESPROC             PFNGLFRAMEBUFFERRENDERBUFFERPROC;
typedef PFNGLFRAMEBUFFERTEXTURE2DOESPROC                PFNGLFRAMEBUFFERTEXTURE2DPROC;
typedef PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVOESPROC PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC;

typedef void (GL_APIENTRY* PFNGLACTIVETEXTUREPROC)(GLenum texture);
typedef void (GL_APIENTRY* PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (GL_APIENTRY* PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
typedef void (GL_APIENTRY* PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);
typedef void (GL_APIENTRY* PFNGLCLIENTACTIVETEXTUREPROC)(GLenum texture);
typedef void (GL_APIENTRY* PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
typedef void (GL_APIENTRY* PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint *buffers);
typedef void (GL_APIENTRY* PFNGLGENBUFFERSPROC)(GLsizei n, GLuint *buffers);
typedef void (GL_APIENTRY* PFNGLCOMPRESSEDTEXIMAGE2DPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);

#define GL_NONE                                         GL_NONE_OES
#define GL_FRAMEBUFFER                                  GL_FRAMEBUFFER_OES
#define GL_RENDERBUFFER                                 GL_RENDERBUFFER_OES
#define GL_RGBA4                                        GL_RGBA4_OES
#define GL_RGB5_A1                                      GL_RGB5_A1_OES
#define GL_RGB565                                       GL_RGB565_OES
#define GL_DEPTH_COMPONENT16                            GL_DEPTH_COMPONENT16_OES
#define GL_RENDERBUFFER_WIDTH                           GL_RENDERBUFFER_WIDTH_OES
#define GL_RENDERBUFFER_HEIGHT                          GL_RENDERBUFFER_HEIGHT_OES
#define GL_RENDERBUFFER_INTERNAL_FORMAT                 GL_RENDERBUFFER_INTERNAL_FORMAT_OES
#define GL_RENDERBUFFER_RED_SIZE                        GL_RENDERBUFFER_RED_SIZE_OES
#define GL_RENDERBUFFER_GREEN_SIZE                      GL_RENDERBUFFER_GREEN_SIZE_OES
#define GL_RENDERBUFFER_BLUE_SIZE                       GL_RENDERBUFFER_BLUE_SIZE_OES
#define GL_RENDERBUFFER_ALPHA_SIZE                      GL_RENDERBUFFER_ALPHA_SIZE_OES
#define GL_RENDERBUFFER_DEPTH_SIZE                      GL_RENDERBUFFER_DEPTH_SIZE_OES
#define GL_RENDERBUFFER_STENCIL_SIZE                    GL_RENDERBUFFER_STENCIL_SIZE_OES
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE           GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_OES
#define GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME           GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_OES
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL         GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL_OES
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE_OES
#define GL_COLOR_ATTACHMENT0                            GL_COLOR_ATTACHMENT0_OES
#define GL_DEPTH_ATTACHMENT                             GL_DEPTH_ATTACHMENT_OES
#define GL_STENCIL_ATTACHMENT                           GL_STENCIL_ATTACHMENT_OES
#define GL_FRAMEBUFFER_COMPLETE                         GL_FRAMEBUFFER_COMPLETE_OES
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT            GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_OES
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT    GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_OES
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS            GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_OES
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS               GL_FRAMEBUFFER_INCOMPLETE_FORMATS_OES
#define GL_FRAMEBUFFER_UNSUPPORTED                      GL_FRAMEBUFFER_UNSUPPORTED_OES
#define GL_FRAMEBUFFER_BINDING                          GL_FRAMEBUFFER_BINDING_OES
#define GL_RENDERBUFFER_BINDING                         GL_RENDERBUFFER_BINDING_OES
#define GL_MAX_RENDERBUFFER_SIZE                        GL_MAX_RENDERBUFFER_SIZE_OES
#define GL_INVALID_FRAMEBUFFER_OPERATION                GL_INVALID_FRAMEBUFFER_OPERATION_OES

#define GL_TEXTURE_CUBE_MAP            GL_TEXTURE_CUBE_MAP_OES
#define GL_TEXTURE_BINDING_CUBE_MAP    GL_TEXTURE_BINDING_CUBE_MAP_OES
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X GL_TEXTURE_CUBE_MAP_POSITIVE_X_OES
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X GL_TEXTURE_CUBE_MAP_NEGATIVE_X_OES
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y GL_TEXTURE_CUBE_MAP_POSITIVE_Y_OES
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_OES
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z GL_TEXTURE_CUBE_MAP_POSITIVE_Z_OES
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_OES
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE   GL_MAX_CUBE_MAP_TEXTURE_SIZE_OES

#endif
