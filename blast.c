





void renderBLASTReport(FILE * passStream, const char * passProxy) {
    //UniprotList * globalLists;
    char commonHeader[] = "qSeqId\sSeqId\pIdent\length\mismatch\gapOpen\qStart\qEnd\sStart\sEnd\eValue\bitScore"; // headers for the full blast tabular format

    // Report no data
    globalLists = getGlobalLists(passPrimary);
    if (true/*globalLists[0].entryCount == 0*/) { // always true for now as we are just making dummy code
        fprintf(passStream, "No entries to report\n");
        return;
    }
}