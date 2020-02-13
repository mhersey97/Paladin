#!/bin/bash
module load linuxbrew/colsa/

art_illumina -ef -i acidovorax_avenae.fna.gz -p -l 249 -f 1 -m 250 -s 50 -o outaavenae 
art_illumina -ef -i escherichia_coli.fna.gz -p -l 249 -f 1 -m 250 -s 50 -o outecoli
art_illumina -ef -i pesudomonas_aeruginosa.fna.gz -p -l 249 -f 1 -m 250 -s 50 -o outpaeruginosa 
art_illumina -ef -i micrococcus_luteus.fna.gz -p -l 249 -f 1 -m 250 -s 50 -o outmluteus
art_illumina -ef -i clostridioides_difficile.fna.gz -p -l 249 -f 1 -m 250 -s 50 -o outcdifficile 
art_illumina -ef -i fusarium_oxysporum.fna.gz -p -l 249 -f 1 -m 250 -s 50 -o outfoxysporum 
art_illumina -ef -i candida_albicans.fna.gz -p -l 249 -f 1 -m 250 -s 50 -o outcalbicans 
art_illumina -ef -i caenorhabditis_elegans.fna.gz -p -l 249 -f 1 -m 250 -s 50 -o outcelegans 

