#ifndef __AUTHENTICATION_H
#define __AUTHENTICATION_H

#include "common.h"
#include "client.h"

int32_t restart_authentication(p_client_node_t p_client_node, client_type_t type);
int32_t start_authentication(p_client_node_t p_client_node);

#endif