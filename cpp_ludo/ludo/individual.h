#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H
#include "floatfann.h"
#include <fann_cpp.h>
#include <iostream>
#include <random>

using namespace std;
using namespace FANN;
class individual
{
public:
    individual();
    individual(int min_weig, int max_weig,int reso, int gene);

    vector<individual*> make_two_children(individual *mother,int cur_gen);
    void adjust_weights_to_resolution();
    void convert_connections_to_genes();
    void generate_connections_from_genes();

    void set_wins(int num_win){
        wins=num_win;
    }
    void set_evaluation(int num_ev){
        evaluation=num_ev;
    }
    int get_evaluation(){
        return evaluation;
    }
    int get_wins(){
        return wins;
    }

    void print_connections()
    {
        for (uint j = 0; j < connum; ++j) {
            printf("weight from %u to %u: %f\n", con[j].from_neuron,
                    con[j].to_neuron, con[j].weight);
        }
    }
    void print_connection(int index)
    {
        printf("weight from %u to %u: %f\n", con[index].from_neuron,
                con[index].to_neuron, con[index].weight);
    }
    void true_random_weights();
    int get_generation(){
        return generation;
    }

    vector<int> genes;

    neural_net ann;

private:
    //random_device gen;
    int min_weight_value,max_weight_value,range_weights,resolution,connum,evaluation,generation,wins;

    connection* con;

    const unsigned int num_layers=3;
    const unsigned int layers[3]={17,6,1};

};

#endif // INDIVIDUAL_H
