#ifndef __LUNA_TYPES_H__
#define __LUNA_TYPES_H__

#include <pcap.h>

#include "boots.h"
#include "bts_util.h"

typedef uint32_t ipaddr_t;


#define USE_M_QUEUE 0



#define PACKET_MTU    1500

#define ACT_LOG         1
#define ACT_TRACE       2
#define ACT_DROP        4
#define ACT_PUSH        8
#define ACT_REDIR       16
#define ACT_TAGDUMP     32
#define ACT_MASK        64
#define ACT_URLDUMP     128

#define ACT_IS_VAILD(_val, _act) \
    (((_val) & (_act)) == (_act))

#define LUNA_ACL_HITS_MAX   32

#define LUNA_URL_LEN_MAX    1280
#define URL_URI_LEN_MAX     LUNA_URL_LEN_MAX

typedef struct {
    uint32_t actions;
    uint32_t mask;
    uint16_t rate; /* redir sample rate, rate%, 1%,2%,3%*/
    uint16_t sample; /* redir sample 100/rate, int*/
    uint32_t resv; /* resverd for align*/
    uint64_t cnt;
    uint64_t vcnt;/*pre drop*/
    uint64_t pushed_cnt;/*pre pushed then second assert*/
    char url[LUNA_URL_LEN_MAX];
} luna_acl_t;

#define ACL_DORP(_acl) \
    (_acl).actions |= ACT_DROP

#define ACL_LOG(_acl) \
    _acl.actions |= ACT_LOG

#define ACL_TRACE(_acl) \
    _acl.actions |= ACT_TRACE

#define ACL_HIT(_acl) \
    bts_u64_inc(&(_acl.cnt))

#define ACL_PRE_NOT_DROP_HIT(_acl) \
    bts_u64_inc(&(_acl.vcnt))

#define ACL_PUSHED_ASSERT_HIT(_acl) \
    bts_u64_inc(&(_acl.pushed_cnt))

#define ACL_PUSHED_ASSERT_SET(_acl, _val) \
    bts_u64_set(&(_acl.pushed_cnt), _val)

#define ACL_CNT_CLEAR(_acl) \
	bts_u64_set(&(_acl.cnt), 0)


#define ACL_CNT_SET(_acl, _val) \
	bts_u64_set(&(_acl.cnt), _val)

#define ACL_CNT_GET(_acl) \
	bts_u64_get(&(_acl.cnt))


typedef enum
{
    APP_TYPE_HTTP_GET_OR_POST = 1,
    APP_TYPE_HTTP_OTHER,    
}APP_TYPE_E;

struct pbuf {
	//struct pbuf *next; /*always NULL*/
	void 	* ptr;	
	uint16_t  len;  /*this buffer len*/
	//uint16_t  total_len; 
	uint16_t  ptr_offset;/*when The module Pid handle the protocol any*/
	//uint8_t flag;
};

enum ad_action_em
{
    AD_IGNORE,
    AD_SUCCESS,
    AD_FAILED,    
};

typedef enum
{
    AD_TEMPLATE_PC,
    AD_TEMPLATE_MOBILE,
    AD_TEMPLATE_MAX,
}ad_template_em;

#define URL_MAX_LEN  1500//URL MAX LEN
#define MAX_HOST_LEN 128
#define MAX_ACCOUNT_LEN   64
#define MAX_USER_AGENT_LEN  256

#define OVECCOUNT 10

#define ENABLE 1
#define DISABLE 0

#define URL_HOST_LEN_MAX    64
#define URL_PARAM_NUMB_MAX  32
#define URL_PARAM_KEY_LEN_MAX   64
#define URL_PARAM_VAL_LEN_MAX   1280

typedef struct {
    uint32_t  pnumb;
    char pkeys[URL_PARAM_NUMB_MAX][URL_PARAM_KEY_LEN_MAX];
    char pvals[URL_PARAM_NUMB_MAX][URL_PARAM_VAL_LEN_MAX];
} luna_url_param_t;

typedef struct {
    uint16_t host_len;
    uint16_t uri_len;
    char url[LUNA_URL_LEN_MAX];
    char host[URL_HOST_LEN_MAX];
    char uri[URL_URI_LEN_MAX];
    char pstr[URL_URI_LEN_MAX];
    luna_url_param_t params;
} luna_url_t;


