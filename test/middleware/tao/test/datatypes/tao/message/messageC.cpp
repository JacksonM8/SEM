// -*- C++ -*-
//
// $Id$

// ****  Code generated by the The ACE ORB (TAO) IDL Compiler ****
// TAO and the TAO IDL Compiler have been developed by:
//       Center for Distributed Object Computing
//       Washington University
//       St. Louis, MO
//       USA
//       http://www.cs.wustl.edu/~schmidt/doc-center.html
// and
//       Distributed Object Computing Laboratory
//       University of California at Irvine
//       Irvine, CA
//       USA
//       http://doc.ece.uci.edu/
// and
//       Institute for Software Integrated Systems
//       Vanderbilt University
//       Nashville, TN
//       USA
//       http://www.isis.vanderbilt.edu/
//
// Information about TAO is available at:
//     http://www.cs.wustl.edu/~schmidt/TAO.html

// TAO_IDL - Generated from
// be/be_codegen.cpp:381


#include "messageC.h"
#include "tao/AnyTypeCode/Null_RefCount_Policy.h"
#include "tao/AnyTypeCode/TypeCode_Constants.h"
#include "tao/AnyTypeCode/Alias_TypeCode_Static.h"
#include "tao/AnyTypeCode/Objref_TypeCode_Static.h"
#include "tao/AnyTypeCode/String_TypeCode_Static.h"
#include "tao/AnyTypeCode/Struct_TypeCode_Static.h"
#include "tao/AnyTypeCode/TypeCode_Struct_Field.h"
#include "tao/CDR.h"
#include "tao/Exception_Data.h"
#include "tao/Invocation_Adapter.h"
#include "tao/Object_T.h"
#include "tao/CDR.h"
#include "tao/AnyTypeCode/Any.h"
#include "tao/AnyTypeCode/Any_Impl_T.h"
#include "tao/AnyTypeCode/Any_Dual_Impl_T.h"
#include "tao/Basic_Arguments.h"
#include "tao/Var_Size_Argument_T.h"
#include "ace/OS_NS_string.h"

#if !defined (__ACE_INLINE__)
#include "messageC.inl"
#endif /* !defined INLINE */

// TAO_IDL - Generated from
// be/be_visitor_arg_traits.cpp:73

TAO_BEGIN_VERSIONED_NAMESPACE_DECL


// Arg traits specializations.
namespace TAO
{
  
  // TAO_IDL - Generated from
  // be/be_visitor_arg_traits.cpp:868

#if !defined (_TEST_MESSAGE__ARG_TRAITS_)
#define _TEST_MESSAGE__ARG_TRAITS_
  
  template<>
  class Arg_Traits<Test::Message>
    : public
        Var_Size_Arg_Traits_T<
            Test::Message,
            TAO::Any_Insert_Policy_Stream <Test::Message>
          >
  {
  };

#endif /* end #if !defined */
}

TAO_END_VERSIONED_NAMESPACE_DECL



// TAO_IDL - Generated from
// be/be_visitor_typecode/struct_typecode.cpp:84

static TAO::TypeCode::Struct_Field<
  char const *,
  ::CORBA::TypeCode_ptr const *> const 
    _tao_fields_Test_Message[] =
      {
        { "inst_name", &CORBA::_tc_string },
        { "time", &CORBA::_tc_long }
      };
  
static TAO::TypeCode::Struct<
  char const *,
  ::CORBA::TypeCode_ptr const *,
  TAO::TypeCode::Struct_Field<
    char const *,
    ::CORBA::TypeCode_ptr const *> const *,
  TAO::Null_RefCount_Policy>
_tao_tc_Test_Message (
  ::CORBA::tk_struct,
  "IDL:Test/Message:1.0",
  "Message",
  _tao_fields_Test_Message,
  2);

namespace Test
{
  ::CORBA::TypeCode_ptr const _tc_Message =
    &_tao_tc_Test_Message;
}



