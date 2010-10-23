LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES += libcutils libutils libui liblog
LOCAL_ARM_MODE := arm

LOCAL_CFLAGS = -DFIMG_GLES_11 \
	       -DMULTI_CONTEXT \
	       -Dlinux \
	       -DPLATFORM_S3C6410 \
	       -DPALETTE_SW_WORKAROUND \
	       -DFIMG_MASKING_WORKAROUND \
	       -DFIMG_WRONG_INTERPOLATION_WORKAROUND \
	       -DFIMG_CLIPPER_WORKAROUND \
	       -DMULTI_PROCESS \
	       -DBM_PMEM \
	       -DCACHE_MEM
#	       -DUSE_3D_PM
#	       -D__DEBUG
#	       -DFSO_JITO
#	       -DHWCLEAR_ONLY
#	       -DGLES11_LOG_ENABLE
#	       -DFIMG_1_2_SHORT_VERTEX_DATA_WORKAROUND
#	       -D__DEBUG

LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc \
                    $(LOCAL_PATH)/../gles20/inc \
                    $(LOCAL_PATH)/../gpi \
                    $(LOCAL_PATH)/../ustl/include

LOCAL_SRC_FILES := \
        src/loadShader.cpp                      \
        src/pointer.cpp                         \
        src/glBuffer.c                          \
        src/glClear.c                           \
        src/glClip.c                            \
        src/glColor.c                           \
        src/glEnable.c                          \
        src/glGets.c                            \
        src/glImplMatrix.c                      \
        src/glMatrix.c                          \
        src/glMisc.c                            \
        src/glPerFragment.c                     \
        src/glTex.c                             \
        src/glVertex.c                          \
        src/hashMap.c                           \
        src/nMatrix.c                           \
        src/render.c                            \
        src/eglInterface.cpp                    \
        ../gles20/src/burstcopy.S               \
        ../gles20/src/sendtofifo.S              \
        ../gles20/src/global.c                  \
        ../gles20/src/primitive.c               \
        ../gles20/src/raster.c                  \
        ../gles20/src/interface.c               \
        ../gles20/src/perfrag.c                 \
        ../gles20/src/shader.c                  \
        ../gles20/src/texture.c                 \
        ../gles20/src/letsleep.c                \
        ../gles20/src/glTex.cpp                 \
        ../gles20/src/glCore.cpp                \
        ../gles20/src/glShader.cpp              \
        ../gles20/src/glShaderApi.cpp           \
        ../gles20/src/glFimg.new.cpp            \
        ../gles20/src/glFramebufferObject.cpp   \
        ../gles20/src/platform.cpp              \
        ../gles20/src/ChunkAlloc.cpp            \
        ../gles20/src/libMain.cpp               \
        ../gles20/src/pixel.cpp                 

LOCAL_MODULE := libGLESv1_CM_fimg
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/egl

LOCAL_CFLAGS += -DPLATFORM_ANDROID

LOCAL_STATIC_LIBRARIES += libustl

include $(BUILD_SHARED_LIBRARY)
