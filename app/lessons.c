/* SPDX-License-Identifier: GPL-3.0-only WITH GPL-3.0-linking-exception */
/* Added 2026-10-24: short public-domain practice melodies.
 * Event pairs are MIDI note (255 = rest), then distance to the next event. */
#include "lessons.h"
#include <string.h>

#define SONG(id, ...) static const uint8_t id[] = { __VA_ARGS__ }
#define E(note, steps) note, steps
enum { R = 255 };

/* Twelve short exercises precede the song library. The course deliberately
 * introduces one new idea at a time before asking for a complete melody. */
SONG(find_c, E(60,4),E(60,4),E(60,4),E(60,4),E(60,4),E(60,4),E(60,4),E(60,4));
SONG(c_d, E(60,4),E(62,4),E(60,4),E(62,4),E(60,4),E(62,4),E(60,4),E(62,4));
SONG(c_d_e, E(60,4),E(62,4),E(64,4),E(62,4),E(60,4),E(62,4),E(64,4),E(60,4));
SONG(five_up, E(60,2),E(62,2),E(64,2),E(65,2),E(67,4),E(65,2),E(64,2),E(62,2),E(60,4));
SONG(five_down, E(67,2),E(65,2),E(64,2),E(62,2),E(60,4),E(62,2),E(64,2),E(65,2),E(67,4));
SONG(finger_walk, E(60,2),E(62,2),E(64,2),E(65,2),E(67,2),E(65,2),E(64,2),E(62,2),E(60,4));
SONG(steady, E(60,2),E(60,2),E(60,2),E(60,2),E(62,2),E(62,2),E(62,2),E(62,2));
SONG(long_short, E(60,4),E(62,2),E(62,2),E(64,4),E(62,2),E(60,2),E(60,4),E(60,4));
SONG(rests, E(60,2),E(R,2),E(60,2),E(R,2),E(62,2),E(R,2),E(64,2),E(R,2),E(60,4));
SONG(step_turn, E(60,2),E(62,2),E(64,2),E(62,2),E(60,4),E(62,2),E(64,2),E(62,2),E(60,4));
SONG(skip_thirds, E(60,2),E(64,2),E(67,4),E(64,2),E(60,4),E(64,2),E(67,2),E(64,2),E(60,4));
SONG(echo_four, E(60,2),E(62,2),E(64,2),E(67,4),E(60,2),E(62,2),E(64,2),E(67,4));

