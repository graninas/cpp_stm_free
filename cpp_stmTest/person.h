#ifndef PERSON_H
#define PERSON_H

#include "common.h"
#include "address.h"
#include "car.h"

namespace sample
{

struct Person
{
    std::string name;
    std::string surname;
    int age;
    Address address;

    std::vector<Car> cars;
};

} // namespace sample

#endif // PERSON_H
