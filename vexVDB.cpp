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


//static vdbGrid oneGrid();

//bool                    vdbGrid::fileIsOpened_ = false;
//openvdb::io::File       vdbGrid::vdbFile_("/home/green/Downloads/teapot.vdb");
//openvdb::GridBase::Ptr  vdbGrid::baseGrid_;// = vdbGrid::vdbFile_.readGrid("density");
//UT_Vector3*             RainData::pointInitialPositions_;

vdbGrid::vdbGrid(){
    
    openvdb::initialize();
    vdbFile_ = new openvdb::io::File("/home/green/Downloads/teapot.vdb");
    //printf("isOpen: %i\n", vdbFile_->isOpen());
    //printf("file version: %li\n", (long int)vdbFile_->fileVersion());
    //printf("openning... ");
    vdbFile_->open();
    //printf("OK\n");
    baseGrid_ = vdbFile_->readGrid("density");
    setState(true);
}

vdbGrid::~vdbGrid(){
    //printf("closing... ");    
    vdbFile_->close();
    //printf("OK\n");
    delete vdbFile_;
}


void vdbGrid::initialize(const char* fileName)
{
    // openvdb::initialize();

    // openvdb::GridBase::Ptr baseGrid_;

    // printf("isOpen: %i\n", vdbFile_.isOpen());
    // printf("openning... ");
    // vdbFile_.open();
    // printf("OK\n");
    
    // if(vdbFile_.hasGrid("density")){
    //     printf("file version: %li\n", (long int)vdbFile_.fileVersion());
    //     baseGrid_ = vdbFile_.readGrid("density");        
    // }     

    // setState(true);
}

void vdbGrid::cleanup(){
    // printf("closing... ");
    // vdbFile_.close();
    // printf("OK\n");
    // vdbGrid::setState(false);
}

static vdbGrid* singleGrid = NULL;

static void *
pre_readVDB()
{
    if(!singleGrid)
        singleGrid = new vdbGrid();
        //printf("singleGrid created\n");
    return singleGrid;
    
    // bool isfileNameValid = true;
    // const char* fileName("/home/green/Downloads/teapot.vdb");
    // bool isfileNameValid = UTisValidRegularFile( fileName );


    // if( (singleGrid->isInitialized() == 0) && (isfileNameValid == true) )
    // {
    //     singleGrid->initialize(fileName);
    // }
    //vdbGrid oneGrid;


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
    //vdbGrid::cleanup();
    vdbGrid *grid = (vdbGrid*)data;

    UT_ASSERT(grid == singleGrid);

    delete grid;
    //printf("singleGrid deleted\n");
    singleGrid = NULL;

}

// static void
// gamma_Evaluate(int, void *argv[], void *data)
// {
//     float       *result = (float *)argv[0];
//     const float *value = (const float *)argv[1];

//     gamma_Table *table = (gamma_Table *)data;
//     *result = table->evaluate(*value);
// }


void readVDB(int argc, void *argv[], void *data)
{

    float* result = (float *)argv[0];
    const char* fileName( (const char *)argv[1] );
    const UT_Vector3* worldCoord = (const UT_Vector3 *)argv[2];

    //printf("Is Initialized 1: %i\n", vdbGrid::isInitialized());

    vdbGrid* theGrid = (vdbGrid*)data;

    //printf("Is Initialized 2:%i\n", vdbGrid::isInitialized());
    //printf("1: %i\n", isfileNameValid);

    //float value = 0.001;
    //float* cellValue;
    //*cellValue = value;
    float pAccesedValue = 0;
    if(singleGrid->isInitialized())
    {
        // if( isfileNameValid == 1 )
        // {   
        //     //printf("foo\n");
        //     //printf("1: %s\n", fileName);
        //     openvdb::io::File file( fileName );

        //     file.open();
        //     openvdb::GridBase::Ptr baseGrid_;
        //     baseGrid_ = file.readGrid("density");
        //     //printf("%s\n", baseGrid_->type());
        //     //cout << baseGrid_->activeVoxelCount () << endl;
//==
        const openvdb::Vec3d samplePosition(worldCoord->x(),worldCoord->y(),worldCoord->z());
        const openvdb::Vec3i indexPosition = theGrid->baseGrid_->worldToIndex(samplePosition);

        openvdb::FloatGrid::Ptr grid = openvdb::gridPtrCast<openvdb::FloatGrid>(theGrid->baseGrid_);
        openvdb::FloatGrid::Accessor accessor = grid->getAccessor();
        const openvdb::Coord xyz(indexPosition);
        pAccesedValue = accessor.getValue(xyz);
//==

        //     file.close();
        // }
        // else
        // {
        //     printf("Error reading file %s\n", fileName);
        // }

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

