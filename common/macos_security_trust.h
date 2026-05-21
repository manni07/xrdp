/**
 * xrdp: A Remote Desktop Protocol server.
 *
 * Copyright (C) 2026 all xrdp contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#if !defined(MACOS_SECURITY_TRUST_H)
#define MACOS_SECURITY_TRUST_H

enum macos_security_trust_result
{
    MACOS_SECURITY_TRUST_NOT_AVAILABLE = 0,
    MACOS_SECURITY_TRUST_TRUSTED,
    MACOS_SECURITY_TRUST_UNTRUSTED,
    MACOS_SECURITY_TRUST_INVALID
};

int
macos_security_trust_available(void);

enum macos_security_trust_result
macos_security_evaluate_der_cert_chain(const char *hostname,
                                        const void *der_chain,
                                        int der_chain_len,
                                        char *error,
                                        unsigned int error_len);

enum macos_security_trust_result
macos_security_diagnose_pem_certificate(const char *cert_path,
                                        char *error,
                                        unsigned int error_len);

#endif /* MACOS_SECURITY_TRUST_H */
