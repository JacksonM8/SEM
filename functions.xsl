<xsl:stylesheet version="2.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:gml="http://graphml.graphdrawing.org/xmlns"
    xmlns:cdit="http://whatever"
    xmlns:o="http://whatever"
    >

    <xsl:function name="o:nl">
        <xsl:text>&#xa;</xsl:text>
    </xsl:function>	

    <xsl:function name="o:t">
        <xsl:param name ="count"/>

        <xsl:variable name="total">
             <xsl:for-each select="1 to $count"><xsl:text>&#x9;</xsl:text></xsl:for-each>
        </xsl:variable>
        <xsl:value-of select="$total" />
    </xsl:function>	

    <xsl:function name="o:fp">
        <xsl:value-of select="concat('-', o:gt())" />
    </xsl:function>

    <xsl:function name="o:gt">
        <xsl:text>&gt;</xsl:text>
    </xsl:function>

    <xsl:function name="o:lt">
        <xsl:text>&lt;</xsl:text>
    </xsl:function>	

    <xsl:function name="o:and">
        <xsl:text>&amp;</xsl:text>
    </xsl:function>
    
    <xsl:function name="o:quote">
        <xsl:text>'</xsl:text>
    </xsl:function>	
    
    <xsl:function name="o:dblquote">
        <xsl:text>"</xsl:text>
    </xsl:function>	

    <xsl:function name="o:quote_wrap">
        <xsl:param name="str" as="xs:string" />
        <xsl:value-of select="concat(o:quote(), $str, o:quote())" />
    </xsl:function>	

    <xsl:function name="o:angle_wrap">
        <xsl:param name="str" as="xs:string" />
        <xsl:value-of select="concat(o:lt(), $str, o:gt())" />
    </xsl:function>	

    <xsl:function name="o:square_wrap">
        <xsl:param name="str" as="xs:string" />
        <xsl:value-of select="concat('[', $str, ']')" />
    </xsl:function>	

     <xsl:function name="o:bracket_wrap">
        <xsl:param name="str" as="xs:string" />
        <xsl:value-of select="concat('(', $str, ')')" />
    </xsl:function>	

    <xsl:function name="o:curly_wrap">
        <xsl:param name="str" as="xs:string" />
        <xsl:value-of select="concat('{', $str, '}')" />
    </xsl:function>	

    <xsl:function name="o:dblquote_wrap">
        <xsl:param name="str" as="xs:string" />
        <xsl:value-of select="concat(o:dblquote(), $str, o:dblquote())" />
    </xsl:function>	
    
    <xsl:function name="o:camel_case">
        <xsl:param name="str" as="xs:string"  />
        <xsl:value-of select="concat(upper-case(substring($str,1,1)), substring($str, 2))" />
    </xsl:function>
    
    <xsl:function name="o:lib_include">
        <xsl:param name="lib" as="xs:string"  />
        <xsl:value-of select="concat('#include ', o:angle_wrap($lib), o:nl())" />
    </xsl:function>	

    <xsl:function name="o:local_include">
        <xsl:param name="lib" as="xs:string"  />
        <xsl:value-of select="concat('#include ', o:dblquote_wrap($lib), o:nl())" />
    </xsl:function>

    <xsl:function name="o:cpp_comment">
        <xsl:param name="text" as="xs:string"  />

        <xsl:value-of select="o:tabbed_cpp_comment($text, 0)" />
    </xsl:function>

    <xsl:function name="o:tabbed_cpp_comment">
        <xsl:param name="text" as="xs:string"  />
        <xsl:param name="tab" as="xs:integer"  />
        <xsl:value-of select="concat(o:t($tab), '// ', $text, o:nl())" />
    </xsl:function>


    <xsl:function name="o:cpp_func_def">
        <xsl:param name="return_type" as="xs:string"  />
        <xsl:param name="namespace" as="xs:string"  />
        <xsl:param name="name" as="xs:string"  />
        <xsl:param name="params" as="xs:string"  />

        <xsl:value-of select="concat($return_type, ' ', if($namespace !='') then concat($namespace, '::') else '', $name, '(', $params, ')')" />
    </xsl:function>

    <xsl:function name="o:cpp_var_decl">
        <xsl:param name="type" as="xs:string"  />
        <xsl:param name="name" as="xs:string"  />
        <xsl:value-of select="concat(o:t(2), $type, ' ', $name, ';', o:nl())" />
    </xsl:function>

    <xsl:function name="o:cmake_find_library">
        <xsl:param name="lib" as="xs:string"  />
        <xsl:param name="lib_vars" as="xs:string"  />
        <xsl:param name="lib_path" as="xs:string"  />

        
        <xsl:value-of select="o:cmake_comment(concat('Find library ', $lib))" />
        <xsl:value-of select="concat('find_library(', $lib_vars, ' ', $lib, ' ', o:dblquote_wrap($lib_path), ')', o:nl())" />
    </xsl:function>

    <xsl:function name="o:cmake_find_package">
        <xsl:param name="lib" as="xs:string"  />

        <xsl:value-of select="o:cmake_comment(concat('Find package ', $lib))" />
        <xsl:value-of select="concat('find_package(', $lib, ' REQUIRED)', o:nl())" />
    </xsl:function>

    <xsl:function name="o:cmake_set_env">
        <xsl:param name="var" as="xs:string"  />
        <xsl:param name="val" as="xs:string"  />
        <xsl:value-of select="concat('set(', $var, ' ',  o:dblquote_wrap($val), ')', o:nl())" />
    </xsl:function>

    <xsl:function name="o:cmake_set_proj_name">
        <xsl:param name="name" as="xs:string"  />

        <xsl:value-of select="o:cmake_set_env('PROJ_NAME', $name)" />
        <xsl:value-of select="concat('project(${PROJ_NAME})', o:nl())" />
    </xsl:function>

    <xsl:function name="o:cmake_include_dir">
        <xsl:param name="dir" as="xs:string"  />
        <xsl:value-of select="concat('include_directories(', o:dblquote_wrap($dir), ')', o:nl())" />
    </xsl:function>

    <xsl:function name="o:cmake_comment">
        <xsl:param name="text" as="xs:string"  />
        <xsl:value-of select="concat('# ', $text, o:nl())" />
    </xsl:function>

    <xsl:function name="o:define_guard">
        <xsl:param name="lib_name" as="xs:string"  />
        
        <xsl:value-of select="concat('#ifndef ', $lib_name, o:nl())" />
        <xsl:value-of select="concat('#define ', $lib_name, o:nl())" />
        <xsl:value-of select="o:nl()" />
    </xsl:function>

    <xsl:function name="o:define_guard_end">
        <xsl:param name="lib_name" as="xs:string" />
        <xsl:value-of select="concat(o:nl(), '#endif //', $lib_name, o:nl())" />
    </xsl:function>


    <xsl:function name="o:cpp_proto_get_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Getter has no get_ but uses lower-case -->
        <xsl:value-of select="lower-case($var_name)" />
    </xsl:function>

    <xsl:function name="o:cpp_proto_set_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Setter has set_ but uses lower-case -->
        <xsl:value-of select="lower-case(concat('set_', $var_name))" />
    </xsl:function>

    <xsl:function name="o:cpp_mw_func">
        <xsl:param name="var_name" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:choose>
            <xsl:when test="$middleware = 'rti' or $middleware = 'ospl'">
                <!-- DDS uses exact case -->
                <xsl:value-of select="$var_name" />
            </xsl:when>
            <xsl:when test="$middleware = 'base'">
                <!-- Base uses exact case -->
                <xsl:value-of select="$var_name" />
            </xsl:when>
            <xsl:when test="cdit:middleware_uses_protobuf($middleware)">
                <!-- Protobuf uses lowercase -->
                <xsl:value-of select="lower-case($var_name)" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:cpp_comment(concat('Middleware ', $middleware, ' not implemented'))" />
            </xsl:otherwise>
        </xsl:choose>
        
    </xsl:function>

    

    <xsl:function name="o:cpp_mw_set_func">
        <xsl:param name="var_name" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="value" as="xs:string" />

        <xsl:variable name="mw_name" select="o:cpp_mw_func($var_name, $middleware)" />

        <xsl:choose>
            <xsl:when test="$middleware = 'rti' or $middleware = 'ospl'">
                <!-- DDS doesn't use get/set functions and return references -->
                <!-- ie val() = val -->
                <xsl:value-of select="concat($mw_name, o:bracket_wrap($value))" />
            </xsl:when>
            <xsl:when test="$middleware = 'base'">
                <!-- Base uses get/set functions -->
                <!-- ie set_val(val) -->
                <xsl:value-of select="concat($mw_name, o:bracket_wrap(''), ' = ', $value)" />
            </xsl:when>
            <xsl:when test="cdit:middleware_uses_protobuf($middleware)">
                <!-- Base uses get/set functions -->
                <xsl:value-of select="concat('set_', $mw_name, o:bracket_wrap($value))" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:cpp_comment(concat('Middleware ', $middleware, ' not implemented'))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="o:cpp_mw_get_vector_func">
        <xsl:param name="var_name" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="index" as="xs:string" />

        <xsl:variable name="mw_name" select="o:cpp_mw_func($var_name, $middleware)" />

        <xsl:choose>
             <xsl:when test="cdit:middleware_uses_protobuf($middleware)">
                <!-- Protobuf uses $var_name($index) -->
                <!-- ie val(i) -->
                <xsl:value-of select="concat($mw_name, o:bracket_wrap($index))" />
            </xsl:when>
            <xsl:when test="$middleware = 'rti' or $middleware = 'ospl' or $middleware = 'base'">
                <!-- DDS/Base uses $var_name()[$index] -->
                <!-- ie val()[i] -->
                <xsl:value-of select="concat($mw_name, o:bracket_wrap(''), '[', $index, ']')" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:cpp_comment(concat('Middleware ', $middleware, ' not implemented'))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <!-- Function to determine if a cast is needed to avoid ambiguous call to setter functions-->
    <xsl:function name="o:member_requires_cast">
        <xsl:param name="member_type" as="xs:string" />
        <xsl:variable name="member_cpp_type" select="cdit:get_cpp_type($member_type)" />
        <xsl:value-of select="contains(lower-case($member_type), 'integer') = true()" />
    </xsl:function>

    <xsl:function name="o:process_member">
        <xsl:param name="member_root" />

        <xsl:param name="in_var" as="xs:string" />
        <xsl:param name="out_var" as="xs:string" />
        <xsl:param name="src_mw" as="xs:string" />
        <xsl:param name="dst_mw" as="xs:string" />
        <xsl:param name="namespace" as="xs:string" />

        <xsl:variable name="member_label" select="cdit:get_key_value($member_root, 'label')" />
        <xsl:variable name="member_type" select="cdit:get_key_value($member_root, 'type')" />
        
        <!-- Check if we need to cast this value -->
        <xsl:variable name="cast" select="if(o:member_requires_cast($member_type) = true()) then o:bracket_wrap(cdit:get_cpp_type($member_type)) else ''" />
        
        <!-- Get the value; Using the base middlewares getter -->
        <xsl:variable name="value" select="concat($cast, $in_var, o:fp(), o:cpp_mw_get_func($member_label, $src_mw))" />

        <!-- Set the value; using the appropriate middlewares setter -->
        <xsl:value-of select="concat(o:t(1), $out_var, o:fp(), o:cpp_mw_set_func($member_label, $dst_mw, $value), ';', o:nl())" />
    </xsl:function>

    <xsl:function name="o:get_convert_h">
        <xsl:param name="aggregate_root" />

        <xsl:param name="mw_type" as="xs:string" />
        <xsl:param name="base_type" as="xs:string" />
        <xsl:param name="mw" as="xs:string" />
        <xsl:param name="namespace" as="xs:string" />

        <xsl:variable name="aggregate_label" select="cdit:get_key_value($aggregate_root, 'label')" />
        <xsl:variable name="aggregate_label_cc" select="o:camel_case($aggregate_label)" />
        <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />
        <xsl:variable name="idl_name" select="cdit:get_key_value($aggregate_root/../..,'label')" />

        <xsl:variable name="define_guard" select="upper-case(concat(upper-case($mw), '_', $aggregate_label, '_CONVERT_H'))" />

        <!-- Define Guard -->
        <xsl:value-of select="o:define_guard($define_guard)" />

        <!-- Include the base message type -->
        <xsl:value-of select="o:cpp_comment('Include the base type')" />
        <!-- path back to root -->
        <xsl:variable name="rel_path" select="'../../../'" />
        <xsl:value-of select="o:local_include(concat($rel_path, 'datatypes/', $aggregate_label_lc, '/', $aggregate_label_lc, '.h'))" />
        <xsl:value-of select="o:nl()" />

        <!-- Forward declare the new type -->
        <xsl:value-of select="o:cpp_comment('Forward declare the concrete type')" />
        <xsl:value-of select="o:forward_declare_class($idl_name, $aggregate_label_cc)" />

        <xsl:value-of select="o:nl()" />
        <xsl:value-of select="o:namespace($namespace)" />

        <!-- translate functions -->
        <xsl:value-of select="o:tabbed_cpp_comment('Translate Functions', 1)" />
        <xsl:value-of select="concat(o:t(1), $mw_type, '* translate(const ', $base_type ,'* val);', o:nl())" />
        <xsl:value-of select="concat(o:t(1), $base_type, '* translate(const ', $mw_type ,'* val);', o:nl())" />

        <xsl:if test="lower-case($mw) = 'proto'">
            <!-- Helper functions -->
                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="o:tabbed_cpp_comment('Helper Functions', 1)" />
                <xsl:value-of select="concat(o:t(1), 'template ', o:angle_wrap('class T'), ' ', $base_type, '* decode(const std::string val);', o:nl())" />
                <xsl:value-of select="concat(o:t(1), 'std::string ', 'encode(const ', $base_type, '* val);', o:nl())" />
                
                <!-- Forward declared template function -->
                <xsl:value-of select="o:nl()" />
                <xsl:value-of select="o:tabbed_cpp_comment('Forward declare the decode function with concrete type', 1)" />
                <xsl:value-of select="concat(o:t(1), 'template ', o:angle_wrap(''), ' ', $base_type, '* decode', o:angle_wrap($mw_type), '(const std::string val);', o:nl())" />
        </xsl:if>

        <xsl:value-of select="concat('};', o:nl())" />

        <!-- End Define Guard -->
        <xsl:value-of select="o:define_guard_end($define_guard)" />


    </xsl:function>

    <xsl:function name="o:get_convert_cpp">
        <xsl:param name="aggregate_root" />
        <xsl:param name="members"/>
        <xsl:param name="vectors"/>
        <xsl:param name="aggregates"/>

        <xsl:param name="mw_type" as="xs:string" />
        <xsl:param name="base_type" as="xs:string" />
        <xsl:param name="mw" as="xs:string" />
        <xsl:param name="base_mw" as="xs:string" />
        <xsl:param name="namespace" as="xs:string" />

        <xsl:variable name="required_datatypes" select="cdit:get_required_datatypes($vectors, $aggregates)" />

        <xsl:variable name="aggregate_label" select="cdit:get_key_value($aggregate_root, 'label')" />
        <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />

        <!-- Include the header -->
        <xsl:value-of select="o:local_include('convert.h')" />
        <!-- Include the generated header -->
        <xsl:value-of select="o:local_include(cdit:get_middleware_file_header($aggregate_label_lc, $mw))" />
        <xsl:value-of select="o:nl()" />

        <!-- Link against the base libraries which this message contains -->
        <xsl:for-each-group select="$required_datatypes" group-by=".">
            <xsl:variable name="datatype" select="lower-case(.)" />
            <xsl:value-of select="o:local_include(concat('../', $datatype, '/convert.h'))" />
        </xsl:for-each-group>

        <!-- Translate Function from BASE to MIDDLEWARE-->
        <xsl:value-of select="o:cpp_comment('Translate from Base -> Middleware')" />
        <xsl:value-of select="o:get_translate_cpp($members, $vectors, $aggregates, $base_type, $mw_type, $base_mw, $mw, $mw)" />

        <!-- Translate Function from MIDDLEWARE to BASE-->
        <xsl:value-of select="o:cpp_comment('Translate from Middleware -> Base')" />
        <xsl:value-of select="o:get_translate_cpp($members, $vectors, $aggregates, $mw_type, $base_type, $mw, $base_mw, $mw)" />

        <xsl:if test="lower-case($mw) = 'proto'">
            <!-- Define decode functions -->
            <xsl:value-of select="concat('template', o:angle_wrap(''), o:nl())" />
            <xsl:value-of select="concat($base_type, '* ', $mw, '::decode', o:angle_wrap($mw_type), '(const std::string val){', o:nl())" />
            <xsl:value-of select="concat(o:t(1), $mw_type, ' out_;', o:nl())" />
            <xsl:value-of select="concat(o:t(1), 'out_.ParseFromString(val);', o:nl())" />
            <xsl:value-of select="concat(o:t(1), 'return translate(', o:and(), 'out_);', o:nl())" />
            <xsl:value-of select="concat('};', o:nl())" />
            <xsl:value-of select="o:nl()" />

            <!-- encode function base->str -->
            <xsl:value-of select="concat('std::string proto::encode(const ', $base_type, '* val){', o:nl())" />
            <xsl:value-of select="concat(o:t(1), 'std::string out_;', o:nl())" />
            <xsl:value-of select="concat(o:t(1), 'auto pb_ = translate(val);', o:nl())" />
            <xsl:value-of select="concat(o:t(1), 'pb_', o:fp(), 'SerializeToString(', o:and(), 'out_);', o:nl())" />
            <xsl:value-of select="concat(o:t(1), 'delete pb_;', o:nl())" />
            <xsl:value-of select="concat(o:t(1), 'return out_;', o:nl())" />
            <xsl:value-of select="concat('};', o:nl())" />
            <xsl:value-of select="o:nl()" />
        </xsl:if>
    </xsl:function>

    <xsl:function name="o:cmake_add_subdirectory">
        <xsl:param name="dir" />
        <!-- set RE_PATH -->
        <xsl:value-of select="concat('add_subdirectory(', o:dblquote_wrap($dir), ')',o:nl())" />
    </xsl:function>

    <xsl:function name="o:cmake_set_re_path">
        <!-- set RE_PATH -->
        <xsl:value-of select="o:cmake_comment('Get the RE_PATH')" />
        <xsl:value-of select="o:cmake_set_env('RE_PATH', '$ENV{RE_PATH}')" />
        <xsl:value-of select="o:cmake_set_env('CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}', '${RE_PATH}/cmake/modules')" />
        <xsl:value-of select="o:nl()" />
    </xsl:function>

     <xsl:function name="o:cmake_find_re_core_library">
        <!-- Find re_core -->
        <xsl:value-of select="o:cmake_find_library('re_core', 'RE_CORE_LIBRARIES', '${RE_PATH}/lib')" />
        <xsl:value-of select="o:nl()" />
    </xsl:function>

    <xsl:function name="o:cmake_include_re_core">
        <!-- Include directories -->
        <xsl:value-of select="o:cmake_comment('Include the RE_PATH directory')" />
        <xsl:value-of select="o:cmake_include_dir('${RE_PATH}/src')" />
        <xsl:value-of select="o:nl()" />
    </xsl:function>

    

    

    <xsl:function name="o:cmake_get_mw_package_name">
        <xsl:param name="mw" />
        <xsl:variable name="mw_lc" select="lower-case($mw)" />
        
        <xsl:choose>
            <xsl:when test="$mw_lc = 'rti'">
                <xsl:value-of select="'RTIDDS'" />
            </xsl:when>
            <xsl:when test="$mw_lc = 'ospl'">
                <xsl:value-of select="'OSPL'" />
            </xsl:when>
            <xsl:when test="$mw_lc = 'qpid'">
                <xsl:value-of select="'QPID'" />
            </xsl:when>
            <xsl:when test="$mw_lc = 'zmq'">
                <xsl:value-of select="'ZMQ'" />
            </xsl:when>
            <xsl:when test="$mw_lc = 'proto'">
                <xsl:value-of select="'Protobuf'" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="''" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="o:cmake_find_mw_package">
        <xsl:param name="mw" />
        <xsl:variable name="package" select="o:cmake_get_mw_package_name($mw)" />
        
        <xsl:if test="$package != ''">
            <xsl:value-of select="o:nl()" />
            <xsl:value-of select="o:cmake_find_package($package)" />
            <xsl:value-of select="o:nl()" />
        </xsl:if>
    </xsl:function>

     <xsl:function name="o:cmake_var_wrap">
        <xsl:param name="var" as="xs:string" />
        
        <xsl:value-of select="concat('${', $var, '}')" />
     </xsl:function>
     
     <xsl:function name="o:get_middleware_file_extension">
        <xsl:param name="mw" as="xs:string" />

        <xsl:variable name="mw_lc" select="lower-case($mw)" />

        <xsl:choose>
            <xsl:when test="$mw_lc = 'rti' or $mw_lc = 'ospl'">
                <xsl:value-of select="'idl'" />
            </xsl:when>
            <xsl:when test="$mw_lc = 'proto'">
                <xsl:value-of select="'proto'" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="''" />            
            </xsl:otherwise>
        </xsl:choose>
     </xsl:function>

     

    <xsl:function name="o:cmake_generate_mw_files">
        <xsl:param name="aggregate_name_lc" />    
        <xsl:param name="srcs" as="xs:string" />
        <xsl:param name="hdrs" as="xs:string" />
        <xsl:param name="mw" as="xs:string" />

        <xsl:value-of select="o:cmake_comment(concat('Generate the ', o:angle_wrap($mw), ' files for Aggregate: ', $aggregate_name_lc))" />

        <xsl:variable name="mw_lc" select="lower-case($mw)" />
        <xsl:variable name="mw_ext" select="o:get_middleware_file_extension($mw_lc)" />

        <xsl:variable name="mw_gen">
            <xsl:choose>
                <xsl:when test="$mw_lc = 'rti'">
                    <xsl:value-of select="'RTI_GENERATE_CPP'" />
                </xsl:when>
                <xsl:when test="$mw_lc = 'ospl'">
                    <xsl:value-of select="'OSPL_GENERATE_CPP'" />
                </xsl:when>
                <xsl:when test="$mw_lc = 'proto'">
                    <xsl:value-of select="'protobuf_generate_cpp'" />
                </xsl:when>
            </xsl:choose>
        </xsl:variable>
        <xsl:value-of select="concat($mw_gen,'(', $srcs, ' ', $hdrs, ' ', $aggregate_name_lc, '.', $mw_ext, ')', o:nl())" />
    </xsl:function>
    
    <xsl:function name="o:cmake_mw_builds_shared_library">
        <xsl:param name="mw" as="xs:string" />
        <xsl:variable name="mw_lc" select="lower-case($mw)" />

        <xsl:choose>
            <xsl:when test="$mw_lc = 'rti' or $mw_lc = 'ospl' or $mw_lc = 'proto'">
                <xsl:value-of select="true()" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="false()" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="o:cmake_mw_builds_module">
        <xsl:param name="mw" as="xs:string" />
        <xsl:variable name="mw_lc" select="lower-case($mw)" />

        <xsl:choose>
            <xsl:when test="$mw_lc = 'proto'">
                <xsl:value-of select="false()" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="true()" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="cdit:get_vector_cpp_type">
        <xsl:param name="vector" />

        <xsl:variable name="vector_type" select="cdit:get_vector_type($vector)" />
        <xsl:variable name="is_vector_complex" select="cdit:is_vector_complex($vector)" />

        <xsl:variable name="vector_type_cpp">
            <xsl:choose>
                <xsl:when test="$is_vector_complex">
                    <xsl:value-of select="concat('::', $vector_type)" />
                </xsl:when>
                <xsl:otherwise>
                    <xsl:value-of select="$vector_type"/>
                </xsl:otherwise>
            </xsl:choose>
        </xsl:variable>
        <xsl:value-of select="concat('std::vector', o:angle_wrap($vector_type_cpp))" />
    </xsl:function>

    <xsl:function name="cdit:get_aggregate_cpp_type">
        <xsl:param name="aggregate" />

        <xsl:variable name="aggregate_type" select="cdit:get_key_value($aggregate, 'type')" />
        <xsl:value-of select="concat('::', $aggregate_type)" />
    </xsl:function>

    <xsl:function name="o:get_base_data_type_h">
        <xsl:param name="aggregate_root" />
        <xsl:param name="members"/>
        <xsl:param name="vectors"/>
        <xsl:param name="aggregates"/>

        <xsl:variable name="required_datatypes" select="cdit:get_required_datatypes($vectors, $aggregates)" />

        <!-- Get the label of the Aggregate -->
        <xsl:variable name="aggregate_label" select="cdit:get_key_value($aggregate_root, 'label')" />
        <xsl:variable name="aggregate_label_cc" select="o:camel_case($aggregate_label)" />
        <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />

        <!-- Define Guard -->
        <xsl:variable name="define_guard" select="upper-case(concat('PORTS_', $aggregate_label, '_H'))" />
        <xsl:value-of select="o:define_guard($define_guard)" />

        <!-- Include Base Types -->
        <xsl:value-of select="o:cpp_comment('Include Statements')" />
        <xsl:value-of select="o:lib_include('core/basemessage.h')" />
        <xsl:value-of select="o:lib_include('string')" />

        <xsl:variable name="rel_dir" select="'../'" />

        <!-- Include std vector if we need -->
        <xsl:if test="count($vectors) > 0">
            <xsl:value-of select="o:lib_include('vector')" />
            <xsl:value-of select="o:nl()" />
        </xsl:if>

        <!-- Include other required base types -->
        <xsl:if test="count($required_datatypes) > 0">
            <xsl:value-of select="o:cpp_comment('Include required datatypes')" />
            <xsl:for-each-group select="$required_datatypes" group-by=".">
                <xsl:variable name="datatype" select="lower-case(.)" />
                <xsl:value-of select="o:local_include(concat($rel_dir, $datatype, '/', $datatype, '.h'))" />
            </xsl:for-each-group>
            <xsl:value-of select="o:nl()" />
        </xsl:if>

        <!-- Define the class, Inherits from BaseMessage -->
        <xsl:value-of select="concat('class ', $aggregate_label_cc, ' : public BaseMessage{', o:nl())" />
        <!-- Handle Members -->
        <xsl:for-each select="$members">
            <xsl:variable name="member_label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="member_type" select="cdit:get_key_value(., 'type')" />
            <xsl:variable name="member_cpp_type" select="cdit:get_cpp_type($member_type)" />

            <xsl:value-of select="concat(o:nl(), o:t(1))" />
            <xsl:value-of select="o:cpp_comment(concat('Basic Member: ', $member_label, ' ', o:angle_wrap($member_type)))" />
            <xsl:value-of select="o:declare_variable_functions($member_label, $member_cpp_type)" />
        </xsl:for-each>

        <!-- Handle Vectors -->
        <xsl:for-each select="$vectors">
            <xsl:variable name="vector_label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="vector_cpp_type" select="cdit:get_vector_cpp_type(.)" />

            <xsl:value-of select="concat(o:nl(), o:t(1))" />
            <xsl:value-of select="o:cpp_comment(concat('Vector Member: ', $vector_label, ' ',$vector_cpp_type))" />
            <xsl:value-of select="o:declare_variable_functions($vector_label, $vector_cpp_type)" />
        </xsl:for-each>

        <!-- Handle Aggregates -->
        <xsl:for-each select="$aggregates">
            <xsl:variable name="aggregate_label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="aggregate_cpp_type" select="cdit:get_aggregate_cpp_type(.)" />

            <xsl:value-of select="concat(o:nl(), o:t(1))" />
            <xsl:value-of select="o:cpp_comment(concat('Aggregate Member: ', $aggregate_label, ' ', o:angle_wrap($aggregate_cpp_type)))" />
            <xsl:value-of select="o:declare_variable_functions($aggregate_label, $aggregate_cpp_type)" />
        </xsl:for-each>

        <xsl:value-of select="concat('};', o:nl())" />
        <xsl:value-of select="o:define_guard_end($define_guard)" />
    </xsl:function>

    <xsl:function name="o:get_base_data_type_cpp">
        <xsl:param name="aggregate_root" />
        <xsl:param name="members"/>
        <xsl:param name="vectors"/>
        <xsl:param name="aggregates"/>

        <!-- Get the label of the Aggregate -->
        <xsl:variable name="aggregate_label" select="cdit:get_key_value($aggregate_root, 'label')" />
        <xsl:variable name="aggregate_label_cc" select="o:camel_case($aggregate_label)" />
        <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />

        <xsl:variable name="class_name" select="$aggregate_label_cc" />

        <xsl:value-of select="o:local_include(concat($aggregate_label_lc, '.h'))" />
        <xsl:value-of select="o:nl()" />

        <!-- Handle Members -->
        <xsl:for-each select="$members">
            <xsl:variable name="member_label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="member_type" select="cdit:get_key_value(., 'type')" />
            <xsl:variable name="member_cpp_type" select="cdit:get_cpp_type($member_type)" />

            <xsl:value-of select="o:define_variable_functions($member_label, $member_cpp_type, $class_name)" />
        </xsl:for-each>

        <!-- Handle Vectors -->
        <xsl:for-each select="$vectors">
            <xsl:variable name="first_child" select="./gml:graph[1]/gml:node[1]" />
            <xsl:variable name="vector_label" select="cdit:get_key_value(., 'label')" />

            <!-- Get the Vector Type -->
            <xsl:variable name="vector_child_kind" select="cdit:get_key_value($first_child, 'kind')" />
            <xsl:variable name="vector_child_type" select="cdit:get_key_value($first_child, 'type')" />
            
            <!-- Get the type of the vector -->
            <xsl:variable name="vector_type" select="if($vector_child_kind = 'AggregateInstance') then concat('::', $vector_child_type) else cdit:get_cpp_type($vector_child_type)" />

            <xsl:variable name="vector_cpp_type" select="concat('std::vector', o:angle_wrap($vector_type))" />

            <xsl:value-of select="o:define_variable_functions($vector_label, $vector_cpp_type, $class_name)" />
        </xsl:for-each>

        <!-- Handle Aggregates -->
        <xsl:for-each select="$aggregates">
            <xsl:variable name="aggregate_label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="aggregate_type" select="cdit:get_key_value(., 'type')" />
            <xsl:variable name="aggregate_cpp_type" select="concat('::', $aggregate_type)" />

            <xsl:value-of select="o:define_variable_functions($aggregate_label, $aggregate_cpp_type, $class_name)" />
        </xsl:for-each>
    </xsl:function>

     <xsl:function name="o:get_base_data_type_cmake">
        <xsl:param name="aggregate_root" />
        <xsl:param name="members"/>
        <xsl:param name="vectors"/>
        <xsl:param name="aggregates"/>

        <xsl:variable name="required_datatypes" select="cdit:get_required_datatypes($vectors, $aggregates)" />

        <!-- Get the label of the Aggregate -->
        <xsl:variable name="aggregate_label" select="cdit:get_key_value($aggregate_root, 'label')" />
        <xsl:variable name="aggregate_label_cc" select="o:camel_case($aggregate_label)" />
        <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />

        <xsl:value-of select="o:cmake_set_re_path()" />
        

        <!-- Set PROJ_NAME -->
        <xsl:variable name="proj_name" select="concat('datatype_', $aggregate_label_lc)" />
        <xsl:variable name="PROJ_NAME" select="o:cmake_var_wrap('PROJ_NAME')" />
        <xsl:value-of select="o:cmake_set_proj_name($proj_name)" />

        <xsl:value-of select="o:cmake_find_re_core_library()" />

        <!-- Set Source files -->
        <xsl:value-of select="concat('set(SOURCE', o:nl())" />
        <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/', $aggregate_label_lc, '.cpp', o:nl())" />
        <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
        <xsl:value-of select="o:nl()" />

        <!-- Set Headers files -->
        <xsl:value-of select="concat('set(HEADERS', o:nl())" />
        <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/', $aggregate_label_lc, '.h', o:nl())" />
        <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
        <xsl:value-of select="o:nl()" />

        <xsl:value-of select="o:cmake_include_re_core()" />

        <!-- add library/link -->
        <xsl:value-of select="concat('add_library(${PROJ_NAME} SHARED ${SOURCE} ${HEADERS})', o:nl())" />

        <xsl:value-of select="o:cmake_link_re_core($PROJ_NAME)" />


        <xsl:for-each-group select="$required_datatypes" group-by=".">
            <xsl:variable name="datatype" select="concat('datatype_', lower-case(.))" />
            <xsl:value-of select="o:cmake_target_link_libraries($PROJ_NAME, $datatype)" />
        </xsl:for-each-group>
    </xsl:function>

    <xsl:function name="o:cmake_target_link_libraries">
        <xsl:param name="proj_name" />
        <xsl:param name="library"/>
        <xsl:value-of select="concat('target_link_libraries(', $proj_name, ' ', $library, ')', o:nl())" />
    </xsl:function>

    <xsl:function name="o:cmake_link_re_core">
        <xsl:param name="proj_name" />
        <xsl:value-of select="o:cmake_target_link_libraries($proj_name, o:cmake_var_wrap('RE_CORE_LIBRARIES'))" />
    </xsl:function>
    

    <xsl:function name="o:get_mw_type_cmake">
        <xsl:param name="aggregate_root" />
        <xsl:param name="members"/>
        <xsl:param name="vectors"/>
        <xsl:param name="aggregates"/>
        <xsl:param name="mw" as="xs:string" />

        <xsl:variable name="required_datatypes" select="cdit:get_required_datatypes($vectors, $aggregates)" />
        
        <xsl:variable name="aggregate_label" select="cdit:get_key_value($aggregate_root, 'label')" />
        <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />

        <xsl:value-of select="o:cmake_set_re_path()" />

        <xsl:variable name="proj_name" select="concat($mw, '_', $aggregate_label_lc)" />
        <xsl:variable name="lib_name" select="concat($mw, '_', $aggregate_label_lc, '_lib')" />

        <xsl:variable name="mw_srcs" select="concat(upper-case($mw), '_SRCS')" />
        <xsl:variable name="mw_hdrs" select="concat(upper-case($mw), '_HDRS')" />

        <xsl:variable name="proj_name" select="concat($mw, '_', $aggregate_label_lc)" />

        <xsl:variable name="build_module" select="o:cmake_mw_builds_module($mw) = true()" />
        <xsl:variable name="build_shared_library" select="o:cmake_mw_builds_shared_library($mw) = true()" />
        <xsl:variable name="middleware_ext" select="o:get_middleware_file_extension($mw)" />
                
        <xsl:if test="$build_shared_library">
            <xsl:value-of select="o:cmake_set_env('SHARED_LIB_NAME', $lib_name)" />
        </xsl:if>

        <xsl:value-of select="o:cmake_set_proj_name($proj_name)" />

        <!-- Get the Middleware specific package -->
        <xsl:value-of select="o:cmake_find_mw_package($mw)" />

        <xsl:if test="$build_shared_library">
            <xsl:variable name="mw_ext" select="o:get_middleware_file_extension(lower-case($mw))" />

            <xsl:value-of select="o:cmake_comment(concat('Copy this ', o:angle_wrap($mw_ext), ' file into the binary directory so the compilation of generated files can succeed'))" />
            <xsl:value-of select="concat('configure_file(', $aggregate_label_lc, '.', $mw_ext, ' ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)', o:nl())" />
            <xsl:value-of select="o:nl()" />

            <!-- Copy the other Middleware specific files required to compile this type-->
            <xsl:if test="count($required_datatypes) and $mw_ext != ''">
                <xsl:value-of select="o:cmake_comment(concat('Copy required ', o:angle_wrap($mw_ext), ' into the binary directory so the compilation of generated files can succeed'))" />
                <xsl:for-each-group select="$required_datatypes" group-by=".">
                    <xsl:variable name="datatype" select="lower-case(.)" />
                    <xsl:value-of select="concat('configure_file(../', $datatype, '/', $datatype, '.', $mw_ext, ' ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)', o:nl())" />
                </xsl:for-each-group>
                <xsl:value-of select="o:nl()" />
            </xsl:if>

            <!-- Generate the Middleware specific files, if any-->
            <xsl:value-of select="o:cmake_generate_mw_files(concat('${CMAKE_CURRENT_BINARY_DIR}/', $aggregate_label_lc), $mw_srcs, $mw_hdrs, $mw)" />
            <xsl:value-of select="o:nl()" />
        </xsl:if>
        
        <xsl:value-of select="o:cmake_find_re_core_library()" />

        <xsl:variable name="mw_helper_libs" select="concat(upper-case($mw), '_HELPER_LIBRARIES')" />

        <xsl:if test="$build_module">
             <!-- Find MW Helper -->
            <xsl:value-of select="o:cmake_find_library(concat($mw,'_helper'), $mw_helper_libs , '${RE_PATH}/lib')" />
            <xsl:value-of select="o:nl()" />
        </xsl:if>

       

        <xsl:if test="$build_shared_library">
            <!-- Set Source files -->
            <xsl:value-of select="concat('set(SOURCE', o:nl())" />
            <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/convert.cpp', o:nl())" />
            <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
            <xsl:value-of select="o:nl()" />

            <!-- Set HEADER files -->
            <xsl:value-of select="concat('set(HEADERS', o:nl())" />
            <xsl:value-of select="concat(o:t(1), '${CMAKE_CURRENT_SOURCE_DIR}/convert.h', o:nl())" />
            <xsl:value-of select="concat(o:t(1), ')', o:nl())" />
            <xsl:value-of select="o:nl()" />
        </xsl:if>

         <!-- Include directories -->
        <xsl:value-of select="o:cmake_include_re_core()" />

        <xsl:variable name="mw_package" select="o:cmake_get_mw_package_name($mw)" />
        <xsl:variable name="mw_package_uc" select="upper-case($mw_package)" />
        <xsl:if test="$mw_package_uc != ''">
            <xsl:value-of select="o:cmake_comment(concat('Include the ', $mw_package, ' directory'))" />
            <xsl:value-of select="o:cmake_include_dir(concat('${', $mw_package_uc, '_INCLUDE_DIRS}'))" />
            <xsl:value-of select="o:nl()" />
        </xsl:if>

        

        <!-- Build Shared Library -->
        <xsl:if test="$build_shared_library">
            <xsl:value-of select="o:cmake_comment('Include the current binary directory to enable linking to the autogenerated files')" />
            <xsl:value-of select="o:cmake_include_dir('${CMAKE_CURRENT_BINARY_DIR}')" />
            <xsl:value-of select="o:nl()" />

            <xsl:if test="count($required_datatypes)">
                <xsl:value-of select="o:cmake_comment(concat('Include the binary directories used by the ', o:angle_wrap($mw), ' libraries used this type'))" />
                <xsl:for-each-group select="$required_datatypes" group-by=".">
                    <xsl:variable name="datatype" select="lower-case(.)" />
                    <xsl:value-of select="o:cmake_include_dir(concat('${CMAKE_CURRENT_BINARY_DIR}/../', $datatype))" />
                </xsl:for-each-group>
                <xsl:value-of select="o:nl()" />
            </xsl:if>
        </xsl:if>
        
        

        

        
        <!-- Build Shared Library -->
        <xsl:if test="$build_shared_library">
            <xsl:value-of select="o:cmake_comment('Build the shared library that will be loaded at compile time.')" />
            <xsl:value-of select="concat('add_library(${SHARED_LIB_NAME} SHARED ${SOURCE} ${HEADERS} ', o:cmake_var_wrap($mw_srcs), ' ',o:cmake_var_wrap($mw_hdrs), ')', o:nl())" />
            <xsl:value-of select="o:nl()" />

            <!-- Link Shared library -->
            <xsl:value-of select="o:cmake_comment('Link the shared library.')" />
            <xsl:value-of select="concat('target_link_libraries(${SHARED_LIB_NAME} ${RE_CORE_LIBRARIES})', o:nl())" />
            
            <xsl:if test="$mw_package_uc != ''">
                <xsl:value-of select="concat('target_link_libraries(${SHARED_LIB_NAME} ${', $mw_package_uc, '_LIBRARIES})', o:nl())" />
            </xsl:if>

            <xsl:if test="$build_module">
                <xsl:value-of select="concat('target_link_libraries(${SHARED_LIB_NAME} ${', $mw_helper_libs, '})', o:nl())" />
            </xsl:if>
            
            <xsl:value-of select="o:nl()" />
            
            <!-- Link against base datatype libraries -->
            <xsl:value-of select="o:cmake_comment('Link the shared library against the base type library')" />
            <xsl:value-of select="concat('target_link_libraries(${SHARED_LIB_NAME} datatype_', $aggregate_label_lc, ')', o:nl())" />
            <xsl:value-of select="o:nl()" />

            <!-- Link against other proto libraries which this message contains -->
            <xsl:if test="count($required_datatypes)">
                <xsl:value-of select="o:cmake_comment(concat('Link shared library against the ', o:angle_wrap($mw), ' libraries used this type'))" />
                <xsl:for-each-group select="$required_datatypes" group-by=".">
                    <xsl:variable name="datatype" select="lower-case(.)" />
                    <xsl:value-of select="concat('target_link_libraries(${SHARED_LIB_NAME} ', $mw, '_', $datatype, '_lib)', o:nl())" />
                </xsl:for-each-group>
                <xsl:value-of select="o:nl()" />
            </xsl:if>
            <xsl:value-of select="o:nl()" />
        </xsl:if>

        <!-- Build Module Library -->
        <xsl:if test="$build_module">
            <xsl:value-of select="o:cmake_comment('Build the actual Module library that will be dynamically loaded.')" />
            <xsl:value-of select="concat('add_library(${PROJ_NAME} MODULE ${CMAKE_CURRENT_SOURCE_DIR}/libportexports.cpp)', o:nl())" />
            <xsl:value-of select="o:nl()" />
            
            <xsl:value-of select="o:cmake_comment('Link the shared library.')" />
            <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} ${RE_CORE_LIBRARIES})', o:nl())" />
            <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} ${', $mw_helper_libs, '})', o:nl())" />

            <xsl:if test="$mw_package_uc != ''">
                <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} ${', $mw_package_uc, '_LIBRARIES})', o:nl())" />
            </xsl:if>

            <!-- Link against base datatype libraries -->
            <xsl:value-of select="o:cmake_comment('Link the shared library against the base type library')" />
            <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} datatype_', $aggregate_label_lc, ')', o:nl())" />
            <xsl:value-of select="o:nl()" />

            <xsl:choose>
                <xsl:when test="$build_shared_library">
                    <!-- Link the module against the shared library -->
                    <xsl:value-of select="o:cmake_comment('Link the Module against the Shared library')" />
                    <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} ${SHARED_LIB_NAME})', o:nl())" />
                    <xsl:value-of select="o:nl()" />
                </xsl:when>
                <xsl:when test="o:middleware_uses_protobuf($mw)">
                    <xsl:value-of select="concat('target_link_libraries(${PROJ_NAME} proto_', $aggregate_label_lc, '_lib)', o:nl())" />
                </xsl:when>
            </xsl:choose>
        </xsl:if>

        <!-- Do Specific rti dds stuff -->
        <xsl:if test="$mw = 'rti'">
            <xsl:value-of select="o:cmake_comment('This ensures rti DDS uses the correct types')" />
            <xsl:value-of select="concat('target_compile_definitions(${PROJ_NAME} PRIVATE -DRTI_UNIX -DRTI_64BIT)', o:nl())" />
            <xsl:value-of select="concat('target_compile_definitions(${SHARED_LIB_NAME} PRIVATE -DRTI_UNIX -DRTI_64BIT)', o:nl())" />
            <xsl:value-of select="o:nl()" />
        </xsl:if>
    </xsl:function>



    
    <xsl:function name="o:get_idl">
        <xsl:param name="aggregate_root"/>
        <xsl:param name="members"/>
        <xsl:param name="vectors"/>
        <xsl:param name="aggregates"/>

        <xsl:variable name="required_datatypes" select="cdit:get_required_datatypes($vectors, $aggregates)" />

        <xsl:variable name="aggregate_label" select="cdit:get_key_value($aggregate_root, 'label')" />
        <xsl:variable name="aggregate_label_cc" select="o:camel_case($aggregate_label)" />
        <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />
        <xsl:variable name="idl_name" select="cdit:get_key_value($aggregate_root/../..,'label')" />

        <!-- Import other IDLs -->
        <xsl:for-each-group select="$required_datatypes" group-by=".">
            <xsl:variable name="datatype" select="lower-case(.)" />
            <xsl:variable name="idl_path" select="concat($datatype, '.idl')" />
            <xsl:value-of select="o:lib_include($idl_path)" />
        </xsl:for-each-group>

        <!-- Module Name -->
        <xsl:value-of select="concat('module ', $idl_name, '{', o:nl())" />
            
        <!-- Struct Name -->
        <xsl:value-of select="concat(o:t(1), 'struct ', $aggregate_label_cc, ' {', o:nl())" />

        <!-- Process Members -->
        <xsl:for-each select="$members">
            <!-- TODO: Need to handle members/vectors/aggregates out of order -->
            <!-- The Sort Order starts at 0 -->
            <xsl:variable name="sort_order" select="number(cdit:get_key_value(., 'sortOrder')) + 1" />
            
            <xsl:variable name="member_label" select="cdit:get_key_value(.,'label')" />
            <xsl:variable name="member_type" select="cdit:get_key_value(.,'type')" />

            <xsl:variable name="member_is_key" select="cdit:is_key_value_true(.,'key')" />
            <xsl:variable name="member_cpp_type" select="cdit:get_cpp_type($member_type)" />
            <xsl:variable name="member_dds_type" select="cdit:get_cpp2dds_type($member_cpp_type)" />
            
            <xsl:value-of select="concat(o:t(2), $member_dds_type, ' ', $member_label, ';')" />

            <xsl:if test="$member_is_key">
                <xsl:value-of select="' //@key'" />
            </xsl:if>
            <xsl:value-of select="o:nl()" />
        </xsl:for-each>

        <!-- Process Vectors -->
        <xsl:for-each select="$vectors">
            <xsl:variable name="sort_order" select="number(cdit:get_key_value(., 'sortOrder')) + 1" />
            
            <xsl:variable name="vector_label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="vector_cpp_type" select="cdit:get_vector_type(.)" />
            <xsl:variable name="vector_dds_type" select="cdit:get_cpp2dds_type($vector_cpp_type)" />
            
            <xsl:variable name="is_vector_complex" select="cdit:is_vector_complex(.)" />
            
            <xsl:choose>
                <xsl:when test="$is_vector_complex">
                    <!-- Complex types -->
                    <xsl:variable name="aggregate_namespace" select="cdit:get_namespace(.)" />
                    <xsl:value-of select="concat(o:t(2), 'sequence', o:angle_wrap(concat($aggregate_namespace, '::', $vector_cpp_type)), ' ', $vector_label, ';', o:nl())" />
                </xsl:when>
                <xsl:otherwise>
                    <!-- Primitive types -->
                    <xsl:value-of select="concat(o:t(2), 'sequence', o:angle_wrap($vector_dds_type), ' ', $vector_label, ';', o:nl())" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:for-each>

        <!-- Process Aggregates -->
        <xsl:for-each select="$aggregates">
            <xsl:variable name="sort_order" select="number(cdit:get_key_value(., 'sortOrder')) + 1" />
            
            <xsl:variable name="aggregate_label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="aggregate_type" select="cdit:get_key_value(.,'type')" />

            <!-- Get the Namespace -->
            <xsl:variable name="aggregate_namespace" select="cdit:get_namespace(.)" />
            
            <xsl:value-of select="concat(o:t(2), $aggregate_namespace, '::', $aggregate_type, ' ', $aggregate_label, ';', o:nl())" />
        </xsl:for-each>

        <xsl:value-of select="concat(o:t(1),'};', o:nl())" />

        <xsl:for-each select="$members">
            <xsl:variable name="member_label" select="cdit:get_key_value(.,'label')" />
            <xsl:variable name="member_is_key" select="cdit:is_key_value_true(.,'key')" />

            <xsl:if test="$member_is_key">
                <xsl:value-of select="concat(o:t(1),'#pragma keylist ', $aggregate_label_cc, ' ', $member_label, o:nl())" />
            </xsl:if>
        </xsl:for-each>

        <xsl:value-of select="concat('};', o:nl())" />
    </xsl:function>

    <xsl:function name="o:get_proto">
        <xsl:param name="aggregate_root"/>
        <xsl:param name="members"/>
        <xsl:param name="vectors"/>
        <xsl:param name="aggregates"/>

        <xsl:variable name="required_datatypes" select="cdit:get_required_datatypes($vectors, $aggregates)" />

        <xsl:variable name="aggregate_label" select="cdit:get_key_value($aggregate_root, 'label')" />
        <xsl:variable name="aggregate_label_cc" select="o:camel_case($aggregate_label)" />
        <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />
        <xsl:variable name="idl_name" select="cdit:get_key_value($aggregate_root/../..,'label')" />

        <xsl:value-of select="concat('syntax = ', o:dblquote_wrap('proto3'), ';', o:nl())" />
        <xsl:value-of select="concat('package ', $idl_name, ';', o:nl())" />
        <xsl:value-of select="o:nl()" />

        <xsl:for-each-group select="$required_datatypes" group-by=".">
            <xsl:variable name="datatype" select="lower-case(.)" />
            <xsl:variable name="proto_path" select="concat($datatype, '.proto')" />

            <xsl:value-of select="concat('import ', o:dblquote_wrap($proto_path), ';', o:nl())" />
        </xsl:for-each-group>

        <xsl:value-of select="concat('message ', $aggregate_label_cc, ' {', o:nl())" />

        <xsl:for-each select="$members">
            <xsl:variable name="member_label" select="cdit:get_key_value(.,'label')" />
            <xsl:variable name="member_type" select="cdit:get_key_value(.,'type')" />
            <xsl:variable name="member_cpp_type" select="cdit:get_cpp_type($member_type)" />
            <xsl:variable name="member_proto_type" select="cdit:get_cpp2proto_type($member_cpp_type)" />
            <xsl:variable name="sort_order" select="number(cdit:get_key_value(., 'sortOrder')) + 1" />
            
            <xsl:value-of select="concat(o:t(1), $member_proto_type, ' ', $member_label, ' = ', $sort_order, ';', o:nl())" />
        </xsl:for-each>

        <xsl:for-each select="$vectors">
            <xsl:variable name="vector_label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="vector_cpp_type" select="cdit:get_vector_type(.)" />
            <xsl:variable name="vector_proto_type" select="cdit:get_cpp2proto_type($vector_cpp_type)" />
            <xsl:variable name="sort_order" select="number(cdit:get_key_value(., 'sortOrder')) + 1" />
            
            <xsl:variable name="is_vector_complex" select="cdit:is_vector_complex(.)" />
            
            <xsl:choose>
                <xsl:when test="$is_vector_complex">
                    <!-- Complex types -->
                    <xsl:variable name="aggregate_namespace" select="cdit:get_namespace(.)" />
                    <xsl:value-of select="concat(o:t(1), 'repeated ', $aggregate_namespace, '.', $vector_cpp_type, ' ', $vector_label, ' = ', $sort_order, ';', o:nl())" />
                </xsl:when>
                <xsl:otherwise>
                    <!-- Primitive types -->
                    <xsl:value-of select="concat(o:t(1), 'repeated ', $vector_proto_type, ' ', $vector_label, ' = ', $sort_order, ';', o:nl())" />
                </xsl:otherwise>
            </xsl:choose>
        </xsl:for-each>

        <xsl:for-each select="$aggregates">
            <xsl:variable name="sort_order" select="number(cdit:get_key_value(., 'sortOrder')) + 1" />
            
            <xsl:variable name="aggregate_label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="aggregate_type" select="cdit:get_key_value(.,'type')" />

            <!-- Get the Namespace -->
            <xsl:variable name="aggregate_namespace" select="cdit:get_namespace(.)" />
            
            <xsl:value-of select="concat(o:t(1), $aggregate_namespace, '.', $aggregate_type, ' ', $aggregate_label, ' = ', $sort_order, ';', o:nl())" />
        </xsl:for-each>
        <xsl:value-of select="concat('}', o:nl())" />
    </xsl:function>


    <xsl:function name="o:xsl_wrap_file">
        <xsl:param name="file_path" as="xs:string" />
        <xsl:message>Created File: <xsl:value-of select="$file_path" /></xsl:message>
        <xsl:value-of select="$file_path" />
    </xsl:function>

    
    <xsl:function name="o:inplace_getter_function">
        <xsl:param name="label" />

        <xsl:value-of select="concat($label, '()')" />
    </xsl:function>

    <xsl:function name="o:process_aggregate">
        <xsl:param name="aggregate_root" />

        <xsl:param name="in_var" as="xs:string" />
        <xsl:param name="out_var" as="xs:string" />
        <xsl:param name="src_mw" as="xs:string" />
        <xsl:param name="dst_mw" as="xs:string" />
        <xsl:param name="namespace" as="xs:string" />

        <xsl:variable name="aggregate_label" select="cdit:get_key_value($aggregate_root, 'label')" />
        <xsl:variable name="aggregate_type" select="cdit:get_key_value($aggregate_root, 'type')" />

        <xsl:variable name="src_var" select="concat('src_', lower-case($aggregate_label), '_')" />
        <xsl:variable name="dst_var" select="concat('dst_', lower-case($aggregate_label), '_')" />

        <xsl:variable name="get_value" select="concat($in_var, o:fp(), o:cpp_mw_get_func($aggregate_label, $src_mw))" />
        
        <xsl:value-of select="o:tabbed_cpp_comment(concat('Translate the Complex type ', o:angle_wrap($aggregate_type)), 1)" />
        <xsl:value-of select="concat(o:t(1), 'auto ', $src_var, ' = ', $get_value, ';', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'auto ', $dst_var, ' = ', $namespace, '::translate(',o:and(), $src_var, ');', o:nl())" />
                
        <xsl:value-of select="concat(o:t(1), 'if(', $dst_var, '){', o:nl())" />
            <xsl:choose>
                <xsl:when test="o:middleware_uses_protobuf($dst_mw)">
                    <!-- Protobuf have to use a swap function from the newly added element -->
                    <xsl:value-of select="concat(o:t(2), $out_var, o:fp(), 'set_allocated_', o:cpp_mw_func($aggregate_label, $dst_mw), '(', $dst_var, ');', o:nl())" />
                </xsl:when>
                <xsl:otherwise>
                    <!-- Complex vectors in other middlewares use indexed insertion using a dereferenced pointer -->
                    <xsl:value-of select="concat(o:t(2), $out_var, o:fp(), o:cpp_mw_set_func($aggregate_label, $dst_mw, concat('*', $dst_var)), ';', o:nl())" />
                </xsl:otherwise>
            </xsl:choose>
            <!-- Free Memory From Translate -->
            <xsl:value-of select="concat(o:t(2), 'delete ', $dst_var, ';', o:nl())" />
            <xsl:value-of select="concat(o:t(1), '}', o:nl())" />
    </xsl:function>

    <xsl:function name="o:get_translate_cpp">
        <xsl:param name="members" />
        <xsl:param name="vectors" />
        <xsl:param name="aggregates" />
        <xsl:param name="src_type" as="xs:string" />
        <xsl:param name="dst_type" as="xs:string" />
        <xsl:param name="src_mw" as="xs:string" />
        <xsl:param name="dst_mw" as="xs:string" />
        <xsl:param name="namespace" as="xs:string" />

        <xsl:variable name="in_var" select="'src'" />
        <xsl:variable name="out_var" select="'dst_'" />

        <xsl:value-of select="concat($dst_type, '* ', $namespace, '::translate(const ', $src_type , ' *', $in_var, '){', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'auto ', $out_var, ' = new ', $dst_type, '();', o:nl())" />
        
        <xsl:for-each select="$members">
            <xsl:value-of select="o:process_member(., $in_var, $out_var, $src_mw, $dst_mw, $namespace)" />
        </xsl:for-each>

        <xsl:for-each select="$vectors">
            <xsl:value-of select="o:process_vector(., $in_var, $out_var, $src_mw, $dst_mw, $namespace)" />
        </xsl:for-each>

        <xsl:for-each select="$aggregates">
            <xsl:value-of select="o:process_aggregate(., $in_var, $out_var, $src_mw, $dst_mw, $namespace)" />
        </xsl:for-each>

        <!-- Return the Translated object -->
        <xsl:value-of select="concat(o:t(1), 'return ', $out_var, ';', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
        <xsl:value-of select="o:nl()" />
    </xsl:function>

    <xsl:function name="o:process_vector">
        <xsl:param name="vector_root" />

        <xsl:param name="in_var" as="xs:string" />
        <xsl:param name="out_var" as="xs:string" />
        <xsl:param name="src_mw" as="xs:string" />
        <xsl:param name="dst_mw" as="xs:string" />
        <xsl:param name="namespace" as="xs:string" />

        <!-- Get the Middleware which is used for the namespace functions -->
        <xsl:variable name="translate_mw" select="if(lower-case($src_mw) = 'base') then $dst_mw else $src_mw" />

        <!-- Get the appropriate fields from the vector -->
        <xsl:variable name="vector_label" select="cdit:get_key_value($vector_root, 'label')" />
        <xsl:variable name="vector_type" select="cdit:get_key_value($vector_root, 'type')" />
        <xsl:variable name="vector_cpp_type" select="cdit:get_vector_type($vector_root)" />
        <xsl:variable name="is_vector_complex" select="cdit:is_vector_complex($vector_root)" />
        <!-- lower-case($vector_label) -->
        <xsl:variable name="src_var" select="concat('src_', lower-case($vector_label), '_i_')" />
        <xsl:variable name="dst_var" select="concat('dst_', lower-case($vector_label), '_i_')" />

        <xsl:variable name="src_mw_uses_pb" select=" o:middleware_uses_protobuf($src_mw)" />
        <xsl:variable name="dst_mw_uses_pb" select=" o:middleware_uses_protobuf($dst_mw)" />

        <!-- Opening Brace -->
        <xsl:value-of select="concat(o:t(1), '{', o:nl())" />
        <xsl:value-of select="o:tabbed_cpp_comment(concat('o:process_vector() ', $vector_label, o:angle_wrap($vector_cpp_type)), 2)" />

        <!-- Get a copy of the vector-->
        <xsl:value-of select="o:tabbed_cpp_comment('Copy Vector', 2)" />
        <xsl:variable name="vector_copy" select="concat($vector_label, '_')" />
        <xsl:value-of select="concat(o:t(2), 'auto ', $vector_copy, ' = ', $in_var, o:fp(), o:cpp_mw_get_func($vector_label, $src_mw), ';', o:nl())" />

        <!-- Get the source vector size -->
        <xsl:value-of select="o:tabbed_cpp_comment('Get the size of the source vector', 2)" />
        <xsl:value-of select="concat(o:t(2), 'auto size_ = ', $vector_copy, '.size();', o:nl())" />

        <xsl:if test="$dst_mw_uses_pb = false()">
            <!-- For non protobuf vectors, resize the target vector -->
            <xsl:value-of select="o:tabbed_cpp_comment('Resize the destination vector', 2)" />
            <xsl:value-of select="concat(o:t(2), $out_var, o:fp(), o:cpp_mw_func($vector_label, $dst_mw), '().resize(size_);', o:nl())" />
        </xsl:if>

        <!-- Define for loop -->
        <xsl:value-of select="o:nl()" />
        <xsl:value-of select="o:tabbed_cpp_comment('Itterate through source vector', 2)" />
        <xsl:value-of select="concat(o:t(2), 'for(int i = 0; i ', o:lt(), ' size_; i++){', o:nl())" />

        

        <xsl:variable name="get_src_val" select="concat($vector_copy, '[i]')" />
        
        <!-- Set the target vector element -->
        <xsl:choose>
            <xsl:when test="$is_vector_complex">
                <!-- Complex types -->
                <xsl:value-of select="o:tabbed_cpp_comment(concat('Translate the Complex type ', o:angle_wrap($vector_cpp_type)), 3)" />
                <xsl:value-of select="concat(o:t(3), 'auto ', $dst_var, ' = ', $namespace, '::translate(',o:and(),  o:bracket_wrap($get_src_val), ');', o:nl())" />
                
                <xsl:value-of select="concat(o:t(3), 'if(', $dst_var, '){', o:nl())" />
                <xsl:choose>
                    <xsl:when test="o:middleware_uses_protobuf($dst_mw)">
                        <xsl:variable name="new_element" select="concat($dst_var, 'pb_')" />
                        <xsl:value-of select="o:tabbed_cpp_comment('Add element to the destination vector', 4)" />
                        <xsl:value-of select="concat(o:t(4), 'auto ', $new_element, ' = ', $out_var, o:fp(), o:cpp_mw_func(concat('add_', $vector_label), $dst_mw),'();', o:nl())" />
                        <xsl:value-of select="o:tabbed_cpp_comment('Swap the contents into the destination vector', 4)" />
                        <!-- Complex vectors in protobuf have to use a swap function from the newly added element -->
                        <xsl:value-of select="concat(o:t(4), $new_element, o:fp(), 'Swap(', $dst_var, ');', o:nl())" />
                    </xsl:when>
                    <xsl:otherwise>
                        <!-- Complex vectors in other middlewares use indexed insertion using a dereferenced pointer -->
                        <xsl:value-of select="concat(o:t(4), $out_var, o:fp(), o:cpp_mw_func($vector_label, $dst_mw), '()[i] = *', $dst_var, ';', o:nl())" />
                    </xsl:otherwise>
                </xsl:choose>
                <!-- Free Memory From Translate -->
                <xsl:value-of select="concat(o:t(4), 'delete ', $dst_var, ';', o:nl())" />
                <xsl:value-of select="concat(o:t(3), '}', o:nl())" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:choose>
                    <xsl:when test="o:middleware_uses_protobuf($dst_mw)">
                        <!-- Primitive vectors in protobuf use an add_ function with value as a parameter -->
                        <xsl:value-of select="concat(o:t(3), $out_var, o:fp(), o:cpp_mw_func(concat('add_', $vector_label), $src_mw),'(', $get_src_val, ');', o:nl())" />
                    </xsl:when>
                    <xsl:otherwise>
                        <!-- Primitive vectors in other middlewares simply use indexed insertion -->
                        <xsl:value-of select="concat(o:t(3), $out_var, o:fp(), o:cpp_mw_func($vector_label, $dst_mw), '()[i] = ', $get_src_val, ';', o:nl())" />
                    </xsl:otherwise>
                </xsl:choose>
            </xsl:otherwise>
        </xsl:choose>

        <xsl:value-of select="concat(o:t(2), '}', o:nl())" />
        <xsl:value-of select="concat(o:t(1), '}', o:nl())" />
    </xsl:function>

 
    <xsl:function name="o:cpp_mw_get_func">
        <xsl:param name="var_name" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:variable name="mw_name" select="o:cpp_mw_func($var_name, $middleware)" />

        <xsl:choose>
            <xsl:when test="$middleware = 'rti' or $middleware = 'ospl'">
                <!-- DDS doesn't use get/set functions -->
                <xsl:value-of select="concat($mw_name, o:bracket_wrap(''))" />
            </xsl:when>
            <xsl:when test="$middleware = 'base'">
                <!-- Base uses get/set functions-->
                <xsl:value-of select="concat('get_', $mw_name, o:bracket_wrap(''))" />
            </xsl:when>
            <xsl:when test="cdit:middleware_uses_protobuf($middleware)">
                <xsl:value-of select="concat($mw_name, o:bracket_wrap(''))" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:cpp_comment(concat('Middleware ', $middleware, ' not implemented'))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>


    <xsl:function name="o:cpp_set_translated_val">
        <xsl:param name="target" as="xs:string" />
        <xsl:param name="var_name" as="xs:string" />
        <xsl:param name="var_type" as="xs:string" />
        <xsl:param name="mw" as="xs:string" />
        <xsl:param name="tab" as="xs:integer" />

        <xsl:variable name="mw_var_name" select="concat($var_name, $mw, '_')" />

        <xsl:value-of select="o:tabbed_cpp_comment(concat('Translate the Complex type ', o:angle_wrap($var_type)), $tab)" />
        <xsl:value-of select="concat(o:t($tab), '{', o:nl())" />
        <xsl:value-of select="concat(o:t($tab+1), 'auto ', $mw_var_name, ' = ', $mw, '::translate(', o:and(), $var_name ,');', o:nl())" />
        <xsl:value-of select="concat(o:t($tab+1), 'if(', $mw_var_name, '){', o:nl())" />
        <xsl:value-of select="concat(o:t($tab+2), 'out_', o:fp(), o:cpp_mw_get_func($var_name, $mw), '()[i] = ', '*', $mw_var_name, ';', o:nl())" />
        <xsl:value-of select="concat(o:t($tab+2), 'delete ', $mw_var_name, ';', o:nl())" />
        <xsl:value-of select="concat(o:t($tab+1), '}', o:nl())" />
        <xsl:value-of select="concat(o:t($tab), '}', o:nl())" />
        
    </xsl:function>


    <xsl:function name="o:cpp_dds_set_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Setter has no set prefix, and is in camel-case -->
        <xsl:value-of select="$var_name" />
    </xsl:function>

    <xsl:function name="o:cpp_dds_get_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Setter has no get prefix, and is in camel-case -->
        <xsl:value-of select="$var_name" />
    </xsl:function>

    <xsl:function name="o:cpp_proto_set_complex_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Setter has set_ but uses lower-case -->
        <xsl:value-of select="lower-case(concat('set_allocated_', $var_name))" />
    </xsl:function>

    <xsl:function name="o:cpp_proto_release_complex_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Setter has set_ but uses lower-case -->
        <xsl:value-of select="lower-case(concat('release_', $var_name))" />
    </xsl:function>

    <xsl:function name="o:cpp_proto_add_vector">
        <xsl:param name="var_name" as="xs:string" />
        <xsl:param name="value" as="xs:string" />
        <xsl:value-of select="(concat('add_', lower-case($var_name), '(', $value, ')'))" />
    </xsl:function>

    <xsl:function name="o:cpp_base_add_vector">
        <xsl:param name="var_name" as="xs:string" />
        <xsl:param name="value" as="xs:string" />
        <xsl:value-of select="concat($var_name, '().push_back(', $value, ')')" />
    </xsl:function>




     <xsl:function name="o:construct_rx">
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="base_type" as="xs:string" />
        <xsl:param name="mw_type" as="xs:string" />

        <xsl:value-of select="'EventPort* ConstructRx(std::string port_name, Component* component){'" />
        <xsl:value-of select="o:nl()" />
        <xsl:value-of select="concat(o:t(1), 'EventPort* p = 0;', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'if(component){', o:nl())" />
        <xsl:value-of select="concat(o:t(2), 'auto fn = component', o:fp(), 'GetCallback(port_name);', o:nl())" />
        <xsl:value-of select="concat(o:t(2), 'if(fn){', o:nl())" />
        <xsl:value-of select="concat(o:t(3), 'p = new ', $middleware, '::InEventPort', o:angle_wrap(concat($base_type, ', ', $mw_type)),'(component, port_name, fn);', o:nl())" />
        <xsl:value-of select="concat(o:t(2), '}', o:nl())" />
        <xsl:value-of select="concat(o:t(1), '}', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'return p;', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
    </xsl:function>

    <xsl:function name="o:construct_tx">
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="base_type" as="xs:string" />
        <xsl:param name="mw_type" as="xs:string" />

        <xsl:value-of select="'EventPort* ConstructTx(std::string port_name, Component* component){'" />
        <xsl:value-of select="o:nl()" />
        <xsl:value-of select="concat(o:t(1), 'return new ', $middleware, '::OutEventPort', o:angle_wrap(concat($base_type, ', ', $mw_type)),'(component, port_name);', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
    </xsl:function>


    <xsl:function name="cdit:middleware_uses_protobuf" as="xs:boolean">
        <xsl:param name="middleware" as="xs:string" />

        <xsl:value-of select="$middleware='qpid' or $middleware='zmq' or $middleware='proto'" />
    </xsl:function>



    <xsl:function name="cdit:get_middleware_file_header" as="xs:string">
        <xsl:param name="aggregate" as="xs:string" />
        <xsl:param name="middleware" as="xs:string" />

        <xsl:choose>
            <xsl:when test="$middleware = 'rti'">
                <xsl:value-of select="concat($aggregate, '.hpp')" />
            </xsl:when>
            <xsl:when test="$middleware = 'ospl'">
                <xsl:value-of select="concat($aggregate, '_DCPS.hpp')" />
            </xsl:when>
            <xsl:when test="$middleware = 'proto'">
                <xsl:value-of select="concat($aggregate, '.pb.h')" />
            </xsl:when>
            <xsl:when test="cdit:middleware_uses_protobuf($middleware)">
                <xsl:value-of select="''" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:cpp_comment(concat('Middlware ', $middleware, ' not implemented'))" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>

    <xsl:function name="o:get_libport_export">
        <xsl:param name="aggregate" />
        <xsl:param name="middleware" as="xs:string" />
        <xsl:param name="base_type" as="xs:string" />
        <xsl:param name="mw_type" as="xs:string" />

        
        <xsl:variable name="using_pb" select ="cdit:middleware_uses_protobuf($middleware)" />

        <xsl:variable name="aggregate_label" select ="cdit:get_key_value($aggregate, 'label')" />
        <xsl:variable name="aggregate_label_lc" select="lower-case($aggregate_label)" />

        <xsl:variable name="mw_header_file" select="cdit:get_middleware_file_header($aggregate_label_lc, $middleware)" />

        
        <!-- Include libportexports.h -->            
        <xsl:value-of select="o:lib_include('core/libportexports.h')" />
        <xsl:value-of select="o:nl()" />

        <!-- Include Convert functions -->
        <xsl:choose>
            <xsl:when test="$using_pb">
                <xsl:value-of select="o:cpp_comment('Include the proto convert functions for the port type')" />
                <xsl:value-of select="o:local_include(concat('../../proto/', $aggregate_label_lc, '/convert.h'))" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="o:local_include('convert.h')" />
            </xsl:otherwise>
        </xsl:choose>

        
        <xsl:if test="$using_pb = false()" >
            <xsl:value-of select="o:local_include($mw_header_file)" />
        </xsl:if>


        
        <xsl:value-of select="o:nl()" />
        
        <!-- Include Templated EventPort classes -->
        <xsl:value-of select="o:cpp_comment(concat('Include the ', $middleware, ' specific template ports'))" />
        <xsl:value-of select="o:lib_include(concat('middleware/', $middleware, '/ineventport.hpp'))" />
        <xsl:value-of select="o:lib_include(concat('middleware/', $middleware, '/outeventport.hpp'))" />
        <xsl:value-of select="o:nl()" />
        
        <!-- Construct RX -->
        <xsl:value-of select="o:construct_rx($middleware, $base_type, $mw_type)" />
        <xsl:value-of select="o:nl()" />
        <xsl:value-of select="o:construct_tx($middleware, $base_type, $mw_type)" />
    </xsl:function>
    

    

    <xsl:function name="o:cpp_base_get_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Getter has get_ and same case -->
        <xsl:value-of select="concat('get_', $var_name)" />
    </xsl:function>

    <xsl:function name="o:cpp_base_get_ptr_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Getter has get_ and same case -->
        <xsl:value-of select="$var_name" />
    </xsl:function>

    <xsl:function name="o:cpp_base_set_func">
        <xsl:param name="var_name" as="xs:string" />
        <!-- Getter has set_ and same case -->
        <xsl:value-of select="concat('set_', $var_name)" />
    </xsl:function>

    <xsl:function name="o:namespace">
        <xsl:param name="namespace" as="xs:string" />
        <xsl:value-of select="concat('namespace ', $namespace, '{', o:nl())" />
    </xsl:function>

    <xsl:function name="o:forward_declare_class">
        <xsl:param name="namespace" as="xs:string" />
        <xsl:param name="class_name" as="xs:string" />

        <xsl:if test="$namespace != ''">
            <xsl:value-of select="concat(o:namespace($namespace), o:t(1))" />
        </xsl:if>

        <xsl:value-of select="concat('class ', $class_name, ';', o:nl())" />

        <xsl:if test="$namespace != ''">
            <xsl:value-of select="concat('};', o:nl())" />
        </xsl:if>
    </xsl:function>

    <xsl:function name="o:declare_attribute_functions">
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:param name="variable_type" as="xs:string" />
        <xsl:variable name="variable_ptr_type" select="concat($variable_type, o:and())" />

        <!-- Copy Setter -->
        <xsl:value-of select="concat(o:t(2), o:cpp_func_def('void', '', o:cpp_base_set_func($variable_name), concat($variable_type, ' val')), ';', o:nl())" />
        <!-- Pointer Setter -->
        <xsl:value-of select="concat(o:t(2), o:cpp_func_def('void', '', o:cpp_base_set_func($variable_name), concat($variable_type, '* val')), ';', o:nl())" />
        
        <!-- Copy Getter -->
        <xsl:value-of select="concat(o:t(2), o:cpp_func_def($variable_type, '', o:cpp_base_get_func($variable_name), ''), ';', o:nl())" />
        <!-- Inplace Getter -->
        <xsl:value-of select="concat(o:t(2), o:cpp_func_def($variable_ptr_type, '', $variable_name, ''), ';', o:nl())" />
    </xsl:function>

    <xsl:function name="o:define_attribute_functions">
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:param name="variable_type" as="xs:string" />
        <xsl:param name="class_name" as="xs:string" />

        <xsl:variable name="cpp_type" select="cdit:get_cpp_type($variable_type)" />

        <xsl:variable name="attr_type" select="cdit:get_attr_type($variable_type)" />

        <xsl:variable name="variable_var" select="concat($variable_name, '_')" />
        <xsl:variable name="variable_ptr_type" select="concat($cpp_type, o:and())" />

        <!-- Copy Setter -->
        <xsl:value-of select="o:cpp_func_def('void', $class_name, o:cpp_base_set_func($variable_name), concat($cpp_type, ' val'))" />
        <xsl:value-of select="concat('{', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'auto a = GetAttribute(', o:dblquote_wrap($variable_name), ');', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'if(a){', o:nl())" />
        <xsl:value-of select="concat(o:t(2), 'a', o:fp(), 'set_', $attr_type, '(val);', o:nl())" />
        <xsl:value-of select="concat(o:t(1),'}', o:nl())" />
        <xsl:value-of select="concat('}', o:nl())" />
        <xsl:value-of select="o:nl()" />

        <!-- Pointer Setter -->
        <xsl:value-of select="o:cpp_func_def('void', $class_name, o:cpp_base_set_func($variable_name), concat($cpp_type, '* val'))" />
        <xsl:value-of select="concat('{', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'if(val){', o:nl())" />
        <xsl:value-of select="concat(o:t(2), o:cpp_base_set_func($variable_name), '(*val);', o:nl())" />
        <xsl:value-of select="concat(o:t(1),'}', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
        <xsl:value-of select="o:nl()" />

        <!-- Copy Getter -->
        <xsl:value-of select="o:cpp_func_def($cpp_type, $class_name, o:cpp_base_get_func($variable_name), '')" />
        <xsl:value-of select="concat('{', o:nl())" />
        <xsl:value-of select="concat(o:t(1), $cpp_type, ' val;', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'auto a = GetAttribute(', o:dblquote_wrap($variable_name), ');', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'if(a){', o:nl())" />
        <xsl:value-of select="concat(o:t(2), 'val = a', o:fp(), 'get_', $attr_type, '();', o:nl())" />
        <xsl:value-of select="concat(o:t(1),'}', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'return val;', o:nl())" />
        <xsl:value-of select="concat('}', o:nl())" />
        <xsl:value-of select="o:nl()" />

        <!-- Inplace Getter -->
        <xsl:value-of select="o:cpp_func_def($variable_ptr_type, $class_name, $variable_name, '')" />
        <xsl:value-of select="concat('{', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'auto a = GetAttribute(', o:dblquote_wrap($variable_name), ');', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'return a', o:fp(), $attr_type, '();', o:nl())" />
        <xsl:value-of select="concat('}', o:nl())" />
        <xsl:value-of select="o:nl()" />
    </xsl:function>


    <xsl:function name="o:declare_variable_functions">
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:param name="variable_type" as="xs:string" />

        <xsl:variable name="variable_ptr_type" select="concat($variable_type, o:and())" />

        
        <!-- Public Declarations -->
        <xsl:value-of select="concat(o:t(1), 'public:', o:nl())" />
        <!-- Copy Setter -->
        <xsl:value-of select="concat(o:t(2), o:cpp_func_def('void', '', o:cpp_base_set_func($variable_name), concat($variable_type, ' val')), ';', o:nl())" />
        <!-- Pointer Setter -->
        <xsl:value-of select="concat(o:t(2), o:cpp_func_def('void', '', o:cpp_base_set_func($variable_name), concat($variable_type, '* val')), ';', o:nl())" />
        
        <!-- Copy Getter -->
        <xsl:value-of select="concat(o:t(2), o:cpp_func_def($variable_type, '', o:cpp_base_get_func($variable_name), ''), ' const;', o:nl())" />
        <!-- Inplace Getter -->
        <xsl:value-of select="concat(o:t(2), o:cpp_func_def($variable_ptr_type, '', $variable_name, ''), ';', o:nl())" />
        
        <!-- Private Declarations -->
        <xsl:value-of select="concat(o:t(1), 'private:', o:nl())" />
        <!-- Variable -->
        <xsl:value-of select="o:cpp_var_decl($variable_type, concat($variable_name, '_'))" />
    </xsl:function>

    <xsl:function name="o:define_variable_functions">
        <xsl:param name="variable_name" as="xs:string" />
        <xsl:param name="variable_type" as="xs:string" />
        <xsl:param name="class_name" as="xs:string" />

        <xsl:variable name="variable_var" select="concat($variable_name, '_')" />
        <xsl:variable name="variable_ptr_type" select="concat($variable_type, o:and())" />

        <!-- Copy Setter -->
        <xsl:value-of select="o:cpp_func_def('void', $class_name, o:cpp_base_set_func($variable_name), concat($variable_type, ' val'))" />
        <xsl:value-of select="concat('{', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'this', o:fp(), $variable_var, ' = val;', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
        <xsl:value-of select="o:nl()" />

        <!-- Pointer Setter -->
        <xsl:value-of select="o:cpp_func_def('void', $class_name, o:cpp_base_set_func($variable_name), concat($variable_type, '* val'))" />
        <xsl:value-of select="concat('{', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'if(val){', o:nl())" />
        <xsl:value-of select="concat(o:t(2), 'this', o:fp(), $variable_var, ' = *val;', o:nl())" />
        <xsl:value-of select="concat(o:t(1),'}', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
        <xsl:value-of select="o:nl()" />

        <!-- Copy Getter -->
        <xsl:value-of select="o:cpp_func_def($variable_type, $class_name, o:cpp_base_get_func($variable_name), '')" />
        <xsl:value-of select="concat(' const {', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'return this', o:fp(), $variable_var, ';', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
        <xsl:value-of select="o:nl()" />

        <!-- Inplace Getter -->
        <xsl:value-of select="o:cpp_func_def($variable_ptr_type, $class_name, $variable_name, '')" />
        <xsl:value-of select="concat('{', o:nl())" />
        <xsl:value-of select="concat(o:t(1), 'return this', o:fp(), $variable_name, '_;', o:nl())" />
        <xsl:value-of select="concat('};', o:nl())" />
        <xsl:value-of select="o:nl()" />
    </xsl:function>
   
                
    <xsl:function name="cdit:get_attr_type" as="xs:string">
        <xsl:param name="type" as="xs:string"  />

        <xsl:choose>
            <xsl:when test="$type = 'String'">
                <xsl:value-of select="'String'" />
            </xsl:when>
            <xsl:when test="$type = 'Boolean'">
                <xsl:value-of select="'Boolean'" />
            </xsl:when>
            <xsl:when test="$type = 'FloatNumber' or $type = 'DoubleNumber' or $type = 'LongDoubleNumber'">
                <xsl:value-of select="'Double'" />
            </xsl:when>
            <xsl:when test="$type = 'LongInteger' or $type ='UnsignedLongInteger'">
                <xsl:value-of select="'Integer'" />
            </xsl:when>
            
            <xsl:otherwise>
                <xsl:value-of select="concat('/*Unknown Type: ', o:quote_wrap($type), ' */')" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>	
    
    <xsl:function name="cdit:get_attr_enum_type" as="xs:string">
        <xsl:param name="type" as="xs:string"  />
        <xsl:value-of select="concat('ATTRIBUTE_TYPE::', upper-case(o:get_attr_type($type)))" />
    </xsl:function>	


    <xsl:function name="cdit:get_cpp_type" as="xs:string">
        <xsl:param name="type" as="xs:string"  />

        <xsl:choose>
            <xsl:when test="$type = 'String'">
                <xsl:value-of select="'std::string'" />
            </xsl:when>
            <xsl:when test="$type = 'Boolean'">
                <xsl:value-of select="'bool'" />
            </xsl:when>
            <xsl:when test="$type = 'FloatNumber' or $type = 'Float'">
                <xsl:value-of select="'float'" />
            </xsl:when>
            <xsl:when test="$type = 'DoubleNumber' or $type = 'LongDoubleNumber' or $type = 'Double'">
                <xsl:value-of select="'double'" />
            </xsl:when>
            <xsl:when test="$type = 'LongInteger' or $type = 'Integer'">
                <xsl:value-of select="'int'" />
            </xsl:when>
            <xsl:when test="$type = 'UnsignedLongInteger'">
                <xsl:value-of select="'unsigned int'" />
            </xsl:when>
            
            <xsl:otherwise>
                <xsl:value-of select="concat('/*get_cpp_type() Unknown Type: ', o:quote_wrap($type), ' */')" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>	

    <xsl:function name="cdit:get_cpp2proto_type" as="xs:string">
        <xsl:param name="type" as="xs:string"  />

        <xsl:choose>
            <xsl:when test="$type = 'std::string'">
                <xsl:value-of select="'string'" />
            </xsl:when>
            <xsl:when test="$type = 'int'">
                <xsl:value-of select="'int64'" />
            </xsl:when>
            <xsl:when test="$type = 'double' or $type = 'float' or $type = 'bool'">
                <xsl:value-of select="$type" />
            </xsl:when>

            <xsl:otherwise>
                <xsl:value-of select="concat('/*Unknown Type: ', o:quote_wrap($type), ' */')" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>	

    <xsl:function name="cdit:get_cpp2dds_type" as="xs:string">
        <xsl:param name="type" as="xs:string"  />

        <xsl:choose>
            <xsl:when test="$type = 'std::string'">
                <xsl:value-of select="'string'" />
            </xsl:when>
            <xsl:when test="$type = 'int'">
                <xsl:value-of select="'long'" />
            </xsl:when>
            <xsl:when test="$type = 'double' or $type = 'float' or $type = 'bool'">
                <xsl:value-of select="$type" />
            </xsl:when>

            <xsl:otherwise>
                <xsl:value-of select="concat('/*Unknown Type: ', o:quote_wrap($type), ' */')" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>	

    <xsl:function name="cdit:get_edge_target_ids">
        <xsl:param name="root" />
        <xsl:param name="edge_kind" as="xs:string" />
        <xsl:param name="edge_source" as="xs:string" />

        <xsl:variable name="kind_key" select="cdit:get_key_id($root, 'kind')" />

        <xsl:variable name="doc_root" select="cdit:get_doc_root($root)" />
        
        <xsl:sequence select="$doc_root//gml:edge[@source = $edge_source]/gml:data[@key=$kind_key and text()=$edge_kind]/../@target" />
    </xsl:function>
    <xsl:function name="cdit:get_edge_source_ids">
        <xsl:param name="root" />
        <xsl:param name="edge_kind" as="xs:string" />
        <xsl:param name="edge_target" as="xs:string" />

        <xsl:variable name="kind_key" select="cdit:get_key_id($root, 'kind')" />

        <xsl:variable name="doc_root" select="cdit:get_doc_root($root)" />
        
        <xsl:sequence select="$doc_root//gml:edge[@target = $edge_target]/gml:data[@key=$kind_key and text()=$edge_kind]/../@source" />
    </xsl:function>

    <xsl:function name="cdit:get_key_id" as="xs:string">
        <xsl:param name="root"/>
        <xsl:param name="key_name" as="xs:string"/>
        
        <xsl:for-each select="$root">
            <xsl:value-of select="//gml:key[@attr.name = $key_name]/@id" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:get_key_value" as="xs:string">
        <xsl:param name="root" />
        <xsl:param name="key_name" as="xs:string"/>

        <xsl:for-each select="$root">
            <xsl:variable name="key_id" select="cdit:get_key_id(., $key_name)" />        
            <xsl:value-of select="$root/gml:data[@key=$key_id]/text()" />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:is_key_value_true" as="xs:boolean">
        <xsl:param name="root" />
        <xsl:param name="key_name" as="xs:string"/>

        <xsl:value-of select="lower-case(cdit:get_key_value($root, $key_name)) = 'true'" />
    </xsl:function>

    <xsl:function name="cdit:get_descendant_entities_of_kind" as="element()*">
        <xsl:param name="root" />
        <xsl:param name="kind" as="xs:string" />

        <xsl:for-each select="$root">
            <xsl:variable name="kind_id" select="cdit:get_key_id(., 'kind')" />        
            <xsl:sequence select="$root//gml:node/gml:data[@key=$kind_id and text() = $kind]/.." />
        </xsl:for-each>
    </xsl:function>

     <xsl:function name="cdit:get_child_entities_of_kind" as="element()*">
        <xsl:param name="root" />
        <xsl:param name="kind" as="xs:string" />

        <xsl:for-each select="$root">
            <xsl:variable name="kind_id" select="cdit:get_key_id(., 'kind')" />        
            <xsl:sequence select="$root/gml:graph/gml:node/gml:data[@key=$kind_id and text() = $kind]/.." />
        </xsl:for-each>
    </xsl:function>

    <xsl:function name="cdit:get_entities_of_kind" as="element()*">
        <xsl:param name="root" />
        <xsl:param name="kind" as="xs:string" />

        <xsl:variable name="kind_id" select="cdit:get_key_id($root, 'kind')" />        
        <xsl:sequence select="$root//gml:data[@key=$kind_id and text() = $kind]/.." />
    </xsl:function>

    <xsl:function name="cdit:get_required_datatypes">
        <xsl:param name="vectors" as="element()*"/>
        <xsl:param name="aggregates" as="element()*"/>

        <xsl:for-each select="$aggregates">
            <xsl:variable name="aggregate_type" select="lower-case(cdit:get_key_value(., 'type'))" />
        
            <xsl:value-of select="$aggregate_type" />
        </xsl:for-each>

        <xsl:for-each select="$aggregates">
            <xsl:variable name="aggregate_type" select="lower-case(cdit:get_key_value(., 'type'))" />
        
            <xsl:value-of select="$aggregate_type" />
        </xsl:for-each>
    </xsl:function>

   



    <xsl:function name="cdit:get_node_id" as="xs:string">
        <xsl:param name="root" />
        <xsl:value-of select="$root/@id" />
    </xsl:function>

    <xsl:function name="cdit:get_doc_root">
        <xsl:param name="root" />
        <!-- There should only be one-->
        <xsl:sequence select="$root/ancestor::gml:graph" />
    </xsl:function>

    <xsl:function name="cdit:get_node_by_id">
        <xsl:param name="root" />
        <xsl:param name="id"  as="xs:string"/>

        <xsl:variable name="doc_root" select="cdit:get_doc_root($root)" />        

        <xsl:sequence select="$doc_root//gml:node[@id = $id]" />
    </xsl:function>

    <xsl:function name="cdit:is_vector_complex" as="xs:boolean">
        <xsl:param name="vector" as="element()*"/>

        <xsl:variable name="first_child" select="$vector/gml:graph[1]/gml:node[1]" />
        <!-- Get the kind of the first child in the Vector Type -->
        <xsl:variable name="vector_child_kind" select="cdit:get_key_value($first_child, 'kind')" />
        
        <!-- Get the type of the vector -->
        <xsl:variable name="is_aggregate" select="$vector_child_kind = 'AggregateInstance' or $vector_child_kind = 'Aggregate'" />
        <xsl:value-of select="$is_aggregate" />
    </xsl:function>

    <xsl:function name="cdit:get_variable_type" as="xs:string">
        <xsl:param name="variable" as="element()*"/>

        <xsl:variable name="first_child" select="cdit:get_node_child($variable, 1)" />

        <xsl:choose>
            <xsl:when test="$first_child">
                <!--<xsl:variable name="first_child" select="$variable/gml:graph[1]/gml:node[1]" />-->
                <xsl:value-of select="cdit:get_key_value($first_child, 'kind')" />
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cdit:get_key_value($variable, 'type')" />
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>
    
    <xsl:function name="cdit:get_node_child">
        <xsl:param name="root" as="element()*"/>
        <xsl:param name="index" as="xs:integer"/>
        <xsl:sequence select="$root/gml:graph[1]/gml:node[$index]" />
    </xsl:function>

    <xsl:function name="cdit:get_vector_type">
        <xsl:param name="vector" as="element()*"/>

        <xsl:variable name="first_child" select="$vector/gml:graph[1]/gml:node[1]" />
        <xsl:variable name="vector_label" select="cdit:get_key_value($vector, 'label')" />

        <!-- Get the Vector Type -->
        <xsl:variable name="vector_child_kind" select="cdit:get_key_value($first_child, 'kind')" />
        <xsl:variable name="vector_child_type" select="cdit:get_key_value($first_child, 'type')" />
        
        <xsl:choose>
            <xsl:when test="cdit:is_vector_complex($vector)">
                <xsl:value-of select="$vector_child_type"/>
            </xsl:when>
            <xsl:otherwise>
                <xsl:value-of select="cdit:get_cpp_type($vector_child_type)"/>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:function>


    <xsl:function name="cdit:get_aggregate_index">
        <xsl:param name="node" as="element()*"/>

        <xsl:variable name="index" select="count($node/preceding-sibling::gml:node) + 1" />
        <xsl:value-of select="$index" />
    </xsl:function>


    <xsl:function name="cdit:get_namespace">
        <xsl:param name="node" as="element()*"/>

        <!-- Follow Definition Edges -->
        <xsl:value-of select="'HelloWorld'" />
    </xsl:function>

    <xsl:function name="cdit:get_subfolder_cmake">
        <xsl:param name="elements" />

        <xsl:for-each select="$elements">
            <xsl:variable name="label" select="cdit:get_key_value(., 'label')" />
            <xsl:variable name="path" select="concat(o:cmake_var_wrap('CMAKE_CURRENT_SOURCE_DIR'),'/', lower-case($label))" />
            <xsl:value-of select="o:cmake_add_subdirectory($path)" />
        </xsl:for-each>
    </xsl:function>
</xsl:stylesheet>