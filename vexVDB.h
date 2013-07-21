#ifndef VEXVDB
#define VEXVDB

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
    vdbGrid();
    ~vdbGrid();

    //float       evaluate(float v)       { return 0; }

    //int         myRefCount;
    bool isInitialized()
    {
        return fileIsOpened_;
    }
    void setState(bool state)
    {
        fileIsOpened_ = state;
    }
    void initialize(const char* fileName);
    void cleanup();


    bool                        fileIsOpened_;
    openvdb::GridBase::Ptr      baseGrid_;
    openvdb::io::File*           vdbFile_;
private:
};


#endif