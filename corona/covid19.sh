#!/bin/bash
module load linuxbrew/colsa



art_illumina -ef -i coronavirus.fna -p -l 100 -f 1 -m 750 -s 50 -o outcorona

samtools fastq  outcorona.sam > outcorona.fq

awk '{if(NR%4==1) {printf(">%s\n",substr($0,2));} else if(NR%4==2) print;}' outcorona.fq > outcorona.fasta

./blastx -db sampledb -outfmt 6 -query outcorona.fasta -out outcorona.txt -threshold 15
