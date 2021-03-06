/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2015
#       All rights reserved
#
#       @author       :shan
#       @name         :Sam Liu
#       @file         :/mnt/hgfs/code/rose/src/dpdk-2.0.0/examples/zebra/vsr\vsr_cmd_api.c
#       @date         :2015-08-22 19:38
#       @algorithm    :
=============================================================================*/

//#include "vsr_cmd_api.h"
#include "vsr_api.h"

//#define DEBUG
#ifdef  DEBUG   
#define debug(fmt,args...)  printf ("func(%s), line(%d)"fmt"\n" ,__FUNCTION__, __LINE__, ##args)
#else   
#define debug(fmt,args...)   
#endif  /* DEBUG */ 

berr rule_vsr_cmd_add(uint32_t index, uint32_t ip, uint64_t msisdn)
{
    int rv;

    /* parameter check */
    /* cmd query, so no need to check  */
   
    /* del old rule */
    rv = vsr_api_del_rule_by_index(index);
    if(rv)
    {
        /* error del */
        debug("vsr_api_del_rule_by_index, index(%d), rv(%d)", index, rv);
        return rv;
    }

    /* add new rule */
    rv = vsr_api_add_rule(index, ip, msisdn);
    if(rv)
    {
        /* error del */
        debug("vsr_api_add_rule, index(%d), rv(%d)", index, rv);
        return rv;
    }

    return E_SUCCESS;

}
berr rule_vsr_cmd_del(uint32_t index)
{
    int rv;
    /* parameter check */
    /* cmd query, so no need to check  */

    /* del old rule */
    rv = vsr_api_del_rule_by_index(index);
    if(rv)
    {
        /* error del */
        debug("vsr_api_del_rule_by_index, index(%d), rv(%d)", index, rv);
        return rv;
    }

    return E_SUCCESS;

}

berr rule_vsr_cmd_get_index_by_ip(uint32_t ip, uint32_t *index)
{
    int i;
    uint32_t get_ip = 0;
    /* check ip parameter */
    /* cmd query, so no need to check  */

    /* check  index pointer */
    if (NULL == index)
    {
        BRET(E_PARAM);
    }


    /* loop to find out the matched ip */
    for (i = 0; i < VSR_RULE_NUM_MAX; i++)
    {
        get_ip = vsr_api_get_ip_by_index((uint32_t)i);
        if ( ip == get_ip)
        {
            break;
        }
        debug("index(%d), input ip(%d.%d.%d.%d), get ip(%d.%d.%d.%d)", i,
                (ip>>24)&0xff,
                (ip>>16)&0xff,
                (ip>> 8)&0xff,
                (ip>> 0)&0xff,
                (get_ip>>24)&0xff,
                (get_ip>>16)&0xff,
                (get_ip>> 8)&0xff,
                (get_ip>> 0)&0xff);
    }

    if ( i == VSR_RULE_NUM_MAX)
    {
        BRET(E_FOUND);
    }
    *index = i;
    debug("match index(%d)", i);

    return E_SUCCESS;
}

berr rule_vsr_cmd_get_ip(uint32_t index, uint32_t *ip)
{
    /* check  index pointer */
    if ( index >= VSR_RULE_NUM_MAX )
    {
        BRET(E_PARAM);
    }

    if (NULL == ip)
    {
        BRET(E_PARAM);
    }

    *ip = vsr_api_get_ip_by_index(index);
    return E_SUCCESS;
}

berr rule_vsr_cmd_get_mobile(uint32_t index, uint64_t *mobile)
{
    /* check  index pointer */
    if ( index >= VSR_RULE_NUM_MAX )
    {
        BRET(E_PARAM);
    }

    if (NULL == mobile)
    {
        BRET(E_PARAM);
    }

    *mobile = vsr_api_get_mobile_by_index(index);
    return E_SUCCESS;
}

berr rule_vsr_cmd_get_index_by_mobile(uint64_t mobile, uint32_t *index)
{
    int i;
    uint64_t get_mobile = 0;
    /* check mobile parameter */
    /* cmd query, no need to check */

    /* check  index pointer */
    if (NULL == index)
    {
        BRET(E_PARAM);
    }

    /* loop to find out the matched ip */
    for (i = 0; i < VSR_RULE_NUM_MAX; i++)
    {
        get_mobile = vsr_api_get_mobile_by_index(i);
        if ( mobile == get_mobile )
        {
            break;
        }
        debug("index(%d), input mobile(%llu), get_mobile(%llu)", i, (ULL)mobile, (ULL)get_mobile);
    }

    if ( i == VSR_RULE_NUM_MAX)
    {
        BRET(E_FOUND);
    }
    *index = i;
    debug("match index(%d)", i);

    return E_SUCCESS;

}

