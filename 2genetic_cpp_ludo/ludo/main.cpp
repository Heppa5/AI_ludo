#include "dialog.h"
#include <QApplication>
#include "game.h"
#include <vector>
#include "ludo_player.h"
#include "ludo_player_random.h"
#include "positions_and_dice.h"


#include "iostream"
#include "floatfann.h"
#include <fann_cpp.h>
#include "random"
#include "working_functions.h"
#include "individual.h"
#include "fstream"

Q_DECLARE_METATYPE( positions_and_dice )

using namespace std;
using namespace FANN;



double max_weight_value=5;
double min_weight_value=-5;
int first_max_weight_value=2;
int first_min_weight_value=-2;
int resolution=7;
int first_resolution=5;
int population_size=40;
int number_of_games_per_individual=200;

float learning_rate=0.2;
bool supervised_learning=false;


int main(int argc, char *argv[]){

    ofstream myfile;
    myfile.open ("output.txt");
    QApplication a(argc, argv);
    qRegisterMetaType<positions_and_dice>();
    game g;
    ludo_player p1;
    ludo_player_random p2,p3, p4;
    Dialog w;
    setup_game(&a,&g,&p1,&p2,&p3,&p4,&w);
    vector<individual*> population;
    int generation=0;
    //int connum=0;
    bool train=false;


    if(train==true)
    {
        cout << "Hej"<< endl;
        // initialize population
        //population_size=1;
        for(int i=0 ; i<population_size ; i++)
        {
            individual* initialization;
            initialization= new individual(min_weight_value,max_weight_value,first_resolution,generation);
            initialization->true_random_weights();
            population.push_back(initialization);

        }
        cout << "Hej"<< endl;
        int connum=population[0]->ann.get_total_connections();

        generation++;
        bool done=false;
        int highest_win_overall=60;
        while(done==false)
        {
            cout << "################################# Generation: " << generation << " ##############################" << endl;
            // evaluation
            for(int i=0; i< population.size() ; i++)
            {
                population[i]->set_wins(0);
                population[i]->set_evaluation(0);
                evaluation_function(&g,&a,population[i],&p1,connum,number_of_games_per_individual);
                cout << "Individual " << i << " got an evaluation of \t" << population[i]->get_evaluation() << "\t and won: " << population[i]->get_wins() << " out of " << number_of_games_per_individual << " games and from gen: " <<population[i]->get_generation() <<  endl;
            }

            // selection
            vector<int> parents;
            for(int i=0; i<population.size()/2 ; i++)
            {
                //vector<int> two_parents = selection_roulette_method(&population,number_of_games_per_individual);
                vector<int> two_parents = rank_selection_method(&population);
                parents.push_back(two_parents[0]);
                parents.push_back(two_parents[1]);
            }

            // generating children
            vector<individual*> children;
            for(int i=0; i<population.size()/2 ; i++)
            {
                vector<individual*> two_children= population[parents[i*2]]->make_two_children(population[parents[i*2+1]],generation);
                children.push_back(two_children[0]);
                children.push_back(two_children[1]);
            }

            int highest=-999;
            int highest_win=0;
            int total=0;
            int highest_win_index=0;
            for (int i= 0 ; i<population.size() ; i++)
            {
                if(population[i]->get_evaluation()>highest)
                    highest=population[i]->get_evaluation();
                if(population[i]->get_wins()>highest_win)
                {
                    highest_win=population[i]->get_wins();
                    highest_win_index=i;
                }
                total=total+population[i]->get_evaluation();
            }
            double average=(double)total/(double)population.size();
            //done=true;
            myfile << generation <<"\t"<<highest <<"\t"<< highest_win <<"\t"<<average <<"\t"<< endl;
            if(population[highest_win_index]->get_wins()>highest_win_overall)
            {
                population[highest_win_index]->ann.save("The_best_net.net");
            }

            for(int i=0; i< children.size() ; i++)
            {
                children[i]->set_wins(0);
                children[i]->set_evaluation(0);
                evaluation_function(&g,&a,children[i],&p1,connum,number_of_games_per_individual);
                //cout << "children " << i << " got an evaluation of \t" << population[i]->get_evaluation() << "\t and won: " << population[i]->get_wins() << " out of " << number_of_games_per_individual << " games and from gen: " <<population[i]->get_generation() <<  endl;
            }

            select_best_offspring(&population,&children,generation,connum,resolution,min_weight_value,max_weight_value);
            update_max_min_weights(&population,&min_weight_value,&max_weight_value,resolution);

            generation++;
        }


        a.quit();

        for(int i=0; i< population.size() ; i++)
        {
            delete population[i];
        }
    }
    else
    {
        number_of_games_per_individual=10000;
        int player_0=0;
        int player_1=0;
        int player_2=0;
        int player_3=0;

        individual* tactic=new individual(min_weight_value,max_weight_value,first_resolution,generation);
        tactic->ann.create_from_file("The_best_net.net");

        p1.change_individual(tactic);
        //p1->set_weights(population[index].con,connum);
        for(int j=0 ; j < number_of_games_per_individual ; j++)
        {
            g.start();
            a.exec();
            while (a.closingDown()){
            }

            if(g.winner==0)
               player_0++;
            else if(g.winner==1)
                player_1++;
            else if(g.winner==2)
                player_2++;
            else if(g.winner==3)
                player_3++;
            g.reset();
            if(g.wait()){}

        }

        cout << "Player 0: " << (double)((double)player_0/(double)number_of_games_per_individual) << endl;
        cout << "Player 1: " << (double)((double)player_1/(double)number_of_games_per_individual) << endl;
        cout << "Player 2: " << (double)((double)player_2/(double)number_of_games_per_individual) << endl;
        cout << "Player 3: " << (double)((double)player_3/(double)number_of_games_per_individual) << endl;

    }


    return 0;
}
