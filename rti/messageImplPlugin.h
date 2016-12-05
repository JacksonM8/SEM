

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from messageImpl.idl using "rtiddsgen".
The rtiddsgen tool is part of the RTI Connext distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the RTI Connext manual.
*/

#ifndef messageImplPlugin_874241052_h
#define messageImplPlugin_874241052_h

#include "messageImpl.h"

struct RTICdrStream;

#ifndef pres_typePlugin_h
#include "pres/pres_typePlugin.h"
#endif

#if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport __declspec(dllexport)
#endif

/* The type used to store keys for instances of type struct
* AnotherSimple.
*
* By default, this type is struct Message
* itself. However, if for some reason this choice is not practical for your
* system (e.g. if sizeof(struct Message)
* is very large), you may redefine this typedef in terms of another type of
* your choosing. HOWEVER, if you define the KeyHolder type to be something
* other than struct AnotherSimple, the
* following restriction applies: the key of struct
* Message must consist of a
* single field of your redefined KeyHolder type and that field must be the
* first field in struct Message.
*/
typedef  struct test_dds_Message_c test_dds_Message_cKeyHolder;

#define test_dds_Message_cPlugin_get_sample PRESTypePluginDefaultEndpointData_getSample 
#define test_dds_Message_cPlugin_get_buffer PRESTypePluginDefaultEndpointData_getBuffer 
#define test_dds_Message_cPlugin_return_buffer PRESTypePluginDefaultEndpointData_returnBuffer 

#define test_dds_Message_cPlugin_get_key PRESTypePluginDefaultEndpointData_getKey 
#define test_dds_Message_cPlugin_return_key PRESTypePluginDefaultEndpointData_returnKey

#define test_dds_Message_cPlugin_create_sample PRESTypePluginDefaultEndpointData_createSample 
#define test_dds_Message_cPlugin_destroy_sample PRESTypePluginDefaultEndpointData_deleteSample 

/* --------------------------------------------------------------------------------------
Support functions:
* -------------------------------------------------------------------------------------- */

NDDSUSERDllExport extern test_dds_Message_c*
test_dds_Message_cPluginSupport_create_data_w_params(
    const struct DDS_TypeAllocationParams_t * alloc_params);

NDDSUSERDllExport extern test_dds_Message_c*
test_dds_Message_cPluginSupport_create_data_ex(RTIBool allocate_pointers);

NDDSUSERDllExport extern test_dds_Message_c*
test_dds_Message_cPluginSupport_create_data(void);

NDDSUSERDllExport extern RTIBool 
test_dds_Message_cPluginSupport_copy_data(
    test_dds_Message_c *out,
    const test_dds_Message_c *in);

NDDSUSERDllExport extern void 
test_dds_Message_cPluginSupport_destroy_data_w_params(
    test_dds_Message_c *sample,
    const struct DDS_TypeDeallocationParams_t * dealloc_params);

NDDSUSERDllExport extern void 
test_dds_Message_cPluginSupport_destroy_data_ex(
    test_dds_Message_c *sample,RTIBool deallocate_pointers);

NDDSUSERDllExport extern void 
test_dds_Message_cPluginSupport_destroy_data(
    test_dds_Message_c *sample);

NDDSUSERDllExport extern void 
test_dds_Message_cPluginSupport_print_data(
    const test_dds_Message_c *sample,
    const char *desc,
    unsigned int indent);

NDDSUSERDllExport extern test_dds_Message_c*
test_dds_Message_cPluginSupport_create_key_ex(RTIBool allocate_pointers);

NDDSUSERDllExport extern test_dds_Message_c*
test_dds_Message_cPluginSupport_create_key(void);

NDDSUSERDllExport extern void 
test_dds_Message_cPluginSupport_destroy_key_ex(
    test_dds_Message_cKeyHolder *key,RTIBool deallocate_pointers);

NDDSUSERDllExport extern void 
test_dds_Message_cPluginSupport_destroy_key(
    test_dds_Message_cKeyHolder *key);

/* ----------------------------------------------------------------------------
Callback functions:
* ---------------------------------------------------------------------------- */

