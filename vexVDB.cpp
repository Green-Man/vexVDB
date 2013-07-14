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
#include <UT/UT_DirUtil.h>
#include <UT/UT_String.h>
#include <UT/UT_Vector.h>

#include <openvdb/openvdb.h>

using namespace std;





class vdbGrid {
public:
     vdbGrid() { }
    ~vdbGrid() { }

    //float       evaluate(float v)       { return 0; }

    //int         myRefCount;
};

static vdbGrid      *oneGrid = NULL;

static void *
pre_readVDB()
{

    // if (!theGammaTable)
    //     theGammaTable = new gamma_Table();
    // else
    //     theGammaTable->myRefCount++;
    // return theGammaTable;
    return NULL;
}

static void
post_readVDB(void *data)
{
    // gamma_Table *table = (gamma_Table *)data;

    // UT_ASSERT(table == theGammaTable);
    // table->myRefCount--;
    // if (!table->myRefCount)
    // {
    //     delete table;
    //     theGammaTable = NULL;
    // }
}

// static void
// gamma_Evaluate(int, void *argv[], void *data)
// {
//     float       *result = (float *)argv[0];
//     const float *value = (const float *)argv[1];

//     gamma_Table *table = (gamma_Table *)data;
//     *result = table->evaluate(*value);
// }







void readVDB(int argc, void *argv[], void *)
{

    float* result = (float *)argv[0];
    const char* filepath( (const char *)argv[1] );
    const UT_Vector3* worldCoord = (const UT_Vector3 *)argv[2];
    const bool isFilepathValid = UTisValidRegularFile( filepath );
    //printf("0: %i\n", isFilepathValid);
    openvdb::initialize();
    //printf("1: %i\n", isFilepathValid);

    //float value = 0.001;
    //float* cellValue;
    
    //*cellValue = value;
    float pAccesedValue = 0;
    if( isFilepathValid == 1 )
    {   
        //printf("foo\n");
        //printf("1: %s\n", filepath);
        openvdb::io::File file( filepath );

        file.open();
        openvdb::GridBase::Ptr baseGrid;
        baseGrid = file.readGrid("density");
        //printf("%s\n", baseGrid->type());
        //cout << baseGrid->activeVoxelCount () << endl;

        const openvdb::Vec3d samplePosition(worldCoord->x(),worldCoord->y(),worldCoord->z());
        const openvdb::Vec3i indexPosition = baseGrid->worldToIndex(samplePosition);
        //printf("pos: %4.2f %4.2f %4.2f\n", worldCoord->x(), worldCoord->y(), worldCoord->z());
        //printf("index: %4.2i %4.2i %4.2i\n\n", indexPosition[0], indexPosition[1], indexPosition[2]);

        openvdb::FloatGrid::Ptr grid = openvdb::gridPtrCast<openvdb::FloatGrid>(baseGrid);
        openvdb::FloatGrid::Accessor accessor = grid->getAccessor();
        const openvdb::Coord xyz(indexPosition);
        pAccesedValue = accessor.getValue(xyz);
        //*val = pAccesedValue;

        file.close();
    }
    else
    {
        printf("Error reading file %s\n", filepath);
    }

    *result = pAccesedValue;
}

void newVEXOp(void *)
{
    // new VEX_VexOp ( "stringDivide@&SSS",
    //                 stringDivide,
    //                 VEX_ALL_CONTEXT,
    //                 NULL,
    //                 NULL);
    // new VEX_VexOp ( "splitUDIMPath@SS&S&S&S",
    //                 splitUDIMPath,
    //                 VEX_ALL_CONTEXT,
    //                 NULL,
    //                 NULL);
    new VEX_VexOp ( "readVDB@&FSV",
                    readVDB,
                    VEX_ALL_CONTEXT,
                    NULL,
                    NULL);

}

