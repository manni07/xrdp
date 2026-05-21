/**
 * xrdp: A Remote Desktop Protocol server.
 *
 * Copyright (C) 2026 xrdp contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if defined(HAVE_CONFIG_H)
#include "config_ac.h"
#endif

#include "test_common.h"
#include "xrdp-neutrinordp-config.h"

START_TEST(test_neutrinordp_config_missing_param_keeps_secure_default)
{
    int ignore_certificate = 0;
    int handled;

    handled = neutrinordp_process_config_param("neutrinordp.unknown",
                                               "true",
                                               &ignore_certificate);

    ck_assert_int_eq(handled, 0);
    ck_assert_int_eq(ignore_certificate, 0);
}
END_TEST

START_TEST(test_neutrinordp_config_ignore_certificate_true_is_admin_opt_out)
{
    int ignore_certificate = 0;
    int handled;

    handled = neutrinordp_process_config_param("neutrinordp.ignore_certificate",
                                               "true",
                                               &ignore_certificate);

    ck_assert_int_eq(handled, 1);
    ck_assert_int_eq(ignore_certificate, 1);
}
END_TEST

START_TEST(test_neutrinordp_config_ignore_certificate_false_keeps_validation)
{
    int ignore_certificate = 1;
    int handled;

    handled = neutrinordp_process_config_param("neutrinordp.ignore_certificate",
                                               "false",
                                               &ignore_certificate);

    ck_assert_int_eq(handled, 1);
    ck_assert_int_eq(ignore_certificate, 0);
}
END_TEST

START_TEST(test_neutrinordp_config_unknown_param_is_not_handled)
{
    int ignore_certificate = 1;
    int handled;

    handled = neutrinordp_process_config_param("neutrinordp.unknown",
                                               "false",
                                               &ignore_certificate);

    ck_assert_int_eq(handled, 0);
    ck_assert_int_eq(ignore_certificate, 1);
}
END_TEST

Suite *
make_suite_test_neutrinordp_config(void)
{
    Suite *s;
    TCase *tc_neutrinordp_config;

    s = suite_create("NeutrinoRDP config");

    tc_neutrinordp_config = tcase_create("neutrinordp_config");
    tcase_add_test(tc_neutrinordp_config,
                   test_neutrinordp_config_missing_param_keeps_secure_default);
    tcase_add_test(tc_neutrinordp_config,
                   test_neutrinordp_config_ignore_certificate_true_is_admin_opt_out);
    tcase_add_test(tc_neutrinordp_config,
                   test_neutrinordp_config_ignore_certificate_false_keeps_validation);
    tcase_add_test(tc_neutrinordp_config,
                   test_neutrinordp_config_unknown_param_is_not_handled);

    suite_add_tcase(s, tc_neutrinordp_config);

    return s;
}
