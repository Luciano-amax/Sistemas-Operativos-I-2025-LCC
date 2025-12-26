PORT ?= 12345
SRC_DIR = ./src/
INCLUDE_DIR = ./include
SRC = $(wildcard $(SRC_DIR)*.erl)
MAKEFLAGS += --no-print-directory
BEAM_DIR = ./beams/

.PHONY: compile clear run

run: compile
	@ erl -noshell -pa $(BEAM_DIR) -eval "init_srv:start_up($(PORT)), halt()."

# Crea un directorio para los *.beams y
# compila todos los archivos *.erl dentro
# de ./src/ dentro de esta carpeta.
compile:
	@ mkdir -p $(BEAM_DIR) ;\
		erlc -I $(INCLUDE_DIR) -o $(BEAM_DIR) $(SRC)

# Limpia los directorios de beams y dumps.
clean:
	@ rm -f $(BEAM_DIR)*.beam
	@ rm -f *.dump

# Limpia, compila y corre el script run.sh
rebuild: clear run
