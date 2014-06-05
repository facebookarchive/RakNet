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
#include "RakMemoryOverride.h"
#include "RakAssert.h"
#include "StringCompressor.h"
#include "BitStream.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "NetworkIDManager.h"
#include <stdlib.h>

using namespace RakNet;

// int RPC3::RemoteRPCFunctionComp( const RPC3::RPCIdentifier &key, const RemoteRPCFunction &data )
// {
// 	return strcmp(key.C_String(), data.identifier.C_String());
// }
int RakNet::RPC3::LocalSlotObjectComp( const LocalSlotObject &key, const LocalSlotObject &data )
{
	if (key.callPriority>data.callPriority)
		return -1;
	if (key.callPriority==data.callPriority)
	{
		if (key.registrationCount<data.registrationCount)
			return -1;
		if (key.registrationCount==data.registrationCount)
			return 0;
		return 1;
	}

	return 1;
}
RPC3::RPC3()
{
	currentExecution[0]=0;
	networkIdManager=0;
	outgoingTimestamp=0;
	outgoingPriority=HIGH_PRIORITY;
	outgoingReliability=RELIABLE_ORDERED;
	outgoingOrderingChannel=0;
	outgoingBroadcast=true;
	incomingTimeStamp=0;
	nextSlotRegistrationCount=0;
}

RPC3::~RPC3()
{
	Clear();
}

void RPC3::SetNetworkIDManager(NetworkIDManager *idMan)
{
	networkIdManager=idMan;
}

bool RPC3::UnregisterFunction(const char *uniqueIdentifier)
{
	return false;
}

bool RPC3::IsFunctionRegistered(const char *uniqueIdentifier)
{
	DataStructures::HashIndex i = GetLocalFunctionIndex(uniqueIdentifier);
	return i.IsInvalid()==false;
}

void RPC3::SetTimestamp(RakNet::Time timeStamp)
{
	outgoingTimestamp=timeStamp;
}

void RPC3::SetSendParams(PacketPriority priority, PacketReliability reliability, char orderingChannel)
{
	outgoingPriority=priority;
	outgoingReliability=reliability;
	outgoingOrderingChannel=orderingChannel;
}

void RPC3::SetRecipientAddress(const SystemAddress &systemAddress, bool broadcast)
{
	outgoingSystemAddress=systemAddress;
	outgoingBroadcast=broadcast;
}

void RPC3::SetRecipientObject(NetworkID networkID)
{
	outgoingNetworkID=networkID;
}

RakNet::Time RPC3::GetLastSenderTimestamp(void) const
{
	return incomingTimeStamp;
}

SystemAddress RPC3::GetLastSenderAddress(void) const
{
	return incomingSystemAddress;
}

RakPeerInterface *RPC3::GetRakPeer(void) const
{
	return rakPeerInterface;
}

const char *RPC3::GetCurrentExecution(void) const
{
	return (const char *) currentExecution;
}

