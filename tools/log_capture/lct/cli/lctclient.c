/*
 * Copyright (C) Intel 2014 - 2015
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <lctclient.h>
#include <getopt.h>
#include <stdio.h>
#include <libgen.h>

/*internal dnt tests only*/
static void lct_macrotest() {
    lct_log(LCT_EV_INFO, "lctapp", "ev_p4", 0);
    lct_log(LCT_EV_INFO, "lctapp", "ev_p5", 0, "event Data0");
    lct_log(LCT_EV_INFO, "lctapp", "ev_p6", 0, "event Data0", "event Data1");
    lct_log(LCT_EV_INFO, "lctapp", "ev_p7", 0, "event Data0", "event Data1",
            "event Data2");
    lct_log(LCT_EV_INFO, "lctapp", "ev_p8", 0, "event Data0", "event Data1",
            "event Data2", "event Data3");
    lct_log(LCT_EV_INFO, "lctapp", "ev_p9", 0, "event Data0", "event Data1",
            "event Data2", "event Data3", "event Data4");
    lct_log(LCT_EV_INFO, "lctapp", "ev_p10", 0, "event Data0", "event Data1",
            "event Data2", "event Data3", "event Data4", "event Data5");
    lct_log(LCT_EV_INFO, "lctapp", "ev_p11", 0, "event Data0", "event Data1",
            "event Data2", "event Data3", "event Data4",
            "event Data5", "/init.rc");
    lct_log(LCT_EV_INFO, "lctapp", "ev_p12", 0, "event Data0", "event Data1",
            "event Data2", "event Data3", "event Data4",
            "event Data5", "/init.rc", LCT_ADDITIONAL_APLOG);

}

/* The options*/
static struct option long_options[] = {
    {"type", required_argument, 0, 't'},
    {"submitter", required_argument, 0, 's'},
    {"event", required_argument, 0, 'e'},
    {"flags", required_argument, 0, 'f'},
    {"data0", required_argument, 0, '0'},
    {"data1", required_argument, 0, '1'},
    {"data2", required_argument, 0, '2'},
    {"data3", required_argument, 0, '3'},
    {"data4", required_argument, 0, '4'},
    {"data5", required_argument, 0, '5'},
    {"file_list", required_argument, 0, 'F'},
    {"add_steps", required_argument, 0, 'a'},
    {"macro", no_argument, 0, 'm'},
    {0, 0, 0, 0}
};

/*The descriptions*/
const char *option_desc[] = {
    "Event type, s - stat, i - info, e - error, default s",
    "Submitter name",
    "Event name",
    "1 for low priority, default 0",
    "Data0 field",
    "Data1 field",
    "Data2 field",
    "Data3 field",
    "Data4 field",
    "Data5 field",
    "A ; separated list of files to attach",
    "Bitmask of additional steps to perform, possible values "
        "\n\t 1 - attach aplogs"
        "\n\t 2 - attach kernel log" "\n\t 4 - attach scu fw logs",
    NULL
};

void print_help(const char *argv0) {
    struct option *opt;
    const char **opt_dsc;
    char *base = basename(argv0);

    printf("Usage: %s", (!base) ? ((!argv0) ? "lctclient" : argv0) : base);
    opt = long_options;
    while (opt->name) {
        printf(" [-%c ", (char)(opt->val & 0xff));
        if (opt->has_arg == required_argument)
            printf("value");
        printf("]");
        opt++;
    }
    opt = long_options;
    opt_dsc = option_desc;
    while (opt->name && *opt_dsc) {
        printf("\n-%c; --%s \t%s", (char)(opt->val & 0xff), opt->name, *opt_dsc);
        opt++;
        opt_dsc++;
    }

    printf("\n");
}

int main(int __attribute((unused)) argc, char **__attribute((unused)) argv) {
    int c;

    uint32_t type = LCT_EV_STAT;
    uint32_t add_steps = 0;
    uint32_t flags = 0;

    char *data[6] = { NULL, };

    char *submiter = "lctclient";
    char *event_name = "";
    char *file_list = NULL;

    while (1) {
        int option_index = 0;
        c = getopt_long(argc, argv, "t:s:e:f:0:1:2:3:4:5:F:a:m",
                        long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
            break;

        switch (c) {
        case 't':
            switch (optarg[0]) {
            case 'S':
            case 's':
                type = LCT_EV_STAT;
                break;
            case 'I':
            case 'i':
                type = LCT_EV_INFO;
                break;
            case 'E':
            case 'e':
                type = LCT_EV_ERROR;
                break;
            }
            break;
        case 's':
            submiter = optarg;
            break;
        case 'e':
            event_name = optarg;
            break;
        case 'f':
            sscanf(optarg, "%10u", &flags);
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
            data[c - '0'] = optarg;
            break;
        case 'F':
            file_list = optarg;
            break;
        case 'a':
            sscanf(optarg, "%10u", &add_steps);
            break;
        case 'm':
            lct_macrotest();
            return 0;
        default:
            print_help(argv[0]);
            break;

        }
    }
    lct_log(type, submiter, event_name, flags, data[0], data[1], data[2],
            data[3], data[4], data[5], file_list, add_steps);
    return 0;
}
