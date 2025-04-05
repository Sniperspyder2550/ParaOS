#!/bin/bash
# Container bauen
docker build -t paraos . || exit 1

# Kompilieren & Ausführen
docker run -it --rm -v $(pwd):/os paraos make run#!/bin/bash
docker build -t paraos .
docker run -it --rm -v $(pwd):/os paraos make run