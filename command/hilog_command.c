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

#include "hilog_command.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define OPTION_TAG '-'
#define OPTION_LIST 'l'
#define OPTION_SET 'C'
#define OPTION_HELP 'h'

#define PARA_LEVEL "level"
#define PARA_LEVEL_LEN 5
#define PARA_MODULE "mod"
#define PARA_MODULE_LEN 3
#define PARA_AUTO "auto"
#define PARA_AUTO_LEN 4

#define APPHILOGCAT_OFF 0

#define HIVIEW_FEATURE_ON 1
#define HIVIEW_FEATURE_OFF 0

#define ARG2 1
#define ARG3 2
#define ARG4 3
#define ARG5 4
#define ARG6 5

#define NUMBER_ARG_2 2
#define NUMBER_ARG_3 3
#define NUMBER_ARG_4 4
#define NUMBER_ARG_6 6

#define HILOG_MODULE_MAX "FFFFF"

char g_logLevelInfo[HILOG_LEVEL_MAX] = {
    'N', // "NONE"
    'N', // "NONE"
    'N', // "NONE"
    'D', // "DEBUG"
    'I', // "INFO"
    'W', // "WARN"
    'E', // "ERROR"
    'F'  // "FATAL"
};

HiviewConfig g_hiviewConfig = {
    .outputOption = OUTPUT_OPTION_FLOW,
    .level = LOG_DEBUG,
    .logSwitch = HIVIEW_FEATURE_ON,
    .dumpSwitch = HIVIEW_FEATURE_OFF,
    .eventSwitch = HIVIEW_FEATURE_OFF,
    .logOutputModule = HILOG_MODULE_MAX,
};

void HilogHelpProc(const char* tag)
{
    printf("%s [-h] [-l level/mod] [-C level <1>] [-C mod <3>]\n", tag);
    printf(" -h                         Help\n");
    printf(" -l                         Query the level definition information\n");
    printf(" -C                         Enable all level logs of all modules\n");
    printf(" -C auto                    Set log level with predefined macro\n");
    printf(" -C level <id>              Set the lowest log level\n");
    printf(" -C mod <id>                Enable the logs of a specified module and disable other modules\n");
    printf(" -C mod <mid> level <lid>   Set the lowest log level and enable specified module\n");
    printf(" -C level <lid> mod <mid>   Set the lowest log level and enable specified module\n");
}

void ListLevelInfo(void)
{
    printf("======Level Information======\n");
    printf(" 3 - DEBUG\n");
    printf(" 4 - INFO\n");
    printf(" 5 - WARN\n");
    printf(" 6 - ERROR\n");
    printf(" 7 - FATAL\n");
}

void HilogListProc(int argc, const char **argv)
{
    if (argc == 2) {
        ListLevelInfo();
    } else {
        printf("Invalid command.\n");
    }
}

bool SetLogLevel(unsigned char level)
{
    if (level >= LOG_DEBUG && level < HILOG_LEVEL_MAX) {
        g_hiviewConfig.level = level;
        printf("Set log level : %d \n", level);
        return true;
    }
    return false;
}

void SetOutputModule(const char *mod)
{
    if ((mod == NULL) || (strlen(mod) != (DOMAIN_ID_LENGTH - 1))) {
        printf("Invalid command.\n");
        return;
    }
    strncpy_s(g_hiviewConfig.logOutputModule, mod, DOMAIN_ID_LENGTH);
}

void SetOutputLevel(const char *cmd)
{
    char *endPtr = NULL;
    int level;
    if (cmd != NULL) {
        level = strtol(cmd, &endPtr, 0);
    } else {
        level = LOG_DEBUG;
    }
    if (SetLogLevel((unsigned char)level) == true) {
        printf("Set the log output level success.\n");
        return;
    }
    printf("Set the log output level failure level=%d.\n", level);
}

