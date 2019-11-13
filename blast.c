#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <curl/curl.h>
#include <unistd.h>
#include <zlib.h>
#include "blast.h"

void renderBLASTReport(FILE * passStream, const char * passProxy) {
    //UniprotList * globalLists;
    char commonHeader[] = "qSeqId\tsSeqId\tpIdent\tlength\tmismatch\tgapOpen\tqStart\tqEnd\tsStart\tsEnd\teValue\tbitScore"; // headers for the full blast tabular format

    // Report no data
    //globalLists = getGlobalLists(passPrimary);
    if (1/*globalLists[0].entryCount == 0*/) { // always true for now as we are just making dummy code
        fprintf(passStream, "No entries to report\n");
        return;
    }
}

void mem_aln2blast(const mem_opt_t *opt, const bntseq_t *bns, kstring_t *str, bseq1_t *s, int n, const mem_aln_t *list, int which, const mem_aln_t *m_) {
    kputs("qSeqId\tsSeqId\tpIdent\tlength\tmismatch\tgapOpen\tqStart\tqEnd\tsStart\tsEnd\teValue\tbitScore\n", str);

}
