/**
 * @file policy.h
 * @author S.Filipovich
 * @date 25 August 2019
 * @brief File containing class Policy for AWL
 *
 * Class Policy is used to store data about AWL policies
 * Class viriables - 
 *
 */

#ifndef POLICY_H_
#define POLICY_H_

#include <list>
#include <iostream>
#include <string>
#include <stdio.h>
using namespace std;


class Policy
{

private:
    string name;
    list <string> categories;

public:
    /**
     * Constructors
     *
     */

    Policy();
    Policy(string s1):name(s1) {}
    /**
     * Getters and setters
     *
     */
    string getName();
    void setName(string);

    void add_category(string);
    list<string>* get_categories_list();
    
    bool check_category(string);


};



#endif  /* POLICY_H_ */