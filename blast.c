#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <curl/curl.h>
#include <unistd.h>
#include <zlib.h>
#include "blast.h"

void renderBLASTReport(FILE * passStream, const char * passProxy) {
    //UniprotList * globalLists;
    //char commonHeader[] = "qSeqId\tsSeqId\tpIdent\tlength\tmismatch\tgapOpen\tqStart\tqEnd\tsStart\tsEnd\teValue\tbitScore"; // headers for the full blast tabular format

    // Report no data
    //globalLists = getGlobalLists(passPrimary);
    if (1/*globalLists[0].entryCount == 0*/) { // always true for now as we are just making dummy code
        fprintf(passStream, "No entries to report\n");
        return;
    }
}

void mem_aln2blast(const mem_opt_t *opt, const bntseq_t *bns, kstring_t *str, bseq1_t *s, int n, const mem_aln_t *list, int which, const mem_aln_t *m_) {
    //copying stuff from mem_aln2sam since some of it is necessary, possibly not all of it
    //to be trimmed down
    int i, l_name;
	mem_aln_t ptmp = list[which], *p = &ptmp, mtmp, *m = 0; // make a copy of the alignment to convert
	if (m_) mtmp = *m_, m = &mtmp;

	// set flag
	p->flag |= m? 0x1 : 0; // is paired in sequencing
	p->flag |= p->rid < 0? 0x4 : 0; // is mapped
	p->flag |= m && m->rid < 0? 0x8 : 0; // is mate mapped
	if (p->rid < 0 && m && m->rid >= 0) // copy mate to alignment
		p->rid = m->rid, p->pos = m->pos, p->is_rev = m->is_rev, p->n_cigar = 0;
	if (m && m->rid < 0 && p->rid >= 0) // copy alignment to mate
		m->rid = p->rid, m->pos = p->pos, m->is_rev = p->is_rev, m->n_cigar = 0;
	p->flag |= p->is_rev? 0x10 : 0; // is on the reverse strand
	p->flag |= m && m->is_rev? 0x20 : 0; // is mate on the reverse strand

    l_name = strlen(s->name);
	ks_resize(str, str->l + s->l_seq + l_name + (s->qual? s->l_seq : 0) + 20);
	kputs(qseqid(s->name), str); kputc('\t', str); // qseqid
    if (p->rid >= 0) { // with coordinate
        kputs(bns->anns[p->rid].name, str); kputc('\t', str); // sseqid
    } else {
        kputs("*\t", str);
    }
    kputs("pIdent\t", str);
    int gap_count = 0; //gapopen = count of I and D in CIGAR string
    int soft_clips = 0; //soft clips sum used to calculate length
    int left_clips = 0;
    int query_length = s->l_seq;
    int length = query_length - soft_clips;
    if(p->rid >= 0) { // with coordinate
        if (p->n_cigar) { // aligned
			for (i = 0; i < p->n_cigar; ++i) {
				int c = p->cigar[i]&0xf;
				if (!(opt->flag&MEM_F_SOFTCLIP) && !p->is_alt && (c == 3 || c == 4))
					c = which? 4 : 3; // use hard clipping for supplementary alignments
                if( c == 1 || c == 2)
                    gap_count++;
                if( c == 3 ) {
                    soft_clips+= p->cigar[i]>>4;
                }
                if( i == 0 && c == 3){
                    left_clips = p->cigar[i]>>4;
                }
			}
		}
        kputw( length , str); kputc('\t', str);//length
        kputs("mismatch\t", str);
        kputw(gap_count, str); kputc('\t', str); //gapopen
    } else {
        kputs("*\t*\t*\t", str);
    }


    if( p->rid >= 0) {
        kputw( left_clips, str); kputc('\t', str); //qStart
        kputw( query_length + soft_clips, str); kputc('\t', str); //qEnd
        long sstart = p->pos + 1;
        kputl(sstart, str); kputc('\t', str); //sstart
        kputl( sstart + length -1, str); kputc('\t', str); //ssend
    } else {
        kputs("*\t*\t*\t*\t", str);
    }
    kputs("eValue\tbitScore\n", str);

}

char* qseqid( char* name) {
    int trim = 0;
    int colon_count = 0;
    int old_length = strlen(name);
    for( int i = 0; i < old_length ; i++) {
        if(name[i] == ':'){
            colon_count++;
        }
        if( colon_count == 3){
            trim = i;
            break;
        }
    }
    char *dest = (char*)malloc(sizeof(char) * (old_length - trim));
    for(int i = 0; i < old_length - trim; i++) {
        dest[i] = name[i+trim+1];
    }
    return dest;
}
