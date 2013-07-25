
#include <include/vexVDB.h>

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

