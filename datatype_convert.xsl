<!-- Functions for reading/interpretting graphml xml -->
<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:cdit="http://github.com/cdit-ma/cdit"
    xmlns:o="http://github.com/cdit-ma/o"
    xmlns:graphml="http://github.com/cdit-ma/graphml"
    xmlns:cpp="http://github.com/cdit-ma/cpp"
    xmlns:cmake="http://github.com/cdit-ma/cmake"
    xmlns:proto="http://github.com/cdit-ma/proto"
    xmlns:idl="http://github.com/cdit-ma/idl"
    >

    <!--
        Gets the convert_h file
    -->
    <xsl:function name="cdit:get_convert_h">
        <xsl:param name="aggregate"/>
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="aggregate_namespace" select="graphml:get_data_value($aggregate, 'namespace')" />
        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        
        <xsl:variable name="middleware_type" select="cpp:get_aggregate_qualified_type($aggregate, $middleware)" />
        <xsl:variable name="base_type" select="cpp:get_aggregate_qualified_type($aggregate, 'base')" />
        <xsl:variable name="middleware_namespace" select="lower-case($middleware)" />
        
        <xsl:variable name="define_guard_name" select="upper-case(o:join_list(($middleware, $aggregate_namespace, $aggregate_label, 'convert'), '_'))" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_start($define_guard_name)" />

        <!-- Include the base message type -->
        <xsl:value-of select="cpp:comment('Include the base type', 0)" />
        <xsl:variable name="header_file" select="cdit:get_datatype_path('Base', $aggregate, concat(lower-case($aggregate_label), '.h'))"/>

        <xsl:value-of select="cpp:include_local_header($header_file)" />
        <xsl:value-of select="o:nl(1)" />
        <xsl:value-of select="cpp:forward_declare_class($aggregate_namespace, cpp:get_aggregate_type_name($aggregate), 0)" />
        <xsl:value-of select="o:nl(1)" />
        
        <!-- Define the namespace -->
        <xsl:value-of select="cpp:namespace_start($middleware_namespace, 0)" />

        <xsl:value-of select="cpp:comment(('Translate from', 'Base', '->', o:wrap_quote($middleware)), 1)" />
        <xsl:value-of select="cpp:declare_function(cpp:pointer_var_def($middleware_type, ''), 'translate', cpp:const_ref_var_def($base_type, 'value'), ';', 1)" />
        <xsl:value-of select="cpp:comment(('Translate from', o:wrap_quote($middleware), '->', 'Base'), 1)" />
        <xsl:value-of select="cpp:declare_function(cpp:pointer_var_def($base_type, ''), 'translate', cpp:const_ref_var_def($middleware_type, 'value'), ';', 1)" />
        
        <!-- Define the special string decode/encode functions only for protobuf -->
        <xsl:if test="lower-case($middleware) = 'proto'">
            <xsl:value-of select="o:nl(1)" />
            <xsl:value-of select="cpp:comment('Helper Functions', 1)" />
            <xsl:value-of select="cpp:declare_templated_function('class T', cpp:pointer_var_def($base_type, ''), 'decode', cpp:const_ref_var_def('std::string', 'value'), ';', 1)" />
            <xsl:value-of select="cpp:declare_function('std::string', 'encode', cpp:const_ref_var_def($base_type, 'value'), ';', 1)" />
            <xsl:value-of select="o:nl(1)" />
            <xsl:value-of select="cpp:comment('Forward declare the templated decode function with the concrete type', 1)" />
            <xsl:value-of select="cpp:declare_templated_function_specialisation($middleware_type, cpp:pointer_var_def($base_type, ''), 'decode', cpp:const_ref_var_def('std::string', 'value'), ';', 1)" />
        </xsl:if>

        <!-- End the namespace -->
        <xsl:value-of select="cpp:namespace_end($middleware_namespace, 0)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- End Define Guard -->
        <xsl:value-of select="cpp:define_guard_end($define_guard_name)" />
    </xsl:function>

    <!--
        Gets the convert_cpp file
    -->
    <xsl:function name="cdit:get_convert_cpp">
        <xsl:param name="aggregate" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="aggregate_namespace" select="graphml:get_data_value($aggregate, 'namespace')" />
        <xsl:variable name="middleware_namespace" select="cdit:get_middleware_namespace($middleware)" />

        <xsl:variable name="middleware_type" select="cpp:get_aggregate_qualified_type($aggregate, $middleware)" />
        <xsl:variable name="base_type" select="cpp:get_aggregate_qualified_type($aggregate, 'base')" />

        <!-- Get the definitions of the AggregateInstances used in this Aggregate -->
        <xsl:variable name="aggregate_instances" select="graphml:get_descendant_nodes_of_kind($aggregate, 'AggregateInstance')" />
        <xsl:variable name="aggregate_definitions" select="graphml:get_definitions($aggregate_instances)" />
        
        <!-- Include the header -->
        <xsl:value-of select="cpp:include_local_header('convert.h')" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include the middleware specific header -->
        <xsl:value-of select="cpp:comment(('Including', o:wrap_quote($middleware), 'generated header'), 0)" />
        <xsl:value-of select="cpp:include_local_header(cdit:get_middleware_generated_header_name($aggregate, $middleware))" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Include the middleware convert functions -->
        <xsl:for-each select="$aggregate_definitions">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Including required middleware convert functions', 0)" />
            </xsl:if>

            <xsl:variable name="header_file" select="cdit:get_datatype_path($middleware, ., 'convert.h')" />
            <xsl:value-of select="cpp:include_local_header($header_file)" />

            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>
        

        <xsl:value-of select="cpp:comment(('Translate from', 'base', '->', o:wrap_quote($middleware)), 0)" />
        <xsl:value-of select="cpp:define_function(cpp:pointer_var_def($middleware_type, ''), $middleware_namespace, 'translate', cpp:const_ref_var_def($base_type, 'value'), cpp:scope_start(0))" />
        <xsl:value-of select="cdit:get_translate_cpp($aggregate, $middleware, 'base', $middleware)" />
        <xsl:value-of select="cpp:scope_end(0)" />
        <xsl:value-of select="o:nl(1)" />
        

        <xsl:value-of select="cpp:comment(('Translate from', o:wrap_quote($middleware), '->', 'base'), 0)" />
        <xsl:value-of select="cpp:define_function(cpp:pointer_var_def($base_type, ''), $middleware_namespace, 'translate', cpp:const_ref_var_def($middleware_type, 'value'), cpp:scope_start(0))" />
        <xsl:value-of select="cdit:get_translate_cpp($aggregate, $middleware, $middleware, 'base')" />
        <xsl:value-of select="cpp:scope_end(0)" />
        <xsl:value-of select="o:nl(1)" />
        
        <!-- Define the special string decode/encode functions only for protobuf -->
        <xsl:if test="lower-case($middleware) = 'proto'">
            <xsl:value-of select="cpp:define_templated_function_specialisation($middleware_type, cpp:pointer_var_def($base_type, ''), $middleware_namespace, 'decode', cpp:const_ref_var_def('std::string', 'value'), cpp:scope_start(0))" />
            <xsl:value-of select="cpp:declare_variable($middleware_type, 'out', cpp:nl(), 1)" />
            <xsl:value-of select="concat(o:t(1), cpp:invoke_function('out', cpp:dot(), 'ParseFromString', 'value', 0), cpp:nl())" />
            <xsl:value-of select="cpp:return(cpp:invoke_function('', '', 'translate', 'out', 0), 1)" />
            <xsl:value-of select="cpp:scope_end(0)" />
            <xsl:value-of select="o:nl(1)" />

            <xsl:value-of select="cpp:define_function('std::string', $middleware_namespace, 'encode', cpp:const_ref_var_def($base_type, 'value'), cpp:scope_start(0))" />
            <xsl:value-of select="cpp:declare_variable('std::string', 'out', cpp:nl(), 1)" />
            <xsl:value-of select="cpp:define_variable('auto', 'pb', cpp:invoke_function('', '', cpp:combine_namespaces(($middleware_namespace, 'translate')), 'value', 0), cpp:nl(), 1)" />
            <xsl:value-of select="concat(o:t(1), cpp:invoke_function('pb', cpp:arrow(), 'SerializeToString', cpp:ref_var('out'), 0), cpp:nl())" />
            <xsl:value-of select="cpp:delete('pb', 1)" />
            <xsl:value-of select="cpp:return('out', 1)" />
            <xsl:value-of select="cpp:scope_end(0)" />
            <xsl:value-of select="o:nl(1)" />
        </xsl:if>
    </xsl:function>

    <xsl:function name="cdit:get_proto_file">
        <xsl:param name="aggregate" />

        <!-- Get the definitions of the AggregateInstances used in this Aggregate -->
        <xsl:variable name="aggregate_instances" select="graphml:get_descendant_nodes_of_kind($aggregate, 'AggregateInstance')" />
        <xsl:variable name="aggregate_definitions" select="graphml:get_definitions($aggregate_instances)" />

        <xsl:variable name="enum_instances" select="graphml:get_child_nodes_of_kind($aggregate, 'EnumInstance')" />
        <xsl:variable name="enums" select="graphml:get_definitions($enum_instances)" />
        
        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:variable name="proto_label" select="o:title_case($aggregate_label)" />


        <!-- Using Protobuf 3 -->
        <xsl:value-of select="proto:syntax('proto3')" />

        <!-- Import the definitions of each aggregate instance used -->
        <xsl:for-each select="$aggregate_definitions">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Import required .proto files', 0)" />
            </xsl:if>
            <xsl:variable name="required_proto_file" select="lower-case(o:join_list((graphml:get_label(.), 'proto'), '.'))" />
            <xsl:value-of select="proto:import($required_proto_file)" />
            
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>

        <!-- Declare the enums used -->
        <xsl:for-each select="$enums">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Declare enums', 0)" />
            </xsl:if>
            <xsl:variable name="enum_label" select="o:title_case(graphml:get_label(.))" />

            <xsl:value-of select="proto:enum($enum_label)" />

            <xsl:for-each select="graphml:get_child_nodes_of_kind(., 'EnumMember')">
                <xsl:variable name="member_label" select="upper-case(graphml:get_label(.))" />
                <xsl:value-of select="proto:enum_value($member_label, position() - 1)" />
            </xsl:for-each>

            <xsl:value-of select="cpp:scope_end(0)" />
            
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>

        <xsl:if test="$aggregate_namespace != ''">
            <xsl:value-of select="proto:package($aggregate_namespace)" />
        </xsl:if>

        <xsl:value-of select="proto:message($proto_label)" />
        
        <xsl:for-each select="graphml:get_child_nodes($aggregate)">
            <xsl:variable name="kind" select="graphml:get_kind(.)" />
            <xsl:variable name="index" select="graphml:get_index(.) + 1" />
            <xsl:variable name="label" select="lower-case(graphml:get_label(.))" />
            <xsl:variable name="type" select="graphml:get_type(.)" />
            <xsl:variable name="is_key" select="graphml:is_key(.)" />

            <xsl:choose>    
                <xsl:when test="$kind = 'Member'">
                    <xsl:variable name="cpp_type" select="cpp:get_primitive_type($type)" />
                    <xsl:variable name="proto_type" select="proto:get_type($cpp_type)" />
                    <xsl:value-of select="proto:member($proto_type, $label, $index)" />
                </xsl:when>
                <xsl:when test="$kind = 'EnumInstance'">
                    <xsl:variable name="enum_definition" select="graphml:get_definition(.)" />
                    <xsl:variable name="proto_type" select="o:title_case(graphml:get_label($enum_definition))" />
                    <xsl:value-of select="proto:member($proto_type, $label, $index)" />
                </xsl:when>
                <xsl:when test="$kind = 'AggregateInstance'">
                    <xsl:variable name="proto_type" select="proto:get_aggregate_qualified_type(graphml:get_definition(.))" />
                    <xsl:value-of select="proto:member($proto_type, $label, $index)" />
                </xsl:when>
                 <xsl:when test="$kind = 'Vector'">
                    <xsl:variable name="vector_child" select="graphml:get_vector_child(.)" />
                    <xsl:variable name="vector_child_kind" select="graphml:get_kind($vector_child)" />
                    <xsl:variable name="vector_child_type" select="graphml:get_type($vector_child)" />

                    <xsl:variable name="proto_type">
                        <xsl:choose>
                            <xsl:when test="$vector_child_kind = 'AggregateInstance'">
                                <xsl:value-of select="proto:get_aggregate_qualified_type(graphml:get_definition($vector_child))" />
                            </xsl:when>
                            <xsl:when test="$vector_child_kind = 'Member'">
                                <xsl:variable name="cpp_type" select="cpp:get_primitive_type($vector_child_type)" />
                                <xsl:value-of select="proto:get_type($cpp_type)" />
                            </xsl:when>
                        </xsl:choose>
                    </xsl:variable>
                    <xsl:value-of select="proto:repeated_member($proto_type, $label, $index)" />
                </xsl:when>
            </xsl:choose>
        </xsl:for-each>
        <xsl:value-of select="cpp:scope_end(0)" />
    </xsl:function>

    <xsl:function name="cdit:get_idl_file">
        <xsl:param name="aggregate" />

        <!-- Get the definitions of the AggregateInstances used in this Aggregate -->
        <xsl:variable name="aggregate_instances" select="graphml:get_descendant_nodes_of_kind($aggregate, 'AggregateInstance')" />
        <xsl:variable name="aggregate_definitions" select="graphml:get_definitions($aggregate_instances)" />

        <xsl:variable name="enum_instances" select="graphml:get_child_nodes_of_kind($aggregate, 'EnumInstance')" />
        <xsl:variable name="enums" select="graphml:get_definitions($enum_instances)" />
        
        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:variable name="label" select="o:title_case($aggregate_label)" />

        <xsl:variable name="define_guard_name" select="upper-case(o:join_list(($aggregate_namespace, $aggregate_label, 'IDL'), '_'))" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_start($define_guard_name)" />

        <!-- Import the definitions of each aggregate instance used -->
        <xsl:for-each select="$aggregate_definitions">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Import required .idl files', 0)" />
            </xsl:if>
            <xsl:variable name="required_proto_file" select="lower-case(o:join_list((graphml:get_label(.), 'idl'), '.'))" />
            <xsl:value-of select="idl:include($required_proto_file)" />
            
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>

        <!-- Declare the enums used -->
        <xsl:for-each select="$enums">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Declare enums', 0)" />
            </xsl:if>
            <xsl:variable name="enum_label" select="o:title_case(graphml:get_label(.))" />
            <xsl:value-of select="idl:enum($enum_label)" />

            <xsl:for-each select="graphml:get_child_nodes_of_kind(., 'EnumMember')">
                <xsl:variable name="member_label" select="upper-case(graphml:get_label(.))" />
                <xsl:value-of select="idl:enum_value($member_label, position() = last())" />
            </xsl:for-each>

            <xsl:value-of select="cpp:scope_end(0)" />
            
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>

        <xsl:variable name="tab" select="if($aggregate_namespace = '') then 0 else 1" />

        <xsl:if test="$aggregate_namespace != ''">
            <xsl:value-of select="idl:module($aggregate_namespace)" />
        </xsl:if>

        <xsl:value-of select="idl:struct($label, $tab)" />
        
        <xsl:for-each select="graphml:get_child_nodes($aggregate)">
            <xsl:variable name="kind" select="graphml:get_kind(.)" />
            <xsl:variable name="index" select="graphml:get_index(.) + 1" />
            <xsl:variable name="label" select="lower-case(graphml:get_label(.))" />
            <xsl:variable name="type" select="graphml:get_type(.)" />
            <xsl:variable name="is_key" select="graphml:is_key(.)" />

            <xsl:choose>    
                <xsl:when test="$kind = 'Member'">
                    <xsl:variable name="cpp_type" select="cpp:get_primitive_type($type)" />
                    <xsl:variable name="idl_type" select="idl:get_type($cpp_type)" />
                    <xsl:value-of select="idl:member($idl_type, $label, $is_key, $tab + 1)" />
                </xsl:when>
                <xsl:when test="$kind = 'EnumInstance'">
                    <xsl:variable name="enum_definition" select="graphml:get_definition(.)" />
                    <xsl:variable name="idl_type" select="o:title_case(graphml:get_label($enum_definition))" />
                    <xsl:value-of select="idl:member($idl_type, $label, $is_key, $tab + 1)" />
                </xsl:when>
                <xsl:when test="$kind = 'AggregateInstance'">
                    <xsl:variable name="idl_type" select="idl:get_aggregate_qualified_type(graphml:get_definition(.))" />
                    <xsl:value-of select="idl:member($idl_type, $label, $is_key, $tab + 1)" />
                </xsl:when>
                 <xsl:when test="$kind = 'Vector'">
                    <xsl:variable name="vector_child" select="graphml:get_vector_child(.)" />
                    <xsl:variable name="vector_child_kind" select="graphml:get_kind($vector_child)" />
                    <xsl:variable name="vector_child_type" select="graphml:get_type($vector_child)" />

                    <xsl:variable name="idl_type">
                        <xsl:choose>
                            <xsl:when test="$vector_child_kind = 'AggregateInstance'">
                                <xsl:value-of select="idl:get_aggregate_qualified_type(graphml:get_definition($vector_child))" />
                            </xsl:when>
                            <xsl:when test="$vector_child_kind = 'Member'">
                                <xsl:variable name="cpp_type" select="cpp:get_primitive_type($vector_child_type)" />
                                <xsl:value-of select="idl:get_type($cpp_type)" />
                            </xsl:when>
                        </xsl:choose>
                    </xsl:variable>
                    <xsl:value-of select="idl:sequence_member($idl_type, $label, $is_key, $tab + 1)" />
                </xsl:when>
            </xsl:choose>
        </xsl:for-each>
        
        <xsl:value-of select="cpp:scope_end($tab)" />

        <!-- Set the pragma of the keys -->
        <xsl:for-each select="graphml:get_child_nodes($aggregate)">
            <xsl:variable name="is_key" select="graphml:is_key(.)" />
            <xsl:variable name="child_label" select="lower-case(graphml:get_label(.))" />
            <xsl:if test="$is_key">
                <xsl:value-of select="idl:key_pragma($label, $child_label, $tab)" />
            </xsl:if>
        </xsl:for-each>

        <xsl:if test="$aggregate_namespace != ''">
            <xsl:value-of select="cpp:scope_end(0)" />
        </xsl:if>

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_end($define_guard_name)" />
    </xsl:function>


    <!--
        Gets the convert_cmake file
    -->
    <xsl:function name="cdit:get_convert_cmake">
        <xsl:param name="aggregate"/>
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />

        <!-- Get all required aggregates -->
        <xsl:variable name="required_aggregates" select="cdit:get_required_aggregates($aggregate)" />
        
        <xsl:variable name="middleware_type" select="cpp:get_aggregate_qualified_type($aggregate, $middleware)" />
        <xsl:variable name="base_type" select="cpp:get_aggregate_qualified_type($aggregate, 'base')" />
        <xsl:variable name="middleware_namespace" select="lower-case($middleware)" />

        
        <xsl:variable name="module_lib_name" select="cmake:get_aggregates_middleware_module_library_name($aggregate, $middleware)" />
        <xsl:variable name="shared_lib_name" select="cmake:get_aggregates_middleware_shared_library_name($aggregate, $middleware)" />
        
        <xsl:variable name="proj_name" select="$module_lib_name" />
        <xsl:variable name="base_lib_name" select="cmake:get_aggregates_middleware_shared_library_name($aggregate, 'base')" />

        <xsl:variable name="middleware_sources" select="cmake:get_middleware_generated_source_var($middleware)" />
        <xsl:variable name="middleware_headers" select="cmake:get_middleware_generated_header_var($middleware)" />
        
        <xsl:variable name="middleware_helper_libraries" select="concat(upper-case($middleware), '_HELPER_LIBRARIES')" />
        <xsl:variable name="middleware_extension" select="cmake:get_middleware_extension($middleware)" />

        <xsl:variable name="build_module_library" select="cdit:build_module_library($middleware) " />
        <xsl:variable name="build_shared_library" select="cdit:build_shared_library($middleware)" />

        <xsl:variable name="binary_dir_var" select=" cmake:current_binary_dir_var()" />
        <xsl:variable name="source_dir_var" select=" cmake:current_source_dir_var()" />

        <xsl:variable name="relative_path" select="cmake:get_relative_path(($middleware, $aggregate_namespace, $aggregate_label))" />

        <xsl:value-of select="cmake:set_project_name($proj_name)" />

        <!-- Find the Middleware specific package -->
        <xsl:value-of select="cmake:find_middleware_package($middleware)" />

        <!-- Find re_core -->
        <xsl:value-of select="cmake:find_re_core_library()" />

        <!-- Find the middleware helper -->
        <xsl:if test="$build_module_library">
            <xsl:value-of select="cmake:find_library(concat($middleware, '_helper'), $middleware_helper_libraries , cmake:get_re_path('lib'))" />
            <xsl:value-of select="o:nl(1)" />
        </xsl:if>

        <!-- Do the things to generate required files for the middleware shared library-->
        <xsl:if test="$build_shared_library">
            <xsl:value-of select="cmake:set_variable('SHARED_LIBRARY_NAME', $shared_lib_name, 0)" />
            <xsl:value-of select="cmake:comment(('Copy the', o:wrap_angle($middleware_extension), 'file into the binary directory so it can be used by the middleware compilers'), 0)" />

            <xsl:variable name="middleware_file" select="cmake:get_aggregates_middleware_file_name($aggregate, $middleware)" />

            <xsl:value-of select="cmake:configure_file($middleware_file, $binary_dir_var)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- Include the required aggregate files -->
            <xsl:for-each select="$required_aggregates">
                <xsl:if test="position() = 1">
                    <xsl:value-of select="cmake:comment(('Copy imported', o:wrap_angle($middleware_extension), 'files into the binary directory so it can be used by the middleware compilers'), 0)" />
                </xsl:if>
                <xsl:variable name="required_aggregate_label" select="graphml:get_label(.)" />
                <xsl:variable name="required_aggregate_namespace" select="graphml:get_data_value(., 'namespace')" />

                
                <xsl:variable name="relative_path" select="cmake:get_relative_path(($aggregate_namespace, $aggregate_label))" />
                <xsl:variable name="required_file" select="o:join_paths(($relative_path, cmake:get_aggregates_middleware_file_path(., $middleware)))" />
                <xsl:value-of select="cmake:configure_file($required_file, $binary_dir_var)" />

                <xsl:if test="position() = last()">
                    <xsl:value-of select="o:nl(1)" />
                </xsl:if>
            </xsl:for-each>

            <!-- Run the middlewares compiler over the middleware artefacts-->
            <xsl:value-of select="cmake:generate_middleware_compiler(o:join_paths(($binary_dir_var, $middleware_file)), $middleware)" />
            <xsl:value-of select="o:nl(1)" />

            <!-- Set Source files -->
            <xsl:value-of select="concat('set(SOURCE', o:nl(1))" />
            <xsl:value-of select="concat(o:t(1), $source_dir_var, '/convert.cpp', o:nl(1))" />
            <xsl:value-of select="concat(o:t(0), ')', o:nl(1))" />
            <xsl:value-of select="o:nl(1)" />

            <!-- Set Header files -->
            <xsl:value-of select="concat('set(HEADERS', o:nl(1))" />
            <xsl:value-of select="concat(o:t(1), $source_dir_var, '/convert.h', o:nl(1))" />
            <xsl:value-of select="concat(o:t(0), ')', o:nl(1))" />
            <xsl:value-of select="o:nl(1)" />
        </xsl:if>

        <xsl:if test="$build_shared_library">
            <xsl:variable name="args" select="o:join_list((cmake:wrap_variable('SOURCE'), cmake:wrap_variable('HEADERS'), cmake:wrap_variable($middleware_sources), cmake:wrap_variable($middleware_headers)), ' ')" />
            <xsl:value-of select="cmake:add_shared_library('SHARED_LIBRARY_NAME', 'SHARED', $args)" />
        </xsl:if>
        <xsl:if test="$build_module_library">
           <xsl:value-of select="cmake:add_shared_library('PROJ_NAME', 'MODULE', o:join_paths(($source_dir_var, 'libportexport.cpp')))" />
        </xsl:if>

        
        <xsl:if test="$build_shared_library">
            <!-- Include re_path -->
            <xsl:value-of select="cmake:comment('Include the runtime environment directory', 0)" />
            <xsl:value-of select="cmake:target_include_directories('SHARED_LIBRARY_NAME', cmake:get_re_path('src'), 0)" />
            <xsl:value-of select="cmake:comment('Include the middleware include directory', 0)" />
            <xsl:value-of select="cmake:target_include_middleware_directories('SHARED_LIBRARY_NAME', $middleware, 0)" />
            <xsl:value-of select="cmake:comment('Include the current binary directory to allow inclusion of generated files', 0)" />
            <xsl:value-of select="cmake:target_include_directories('SHARED_LIBRARY_NAME', $binary_dir_var, 0)" />
            <xsl:value-of select="o:nl(1)" />

            

            <!-- Include the required aggregate files -->
            <xsl:value-of select="cmake:comment('Include required aggregates source dirs', 0)" />
            <xsl:variable name="required_path" select="o:join_paths(($source_dir_var, $relative_path))" />
            <xsl:value-of select="cmake:target_include_directories('SHARED_LIBRARY_NAME', $required_path, 0)" />
            

            <!-- Use Windows specific settings -->
            <xsl:if test="$middleware = 'proto'">

                <!-- Include the required aggregate files -->
                <xsl:for-each select="$required_aggregates">
                    <xsl:if test="position() = 1">
                        <xsl:value-of select="cmake:comment(('Include aggregate'), 0)" />
                    </xsl:if>

                    <xsl:variable name="required_aggregate_label" select="graphml:get_label(.)" />
                    <xsl:variable name="required_aggregate_namespace" select="graphml:get_data_value(., 'namespace')" />

                    
                    <xsl:variable name="relative_path" select="cmake:get_relative_path(($aggregate_namespace, $aggregate_label))" />
                    <xsl:variable name="required_file" select="o:join_paths(($binary_dir_var, $relative_path, cdit:get_aggregates_path(.)))" />
                    <xsl:value-of select="cmake:target_include_directories('SHARED_LIBRARY_NAME', $required_file, 0)" />

                    <xsl:if test="position() = last()">
                        <xsl:value-of select="o:nl(1)" />
                    </xsl:if>
                </xsl:for-each>

                <xsl:value-of select="cmake:comment('Windows specific protobuf settings', 0)" />
                <xsl:value-of select="cmake:if_start('MSVC', 0)" />
                <xsl:value-of select="cmake:target_compile_definitions('SHARED_LIBRARY_NAME', '-DPROTOBUF_USE_DLLS', 1)" />
                <xsl:value-of select="cmake:if_end('MSVC', 0)" />
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>

            <xsl:value-of select="cmake:comment('Link against runtime environment', 0)" />
            <xsl:value-of select="cmake:target_link_libraries('SHARED_LIBRARY_NAME', cmake:wrap_variable('RE_CORE_LIBRARIES'), 0)" />
            <xsl:value-of select="cmake:comment('Link against the middleware libraries', 0)" />
            <xsl:value-of select="cmake:target_link_middleware_libraries('SHARED_LIBRARY_NAME', $middleware, 0)" />

            <xsl:if test="$middleware != 'base'">
                <xsl:value-of select="cmake:comment('Link against the base aggregate', 0)" />
                <xsl:value-of select="cmake:target_link_libraries('SHARED_LIBRARY_NAME', $base_lib_name, 0)" />
            </xsl:if>

            
             <!-- Include the required aggregate files -->
            <xsl:for-each select="$required_aggregates">
                <xsl:if test="position() = 1">
                    <xsl:value-of select="cmake:comment(('Link against', o:wrap_angle($middleware_extension), 'libraries'), 0)" />
                </xsl:if>
                <xsl:variable name="required_lib" select="cmake:get_aggregates_middleware_shared_library_name(., $middleware)" />
                <xsl:value-of select="cmake:target_link_libraries('SHARED_LIBRARY_NAME', $required_lib, 0)" />
                <xsl:if test="position() = last()">
                    <xsl:value-of select="o:nl(1)" />
                </xsl:if>
            </xsl:for-each>


        </xsl:if>
        <xsl:if test="$build_module_library">
            <!-- Include re_path -->
            <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', cmake:get_re_path('src'), 0)" />

            <xsl:variable name="required_source_dir" select="o:join_paths(($source_dir_var, $relative_path))" />
            <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', $required_source_dir, 0)" />
            <xsl:value-of select="cmake:target_include_middleware_directories('PROJ_NAME', $middleware, 0)" />

            

            <xsl:value-of select="cmake:target_link_middleware_libraries('PROJ_NAME', $middleware, 0)" />
            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', cmake:wrap_variable('RE_CORE_LIBRARIES'), 0)" />
            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', cmake:wrap_variable($middleware_helper_libraries), 0)" />
            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', $shared_lib_name, 0)" />
    
            
            <xsl:value-of select="o:nl(1)" />
        </xsl:if>
    </xsl:function>

    <xsl:function name="cdit:get_aggregate_base_h">
        <xsl:param name="aggregate" as="element()" />

        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="class_name" select="o:title_case($aggregate_label)" />
        <xsl:variable name="tab" select="if($aggregate_namespace = '') then 0 else 1" />

        <xsl:variable name="relative_path" select="cmake:get_relative_path(($aggregate_namespace, $aggregate_label))" />
        

        <!-- Get all required aggregates -->
        <xsl:variable name="required_aggregates" select="cdit:get_required_aggregates($aggregate)" />

        <xsl:variable name="define_guard_name" select="upper-case(o:join_list(('base', $aggregate_namespace, $aggregate_label), '_'))" />

        <xsl:variable name="children" select="graphml:get_child_nodes($aggregate)" />
        <xsl:variable name="enums" select="graphml:get_definitions(graphml:get_child_nodes_of_kind($aggregate, 'EnumInstance'))" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_start($define_guard_name)" />
        
        <!-- Library Includes-->
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('core', 'basemessage.h')))" />
        <xsl:value-of select="cpp:include_library_header('string')" />

        <!-- Include Vector -->
        <xsl:if test="count(graphml:get_child_nodes_of_kind($aggregate, 'Vector')) > 0">
            <xsl:value-of select="cpp:include_library_header('vector')" />
        </xsl:if>

        <!-- Import the definitions of each aggregate instance used -->
        <xsl:for-each select="$required_aggregates">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Include required base Aggregate header files', 0)" />
            </xsl:if>
            <xsl:variable name="required_file" select="cdit:get_base_aggregate_h_path(.)" />
            <xsl:value-of select="cpp:include_local_header(o:join_paths(($relative_path, $required_file)))" />
            
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>

        <!-- Import the definitions of each enum instances used -->
        <xsl:for-each select="$enums">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cpp:comment('Include required enum header files', 0)" />
            </xsl:if>
            <xsl:variable name="required_file" select="cdit:get_base_enum_h_path(.)" />
            <xsl:value-of select="cpp:include_local_header(o:join_paths(($relative_path, $required_file)))" />
            
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>

        

        <!-- Define Namespaces -->
        <xsl:value-of select="cpp:namespace_start('Base', 0)" />
        <xsl:if test="$aggregate_namespace != ''">
            <xsl:value-of select="cpp:namespace_start($aggregate_namespace, $tab)" />
        </xsl:if>
        
        <xsl:value-of select="cpp:declare_class($class_name, 'public ::BaseMessage', $tab + 1)" />

        
        
        <!-- Define functions -->
        <xsl:for-each select="$children">
            <xsl:value-of select="cdit:declare_datatype_functions(., $tab + 1)" />
        </xsl:for-each>

        
        
        <xsl:value-of select="cpp:scope_end($tab + 1)" />

        <!-- End Namespaces -->
        <xsl:if test="$aggregate_namespace != ''">
            <xsl:value-of select="cpp:namespace_end($aggregate_namespace, $tab)" />
        </xsl:if>
        <xsl:value-of select="cpp:namespace_end('Base', 0)" />

        <xsl:value-of select="cpp:define_guard_end($define_guard_name)" />
    </xsl:function>

    

    <xsl:function name="cdit:get_port_export">
        <xsl:param name="aggregate" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('core', 'libportexport.h')))" />
        <xsl:value-of select="o:nl(1)" />

        
        <xsl:variable name="convert_header_path">
            <xsl:if test="cdit:middleware_uses_protobuf($middleware)">
                <xsl:value-of select="o:join_paths(('proto', cdit:get_aggregates_path($aggregate)))" />
            </xsl:if>
        </xsl:variable>

        <xsl:value-of select="cpp:comment(('Include the convert function'), 0)" />
        <xsl:value-of select="cpp:include_local_header(o:join_paths(($convert_header_path, 'convert.h')))" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:value-of select="cpp:comment(('Include the', $middleware, 'specific templated classes'), 0)" />
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('middleware', $middleware, 'ineventport.hpp')))" />
        <xsl:value-of select="cpp:include_library_header(o:join_paths(('middleware', $middleware, 'outeventport.hpp')))" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:variable name="base_type" select="cpp:get_aggregate_qualified_type($aggregate, 'base')" />
        <xsl:variable name="middleware_type" select="cpp:get_aggregate_qualified_type($aggregate, $middleware)" />
        <xsl:variable name="port_type" select="cpp:join_args(($base_type, $middleware_type))" />

        <xsl:variable name="func_args" select="cpp:join_args((cpp:const_ref_var_def('std::string', 'port_name'), cpp:declare_variable(cpp:weak_ptr('Component'), 'component', '', 0)))" />


        <xsl:variable name="inport_type" select="cpp:templated_type(cpp:combine_namespaces(($middleware, 'InEventPort')), $port_type)" />
        <xsl:variable name="outport_type" select="cpp:templated_type(cpp:combine_namespaces(($middleware, 'OutEventPort')), $port_type)" />

        <xsl:value-of select="cpp:define_function(cpp:pointer_var_def('EventPort', ''), '', 'ConstructInEventPort', $func_args, cpp:scope_start(0))" />
        <xsl:variable name="inport_func" select="cpp:invoke_templated_static_function($inport_type, 'ConstructInEventPort', cpp:join_args(('port_name', 'component')), '', 0)" />
        <xsl:value-of select="cpp:return($inport_func, 1)" />
        <xsl:value-of select="cpp:scope_end(0)" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:value-of select="cpp:define_function(cpp:pointer_var_def('EventPort', ''), '', 'ConstructOutEventPort', $func_args, cpp:scope_start(0))" />
        <xsl:variable name="outport_func" select="cpp:invoke_templated_static_function($outport_type, 'ConstructOutEventPort', cpp:join_args(('port_name', 'component')), '', 0)" />
        <xsl:value-of select="cpp:return($outport_func, 1)" />
        <xsl:value-of select="cpp:scope_end(0)" />
    </xsl:function>


    <xsl:function name="cdit:get_aggregate_base_cpp">
        <xsl:param name="aggregate" as="element()" />

        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="class_type" select="cpp:get_aggregate_qualified_type($aggregate, 'base')" />
        
        <xsl:variable name="header_file" select="cdit:get_base_aggregate_h_name($aggregate)" />
        <xsl:value-of select="cpp:include_local_header($header_file)" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Define functions -->
        <xsl:for-each select="graphml:get_child_nodes($aggregate)">
            <xsl:value-of select="cdit:define_datatype_functions(., $class_type)" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:get_aggregate_base_cmake">
        <xsl:param name="aggregate" as="element()" />

        <xsl:variable name="aggregate_label" select="graphml:get_label($aggregate)" />
        <xsl:variable name="aggregate_namespace" select="graphml:get_namespace($aggregate)" />
        
        <xsl:variable name="class_type" select="cpp:get_aggregate_qualified_type($aggregate, 'base')" />
        
        <xsl:variable name="aggregate_h" select="concat(lower-case($aggregate_label), '.h')" />
        <xsl:variable name="aggregate_cpp" select="concat(lower-case($aggregate_label), '.cpp')" />

        <xsl:variable name="shared_lib_name" select="cmake:get_aggregates_middleware_shared_library_name($aggregate, 'base')" />
        <xsl:variable name="proj_name" select="$shared_lib_name" />

        <xsl:variable name="binary_dir_var" select=" cmake:current_binary_dir_var()" />
        <xsl:variable name="source_dir_var" select=" cmake:current_source_dir_var()" />

        <xsl:value-of select="cmake:set_project_name($proj_name)" />

        <!-- Find re_core -->
        <xsl:value-of select="cmake:find_re_core_library()" />

         <!-- Set Source files -->
        <xsl:value-of select="concat('set(SOURCE', o:nl(1))" />
        <xsl:value-of select="concat(o:t(1), o:join_paths(($source_dir_var, $aggregate_cpp)), o:nl(1))" />
        <xsl:value-of select="concat(o:t(0), ')', o:nl(1))" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Set Header files -->
        <xsl:value-of select="concat('set(HEADERS', o:nl(1))" />
        <xsl:value-of select="concat(o:t(1), o:join_paths(($source_dir_var, $aggregate_h)), o:nl(1))" />
        <xsl:value-of select="concat(o:t(0), ')', o:nl(1))" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:variable name="args" select="o:join_list((cmake:wrap_variable('SOURCE'), cmake:wrap_variable('HEADERS')), ' ')" />
        <xsl:value-of select="cmake:add_shared_library('PROJ_NAME', 'SHARED', $args)" />
        <xsl:value-of select="o:nl(1)" />

            
        


        <xsl:value-of select="cmake:comment('Include the runtime environment directory', 0)" />
        <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', cmake:get_re_path('src'), 0)" />

        <xsl:value-of select="cmake:comment('Link against runtime environment', 0)" />
        <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', cmake:wrap_variable('RE_CORE_LIBRARIES'), 0)" />
        <xsl:value-of select="o:nl(1)" />

        <xsl:variable name="relative_path" select="cmake:get_relative_path(($aggregate_namespace, $aggregate_label))" />
        <xsl:variable name="required_aggregates" select="cdit:get_required_aggregates($aggregate)" />

        <!-- Include the required aggregate files -->
        <xsl:if test="count($required_aggregates) > 0">
            <xsl:value-of select="cmake:comment('Include required aggregates source dirs', 0)" />
            <xsl:variable name="required_path" select="o:join_paths(($source_dir_var, $relative_path))" />
            <xsl:value-of select="cmake:target_include_directories('PROJ_NAME', $required_path, 0)" />
            <xsl:value-of select="o:nl(1)" />
        </xsl:if>

        <!-- Include the required aggregate files -->
        <xsl:for-each select="$required_aggregates">
            <xsl:if test="position() = 1">
                <xsl:value-of select="cmake:comment('Link against required aggregates libraries', 0)" />
            </xsl:if>
            <xsl:variable name="required_lib_name" select="cmake:get_aggregates_middleware_shared_library_name(., 'base')" />
            <xsl:value-of select="cmake:target_link_libraries('PROJ_NAME', $required_lib_name, 0)" />
            <xsl:if test="position() = last()">
                <xsl:value-of select="o:nl(1)" />
            </xsl:if>
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:get_middleware_cmake">
        <xsl:param name="aggregates" as="element()*" />

        <!-- Include the required aggregate folders -->
        <xsl:variable name="sub_directories" as="xs:string*">
            <xsl:for-each select="$aggregates">
                <xsl:sequence select="cdit:get_aggregates_path(.)" />
            </xsl:for-each>
        </xsl:variable>

        <xsl:value-of select="cmake:add_subdirectories($sub_directories)" />
    </xsl:function>

    <xsl:function name="cdit:get_datatypes_cmake">
        <xsl:param name="middlewares" as="xs:string*" />
        
        <xsl:value-of select="cmake:cmake_minimum_required('3.1')" />
        <xsl:value-of select="cmake:set_cpp11()" />
        <xsl:value-of select="cmake:setup_re_path()" />

        <xsl:variable name="lib_dir" select="o:join_paths((cmake:current_source_dir_var(),'..', 'lib'))" />

        <xsl:value-of select="cmake:set_library_output_directory($lib_dir)" />
        <xsl:value-of select="cmake:set_archive_output_directory($lib_dir)" />
        
        <xsl:value-of select="cmake:add_subdirectories($middlewares)" />
    </xsl:function>

    

    <xsl:function name="cdit:get_enum_h">
        <xsl:param name="enum" as="element()" />
        
        <xsl:variable name="namespaces" select="o:trim_list(('Base', graphml:get_namespace($enum)))" />
        <xsl:variable name="label" select="graphml:get_label($enum)" />
        <xsl:variable name="tab" select="count($namespaces)" />
        <xsl:variable name="qualified_type" select="cpp:combine_namespaces(($namespaces, $label))" />

        <xsl:variable name="define_guard_name" select="upper-case(o:join_list(('ENUMS', $namespaces, $label), '_'))" />

        <xsl:variable name="enum_members" select="graphml:get_child_nodes_of_kind($enum, 'EnumMember')" />

        <!-- Define Guard -->
        <xsl:value-of select="cpp:define_guard_start($define_guard_name)" />
        <xsl:value-of select="cpp:include_library_header('string')" />
        <xsl:value-of select="o:nl(1)" />

        <!-- Define the namespaces -->
        <xsl:for-each select="$namespaces">
            <xsl:value-of select="cpp:namespace_start(., position() - 1)" />
        </xsl:for-each>

        <!-- Define the Enum -->
        <xsl:value-of select="cpp:enum($label, $tab)" />
        <xsl:for-each select="$enum_members">
            <xsl:variable name="member_label" select="upper-case(graphml:get_label(.))" />
            <xsl:value-of select="cpp:enum_value($member_label, position() - 1, position() = last(), $tab + 1)" />
        </xsl:for-each>
        <xsl:value-of select="cpp:scope_end($tab)" />

        <!-- End the namespaces -->
        <xsl:for-each select="$namespaces">
            <xsl:value-of select="cpp:namespace_end(., position() - 1)" />
        </xsl:for-each>

        <!-- Define to_string function -->
        <xsl:value-of select="o:nl(1)" />
        <xsl:value-of select="cpp:define_function('inline std::string', '', 'to_string', cpp:const_ref_var_def($qualified_type, 'value'), cpp:scope_start(0))" />
        <xsl:value-of select="cpp:switch('value', 1)" />

        <xsl:for-each select="$enum_members">
            <xsl:variable name="enum_type" select="cdit:get_resolved_enum_member_type(.)" />
            <xsl:value-of select="cpp:switch_case($enum_type, 2)" />
            <xsl:value-of select="cpp:return(o:wrap_dblquote($enum_label), 3)" />
            <xsl:value-of select="cpp:scope_end(2)" />
        </xsl:for-each>
        <xsl:value-of select="cpp:switch_default(2)" />
        <xsl:value-of select="cpp:return(o:wrap_dblquote('UNKNOWN_TYPE'), 3)" />
        <xsl:value-of select="cpp:scope_end(2)" />
        <xsl:value-of select="cpp:scope_end(1)" />
        <xsl:value-of select="cpp:scope_end(0)" />
        
        <!-- Define to_integer function -->
        <xsl:value-of select="o:nl(1)" />
        <xsl:value-of select="cpp:define_function('inline int', '', 'to_integer', cpp:const_ref_var_def($qualified_type, 'value'), cpp:scope_start(0))" />
        <xsl:value-of select="cpp:return(cpp:static_cast('int', 'value'), 1)" />
        <xsl:value-of select="cpp:scope_end(0)" />

        <xsl:value-of select="cpp:define_guard_end($define_guard_name)" />
    </xsl:function>

    <xsl:function name="cpp:declare_aggregate_member_functions">
        <xsl:param name="node" as="element()" />
        <xsl:param name="tab" />

        <xsl:variable name="label" select="graphml:get_label($node)" />
        <xsl:variable name="kind" select="graphml:get_kind($node)" />
        <xsl:variable name="cpp_type" select="cpp:get_qualified_type($node)" />
        <xsl:variable name="var_label" select="cdit:get_variable_label($node)" />

        <xsl:value-of select="cpp:comment((o:wrap_square($kind), ':', $label, o:wrap_angle(graphml:get_id($node))), $tab)" />
        <xsl:choose>
            <xsl:when test="$cpp_type != ''">
                <!-- Public Declarations -->
                <xsl:value-of select="cpp:public($tab)" />
                <xsl:value-of select="cpp:declare_function('void', concat('set_', $label), cpp:const_ref_var_def($cpp_type, 'value'), ';', $tab + 1)" />
                <xsl:value-of select="cpp:declare_function(cpp:const_ref_var_def($cpp_type, ''), concat('get_', $label), '', ' const;', $tab + 1)" />
                <xsl:value-of select="cpp:declare_function(cpp:ref_var_def($cpp_type, ''), $label, '', ';', $tab + 1)" />
                <!-- Private Declarations -->
                <xsl:value-of select="cpp:private($tab)" />
                <xsl:value-of select="cpp:declare_variable($cpp_type, $var_label, cpp:nl(), $tab + 1)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cpp:comment('Cannot find valid CPP type for this element', $tab)" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>



    <xsl:function name="cdit:translate_member">
        <xsl:param name="member" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="source_middleware" as="xs:string" />
        <xsl:param name="target_middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="get_func" select="cdit:invoke_middleware_get_function('value', cpp:dot(), $member, $source_middleware)" />
        <xsl:variable name="set_func" select="cdit:invoke_middleware_set_function('out', cpp:arrow(), $member, $target_middleware, $get_func)" />

        <xsl:value-of select="concat(o:t($tab), $set_func, cpp:nl())" />
    </xsl:function>

    <xsl:function name="cdit:translate_enum_instance">
        <xsl:param name="enum_instance" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="source_middleware" as="xs:string" />
        <xsl:param name="target_middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="aggregate" select="graphml:get_parent_node($enum_instance)" />
        
        <!-- The namespace of the enum is dependant on its target middleware -->
        <xsl:variable name="enum_namespace">
            <xsl:choose>
                <xsl:when test="$target_middleware = 'base'">
                    <!-- All Enums exist in the Base namespace -->
                    <xsl:value-of select="'Base'" />
                </xsl:when>
                <xsl:otherwise>
                    <!-- Enums are defined in the generated namespace for the middlewares -->
                    <xsl:value-of select="graphml:get_data_value($aggregate, 'namespace')" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <!-- Get the type of the Enum -->
        <xsl:variable name="get_func" select="cdit:invoke_middleware_get_function('value', cpp:dot(), $enum_instance, $source_middleware)" />
        <xsl:variable name="enum_cast_type" select="cpp:get_enum_qualified_type($enum_instance, $target_middleware)" />

        <xsl:variable name="cast_enum_value" select="o:join_list((o:wrap_bracket($enum_cast_type), cpp:static_cast('int', $get_func)), ' ')" />
        <xsl:variable name="set_func" select="cdit:invoke_middleware_set_function('out', cpp:arrow(), $enum_instance, $target_middleware, $cast_enum_value)" />
        
        <xsl:value-of select="cpp:scope_start($tab)" />
        <xsl:value-of select="cpp:comment('Cast the enums integer representation', $tab + 1)" />
        <xsl:value-of select="concat(o:t($tab + 1), $set_func, cpp:nl())" />
        <xsl:value-of select="cpp:scope_end($tab)" />
    </xsl:function>

    <xsl:function name="cdit:translate_aggregate_instance">
        <xsl:param name="aggregate_instance" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="source_middleware" as="xs:string" />
        <xsl:param name="target_middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />


        <xsl:variable name="middleware_namespace" select="cdit:get_middleware_namespace($middleware)" />

        <xsl:variable name="get_func" select="cdit:invoke_middleware_get_function('value', cpp:dot(), $aggregate_instance, $source_middleware)" />
        <xsl:variable name="temp_variable" select="lower-case(concat('value_', graphml:get_label($aggregate_instance)))" />
        <xsl:variable name="translate_func" select="cpp:invoke_function('', '', cpp:combine_namespaces(($middleware_namespace, 'translate')), $get_func, 0)" />

        <xsl:variable name="value">
            <xsl:choose>
                <xsl:when test="$target_middleware = 'proto'">
                    <!-- Protobuf has a swap function which takes a pointer -->
                    <xsl:value-of select="$temp_variable" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="concat('*', $temp_variable)" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>

        <xsl:variable name="set_func" select="cdit:invoke_middleware_set_function('out', cpp:arrow(), $aggregate_instance, $target_middleware, $value)" />

        
        <xsl:value-of select="cpp:scope_start($tab)" />
        <xsl:value-of select="cpp:comment(('Member', o:wrap_square(graphml:get_kind($aggregate_instance)), 'Type', o:wrap_angle(graphml:get_type($aggregate_instance))), $tab + 1)" />
        <xsl:value-of select="cpp:comment('Set and cleanup translated Aggregate', $tab + 1)" />
        <xsl:value-of select="cpp:define_variable(cpp:auto(), $temp_variable, $translate_func, cpp:nl(), $tab + 1)" />

        <xsl:value-of select="concat(o:t($tab + 1), $set_func, cpp:nl())" />
        <xsl:value-of select="cpp:delete($temp_variable, $tab + 1)" />
        <xsl:value-of select="cpp:scope_end($tab)" />
    </xsl:function>

    <xsl:function name="cdit:translate_vector">
        <xsl:param name="vector" as="element()" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="source_middleware" as="xs:string" />
        <xsl:param name="target_middleware" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="middleware_namespace" select="cdit:get_middleware_namespace($middleware)" />
        <xsl:variable name="vector_child" select="graphml:get_vector_child($vector)" />
        <xsl:variable name="vector_child_type" select="graphml:get_type($vector_child)" />
        <xsl:variable name="vector_child_kind" select="graphml:get_kind($vector_child)" />

        <xsl:variable name="get_func" select="cdit:invoke_middleware_get_function('value', cpp:dot(), $vector, $source_middleware)" />
        <xsl:variable name="temp_variable" select="'element'" />
        <xsl:variable name="temp_element_variable" select="o:join_list(($target_middleware, $temp_variable), '_')" />
        <xsl:variable name="temp_size_variable" select="o:join_list(($temp_variable, 'size'), '_')" />
        <xsl:variable name="get_size" select="cpp:invoke_function($temp_variable, cpp:dot(), 'size', '', 0)" />
        <xsl:variable name="get_value" select="cpp:array_get($temp_variable, 'i')" />
      
        <xsl:variable name="translate_func" select="cpp:invoke_function('', '', cpp:combine_namespaces(($middleware_namespace, 'translate')), $temp_variable, 0)" />
        <xsl:variable name="set_func">
            <xsl:choose>
                <xsl:when test="$vector_child_kind = 'AggregateInstance'">
                    <xsl:value-of select="cdit:invoke_middleware_add_vector_function('out', cpp:arrow(), $vector, $target_middleware, $temp_element_variable)" />
                </xsl:when>
                <xsl:when test="$vector_child_kind = 'Member'">
                    <xsl:value-of select="cdit:invoke_middleware_add_vector_function('out', cpp:arrow(), $vector, $target_middleware, $temp_variable)" />
                </xsl:when>
            </xsl:choose>
        </xsl:variable>

        <xsl:value-of select="cpp:scope_start($tab)" />
        <xsl:value-of select="cpp:comment(('Vector', o:wrap_square($vector_child_kind), 'Type', o:wrap_angle($vector_child_type)), $tab + 1)" />
        <xsl:value-of select="cpp:comment('Iterate and set all elements in vector', $tab + 1)" />
        <xsl:value-of select="cpp:for_each(cpp:declare_variable(cpp:const_ref_auto(), $temp_variable, '', 0), $get_func, cpp:scope_start(0), $tab + 1)" />
       <xsl:choose>
            <xsl:when test="$vector_child_kind = 'AggregateInstance'">
                <!-- Vector aggregates require translation -->
                <xsl:value-of select="cpp:comment('Set and cleanup translated Aggregate', $tab + 2)" />
                <xsl:value-of select="cpp:define_variable(cpp:auto(), $temp_element_variable, $translate_func, cpp:nl(), $tab + 2)" />
                <xsl:value-of select="concat(o:t($tab + 2), $set_func, cpp:nl())" />
                <xsl:value-of select="cpp:delete($temp_element_variable, $tab + 2)" />
            </xsl:when>
            <xsl:when test="$vector_child_kind = 'Member'">
                <!-- Member aggregates require translation -->
                <xsl:value-of select="concat(o:t($tab + 2), $set_func, cpp:nl())" />
            </xsl:when>
        </xsl:choose>

        <xsl:value-of select="cpp:scope_end($tab + 1)" />
        <xsl:value-of select="cpp:scope_end($tab)" />
    </xsl:function>

    <xsl:function name="cdit:get_translate_cpp">
        <xsl:param name="aggregate" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="source_middleware" as="xs:string" />
        <xsl:param name="target_middleware" as="xs:string" />

        <xsl:variable name="target_type" select="cpp:get_aggregate_qualified_type($aggregate, $target_middleware)" />

        <xsl:value-of select="cpp:define_variable('auto', 'out', cpp:new_object($target_type, ''), cpp:nl(), 1)" />

        <!-- Handle the children of the aggregate -->
        <xsl:for-each select="graphml:get_child_nodes($aggregate)">
            <xsl:variable name="kind" select="graphml:get_kind(.)" />

            <xsl:choose>    
                <xsl:when test="$kind = 'Member'">
                    <xsl:value-of select="cdit:translate_member(., $middleware, $source_middleware, $target_middleware, 1)" />
                </xsl:when>
                <xsl:when test="$kind = 'EnumInstance'">
                    <xsl:value-of select="cdit:translate_enum_instance(., $middleware, $source_middleware, $target_middleware, 1)" />
                </xsl:when>
                <xsl:when test="$kind = 'AggregateInstance'">
                    <xsl:value-of select="cdit:translate_aggregate_instance(., $middleware, $source_middleware, $target_middleware, 1)" />
                </xsl:when>
                <xsl:when test="$kind = 'Vector'">
                    <xsl:value-of select="cdit:translate_vector(., $middleware, $source_middleware, $target_middleware, 1)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="cpp:comment(('Kind ', o:wrap_quote($kind), 'Not implemented!'), 1)" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:for-each>

        <xsl:value-of select="cpp:return('out', 1)" />
    </xsl:function>
</xsl:stylesheet>