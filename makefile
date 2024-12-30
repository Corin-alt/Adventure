#
# CONFIGURATION GENERALE
#

EXEC = editor/editor game/server game/client
OBJETS =  data_structure/map_path.o game/game_int.o elements/map.o elements/hero.o utils/include.o utils/fenetre.o elements/entity.o editor/interface.o utils/ncurses_utils.o utils/sockets_utils.o utils/threads_utils.o utils/files_utils.o utils/other_utils.o
NOM_PROJET = projet_601_604

#
# SUFFIXES
#

.SUFFIXES: .c .o

#
# OBJETS
#

EXEC_O = $(EXEC:=.o)
OBJETS_O = $(OBJETS) $(EXEC_O)

#
# ARGUMENTS ET COMPILATEUR
#

CC = gcc
CCFLAGS_STD = -Wall -O3 -Werror
CCFLAGS_DEBUG = -D _DEBUG_
CCFLAGS = $(CCFLAGS_STD)
CCLIBS = -lncursesw -pthread

#
# REGLES
#

all: msg $(OBJETS) $(EXEC_O)
	@echo "Creation des executables..."
	@for i in $(EXEC); do \
	$(CC) -o $$i $$i.o $(OBJETS) $(CCLIBS); \
	done
	@echo "Termine."

msg:
	@echo "Creation des objets..."

debug: CCFLAGS = $(CCFLAGS_STD) $(CCFLAGS_DEBUG)
debug: all

#
# REGLES PAR DEFAUT
#

.c.o: .h
	@cd $(dir $<) && ${CC} ${CCFLAGS} -c $(notdir $<) -o $(notdir $@)

#
# REGLES GENERALES
#

clean:
	@echo "Suppresion des objets, des fichiers temporaires..."
	@rm -f $(OBJETS) $(EXEC_O)
	@rm -f *~ *#
	@rm -f $(EXEC)
	@rm -f dependances
	@echo "Termine."

depend:
	@echo "Creation des dependances..."
	@sed -e "/^# DEPENDANCES/,$$ d" makefile > dependances
	@echo "# DEPENDANCES" >> dependances
	@for i in $(OBJETS_O); do \
	$(CC) -MM -MT $$i $(CCFLAGS) `echo $$i | sed "s/\(.*\)\\.o$$/\1.c/"` >> dependances; \
	done
	@cat dependances > makefile
	@rm dependances
	@echo "Termine."

#
# CREATION ARCHIVE
#

ARCHIVE_FILES = *

archive: clean
	@echo "Creation de l'archive $(NOM_PROJET)$(shell date '+%y%m%d.tar.gz')..."
	@REP=`basename "$$PWD"`; cd .. && tar zcf $(NOM_PROJET)$(shell date '+%y%m%d.tar.gz') $(addprefix "$$REP"/,$(ARCHIVE_FILES))
	@echo "Termine."

# DEPENDANCES
data_structure/map_path.o: data_structure/map_path.c \
 data_structure/map_path.h data_structure/../elements/map.h \
 data_structure/../elements/map.h \
 data_structure/../elements/../elements/entity.h \
 data_structure/../elements/../elements/../utils/fenetre.h \
 data_structure/../elements/../utils/macros.h \
 data_structure/../utils/macros.h data_structure/../utils/other_utils.h
game/game_int.o: game/game_int.c game/../utils/macros.h \
 game/../utils/ncurses_utils.h game/../elements/map.h \
 game/../elements/map.h game/../elements/../elements/entity.h \
 game/../elements/../elements/../utils/fenetre.h \
 game/../elements/../utils/macros.h game/../elements/entity.h \
 game/game_int.h game/../utils/fenetre.h game/../elements/hero.h
elements/map.o: elements/map.c elements/../utils/ncurses_utils.h \
 elements/../utils/files_utils.h elements/../utils/../elements/map.h \
 elements/../utils/../elements/map.h \
 elements/../utils/../elements/../elements/entity.h \
 elements/../utils/../elements/../elements/../utils/fenetre.h \
 elements/../utils/../elements/../utils/macros.h \
 elements/../utils/../elements/entity.h \
 elements/../utils/../elements/hero.h elements/../utils/include.h \
 elements/../utils/other_utils.h elements/../utils/threads_utils.h \
 elements/../utils/macros.h elements/../elements/entity.h elements/map.h
