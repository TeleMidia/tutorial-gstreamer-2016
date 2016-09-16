TARGETS= bio.pdf proposal.pdf chapter.pdf

.PHONY: all
all: $(TARGETS)

.PHONY: clean
clean:
	-latexmk -C
	-rm -f *.bbl

.SUFFIXES: .pdf .tex
.tex.pdf:
	latexmk -pdf -shell-escape $<
