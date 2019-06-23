#ifndef __DISTRIBUTION_H
#define __DISTRIBUTION_H

#include "client.h"
#include "json.h"

typedef int8_t *(*elink_handler)(p_client_node_t, json_object *);

typedef struct
{
    const int8_t *type;
    elink_handler handler;
} elink_feature_t;

int32_t data_distribution(p_client_node_t p_client_node);

#endif