NDDSUSERDllExport extern PRESTypePluginParticipantData 
test_dds_Message_cPlugin_on_participant_attached(
    void *registration_data, 
    const struct PRESTypePluginParticipantInfo *participant_info,
    RTIBool top_level_registration, 
    void *container_plugin_context,
    RTICdrTypeCode *typeCode);

NDDSUSERDllExport extern void 
test_dds_Message_cPlugin_on_participant_detached(
    PRESTypePluginParticipantData participant_data);

NDDSUSERDllExport extern PRESTypePluginEndpointData 
test_dds_Message_cPlugin_on_endpoint_attached(
    PRESTypePluginParticipantData participant_data,
    const struct PRESTypePluginEndpointInfo *endpoint_info,
    RTIBool top_level_registration, 
    void *container_plugin_context);

NDDSUSERDllExport extern void 
test_dds_Message_cPlugin_on_endpoint_detached(
    PRESTypePluginEndpointData endpoint_data);

NDDSUSERDllExport extern void    
test_dds_Message_cPlugin_return_sample(
    PRESTypePluginEndpointData endpoint_data,
    test_dds_Message_c *sample,
    void *handle);    

NDDSUSERDllExport extern RTIBool 
test_dds_Message_cPlugin_copy_sample(
    PRESTypePluginEndpointData endpoint_data,
    test_dds_Message_c *out,
    const test_dds_Message_c *in);

/* ----------------------------------------------------------------------------
(De)Serialize functions:
* ------------------------------------------------------------------------- */

NDDSUSERDllExport extern RTIBool 
test_dds_Message_cPlugin_serialize(
    PRESTypePluginEndpointData endpoint_data,
    const test_dds_Message_c *sample,
    struct RTICdrStream *stream, 
    RTIBool serialize_encapsulation,
    RTIEncapsulationId encapsulation_id,
    RTIBool serialize_sample, 
    void *endpoint_plugin_qos);

NDDSUSERDllExport extern RTIBool 
test_dds_Message_cPlugin_deserialize_sample(
    PRESTypePluginEndpointData endpoint_data,
    test_dds_Message_c *sample, 
    struct RTICdrStream *stream,
    RTIBool deserialize_encapsulation,
    RTIBool deserialize_sample, 
    void *endpoint_plugin_qos);

NDDSUSERDllExport extern RTIBool
test_dds_Message_cPlugin_serialize_to_cdr_buffer(
    char * buffer,
    unsigned int * length,
    const test_dds_Message_c *sample); 

NDDSUSERDllExport extern RTIBool 
test_dds_Message_cPlugin_deserialize(
    PRESTypePluginEndpointData endpoint_data,
    test_dds_Message_c **sample, 
    RTIBool * drop_sample,
    struct RTICdrStream *stream,
    RTIBool deserialize_encapsulation,
    RTIBool deserialize_sample, 
    void *endpoint_plugin_qos);

NDDSUSERDllExport extern RTIBool
test_dds_Message_cPlugin_deserialize_from_cdr_buffer(
    test_dds_Message_c *sample,
    const char * buffer,
    unsigned int length);    

NDDSUSERDllExport extern RTIBool
test_dds_Message_cPlugin_skip(
    PRESTypePluginEndpointData endpoint_data,
    struct RTICdrStream *stream, 
    RTIBool skip_encapsulation,  
    RTIBool skip_sample, 
    void *endpoint_plugin_qos);

NDDSUSERDllExport extern unsigned int 
test_dds_Message_cPlugin_get_serialized_sample_max_size_ex(
    PRESTypePluginEndpointData endpoint_data,
    RTIBool * overflow,
    RTIBool include_encapsulation,
    RTIEncapsulationId encapsulation_id,
    unsigned int current_alignment);    

NDDSUSERDllExport extern unsigned int 
test_dds_Message_cPlugin_get_serialized_sample_max_size(
    PRESTypePluginEndpointData endpoint_data,
    RTIBool include_encapsulation,
    RTIEncapsulationId encapsulation_id,
    unsigned int current_alignment);

NDDSUSERDllExport extern unsigned int 
test_dds_Message_cPlugin_get_serialized_sample_min_size(
    PRESTypePluginEndpointData endpoint_data,
    RTIBool include_encapsulation,
    RTIEncapsulationId encapsulation_id,
    unsigned int current_alignment);

