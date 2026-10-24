/* SPDX-License-Identifier: GPL-3.0-only */
#include "instrument.h"
#include "learn.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static struct groove_step pattern[GROOVE_TRACKS][GROOVE_STEPS];
static unsigned lengths[GROOVE_TRACKS];

void groove_sequence_read(unsigned track, unsigned first, struct groove_step *cells, unsigned count)
{ memcpy(cells, pattern[track] + first, count * sizeof *cells); }
unsigned groove_sequence_length(unsigned track) { return lengths[track]; }
unsigned groove_pad_count(unsigned track) { return track == 7 ? 8U : 0U; }

static struct groove_learn_view view(void)
{ struct groove_learn_view value; groove_learn_get(&value); return value; }

int main(void)
{
    lengths[0] = 16;
    pattern[0][2].note = 60; pattern[0][2].velocity = 100;
    pattern[0][5].note = 62; pattern[0][5].velocity = 90;
    pattern[0][9].note = 64; pattern[0][9].velocity = 80;

    assert(groove_learn_begin(0, 0, 15) == 0);
    assert(view().total == 3 && view().step == 2 && view().expected_note == 60);
    assert(groove_learn_note(61) == GROOVE_LEARN_WRONG);
    assert(view().mistakes == 1 && view().step == 2);
    assert(groove_learn_note(60) == GROOVE_LEARN_CORRECT);
    assert(view().correct == 1 && view().step == 5 && view().expected_note == 62);
    assert(groove_learn_note(62) == GROOVE_LEARN_CORRECT);
    assert(groove_learn_note(64) == GROOVE_LEARN_COMPLETE);
    assert(view().complete && view().correct == 3);
    assert(groove_learn_note(64) == GROOVE_LEARN_IGNORED);

    groove_learn_reset();
    assert(!view().complete && view().correct == 0 && view().mistakes == 0 && view().step == 2);
    assert(groove_learn_begin(7, 0, 15) == GROOVE_PROJECT_INVALID);
    assert(groove_learn_begin(1, 0, 15) == GROOVE_PROJECT_INVALID);
    puts("learn_test: ok");
    return 0;
}