bool RPC3::SendCallOrSignal(RakString uniqueIdentifier, char parameterCount, RakNet::BitStream *serializedParameters, bool isCall)
{	
	SystemAddress systemAddr;
//	unsigned int outerIndex;
//	unsigned int innerIndex;

	if (uniqueIdentifier.IsEmpty())
		return false;

	RakNet::BitStream bs;
	if (outgoingTimestamp!=0)
	{
		bs.Write((MessageID)ID_TIMESTAMP);
		bs.Write(outgoingTimestamp);
	}
	bs.Write((MessageID)ID_RPC_PLUGIN);
	bs.Write(parameterCount);
	if (outgoingNetworkID!=UNASSIGNED_NETWORK_ID && isCall)
	{
		bs.Write(true);
		bs.Write(outgoingNetworkID);
	}
	else
	{
		bs.Write(false);
	}
	bs.Write(isCall);
	// This is so the call SetWriteOffset works
	bs.AlignWriteToByteBoundary();
	BitSize_t writeOffset = bs.GetWriteOffset();
	if (outgoingBroadcast)
	{
		unsigned systemIndex;
		for (systemIndex=0; systemIndex < rakPeerInterface->GetMaximumNumberOfPeers(); systemIndex++)
		{
			systemAddr=rakPeerInterface->GetSystemAddressFromIndex(systemIndex);
			if (systemAddr!=RakNet::UNASSIGNED_SYSTEM_ADDRESS && systemAddr!=outgoingSystemAddress)
			{
// 				if (GetRemoteFunctionIndex(systemAddr, uniqueIdentifier, &outerIndex, &innerIndex, isCall))
// 				{
// 					// Write a number to identify the function if possible, for faster lookup and less bandwidth
// 					bs.Write(true);
// 					if (isCall)
// 						bs.WriteCompressed(remoteFunctions[outerIndex]->operator [](innerIndex).functionIndex);
// 					else
// 						bs.WriteCompressed(remoteSlots[outerIndex]->operator [](innerIndex).functionIndex);
// 				}
// 				else
// 				{
// 					bs.Write(false);
					StringCompressor::Instance()->EncodeString(uniqueIdentifier, 512, &bs, 0);
//				}

				bs.WriteCompressed(serializedParameters->GetNumberOfBitsUsed());

				// serializedParameters->PrintBits();

				bs.WriteAlignedBytes((const unsigned char*) serializedParameters->GetData(), serializedParameters->GetNumberOfBytesUsed());
				SendUnified(&bs, outgoingPriority, outgoingReliability, outgoingOrderingChannel, systemAddr, false);

				// Start writing again after ID_AUTO_RPC_CALL
				bs.SetWriteOffset(writeOffset);
			}
		}
	}
	else
	{
		systemAddr = outgoingSystemAddress;
		if (systemAddr!=RakNet::UNASSIGNED_SYSTEM_ADDRESS)
		{
// 			if (GetRemoteFunctionIndex(systemAddr, uniqueIdentifier, &outerIndex, &innerIndex, isCall))
// 			{
// 				// Write a number to identify the function if possible, for faster lookup and less bandwidth
// 				bs.Write(true);
// 				if (isCall)
// 					bs.WriteCompressed(remoteFunctions[outerIndex]->operator [](innerIndex).functionIndex);
// 				else
// 					bs.WriteCompressed(remoteSlots[outerIndex]->operator [](innerIndex).functionIndex);
// 			}
// 			else
// 			{
// 				bs.Write(false);
				StringCompressor::Instance()->EncodeString(uniqueIdentifier, 512, &bs, 0);
//			}

			bs.WriteCompressed(serializedParameters->GetNumberOfBitsUsed());
			bs.WriteAlignedBytes((const unsigned char*) serializedParameters->GetData(), serializedParameters->GetNumberOfBytesUsed());
			SendUnified(&bs, outgoingPriority, outgoingReliability, outgoingOrderingChannel, systemAddr, false);
		}
		else
			return false;
	}
	return true;
}

void RPC3::OnAttach(void)
{
	outgoingSystemAddress=RakNet::UNASSIGNED_SYSTEM_ADDRESS;
	outgoingNetworkID=UNASSIGNED_NETWORK_ID;
	incomingSystemAddress=RakNet::UNASSIGNED_SYSTEM_ADDRESS;
}

PluginReceiveResult RPC3::OnReceive(Packet *packet)
{
	RakNet::Time timestamp=0;
	unsigned char packetIdentifier, packetDataOffset;
	if ( ( unsigned char ) packet->data[ 0 ] == ID_TIMESTAMP )
	{
		if ( packet->length > sizeof( unsigned char ) + sizeof( RakNet::Time ) )
		{
			packetIdentifier = ( unsigned char ) packet->data[ sizeof( unsigned char ) + sizeof( RakNet::Time ) ];
			// Required for proper endian swapping
			RakNet::BitStream tsBs(packet->data+sizeof(MessageID),packet->length-1,false);
			tsBs.Read(timestamp);
			packetDataOffset=sizeof( unsigned char )*2 + sizeof( RakNet::Time );
		}
		else
			return RR_STOP_PROCESSING_AND_DEALLOCATE;
	}
	else
	{
		packetIdentifier = ( unsigned char ) packet->data[ 0 ];
		packetDataOffset=sizeof( unsigned char );
	}

	switch (packetIdentifier)
	{
	case ID_RPC_PLUGIN:
		incomingTimeStamp=timestamp;
		incomingSystemAddress=packet->systemAddress;
		OnRPC3Call(packet->systemAddress, packet->data+packetDataOffset, packet->length-packetDataOffset);
		return RR_STOP_PROCESSING_AND_DEALLOCATE;
// 	case ID_AUTO_RPC_REMOTE_INDEX:
// 		OnRPCRemoteIndex(packet->systemAddress, packet->data+packetDataOffset, packet->length-packetDataOffset);
// 		return RR_STOP_PROCESSING_AND_DEALLOCATE;		
	}

	return RR_CONTINUE_PROCESSING;
}

