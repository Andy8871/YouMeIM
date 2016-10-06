LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
ifeq ($(NEON),1)
LOCAL_ARM_NEON := true
LOCAL_CFLAGS += -DHAVE_NEON
LOCAL_CPPFLAGS += -DHAVE_NEON
endif
endif

LOCAL_MODULE := curl

LOCAL_CPP_EXTENSION := .cc  .cpp

LOCAL_CFLAGS := \
-DHAVE_CONFIG_H -DANDROID\
-Os \
-fvisibility=hidden\

LOCAL_CPPFLAGS +=\
-DHAVE_CONFIG_H -DANDROID\
-Os \
-std=c++11\
-fvisibility=hidden\

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../.. $(LOCAL_PATH)/..

LOCAL_CFLAGS := -Wpointer-arith -Wwrite-strings -Wunused -Winline -Wnested-externs -Wmissing-declarations -Wmissing-prototypes -Wno-long-long -Wfloat-equal -Wno-multichar -Wsign-compare -Wno-format-nonliteral -Wendif-labels -Wstrict-prototypes -Wdeclaration-after-statement -Wno-system-headers -DHAVE_CONFIG_H

LOCAL_CPP_EXTENSION := .cc  .cpp

LOCAL_SRC_FILES := \
$(LOCAL_PATH)/../amigaos.c \
$(LOCAL_PATH)/../asyn-ares.c \
$(LOCAL_PATH)/../asyn-thread.c \
$(LOCAL_PATH)/../axtls.c \
$(LOCAL_PATH)/../base64.c \
$(LOCAL_PATH)/../bundles.c \
$(LOCAL_PATH)/../conncache.c \
$(LOCAL_PATH)/../connect.c \
$(LOCAL_PATH)/../content_encoding.c \
$(LOCAL_PATH)/../cookie.c \
$(LOCAL_PATH)/../curl_addrinfo.c \
$(LOCAL_PATH)/../curl_des.c \
$(LOCAL_PATH)/../curl_endian.c \
$(LOCAL_PATH)/../curl_fnmatch.c \
$(LOCAL_PATH)/../curl_gethostname.c \
$(LOCAL_PATH)/../curl_gssapi.c \
$(LOCAL_PATH)/../curl_memrchr.c \
$(LOCAL_PATH)/../curl_multibyte.c \
$(LOCAL_PATH)/../curl_ntlm_core.c \
$(LOCAL_PATH)/../curl_ntlm_msgs.c \
$(LOCAL_PATH)/../curl_ntlm_wb.c \
$(LOCAL_PATH)/../curl_ntlm.c \
$(LOCAL_PATH)/../curl_rtmp.c \
$(LOCAL_PATH)/../curl_sasl_gssapi.c \
$(LOCAL_PATH)/../curl_sasl_sspi.c \
$(LOCAL_PATH)/../curl_sasl.c \
$(LOCAL_PATH)/../curl_sspi.c \
$(LOCAL_PATH)/../curl_threads.c \
$(LOCAL_PATH)/../cyassl.c \
$(LOCAL_PATH)/../darwinssl.c \
$(LOCAL_PATH)/../dict.c \
$(LOCAL_PATH)/../dotdot.c \
$(LOCAL_PATH)/../easy.c \
$(LOCAL_PATH)/../escape.c \
$(LOCAL_PATH)/../file.c \
$(LOCAL_PATH)/../fileinfo.c \
$(LOCAL_PATH)/../formdata.c \
$(LOCAL_PATH)/../ftp.c \
$(LOCAL_PATH)/../ftplistparser.c \
$(LOCAL_PATH)/../getenv.c \
$(LOCAL_PATH)/../getinfo.c \
$(LOCAL_PATH)/../gopher.c \
$(LOCAL_PATH)/../gskit.c \
$(LOCAL_PATH)/../gtls.c \
$(LOCAL_PATH)/../hash.c \
$(LOCAL_PATH)/../hmac.c \
$(LOCAL_PATH)/../hostasyn.c \
$(LOCAL_PATH)/../hostcheck.c \
$(LOCAL_PATH)/../hostip.c \
$(LOCAL_PATH)/../hostip4.c \
$(LOCAL_PATH)/../hostip6.c \
$(LOCAL_PATH)/../hostsyn.c \
$(LOCAL_PATH)/../http_chunks.c \
$(LOCAL_PATH)/../http_digest.c \
$(LOCAL_PATH)/../http_negotiate_sspi.c \
$(LOCAL_PATH)/../http_negotiate.c \
$(LOCAL_PATH)/../http_proxy.c \
$(LOCAL_PATH)/../http.c \
$(LOCAL_PATH)/../http2.c \
$(LOCAL_PATH)/../idn_win32.c \
$(LOCAL_PATH)/../if2ip.c \
$(LOCAL_PATH)/../imap.c \
$(LOCAL_PATH)/../inet_ntop.c \
$(LOCAL_PATH)/../inet_pton.c \
$(LOCAL_PATH)/../krb5.c \
$(LOCAL_PATH)/../ldap.c \
$(LOCAL_PATH)/../llist.c \
$(LOCAL_PATH)/../md4.c \
$(LOCAL_PATH)/../md5.c \
$(LOCAL_PATH)/../memdebug.c \
$(LOCAL_PATH)/../mprintf.c \
$(LOCAL_PATH)/../multi.c \
$(LOCAL_PATH)/../netrc.c \
$(LOCAL_PATH)/../non-ascii.c \
$(LOCAL_PATH)/../nonblock.c \
$(LOCAL_PATH)/../nss.c \
$(LOCAL_PATH)/../openldap.c \
$(LOCAL_PATH)/../openssl.c \
$(LOCAL_PATH)/../parsedate.c \
$(LOCAL_PATH)/../pingpong.c \
$(LOCAL_PATH)/../pipeline.c \
$(LOCAL_PATH)/../polarssl_threadlock.c \
$(LOCAL_PATH)/../polarssl.c \
$(LOCAL_PATH)/../pop3.c \
$(LOCAL_PATH)/../progress.c \
$(LOCAL_PATH)/../rawstr.c \
$(LOCAL_PATH)/../rtsp.c \
$(LOCAL_PATH)/../schannel.c \
$(LOCAL_PATH)/../security.c \
$(LOCAL_PATH)/../select.c \
$(LOCAL_PATH)/../sendf.c \
$(LOCAL_PATH)/../share.c \
$(LOCAL_PATH)/../slist.c \
$(LOCAL_PATH)/../smb.c \
$(LOCAL_PATH)/../smtp.c \
$(LOCAL_PATH)/../socks_gssapi.c \
$(LOCAL_PATH)/../socks_sspi.c \
$(LOCAL_PATH)/../socks.c \
$(LOCAL_PATH)/../speedcheck.c \
$(LOCAL_PATH)/../splay.c \
$(LOCAL_PATH)/../ssh.c \
$(LOCAL_PATH)/../strdup.c \
$(LOCAL_PATH)/../strequal.c \
$(LOCAL_PATH)/../strerror.c \
$(LOCAL_PATH)/../strtok.c \
$(LOCAL_PATH)/../strtoofft.c \
$(LOCAL_PATH)/../telnet.c \
$(LOCAL_PATH)/../tftp.c \
$(LOCAL_PATH)/../timeval.c \
$(LOCAL_PATH)/../transfer.c \
$(LOCAL_PATH)/../url.c \
$(LOCAL_PATH)/../version.c \
$(LOCAL_PATH)/../vtls.c \
$(LOCAL_PATH)/../warnless.c \
$(LOCAL_PATH)/../wildcard.c \
$(LOCAL_PATH)/../x509asn1.c


include $(BUILD_STATIC_LIBRARY)



