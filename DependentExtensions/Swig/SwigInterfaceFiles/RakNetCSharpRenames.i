//--------------------------Renames--------------------------------
//These are renames, to make the interface a different name than the C++ interface
//There are many reasons to do this sometimes the original function is useful but not perfect and a pure
//C# helper is created. The original is renamed and hidden.
//operators need renaming
//Some items may conflict with C#, they would need renaming

%rename(GET_BASE_OBJECT_FROM_IDORIG) RakNet::NetworkIDManager::GET_BASE_OBJECT_FROM_ID;
%rename(SetNetworkIDManagerOrig) RakNet::NetworkIDObject::SetNetworkIDManager;
%rename(GetBandwidthOrig) RakNet::RakPeer::GetBandwidth;
%rename(GetBandwidthOrig) RakNet::RakPeerInterface::GetBandwidth;
%rename(Equals) operator ==;
%rename(CopyData) operator =;
%rename(OpLess) operator <;
%rename(OpGreater) operator >;
%rename(OpArray) operator [];
%rename(OpLessEquals) operator <=;
%rename(OpGreaterEquals) operator >=;
%rename(OpNotEqual) operator !=;
%rename(OpPlus) operator +;
%rename(OpPlusPlus) operator ++;
%rename(OpMinusMinus) operator --;
%rename(OpMinus) operator -;
%rename(OpDivide) operator /;
%rename(OpMultiply) operator *;