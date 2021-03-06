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
Q_DECLARE_METATYPE( positions_and_dice )

#include <iostream>

std::random_device gen;


const unsigned int num_input = 17; // placement of each piece, closest opponent for each piece,distance to globe for each piece,distance to star for each piece, dice roll = 17
const unsigned int num_output = 1; // which piece to move
const unsigned int num_layers = 5;
const unsigned int num_neurons_hidden = 9;
const int max_weight_value=5;
const int min_weight_value=-5;

const int population_size=25;
const int resolution=5; // number of bits resolution
const int number_of_games_per_individual=1000;
unsigned int connum;           // connections number
int range_weights=0;
using namespace std;




void adjust_weights_to_resolution(uint connum, struct fann_connection *con, uint resolution )
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

vector<int> convert_connections_to_genes(uint connum, struct fann_connection *con, uint resolution)
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

void generate_connections_from_genes(uint connum, struct fann_connection *con, uint resolution, vector<int> genes)
{
    for (int i=0; i< connum ; i++)
    {
        //con[i].weight=(-1+genes[i]*2/pow(2,resolution));
        con[i].weight = (min_weight_value+range_weights*genes[i]*1/pow(2,resolution));
        //cout << con[i].weight << "\t" << genes[i] << endl;
    }
}

vector<int > selection_roulette_method(vector<individual> *population )
{

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

void evaluation_function(game *g,QApplication* a, vector<individual> *pop, int index, ludo_player *p1)
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

vector<individual> create_two_children(vector<individual> *pop, int father_index, int mother_index )
{
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

vector<individual> generate_children(vector<individual> *pop, int generation, vector<int> parents , fann *ann)
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

        auto two_children=create_two_children(pop, i*2+0, i*2+1 );
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
        generate_connections_from_genes(connum, con_local1, resolution, two_children[0].genes);
        generate_connections_from_genes(connum, con_local2, resolution, two_children[1].genes);

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
void select_best_offspring(vector<individual>* pop, vector<individual>* child, fann *ann , int generation)
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

        adjust_weights_to_resolution(connum,con_local,resolution);
        indi_initialization.connum=connum;
        indi_initialization.con=con_local;
        indi_initialization.generation=generation;
        indi_initialization.genes=convert_connections_to_genes(connum,con_local,resolution);
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

int main(int argc, char *argv[]){


    ofstream myfile;
    myfile.open ("output.txt");


    range_weights=max_weight_value-min_weight_value;
    struct fann *ann = fann_create_standard(num_layers, num_input,num_neurons_hidden,num_neurons_hidden,num_neurons_hidden, num_output);
    // setting activation functions as sigmoid_symmetric functions.
    //fann_set_activation_function_layer(ann, FANN_SIGMOID_SYMMETRIC, 0);
    fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
    //fann_set_activation_function_output(ann, FANN_LINEAR_PIECE);
    fann_set_activation_function_output(ann, FANN_LINEAR_PIECE_SYMMETRIC);
    fann_randomize_weights(ann,min_weight_value,max_weight_value);

    struct fann_connection *con;   // weight matrix
    connum = fann_get_total_connections(ann);
    con = (fann_connection*)calloc(connum, sizeof(*con));
    fann_get_connection_array(ann, con);

    ludo_player p1(ann);
    // ######################## Setting up game #######################################
    QApplication a(argc, argv);
    qRegisterMetaType<positions_and_dice>();
    ludo_player_random  p2,p3, p4;
    game g;
    g.setGameDelay(000); //if you want to see the game, set a delay
    // Add a GUI <-- remove the '/' to uncomment block
   /* Dialog w;
    QObject::connect(&g,SIGNAL(update_graphics(std::vector<int>)),&w,SLOT(update_graphics(std::vector<int>)));
    QObject::connect(&g,SIGNAL(set_color(int)),                   &w,SLOT(get_color(int)));
    QObject::connect(&g,SIGNAL(set_dice_result(int)),             &w,SLOT(get_dice_result(int)));
    QObject::connect(&g,SIGNAL(declare_winner(int)),              &w,SLOT(get_winner()));
    QObject::connect(&g,SIGNAL(close()),&a,SLOT(quit()));
    w.show();*/
    // //Or don't add the GUI
    QObject::connect(&g,SIGNAL(close()),&a,SLOT(quit()));
    //*/

    //set up for each player
    QObject::connect(&g, SIGNAL(player1_start(positions_and_dice)),&p1,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p1,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player1_end(std::vector<int>)),    &p1,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p1,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    QObject::connect(&g, SIGNAL(player2_start(positions_and_dice)),&p2,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p2,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player2_end(std::vector<int>)),    &p2,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p2,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    QObject::connect(&g, SIGNAL(player3_start(positions_and_dice)),&p3,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p3,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player3_end(std::vector<int>)),    &p3,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p3,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    QObject::connect(&g, SIGNAL(player4_start(positions_and_dice)),&p4,SLOT(start_turn(positions_and_dice)));
    QObject::connect(&p4,SIGNAL(select_piece(int)),                &g, SLOT(movePiece(int)));
    QObject::connect(&g, SIGNAL(player4_end(std::vector<int>)),    &p4,SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&p4,SIGNAL(turn_complete(bool)),              &g, SLOT(turnComplete(bool)));

    // ############################ END OF SETTING UP GAME ############################################

    // Jesper Adding code start








    vector<individual> population;
    for (int i=0; i<population_size ; i++)
    {

        //cout << "\n########## NEW INDIVIDUAL IN INITIALIZATION ####### \n";
        individual indi_initialization;
        fann_randomize_weights(ann,min_weight_value,max_weight_value);
        fann_connection *con_local = new fann_connection;
        con_local = (fann_connection*)calloc(connum, sizeof(*con_local));
        fann_get_connection_array(ann, con_local);  

        /*if(i==0)
        {
             cout << "Jesper ######################3" << endl;
            for (uint j = 0; j < connum; ++j) {
                printf("weight from %u to %u: %f\n", con_local[j].from_neuron,
                       con_local[j].to_neuron, con_local[j].weight);
            }
        }*/
        adjust_weights_to_resolution(connum,con_local,resolution);
        indi_initialization.connum=connum;
        indi_initialization.con=con_local;
        indi_initialization.generation=0;
        indi_initialization.genes=convert_connections_to_genes(connum,con_local,resolution);

        /*if(i==0)
        {
            cout << "Jesper ######################3" << endl;
            /*for (uint i = 0; i < connum; ++i) {
                printf("weight from %u to %u: %f\n", con_local[i].from_neuron,
                       con_local[i].to_neuron, con_local[i].weight);
            }

            //cout << con_local[indi_initialization.genes.size()-1].weight<<"\t"<<indi_initialization.genes[indi_initialization.genes.size()-1] << endl;
        }*/
        population.push_back(indi_initialization);

    }


    /*for (int i=0; i<population.size() ; i++)
    {
        cout << "\n########## NEW INDIVIDUAL IN check ####### \n";

        fann_connection *con2=population[i].con;
        cout << &population[i].con << endl;
        for (uint i = 0; i < connum; ++i) {
            printf("weight from %u to %u: %f\n", con2[i].from_neuron,
                   con2[i].to_neuron, con2[i].weight);
        }
    }*/


    // Jesper adding code
    bool done=false;
    int generation=1;
    while (done==false)
    {
        if(number_of_games_per_individual>99)
            cout << "##########################Evaluation - generation: " << generation << " ############################" << endl;

        // evaluation of population
        for (int i=0 ; i<population.size(); i++)
        {
            //cout << "New player " << i << endl;
            //p1.set_weights(population[i].con,connum);
            population[i].evaluation=0;
            population[i].wins=0;
            evaluation_function(&g,&a,&population,i,&p1);
            if(number_of_games_per_individual>99)
                cout << "Individual " << i << " got an evaluation of \t" << population[i].evaluation << "\t and won: " << population[i].wins << " out of " << number_of_games_per_individual << " games and from gen: " <<population[i].generation <<  endl;
        }


        // ############ Selection ################
        vector<int> parents;
        for(int i=0; i<population.size()/2 ; i++)
        {
            vector<int> two_parents = selection_roulette_method(&population);
            parents.push_back(two_parents[0]);
            parents.push_back(two_parents[1]);
        }



        // Mutation
        vector <individual> children=generate_children(&population, generation, parents , ann);
        // Just change entire population


        /*for (int i=0; i<children.size() ; i++)
        {
            cout << "\n########## NEW INDIVIDUAL IN check ####### \n";

            fann_connection *con2=children[i].con;
            for (uint i = 0; i < connum; ++i) {
                printf("weight from %u to %u: %f\n", con2[i].from_neuron,
                       con2[i].to_neuron, con2[i].weight);
            }
        }*/

        int highest=-999;
        int highest_win=0;
        int total=0;
        for (int i= 0 ; i<population.size() ; i++)
        {
            if(population[i].evaluation>highest)
                highest=population[i].evaluation;
            if(population[i].wins>highest_win)
                highest_win=population[i].wins;
            total=total+population[i].evaluation;
        }
        double average=(double)total/(double)population.size();
        //done=true;
        myfile << generation <<"\t"<<highest <<"\t"<< highest_win <<"\t"<<average <<"\t"<< endl;
        /*for(int i= 0; i< population.size() ; i++)
        {
            if(population[i].evaluation>400)
            {
                cout << "\nWOOOOOW Jeg har noget tilnærmelsesvist ok\n\n\n" << endl;
                fann_set_weight_array(ann,population[i].con,connum);
                fann_save(ann,"hahah.net");
                done=true;
            }
        }*/

        for(int i= 0; i< children.size() ; i++)
        {
            children[i].evaluation=0;
            evaluation_function(&g,&a,&children,i,&p1);
        }
        //let_best_3_indi_stay(&population,&children);
        select_best_offspring(&population, &children,ann ,  generation );
        //population=children;
       // std::sort(population.begin(), population.end(), compareBySize);
        generation++;

        /*for (int i=0; i<1 ; i++)
        {
            cout << "\n########## NEW INDIVIDUAL IN check ####### \n";

            fann_connection *con2=population[i].con;
            cout << &population[i].con << endl;
            for (uint i = 0; i < connum; ++i) {
                printf("weight from %u to %u: %f\n", con2[i].from_neuron,
                       con2[i].to_neuron, con2[i].weight);
            }
        }*/


    }
    return 0;
}
