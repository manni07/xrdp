/**
 * xrdp: A Remote Desktop Protocol server.
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

#ifndef XRDP_NEUTRINORDP_CONFIG_H
#define XRDP_NEUTRINORDP_CONFIG_H

#define XRDP_NEUTRINORDP_IGNORE_CERTIFICATE_PARAM \
    "neutrinordp.ignore_certificate"

int g_strcmp(const char *c1, const char *c2);
int g_text2bool(const char *s);

static inline int
neutrinordp_process_config_param(const char *name, const char *value,
                                 int *ignore_certificate)
{
    int handled = 0;

    if (g_strcmp(name, XRDP_NEUTRINORDP_IGNORE_CERTIFICATE_PARAM) == 0)
    {
        *ignore_certificate = g_text2bool(value);
        handled = 1;
    }

    return handled;
}

#endif /* XRDP_NEUTRINORDP_CONFIG_H */
