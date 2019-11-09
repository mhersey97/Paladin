#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <curl/curl.h>
#include <unistd.h>
#include <zlib.h>


void renderBLASTReport(FILE * passStream, const char * passProxy) {
    //UniprotList * globalLists;
    char commonHeader[] = "qSeqId\tsSeqId\tpIdent\tlength\tmismatch\tgapOpen\tqStart\tqEnd\tsStart\tsEnd\teValue\tbitScore"; // headers for the full blast tabular format

    // Report no data
    globalLists = getGlobalLists(passPrimary);
    if (true/*globalLists[0].entryCount == 0*/) { // always true for now as we are just making dummy code
        fprintf(passStream, "No entries to report\n");
        return;
    }
}