// TAO_IDL - Generated from 
// be/be_visitor_structure/structure_cs.cpp:66

void 
Test::Message::_tao_any_destructor (
    void *_tao_void_pointer
  )
{
  Message *_tao_tmp_pointer =
    static_cast<Message *> (_tao_void_pointer);
  delete _tao_tmp_pointer;
}

// TAO_IDL - Generated from
// be/be_visitor_interface/interface_cs.cpp:60

// Traits specializations for Test::Hello.

Test::Hello_ptr
TAO::Objref_Traits<Test::Hello>::duplicate (
    Test::Hello_ptr p)
{
  return Test::Hello::_duplicate (p);
}

void
TAO::Objref_Traits<Test::Hello>::release (
    Test::Hello_ptr p)
{
  ::CORBA::release (p);
}

Test::Hello_ptr
TAO::Objref_Traits<Test::Hello>::nil (void)
{
  return Test::Hello::_nil ();
}

::CORBA::Boolean
TAO::Objref_Traits<Test::Hello>::marshal (
    const Test::Hello_ptr p,
    TAO_OutputCDR & cdr)
{
  return ::CORBA::Object::marshal (p, cdr);
}

// Function pointer for collocation factory initialization.
TAO::Collocation_Proxy_Broker * 
(*Test__TAO_Hello_Proxy_Broker_Factory_function_pointer) (
    ::CORBA::Object_ptr obj
  ) = 0;

// TAO_IDL - Generated from
// be/be_visitor_operation/operation_cs.cpp:78

void Test::Hello::send (
    const ::Test::Message & message)
{
  if (!this->is_evaluated ())
    {
      ::CORBA::Object::tao_object_initialize (this);
    }
  
  if (this->the_TAO_Hello_Proxy_Broker_ == 0)
    {
      Test_Hello_setup_collocation ();
    }
  
  TAO::Arg_Traits< void>::ret_val _tao_retval;
  TAO::Arg_Traits< ::Test::Message>::in_arg_val _tao_message (message);
  
  TAO::Argument *_the_tao_operation_signature [] =
    {
      &_tao_retval,
      &_tao_message
    };
  
  TAO::Invocation_Adapter _tao_call (
      this,
      _the_tao_operation_signature,
      2,
      "send",
      4,
      this->the_TAO_Hello_Proxy_Broker_,
      TAO::TAO_ONEWAY_INVOCATION
    );
  
  _tao_call.invoke (0, 0);
}

Test::Hello::Hello (void)
 : the_TAO_Hello_Proxy_Broker_ (0)
{
  this->Test_Hello_setup_collocation ();
}

void
Test::Hello::Test_Hello_setup_collocation ()
{
  if (::Test__TAO_Hello_Proxy_Broker_Factory_function_pointer)
    {
      this->the_TAO_Hello_Proxy_Broker_ =
        ::Test__TAO_Hello_Proxy_Broker_Factory_function_pointer (this);
    }
}

Test::Hello::~Hello (void)
{}

void 
Test::Hello::_tao_any_destructor (void *_tao_void_pointer)
{
  Hello *_tao_tmp_pointer =
    static_cast<Hello *> (_tao_void_pointer);
  ::CORBA::release (_tao_tmp_pointer);
}

Test::Hello_ptr
Test::Hello::_narrow (
    ::CORBA::Object_ptr _tao_objref
  )
{
  return
    TAO::Narrow_Utils<Hello>::narrow (
        _tao_objref,
        "IDL:Test/Hello:1.0",
        Test__TAO_Hello_Proxy_Broker_Factory_function_pointer
      );
}

Test::Hello_ptr
Test::Hello::_unchecked_narrow (
    ::CORBA::Object_ptr _tao_objref
  )
{
  return
    TAO::Narrow_Utils<Hello>::unchecked_narrow (
        _tao_objref,
        "IDL:Test/Hello:1.0",
        Test__TAO_Hello_Proxy_Broker_Factory_function_pointer
      );
}

