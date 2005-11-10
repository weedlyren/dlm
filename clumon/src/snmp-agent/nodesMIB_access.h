/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.access_functions.conf,v 1.6.2.2 2004/06/21 10:12:28 dts12 Exp $
 */

#ifndef nodesMIB_access_h
#define nodesMIB_access_h

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>


extern "C" {
  /** User-defined data access functions for data in table rhcNodesTable */
  /** row level accessors */
  Netsnmp_First_Data_Point rhcNodesTable_get_first_data_point;
  Netsnmp_Next_Data_Point rhcNodesTable_get_next_data_point;
  Netsnmp_Free_Loop_Context rhcNodesTable_loop_free;
  Netsnmp_Make_Data_Context rhcNodesTable_context_convert_function;
  Netsnmp_Free_Data_Context rhcNodesTable_data_free;
  
  int rhcNodesTable_commit_row(void** my_data_context, int new_or_del);
  void* rhcNodesTable_create_data_context(netsnmp_variable_list* index_data);

  /** column accessors */
  char* get_rhcNodeName(void* data_context, size_t* ret_len);
  long* get_rhcNodeStatusCode(void* data_context, size_t* ret_len);
  char* get_rhcNodeStatusString(void* data_context, size_t* ret_len);
  long* get_rhcNodeRunningServicesNum(void* data_context, size_t* ret_len);
};

#endif  // nodesMIB_access_h