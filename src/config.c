#include "rtklib.h"
#include <cjson/cJSON.h>

static prcopt_t prcopt_;
static solopt_t solopt_;
static filopt_t filopt_;

extern int readcfg(const char* file,prcopt_t* popt,solopt_t* sopt,filopt_t* fopt){
    prcopt_ = prcopt_default;
    solopt_ = solopt_default;
    filopt_ = (filopt_t){};

    

    return 1;
}