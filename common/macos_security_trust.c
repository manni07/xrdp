/**
 * xrdp: A Remote Desktop Protocol server.
 *
 * Copyright (C) 2026 all xrdp contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#if defined(HAVE_CONFIG_H)
#include <config_ac.h>
#endif

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if defined(XRDP_MACOS_SECURITY)
#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>
#endif

#include "base64.h"
#include "macos_security_trust.h"
#include "os_calls.h"

static void
set_error(char *error, unsigned int error_len, const char *format, ...)
{
    va_list args;

    if (error == 0 || error_len == 0)
    {
        return;
    }

    va_start(args, format);
    vsnprintf(error, error_len, format, args);
    va_end(args);
}

int
macos_security_trust_available(void)
{
#if defined(XRDP_MACOS_SECURITY)
    return 1;
#else
    return 0;
#endif
}

#if defined(XRDP_MACOS_SECURITY)
static void
copy_cf_error(CFErrorRef cf_error, char *error, unsigned int error_len)
{
    CFStringRef description;
    char text[512];

    if (cf_error == 0)
    {
        set_error(error, error_len, "certificate trust evaluation failed");
        return;
    }

    description = CFErrorCopyDescription(cf_error);
    if (description == 0)
    {
        set_error(error, error_len, "certificate trust evaluation failed");
        return;
    }

    if (CFStringGetCString(description, text, sizeof(text),
                           kCFStringEncodingUTF8))
    {
        set_error(error, error_len, "%s", text);
    }
    else
    {
        set_error(error, error_len, "certificate trust evaluation failed");
    }

    CFRelease(description);
}
#endif

enum macos_security_trust_result
macos_security_evaluate_der_cert_chain(const char *hostname,
                                        const void *der_chain,
                                        int der_chain_len,
                                        char *error,
                                        unsigned int error_len)
{
#if defined(XRDP_MACOS_SECURITY)
    CFDataRef cert_data = 0;
    SecCertificateRef cert = 0;
    CFArrayRef certs = 0;
    CFStringRef cf_hostname = 0;
    SecPolicyRef policy = 0;
    SecTrustRef trust = 0;
    CFErrorRef cf_error = 0;
    enum macos_security_trust_result result = MACOS_SECURITY_TRUST_INVALID;
    OSStatus status;

    if (der_chain == 0 || der_chain_len <= 0)
    {
        set_error(error, error_len, "empty certificate data");
        return MACOS_SECURITY_TRUST_INVALID;
    }

    cert_data = CFDataCreate(kCFAllocatorDefault, der_chain, der_chain_len);
    if (cert_data == 0)
    {
        set_error(error, error_len, "failed to create certificate data");
        goto cleanup;
    }

    cert = SecCertificateCreateWithData(kCFAllocatorDefault, cert_data);
    if (cert == 0)
    {
        set_error(error, error_len, "failed to parse DER certificate");
        goto cleanup;
    }

    certs = CFArrayCreate(kCFAllocatorDefault, (const void **)&cert, 1,
                          &kCFTypeArrayCallBacks);
    if (certs == 0)
    {
        set_error(error, error_len, "failed to create certificate chain");
        goto cleanup;
    }

    if (hostname != 0 && hostname[0] != '\0')
    {
        cf_hostname = CFStringCreateWithCString(kCFAllocatorDefault, hostname,
                                                kCFStringEncodingUTF8);
        if (cf_hostname == 0)
        {
            set_error(error, error_len, "failed to create certificate hostname");
            goto cleanup;
        }
        policy = SecPolicyCreateSSL(true, cf_hostname);
    }
    else
    {
        policy = SecPolicyCreateBasicX509();
    }

    if (policy == 0)
    {
        set_error(error, error_len, "failed to create certificate policy");
        goto cleanup;
    }

    status = SecTrustCreateWithCertificates(certs, policy, &trust);
    if (status != errSecSuccess || trust == 0)
    {
        set_error(error, error_len, "failed to create trust evaluation");
        goto cleanup;
    }

    if (SecTrustEvaluateWithError(trust, &cf_error))
    {
        set_error(error, error_len, "");
        result = MACOS_SECURITY_TRUST_TRUSTED;
    }
    else
    {
        copy_cf_error(cf_error, error, error_len);
        result = MACOS_SECURITY_TRUST_UNTRUSTED;
    }

cleanup:
    if (cf_error != 0)
    {
        CFRelease(cf_error);
    }
    if (trust != 0)
    {
        CFRelease(trust);
    }
    if (policy != 0)
    {
        CFRelease(policy);
    }
    if (cf_hostname != 0)
    {
        CFRelease(cf_hostname);
    }
    if (certs != 0)
    {
        CFRelease(certs);
    }
    if (cert != 0)
    {
        CFRelease(cert);
    }
    if (cert_data != 0)
    {
        CFRelease(cert_data);
    }

    return result;
#else
    (void)hostname;
    (void)der_chain;
    (void)der_chain_len;
    set_error(error, error_len, "macOS Security.framework is not available");
    return MACOS_SECURITY_TRUST_NOT_AVAILABLE;
#endif
}

enum macos_security_trust_result
macos_security_diagnose_pem_certificate(const char *cert_path,
                                        char *error,
                                        unsigned int error_len)
{
#if defined(XRDP_MACOS_SECURITY)
    static const char begin_marker[] = "-----BEGIN CERTIFICATE-----";
    static const char end_marker[] = "-----END CERTIFICATE-----";
    int fd;
    int file_size;
    int read_bytes;
    char *file_data = 0;
    char *begin;
    char *end;
    char *base64_data = 0;
    char *der_data = 0;
    char *src;
    char *dst;
    size_t der_len = 0;
    enum macos_security_trust_result result = MACOS_SECURITY_TRUST_INVALID;

#define IS_BASE64_SPACE(_c) \
    ((_c) == '\r' || (_c) == '\n' || (_c) == '\t' || (_c) == ' ')

    if (cert_path == 0 || cert_path[0] == '\0')
    {
        set_error(error, error_len, "empty certificate path");
        return MACOS_SECURITY_TRUST_INVALID;
    }

    file_size = g_file_get_size(cert_path);
    if (file_size <= 0)
    {
        set_error(error, error_len, "certificate file is empty or unreadable");
        return MACOS_SECURITY_TRUST_INVALID;
    }

    fd = g_file_open_ro(cert_path);
    if (fd < 0)
    {
        set_error(error, error_len, "certificate file is not readable");
        return MACOS_SECURITY_TRUST_INVALID;
    }

    file_data = (char *)g_malloc(file_size + 1, 0);
    read_bytes = g_file_read(fd, file_data, file_size);
    g_file_close(fd);
    if (read_bytes != file_size)
    {
        set_error(error, error_len, "failed to read certificate file");
        goto cleanup;
    }
    file_data[file_size] = '\0';

    begin = strstr(file_data, begin_marker);
    if (begin == 0)
    {
        set_error(error, error_len, "PEM certificate begin marker not found");
        goto cleanup;
    }
    begin += sizeof(begin_marker) - 1;
    end = strstr(begin, end_marker);
    if (end == 0 || end <= begin)
    {
        set_error(error, error_len, "PEM certificate end marker not found");
        goto cleanup;
    }

    base64_data = (char *)g_malloc((end - begin) + 1, 0);
    dst = base64_data;
    for (src = begin; src < end; ++src)
    {
        if (!IS_BASE64_SPACE(*src))
        {
            *dst++ = *src;
        }
    }
    *dst = '\0';

    der_data = (char *)g_malloc(dst - base64_data, 0);
    if (base64_decode(base64_data, der_data, dst - base64_data, &der_len) != 0
            || der_len > (size_t)(dst - base64_data))
    {
        set_error(error, error_len, "failed to decode PEM certificate");
        goto cleanup;
    }

    result = macos_security_evaluate_der_cert_chain(0, der_data,
             (int)der_len, error, error_len);

cleanup:
    g_free(der_data);
    g_free(base64_data);
    g_free(file_data);
    return result;
#undef IS_BASE64_SPACE
#else
    (void)cert_path;
    set_error(error, error_len, "macOS Security.framework is not available");
    return MACOS_SECURITY_TRUST_NOT_AVAILABLE;
#endif
}
