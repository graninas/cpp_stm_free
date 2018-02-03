#ifndef CAR_H
#define CAR_H

#include "common.h"

namespace sample
{

struct Car
{
    std::string number;
    std::string model;
    int mileage;

    std::list<std::string> accessories;
};


} // namespace sample


#endif // CAR_H
