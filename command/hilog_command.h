/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#ifndef HOS_LITE_HIVIEW_COMMAND_H
#define HOS_LITE_HIVIEW_COMMAND_H

#include <stdbool.h>

#include "log.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define CMD_MIN_LEN 2
#define CMD_MAX_LEN 32
#define CMD_HILOGCAT "hilog"
#define CMD_HIEVENT "hievent"
#define OPTION_TAG '-'
#define OPTION_LIST 'l'
#define OPTION_SET 'C'
#define OPTION_SIMULATE 's'
#define OPTION_HELP 'h'
#define OPTION_2_FILE 'f'
#define OPTION_START 't'
#define OPTION_UART 'R'

#define PARA_LEVEL "level"
#define PARA_LEVEL_LEN 5
#define PARA_MODULE "mod"
#define PARA_MODULE_LEN 3
#define PARA_AUTO "auto"
#define PARA_AUTO_LEN 4
#define OP_ASSIGN '='
#define STR_MAX_LEN 128

typedef struct {
    const unsigned char outputOption : 4; /* Control log output mode. Cannot be modified during running. */
    unsigned char hiviewInited : 1;       /* Indicates whether the hiview service is inited. */
    unsigned char level : 3;              /* Control log output level. HILOG_LV_XXX */
    unsigned char logSwitch : 1;          /* Indicates whether to enable the log component. */
    unsigned char eventSwitch : 1;        /* Indicates whether to enable the event component. */
    unsigned char dumpSwitch : 1;         /* Indicates whether to enable the dump component. */
    unsigned char logOutputModule;        /* Control log output module. */
    unsigned short writeFailureCount;
} HiviewConfig;

typedef enum {
    OUTPUT_OPTION_DEBUG = 0, /* Output to the UART without buffer. Commercial versions are forbidden. */
    OUTPUT_OPTION_FLOW,      /* Output to UART via SAMR */
    OUTPUT_OPTION_TEXT_FILE,
    OUTPUT_OPTION_BIN_FILE,
    OUTPUT_OPTION_MAX
} HiviewOutputOption;

#define HIVIEW_FEATURE_ON 1
#define HIVIEW_FEATURE_OFF 0
#define HILOG_MODULE_ALL 0xff

#define HILOG_MODULE_MAX_NUM 50
#define DOMIAN_ID_LENTH 6

extern HiviewConfig g_hiviewConfig;

int HilogCmdProc(const char* tag, int argc, const char **argv);
bool FilterLevelLog(unsigned char setLevel, unsigned char logLevel);
bool FilterModuleLog(unsigned char setModule, const char *logModule);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif /* End of #ifndef HOS_LITE_HIVIEW_COMMAND_H */
