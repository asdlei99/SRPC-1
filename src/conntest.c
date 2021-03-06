/*
 * Copyright (c) 2013, Court of the University of Glasgow
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * - Neither the name of the University of Glasgow nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * connection test client
 *
 * usage: ./conntest [-a attempts] [-h host] [-p port] [-s service]
 *
 * attempts to connect to the specified host, port, and service the
 * specified number of times; each time, if successful, issues rpc_disconnect
 *
 * tests for any memory leaks in the runtime associated with connection
 * records
 */

#include "srpc.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define HOST "localhost"
#define PORT 20000
#define SERVICE "Echo"
#define ATTEMPTS 10
#define USAGE "./conntest [-a attempts] [-h host] [-p port] [-s service]"

int main(int argc, char *argv[]) {
    RpcConnection rpc;
    char *host;
    char *service;
    unsigned short port;
    int i, j;
    int attempts;
    struct timespec time_delay = {1, 0};	/* delay one second */

    host = HOST;
    service = SERVICE;
    port = PORT;
    attempts = ATTEMPTS;
    for (i = 1; i < argc; ) {
        if ((j = i + 1) == argc) {
            fprintf(stderr, "usage: %s\n", USAGE);
            exit(1);
        }
        if (strcmp(argv[i], "-h") == 0)
            host = argv[j];
        else if (strcmp(argv[i], "-p") == 0)
            port = atoi(argv[j]);
        else if (strcmp(argv[i], "-s") == 0)
            service = argv[j];
        else if (strcmp(argv[i], "-a") == 0)
            attempts = atoi(argv[j]);
        else {
            fprintf(stderr, "Unknown flag: %s %s\n", argv[i], argv[j]);
        }
        i = j + 1;
    }
    assert(rpc_init(0));
    for (i = 0; i < attempts; i++) {
        rpc = rpc_connect(host, port, service, 0);
        if (rpc != NULL) {
            printf("Success: connect(%s,%05u,%s)\n",
                   host, port, service);
            rpc_disconnect(rpc);
        } else {
            printf("Failure: connect(%s,%05u,%s)\n",
                   host, port, service);
        }
    }
    nanosleep(&time_delay, NULL); /* gives time for harvesting connections
                                    in case running it in valgrind */
    return 0;
}
