#!/bin/sh
set -eu
cc -std=c11 -Wall -Wextra -Werror -DFM1_HOST_TEST \
  -Iapp -Iapp/synth -Isdk/include \
  tests/learn_test.c app/learn.c -o /tmp/fm1-learn-test
/tmp/fm1-learn-test
cc -std=c11 -Wall -Wextra -Werror -DFM1_HOST_TEST \
  -Iapp -Iapp/synth -Isdk/include \
  tests/lessons_test.c app/lessons.c -o /tmp/fm1-lessons-test
/tmp/fm1-lessons-test
node tests/midi_project_test.mjs
