# Aitk
#
# t0kt0ckus
# (C) 2014
#
LOCAL_PATH := $(call my-dir)

# Prebuilt libraries: "system" libdl.so and libdvm.so
#
EXT_LIBS_PATH := $(LOCAL_PATH)/libs
include $(CLEAR_VARS)
LOCAL_MODULE    := pbdl
LOCAL_SRC_FILES := $(EXT_LIBS_PATH)/libdl.so
include $(PREBUILT_SHARED_LIBRARY)
include $(CLEAR_VARS)
LOCAL_MODULE    := pbdvm
LOCAL_SRC_FILES := $(EXT_LIBS_PATH)/libdvm.so
include $(PREBUILT_SHARED_LIBRARY)

# hijack: crmulliner's tool "fork" [https://github.com/crmulliner/adbi/hijack]
#
include $(CLEAR_VARS)
LOCAL_MODULE    := hijack
HIJACK_SRC_PATH := $(LOCAL_PATH)/hijack
LOCAL_SRC_FILES := $(HIJACK_SRC_PATH)/hijack.c
LOCAL_ARM_MODE  := arm
include $(BUILD_EXECUTABLE)

# libyal: yet another logger ...
#
include $(CLEAR_VARS)
LOCAL_MODULE     := yal
LIBYAL_SRC_PATH := $(LOCAL_PATH)/yal
LOCAL_SRC_FILES  := $(LIBYAL_SRC_PATH)/yal.c
LOCAL_ARM_MODE := arm
include $(BUILD_STATIC_LIBRARY)

# libadbi: crmulliner's library "fork" [https://github.com/crmulliner/adbi/instruments]
#
include $(CLEAR_VARS)
LOCAL_MODULE     := adbi
LIBADBI_SRC_PATH := $(LOCAL_PATH)/adbi
LOCAL_SRC_FILES  := $(LIBADBI_SRC_PATH)/adbi.c $(LIBADBI_SRC_PATH)/hook.c $(LIBADBI_SRC_PATH)/util.c
LOCAL_ARM_MODE := arm
LOCAL_STATIC_LIBRARIES := yal
include $(BUILD_STATIC_LIBRARY)

# libadbi_test: attemmpts to hook epoll_wait() to test libadbi
#
include $(CLEAR_VARS)
LOCAL_MODULE     := adbi_test
LIBTEST_SRC_PATH := $(LOCAL_PATH)/adbi_test
LOCAL_SRC_FILES  := $(LIBTEST_SRC_PATH)/epoll.c $(LIBTEST_SRC_PATH)/epoll_arm.c.arm
LOCAL_STATIC_LIBRARIES := adbi
include $(BUILD_SHARED_LIBRARY)

# libddi: crmulliner's library "fork" [https://github.com/crmulliner/ddi]
#
include $(CLEAR_VARS)
LOCAL_MODULE     := ddi
LIBDDI_SRC_PATH := $(LOCAL_PATH)/ddi
LOCAL_SRC_FILES  := $(LIBDDI_SRC_PATH)/dexstuff.c.arm $(LIBDDI_SRC_PATH)/dalvik_hook.c $(LIBDDI_SRC_PATH)/ddi.c
LOCAL_STATIC_LIBRARIES := adbi
LOCAL_SHARED_LIBRARIES := pdvm pbdl
include $(BUILD_STATIC_LIBRARY)

# libddi_test: attemmpts to hook epoll_wait()/String to test libddi
#
include $(CLEAR_VARS)
LOCAL_MODULE     := ddi_test
LIBDDI_SRC_PATH := $(LOCAL_PATH)/ddi_test
LOCAL_SRC_FILES  := $(LIBDDI_SRC_PATH)/mon_arm.c.arm $(LIBDDI_SRC_PATH)/mon.c
LOCAL_STATIC_LIBRARIES := adbi ddi
LOCAL_SHARED_LIBRARIES := pbdvm pbdl
include $(BUILD_SHARED_LIBRARY)

# libaitk_dumpall: list all dvm-loaded classes (pthread version)
#
include $(CLEAR_VARS)
LOCAL_MODULE     := aitk_dumpall
AITK_SRC_PATH := $(LOCAL_PATH)/aitk_dumpall
LOCAL_SRC_FILES  := $(AITK_SRC_PATH)/dumpall_arm.c.arm $(AITK_SRC_PATH)/dumpall.c
LOCAL_STATIC_LIBRARIES := adbi ddi
LOCAL_SHARED_LIBRARIES := pbdvm pbdl
include $(BUILD_SHARED_LIBRARY)

# aitk_dumpclass: dump info about a dvm-loaded class
#
include $(CLEAR_VARS)
LOCAL_MODULE     := aitk_dumpclass
AITK_SRC_PATH := $(LOCAL_PATH)/aitk_dumpclass
LOCAL_SRC_FILES  := $(AITK_SRC_PATH)/dumpclass_arm.c.arm $(AITK_SRC_PATH)/dumpclass.c
LOCAL_STATIC_LIBRARIES := adbi ddi
LOCAL_SHARED_LIBRARIES := pbdvm pbdl
include $(BUILD_SHARED_LIBRARY)

# aitk_send_raw_pdu: 
#
include $(CLEAR_VARS)
LOCAL_MODULE     := aitk_send_raw_pdu
AITK_SRC_PATH := $(LOCAL_PATH)/aitk_send_raw_pdu
LOCAL_SRC_FILES  := $(AITK_SRC_PATH)/send_raw_pdu_arm.c.arm $(AITK_SRC_PATH)/send_raw_pdu.c
LOCAL_STATIC_LIBRARIES := adbi ddi
LOCAL_SHARED_LIBRARIES := pbdvm pbdl
include $(BUILD_SHARED_LIBRARY)