void RPC3::OnRPC3Call(const SystemAddress &systemAddress, unsigned char *data, unsigned int lengthInBytes)
{
	RakNet::BitStream bs(data,lengthInBytes,false);

	DataStructures::HashIndex functionIndex;
	LocalRPCFunction *lrpcf;
	bool hasParameterCount=false;
	char parameterCount;
	NetworkIDObject *networkIdObject;
	NetworkID networkId;
	bool hasNetworkId=false;
//	bool hasFunctionIndex=false;
//	unsigned int functionIndex;
	BitSize_t bitsOnStack;
	char strIdentifier[512];
	incomingExtraData.Reset();
	bs.Read(parameterCount);
	bs.Read(hasNetworkId);
	if (hasNetworkId)
	{
		bool readSuccess = bs.Read(networkId);
		RakAssert(readSuccess);
		RakAssert(networkId!=UNASSIGNED_NETWORK_ID);
		if (networkIdManager==0)
		{
			// Failed - Tried to call object member, however, networkIDManager system was never registered
			SendError(systemAddress, RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE, "");
			return;
		}
		networkIdObject = networkIdManager->GET_OBJECT_FROM_ID<NetworkIDObject*>(networkId);
		if (networkIdObject==0)
		{
			// Failed - Tried to call object member, object does not exist (deleted?)
			SendError(systemAddress, RPC_ERROR_OBJECT_DOES_NOT_EXIST, "");
			return;
		}
	}
	else
	{
		networkIdObject=0;
	}
	bool isCall;
	bs.Read(isCall);
	bs.AlignReadToByteBoundary();
//	bs.Read(hasFunctionIndex);
//	if (hasFunctionIndex)
//		bs.ReadCompressed(functionIndex);
//	else
		StringCompressor::Instance()->DecodeString(strIdentifier,512,&bs,0);
	bs.ReadCompressed(bitsOnStack);
	RakNet::BitStream serializedParameters;
	if (bitsOnStack>0)
	{
		serializedParameters.AddBitsAndReallocate(bitsOnStack);
		// BITS_TO_BYTES is correct, why did I change this?
		bs.ReadAlignedBytes(serializedParameters.GetData(), BITS_TO_BYTES(bitsOnStack));
		serializedParameters.SetWriteOffset(bitsOnStack);
	}
// 	if (hasFunctionIndex)
// 	{
// 		if (
// 			(isCall==true && functionIndex>localFunctions.Size()) ||
// 			(isCall==false && functionIndex>localSlots.Size())
// 			)
// 		{
// 			// Failed - other system specified a totally invalid index
// 			// Possible causes: Bugs, attempts to crash the system, requested function not registered
// 			SendError(systemAddress, RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE, "");
// 			return;
// 		}
// 	}
// 	else
	{
		// Find the registered function with this str
		if (isCall)
		{
// 			for (functionIndex=0; functionIndex < localFunctions.Size(); functionIndex++)
// 			{
// 				bool isObjectMember = boost::fusion::get<0>(localFunctions[functionIndex].functionPointer);
// 				//		boost::function<_RPC3::InvokeResultCodes (_RPC3::InvokeArgs)> functionPtr = boost::fusion::get<0>(localFunctions[functionIndex].functionPointer);
// 
// 				if (isObjectMember == (networkIdObject!=0) &&
// 					strcmp(localFunctions[functionIndex].identifier.C_String(), strIdentifier)==0)
// 				{
// 					// SEND RPC MAPPING
// 					RakNet::BitStream outgoingBitstream;
// 					outgoingBitstream.Write((MessageID)ID_AUTO_RPC_REMOTE_INDEX);
// 					outgoingBitstream.Write(hasNetworkId);
// 					outgoingBitstream.WriteCompressed(functionIndex);
// 					StringCompressor::Instance()->EncodeString(strIdentifier,512,&outgoingBitstream,0);
// 					outgoingBitstream.Write(isCall);
// 					SendUnified(&outgoingBitstream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, systemAddress, false);
// 					break;
// 				}
// 			}


			functionIndex = localFunctions.GetIndexOf(strIdentifier);
			if (functionIndex.IsInvalid())
			{
				SendError(systemAddress, RPC_ERROR_FUNCTION_NOT_REGISTERED, strIdentifier);
				return;
			}
			lrpcf = localFunctions.ItemAtIndex(functionIndex);

			bool isObjectMember = boost::fusion::get<0>(lrpcf->functionPointer);
			if (isObjectMember==true && networkIdObject==0)
			{
				// Failed - Calling C++ function as C function
				SendError(systemAddress, RPC_ERROR_CALLING_CPP_AS_C, strIdentifier);
				return;
			}

			if (isObjectMember==false && networkIdObject!=0)
			{
				// Failed - Calling C function as C++ function
				SendError(systemAddress, RPC_ERROR_CALLING_C_AS_CPP, strIdentifier);
				return;
			}
		}
		else
		{
			functionIndex = localSlots.GetIndexOf(strIdentifier);
			if (functionIndex.IsInvalid())
			{
				SendError(systemAddress, RPC_ERROR_FUNCTION_NOT_REGISTERED, strIdentifier);
				return;
			}
		}
	}

	if (isCall)
	{
		bool isObjectMember = boost::fusion::get<0>(lrpcf->functionPointer);
		boost::function<_RPC3::InvokeResultCodes (_RPC3::InvokeArgs)> functionPtr = boost::fusion::get<1>(lrpcf->functionPointer);
		//	int arity = boost::fusion::get<2>(localFunctions[functionIndex].functionPointer);
		//	if (isObjectMember)
		//		arity--; // this pointer
		if (functionPtr==0)
		{
			// Failed - Function was previously registered, but isn't registered any longer
			SendError(systemAddress, RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED, strIdentifier);
			return;
		}

		// Boost doesn't support this for class members
		//	if (arity!=parameterCount)
		//	{
		//		// Failed - The number of parameters that this function has was explicitly specified, and does not match up.
		//		SendError(systemAddress, RPC_ERROR_INCORRECT_NUMBER_OF_PARAMETERS, localFunctions[functionIndex].identifier);
		//		return;
		//	}

		_RPC3::InvokeArgs functionArgs;
		functionArgs.bitStream=&serializedParameters;
		functionArgs.networkIDManager=networkIdManager;
		functionArgs.caller=this;
		functionArgs.thisPtr=networkIdObject;
		
		// serializedParameters.PrintBits();

		_RPC3::InvokeResultCodes res2 = functionPtr(functionArgs);
	}
	else
	{
		InvokeSignal(functionIndex, &serializedParameters, false);
	}

}
void RPC3::InterruptSignal(void)
{
	interruptSignal=true;
}
void RPC3::InvokeSignal(DataStructures::HashIndex functionIndex, RakNet::BitStream *serializedParameters, bool temporarilySetUSA)
{
	if (functionIndex.IsInvalid())
		return;

	SystemAddress lastIncomingAddress=incomingSystemAddress;
	if (temporarilySetUSA)
		incomingSystemAddress=RakNet::UNASSIGNED_SYSTEM_ADDRESS;
	interruptSignal=false;
	LocalSlot *localSlot = localSlots.ItemAtIndex(functionIndex);
	unsigned int i;
	_RPC3::InvokeArgs functionArgs;
	functionArgs.bitStream=serializedParameters;
	functionArgs.networkIDManager=networkIdManager;
	functionArgs.caller=this;
	i=0;
	while (i < localSlot->slotObjects.Size())
	{
		if (localSlot->slotObjects[i].associatedObject!=UNASSIGNED_NETWORK_ID)
		{
			functionArgs.thisPtr = networkIdManager->GET_OBJECT_FROM_ID<NetworkIDObject*>(localSlot->slotObjects[i].associatedObject);
			if (functionArgs.thisPtr==0)
			{
				localSlot->slotObjects.RemoveAtIndex(i);
				continue;
			}
		}
		else
			functionArgs.thisPtr=0;
		functionArgs.bitStream->ResetReadPointer();

		bool isObjectMember = boost::fusion::get<0>(localSlot->slotObjects[i].functionPointer);
		boost::function<_RPC3::InvokeResultCodes (_RPC3::InvokeArgs)> functionPtr = boost::fusion::get<1>(localSlot->slotObjects[i].functionPointer);
		if (functionPtr==0)
		{
			if (temporarilySetUSA==false)
			{
				// Failed - Function was previously registered, but isn't registered any longer
				SendError(lastIncomingAddress, RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED, localSlots.KeyAtIndex(functionIndex).C_String());
			}
			return;
		}
		_RPC3::InvokeResultCodes res2 = functionPtr(functionArgs);

		// Not threadsafe
		if (interruptSignal==true)
			break;

		i++;
	}

	if (temporarilySetUSA)
		incomingSystemAddress=lastIncomingAddress;
}
// void RPC3::OnRPCRemoteIndex(const SystemAddress &systemAddress, unsigned char *data, unsigned int lengthInBytes)
// {
// 	// A remote system has given us their internal index for a particular function.
// 	// Store it and use it from now on, to save bandwidth and search time
// 	bool objectExists;
// 	RakString strIdentifier;
// 	unsigned int insertionIndex;
// 	unsigned int remoteIndex;
// 	RemoteRPCFunction newRemoteFunction;
// 	RakNet::BitStream bs(data,lengthInBytes,false);
// 	RPCIdentifier identifier;
// 	bool isObjectMember;
// 	bool isCall;
// 	bs.Read(isObjectMember);
// 	bs.ReadCompressed(remoteIndex);
// 	bs.Read(strIdentifier);
// 	bs.Read(isCall);
// 
// 	if (strIdentifier.IsEmpty())
// 		return;
// 
// 	DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, RPC3::RemoteRPCFunctionComp> *theList;
// 	if (
// 		(isCall==true && remoteFunctions.Has(systemAddress)) ||
// 		(isCall==false && remoteSlots.Has(systemAddress))
// 		)
// 	{
// 		if (isCall==true)
// 			theList = remoteFunctions.Get(systemAddress);
// 		else
// 			theList = remoteSlots.Get(systemAddress);
// 		insertionIndex=theList->GetIndexFromKey(identifier, &objectExists);
// 		if (objectExists==false)
// 		{
// 			newRemoteFunction.functionIndex=remoteIndex;
// 			newRemoteFunction.identifier = strIdentifier;
// 			theList->InsertAtIndex(newRemoteFunction, insertionIndex, _FILE_AND_LINE_ );
// 		}
// 	}
// 	else
// 	{
// 		theList = RakNet::OP_NEW<DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, RPC3::RemoteRPCFunctionComp> >(_FILE_AND_LINE_);
// 
// 		newRemoteFunction.functionIndex=remoteIndex;
// 		newRemoteFunction.identifier = strIdentifier;
// 		theList->InsertAtEnd(newRemoteFunction, _FILE_AND_LINE_ );
// 
// 		if (isCall==true)
// 			remoteFunctions.SetNew(systemAddress,theList);
// 		else
// 			remoteSlots.SetNew(systemAddress,theList);
// 	}
// }

