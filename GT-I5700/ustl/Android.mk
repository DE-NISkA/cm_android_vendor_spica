LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SHARED_LIBRARIES := libexpat libcrypto
LOCAL_CFLAGS += -DPLATFORM_ANDROID -DPLATFORM_S3C6410

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_SRC_FILES:= \
		    src/bktrace.cpp \
		    src/memblock.cpp \
		    src/ofstream.cpp \
		    src/ualgobase.cpp \
		    src/unew.cpp \
		    src/cmemlink.cpp \
		    src/memlink.cpp \
		    src/sistream.cpp \
		    src/ubitset.cpp \
		    src/ustdxept.cpp \
		    src/fstream.cpp \
		    src/mistream.cpp \
		    src/sostream.cpp \
		    src/uexception.cpp \
		    src/ustring.cpp

LOCAL_MODULE:=libustl
include $(BUILD_STATIC_LIBRARY)

