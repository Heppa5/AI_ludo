#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include <vector>

using namespace std;
struct individual {
    vector<int> genes;
    int evaluation=0;
    struct fann_connection con;
    unsigned int connum;
    int generation;
};
#endif // INDIVIDUAL_H
