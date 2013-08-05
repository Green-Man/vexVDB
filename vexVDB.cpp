
#include <include/vexVDB.h>
#include <UT/UT_SpinLock.h>


using namespace std;

const string GRIDNAME = "Cd";

//mode: 0 - reading, 1 - writing
vdbGrid::vdbGrid(const char* path, int mode){
    openvdb::initialize();
    vdbFile_ = new openvdb::io::File(path);
    if(mode == 1){
        typedGrid_ = openvdb::Vec3SGrid::create();
        typedGrid_->insertMeta("metadata", openvdb::FloatMetadata(42.0));
        typedGrid_->setTransform(openvdb::math::Transform::createLinearTransform(0.01));
        typedGrid_->setGridClass(openvdb::GRID_FOG_VOLUME);
        typedGrid_->setName(GRIDNAME);

        //const openvdb::Coord min(-1,-1,-1);
        //const openvdb::Coord max(1,1,1);
        //const openvdb::math::CoordBBox bbox(min, max);
        typedGrid_->setBackground(openvdb::Vec3f(0,0,0));
        //typedGrid_->fill(bbox, openvdb::Vec3f(1,0,0), true);
        gridsVector_.push_back(typedGrid_);
    }
    else{
        vdbFile_->open();
        baseGrid_ = vdbFile_->readGrid(GRIDNAME);
        typedGrid_ = openvdb::gridPtrCast<openvdb::Vec3SGrid>(baseGrid_);
        filteredGrid_ = new openvdb::tools::Filter<openvdb::Vec3fGrid>(*typedGrid_);
        filteredGrid_->mean(2);
    }
    isFileOpened_ = true;
}

vdbGrid::~vdbGrid(){
    vdbFile_->close();
    delete vdbFile_;
    delete filteredGrid_;
}

static vdbGrid* singleGrid = NULL;


static void* pre_writeVDB(){
    // if(!singleGrid)
    //     singleGrid = new vdbGrid(1);
    // return singleGrid;
    return NULL;
}

static void post_writeVDB(void *data){
    vdbGrid *theGrid = (vdbGrid*)data;
    
    theGrid->typedGrid_->pruneGrid();
    //writing the file
    
    theGrid->vdbFile_->write(theGrid->gridsVector_);

    UT_ASSERT(theGrid == singleGrid);
    delete theGrid;
    singleGrid = NULL;
}

//static UT_Lock theMutex;


void openVDB(int argc, void *argv[], void *data){
    uintptr_t* handle = (uintptr_t*)argv[0];
    int* openType = (int*)argv[1];

    const char* vdbPath = (const char*)argv[1];
    if(!singleGrid)
        singleGrid = new vdbGrid(vdbPath, *openType);
  
    *handle = (uintptr_t)singleGrid;
    //data = singleGrid;

    //{
        //UT_Lock::Scope  lock(theMutex);
    //}
}

void closeVDB(void *data){
    vdbGrid *theGrid = (vdbGrid*)data; //just a magick!
    UT_ASSERT(theGrid == singleGrid);
    delete theGrid;
    singleGrid = NULL;

}
void writeVDB(int argc, void *argv[], void *data){
    openvdb::Vec3f* pos = (openvdb::Vec3f*)argv[0];
    openvdb::Vec3f* Cd = (openvdb::Vec3f*)argv[1];

    vdbGrid* theGrid = (vdbGrid*)data;

    if(theGrid->isFileOpened_){
        openvdb::Vec3SGrid::Accessor accessor = theGrid->typedGrid_->getAccessor();
    //     const openvdb::Vec3d samplePosition(worldCoord->x(),worldCoord->y(),worldCoord->z());
        const openvdb::Vec3i indexPosition = theGrid->typedGrid_->worldToIndex(*pos);
        const openvdb::Coord ijk(indexPosition);
        accessor.setValue(ijk, *Cd);
        //printf("writing value %f\n", Cd->x());
    }
}

void readVDB(int argc, void *argv[], void *data){
    openvdb::Vec3f* result = (openvdb::Vec3f*)argv[0];

    uintptr_t* handlePtr = (uintptr_t*)argv[1];

    const UT_Vector3* worldCoord = (const UT_Vector3 *)argv[2];
    vdbGrid* theGrid = (vdbGrid*)*handlePtr;

    openvdb::Vec3f pAccesedValue(0,0,0);
    if(theGrid->isFileOpened_){
        openvdb::Vec3fGrid::Accessor accessor = theGrid->typedGrid_->getAccessor();
        const openvdb::Vec3d samplePosition(worldCoord->x(),worldCoord->y(),worldCoord->z());
        const openvdb::Vec3i indexPosition = theGrid->typedGrid_->worldToIndex(samplePosition);
        const openvdb::Coord xyz(indexPosition);
        //pAccesedValue = accessor.getValue(xyz);
        //SAMPLING WORKING
        //openvdb::tools::GridSampler<openvdb::tree::ValueAccessor<openvdb::Vec3fTree>, openvdb::tools::BoxSampler > sampler(accessor, grid->transform());
        //openvdb::tools::GridSampler<openvdb::Vec3fTree, openvdb::tools::BoxSampler>  sampler(theGrid->typedGrid_->constTree(), theGrid->typedGrid_->transform());
        openvdb::tools::GridSampler<openvdb::Vec3fTree, openvdb::tools::PointSampler>  sampler(theGrid->typedGrid_->constTree(), theGrid->typedGrid_->transform());
        pAccesedValue = sampler.wsSample(samplePosition);

        *result = pAccesedValue;
    }
}

void newVEXOp(void *)
{
    new VEX_VexOp ( "readVDB@&VIV",
                    readVDB,
                    VEX_ALL_CONTEXT,
                    NULL,
                    NULL);
                    //VEX_OPTIMIZE_0);
    new VEX_VexOp ( "writeVDB@VV",
                    writeVDB,
                    VEX_ALL_CONTEXT,
                    pre_writeVDB,
                    post_writeVDB);
                    //VEX_OPTIMIZE_0);
    new VEX_VexOp ( "openVDB@&ISI",
                    openVDB,
                    VEX_ALL_CONTEXT,
                    NULL,
                    closeVDB);
                    //VEX_OPTIMIZE_0);
    // new VEX_VexOp ( "closeVDB@I",
    //                 closeVDB,
    //                 VEX_ALL_CONTEXT,
    //                 NULL,
    //                 NULL);
    //                 //VEX_OPTIMIZE_0);
}

