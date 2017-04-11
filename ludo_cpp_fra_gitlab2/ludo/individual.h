#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <vector>

using namespace std;
struct individual {
    vector<int> genes;
    int evaluation;
    fann_connection *con;
    unsigned int connum;
    int generation;
};
#endif // INDIVIDUAL_H