Test::Hello_ptr
Test::Hello::_duplicate (Hello_ptr obj)
{
  if (! ::CORBA::is_nil (obj))
    {
      obj->_add_ref ();
    }
  
  return obj;
}

void
Test::Hello::_tao_release (Hello_ptr obj)
{
  ::CORBA::release (obj);
}

::CORBA::Boolean
Test::Hello::_is_a (const char *value)
{
  if (
      !ACE_OS::strcmp (
          value,
          "IDL:Test/Hello:1.0"
        ) ||
      !ACE_OS::strcmp (
          value,
          "IDL:omg.org/CORBA/Object:1.0"
        )
    )
    {
      return true; // success using local knowledge
    }
  else
    {
      return this->::CORBA::Object::_is_a (value);
    }
}

const char* Test::Hello::_interface_repository_id (void) const
{
  return "IDL:Test/Hello:1.0";
}

::CORBA::Boolean
Test::Hello::marshal (TAO_OutputCDR &cdr)
{
  return (cdr << this);
}

// TAO_IDL - Generated from
// be/be_visitor_typecode/objref_typecode.cpp:76

static TAO::TypeCode::Objref<char const *,
                             TAO::Null_RefCount_Policy>
  _tao_tc_Test_Hello (
    ::CORBA::tk_objref,
    "IDL:Test/Hello:1.0",
    "Hello");
  
namespace Test
{
  ::CORBA::TypeCode_ptr const _tc_Hello =
    &_tao_tc_Test_Hello;
}



// TAO_IDL - Generated from 
// be/be_visitor_structure/any_op_cs.cpp:54


TAO_BEGIN_VERSIONED_NAMESPACE_DECL

// Copying insertion.
void operator<<= (
    ::CORBA::Any &_tao_any,
    const Test::Message &_tao_elem
  )
{
  if (0 == &_tao_elem) // Trying to de-reference NULL object
    _tao_any <<= static_cast<Test::Message *>( 0 ); // Use non-copying insertion of a NULL
  else
    TAO::Any_Dual_Impl_T<Test::Message>::insert_copy (
        _tao_any,
        Test::Message::_tao_any_destructor,
        Test::_tc_Message,
        _tao_elem
      );
}

// Non-copying insertion.
void operator<<= (
    ::CORBA::Any &_tao_any,
    Test::Message *_tao_elem
  )
{
  TAO::Any_Dual_Impl_T<Test::Message>::insert (
      _tao_any,
      Test::Message::_tao_any_destructor,
      Test::_tc_Message,
      _tao_elem
    );
}

// Extraction to non-const pointer (deprecated).
::CORBA::Boolean operator>>= (
    const ::CORBA::Any &_tao_any,
    Test::Message *&_tao_elem
  )
{
  return _tao_any >>= const_cast<
      const Test::Message *&> (
      _tao_elem
    );
}

// Extraction to const pointer.
::CORBA::Boolean operator>>= (
    const ::CORBA::Any &_tao_any,
    const Test::Message *&_tao_elem
  )
{
  return
    TAO::Any_Dual_Impl_T<Test::Message>::extract (
        _tao_any,
        Test::Message::_tao_any_destructor,
        Test::_tc_Message,
        _tao_elem
      );
}
TAO_END_VERSIONED_NAMESPACE_DECL



// TAO_IDL - Generated from
// be/be_visitor_interface/any_op_cs.cpp:51
TAO_BEGIN_VERSIONED_NAMESPACE_DECL



namespace TAO
{
  template<>
  ::CORBA::Boolean
  Any_Impl_T<Test::Hello>::to_object (
      ::CORBA::Object_ptr &_tao_elem
    ) const
  {
    _tao_elem = ::CORBA::Object::_duplicate (this->value_);
    return true;
  }
}
TAO_END_VERSIONED_NAMESPACE_DECL



#if defined (ACE_ANY_OPS_USE_NAMESPACE)