SONG(ode, E(64,2),E(64,2),E(65,2),E(67,2),E(67,2),E(65,2),E(64,2),E(62,2),E(60,2),E(60,2),E(62,2),E(64,2),E(64,3),E(62,1),E(62,4));
SONG(twinkle, E(60,2),E(60,2),E(67,2),E(67,2),E(69,2),E(69,2),E(67,4),E(65,2),E(65,2),E(64,2),E(64,2),E(62,2),E(62,2),E(60,4));
SONG(frere, E(60,2),E(62,2),E(64,2),E(60,2),E(60,2),E(62,2),E(64,2),E(60,2),E(64,2),E(65,2),E(67,4),E(64,2),E(65,2),E(67,4));
SONG(rowboat, E(60,3),E(60,1),E(60,2),E(62,1),E(64,3),E(64,2),E(62,1),E(64,2),E(65,1),E(67,6),E(72,1),E(72,1),E(72,1),E(67,1),E(67,1),E(67,1),E(64,1),E(64,1),E(64,1),E(60,1),E(60,1),E(60,1),E(67,2),E(65,1),E(64,2),E(62,1),E(60,4));
SONG(mary, E(64,2),E(62,2),E(60,2),E(62,2),E(64,2),E(64,2),E(64,4),E(62,2),E(62,2),E(62,4),E(64,2),E(67,2),E(67,4),E(64,2),E(62,2),E(60,2),E(62,2),E(64,2),E(64,2),E(64,2),E(64,2),E(62,2),E(62,2),E(64,2),E(62,2),E(60,4));
SONG(london, E(67,2),E(69,2),E(67,2),E(65,2),E(64,2),E(65,2),E(67,4),E(62,2),E(64,2),E(65,4),E(64,2),E(65,2),E(67,4),E(67,2),E(69,2),E(67,2),E(65,2),E(64,2),E(65,2),E(67,4),E(62,4),E(67,4),E(64,2),E(60,4));
SONG(jingle, E(64,2),E(64,2),E(64,4),E(64,2),E(64,2),E(64,4),E(64,2),E(67,2),E(60,3),E(62,1),E(64,6),E(65,2),E(65,2),E(65,3),E(65,1),E(65,2),E(64,2),E(64,2),E(64,1),E(64,1),E(64,2),E(62,2),E(62,2),E(64,2),E(62,4),E(67,4));
SONG(oldmac, E(60,2),E(60,2),E(60,2),E(67,2),E(69,2),E(69,2),E(67,4),E(64,2),E(64,2),E(62,2),E(62,2),E(60,4));
SONG(birthday, E(67,1),E(67,1),E(69,2),E(67,2),E(72,2),E(71,4),E(67,1),E(67,1),E(69,2),E(67,2),E(74,2),E(72,4),E(67,1),E(67,1),E(79,2),E(76,2),E(72,2),E(71,2),E(69,4),E(77,1),E(77,1),E(76,2),E(72,2),E(74,2),E(72,4));
SONG(saints, E(60,2),E(64,2),E(65,2),E(67,6),E(60,2),E(64,2),E(65,2),E(67,6),E(60,2),E(64,2),E(65,2),E(67,3),E(64,3),E(60,3),E(64,3),E(62,6));
SONG(yankee, E(60,2),E(60,2),E(62,2),E(64,2),E(60,2),E(64,2),E(62,4),E(60,2),E(60,2),E(62,2),E(64,2),E(60,4),E(59,4),E(60,2),E(60,2),E(62,2),E(64,2),E(65,2),E(64,2),E(62,2),E(60,2),E(59,2),E(67,2),E(60,4));
SONG(auclair, E(60,2),E(60,2),E(60,2),E(62,2),E(64,4),E(62,4),E(60,2),E(64,2),E(62,2),E(62,2),E(60,6));
SONG(lightly, E(64,4),E(60,2),E(60,4),E(62,2),E(59,2),E(59,4),E(57,2),E(59,2),E(60,2),E(62,2),E(64,2),E(64,2),E(64,4));
SONG(amazing, E(67,2),E(60,4),E(64,2),E(60,2),E(64,4),E(62,2),E(60,4),E(57,2),E(67,4),E(60,4),E(64,2),E(60,2),E(64,4),E(62,6));
SONG(lullaby, E(64,1),E(64,1),E(67,4),E(64,1),E(64,1),E(67,4),E(64,2),E(67,2),E(72,2),E(71,2),E(69,2),E(69,2),E(67,4),E(62,2),E(64,2),E(65,2),E(62,2),E(62,2),E(64,2),E(65,4));
SONG(beethoven5, E(67,1),E(67,1),E(67,1),E(63,5),E(65,1),E(65,1),E(65,1),E(62,5),E(67,1),E(67,1),E(67,1),E(63,3),E(68,1),E(68,1),E(68,1),E(67,3));
SONG(einekleine, E(67,2),E(74,2),E(67,2),E(74,2),E(67,2),E(74,2),E(67,2),E(71,2),E(74,4),E(72,2),E(69,2),E(72,2),E(76,2),E(69,4));
SONG(bluedanube, E(60,2),E(64,2),E(67,4),E(67,2),E(71,2),E(74,4),E(74,2),E(76,2),E(72,4),E(72,2),E(69,2),E(65,4),E(65,2),E(69,2),E(72,4));
SONG(cancan, E(67,1),E(67,1),E(67,1),E(60,1),E(64,1),E(67,1),E(72,1),E(72,1),E(72,1),E(71,1),E(69,1),E(65,1),E(67,1),E(67,1),E(67,1),E(60,1),E(64,1),E(67,1),E(72,1),E(69,1),E(67,1),E(64,1),E(60,3));
SONG(williamtell, E(60,1),E(60,1),E(60,2),E(60,1),E(60,1),E(60,2),E(60,1),E(64,1),E(65,1),E(67,2),E(60,1),E(64,1),E(65,1),E(67,2),E(67,1),E(67,1),E(67,1),E(67,1),E(65,1),E(64,1),E(62,1),E(60,3));
SONG(greensleeves, E(57,2),E(60,4),E(62,2),E(64,3),E(65,1),E(64,2),E(62,4),E(59,2),E(55,3),E(57,1),E(59,2),E(60,4),E(57,2),E(57,3),E(56,1),E(57,4));
SONG(scarborough, E(62,3),E(62,1),E(69,4),E(69,2),E(64,2),E(65,2),E(64,2),E(62,4),E(69,2),E(72,2),E(74,4),E(72,2),E(69,2),E(71,2),E(67,4));
SONG(auldlang, E(60,2),E(65,3),E(65,1),E(65,2),E(69,2),E(67,3),E(65,1),E(67,2),E(69,2),E(65,2),E(65,2),E(69,2),E(72,4),E(74,2));
SONG(ohsusanna, E(60,1),E(62,1),E(64,2),E(67,2),E(67,2),E(69,2),E(67,2),E(64,2),E(60,2),E(62,2),E(64,2),E(64,2),E(62,2),E(60,2),E(62,4));
SONG(camptown, E(67,2),E(67,2),E(64,2),E(67,2),E(69,2),E(67,2),E(64,4),E(64,2),E(62,2),E(64,2),E(62,4),E(67,2),E(67,2),E(64,2),E(67,2),E(69,2),E(67,2),E(64,4));
SONG(thisoldman, E(67,2),E(64,2),E(67,4),E(67,2),E(64,2),E(67,4),E(69,2),E(67,2),E(65,2),E(64,2),E(62,2),E(64,2),E(65,2),E(64,2),E(65,2),E(67,2),E(60,2),E(60,2),E(60,4));
SONG(skiptomylou, E(60,2),E(64,2),E(67,2),E(67,2),E(69,2),E(67,2),E(64,4),E(60,2),E(64,2),E(67,2),E(69,2),E(67,4),E(64,4));
SONG(popweasel, E(60,2),E(60,1),E(62,1),E(62,2),E(64,2),E(67,2),E(64,2),E(60,2),E(60,1),E(62,1),E(62,2),E(64,4),E(60,2),E(60,1),E(62,1),E(62,2),E(64,2),E(67,2),E(64,2),E(60,4),E(69,2),E(62,2),E(65,2),E(64,2),E(60,4));
SONG(jollygood, E(67,2),E(67,2),E(69,2),E(67,2),E(72,2),E(71,4),E(67,2),E(67,2),E(69,2),E(67,2),E(74,2),E(72,4),E(67,2),E(67,2),E(79,2),E(76,2),E(72,2),E(71,2),E(69,4));
SONG(sailor, E(62,2),E(62,2),E(62,2),E(62,2),E(62,2),E(62,2),E(62,2),E(65,2),E(69,2),E(69,2),E(69,2),E(69,2),E(67,2),E(65,2),E(67,2),E(62,4));

