
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES += libcutils libutils libui
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS = -DFIMG_MASKING_WORKAROUND \
               -DMULTI_PROCESS \
               -DMULTI_CONTEXT \
               -Dlinux \
               -DPLATFORM_S3C6410 \
               -DFIMG_WRONG_INTERPOLATION_WORKAROUND \
               -DFIMG_CLIPPER_WORKAROUND \
               -DPALETTE_SW_WORKAROUND \
               -DBM_PMEM \
               -DCACHE_MEM \
               -DCONFIG_ANDOIRD_PMEM

LOCAL_C_INCLUDES += $(LOCAL_PATH)/inc \
                    $(LOCAL_PATH)/../gpi \
                    $(LOCAL_PATH)/../ustl/include


LOCAL_SRC_FILES := \
        src/burstcopy.S                 \
        src/sendtofifo.S		\
        src/global.c                    \
        src/primitive.c                 \
        src/raster.c                    \
        src/interface.c                 \
        src/perfrag.c                   \
        src/shader.c                    \
        src/texture.c                   \
        src/letsleep.c                  \
        src/glTex.cpp                   \
        src/glCore.cpp                  \
        src/glShader.cpp                \
        src/glShaderApi.cpp             \
        src/glFimg.new.cpp              \
        src/glFramebufferObject.cpp     \
        src/platform.cpp                \
        src/ChunkAlloc.cpp              \
        src/libMain.cpp                 \
        src/pixel.cpp                   

LOCAL_STATIC_LIBRARIES += libustl
LOCAL_CFLAGS += -DPLATFORM_ANDROID
LOCAL_MODULE := libGLESv2_fimg
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/egl

include $(BUILD_SHARED_LIBRARY)