namespace Test
{
  
  
  /// Copying insertion.
  void
  operator<<= (
      ::CORBA::Any &_tao_any,
      Hello_ptr _tao_elem)
  {
    Hello_ptr _tao_objptr =
      Hello::_duplicate (_tao_elem);
    _tao_any <<= &_tao_objptr;
  }
  
  /// Non-copying insertion.
  void
  operator<<= (
      ::CORBA::Any &_tao_any,
      Hello_ptr *_tao_elem)
  {
    TAO::Any_Impl_T<Hello>::insert (
        _tao_any,
        Hello::_tao_any_destructor,
        _tc_Hello,
        *_tao_elem)
  }
  
  ::CORBA::Boolean
  operator>>= (
      const ::CORBA::Any &_tao_any,
      Hello_ptr &_tao_elem)
  {
    return
      TAO::Any_Impl_T<Hello>::extract (
          _tao_any,
          Hello::_tao_any_destructor,
          _tc_Hello,
          _tao_elem)
  }
}

#else

TAO_BEGIN_VERSIONED_NAMESPACE_DECL



/// Copying insertion.
void
operator<<= (
    ::CORBA::Any &_tao_any,
    Test::Hello_ptr _tao_elem)
{
  Test::Hello_ptr _tao_objptr =
    Test::Hello::_duplicate (_tao_elem);
  _tao_any <<= &_tao_objptr;
}

/// Non-copying insertion.
void
operator<<= (
    ::CORBA::Any &_tao_any,
    Test::Hello_ptr *_tao_elem)
{
  TAO::Any_Impl_T<Test::Hello>::insert (
      _tao_any,
      Test::Hello::_tao_any_destructor,
      Test::_tc_Hello,
      *_tao_elem);
}

::CORBA::Boolean
operator>>= (
    const ::CORBA::Any &_tao_any,
    Test::Hello_ptr &_tao_elem)
{
  return
    TAO::Any_Impl_T<Test::Hello>::extract (
        _tao_any,
        Test::Hello::_tao_any_destructor,
        Test::_tc_Hello,
        _tao_elem);
}

TAO_END_VERSIONED_NAMESPACE_DECL



#endif

// TAO_IDL - Generated from
// be/be_visitor_structure/cdr_op_cs.cpp:61


TAO_BEGIN_VERSIONED_NAMESPACE_DECL

::CORBA::Boolean operator<< (
    TAO_OutputCDR &strm,
    const Test::Message &_tao_aggregate
  )
{
  return
    (strm << _tao_aggregate.inst_name.in ()) &&
    (strm << _tao_aggregate.time);
}

::CORBA::Boolean operator>> (
    TAO_InputCDR &strm,
    Test::Message &_tao_aggregate
  )
{
  return
    (strm >> _tao_aggregate.inst_name.out ()) &&
    (strm >> _tao_aggregate.time);
}

TAO_END_VERSIONED_NAMESPACE_DECL



// TAO_IDL - Generated from
// be/be_visitor_interface/cdr_op_cs.cpp:63

TAO_BEGIN_VERSIONED_NAMESPACE_DECL

::CORBA::Boolean operator<< (
    TAO_OutputCDR &strm,
    const Test::Hello_ptr _tao_objref)
{
  ::CORBA::Object_ptr _tao_corba_obj = _tao_objref;
  return (strm << _tao_corba_obj);
}

::CORBA::Boolean operator>> (
    TAO_InputCDR &strm,
    Test::Hello_ptr &_tao_objref)
{
  ::CORBA::Object_var obj;
  
  if (!(strm >> obj.inout ()))
    {
      return false;
    }
  
  typedef ::Test::Hello RHS_SCOPED_NAME;
  
  // Narrow to the right type.
  _tao_objref =
    TAO::Narrow_Utils<RHS_SCOPED_NAME>::unchecked_narrow (
        obj.in (),
        Test__TAO_Hello_Proxy_Broker_Factory_function_pointer
      );
    
  return true;
}

TAO_END_VERSIONED_NAMESPACE_DECL


