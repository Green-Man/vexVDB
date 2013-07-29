
#include <include/vexVDB.h>



using namespace std;

const string GRIDNAME = "Cd";
const string FILEPATH = "/home/green/Downloads/data.vdb";

//mode: 0 - reading, 1 - writing
vdbGrid::vdbGrid(int mode){
    openvdb::initialize();
    vdbFile_ = new openvdb::io::File(FILEPATH);

    if(mode == 1){
        typedGrid_ = openvdb::Vec3fGrid::create();
        typedGrid_->insertMeta("metadata", openvdb::FloatMetadata(42.0));
        typedGrid_->setGridClass(openvdb::GRID_FOG_VOLUME);
        typedGrid_->setName(GRIDNAME);
        

        gridsVector_.push_back(typedGrid_);
        vdbFile_->write(gridsVector_);
    }
    else{
        vdbFile_->open();
        baseGrid_ = vdbFile_->readGrid(GRIDNAME);
        typedGrid_ = openvdb::gridPtrCast<openvdb::Vec3fGrid>(baseGrid_);

        //filteredGrid_ = new openvdb::tools::Filter<openvdb::Vec3fGrid>(*typedGrid_);
        //filteredGrid_->gaussian();
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
    if(!singleGrid)
        singleGrid = new vdbGrid();
    return singleGrid;
    return NULL;
}

static void post_writeVDB(void *data){
    vdbGrid *grid = (vdbGrid*)data;
    UT_ASSERT(grid == singleGrid);
    delete grid;
    singleGrid = NULL;
}

static void* pre_readVDB(){
    if(!singleGrid)
        singleGrid = new vdbGrid();
    return singleGrid;
    return NULL;
}

static void post_readVDB(void *data){
    vdbGrid *grid = (vdbGrid*)data;
    UT_ASSERT(grid == singleGrid);
    delete grid;
    singleGrid = NULL;
}

void writeVDB(int argc, void *argv[], void *data){

}

void readVDB(int argc, void *argv[], void *data){
    openvdb::Vec3f* result;
    result = (openvdb::Vec3f*)argv[0];

    const UT_Vector3* worldCoord = (const UT_Vector3 *)argv[2];
    vdbGrid* theGrid = (vdbGrid*)data;

    openvdb::Vec3f pAccesedValue;
    if(theGrid->isFileOpened_){
        openvdb::Vec3fGrid::Accessor accessor = theGrid->typedGrid_->getAccessor();
        const openvdb::Vec3d samplePosition(worldCoord->x(),worldCoord->y(),worldCoord->z());
        const openvdb::Vec3i indexPosition = theGrid->typedGrid_->worldToIndex(samplePosition);
        const openvdb::Coord xyz(indexPosition);
        //pAccesedValue = accessor.getValue(xyz);
        //SAMPLING WORKING
        //openvdb::tools::GridSampler<openvdb::tree::ValueAccessor<openvdb::Vec3fTree>, openvdb::tools::BoxSampler > sampler(accessor, grid->transform());
        openvdb::tools::GridSampler<openvdb::Vec3fTree, openvdb::tools::BoxSampler>  sampler(theGrid->typedGrid_->constTree(), theGrid->typedGrid_->transform());
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
    new VEX_VexOp ( "writeVDB@VV",
                    writeVDB,
                    VEX_ALL_CONTEXT,
                    pre_writeVDB,
                    post_writeVDB);
                    //VEX_OPTIMIZE_0);

}

