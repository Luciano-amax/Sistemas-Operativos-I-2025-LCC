ERL_SOURCES := $(shell find src -name "*.erl")
BEAM_FILES := $(patsubst src/%.erl, %.beam, $(ERL_SOURCES))

all:
	@for file in $(ERL_SOURCES); do \
		echo "Compiling $$file..."; \
		erlc -o . $$file || exit 1; \
	done

run:
	erl -noshell -pa . -s main start

clean:
	rm -f *.beam