typedef struct
{

    char *eth;
    int   eth_tx;  
    pcap_t  *fp;
    
    /* L2 offset */
    uint16_t l2_offset;

    /* L3 offset */
    uint16_t l3_offset;
    uint16_t total_len; /* ip packet total length */

    /* L4 offset */
    uint16_t l4_offset;
    uint16_t l4_paylen;

    /* L5 offset */
    uint16_t l5_offset;
    uint16_t l5_len;

    
    /*OT L3*/
	uint32_t outer_srcip4;
	uint32_t outer_dstip4;
    /*OT L4 Protoco;*/
    uint8_t outer_protocol;
    /*OT L4*/
	uint16_t outer_srcport;
	uint16_t outer_dstport;

    uint32_t outer_seq;
    uint32_t outer_ack;

    uint32_t teid;
    /*IN L3*/
	uint32_t inner_srcip4;
	uint32_t inner_dstip4;
    /*IN L4 Protoco;*/
    uint8_t inner_protocol;    
    /*IN L4*/
	uint16_t inner_srcport;
	uint16_t inner_dstport;


#ifdef  _ENABLE_IPV6 
	uint32_t outer_srcip6[4];
	uint32_t outer_dstip6[4];
	uint32_t inner_srcip6[4];
	uint32_t inner_dstip6[4];		
#endif	

    uint16_t app_type;

    uint16_t url_len;
    uint16_t host_len;
	uint16_t referer_len;
	char url[URL_MAX_LEN+1];
	uint8_t host[MAX_HOST_LEN+1];
    char account[MAX_ACCOUNT_LEN+1];
	char referer[URL_MAX_LEN+1];
    uint16_t user_agent_len;
    char  user_agent[MAX_USER_AGENT_LEN+1];
    luna_url_t ori_url;
    luna_url_t ref_url;
    
    luna_acl_t acl;


    char     uri[URL_MAX_LEN+1];	
    uint16_t uri_len;
    char reg[URL_MAX_LEN];
  

    struct pbuf pbuf;
    //struct rte_mbuf *m;
    uint16_t match;   /* 0 for not match, 1 for vsr match, 2 for other match  */
    enum ad_action_em ad_act;

    /* ad template select */
    ad_template_em template;    /* ad template id , ad_template_enum */
    uint16_t content_len; /* ad template content total length */
    uint16_t content_offset; /* ad template content offset, used for multiple transmit */
    uint16_t fill_len;       /* single time fill length */ 
    uint16_t data_len;       /* total packet length, used for template */
    uint8_t  pushed_second_assert;
	uint8_t  snet_hit_id;
	char * hijack_url;

    uint32_t rule_hits;
    uint32_t rule_idxs[LUNA_ACL_HITS_MAX];
}hytag_t;

typedef struct {
    uint32_t idx;
    uint32_t acl;
    uint32_t group;
    void *data;
} luna_rule_t;

typedef struct {
    uint32_t offset;
    uint32_t size;
    uint32_t inuse;
    luna_rule_t *rules;
} luna_rule_tab_t;

#define HYTAG_ACL_MERGE(_tagacl, _ruleacl) \
{ \
    (_tagacl).actions |= (_ruleacl).actions; \
    (_tagacl).mask |= (_ruleacl).mask;\
    if ((0 == strlen((_tagacl).url)) && (0 != strlen((_ruleacl).url))) { \
        strcpy((_tagacl).url, (_ruleacl).url); \
    } \
    ACL_HIT(_tagacl); \
}

#define HYTAG_ACL_SET(_acl, _val)   _acl.actions |= _val

#define USE_D_PACKET 0 // comment for now by shan275

#define DEBUG_USE_CYCLE 0
#if DEBUG_USE_CYCLE

#define CYCLE_INIT(__v) uint64_t start, end;int _cycle_off = __v;
#define CYCLE_START()   if(_cycle_off) start = rte_rdtsc()  
#define CYCLE_END()     if(_cycle_off) {end = rte_rdtsc();printf("<%s +%d> Take times: %lu\n", __FUNCTION__, __LINE__, end- start);}
#else
#define CYCLE_INIT(__v)
#define CYCLE_START()
#define CYCLE_END()
#endif


#endif /* !__LUNA_TYPES_H__ */