void RPC3::OnClosedConnection(const SystemAddress &systemAddress, RakNetGUID rakNetGUID, PI2_LostConnectionReason lostConnectionReason )
{
// 	if (remoteFunctions.Has(systemAddress))
// 	{
// 		DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, RPC3::RemoteRPCFunctionComp> *theList = remoteFunctions.Get(systemAddress);
// 		delete theList;
// 		remoteFunctions.Delete(systemAddress);
// 	}
// 
// 	if (remoteSlots.Has(systemAddress))
// 	{
// 		DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, RPC3::RemoteRPCFunctionComp> *theList = remoteSlots.Get(systemAddress);
// 		delete theList;
// 		remoteSlots.Delete(systemAddress);
// 	}
}

void RPC3::OnShutdown(void)
{
	// Not needed, and if the user calls Shutdown inadvertantly, it unregisters his functions
	// Clear();
}

void RPC3::Clear(void)
{
	unsigned j;
// 	for (j=0; j < remoteFunctions.Size(); j++)
// 	{
// 		DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, RPC3::RemoteRPCFunctionComp> *theList = remoteFunctions[j];
// 		RakNet::OP_DELETE(theList,_FILE_AND_LINE_);
// 	}
// 	for (j=0; j < remoteSlots.Size(); j++)
// 	{
// 		DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, RPC3::RemoteRPCFunctionComp> *theList = remoteSlots[j];
// 		RakNet::OP_DELETE(theList,_FILE_AND_LINE_);
// 	}

	DataStructures::List<RakNet::RakString> keyList;
	DataStructures::List<LocalSlot*> outputList;
	localSlots.GetAsList(outputList,keyList,_FILE_AND_LINE_);
	for (j=0; j < outputList.Size(); j++)
	{
		RakNet::OP_DELETE(outputList[j],_FILE_AND_LINE_);
	}
	localSlots.Clear(_FILE_AND_LINE_);

	DataStructures::List<LocalRPCFunction*> outputList2;
	localFunctions.GetAsList(outputList2,keyList,_FILE_AND_LINE_);
	for (j=0; j < outputList2.Size(); j++)
	{
		RakNet::OP_DELETE(outputList2[j],_FILE_AND_LINE_);
	}
	localFunctions.Clear(_FILE_AND_LINE_);
//	remoteFunctions.Clear();
//	remoteSlots.Clear();
	outgoingExtraData.Reset();
	incomingExtraData.Reset();
}

