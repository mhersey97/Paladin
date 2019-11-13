// Handles the output for BLAST tabular, acting as a mirror of the uniprot.h
#include "kstring.h"
#include "bntseq.h"
#include "bwa.h"
#include "bwamem.h"
#ifndef BLAST_H_
#define BLAST_H_

// rendering
void renderBLASTReport(FILE * passStream, const char * passProxy);

void mem_aln2blast(const mem_opt_t *opt, const bntseq_t *bns, kstring_t *str, bseq1_t *s, int n, const mem_aln_t *list, int which, const mem_aln_t *m_);

#endif
