#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "boots.h"
#include "bts_debug.h"
#include "bts_cnt.h"

#include "luna_types.h"
#include "luna_util.h"
#include "mask.h"

//#define DEBUG
#ifdef  DEBUG   
#define debug(fmt,args...)  printf ("func(%s), line(%d)"fmt"\n" ,__FUNCTION__, __LINE__, ##args)
#else   
#define debug(fmt,args...)   
#endif  /* DEBUG */ 

mask_rule_tab_t  rule_tab;

berr mask_rule_add(uint32_t id,  uint32_t mask, luna_acl_t *acl)
{
    if(id >= MAX_MASK_RULE || acl == NULL) 
    {
        return E_PARAM;
    }
    
    mask_rule_t *rule = &(rule_tab.rules[id]);
   
    rule->id = id;
    rule->mask = mask;

    memcpy(&rule->acl, acl, sizeof(luna_acl_t));
    rule->used = 1;

    if(id >= rule_tab.inuse)
    {
        rule_tab.inuse = id +1;
    }

    return E_SUCCESS;
}

mask_rule_t * mask_rule_get(uint32_t id)
{
    if(id >= MAX_MASK_RULE) 
    {
        return NULL;
    }    

    return &(rule_tab.rules[id]);   
}


berr mask_rule_del(uint32_t id)
{
    if(id >= MAX_MASK_RULE) 
    {
        return E_PARAM;
    }
    
    memset(&rule_tab.rules[id], 0, sizeof(mask_rule_t));

    return E_SUCCESS;
}


berr luna_mask(hytag_t *hytag)
{
    int i;
    mask_rule_t *rule = NULL;

	CNT_INC(MASK_PKTS);

    if (NULL == hytag) {
        CNT_INC(MASK_PARAM);
        return E_PARAM;
    }

    if (hytag->acl.mask == 0) {
        CNT_INC(MASK_ZERO);
        return E_SUCCESS;
    }

    CNT_INC(MASK_NOTZERO);

    debug("mask=0x%x", hytag->acl.mask);
    debug("rule_tab.inuse:%u", rule_tab.inuse);

    for (i = 0; i < rule_tab.inuse; i++) 
    {
        /*
         *
        typedef struct {
            uint32_t id;
            uint32_t used;
            uint32_t mask;
            luna_acl_t acl;
        } mask_rule_t;

        typedef struct {
            uint32_t actions;
            uint32_t mask;
            bts_atomic64_t cnt;
            bts_atomic64_t vcnt;
            bts_atomic64_t pushed_cnt;
            char url[LUNA_URL_LEN_MAX];
        } luna_acl_t;
         *
         */
        rule = &rule_tab.rules[i];
        debug("rule->id             :%u", rule->id);
        debug("rule->used           :%u", rule->used);
        debug("rule->mask           :%u", rule->mask);
        debug("rule->acl.actions    :0x%x", rule->acl.actions);
        debug("rule->acl.mask       :0x%x", rule->acl.mask);
        debug("rule->acl.url        :%s", rule->acl.url);
        if (0 == rule->used) {
            continue;
        }
        debug("OPT: mask_url  ===================================================================");
        debug("rule->mask:0x%x, rule->mask & hytag->acl.mask:0x%x", rule->mask, rule->mask & hytag->acl.mask);

        if (hytag->acl.mask == (rule->mask & hytag->acl.mask)) {
            debug("OPT: mask_url  ===================================================================");
            ACL_HIT(rule->acl);
            HYTAG_ACL_MERGE(hytag->acl, rule->acl);
            CNT_INC(MASK_MATCHED);

            if (OPT_IS_SELECTED(OPT_MASK_URL)) 
            {
                debug("OPT: mask_url  ===================================================================");
                debug("ORI_URL:");
                luna_url_dump(&hytag->ori_url);
                debug("REF_URL:");
                luna_url_dump(&hytag->ref_url);
                debug("REDIR_URL:%s", hytag->acl.url); 
                debug("--------------------------------------------------------------------------------------------------------\n");
            }
            return E_SUCCESS; 
        }
    }

    CNT_INC(MASK_DISMATCH);

    return E_SUCCESS;
}

/* End of file */