# Top-level makefile for building OAIC project

all:

# documentation
.PHONY: docs
docs-sources :=					\
	docs/source/index.rst			\
	docs/source/oran_installation.rst	\
	docs/source/quickstart.rst		\

docs: docs/build/index.html

docs/build/index.html: docs/source/conf.py ${docs-sources}
	sphinx-build -b html docs/source/ docs/build/html

all: docs

clean-docs:
	rm -rf docs/build

clean: clean-docs

