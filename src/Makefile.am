## Process this file with automake to produce Makefile.in

PLAYERSOURCES = \
  block.cpp \
  pass.cpp \
  dribble.cpp\
  markOn.cpp \
  throughpass1.cpp \
  bhv_predictor.cpp\
  shoot.cpp \
	dfs_block.cpp \
	dfs_mark.cpp \
	offs_pass.cpp \
	defensive_action.cpp \
	offensive_action.cpp \
	bhv_basic_move.cpp \
	bhv_basic_offensive_kick.cpp \
	bhv_basic_tackle.cpp \
	bhv_danger_area_tackle.cpp \
	bhv_go_to_static_ball.cpp \
	bhv_goalie_basic_move.cpp \
	bhv_goalie_chase_ball.cpp \
	bhv_goalie_free_kick.cpp \
	bhv_pre_process.cpp \
	bhv_prepare_set_play_kick.cpp \
	bhv_set_play.cpp \
	bhv_set_play_free_kick.cpp \
	bhv_set_play_goal_kick.cpp \
	bhv_set_play_indirect_free_kick.cpp \
	bhv_set_play_kick_in.cpp \
	bhv_set_play_kick_off.cpp \
	bhv_their_goal_kick_move.cpp \
	bhv_penalty_kick.cpp \
	body_kick_to_corner.cpp \
	neck_goalie_turn_neck.cpp \
	role_center_back.cpp \
	role_center_forward.cpp \
	role_defensive_half.cpp \
	role_goalie.cpp \
	role_offensive_half.cpp \
	role_sample.cpp \
	role_side_back.cpp \
	role_side_forward.cpp \
	sample_player.cpp \
	soccer_role.cpp \
	strategy.cpp \
	main_player.cpp

PLAYERHEADERS = \
  block.h \
  pass.h \
  dribble.h\
  markOn.h \
  throughpass1.h \
  bhv_predictor.h \
  shoot.h \
	dfs_block.h \
	dfs_mark.h \
	offs_pass.h \
	defensive_action.h \
	offensive_action.h \
	bhv_basic_move.h \
	bhv_basic_offensive_kick.h \
	bhv_basic_tackle.h \
	bhv_danger_area_tackle.h \
	bhv_go_to_static_ball.h \
	bhv_goalie_basic_move.h \
	bhv_goalie_chase_ball.h \
	bhv_goalie_free_kick.h \
	bhv_pre_process.h \
	bhv_prepare_set_play_kick.h \
	bhv_set_play.h \
	bhv_set_play_free_kick.h \
	bhv_set_play_goal_kick.h \
	bhv_set_play_indirect_free_kick.h \
	bhv_set_play_kick_in.h \
	bhv_set_play_kick_off.h \
	bhv_their_goal_kick_move.h \
	bhv_penalty_kick.h \
	body_kick_to_corner.h \
	neck_goalie_turn_neck.h \
	role_center_back.h \
	role_center_forward.h \
	role_defensive_half.h \
	role_goalie.h \
	role_offensive_half.h \
	role_sample.h \
	role_side_back.h \
	role_side_forward.h \
	sample_player.h \
	soccer_role.h \
	strategy.h

COACHSOURCES = \
	sample_coach.cpp \
	main_coach.cpp

COACHHEADERS = \
	sample_coach.h

TRAINERSOURCES = \
	sample_trainer.cpp \
	main_trainer.cpp

TRAINERHEADERS = \
	sample_trainer.h


noinst_PROGRAMS = sample_player sample_coach sample_trainer

noinst_DATA = \
	reconnect.sh.in \
	start.sh.in \
	train.sh.in \
	coach.conf \
	player.conf \
	team_logo.xpm \
	formations-dt/before-kick-off.conf \
	formations-dt/goal-kick-opp.conf \
	formations-dt/goal-kick-our.conf \
	formations-dt/goalie-catch-opp.conf \
	formations-dt/goalie-catch-our.conf \
	formations-dt/goalie-formation.conf \
	formations-dt/indirect-freekick-opp-formation.conf \
	formations-dt/indirect-freekick-our-formation.conf \
	formations-dt/kickin-our-formation.conf \
	formations-dt/normal-formation.conf \
	formations-dt/setplay-opp-formation.conf \
	formations-dt/setplay-our-formation.conf \
	formations-uva/before-kick-off.conf \
	formations-uva/goal-kick-opp.conf \
	formations-uva/goal-kick-our.conf \
	formations-uva/goalie-catch-opp.conf \
	formations-uva/goalie-catch-our.conf \
	formations-uva/goalie-formation.conf \
	formations-uva/indirect-freekick-opp-formation.conf \
	formations-uva/indirect-freekick-our-formation.conf \
	formations-uva/kickin-our-formation.conf \
	formations-uva/normal-formation.conf \
	formations-uva/setplay-opp-formation.conf \
	formations-uva/setplay-our-formation.conf

sample_player_SOURCES = \
	$(PLAYERSOURCES)

sample_player_LDFLAGS =

sample_player_LDADD =

sample_coach_SOURCES = \
	$(COACHSOURCES)

sample_coach_LDFLAGS =

sample_coach_LDADD =

sample_trainer_SOURCES = \
	$(TRAINERSOURCES)

sample_trainer_LDFLAGS =

sample_trainer_LDADD =

noinst_HEADERS = \
	$(PLAYERHEADERS) \
	$(COACHHEADERS) \
	$(TRAINERHEADERS)

AM_CPPFLAGS =
AM_CFLAGS = -Wall
AM_CXXFLAGS = -Wall
AM_LDLAGS =

CLEANFILES = core *~

DISTCLEANFILES = start.sh train.sh

EXTRA_DIST = $(noinst_DATA) \
	start-debug.sh \
	start-offline.sh
