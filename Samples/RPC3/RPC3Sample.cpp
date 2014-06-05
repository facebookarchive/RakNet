/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "RPC3.h"
#include "RakPeerInterface.h"

#include <stdio.h>
#include "Kbhit.h"
#include <string.h>
#include <stdlib.h>
#include "BitStream.h"
#include "MessageIdentifiers.h"
#include "StringCompressor.h"
#include "RakSleep.h"
#include "NetworkIDObject.h"
#include "NetworkIDManager.h"
#include "GetTime.h"
#include "Gets.h"

// This has to be a pointer, because it uses UNASSIGNED_NETWORK_ID, initialized globally
RakNet::RPC3 *rpc3Inst;

struct NormalizedVector
{
public:
	NormalizedVector() {x=1.0f; y=0.0f; z=0.0f;}
	float x,y,z;
};

// Shift operators have to be in the namespace RakNet or they might use the default one in BitStream.h instead. Error occurs with std::string
namespace RakNet
{
	// Specialize the << and >> operator to serialize each element of NormalizedVector individually
	// This allows us to endian swap each parameter (which could not otherwise happen) and also send the data in a compressed form
	RakNet::BitStream& operator<<(RakNet::BitStream& out, NormalizedVector& in)
	{
		out.WriteNormVector(in.x,in.y,in.z);
		return out;
	}
	RakNet::BitStream& operator>>(RakNet::BitStream& in, NormalizedVector& out)
	{
		bool success = in.ReadNormVector(out.x,out.y,out.z);
		assert(success);
		return in;
	}
}

class C;
class D;

class A {
public: A() {a=1;} int a;};
class B {
public:
	B() {b=2;} int b;
	virtual void ClassMemberFunc(A *a1, A &a2, C *c1, D *d1, RakNet::BitStream *bs1, RakNet::BitStream &bs2, RakNet::RPC3 *rpc3Inst);
};
class C : public A, public B, public RakNet::NetworkIDObject {
public:
	C() {c=3;} int c;
	virtual void ClassMemberFunc(A *a1, A &a2, C *c1, D *d1, RakNet::BitStream *bs1, RakNet::BitStream &bs2, RakNet::RPC3 *rpc3Inst);
	void ClassMemberFunc2(RakNet::RPC3 *rpc3Inst);
	virtual void TestSlot(void) {printf("C::TestSlot\n");}
};

class D : public B, public RakNet::NetworkIDObject {
public:
	D() {for (int i=0; i < 10; i++) tenBytes[i]=i;}
	char tenBytes[10];
	bool Verify(void) {for (int i=0; i < 10; i++) if (tenBytes[i]!=i) return false; return true;}
	virtual void TestSlot(void) {printf("D::TestSlot\n");}
};

// The number of parameters for a C++ function is limited by BOOST_FUSION_INVOKE_MAX_ARITY-1 found in boost/fusion/functional/invocation/limits.hpp
// I define this in RPC3_Boost.h to be 10. The default is 6.
// rpcFromNetwork is automatically filled in from the RPC class. Pass 0 when calling locally. Will be set to the plugin instance when this function is called from the remote system
void B::ClassMemberFunc(A *a1, A &a2, C *c1, D *d1, RakNet::BitStream *bs1, RakNet::BitStream &bs2, RakNet::RPC3 *rpcFromNetwork){
	if (rpcFromNetwork==0)
		printf("\nB::ClassMemberFunc called locally\n");
	else
		printf("\nB::ClassMemberFunc called from %s\n", rpcFromNetwork->GetLastSenderAddress().ToString());
	printf("a1=%i a2=%i c1=%i\n", a1->a, a2.a, c1->c);
	printf("d1::Verify=%i\n", d1->Verify());
	RakNet::RakString rs1, rs2;
	bs1->Read(rs1);
	bs2.Read(rs2);
	printf("rs1=%s\n", rs1.C_String());
	printf("rs2=%s\n", rs2.C_String());
	printf("rpc3Inst=%p\n", rpc3Inst);
}

