#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "common.h"
#include "person.h"

namespace sample
{

struct Account
{
    Person person;
    std::string login;
    std::string password;
};


} // namespace sample

#endif // ACCOUNT_H