int ProcAutoSet(void)
{
#ifdef OHOS_RELEASE
#if APPHILOGCAT_STATUS_RELEASE == APPHILOGCAT_OFF
    return 0;
#else
    SetLogLevel(CONFIG_LOG_LEVEL_RELEASE);
    printf("Default log level: %d \n", CONFIG_LOG_LEVEL_RELEASE);
#endif
#else  // OHOS_DEBUG
#if APPHILOGCAT_STATUS_DEBUG == APPHILOGCAT_OFF
    return 0;
#else
    SetLogLevel(CONFIG_LOG_LEVEL_DEBUG);
    printf("Default log level: %d \n", CONFIG_LOG_LEVEL_DEBUG);
#endif
#endif
    return 1;
}

int HilogSetProc(int argc, const char **argv)
{
    switch (argc) {
        case NUMBER_ARG_2: // XXhilogcat -C
            SetLogLevel(LOG_DEBUG);
            SetOutputModule(HILOG_MODULE_MAX);
            return 1;
        case NUMBER_ARG_3: // XXhilogcat -C auto
            if (strncmp(&argv[ARG3][0], PARA_AUTO, PARA_AUTO_LEN) == 0) {
                return ProcAutoSet();
            }
            break;
        case NUMBER_ARG_6: // XXhilogcat -C mod <mid> level <lid> or XXhilogcat -C level <lid> mod <mid>
            if (strncmp(&argv[ARG5][0], PARA_LEVEL, PARA_LEVEL_LEN) == 0) {
                SetOutputLevel(&argv[ARG6][0]);
            } else if (strncmp(&argv[ARG5][0], PARA_MODULE, PARA_MODULE_LEN) == 0) {
                SetOutputModule(&argv[ARG6][0]);
            } else {
                printf("Invalid command.\n");
                return 0;
            }
            // fall through
        case NUMBER_ARG_4: // XXhilogcat -C level <id> or XXhilogcat -C mod <id>
            if (strncmp(&argv[ARG3][0], PARA_LEVEL, PARA_LEVEL_LEN) == 0) {
                SetOutputLevel(&argv[ARG4][0]);
            } else if (strncmp(&argv[ARG3][0], PARA_MODULE, PARA_MODULE_LEN) == 0) {
                SetOutputModule(&argv[ARG4][0]);
            } else {
                printf("Invalid command.\n");
                return 0;
            }
            return 1;
        default:
            break;
    }
    printf("Invalid command.\n");
    return 0;
}

int HilogCmdProc(const char* tag, int argc, const char **argv)
{
    if (argc <= 1) {
        printf("Invalid command.\n");
        return 0;
    }

    int i = 0;
    int ret = 0;
    if (argv[ARG2][i++] == OPTION_TAG) {
        switch (argv[ARG2][i++]) {
            case OPTION_HELP:
                HilogHelpProc(tag);
                break;
            case OPTION_LIST:
                HilogListProc(argc, argv);
                break;
            case OPTION_SET:
                ret = HilogSetProc(argc, argv);
                break;
            default:
                printf("Invalid command.\n");
                break;
        }
    } else {
        printf("Invalid command.\n");
    }
    return ret;
}

bool FilterLevelLog(unsigned char setLevel, unsigned char logLevel)
{
    int logMinLevel;

    for (logMinLevel = LOG_DEBUG; logMinLevel < HILOG_LEVEL_MAX; logMinLevel++) {
        if (logLevel == g_logLevelInfo[logMinLevel]) {
            break;
        }
    }
    // Loglevel >= set level, may print log
    if (logMinLevel >= setLevel) {
        return true;
    }
    return false;
}

bool FilterModuleLog(unsigned char *setModule, const char *logModule)
{
    if (strncmp(setModule, HILOG_MODULE_MAX, DOMAIN_ID_LENGTH) == 0) {
        return true;
    }
    return strncmp(logModule, g_hiviewConfig.logOutputModule, DOMAIN_ID_LENGTH) == 0;
}