// C and D derive from networkIDObject, so cannot be passed as references. A pointer is required to do the object lookup
void C::ClassMemberFunc(A *a1, A &a2, C *c1, D *d1, RakNet::BitStream *bs1, RakNet::BitStream &bs2, RakNet::RPC3 *rpcFromNetwork)	{
	printf("\nC::ClassMemberFunc\n");
	B::ClassMemberFunc(a1,a2,c1,d1,bs1,bs2,rpcFromNetwork);

	if (rpcFromNetwork==0)
	{
		// The RakNet::RPC3 * parameter can be passed to Call() if you want to - it is skipped and not serialized or deserialized so it doesn't matter.
		// The point of it is so when this function is called on the remote system, it is set to the instance of the plugin so you can query network parameters from the caller
		// 
		// By default, pointers to objects that derive from NetworkIDObject (classes C and D), only transmit the NetworkID of the object.
		// If you also want to dereference the pointer and serialize the object itself, use RakNet::_RPC3::Deref(myVariable)
		// In this case, parameters that both derive from NetworkIDObject and are pointers are the variables c1 and d1
		// c1 will only transmit c1->GetNetworkID() (default behavior)
		// d1 will transmit d1->GetNetworkID() and also bitStream << (*d1) (contents of the pointer)
		//
		rpc3Inst->CallCPP("&C::ClassMemberFunc", GetNetworkID(), a1,a2,c1,RakNet::_RPC3::Deref(d1),bs1,bs2,rpcFromNetwork);
	}
}	

void C::ClassMemberFunc2(RakNet::RPC3 *rpcFromNetwork)	{
	printf("\nC::ClassMemberFunc2\n");

	if (rpcFromNetwork==0)
	{
		// The RakNet::RPC3 * parameter can be passed to Call() if you want to - it is skipped and not serialized or deserialized so it doesn't matter.
		// The point of it is so when this function is called on the remote system, it is set to the instance of the plugin so you can query network parameters from the caller
		// 
		// By default, pointers to objects that derive from NetworkIDObject (classes C and D), only transmit the NetworkID of the object.
		// If you also want to dereference the pointer and serialize the object itself, use RakNet::_RPC3::Deref(myVariable)
		// In this case, parameters that both derive from NetworkIDObject and are pointers are the variables c1 and d1
		// c1 will only transmit c1->GetNetworkID() (default behavior)
		// d1 will transmit d1->GetNetworkID() and also bitStream << (*d1) (contents of the pointer)
		//
		rpc3Inst->CallCPP("&C::ClassMemberFunc2", GetNetworkID(), rpcFromNetwork);
	}
}	

