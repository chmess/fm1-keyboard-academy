/* SPDX-License-Identifier: GPL-3.0-only WITH GPL-3.0-linking-exception */
#ifndef FM1_LEARN_H
#define FM1_LEARN_H

#include <stdint.h>

enum groove_learn_result {
    GROOVE_LEARN_IGNORED,
    GROOVE_LEARN_WRONG,
    GROOVE_LEARN_CORRECT,
    GROOVE_LEARN_COMPLETE
};

struct groove_learn_view {
    uint8_t active, complete, track, first, last, step, expected_note;
    uint8_t correct, mistakes, total;
};

/* The lesson reads the selected melodic sequencer track without changing it.
 * first/last are inclusive and are clamped to the track's current loop. */
int groove_learn_begin(unsigned track, unsigned first, unsigned last);
void groove_learn_reset(void);
enum groove_learn_result groove_learn_note(unsigned note);
void groove_learn_get(struct groove_learn_view *view);

#endif
