/**
 * xrdp: A Remote Desktop Protocol server.
 *
 * Copyright (C) 2026 all xrdp contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#if defined(HAVE_CONFIG_H)
#include "config_ac.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "macos_security_trust.h"
#include "test_common.h"

static const char g_self_signed_pem[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDCTCCAfGgAwIBAgIUEtei9gakiqsZnaHXrWRJNDlL0hcwDQYJKoZIhvcN"
    "AQELBQAwFDESMBAGA1UEAwwJbG9jYWxob3N0MB4XDTI2MDUyMDIwMDcxNFoX"
    "DTI2MDUyMTIwMDcxNFowFDESMBAGA1UEAwwJbG9jYWxob3N0MIIBIjANBgkq"
    "hkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyW3RYsTmSuCzHpqtS58q5NcnYeR"
    "itZ1KA4dvNeDP+LlL0Db4xpbAnVGuTDFpdVxtdP34VwvcqTTkEhvB/qlY4/"
    "LUflWiTxRMkVlwPx4wH7Z0NvOtKm9nZbiFH9FjkWAwuG++CQbpaP8VXRox2"
    "NKyfGrHG5MRSxywKlA+WUkokYX9AtkGy9jRaCGkhTN8cfqEZlYpSEWqLCyT"
    "xamOm279tFGmX2hGZxh//RJFI69n2w1V3epIAGs+OzIloiraVURse0i1Byl"
    "8S1RlmP4qIe8GZvroCfbAlWW1XoT2FXfUWaGMqQxfBN7KAhYcWYwwsJ7jxF"
    "bQFT5vSt6e8sznEvLySQIDAQABo1MwUTAdBgNVHQ4EFgQUEHH73PKXyaqS"
    "iikUrIXk8v786wcwHwYDVR0jBBgwFoAUEHH73PKXyaqSiikUrIXk8v786w"
    "cwDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAWE3aFzG3"
    "eMMonzC5K8tP/J/Sizim/ckJ40wOF/McFJ0iatra6RqYIA9k7YJDllz893S"
    "wy7PtzFrmMMZTfR4rNuTX+Mh12vFTYzdtyKWRrVrUyQf1MVFz1Vs+vg+zr"
    "pit7AXvBurQbJEqrQVaxgEQRSyp1twJ+oo2PhCeEuCdgY3y6CTxHJIgqFp"
    "3tBFHLbgeFAJB+bpLdZD4+qDQBI0YI8sAKgpTduUEq2tQBbzSNi6f65F6A"
    "QjVIL2Kru6mI86Ovn+fw0hokqe2Lz+Tki4vtAq7vhA416DkXse21V4tT7N"
    "49da5tQ9Kpl74onAfMTYocWY7vcQak4RdFWX0LAybkg==\n"
    "-----END CERTIFICATE-----\n";

static int
write_temp_pem(char *path, unsigned int path_len)
{
    int fd;
    FILE *fp;

    snprintf(path, path_len, "/tmp/xrdp-test-cert-XXXXXX");
    fd = mkstemp(path);
    if (fd < 0)
    {
        return 1;
    }

    fp = fdopen(fd, "w");
    if (fp == NULL)
    {
        close(fd);
        unlink(path);
        return 1;
    }

    fputs(g_self_signed_pem, fp);
    fclose(fp);
    return 0;
}

START_TEST(test_macos_security_trust_reports_platform_availability)
{
#if defined(__APPLE__) && defined(XRDP_MACOS_SECURITY)
    ck_assert_int_eq(macos_security_trust_available(), 1);
#else
    ck_assert_int_eq(macos_security_trust_available(), 0);
#endif
}
END_TEST

START_TEST(test_macos_security_trust_rejects_malformed_der)
{
    char error[256] = { 0 };
    const char der[] = { 0x01, 0x02, 0x03 };
    enum macos_security_trust_result result;

    result = macos_security_evaluate_der_cert_chain("localhost",
             der, sizeof(der), error, sizeof(error));

#if defined(__APPLE__) && defined(XRDP_MACOS_SECURITY)
    ck_assert_int_eq(result, MACOS_SECURITY_TRUST_INVALID);
    ck_assert_str_ne(error, "");
#else
    ck_assert_int_eq(result, MACOS_SECURITY_TRUST_NOT_AVAILABLE);
#endif
}
END_TEST

START_TEST(test_macos_security_trust_diagnoses_self_signed_pem_without_failing_tls)
{
    char path[64];
    char error[256] = { 0 };
    enum macos_security_trust_result result;

    ck_assert_int_eq(write_temp_pem(path, sizeof(path)), 0);
    result = macos_security_diagnose_pem_certificate(path, error, sizeof(error));
    unlink(path);

#if defined(__APPLE__) && defined(XRDP_MACOS_SECURITY)
    ck_assert_int_eq(result, MACOS_SECURITY_TRUST_UNTRUSTED);
    ck_assert_str_ne(error, "");
#else
    ck_assert_int_eq(result, MACOS_SECURITY_TRUST_NOT_AVAILABLE);
#endif
}
END_TEST

Suite *
make_suite_test_macos_security_trust(void)
{
    Suite *s;
    TCase *tc;

    s = suite_create("macos_security_trust");
    tc = tcase_create("macos_security_trust");

    tcase_add_test(tc, test_macos_security_trust_reports_platform_availability);
    tcase_add_test(tc, test_macos_security_trust_rejects_malformed_der);
    tcase_add_test(tc, test_macos_security_trust_diagnoses_self_signed_pem_without_failing_tls);

    suite_add_tcase(s, tc);
    return s;
}
