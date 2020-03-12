#!/bin/bash
module load linuxbrew/colsa/

art_illumina -ef -i acidovorax_avenae.fna.gz -p -l 100 -f 1 -m 250 -s 50 -o outaavenaeshort 
art_illumina -ef -i escherichia_coli.fna.gz -p -l 100 -f 1 -m 250 -s 50 -o outecolishort
art_illumina -ef -i pesudomonas_aeruginosa.fna.gz -p -l 100 -f 1 -m 250 -s 50 -o outpaeruginosashort 
art_illumina -ef -i micrococcus_luteus.fna.gz -p -l 100 -f 1 -m 250 -s 50 -o outmluteusshort
art_illumina -ef -i clostridioides_difficile.fna.gz -p -l 100 -f 1 -m 250 -s 50 -o outcdifficileshort 
art_illumina -ef -i fusarium_oxysporum.fna.gz -p -l 100 -f 1 -m 250 -s 50 -o outfoxysporumshort 
art_illumina -ef -i candida_albicans.fna.gz -p -l 100 -f 1 -m 250 -s 50 -o outcalbicansshort 
art_illumina -ef -i caenorhabditis_elegans.fna.gz -p -l 100 -f 1 -m 250 -s 50 -o outcelegansshort
#longer files
