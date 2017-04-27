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
    individual(double min_weig, double max_weig,int reso, int gene);

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
        update_con_matrix();
        for (uint j = 0; j < connum; ++j) {
            printf("weight from %u to %u: %f\n", con[j].from_neuron,
                    con[j].to_neuron, con[j].weight);
        }
    }

    vector<double> get_low_high_weight()
    {
        double low=99;
        double high=-99;
        update_con_matrix();
        //cout << "ER JEG HER?! " << endl;
        for (uint j = 0; j < connum; ++j) {

            //cout << "This is the weight " << con[j].weight << endl;
            if(con[j].weight<low)
                low=con[j].weight;
            else if(con[j].weight>high)
                high=con[j].weight;
        }
        vector<double> result;
        result.push_back(low);
        result.push_back(high);

        return result;
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
    void update_min_max_res(double min, double max, int res)
    {
        min_weight_value=min;
        max_weight_value=max;
        resolution=res;
        range_weights=max-min;
    }

    void update_con_matrix(){
        ann.get_connection_array(con);
    }

    vector<int> genes;
    connection* con;
    neural_net ann;

private:
    //random_device gen;
    int resolution,connum,evaluation,generation,wins;
    double min_weight_value,max_weight_value,range_weights;


    double minimum_noise=0.1;
    double percent_noise=0.2;

    const unsigned int num_layers=3;
    const unsigned int layers[3]={11,13,1};

};

#endif // INDIVIDUAL_H
