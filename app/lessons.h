/* SPDX-License-Identifier: GPL-3.0-only WITH GPL-3.0-linking-exception */
#ifndef FM1_LESSONS_H
#define FM1_LESSONS_H

#include "instrument.h"

enum { GROOVE_CHAPTER_COUNT = 8, GROOVE_LESSON_COUNT = 42 };
const char *groove_chapter_name(unsigned chapter);
const char *groove_chapter_tip(unsigned chapter);
unsigned groove_chapter_first(unsigned chapter);
unsigned groove_chapter_count(unsigned chapter);
const char *groove_lesson_name(unsigned index);
int groove_lesson_build(unsigned index, unsigned track, struct groove_project *project);
int groove_lesson_load(unsigned index, unsigned track);

#endif
