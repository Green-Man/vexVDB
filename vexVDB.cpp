
#include <include/vexVDB.h>
#include <UT/UT_SpinLock.h>


using namespace std;

const string GRIDNAME = "Cd";

//mode: 0 - reading, 1 - writing
vdbGrid::vdbGrid(const char* path, int mode){
    isFileOpened_ = false;
    fileName_ = path;
    openvdb::initialize();
    vdbFile_ = new openvdb::io::File(path);
    if(mode == 1){
        typedGrid_ = openvdb::Vec3SGrid::create();
        typedGrid_->insertMeta("metadata", openvdb::FloatMetadata(42.0));
        typedGrid_->setTransform(openvdb::math::Transform::createLinearTransform(0.005));
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
        filteredGrid_->mean(1);
        isFileOpened_ = true;
    }

}

vdbGrid::~vdbGrid(){
    if(isFileOpened_){
        vdbFile_->close();
        isFileOpened_ = false;
    }
    delete vdbFile_;
    if(filteredGrid_)
        delete filteredGrid_;
}

static vdbGrid* singleGrid = NULL;


static UT_Lock theMutex;
void openVDB(int argc, void *argv[], void *data){
    int* openType = (int*)argv[2];
    const char* vdbPath = (const char*)argv[1];
    if(!singleGrid){
        singleGrid = new vdbGrid(vdbPath, *openType);   
    }
    uintptr_t* handle = (uintptr_t*)argv[0];    
    *handle = (uintptr_t)singleGrid; 
}

void closeVDB(void *data){
    // uintptr_t* handlePtr = (uintptr_t*)data;
    // vdbGrid *theGrid = (vdbGrid*)*handlePtr;  
    // UT_ASSERT(theGrid == singleGrid);
    // if(theGrid) 
    //     delete theGrid;
    // singleGrid = NULL;
}

static void * pre_VDB(){
    uintptr_t* p = new uintptr_t;
    return p;
}

static void post_writeVDB(void *data){

        
        uintptr_t* handlePtr = (uintptr_t*)data;
        vdbGrid *theGrid = (vdbGrid*)*handlePtr;
        {
            UT_Lock::Scope  lock(theMutex);
            theGrid->typedGrid_->pruneGrid(2);

            //writing the file
            theGrid->vdbFile_->write(theGrid->gridsVector_);

            UT_ASSERT(theGrid == singleGrid);
            if(theGrid) 
                delete theGrid;
        }
        singleGrid = NULL;

}

void writeVDB(int argc, void *argv[], void *data){

        uintptr_t* handlePtr = (uintptr_t*)argv[0];
        uintptr_t* tmpHandle = (uintptr_t*)data;
        *tmpHandle = *handlePtr;

        vdbGrid* theGrid = (vdbGrid*)*handlePtr;
        openvdb::Vec3f* pos = (openvdb::Vec3f*)argv[1];
        openvdb::Vec3f* Cd = (openvdb::Vec3f*)argv[2];
        //{
            //UT_Lock::Scope  lock(theMutex);
        //}

        openvdb::Vec3SGrid::Accessor accessor = theGrid->typedGrid_->getAccessor();
        //     const openvdb::Vec3d samplePosition(worldCoord->x(),worldCoord->y(),worldCoord->z());
        const openvdb::Vec3i indexPosition = theGrid->typedGrid_->worldToIndex(*pos);
        const openvdb::Coord ijk(indexPosition);
        accessor.setValue(ijk, *Cd);


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
    new VEX_VexOp ( "writeVDB@IVV",
                    writeVDB,
                    VEX_ALL_CONTEXT,
                    pre_VDB,
                    post_writeVDB);
                    //VEX_OPTIMIZE_0);
    new VEX_VexOp ( "openVDB@&ISI",
                    openVDB,
                    VEX_ALL_CONTEXT,
                    pre_VDB,
                    closeVDB);
                    //VEX_OPTIMIZE_0);
    // new VEX_VexOp ( "closeVDB@I",
    //                 closeVDB,
    //                 VEX_ALL_CONTEXT,
    //                 NULL,
    //                 NULL);
    //                 //VEX_OPTIMIZE_0);
}

