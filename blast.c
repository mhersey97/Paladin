#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <curl/curl.h>
#include <unistd.h>
#include <zlib.h>
#include <math.h>
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

    int gap_count = 0; //gapopen = count of I and D in CIGAR string
    int soft_clips = 0; //soft clips sum used to calculate length
    int left_clips = 0;
    int query_length = s->l_seq;
    if(p->rid >= 0) { // with coordinate
        kputs(qseqid(s->name), str); kputc('\t', str); // qseqid
        kputs(bns->anns[p->rid].name, str); kputc('\t', str); // sseqid
        if (p->n_cigar) { // aligned
			for (i = 0; i < p->n_cigar; ++i) {
				int c = p->cigar[i]&0xf;
				if (!(opt->flag&MEM_F_SOFTCLIP) && !p->is_alt && (c == 3 || c == 4))
					c = which? 4 : 3; // use hard clipping for supplementary alignments
                if( c == 1 || c == 2) //I or D
                    gap_count++;
                if( c == 3 ) { //S
                    soft_clips+= p->cigar[i]>>4;
                }
                if( i == 0 && c == 3){
                    left_clips = p->cigar[i]>>4;
                }
			}
		}
        int length = query_length - soft_clips;
        double ident = 100 * ((double) ((double)(length - p->NM) / length));
        kputw( ident, str);
        if (ident != 100) {
            kputc('.', str);
            kputw(1000 * (ident - (int) ident), str);
        }
        kputc('\t', str); //% identical
        kputw( length , str); kputc('\t', str);//length
        kputw( p->NM, str); kputc('\t', str);//mismatch
        kputw( gap_count, str); kputc('\t', str); //gapopen
        kputw( left_clips, str); kputc('\t', str); //qStart
        kputw( (length * 3) + left_clips, str); kputc('\t', str); //qEnd uses nucleotide length
        long sstart = p->pos + 1;
        kputl(sstart, str); kputc('\t', str); //sstart
        kputl( sstart + length -1, str); kputc('\t', str); //ssend

        //calculate bitscore and e-value
        double lambda = calcLambda(opt->e_del, opt->o_del);
        double k = calcK( opt->e_del, opt->o_del);
        double bitScore = ( (lambda * (p->score *5.4)) - log(k))/( log(2));
	    double eval = (length * opt->l)/(pow(2,bitScore));
	    char * out = malloc(5);
	    sprintf(out, "%5.2e", eval);
        kputs( out, str); kputc('\t', str);
        sprintf(out, "%0.2f", bitScore);
        kputs( out, str);//kputc('\n', str);
        kputc('\n', str);
        free(out);
    }

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

double calcLambda( int a, int b) {
    if( a> 12 || b > a ){
        return 0.318;
    } else if( a == 12) {
        if( b >= 3 ) {
            return 0.305;
        } else if( b == 2 ) {
            return 0.300;
        } else {
            return 0.275;
        }
    } else if( a == 11 ) {
        if( b >= 3 ){
            return 0.301;
        } else if ( b == 2) {
            return 0.286;
        } else {
            return 0.255;
        }
    } else if ( a == 10 ) {
        if( b >= 4 ) {
            return 0.293;
        } else if ( b == 3) {
            return 0.281;
        } else if ( b == 2 ) {
            return 0.266;
        } else {
            return 0.216;
        }
    } else if ( a == 9 ) {
        if( b >= 5 ) {
            return 0.286;
        } else if ( b == 3 || b == 4) {
            return 0.273;
        } else if ( b == 2 ) {
            return 0.244;
        } else {
            return 0.176;
        }
    } else if (a == 8 ) {
        if ( b == 7 || b == 8) {
            return 0.270;
        } else if (b >= 4 || b <= 6 ) {
            return 0.262;
        } else if ( b == 3 ) {
            return 0.243;
        } else if ( b == 2 ) {
            return 0.215;
        } else {
            return 1.0;
        }
    } else if ( a == 7 ) {
        if ( b == 6 || b == 7 ) {
            return 0.247;
        } else if ( b == 4 || b == 5 ) {
            return 0.230;
        } else if ( b == 3 ) {
            return 0.208;
        } else if ( b == 2 ) {
            return 0.164;
        } else {
            return 1.0;
        }
    } else if ( a == 6 ) {
        if ( b == 5 || b == 6 ) {
            return 0.200;
        } else if ( b == 4 ) {
            return 0.179;
        } else if ( b == 3 ) {
            return 0.153;
        } else {
            return 0.269;
        }
    } else if ( a == 5 ) {
        if ( b == 5 ) {
            return 0.131;
        } else {
            return 1.00;
        }
    } else {
        return 1.00;
    }
}

double calcK(int a, int b){
    if( a> 12 || b > a ){
        return 0.13;
    } else if( a == 12) {
        if( b >= 3 ) {
            return 0.10;
        } else if( b == 2 ) {
            return 0.09;
        } else {
            return 0.05;
        }
    } else if( a == 11 ) {
        if( b >= 3 ){
            return 0.09;
        } else if ( b == 2) {
            return 0.07;
        } else {
            return 0.0355;
        }
    } else if ( a == 10 ) {
        if( b >= 4 ) {
            return 0.08;
        } else if ( b == 3) {
            return 0.06;
        } else if ( b == 2 ) {
            return 0.04;
        } else {
            return 0.014;
        }
    } else if ( a == 9 ) {
        if( b >= 5 ) {
            return 0.08;
        } else if ( b == 3 || b == 4) {
            return 0.06;
        } else if ( b == 2 ) {
            return 0.030;
        } else {
            return 0.008;
        }
    } else if (a == 8 ) {
        if ( b == 7 || b == 8) {
            return 0.06;
        } else if (b >= 4 || b <= 6 ) {
            return 0.05;
        } else if ( b == 3 ) {
            return 0.035;
        } else if ( b == 2 ) {
            return 0.021;
        } else {
            return 1.0;
        }
    } else if ( a == 7 ) {
        if ( b == 6 || b == 7 ) {
            return 0.05;
        } else if ( b == 4 || b == 5 ) {
            return 0.030;
        } else if ( b == 3 ) {
            return 0.021;
        } else if ( b == 2 ) {
            return 0.009;
        } else {
            return 1.0;
        }
    } else if ( a == 6 ) {
        if ( b == 5 || b == 6 ) {
            return 0.021;
        } else if ( b == 4 ) {
            return 0.014;
        } else if ( b == 3 ) {
            return 0.010;
        } else {
            return 0.079;
        }
    } else if ( a == 5 ) {
        if ( b == 5 ) {
            return 0.009;
        } else {
            return 1.00;
        }
    } else {
        return 1.00;
    }
}
