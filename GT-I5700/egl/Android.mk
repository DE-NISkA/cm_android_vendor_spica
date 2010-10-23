LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SHARED_LIBRARIES += libcutils libutils libui libhardware libdl
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS+=-DDIRECT_WINDOW_DRAWING -DDOUBLE_BUFFER -Dlinux -DDEFAULT_FB_NUM=$(DEFAULT_FB_NUM)
LOCAL_CFLAGS+=-DCACHE_MEM -DCONFIG_ANDROID_PMEM

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/../gpi

LOCAL_SRC_FILES := \
		egl.cpp 			\
		egl_gles.cpp 			\
		egl_vg.c 			\
		platform/platform_eclair.cpp 	\
		adopter/sec/sec_gles.cpp 	\
		adopter/sec/sec_gles11.cpp 	\
		adopter/sec/sec_gles20.cpp 	\
		adopter/huone/alexvg_engine.c   \
		common/egl_config.c             \
		../gles20/src/ChunkAlloc.cpp    \
		burstcopy.S

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/egl
LOCAL_MODULE := libEGL_fimg

include $(BUILD_SHARED_LIBRARY)
