#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <vector>

using namespace std;
struct individual {
    vector<int> genes;
    int evaluation=0;
    fann_connection *con;
    unsigned int connum;
    int generation;
    int wins=0;
};
#endif // INDIVIDUAL_H