NDDSUSERDllExport extern unsigned int
test_dds_Message_cPlugin_get_serialized_sample_size(
    PRESTypePluginEndpointData endpoint_data,
    RTIBool include_encapsulation,
    RTIEncapsulationId encapsulation_id,
    unsigned int current_alignment,
    const test_dds_Message_c * sample);

/* --------------------------------------------------------------------------------------
Key Management functions:
* -------------------------------------------------------------------------------------- */
NDDSUSERDllExport extern PRESTypePluginKeyKind 
test_dds_Message_cPlugin_get_key_kind(void);

NDDSUSERDllExport extern unsigned int 
test_dds_Message_cPlugin_get_serialized_key_max_size_ex(
    PRESTypePluginEndpointData endpoint_data,
    RTIBool * overflow,
    RTIBool include_encapsulation,
    RTIEncapsulationId encapsulation_id,
    unsigned int current_alignment);

NDDSUSERDllExport extern unsigned int 
test_dds_Message_cPlugin_get_serialized_key_max_size(
    PRESTypePluginEndpointData endpoint_data,
    RTIBool include_encapsulation,
    RTIEncapsulationId encapsulation_id,
    unsigned int current_alignment);

NDDSUSERDllExport extern RTIBool 
test_dds_Message_cPlugin_serialize_key(
    PRESTypePluginEndpointData endpoint_data,
    const test_dds_Message_c *sample,
    struct RTICdrStream *stream,
    RTIBool serialize_encapsulation,
    RTIEncapsulationId encapsulation_id,
    RTIBool serialize_key,
    void *endpoint_plugin_qos);

NDDSUSERDllExport extern RTIBool 
test_dds_Message_cPlugin_deserialize_key_sample(
    PRESTypePluginEndpointData endpoint_data,
    test_dds_Message_c * sample,
    struct RTICdrStream *stream,
    RTIBool deserialize_encapsulation,
    RTIBool deserialize_key,
    void *endpoint_plugin_qos);

NDDSUSERDllExport extern RTIBool 
test_dds_Message_cPlugin_deserialize_key(
    PRESTypePluginEndpointData endpoint_data,
    test_dds_Message_c ** sample,
    RTIBool * drop_sample,
    struct RTICdrStream *stream,
    RTIBool deserialize_encapsulation,
    RTIBool deserialize_key,
    void *endpoint_plugin_qos);

NDDSUSERDllExport extern RTIBool
test_dds_Message_cPlugin_serialized_sample_to_key(
    PRESTypePluginEndpointData endpoint_data,
    test_dds_Message_c *sample,
    struct RTICdrStream *stream, 
    RTIBool deserialize_encapsulation,  
    RTIBool deserialize_key, 
    void *endpoint_plugin_qos);

NDDSUSERDllExport extern RTIBool 
test_dds_Message_cPlugin_instance_to_key(
    PRESTypePluginEndpointData endpoint_data,
    test_dds_Message_cKeyHolder *key, 
    const test_dds_Message_c *instance);

NDDSUSERDllExport extern RTIBool 
test_dds_Message_cPlugin_key_to_instance(
    PRESTypePluginEndpointData endpoint_data,
    test_dds_Message_c *instance, 
    const test_dds_Message_cKeyHolder *key);

NDDSUSERDllExport extern RTIBool 
test_dds_Message_cPlugin_instance_to_keyhash(
    PRESTypePluginEndpointData endpoint_data,
    DDS_KeyHash_t *keyhash,
    const test_dds_Message_c *instance);

NDDSUSERDllExport extern RTIBool 
test_dds_Message_cPlugin_serialized_sample_to_keyhash(
    PRESTypePluginEndpointData endpoint_data,
    struct RTICdrStream *stream, 
    DDS_KeyHash_t *keyhash,
    RTIBool deserialize_encapsulation,
    void *endpoint_plugin_qos); 

/* Plugin Functions */
NDDSUSERDllExport extern struct PRESTypePlugin*
test_dds_Message_cPlugin_new(void);

NDDSUSERDllExport extern void
test_dds_Message_cPlugin_delete(struct PRESTypePlugin *);

#if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport
#endif

#endif /* messageImplPlugin_874241052_h */

