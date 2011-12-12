/*
 * Slurp - a web crawler
 * Copyright (C) 2011 Joseph Max DeLiso

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDebug>

#include <event2/thread.h>
#include <event2/event.h>

#include "globals.h"
#include "util.h"

namespace slurp {

    void initLibraries() {
        /* insert some macro magic here for a windows compile */
        if (evthread_use_pthreads()) {
            die("error: could not initialize libevent with pthreads",
                EXIT_FAILURE);
        }

        if (curl_global_init(CURL_GLOBAL_ALL)) {
            die("error: could not initialize libcurl", EXIT_FAILURE);
        }
    }

    int validateArgs(int argc, char **argv, char **env,
                     QQueue < QString > &seedURIs, int &quota,
                     int &maxThreads) {
        int i;
        int flags = 0;

        /* TODO: QT probably has something nicer to handle this */

        for (i = 1; i < argc; i++) {
            if (argv[i][0] == '-') {
                switch (argv[i][1]) {
                case 'V':
                    qDebug() << "slurp v"
                        << VERSION_ID[0] << "."
                        << VERSION_ID[1] << "." << VERSION_ID[2]
                        << "\nusing libevent " << event_get_version()
                        << "\nusing " << curl_version()
                        << "\nusing QT " << qVersion() << "\n";

                    die(LICENSE_INFO, EXIT_SUCCESS);
                    break;

                case 'h':
                    die(HELP_MENU, EXIT_SUCCESS);
                    break;

                case 'v':
                    flags |= FLAGS_VERBOSE;
                    break;

                case 'n':
                    if (strlen(argv[i] + 2)) {
                        quota = atoi((argv[i] + 2));
                    } else if (i + 1 < argc) {
                        quota = atoi(argv[i + 1]);
                        i++;
                    } else {
                        die("error: could not find numeric portion of -n option", EXIT_FAILURE);
                    }

                    break;

                case 't':
                    if (strlen(argv[i] + 2)) {
                        maxThreads = atoi((argv[i] + 2));
                    } else if (i + 1 < argc) {
                        maxThreads = atoi(argv[i + 1]);
                        i++;
                    } else {
                        die("error: could not find numeric portion of -t option", EXIT_FAILURE);
                    }

                    if (maxThreads <= 0) {
                        die("error: t must be greater than zero", EXIT_FAILURE);
                    }

                    break;

                default:
                    qDebug() << "warning: unrecognized option: " << argv[i]
                        << "\n";

                    break;
                }
            } else {
                seedURIs.enqueue(argv[i]);
            }
        }

        for (i = 0; env[i]; i++) {
            /* search for relevant environment variables */
        }

        return flags;
    }

    void die(const char *errmsg, int errcode) {
        qFatal(errmsg);
        exit(errcode);
    }

    void curlVerify(const char *where, CURLMcode code) {
        if (CURLM_OK != code) {
            const char *s;

            switch (code) {
            case CURLM_CALL_MULTI_PERFORM:
                s = "CURLM_CALL_MULTI_PERFORM";
                break;
            case CURLM_BAD_HANDLE:
                s = "CURLM_BAD_HANDLE";
                break;
            case CURLM_BAD_EASY_HANDLE:
                s = "CURLM_BAD_EASY_HANDLE";
                break;
            case CURLM_OUT_OF_MEMORY:
                s = "CURLM_OUT_OF_MEMORY";
                break;
            case CURLM_INTERNAL_ERROR:
                s = "CURLM_INTERNAL_ERROR";
                break;
            case CURLM_UNKNOWN_OPTION:
                s = "CURLM_UNKNOWN_OPTION";
                break;
            case CURLM_LAST:
                s = "CURLM_LAST";
                break;
            default:
                s = "CURLM_unknown";
                break;
            case CURLM_BAD_SOCKET:
                s = "CURLM_BAD_SOCKET";
                qDebug() << "debug: " << where << " returns " << s;
                return;
            }

            qDebug() << "debug: " << where << " returns " << s;
            exit(code);
        }
    }

    void libeventVerify(const char *where, int code) {
        if (code == -1) {
            qFatal(where);
            exit(code);
        }
    }

}                               /* namespace slurp */
