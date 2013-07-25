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

// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <time.h>
// #include <math.h>
// #include <UT/UT_DSOVersion.h>
// #include <UT/UT_Thread.h>
// #include <VEX/VEX_VexOp.h>
// #include <UT/UT_WorkBuffer.h>

// #include <cstring>
// #include <UT/UT_DirUtil.h>
// #include <UT/UT_String.h>
// #include <UT/UT_Vector.h>

// #include <openvdb/openvdb.h>
#include <vexVDB.h>

using namespace std;




vdbGrid::vdbGrid(){
    
    openvdb::initialize();
    vdbFile_ = new openvdb::io::File("/home/green/Downloads/smoke.vdb");
    vdbFile_->open();
    baseGrid_ = vdbFile_->readGrid("density");
    floatGrid_ = openvdb::gridPtrCast<openvdb::FloatGrid>(baseGrid_);

    filteredGrid_ = new openvdb::tools::Filter<openvdb::FloatGrid>(*floatGrid_);
    filteredGrid_->gaussian();

    isFileOpened_ = true;
}

vdbGrid::~vdbGrid(){
    vdbFile_->close();
    delete vdbFile_;
    delete filteredGrid_;
}

static vdbGrid* singleGrid = NULL;

static void *
pre_readVDB()
{
    if(!singleGrid)
        singleGrid = new vdbGrid();
    return singleGrid;
    return NULL;
}

static void
post_readVDB(void *data)
{

    vdbGrid *grid = (vdbGrid*)data;
    UT_ASSERT(grid == singleGrid);

    delete grid;
    //printf("singleGrid deleted\n");
    singleGrid = NULL;

}


void readVDB(int argc, void *argv[], void *data)
{
    float* result = (float *)argv[0];
    //const char* fileName( (const char *)argv[1] );
    const UT_Vector3* worldCoord = (const UT_Vector3 *)argv[2];
    vdbGrid* theGrid = (vdbGrid*)data;


    float pAccesedValue = 0;
    if(theGrid->isFileOpened_){


        openvdb::FloatGrid::Accessor accessor = theGrid->floatGrid_->getAccessor();

        const openvdb::Vec3d samplePosition(worldCoord->x(),worldCoord->y(),worldCoord->z());
        const openvdb::Vec3i indexPosition = theGrid->floatGrid_->worldToIndex(samplePosition);
        const openvdb::Coord xyz(indexPosition);


        //pAccesedValue = accessor.getValue(xyz);

        //SAMPLING WORKING
        //openvdb::tools::GridSampler<openvdb::tree::ValueAccessor<openvdb::FloatTree>, openvdb::tools::BoxSampler > sampler(accessor, grid->transform());
        openvdb::tools::GridSampler<openvdb::FloatTree, openvdb::tools::BoxSampler>  sampler(theGrid->floatGrid_->constTree(), theGrid->floatGrid_->transform());
        pAccesedValue = sampler.wsSample(samplePosition);



        *result = pAccesedValue;
    }

}

void newVEXOp(void *)
{
    new VEX_VexOp ( "readVDB@&FSV",
                    readVDB,
                    VEX_ALL_CONTEXT,
                    pre_readVDB,
                    post_readVDB);
                    //VEX_OPTIMIZE_0);

}

