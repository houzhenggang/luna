#ifndef __PID_OUTERL4_H
#define __PID_OUTERL4_H

#include "pid.h"

#define HTTP_PORT_80 	80
#define HTTP_PORT_8080 	80


berr pid_tcp(struct pbuf *p, hytag_t *hytag, int inner_outer)
{
    int rv = 0;
    uint16_t tcphr_len = 0;
	struct tcp_hdr_s *tcp_hdr;

    if(check_pbuf_len(p, TCP_HLEN))
	{
        if(inner_outer == HEADER_OUTER)
            cnt_inc(OUTERL4_HD);//drop packet and incr counter, then return;
        else
            cnt_inc(INNERL4_HD);
		BRET(E_EXCEED);
	}

	PBUF_CUR_FORMAT(struct tcp_hdr_s *, tcp_hdr, p);

	if(inner_outer == HEADER_OUTER)
	{
		/* convert src and dest ports to host byte order */
	  	hytag->outer_srcport= ntohs(tcp_hdr->src);
	  	hytag->outer_dstport= ntohs(tcp_hdr->dest);
        cnt_inc(OUTERL4_TCP);
	}
	else 
	{
		/* convert src and dest ports to host byte order */
	  	hytag->inner_srcport= ntohs(tcp_hdr->src);
	  	hytag->inner_dstport= ntohs(tcp_hdr->dest);
        cnt_inc(INNERL4_TCP);
	}

    tcphr_len = TCP_HDR_LEN(tcp_hdr);
    
    UPDATE_PBUF_OFFSET(p, tcphr_len);
    hytag->l5_offset = p->ptr_offset;
    hytag->l5_len    = hytag->total_len - ( hytag->l5_offset - hytag->l3_offset);

	
	uint16_t srcport = ntohs(tcp_hdr->src); 
	uint16_t dstport = ntohs(tcp_hdr->dest); 

	if( dstport == 80 || dstport == 8080 )
	{
	     cnt_inc(APP_HTTP_UP);
		 rv=  pid_http_up(p, hytag);     
	     cnt_inc(APP_HTTP_UP_OVER);
         return rv;
	}
    else if(srcport == 80 || srcport == 8080 )
    {
	     cnt_inc(APP_HTTP_DN);
		 //return pid_http_up(p, hytag);          
    }
	else
	{
	     cnt_inc(APP_OTHER);
	}
    return E_SUCCESS;
}


#endif




