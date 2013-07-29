#ifndef VEXVDB
#define VEXVDB

//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <time.h>
//#include <math.h>
#include <UT/UT_DSOVersion.h>
//#include <UT/UT_Thread.h>
#include <VEX/VEX_VexOp.h>
//#include <UT/UT_WorkBuffer.h>

#include <cstring>
#include <UT/UT_DirUtil.h>
#include <UT/UT_String.h>
#include <UT/UT_Vector.h>

#include <openvdb/openvdb.h>

#include <openvdb/tools/Filter.h>
#include <openvdb/tools/Interpolation.h>
#include <openvdb/tree/ValueAccessor.h>

using namespace std;

class vdbGrid {
public:
	vdbGrid();
    vdbGrid(int mode);
    ~vdbGrid();

    bool                                        isFileOpened_;
    openvdb::GridBase::Ptr                      baseGrid_;
    openvdb::io::File*                          vdbFile_;
    string                                      fileName_;
    openvdb::Vec3fGrid::Ptr                     typedGrid_; 
    openvdb::GridPtrVec 						gridsVector_;
    openvdb::tools::Filter<openvdb::Vec3fGrid>*  filteredGrid_;
private:

};


#endif