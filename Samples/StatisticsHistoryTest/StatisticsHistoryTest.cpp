/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

// TODO - see PLplot plplot.sourceforge.net/download.phpd for visualization


#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include "GetTime.h"
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "StatisticsHistory.h"
#include <math.h>
#include "RakSleep.h"

using namespace RakNet;

enum HistoryObject
{
	HO_SIN_WAVE,
	HO_COS_WAVE,
};
static const int numGraphColumns=79;
static const int numGraphRows=5;

void PrintGraph(DataStructures::Queue<StatisticsHistory::TimeAndValue> &histogram, SHValueType highest, SHValueType lowest)
{
	if (histogram.Size()==0)
		return;
	

	SHValueType range = highest - lowest;
	bool drawPoint[numGraphRows][numGraphColumns];
	memset(drawPoint, 0, sizeof(drawPoint));

	unsigned int numR;

	for (unsigned int c = 0; c < histogram.Size() && c < numGraphColumns; c++)
	{
		double r = (double) numGraphRows * ((histogram[c].val-lowest) / range);
		if (r - floor(r)>.5)
			r=ceil(r);
		else
			r=floor(r);
		numR = (unsigned int) r;
		for (unsigned int r = 0; r < numR && r < numGraphRows; r++)
		{
			drawPoint[r][c]=true;
		}
	}

	for (int r=numGraphRows-1; r >= 0; r--)
	{
		for (unsigned int c = 0; c < numGraphColumns; c++)
		{
			if (drawPoint[r][c])
				printf("|");
			else
				printf(" ");
		}
		printf("\n");
	}
	for (unsigned int c = 0; c < numGraphColumns; c++)
		printf("-");
	printf("\n\n");
}
int main(void)
{
	printf("Tests StatisticsHistory in single player.\n");
	printf("Difficulty: Intermediate\n\n");

	DataStructures::Queue<StatisticsHistory::TimeAndValue> histogram;
	StatisticsHistory::TimeAndValueQueue *tav;
	StatisticsHistory::TimeAndValueQueue tavInst;
	StatisticsHistory statisticsHistory;
	statisticsHistory.SetDefaultTimeToTrack(10000);
	statisticsHistory.AddObject(StatisticsHistory::TrackedObjectData(HO_SIN_WAVE,0,0));
	statisticsHistory.AddObject(StatisticsHistory::TrackedObjectData(HO_COS_WAVE,0,0));
	double f;

	Time nextPrint=0;
	while (1)
	{
		f = (double) ((double)GetTime() / (double)1000);
		statisticsHistory.AddValueByObjectID(HO_SIN_WAVE,"Waveform",sin(f),GetTime(), false);
		statisticsHistory.AddValueByObjectID(HO_COS_WAVE,"Waveform",cos(f),GetTime(), false);

		// Show sin wave
		if (GetTime()>nextPrint)
		{
			system("cls");

			Time curTime = GetTime();

			statisticsHistory.GetHistoryForKey(HO_SIN_WAVE, "Waveform", &tav, curTime);
			tav->ResizeSampleSet(numGraphColumns, histogram, StatisticsHistory::DC_CONTINUOUS);
			PrintGraph(histogram, 1, -1);


			// Show cos wave
			statisticsHistory.GetHistoryForKey(HO_COS_WAVE, "Waveform", &tav, curTime);
			tav->ResizeSampleSet(numGraphColumns, histogram, StatisticsHistory::DC_CONTINUOUS);
			PrintGraph(histogram, 1, -1);

			nextPrint = GetTime() + 500;

			// Show sin wave + cos wave
			statisticsHistory.MergeAllObjectsOnKey("Waveform", &tavInst, StatisticsHistory::DC_CONTINUOUS);
			tavInst.ResizeSampleSet(numGraphColumns, histogram, StatisticsHistory::DC_CONTINUOUS);
			PrintGraph(histogram, 2, -2);
		}

		RakSleep(30);
	}
	

	return 1;
}

