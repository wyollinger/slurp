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
#include <QWebPage>

#include "util.h"
#include "globals.h"

namespace slurp {

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
                        << "\nusing QT " << qVersion() << "\n";
                       /* << "\nusing QWebKit version " << qWebKitVersion() << "\n"; 
                        *  TODO: find out why qWebKitVersion is undefined
                        */
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
            /* TODO: search for relevant environment variables */
        }

        return flags;
    }

    void die(const char *errmsg, int errcode) {
        qFatal(errmsg);
        exit(errcode);
    }

}                               /* namespace slurp */