berr vsr_dump_buff(vsr_rule_entry_t * rule_entry, uint8_t * buff, uint32_t size)
{
    int l = 0;
    int len = size;
    int i;
    if (rule_entry->effective)
    {
        l += snprintf((char *)(buff + l), len - l, "rule index:%u, ip:%u.%u.%u.%u, mobile:%llu, match_pkts:%llu\r\n",
                rule_entry->index,
                (rule_entry->ip >> 24) & 0xff,
                (rule_entry->ip >> 16) & 0xff,
                (rule_entry->ip >>  8) & 0xff,
                (rule_entry->ip >>  0) & 0xff,
                (ULL)rule_entry->msisdn,
                (ULL)rule_entry->match_pkt);
        //l += snprintf((char *)(buff + l), len -l, "effective:%u\r\n", rule_entry->effective);
        l += snprintf((char *)(buff + l), len -l, "url_num:%u\r\n", rule_entry->url_num);
        for ( i = 0; i < rule_entry->url_num; i++ )
        //for ( i = 0; i < VSR_URL_NUM_MAX; i++ )
        {
            if ( rule_entry->url_entry[i].effective)
            {
                l += snprintf((char *)(buff + l), len -l, "url index:%u, len:%u, url:%s, match_pkts:%llu\r\n", i,
                        rule_entry->url_entry[i].len, rule_entry->url_entry[i].url, (ULL)rule_entry->url_entry[i].match_pkt);
#if 0
                l += snprintf((char *)(buff + l), len -l, "effective:%u, hash:0x%x\r\n",
                        rule_entry->url_entry[i].effective, rule_entry->url_entry[i].hash);
#endif
            }
        }
    }

    return E_SUCCESS;
}

berr rule_vsr_cmd_dump(uint32_t index, uint8_t *buff, uint32_t len)
{
    int rv;
    vsr_rule_entry_t rule_entry;
    /* check index parameter */

    /* check buff */
    if(NULL == buff)
    {
        debug("para err");
        BRET(E_PARAM);
    }

    /*get rule entry content*/
    rv = vsr_api_get_rule_entry(index, &rule_entry);
    if (rv)
    {
        debug("para err");
        return rv;
    }

    /*dump data to buff */
    vsr_dump_buff(&rule_entry, buff, len);
    return E_SUCCESS;
}


berr vsr_dump_total_buff(uint32_t encourage_num, uint64_t ip_num, uint64_t url_num, uint8_t * buff, uint32_t size)
{
    int l = 0;
    int len = size;

    l += snprintf((char *)(buff + l), len -l, "rule       num:%llu\r\n", (ULL) ip_num);
    l += snprintf((char *)(buff + l), len -l, "url        num:%llu\r\n", (ULL)url_num);
    l += snprintf((char *)(buff + l), len -l, "encourage  num:%u\r\n",  encourage_num);

    return E_SUCCESS;
}
berr rule_vsr_cmd_total_dump(uint8_t *buff, uint32_t len)
{
    uint32_t encourage_num; 
    uint64_t ip_num; 
    uint64_t url_num;
    /* check index parameter */

    /* check buff */
    if(NULL == buff)
    {
        debug("para err");
        BRET(E_PARAM);
    }

    encourage_num = vsr_api_encourage_get();
    ip_num        = vsr_api_ip_num_get();
    url_num       = vsr_api_url_num_get();

    /*dump data to buff */
    vsr_dump_total_buff(encourage_num, ip_num, url_num, buff, len);
    return E_SUCCESS;
}

berr rule_vsr_cmd_flush_url(uint32_t index)
{
    int rv;
    /* index parameter check */

    /*flush url*/
    rv = vsr_api_flush_url(index);
    if(rv)
    {
        debug("para err");
        return rv;
    }

    return E_SUCCESS;
}

berr rule_vsr_cmd_clear_statistics(uint32_t index)
{
    int rv;

    /* index parameter check */

    /*clear statistics */
    rv = vsr_api_clear_statistics(index);
    if (rv)
    {
        debug("para err");
        return rv;
    }

    return E_SUCCESS;

}

berr rule_vsr_cmd_encourage(uint32_t num)
{
    int rv;

    /* index parameter check */

    /*clear statistics */
    rv = vsr_api_encourage_set(num);
    if (rv)
    {
        debug("para err");
        return rv;
    }
    return E_SUCCESS;
}


