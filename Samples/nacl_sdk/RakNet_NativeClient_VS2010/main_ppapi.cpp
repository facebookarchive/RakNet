/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include <cstdio>
#include <string>
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

// KMJ
#include "ppapi/c/private/ppb_udp_socket_private.h"
#include "RakPeerInterface.h"
#include "RakNetTypes.h"
#include "RakSleep.h"
using namespace RakNet;

/// The Instance class.  One of these exists for each instance of your NaCl
/// module on the web page.  The browser will ask the Module object to create
/// a new Instance for each occurence of the <embed> tag that has these
/// attributes:
///     type="application/x-nacl"
///     src="hello_tutorial.nmf"
/// To communicate with the browser, you must override HandleMessage() for
/// receiving messages from the browser, and use PostMessage() to send messages
/// back to the browser.  Note that this interface is asynchronous.
class ConnectivityTestInstance : public pp::Instance {
	RakPeerInterface *rakPeer;
 public:
  /// The constructor creates the plugin-side instance.
  /// @param[in] instance the handle to the browser-side plugin instance.
  explicit ConnectivityTestInstance(PP_Instance instance) : pp::Instance(instance)
  {
  	// PostMessage(pp::Var("ConnectivityTestInstance constructor"));
		/*
		RakNet::ConnectionAttemptResult car = rakPeer->Connect("127.0.0.1", 1234, "Rumpelstiltskin", (int) strlen("Rumpelstiltskin"));
		RakAssert(car==RakNet::CONNECTION_ATTEMPT_STARTED);
		RakSleep(100);
		RakNet::Packet* p;
		for (p=rakPeer->Receive(); p; rakPeer->DeallocatePacket(p), p=rakPeer->Receive())
			{}
			*/
  	}
  virtual ~ConnectivityTestInstance() {}

  /// Handler for messages coming in from the browser via postMessage().  The
  /// @a var_message can contain anything: a JSON string; a string that encodes
  /// method names and arguments; etc.  For example, you could use
  /// JSON.stringify in the browser to create a message that contains a method
  /// name and some parameters, something like this:
  ///   var json_message = JSON.stringify({ "myMethod" : "3.14159" });
  ///   nacl_module.postMessage(json_message);
  /// On receipt of this message in @a var_message, you could parse the JSON to
  /// retrieve the method name, match it to a function call, and then call it
  /// with the parameter.
  /// @param[in] var_message The message posted by the browser.
  virtual void HandleMessage(const pp::Var& var_message) {
  	/*
	  if (!var_message.is_string())
	    return;
	  std::string message = var_message.AsString();
	  pp::Var var_reply;
	  if (message == kHelloString) {
	    var_reply = pp::Var(kReplyString);
	    PostMessage(var_reply);
	  }
	  */
	  // Just some do nothing code so I can set a breakpoint;
	  rakPeer=0;
	  	  
	  	  /*
	  rakPeer=RakNet::RakPeerInterface::GetInstance();
  	RakNet::SocketDescriptor socketDescriptor(0,0);
		socketDescriptor.socketFamily=AF_INET;
		StartupResult sr = rakPeer->Startup(8,&socketDescriptor, 1);
		if (sr==RakNet::RAKNET_STARTED)
		{
			PostMessage(pp::Var("RAKNET_STARTED"));
		}
		else
		{
			PostMessage(pp::Var("RakNet did not start"));
		}
		
		RakNet::ConnectionAttemptResult car = rakPeer->Connect("127.0.0.1", 1234, "Rumpelstiltskin", (int) strlen("Rumpelstiltskin"));
		RakAssert(car==RakNet::CONNECTION_ATTEMPT_STARTED);
		RakSleep(0);
		RakNet::Packet* p;
		for (p=rakPeer->Receive(); p; rakPeer->DeallocatePacket(p), p=rakPeer->Receive())
			{}
			*/
	}
};

/// The Module class.  The browser calls the CreateInstance() method to create
/// an instance of your NaCl module on the web page.  The browser creates a new
/// instance for each <embed> tag with type="application/x-nacl".
class ConnectivityTestModule : public pp::Module {
 public:
  ConnectivityTestModule() : pp::Module() {}
  virtual ~ConnectivityTestModule() {}

  /// Create and return a ConnectivityTestInstance object.
  /// @param[in] instance The browser-side instance.
  /// @return the plugin-side instance.
  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return new ConnectivityTestInstance(instance);
  }
};

namespace pp {
/// Factory function called by the browser when the module is first loaded.
/// The browser keeps a singleton of this module.  It calls the
/// CreateInstance() method on the object you return to make instances.  There
/// is one instance per <embed> tag on the page.  This is the main binding
/// point for your NaCl module with the browser.
Module* CreateModule() {
  return new ConnectivityTestModule();
}
}  // namespace pp
