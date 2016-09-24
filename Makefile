TARGETS= chapter.pdf

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	-latexmk -C
	-rm -f *.bbl

.SUFFIXES: .pdf .tex
.tex.pdf:
	latexmk -pdf -shell-escape $<

chapter.tex: chapter-*.tex

# Words that have to be wrapped in \en{...}.
words=
words:= always
words:= $(words)|always\s+pads?
words:= $(words)|bins?
words:= $(words)|buffers?
words:= $(words)|bus
words:= $(words)|caches?
words:= $(words)|callbacks?
words:= $(words)|dataflow
words:= $(words)|downstream
words:= $(words)|end-of-stream
words:= $(words)|equalizers?
words:= $(words)|events?
words:= $(words)|fast-forward
words:= $(words)|flags?
words:= $(words)|flags?
words:= $(words)|flush
words:= $(words)|frameworks?
words:= $(words)|linker
words:= $(words)|linkers?
words:= $(words)|makefiles?
words:= $(words)|mutex
words:= $(words)|mutexes
words:= $(words)|null
words:= $(words)|pads?
words:= $(words)|paused
words:= $(words)|pipelines?
words:= $(words)|pixels?
words:= $(words)|players?
words:= $(words)|playing
words:= $(words)|plugins?
words:= $(words)|pulse-code\s+modulation
words:= $(words)|quality\s+of\s+service
words:= $(words)|raw
words:= $(words)|ready
words:= $(words)|red-green-blue
words:= $(words)|request
words:= $(words)|request\s+pads?
words:= $(words)|rewind
words:= $(words)|seek
words:= $(words)|shell
words:= $(words)|sink\s+pads?
words:= $(words)|sinks?
words:= $(words)|sometimes
words:= $(words)|sometimes\s+pads?
words:= $(words)|source\s+pads?
words:= $(words)|sources?
words:= $(words)|start
words:= $(words)|stop
words:= $(words)|streaming\s+threads?
words:= $(words)|string
words:= $(words)|threads?
words:= $(words)|upstream
words:= $(words)|user\s+data
words:= $(words)|userdata
.PHONY: check
check:
	./sc.pl --sc-local='/(\s\b($(words))\b\s)/english' chapter.tex