// The number of parameters for a C function is limited by BOOST_FUSION_INVOKE_MAX_ARITY found in boost/fusion/functional/invocation/limits.hpp
// I define this in RPC3_Boost.h to be 9. The default is 6.
void CFunc(RakNet::RakString rakString, int intArray[10], C *c1, const char *str, NormalizedVector *nv1, NormalizedVector &nv2, RakNet::RPC3 *rpcFromNetwork )
{
	// We pass 0 to the rpcFromNetwork when calling this function locally. When it is called by the RPC3 system, it is set to the address of the plugin
	if (rpcFromNetwork==0)
		printf("\nCFunc called locally\n");
	else
		printf("\nCFunc called from %s\n", rpcFromNetwork->GetLastSenderAddress().ToString());
	printf("rakString=%s\n", rakString.C_String());
	printf("intArray = ");
	for (int i=0; i < 10; i++)
		printf("%i ", intArray[i]);
	printf("\n");
	printf("c1=%i\n", c1->c);
	printf("str=%s\n", str);
	printf("nv1->x=%f nv1->y=%f nv1->z=%f\n", nv1->x, nv1->y, nv1->z);
	printf("nv2.x=%f nv2.y=%f nv2.z=%f\n", nv2.x, nv2.y, nv2.z);
	printf("rpc3Inst=%p\n", rpc3Inst);

	// the parameter "int intArray[10]" is actually a pointer due to the design of C and C++
	// The RakNet::_RPC3::PtrToArray() function will tell the RPC3 system that this is actually an array of n elements
	// Each element will be endian swapped appropriately
	if (rpcFromNetwork==0)
		rpc3Inst->CallC("CFunc", rakString,RakNet::_RPC3::PtrToArray(10,intArray),c1,str,nv1,nv2,rpcFromNetwork);
}
int main(void)
{
	printf("Demonstration of the RPC3 plugin.\n");
	printf("It is similar to Raknet's RPC system, but automatically\n");
	printf("serializes and deserializes the parameters to the function call\n");
	printf("Difficulty: Intermediate\n\n");

	DataStructures::OrderedList<int,int> ol;
	ol.Insert(3,3,false,_FILE_AND_LINE_);
	ol.Insert(4,4,false,_FILE_AND_LINE_);
	ol.Insert(5,5,false,_FILE_AND_LINE_);
	ol.Insert(4,4,false,_FILE_AND_LINE_);
	bool objectExists;
	int idx = ol.GetIndexFromKey(4,&objectExists);

	RakNet::RakPeerInterface *rakPeer;
	RakNet::SystemAddress tempAddr = RakNet::UNASSIGNED_SYSTEM_ADDRESS;
	A a;
	B b;
	C c;
	D d;

	NormalizedVector normalizedVector;
	RakNet::TimeMS stage2=0;
	RakNet::NetworkIDManager networkIDManager;
	rpc3Inst = new RakNet::RPC3;
	rpc3Inst->SetNetworkIDManager(&networkIDManager);
	RakNet::NetworkID idZero, idOne;
	idZero=0;
	idOne=1;
	rpc3Inst->SetNetworkIDManager(&networkIDManager);
	c.SetNetworkIDManager(&networkIDManager);
	d.SetNetworkIDManager(&networkIDManager);
	c.SetNetworkID(idZero);
	d.SetNetworkID(idOne);

	// Register a regular C function, and a class member function
	// Unlike AutoRPC, you don't have to specify if it is C or C++, or the number of parameters
	RPC3_REGISTER_FUNCTION(rpc3Inst, CFunc);
	// Note the & operator as the macro and RPC3::RegisterFunction takes a class pointer
	RPC3_REGISTER_FUNCTION(rpc3Inst, &C::ClassMemberFunc);
	RPC3_REGISTER_FUNCTION(rpc3Inst, &C::ClassMemberFunc2);

	// All slots are called when a signal is sent. This is true whether the slot is local or remote
	rpc3Inst->RegisterSlot("TestSlot",&C::TestSlot, c.GetNetworkID(), 0);
	rpc3Inst->RegisterSlot("TestSlot",&D::TestSlot, d.GetNetworkID(), 0);

	printf("(S)erver or (C)lient?: ");
	bool isServer;
	char str[256];
	Gets(str, sizeof(str));
	if (str[0]=='s' || str[0]=='S')
		isServer=true;
	else
		isServer=false;

	rakPeer = RakNet::RakPeerInterface::GetInstance();
	if (isServer)
	{
		RakNet::SocketDescriptor socketDescriptor(50000,0);
		socketDescriptor.socketFamily=AF_INET; // Only IPV4 supports broadcast on 255.255.255.255
		rakPeer->Startup(10, &socketDescriptor, 1);
		rakPeer->SetMaximumIncomingConnections(10);
		printf("Server started.\n");
	}
	else
	{
		RakNet::SocketDescriptor socketDescriptor(0,0);
		socketDescriptor.socketFamily=AF_INET; // Only IPV4 supports broadcast on 255.255.255.255
		rakPeer->Startup(1, &socketDescriptor, 1);

		// Send out a LAN broadcast to find other instances on the same computer
		rakPeer->Ping( "255.255.255.255", 50000, true, 0 );

		printf("Client started. Will automatically connect to running servers.\n");
	}
	rakPeer->AttachPlugin(rpc3Inst);

	RakNet::Packet *p;
	while (1)
	{
		for (p=rakPeer->Receive(); p; rakPeer->DeallocatePacket(p), p=rakPeer->Receive())
		{
			switch (p->data[0])
			{
			case ID_DISCONNECTION_NOTIFICATION:
				printf("ID_DISCONNECTION_NOTIFICATION\n");
				break;
			case ID_ALREADY_CONNECTED:
				printf("ID_ALREADY_CONNECTED\n");
				break;
			case ID_CONNECTION_ATTEMPT_FAILED:
				printf("Connection attempt failed\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("ID_NO_FREE_INCOMING_CONNECTIONS\n");
				break;
			case ID_UNCONNECTED_PONG:
				// Found the server
				rakPeer->Connect(p->systemAddress.ToString(false),p->systemAddress.GetPort(),0,0,0);
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
				// This tells the client they have connected
				printf("ID_CONNECTION_REQUEST_ACCEPTED\n");
				break;
			case ID_NEW_INCOMING_CONNECTION:
			{
				RakNet::BitStream testBitStream1, testBitStream2;
				testBitStream1.Write("Hello World 1");
				testBitStream2.Write("Hello World 2");
				c.ClassMemberFunc(&a,a,&c,&d,&testBitStream1,testBitStream2,0);
				c.ClassMemberFunc2(0);
				RakNet::RakString rs("RakString test");
				int intArray[10];
				for (int i=0; i < sizeof(intArray)/sizeof(int); i++)
					intArray[i]=i;
				CFunc(rs, intArray,&c,"Test string",&normalizedVector,normalizedVector,0);
				stage2=RakNet::GetTimeMS()+500;
				break;
			}				
			case ID_RPC_REMOTE_ERROR:
				{
					// Recipient system returned an error
					switch (p->data[1])
					{
					case RakNet::RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE:
						printf("RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE\n");
						break;
					case RakNet::RPC_ERROR_OBJECT_DOES_NOT_EXIST:
						printf("RPC_ERROR_OBJECT_DOES_NOT_EXIST\n");
						break;
					case RakNet::RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE:
						printf("RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE\n");
						break;
					case RakNet::RPC_ERROR_FUNCTION_NOT_REGISTERED:
						printf("RPC_ERROR_FUNCTION_NOT_REGISTERED\n");
						break;
					case RakNet::RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED:
						printf("RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED\n");
						break;
					case RakNet::RPC_ERROR_CALLING_CPP_AS_C:
						printf("RPC_ERROR_CALLING_CPP_AS_C\n");
						break;
					case RakNet::RPC_ERROR_CALLING_C_AS_CPP:
						printf("RPC_ERROR_CALLING_C_AS_CPP\n");
						break;
					}
					printf("Function: %s", p->data+2);
				}
			}
		}

		if (stage2 && stage2 < RakNet::GetTimeMS())
		{
			stage2=0;

			RakNet::BitStream testBitStream1, testBitStream2;
			testBitStream1.Write("Hello World 1 (2)");
			testBitStream2.Write("Hello World 2 (2)");
			c.ClassMemberFunc(&a,a,&c,&d,&testBitStream1,testBitStream2,0);
			RakNet::RakString rs("RakString test (2)");
			int intArray[10];
			for (int i=0; i < sizeof(intArray)/sizeof(int); i++)
				intArray[i]=i;
			CFunc(rs, intArray,&c,"Test string (2)",&normalizedVector,normalizedVector,0);
			rpc3Inst->Signal("TestSlot");
		}

		RakSleep(0);
	}

	rakPeer->Shutdown(100,0);
	RakNet::RakPeerInterface::DestroyInstance(rakPeer);
	delete rpc3Inst;

	return 1;
}
