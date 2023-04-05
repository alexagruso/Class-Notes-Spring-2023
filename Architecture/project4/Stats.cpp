/******************************
 * Submitted by: Alex Agruso
 * CS 3339 - Spring 2022, Texas State University
 * Project 3 Pipelining
 * Copyright 2022, Lee B. Hinkle, all rights reserved
 * Based on prior work by Martin Burtscher and Molly O'Neil
 * Redistribution in source or binary form, with or without modification,
 * is *not* permitted. Use in source or binary form, with or without
 * modification, is only permitted for academic use in CS 3339 at
 * Texas State University.
 ******************************/

#include "Stats.h"

Stats::Stats()
{
    cycles = PIPESTAGES - 1; // pipeline startup cost
    flushes = 0;
    bubbles = 0;

    memops = 0;
    branches = 0;
    taken = 0;

    hazards = 0;

    for (int i = IF1; i < PIPESTAGES; i++)
    {
        resultReg[i] = -1;
        resultStage[i] = 0;
        stageHazards[i] = 0;
    }
}

void Stats::clock()
{
    cycles++;

    // advance all stages in pipeline
    for (int i = WB; i > IF1; i--)
    {
        resultReg[i] = resultReg[i - 1];
        resultStage[i] = resultStage[i - 1];
    }
    // inject a NOP in pipestage IF1
    resultReg[IF1] = -1;
    resultStage[IF1] = 0;
}

// r == the register being read
void Stats::registerSrc(int r, PIPESTAGE stage)
{
    for (int i = EXE1; i < WB; i++)
    {
        if (resultReg[i] == r)
        {
            int bubbles = (resultStage[i] - i) - (stage - ID);

            for (int j = 0; j < bubbles; j++)
            {
                bubble();
            }

            hazards++;
            stageHazards[i]++;

            break;
        }
    }
}

// r == the register to be written to
void Stats::registerDest(int r, PIPESTAGE stage)
{
    resultReg[ID] = r;
    resultStage[ID] = stage;
}

void Stats::flush(int count)
{
    for (int i = 0; i < count; i++)
    {
        for (int j = WB; j > IF1; j--)
        {
            resultReg[j] = resultReg[j - 1];
            resultStage[j] = resultStage[j - 1];
        }

        resultReg[IF1] = -1;
        resultStage[IF1] = 0;
    }

    flushes += count;
    cycles += count;
}

void Stats::bubble()
{
    for (int i = WB; i > EXE1; i--)
    {
        resultReg[i] = resultReg[i - 1];
        resultStage[i] = resultStage[i - 1];
    }

    resultReg[EXE1] = -1;
    resultStage[EXE1] = 0;

    bubbles++;
    cycles++;
}

void Stats::showPipe()
{
    // this method is to assist testing and debug, please do not delete or edit
    // you are welcome to use it but remove any debug outputs before you submit
    cout << "              IF1  IF2 *ID* EXE1 EXE2 MEM1 MEM2 WB         #C      #B      #F" << endl;
    cout << "  resultReg ";
    for (int i = 0; i < PIPESTAGES; i++)
    {
        cout << "  " << dec << setw(2) << resultReg[i] << " ";
    }
    cout << "   " << setw(7) << cycles << " " << setw(7) << bubbles << " " << setw(7) << flushes;
    cout << endl;
}
