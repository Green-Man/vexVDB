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
#include <UT/UT_Regex.h>
#include <UT/UT_String.h>

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


void stringDivide(int argc, void *argv[], void *)
{
    UT_WorkBuffer buffer;
    buffer.strcpy((char *)argv[1]);
    buffer.strcat((char *)argv[2]);
    //buffer.strcat((char *)"***");
    
    VEX_VexOp::stringFree((char *)argv[0]);
    argv[0] = (void *)VEX_VexOp::stringAlloc( buffer.buffer() );
}


void splitUDIMPath(int argc, void *argv[], void *)
{
    //Read path from 1st parm and split it
    UT_String fullPath((char *)argv[0]);
    UT_String fileName = fullPath.pathUpToExtension();
    

    UT_String fileExt = fullPath.fileExtension();
    
    //Read regexp from 2nd parm
    UT_Regex pattern( ( char* )argv[1] );    
    UT_String suffix;
    UT_WorkBuffer buf;
    if( pattern.search(( char* )fileName) )
    {
        suffix = "_u1_v1";
        pattern.replace(buf, fileName, "");
        fileName = buf.buffer(); 
    }
    else
    {
        suffix = "";
    }

    VEX_VexOp::stringFree((char *)argv[2]);
    VEX_VexOp::stringFree((char *)argv[3]);
    VEX_VexOp::stringFree((char *)argv[4]);

    argv[2] = (void *)VEX_VexOp::stringAlloc( fileName );
    argv[3] = (void *)VEX_VexOp::stringAlloc( suffix );
    argv[4] = (void *)VEX_VexOp::stringAlloc( fileExt );

}

void newVEXOp(void *)
{
    new VEX_VexOp ( "stringDivide@&SSS",
                    stringDivide,
                    VEX_ALL_CONTEXT,
                    NULL,
                    NULL);
    new VEX_VexOp ( "splitUDIMPath@SS&S&S&S",
                    splitUDIMPath,
                    VEX_ALL_CONTEXT,
                    NULL,
                    NULL);

}

// time_Evaluate(int, void *argv[], void *)
// {
//     int         *result = (int *)argv[0];

//     *result = time(0);
// }