struct lesson { const char *name; const uint8_t *events; uint8_t bytes, tempo; };
#define L(name, data, bpm) {name, data, sizeof data, bpm}
static const struct lesson lessons[GROOVE_LESSON_COUNT] = {
    L("FIND MIDDLE C",find_c,72), L("C AND D",c_d,76), L("C D E",c_d_e,80),
    L("FIVE UP",five_up,80), L("FIVE DOWN",five_down,80), L("FINGER WALK",finger_walk,84),
    L("STEADY BEAT",steady,72), L("LONG SHORT",long_short,76), L("PLAY THE REST",rests,72),
    L("STEP AND TURN",step_turn,84), L("SKIP THIRDS",skip_thirds,80), L("FOUR NOTE ECHO",echo_four,84),
    L("ODE TO JOY",ode,96), L("TWINKLE",twinkle,96), L("FRERE JACQ",frere,100),
    L("ROW BOAT",rowboat,108), L("MARY LAMB",mary,100), L("LONDON BRDG",london,100),
    L("OLD MACDONALD",oldmac,104), L("THIS OLD MAN",thisoldman,104), L("SKIP TO LOU",skiptomylou,112), L("LIGHTLY ROW",lightly,96),
    L("THE SAINTS",saints,110), L("YANKEE DOODLE",yankee,112), L("AU CLAIR",auclair,92), L("AMAZING GRCE",amazing,72),
    L("GREENSLEEVES",greensleeves,84), L("SCARBOROUGH",scarborough,84), L("AULD LANG",auldlang,88), L("OH SUSANNA",ohsusanna,104),
    L("CAMPTOWN",camptown,112), L("DRUNK SAILOR",sailor,112),
    L("BEETHOVEN 5",beethoven5,104), L("EINE KLEINE",einekleine,120), L("BLUE DANUBE",bluedanube,92),
    L("CAN CAN",cancan,132), L("WILLIAM TELL",williamtell,132),
    L("JINGLE BELLS",jingle,112), L("HAPPY BDAY",birthday,96), L("BRAHMS LULL",lullaby,72),
    L("POP WEASEL",popweasel,112), L("JOLLY GOOD",jollygood,104)
};

