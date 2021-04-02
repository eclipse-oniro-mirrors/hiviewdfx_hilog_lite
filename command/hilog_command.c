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

#define APPHILOGCAT_OFF 0

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

char g_logModuleInfo[HILOG_MODULE_MAX_NUM][DOMIAN_ID_LENTH] = {
    "00000",
    "01100", // "NONE"
    "01200", // "NONE"
    "01300", // "NONE"
    "01400", // "NONE"
    "01500", // "NONE"
    "01600", // "NONE"
    "01700", // "NONE"
    "01800", // "NONE"
    "01900", // "NONE"
    "02500", // "NONE"
    "02600", // "NONE"
    "02D00", // "NONE"
};

HiviewConfig g_hiviewConfig = {
    .outputOption = OUTPUT_OPTION_FLOW,
    .level = LOG_DEBUG,
    .logSwitch = HIVIEW_FEATURE_ON,
    .dumpSwitch = HIVIEW_FEATURE_OFF,
    .eventSwitch = HIVIEW_FEATURE_OFF,
};

void HilogHelpProc(const char* tag)
{
    printf("%s [-h] [-l level/mod] [-C level <1>] [-C mod <3>]\n", tag);
    printf(" -h            Help\n");
    printf(" -l            Query the level and module definition information\n");
    printf(" -l level      Query the level definition information\n");
    printf(" -l mod        Query the level definition information\n");
    printf(" -C            Enable all level logs of all modules\n");
    printf(" -C level <id> Set the lowest log level\n");
    printf(" -C mod <id>   Enable the logs of a specified module and disable other modules\n");
    printf(" -C auto       Set log level with predefined macro\n");
    printf(" -f <filename> Enable the logs to a specified file\n");
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

void ListModuleInfo(void)
{
    int i, ret;
    char modInfo[STR_MAX_LEN] = { '\0' };
    printf("======Module Information======\n");
    for (i = 0; i < HILOG_MODULE_MAX_NUM; i++) {
        if (g_logModuleInfo[i][0] == 0) {
            break;
        }
        ret = snprintf_s(modInfo, sizeof(modInfo), sizeof(modInfo) - 1, " %d - %s\n", i, g_logModuleInfo[i]);
        if (ret > 0) {
            modInfo[ret] = '\0';
            printf((const char *)modInfo);
        }
    }
}

void HilogListProc(const char *cmd)
{
    if (cmd == NULL) {
        ListLevelInfo();
        ListModuleInfo();
    } else if (strncmp(cmd, PARA_LEVEL, PARA_LEVEL_LEN) == 0) {
        ListLevelInfo();
    } else if (strncmp(cmd, PARA_MODULE, PARA_MODULE_LEN) == 0) {
        ListModuleInfo();
    }
}

bool SetLogLevel(unsigned char level)
{
    if (level >= LOG_DEBUG && level < HILOG_LEVEL_MAX) {
        g_hiviewConfig.level = level;
        return true;
    }
    return false;
}

void SetLogOutputModule(unsigned char mod)
{
    g_hiviewConfig.logOutputModule = mod;
}

void SetOutputModule(const char *cmd)
{
    char *endPtr = NULL;
    int mod = strtol(cmd, &endPtr, 0);
    SetLogOutputModule((unsigned char)mod);
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

int HilogSetProc(const char *option, const char *attr)
{
    if (option == NULL) {
        SetLogLevel(LOG_DEBUG);
        SetLogOutputModule(HILOG_MODULE_MAX_NUM);
    } else if (strncmp(option, PARA_LEVEL, PARA_LEVEL_LEN) == 0) {
        SetOutputLevel(attr);
    } else if (strncmp(option, PARA_MODULE, PARA_MODULE_LEN) == 0) {
        SetOutputModule(attr);
    } else if (strncmp(option, PARA_AUTO, PARA_AUTO_LEN) == 0) {
#ifdef OHOS_RELEASE
    #if APPHILOGCAT_STATUS_RELEASE == APPHILOGCAT_OFF
        printf("Applogcat Off \n");
        return 0;
    #else
        SetLogLevel(CONFIG_LOG_LEVEL_RELEASE);
        printf("Default log level: %d \n", CONFIG_LOG_LEVEL_RELEASE);
    #endif
#else  // OHOS_DEBUG
    #if APPHILOGCAT_STATUS_DEBUG == APPHILOGCAT_OFF
        printf("Applogcat Off \n");
        return 0;
    #else
        SetLogLevel(CONFIG_LOG_LEVEL_DEBUG);
        printf("Default log level: %d \n", CONFIG_LOG_LEVEL_DEBUG);
    #endif
#endif
    }
    printf("Applogcat On \n");
    return 1;
}

int HilogCmdProc(const char* tag, int argc, const char **argv)
{
#define ARG2 1
#define ARG3 2
#define ARG4 3

    int i = 0;

    if (argv[ARG2][i++] == OPTION_TAG) {
        switch (argv[ARG2][i++]) {
            case OPTION_HELP:
                HilogHelpProc(tag);
                return 0;
            case OPTION_LIST:
                HilogListProc(&argv[ARG3][0]);
                return 0;
            case OPTION_SET:
                return HilogSetProc(&argv[ARG3][0], &argv[ARG4][0]);
            case OPTION_2_FILE:
                return 0;
            default:
                printf("Invalid command.\n");
                return 0;
        }
    }
    printf("Invalid command.\n");
    return 0;
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

bool FilterModuleLog(unsigned char setModule, const char *logModule)
{
    if (setModule == HILOG_MODULE_MAX_NUM) {
        return true;
    }
    int ret = strncmp(logModule, g_logModuleInfo[setModule], DOMAIN_LENGTH);
    // If module = setmodule, may print log
    if (ret == 0) {
        return true;
    }
    return false;
}