elements/hero.o: elements/hero.c elements/../utils/macros.h \
 elements/../utils/other_utils.h elements/hero.h elements/map.h \
 elements/../elements/entity.h elements/../elements/../utils/fenetre.h
utils/include.o: utils/include.c utils/other_utils.h utils/include.h
utils/fenetre.o: utils/fenetre.c utils/fenetre.h
elements/entity.o: elements/entity.c elements/../utils/macros.h \
 elements/../utils/fenetre.h elements/../utils/other_utils.h \
 elements/entity.h
editor/interface.o: editor/interface.c editor/../utils/macros.h \
 editor/../utils/ncurses_utils.h editor/../elements/map.h \
 editor/../elements/map.h editor/../elements/../elements/entity.h \
 editor/../elements/../elements/../utils/fenetre.h \
 editor/../elements/../utils/macros.h editor/../elements/entity.h \
 editor/interface.h editor/../utils/fenetre.h
utils/ncurses_utils.o: utils/ncurses_utils.c utils/ncurses_utils.h \
 utils/../elements/entity.h utils/../elements/../utils/fenetre.h \
 utils/../elements/map.h utils/../elements/map.h \
 utils/../elements/../elements/entity.h \
 utils/../elements/../utils/macros.h
utils/sockets_utils.o: utils/sockets_utils.c utils/sockets_utils.h \
 utils/ncurses_utils.h
utils/threads_utils.o: utils/threads_utils.c utils/threads_utils.h \
 utils/ncurses_utils.h
utils/files_utils.o: utils/files_utils.c utils/macros.h \
 utils/../elements/map.h utils/../elements/map.h \
 utils/../elements/../elements/entity.h \
 utils/../elements/../elements/../utils/fenetre.h \
 utils/../elements/../utils/macros.h utils/../elements/entity.h \
 utils/../elements/hero.h utils/other_utils.h utils/ncurses_utils.h \
 utils/include.h
utils/other_utils.o: utils/other_utils.c utils/macros.h \
 utils/other_utils.h utils/ncurses_utils.h
editor/editor.o: editor/editor.c editor/interface.h \
 editor/../utils/fenetre.h editor/../elements/map.h \
 editor/../elements/map.h editor/../elements/../elements/entity.h \
 editor/../elements/../elements/../utils/fenetre.h \
 editor/../elements/../utils/macros.h editor/../utils/macros.h \
 editor/../utils/ncurses_utils.h
game/server.o: game/server.c game/../data_structure/map_path.h \
 game/../data_structure/../elements/map.h \
 game/../data_structure/../elements/map.h \
 game/../data_structure/../elements/../elements/entity.h \
 game/../data_structure/../elements/../elements/../utils/fenetre.h \
 game/../data_structure/../elements/../utils/macros.h \
 game/../utils/sockets_utils.h game/../utils/files_utils.h \
 game/../utils/../elements/map.h game/../utils/../elements/entity.h \
 game/../utils/../elements/hero.h game/../utils/../elements/map.h \
 game/../utils/../elements/../utils/macros.h game/../utils/include.h \
 game/../utils/other_utils.h game/../utils/threads_utils.h \
 game/../utils/ncurses_utils.h game/../utils/include.h
game/client.o: game/client.c game/../utils/ncurses_utils.h \
 game/game_int.h game/../utils/fenetre.h game/../elements/map.h \
 game/../elements/map.h game/../elements/../elements/entity.h \
 game/../elements/../elements/../utils/fenetre.h \
 game/../elements/../utils/macros.h game/../elements/hero.h \
 game/../utils/threads_utils.h game/../utils/sockets_utils.h \
 game/../utils/files_utils.h game/../utils/../elements/map.h \
 game/../utils/../elements/entity.h game/../utils/../elements/hero.h \
 game/../utils/include.h game/../utils/other_utils.h \
 game/../utils/include.h