static const uint8_t chapter_first[GROOVE_CHAPTER_COUNT + 1] = {0,3,6,9,12,22,32,37,42};
static const char *const chapter_names[GROOVE_CHAPTER_COUNT] = {
    "FIND THE KEYS", "FIVE FINGERS", "RHYTHM", "STEPS AND SKIPS",
    "EASY SONGS", "FOLK SONGS", "CLASSICS", "CHALLENGE"
};
static const char *const chapter_tips[GROOVE_CHAPTER_COUNT] = {
    "FIND C BY TWO BLACK KEYS", "ONE FINGER ON EACH KEY", "KEEP A STEADY PULSE", "HEAR STEPS AND JUMPS",
    "LEARN ONE PHRASE AT A TIME", "LISTEN THEN PLAY IT BACK", "START SLOW AND STAY RELAXED", "LIGHTS FADE AS YOU IMPROVE"
};

const char *groove_chapter_name(unsigned chapter)
{ return chapter < GROOVE_CHAPTER_COUNT ? chapter_names[chapter] : 0; }
const char *groove_chapter_tip(unsigned chapter)
{ return chapter < GROOVE_CHAPTER_COUNT ? chapter_tips[chapter] : 0; }
unsigned groove_chapter_first(unsigned chapter)
{ return chapter < GROOVE_CHAPTER_COUNT ? chapter_first[chapter] : GROOVE_LESSON_COUNT; }
unsigned groove_chapter_count(unsigned chapter)
{ return chapter < GROOVE_CHAPTER_COUNT ? chapter_first[chapter + 1] - chapter_first[chapter] : 0; }

const char *groove_lesson_name(unsigned index)
{ return index < GROOVE_LESSON_COUNT ? lessons[index].name : NULL; }

int groove_lesson_build(unsigned index, unsigned track, struct groove_project *p)
{
    if (!p || index >= GROOVE_LESSON_COUNT || track >= GROOVE_TRACKS || groove_pad_count(track))
        return GROOVE_PROJECT_INVALID;
    const struct lesson *lesson = &lessons[index];
    memset(p->step[track], 0, sizeof p->step[track]);
    unsigned position = 0;
    for (unsigned i = 0; i < lesson->bytes; i += 2) {
        unsigned note = lesson->events[i], distance = lesson->events[i + 1];
        if (!distance || position >= GROOVE_STEPS || distance > GROOVE_STEPS - position)
            return GROOVE_PROJECT_INVALID;
        if (note != R) {
            struct groove_step *cell = &p->step[track][position];
            cell->note = (uint8_t)note; cell->velocity = 100; cell->drums = 15U << 5;
        }
        position += distance;
    }
    unsigned length = position <= 16 ? 16 : position <= 32 ? 32 : position <= 48 ? 48 : 64;
    p->preset[track].sequence_pages = (uint8_t)(length == 64 ? 0 : length / 16);
    p->tempo = lesson->tempo;
    memset(p->name, ' ', sizeof p->name);
    unsigned n = (unsigned)strlen(lesson->name); if (n > sizeof p->name) n = sizeof p->name;
    memcpy(p->name, lesson->name, n);
    return groove_project_valid(p) ? 0 : GROOVE_PROJECT_INVALID;
}

int groove_lesson_load(unsigned index, unsigned track)
{
    struct groove_project *project = groove_project_scratch();
    int r = groove_project_capture(project);
    if (!r) {
        /* A lesson is a clean one-player arrangement. Keep every timbre and
         * FM algorithm, but silence old patterns that would obscure the demo. */
        memset(project->step, 0, sizeof project->step);
        project->mute &= (uint8_t)~(1U << track);
        project->solo = 0;
        r = groove_lesson_build(index, track, project);
    }
    if (r) return r;
    groove_sequence_keep_transport(0);
    r = groove_project_apply_edit(project, 0xb0000U + index * GROOVE_TRACKS + track);
    if (!r) {
        groove_sequence_keep_transport(1);
        groove_project_touch();
    }
    return r;
}
