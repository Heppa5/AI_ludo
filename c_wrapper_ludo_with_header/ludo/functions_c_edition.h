#ifndef FUNCTIONS_C_EDITION_H
#define FUNCTIONS_C_EDITION_H

#include "dialog.h"
#include <QApplication>
#include "game.h"
#include <vector>
#include "ludo_player.h"
#include "ludo_player_random.h"
#include "positions_and_dice.h"
#include "individual.h"
#include <random>
#include <fstream>


void adjust_weights_to_resolution(uint connum, struct fann_connection *con, uint resolution , int min_weight_value, int range_weights)
{
    for (uint i = 0; i < connum; ++i)
    {
        double cur_weight=(double)con[i].weight;
        double error=99;
        int min_error_index=0;

        for( int j=0 ; j< pow(2,resolution) ; j++)
        {
            //if(abs(cur_weight-(-1+j*2/(pow(2,resolution)-1)))< error )
            //if(abs(cur_weight-(min_weight_value+(j*2/(pow(2,resolution)))*max_weight_value))< error )
            if(abs(cur_weight-(min_weight_value+(j*1/(pow(2,resolution)))*range_weights))< error )
            {

                error=abs(cur_weight-(min_weight_value+(j*1/(pow(2,resolution)))*range_weights));
                min_error_index=j;
            }
        }

        con[i].weight=min_weight_value+(min_error_index*1/(pow(2,resolution)))*range_weights;
        //cout << error << "\t" << min_error_index << "\t" << cur_weight << "\t" << con[i].weight << endl;

        // cout << "Cur_weight= " <<(double) cur_weight << " "<< con[i].weight <<endl;
    }
}

vector<int> convert_connections_to_genes(uint connum, struct fann_connection *con, uint resolution, int min_weight_value, int range_weights)
{
    vector<int> genes;
    for (uint i=0 ; i < connum ; i++)
    {
        for( int j=0 ; j< pow(2,resolution) ; j++)
        {
            //if(con[i].weight==(-1+j*2/pow(2,resolution)))
            //cout << con[i].weight << "\t" << (min_weight_value+(j*1/(pow(2,resolution)))*range_weights) << "\t" << (con[i].weight==(min_weight_value+(j*1/(pow(2,resolution)))*range_weights)) << endl;
            if((double)con[i].weight == (double)(min_weight_value+(j*1/(pow(2,resolution)))*range_weights))
            {
                genes.push_back(j);
            }
        }
    }
    /*for ( int j=0; j<genes.size() ; j++)
    {
        cout << " is " << genes[j] << endl;
    }*/
    return genes;
}

void generate_connections_from_genes(uint connum, struct fann_connection *con, uint resolution, vector<int> genes,int min_weight_value, int range_weights)
{
    for (int i=0; i< connum ; i++)
    {
        //con[i].weight=(-1+genes[i]*2/pow(2,resolution));
        con[i].weight = (min_weight_value+range_weights*genes[i]*1/pow(2,resolution));
        //cout << con[i].weight << "\t" << genes[i] << endl;
    }
}

vector<int > selection_roulette_method(vector<individual> *population)
{
    random_device gen;
    bool debug =false;
    vector<individual>& vecRef = *population; // vector is not copied here

    // create roulette wheel

    vector<int> roulette;
    int more_than_one=0;
    for(int i=0; i<vecRef.size();i++)
    {
        if(vecRef[i].evaluation>0)
        {
            more_than_one++;
            for(int j=0; j<vecRef[i].evaluation;j++)
            {
                roulette.push_back(i);
            }
        }
    }
    if (more_than_one<=1)
    {
        for(int i=0; i<vecRef.size();i++)
        {
            roulette.push_back(i);
        }
    }

    if (debug == true)
    {
        for( int i = 0; i< roulette.size() ; i++)
        {
            //cout << roulette[i] << endl;
        }
        //cout << "Size of roulette is: " << roulette.size() << endl;
    }
    std::uniform_int_distribution<> dis(0, roulette.size()-1);

    int father_index_roulette = dis(gen);

    int father_index_population = roulette[father_index_roulette];
    // remove father from roulette
    for(int i = 0 ; i < roulette.size() ; i++)
    {
        if ( roulette[i]==father_index_population)
        {
            if(debug == true)
            {
                cout << "Deleting father index: " << father_index_population << " and random number was: " << father_index_roulette << "\n So we're deleting from " << i << " to " << i + vecRef[father_index_population].evaluation << endl;
                //cout << roulette[i + vecRef[father_index_population].evaluation-1] << endl;
            }
            roulette.erase(roulette.begin() + i, roulette.begin() + i + vecRef[father_index_population].evaluation-1);
            break;
        }
    }
    std::uniform_int_distribution<> dis2(0, roulette.size()-1);
    int mother_index_roulette = dis2(gen);
    int mother_index_population = roulette[mother_index_roulette];
    vector<int> parents;
    parents.push_back(father_index_population);
    parents.push_back(mother_index_population);
    //cout << &population[father_index_population]<<&vecRef[father_index_population] << endl;
    return parents;
}

