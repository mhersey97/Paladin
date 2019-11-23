#!/bin/bash
./blastx -db sampledb -outfmt 6 -query queries.fasta -out blastResults.txt
