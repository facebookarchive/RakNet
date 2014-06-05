/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "IncludeAllTests.h"

#include "RakString.h"
#include "DS_List.h"
#include "Gets.h"

using namespace RakNet;
/*

The TestInterface used in this was made to be able to be flexible
when adding new test cases. While I do not use the paramslist, it is available.

*/

int main(int argc, char *argv[])
{

	int returnVal;
	char str[512];
	int numTests=0;
	int testListSize=0;
	int passedTests=0;
	DataStructures::List <TestInterface*> testList;//Pointer list
	DataStructures::List <int> testResultList;//A list of pass and/or fail and the error codes
	DataStructures::List <RakString> testsToRun;//A list of tests to run
	DataStructures::List <int> testsToRunIndexes;//A list of tests to run by index

	//Add all the tests to the test list
	testList.Push(new EightPeerTest(),_FILE_AND_LINE_);
	testList.Push(new MaximumConnectTest(),_FILE_AND_LINE_);
	testList.Push(new PeerConnectDisconnectWithCancelPendingTest(),_FILE_AND_LINE_);
	testList.Push(new PeerConnectDisconnectTest(),_FILE_AND_LINE_);
	testList.Push(new ManyClientsOneServerBlockingTest(),_FILE_AND_LINE_);
	testList.Push(new ManyClientsOneServerNonBlockingTest(),_FILE_AND_LINE_);
	testList.Push(new ManyClientsOneServerDeallocateBlockingTest(),_FILE_AND_LINE_);
	testList.Push(new ReliableOrderedConvertedTest(),_FILE_AND_LINE_);
	testList.Push(new DroppedConnectionConvertTest(),_FILE_AND_LINE_);
	testList.Push(new ComprehensiveConvertTest(),_FILE_AND_LINE_);
	testList.Push(new CrossConnectionConvertTest(),_FILE_AND_LINE_);
	testList.Push(new PingTestsTest(),_FILE_AND_LINE_);
	testList.Push(new OfflineMessagesConvertTest(),_FILE_AND_LINE_);
	testList.Push(new LocalIsConnectedTest(),_FILE_AND_LINE_);
	testList.Push(new SecurityFunctionsTest(),_FILE_AND_LINE_);
	testList.Push(new ConnectWithSocketTest(),_FILE_AND_LINE_);
	testList.Push(new SystemAddressAndGuidTest(),_FILE_AND_LINE_);	
	testList.Push(new PacketAndLowLevelTestsTest(),_FILE_AND_LINE_);
	testList.Push(new MiscellaneousTestsTest(),_FILE_AND_LINE_);

	testListSize=testList.Size();

	bool isVerbose=true;
	bool disallowTestToPause=false;

	DataStructures::List<RakString> testcases;

	if (argc>1)//we have command line arguments
	{

		for (int p=1;p<argc;p++)
		{
			testsToRun.Push(argv[p],_FILE_AND_LINE_);

		}

	}

	DataStructures::List<RakString> noParamsList;

	if (testsToRun.Size()==0&&testsToRunIndexes.Size()==0)
	{
		numTests=testListSize;
		for(int i=0;i<testListSize;i++)
		{
			printf("\n\nRunning test %s.\n\n",testList[i]->GetTestName().C_String());
			returnVal=testList[i]->RunTest(noParamsList,isVerbose,disallowTestToPause);
			testList[i]->DestroyPeers();

			if (returnVal==0)
			{passedTests++;}
			else
			{
				printf("Test %s returned with error %s",testList[i]->GetTestName().C_String(),testList[i]->ErrorCodeToString(returnVal).C_String());
		
			}
		}

	}

	if (testsToRun.Size()!=0)//If string arguments convert to indexes.Suggestion: if speed becoms an issue keep a sorted list and binary search it
	{
		int TestsToRunSize= testsToRun.Size();

		RakString testName;
		for(int i=0;i<TestsToRunSize;i++)
		{
			testName=testsToRun[i];

			for(int j=0;j<testListSize;j++)
			{

				if (testList[j]->GetTestName().StrICmp(testName)==0)
				{

					testsToRunIndexes.Push(j,_FILE_AND_LINE_);

				}

			}

		}

	}

	if (testsToRunIndexes.Size()!=0)//Run selected indexes
	{
		numTests=testsToRunIndexes.Size();

		for(int i=0;i<numTests;i++)
		{

			if (testsToRunIndexes[i]<testListSize)
			{

				printf("\n\nRunning test %s.\n\n",testList[testsToRunIndexes[i]]->GetTestName().C_String());
				returnVal=testList[testsToRunIndexes[i]]->RunTest(noParamsList,isVerbose,disallowTestToPause);
				testList[i]->DestroyPeers();

				if (returnVal==0)
				{passedTests++;}
				else
				{
					printf("Test %s returned with error %s",testList[testsToRunIndexes[i]]->GetTestName().C_String(),testList[testsToRunIndexes[i]]->ErrorCodeToString(returnVal).C_String());

				}
			}
		}
	}

	if (numTests>0)
	{
		printf("\nPassed %i out of %i tests.\n",passedTests,numTests);
	}

	printf("Press enter to continue \n");
	Gets(str, sizeof(str));
	//Cleanup
	int len=testList.Size();

	for (int i=0;i<len;i++)
	{
		delete testList[i];

	}
	testList.Clear(false,_FILE_AND_LINE_);

	return 0;
}

