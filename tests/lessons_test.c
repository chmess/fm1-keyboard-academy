/* SPDX-License-Identifier: GPL-3.0-only */
#include "lessons.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

unsigned groove_pad_count(unsigned track) { return track == 7 ? 8U : 0U; }
int groove_project_valid(const struct groove_project *p)
{
    return p && p->tempo >= 30 && p->tempo <= 300 && p->sequence.version == 4;
}
struct groove_project *groove_project_scratch(void) { return NULL; }
int groove_project_capture(struct groove_project *p) { (void)p; return -1; }
void groove_sequence_keep_transport(unsigned restore) { (void)restore; }
int groove_project_apply_edit(const struct groove_project *p, unsigned op) { (void)p; (void)op; return -1; }
void groove_project_touch(void) {}

int main(void)
{
    for (unsigned i = 0; i < GROOVE_LESSON_COUNT; ++i) {
        struct groove_project project;
        memset(&project, 0x5a, sizeof project);
        project.sequence.version = 4;
        struct groove_preset before = project.preset[0];
        assert(groove_lesson_name(i));
        assert(groove_lesson_build(i, 0, &project) == 0);
        assert(!memcmp(&before, &project.preset[0], 221));
        unsigned notes = 0;
        for (unsigned step = 0; step < GROOVE_STEPS; ++step) notes += project.step[0][step].velocity != 0;
        assert(notes >= 4);
    }
    assert(!groove_lesson_name(GROOVE_LESSON_COUNT));
    unsigned total = 0;
    for (unsigned chapter = 0; chapter < GROOVE_CHAPTER_COUNT; ++chapter) {
        assert(groove_chapter_name(chapter));
        assert(groove_chapter_tip(chapter));
        assert(groove_chapter_first(chapter) == total);
        assert(groove_chapter_count(chapter));
        total += groove_chapter_count(chapter);
    }
    assert(total == GROOVE_LESSON_COUNT);
    puts("lessons_test: ok");
    return 0;
}