void evaluation_function(game *g,QApplication* a, vector<individual> *pop, int index, ludo_player *p1,uint connum, int number_of_games_per_individual)
{
    vector<individual>& population = *pop; // vector is not copied here
    p1->set_weights(population[index].con,connum);
    for(int j=0 ; j < number_of_games_per_individual ; j++)
    {
        g->start();
        a->exec();
        while (a->closingDown()){
        }
        // ## ################# evaluation function ###########################
        std::vector<int> player_positions=g->player_positions;
        for(int h=0; h< 4 ; h++)
        {
            if(player_positions[h]==-1 ){
                population[index].evaluation =population[index].evaluation-40;
                int wpw=2;
            }
            else if(player_positions[h]==99)
                population[index].evaluation =population[index].evaluation+60;
            else
                population[index].evaluation =population[index].evaluation+player_positions[h];
        }
        if(g->return_winner()==0)
            population[index].wins++;

        g->reset();
        if(g->wait()){}

    }
    population[index].evaluation = population[index].evaluation/number_of_games_per_individual;
    population[index].evaluation = population[index].evaluation + population[index].wins*25;
}

vector<individual> create_two_children(vector<individual> *pop, int father_index, int mother_index , int resolution )
{
    random_device gen;
    bool debug=false;
    vector<individual>& population = *pop; // vector is not copied here
    std::uniform_int_distribution<> dis(0, pow(2,resolution)-1);
    unsigned int bit_mask = dis(gen);
    if(debug)
        cout << "Bit mask is: " <<  bit_mask <<endl;
    individual father=population[father_index];
    individual mother=population[mother_index];
    individual children1;
    individual children2;
    for(int i=0; i< father.genes.size() ; i++) // bit 1 -> take fathers gen to children1  / bit 0 -> take fathers gene to children2
    {
        int gene1=0;
        int gene2=0;
        for (int j=0 ; j<resolution ; j++)
        {
            if (debug)
                cout << "Bit mask is: " <<  bit_mask <<endl;
            int bit = (bit_mask & ( 1 << j ))>>j;
            int father_bit = (father.genes[i] & ( 1 << j ))>>j;
            int mother_bit = (mother.genes[i] & ( 1 << j ))>>j;
            if(debug)
            {
                cout << "We are at iteration " << j << " and bit from bit mask is: " << bit << " and bit from mother is: " << mother_bit << " and bit from father is: " << father_bit <<  endl;
                cout << "Father gene: " << father.genes[i] << " Mother gene: " << mother.genes[i] << endl;
            }
            if ( bit == 1)
            {
                gene1=gene1+father_bit*pow(2,j);
                gene2=gene2+mother_bit*pow(2,j);
            }
            else
            {
                gene2=gene2+father_bit*pow(2,j);
                gene1=gene1+mother_bit*pow(2,j);
            }
        }
        children1.genes.push_back(gene1);
        children2.genes.push_back(gene2);
    }
    vector<individual> two_children;
    two_children.push_back(children1);
    two_children.push_back(children2);
    return two_children;
}

