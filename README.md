# Open AI Cellular

Core software for Open AI Cellular

## Installation and Quick Start Guide

The full documentation is contained in this repository and rendered
[on GitHub pages here](https://openaicellular.github.io/oaic/).

Clone this repository and its submodules recursively:

    git clone https://github.com/openaicellular/oaic.git
    git submodule update --init --recursive

## Documentation

Install [sphinx](https://www.sphinx-doc.org/en/master/) with the
[read-the-docs](https://readthedocs.org/) theme:

    sudo -H python3 -m pip install -r requirements.txt

To build the core documentation, simply run `make`.
The resulting documentation is put in `docs/build/html`.

