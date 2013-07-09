/*
 * Copyright (c) 2013
 *      Side Effects Software Inc.  All rights reserved.
 *
 * Redistribution and use of Houdini Development Kit samples in source and
 * binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. The name of Side Effects Software may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE `AS IS' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *----------------------------------------------------------------------------
 * This is a sample VEX operator DSO
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <UT/UT_DSOVersion.h>
#include <UT/UT_Thread.h>
#include <VEX/VEX_VexOp.h>
#include <UT/UT_WorkBuffer.h>

#include <cstring>

using namespace std;
//namespace HDK_Sample {

// #if !defined(WIN32)
// static void
// drand_Evaluate(int, void *argv[], void *)
// {
//     float       *result = (float *)argv[0];
//     const int   *seed = (const int *)argv[1];

//     srand48(*seed);
//     *result = drand48();
// }
// #endif

// static void
// time_Evaluate(int, void *argv[], void *)
// {
//     int         *result = (int *)argv[0];

//     *result = time(0);
// }

// // Simple class to show shared storage.  A single gamma table is shared between
// // all instances of the gamma() function.
// class gamma_Table {
// public:
//      gamma_Table() : myRefCount(1) { }
//     ~gamma_Table() { }

//     float       evaluate(float v)       { return 0; }

//     int         myRefCount;
// };

// static gamma_Table      *theGammaTable = NULL;

// static void *
// gamma_Init()
// {

//     if (!theGammaTable)
//         theGammaTable = new gamma_Table();
//     else
//         theGammaTable->myRefCount++;
//     return theGammaTable;
// }

// static void
// gamma_Cleanup(void *data)
// {
//     gamma_Table *table = (gamma_Table *)data;

//     UT_ASSERT(table == theGammaTable);
//     table->myRefCount--;
//     if (!table->myRefCount)
//     {
//         delete table;
//         theGammaTable = NULL;
//     }
// }

// static void
// gamma_Evaluate(int, void *argv[], void *data)
// {
//     float       *result = (float *)argv[0];
//     const float *value = (const float *)argv[1];

//     gamma_Table *table = (gamma_Table *)data;
//     *result = table->evaluate(*value);
// }

// static void
// myprint_Evaluate(int argc, VEX_VexOpArg argv[], void *data)
// {
//     printf("%d args:\n", argc);
//     for (int i = 0; i < argc; i++)
//     {
//         if (argv[i].myArray)
//             continue; // Doesn't support arrays
//         switch (argv[i].myType)
//         {
//             case VEX_TYPE_INTEGER:
//                 printf("  int %d\n", *(const int *)argv[i].myArg);
//                 break;
//             case VEX_TYPE_FLOAT:
//                 printf("  float %f\n", *(const float *)argv[i].myArg);
//                 break;
//             case VEX_TYPE_STRING:
//                 printf("  string %s\n", (const char *)argv[i].myArg);
//                 break;
//             default:
//                 break;
//         }
//     }
// }

// }

// //
// // Installation function
// //
// using namespace HDK_Sample;
// void
// newVEXOp(void *)
// {
// #if !defined(WIN32)
//     //  Returns a random number based on the seed argument
//     new VEX_VexOp("drand@&FI",          // Signature
//                 drand_Evaluate,         // Evaluator
//                 VEX_ALL_CONTEXT,        // Context mask
//                 NULL,                   // init function
//                 NULL);                  // cleanup function
// #endif

//     // Return the time() function.  This is non-deterministic, so the
//     // optimization level has to be lowered.
//     new VEX_VexOp("time@&I",            // Signature
//                 time_Evaluate,          // Evaluator
//                 VEX_ALL_CONTEXT,        // Context mask
//                 NULL,                   // init function
//                 NULL,                   // cleanup function
//                 VEX_OPTIMIZE_1);        // Optimization level

//     // Use the default optimization (better performance)
//     new VEX_VexOp("gamma@&FF",          // Signature
//                 gamma_Evaluate,         // Evaluator
//                 VEX_ALL_CONTEXT,        // Context mask
//                 gamma_Init,             // init function
//                 gamma_Cleanup);         // Cleanup function

//     // A variadic function to print integers and floats
//     new VEX_VexOp("myprint@+",          // Signature
//                 myprint_Evaluate,       // Evaluator
//                 VEX_ALL_CONTEXT,        // Context mask
//                 NULL,                   // init function
//                 NULL,                   // Cleanup function
//                 VEX_OPTIMIZE_0);        // Optimization level

static void divide_string(int, void *argv[], void *)
{
    UT_WorkBuffer   wbuf;

    char myString[3];
    strcpy(myString ,"foo");

    cout << myString << endl;

    wbuf.strcpy(myString);
    //VEX_VexOp::stringFree((const char *)argv[0]);

    //argv[0] = VEX_VexOp::stringAlloc(wbuf.buffer());
}

void newVEXOp(void *)
{
    new VEX_VexOp ( "string_divide@&SS",
                    divide_string,
                    VEX_ALL_CONTEXT,
                    NULL,
                    NULL);
}

// time_Evaluate(int, void *argv[], void *)
// {
//     int         *result = (int *)argv[0];

//     *result = time(0);
// }