#!/bin/bash
# Usage: ./zip_submission.sh

id1="212825715"
id2="322634783"
folder="${id1}_${id2}_project"

mkdir -p "$folder"
cp symnmf.py symnmf.c symnmfmodule.c symnmf.h symnmfmodule.h analysis.py setup.py kmeans.py Makefile "$folder"/

tar -czvf "${folder}.tar.gz" "$folder"
rm -rf "$folder"