vector<individual> generate_children(vector<individual> *pop, int generation, vector<int> parents , fann *ann,uint connum, int resolution,int min_weight_value, int range_weights)
{
    vector<individual>& population = *pop; // vector is not copied here
    bool debug = false;
    if(debug)
    {
        for(int i=0; i< population.size() ; i++)
        {
            cout << "Population " << i << endl;
            for ( int j=0; j<population[0].genes.size() ; j++)
            {
                cout << "parent "<< i <<" and gene " << j << " is " << population[i].genes[j] << endl;
            }
        }
    }

    vector<individual> new_population;
    for (int i=0; i<parents.size()/2 ; i++)
    {

        fann_connection *con_local1 = new fann_connection;
        con_local1 = (fann_connection*)calloc(connum, sizeof(*con_local1));
        fann_get_connection_array(ann, con_local1);

        fann_connection *con_local2 = new fann_connection;
        con_local2 = (fann_connection*)calloc(connum, sizeof(*con_local2));
        fann_get_connection_array(ann, con_local2);

        auto two_children=create_two_children(pop, i*2+0, i*2+1,resolution);
        if(debug)
        {
            for ( int j=0; j<two_children[0].genes.size() ; j++)
            {
                cout << "parent 0 and gene " << j << " is " << population[0].genes[j] << endl;
                cout << "parent 1 and gene " << j << " is " << population[1].genes[j] << endl;
                cout << "Children 0 and gene " << j << " is " << two_children[0].genes[j] << endl;
                cout << "Children 1 and gene " << j << " is " << two_children[1].genes[j] << endl;
            }
        }
        generate_connections_from_genes(connum, con_local1, resolution, two_children[0].genes,min_weight_value,range_weights);
        generate_connections_from_genes(connum, con_local2, resolution, two_children[1].genes,min_weight_value,range_weights);

        two_children[0].con=con_local1;
        two_children[1].con=con_local2;
        two_children[0].generation=generation;
        two_children[1].generation=generation;
        two_children[0].connum=connum;
        two_children[1].connum=connum;

        new_population.push_back(two_children[0]);
        new_population.push_back(two_children[1]);

    }
    return new_population;
}

bool compareBySize(individual &a, individual &b)
{
    return a.evaluation > b.evaluation;
}
void select_best_offspring(vector<individual>* pop, vector<individual>* child, fann *ann , int generation,uint connum,int resolution,int min_weight_value, int max_weight_value)
{
    vector<individual>& population = *pop;
    vector<individual>& children = *child;
    std::sort(population.begin(), population.end(), compareBySize);
    std::sort(children.begin(), children.end(), compareBySize);

    int number_of_random_each_generation=5;
    for(int i=0; i< number_of_random_each_generation;i++)
    {
        individual indi_initialization;
        fann_randomize_weights(ann,min_weight_value,max_weight_value);
        fann_connection *con_local = new fann_connection;
        con_local = (fann_connection*)calloc(connum, sizeof(*con_local));
        fann_get_connection_array(ann, con_local);

        adjust_weights_to_resolution(connum,con_local,resolution,min_weight_value,(max_weight_value-min_weight_value) );
        indi_initialization.connum=connum;
        indi_initialization.con=con_local;
        indi_initialization.generation=generation;
        indi_initialization.genes=convert_connections_to_genes(connum,con_local,resolution,min_weight_value, (max_weight_value-min_weight_value));
        population[population.size()-1-i]=indi_initialization;
    }


    /*for(int i=0; i< population.size() ; i++)
    {
        cout << population[i].evaluation << endl;
    }*/

    for(int i=0; i< population.size()-number_of_random_each_generation ; i++)
    {
        if(children[i].evaluation > population[population.size()-1-number_of_random_each_generation-i].evaluation)
        {
            cout << "Success - better child \n" << " Child evaluation was: " << children[i].evaluation << " individual evaluation was: " << population[population.size()-1-i-number_of_random_each_generation].evaluation << endl;
            population[population.size()-1-number_of_random_each_generation-i]=children[i];
        }
    }

}

void let_best_3_indi_stay(vector<individual>* pop, vector<individual>* child )
{
    vector<individual>& population = *pop;
    vector<individual>& children = *child;
    std::sort(population.begin(), population.end(), compareBySize);
    std::sort(children.begin(), children.end(), compareBySize);
    int offset=3;
    for(int i=offset; i< population.size() ; i++)
    {
        population[i]=children[i-offset];
    }

}
#endif // FUNCTIONS_C_EDITION_H
