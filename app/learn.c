/* SPDX-License-Identifier: GPL-3.0-only WITH GPL-3.0-linking-exception */
/* Added 2026-10-24: non-destructive guided practice over an FM1 project track. */
#include "instrument.h"
#include "learn.h"
#include <string.h>

static struct groove_learn_view lesson;

static int active_note(unsigned step, uint8_t *note)
{
    struct groove_step cell = {0};
    groove_sequence_read(lesson.track, step, &cell, 1);
    if (!cell.velocity)
        return 0;
    *note = cell.note;
    return 1;
}

static int seek(unsigned first)
{
    for (unsigned step = first; step <= lesson.last; ++step) {
        uint8_t note;
        if (active_note(step, &note)) {
            lesson.step = (uint8_t)step;
            lesson.expected_note = note;
            return 1;
        }
    }
    lesson.complete = 1;
    return 0;
}

int groove_learn_begin(unsigned track, unsigned first, unsigned last)
{
    memset(&lesson, 0, sizeof lesson);
    if (track >= GROOVE_TRACKS || groove_pad_count(track))
        return GROOVE_PROJECT_INVALID;
    unsigned length = groove_sequence_length(track);
    if (!length)
        return GROOVE_PROJECT_INVALID;
    if (first >= length) first = length - 1U;
    if (last >= length) last = length - 1U;
    if (last < first) last = first;
    lesson.active = 1;
    lesson.track = (uint8_t)track;
    lesson.first = (uint8_t)first;
    lesson.last = (uint8_t)last;
    for (unsigned step = first; step <= last; ++step) {
        uint8_t note;
        if (active_note(step, &note) && lesson.total != 255)
            ++lesson.total;
    }
    if (!lesson.total) {
        memset(&lesson, 0, sizeof lesson);
        return GROOVE_PROJECT_EMPTY;
    }
    (void)seek(first);
    return 0;
}

void groove_learn_reset(void)
{
    if (!lesson.active)
        return;
    lesson.complete = lesson.correct = lesson.mistakes = 0;
    (void)seek(lesson.first);
}

enum groove_learn_result groove_learn_note(unsigned note)
{
    if (!lesson.active || lesson.complete || note > 127)
        return GROOVE_LEARN_IGNORED;
    if (note != lesson.expected_note) {
        if (lesson.mistakes != 255) ++lesson.mistakes;
        return GROOVE_LEARN_WRONG;
    }
    if (lesson.correct != 255) ++lesson.correct;
    if (!seek((unsigned)lesson.step + 1U))
        return GROOVE_LEARN_COMPLETE;
    return GROOVE_LEARN_CORRECT;
}

void groove_learn_get(struct groove_learn_view *view)
{
    if (view) *view = lesson;
}
