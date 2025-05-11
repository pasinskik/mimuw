/**
 * @file leader.c
 * @brief Implementation of leader becoming and resigning logic.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

#include "leader.h"
#include "state.h"
#include "socket_utils.h"
#include "message.h"

/**
 * This function handles a received LEADER message.
 * If sync_level == 0: become leader.
 * If sync_level == 255 and currently leader: step down.
 * Other values or invalid transitions are ignored.
 *
 * @param buf Buffer containing the received message.
 * @param len Length of the received message.
 */
void leader_handle(const uint8_t *buf, size_t len) {
    uint8_t sync_level;
    if (!msg_parse_leader(buf, len, &sync_level)) {
        print_invalid_message(buf, len);
        return;
    }

    if (sync_level == 0) {
        state.level = 0;
        struct timeval tv; gettimeofday(&tv, NULL);
        // printf("Became leader at level=0, will start sync in 2000ms");
        usleep(2000000);
    } else if (sync_level == 255 && state.level == 0) {
        state.level = 255;
        // printf("INFO: Stepping down as leader, level set to 255");
    } else {
        print_invalid_message(buf, len);
    }
}

/**
 * This function desynchronizes from a leader after not hearing from them in 20 seconds.
 */
void ditch_leader(void) {
    state.sync_source = NULL;
    state.level = 255;
    state.offset = 0;
}
