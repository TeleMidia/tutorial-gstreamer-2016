TARGETS= minicurso.pdf minicurso_bio.pdf

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	latexmk -C

.SUFFIXES: .pdf .tex
.tex.pdf:
	latexmk -pdf $<
