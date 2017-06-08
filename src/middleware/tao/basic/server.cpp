#include <iostream>
#include "server.h"
#include "tao/IORTable/IORTable.h"

Hello::Hello (CORBA::ORB_ptr orb)
: orb_ (CORBA::ORB::_duplicate (orb))
{
}


void Hello::send(const Test::Message& message){
    std::cout << "\ttime: " << message.time << std::endl;
    std::cout << "\tinst_name: " << message.inst_name << std::endl;
}


int main(int argc, char ** argv){
    //Initialize the orb
    CORBA::ORB_var orb = CORBA::ORB_init (argc, argv);

    // Get a reference to the RootPOA.
    CORBA::Object_var obj = orb->resolve_initial_references ("RootPOA");

    // Get the POA_var object from Object_var.
    PortableServer::POA_var root_poa = PortableServer::POA::_narrow (obj.in());

    // Get the POAManager of the RootPOA.
    PortableServer::POAManager_var poa_manager = root_poa->the_POAManager ();

    //Activate the POA manager
    poa_manager->activate ();
    
    // Construct the policy list for the LoggingServerPOA.
    CORBA::PolicyList policies (2);
    policies.length (2);
    policies[0] = root_poa->create_id_assignment_policy (PortableServer::USER_ID);
    policies[1] = root_poa->create_lifespan_policy (PortableServer::PERSISTENT);

    // Create the child POA for the test logger factory servants.
    ::PortableServer::POA_var child_poa = root_poa->create_POA ("LoggingServerPOA", poa_manager.in (), policies);

     // Destroy the POA policies
    for (::CORBA::ULong i = 0; i < policies.length (); ++ i){
        policies[i]->destroy ();
    }
      
    auto mgr = child_poa->the_POAManager ();
    mgr->activate ();
    
    
    Hello *hello_impl = 0;
    ACE_NEW_RETURN (hello_impl, Hello (orb.in ()), 1);
    Test::Hello_var hello = hello_impl;
    //Transfer ownership
    PortableServer::ServantBase_var owner_transfer(hello_impl);
    //Get the ID
    PortableServer::ObjectId_var id = root_poa->activate_object (hello_impl);
    //Get the object
    
    CORBA::Object_var object = root_poa->id_to_reference (id.in ());
    //Construct a narrow?
    //Test::Hello_var hello = Test::Hello::_narrow (object.in ());
    
    //Construct 
    CORBA::String_var ior = orb->object_to_string (hello.in ());

    std::cout << "Acrtivated Impl:" << std::endl;

     // Locate the IORTable for the application.
    ::CORBA::Object_var temp = orb->resolve_initial_references ("IORTable");
    ::IORTable::Table_var ior_table = IORTable::Table::_narrow (temp.in ());

    if (::CORBA::is_nil (ior_table.in ()))
        ACE_ERROR_RETURN ((LM_ERROR,
                        ACE_TEXT ("%T (%t) - %M - failed to resolve IOR table\n")),
                        1);

    
    std::cout << "Acrtivated Impl:" << std::endl;
    

    


    ior_table->bind ("LoggingServer", ior.in ());


    // Get the object reference.
    //CORBA::Object_var stock_factory = child_poa->id_to_reference (oid.in ());

    // Stringify all the object referencs.
    //CORBA::String_var ior = orb->object_to_string (stock_factory.in ());
    // Print them out !
    std::cout << "RUNNING ORB:" << std::endl;

    orb-> run ();

    // Destroy POA, waiting until the destruction terminates.
    root_poa->destroy (1, 1);
    orb->destroy ();
}