void RPC3::SendError(SystemAddress target, unsigned char errorCode, const char *functionName)
{
	RakNet::BitStream bs;
	bs.Write((MessageID)ID_RPC_REMOTE_ERROR);
	bs.Write(errorCode);
	bs.WriteAlignedBytes((const unsigned char*) functionName,(const unsigned int) strlen(functionName)+1);
	SendUnified(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, target, false);
}

// bool RPC3::GetRemoteFunctionIndex(const SystemAddress &systemAddress, RPC3::RPCIdentifier identifier, unsigned int *outerIndex, unsigned int *innerIndex, bool isCall)
// {
// 	bool objectExists=false;
// 	if (isCall)
// 	{
// 		if (remoteFunctions.Has(systemAddress))
// 		{
// 			*outerIndex = remoteFunctions.GetIndexAtKey(systemAddress);
// 			DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, RPC3::RemoteRPCFunctionComp> *theList = remoteFunctions[*outerIndex];
// 			*innerIndex = theList->GetIndexFromKey(identifier, &objectExists);
// 		}
// 	}
// 	else
// 	{
// 		if (remoteSlots.Has(systemAddress))
// 		{
// 			*outerIndex = remoteFunctions.GetIndexAtKey(systemAddress);
// 			DataStructures::OrderedList<RPCIdentifier, RemoteRPCFunction, RPC3::RemoteRPCFunctionComp> *theList = remoteSlots[*outerIndex];
// 			*innerIndex = theList->GetIndexFromKey(identifier, &objectExists);
// 		}
// 	}
// 	return objectExists;
// }
DataStructures::HashIndex RPC3::GetLocalSlotIndex(const char *sharedIdentifier)
{
	return localSlots.GetIndexOf(sharedIdentifier);
}
DataStructures::HashIndex RPC3::GetLocalFunctionIndex(RPC3::RPCIdentifier identifier)
{
	return localFunctions.GetIndexOf(identifier.C_String());
}

