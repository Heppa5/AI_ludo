#include "ludo_player_train.h"


#include <random>

ludo_player_train::ludo_player_train():
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
}

/*int ludo_player_train::make_decision(){
    if(dice_roll == 6){
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]<0){
                return i;
            }
        }
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
                return i;
            }
        }
    } else {
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
                return i;
            }
        }
        for(int i = 0; i < 4; ++i){ //maybe they are all locked in
            if(pos_start_of_turn[i]<0){
                return i;
            }
        }
    }
    return -1;
}*/

/*
 * Can I get out?
 * Do I get send home if I move?
 * Can I send an opponent home?
 * Do I get to a star?
 * Do I get to a Globe?
 * Do I enter goal if I move?
 *
 * Am I on a globe?
 * Can I get to a friendly piece?
 * Am I'm safe? (are there a friendly piece at my position)? **
 * Is there a opponent piece, which can send me home and can my dice roll put me away from danger?
 * -> start_position - how many opponents are 5 fields behind me?
 * -> start_position + dice_roll - how many opponents are 5 fields behind me?
 */


int ludo_player_train::make_decision(){
    //int num_input=fann_get_num_input(ann);
    bool debug = false;
    if (debug)
    {
         cout << endl << endl << endl << "######################## NEW DECISION ########" << endl;
         for(int i=4; i<16 ; i++)
             cout << "Opponent piece " << i << " is at position: " << pos_start_of_turn[i] << endl;
    }
    vector<double> results;
    for (int piece=0; piece < 4 ; piece++)
    {
        if (debug)
        {
            cout << "##### Piece:  " << piece << "   with position " << pos_start_of_turn[piece] << " and  dice roll: " << dice_roll<<endl;
        }
        if(pos_start_of_turn[piece] != 99 && (pos_start_of_turn[piece] !=-1 || dice_roll==6 ))
        {
            fann_type input[11];

            // can I get out?
            if(pos_start_of_turn[piece]==-1 && dice_roll==6)
                input[0]=1;
            else
                input[0]=0;

            // Do I get to a globe if I move?

            input[4]=0;
            if(pos_start_of_turn[piece]!=-1 && pos_start_of_turn[piece]<52)
            {
                if(pos_start_of_turn[piece]+dice_roll % 13 == 0 || (pos_start_of_turn[piece]+dice_roll - 8) % 13 == 0 )
                {
                    input[4]=1;
                }
            }

            //Do I get send home if I move (check for an opponent being on a globe and if I can move my piece there)
            input[1]=0;
            for(int opponent=4; opponent < 16 ; opponent++)
            {
                // if I get to an opponents position by moving this piece (+ fault check for not being in start)
                if(pos_start_of_turn[piece]+dice_roll==pos_start_of_turn[opponent] && pos_start_of_turn[piece] != -1)
                {
                    int j=pos_start_of_turn[opponent];
                    if(j % 13 == 0 || (j - 8) % 13 == 0 )
                    {
                        input[1]=1;
                    }
                }
            }

            // do I reach a star?
            input[3]=0;
            int star_index=99;
            for( int i = 0; i < 8 ; i++)
            {
                if(pos_start_of_turn[piece]+dice_roll==star[i] && pos_start_of_turn[piece] != -1)
                {
                    input[3]=1;
                    star_index=i;
                }
            }


            // Can I send an opponent home?
            input[2]=0;
            for(int opponent=4; opponent < 16 ; opponent++)
            {
                // if I get to an opponents position by moving this piece (+ fault check for not being in start)
                if(pos_start_of_turn[piece]+dice_roll==pos_start_of_turn[opponent] && pos_start_of_turn[piece] != -1)
                {
                    input[2]=1;
                }
                else if(star_index != 99) // if I can hit a star check if there is an opponent on the next star
                {
                    if( star_index < 8) // avoid going out of range
                    {
                        if(star[star_index+1]==pos_start_of_turn[opponent])
                        {
                            input[2]=1;                    }
                    }

                }
            }



            // Do I enter goal if I move?
            if(pos_start_of_turn[piece]+dice_roll==56)
            {
                input[5]=1;
            }
            else
            {
                input[5]=0;
            }

            // Am I on a globe?
            input[6]=0;
            if(pos_start_of_turn[piece]!=-1 && pos_start_of_turn[piece]<52)
            {
                if(pos_start_of_turn[piece]% 13 == 0 || (pos_start_of_turn[piece] - 8) % 13 == 0 )
                {
                    input[6]=1;
                }
            }
            // Can I get to a friendly piece?
            input[7] = 0;
            if(pos_start_of_turn[piece]!=-1 && pos_start_of_turn[piece]<52)
            {
                for(int i=0; i<4 ; i++)
                {
                    if(pos_start_of_turn[piece]+dice_roll==pos_start_of_turn[i])
                    {
                        input[7]=1;
                    }
                }
            }
            //Am I'm safe? (are there a friendly piece at my position)?
            input[8] = 0;
            for(int i=0 ; i<4 ; i++)
            {
                if(i!=piece && (pos_start_of_turn[piece] != -1 ||pos_start_of_turn[piece] != 99) )
                {
                    if(pos_start_of_turn[piece]==pos_start_of_turn[i])
                    {
                        input[8]=1;
                    }
                }
            }

            //* -> start_position - how many opponents are 5 fields behind me?
            input[9]=0;
            double count=0;
            if(pos_start_of_turn[piece] != -1 && pos_start_of_turn[piece] < 52)
            {
                for(int opponent=4; opponent < 16 ; opponent++)
                {
                    int position_to_check = pos_start_of_turn[piece]-6;
                    if(pos_start_of_turn[opponent]>position_to_check && pos_start_of_turn[opponent]<pos_start_of_turn[piece])
                        count++;
                    else if(position_to_check<0)
                    {
                        if( (pos_start_of_turn[opponent]>=52-position_to_check && pos_start_of_turn[opponent]<52) || pos_start_of_turn[opponent]<pos_start_of_turn[piece] && pos_start_of_turn[opponent] >=0 )
                            count++;
                    }
                }
            }
            input[9]=(double)(count/6.00);

            //* -> start_position + dice_roll - how many opponents are 5 fields behind me?
            input[10]=0;
            count=0;
            if(pos_start_of_turn[piece] != -1 && pos_start_of_turn[piece] < 52)
            {
                for(int opponent=4; opponent < 16 ; opponent++)
                {
                    int position_to_check = pos_start_of_turn[piece]-6+dice_roll;
                    if(pos_start_of_turn[opponent]>position_to_check && pos_start_of_turn[opponent]<pos_start_of_turn[piece])
                        count++;
                    else if(position_to_check<0)
                    {
                        if( (pos_start_of_turn[opponent]>=52-position_to_check && pos_start_of_turn[opponent]<52) || pos_start_of_turn[opponent]<pos_start_of_turn[piece] && pos_start_of_turn[opponent] >=0 )
                            count++;
                    }
                }
            }
            input[10]=(double)(count/6.00);

            if(debug)
            {
               cout << " * Can I get out? \t\t\t" << input[0] << endl;
               cout << " * Do I get send home if I move? \t \t" << input[1] << endl;
               cout << " * Can I send an opponent home? \t \t" << input[2] << endl;
               cout << " * Do I get to a star? \t\t\t \t" << input[3] << endl;
               cout << " * Do I get to a Globe? \t\t \t" << input[4] << endl;
               cout << " * Do I enter goal if I move? \t\t \t" << input[5] << endl;
               cout << " * Am I on a globe? \t \t \t" << input[6] << endl;
               cout << " * Can I get to a friendly piece? \t \t \t" << input[7] << endl;
               cout << " * Are there a friendly piece at my position \t \t " << input[8] << endl;
               cout << " * start_position - how many opponents are 5 fields behind me? \t " << input[9] << endl;
               cout << " * start_position + dice_roll - how many opponents are 5 fields behind me?" << input[10] << endl;
            }
            auto result=tactic->ann.run(input);
            results.push_back(result[0]);
            if(debug)
            {
                cout << "Output for piece " << piece << " is: " << result[0]<< endl;
            }
        }
        else
        {
            if (debug)
                cout << "REJECTED"<< endl;
            results.push_back(0.0);
        }
    }
    if(debug)
        cout << "Now we find highest result" << endl;
    double highest=-1;
    vector<int> index;
    for(int i=0; i<results.size();i++)
    {
        if(results[i]>highest)
        {
            index.clear();
            index.push_back(i);
            highest=results[i];
        }
        else if(results[i]==highest)
        {
            index.push_back(i);
        }
    }
    if(debug)
        cout << "Now we choose a piece from "<< index.size() << " Pieces"<< endl;
    int piece_to_move=-1;
    if(index.size()>1)
    {
        random_device gen;
        std::uniform_int_distribution<> dis(0,index.size()-1 );
        piece_to_move = dis(gen);
    }
    else
    {
        piece_to_move=index[0];
    }

    if(debug)
    {
       cout <<"We have choosen to move piece " << piece_to_move << endl;
    }


    return piece_to_move;


    /*if(dice_roll == 6){
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]<0){
                return i;
            }
        }
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
                return i;
            }
        }
    } else {
        for(int i = 0; i < 4; ++i){
            if(pos_start_of_turn[i]>=0 && pos_start_of_turn[i] != 99){
                return i;
            }
        }
        for(int i = 0; i < 4; ++i){ //maybe they are all locked in
            if(pos_start_of_turn[i]<0){
                return i;
            }
        }
    }*/

    return -1;
}


/*int ludo_player_train::make_decision(){
    //int num_input=fann_get_num_input(ann);
    fann_type input[17];

    bool debug=false;
    // Fill up input
    if(debug)
        cout << "The dice is: " << dice_roll << endl;

    // setting all to zero at first
    for (int i=0 ; i<17 ; i++)
    {
        input[i]=0;
    }


    for(int i=0; i<4 ;i++) // 4 opponents
    {

        if(debug)
            cout << "The position of piece " << i << " is: " << pos_start_of_turn[i] << endl;
        if(pos_start_of_turn[i]==-1)
            input[i]=pos_start_of_turn[i];
        else if(pos_start_of_turn[i]>0 && pos_start_of_turn[i] != 99)
            input[i]=pos_start_of_turn[i];
        else // if in goal then set it to zero
            input[i]=0;
    }
    int offset=4;

    for(int i=offset; i<offset+4;i++)
    {
        if (pos_start_of_turn[i-offset]!=-1 && pos_start_of_turn[i-offset]!=99)
        {
            int dist=99;
            for (int j=4; j<pos_start_of_turn.size() ; j++) // check for the other players pieces
            {
                if (pos_start_of_turn[j]!=-1 && pos_start_of_turn[j]!=99 && pos_start_of_turn[j]> pos_start_of_turn[i-offset] &&pos_start_of_turn[j]-pos_start_of_turn[i-offset]<dist )
                    dist=(pos_start_of_turn[j]-pos_start_of_turn[i-offset]);
            }
            if(dist!=99)
                input[i]=dist;
            else
                input[i]=0;
        }
        else
        {
            input[i]=0;
        }
    }
    // check for closest star
    offset=8;
    for(int i=0; i<4;i++)
    {
        int j=0;
        bool done=false;
        while(done==false)
        {
            if(pos_start_of_turn[i]<star[j])
            {
                input[i+offset]=(star[j]-pos_start_of_turn[i]);
                done=true;
            }
            else if(j>=7)
            {
                done=true;
            }
            else
            {
                input[i+offset]=0;
            }
            j++;
        }
    }

    // check for closest globe
    offset=12;
    for( int i= 0; i< 4 ; i++)
    {
        if(pos_start_of_turn[i]!=-1 && pos_start_of_turn[i]<52)
        {
            int j=pos_start_of_turn[i];
            bool done=false;
            while(done==false)
            {
                if(j>=52)
                {
                    done=true;
                    input[i+offset]=0;
                }
                else if(j % 13 == 0 || (j - 8) % 13 == 0 )
                {
                    input[i+offset]=(j-pos_start_of_turn[i]);
                    done=true;
                }
                j++;
            }
        }
        else
        {
            input[i+offset]=0;
        }
    }



    input[16]=dice_roll;
    // Calculate output
    auto calc_out =tactic->ann.run(input);

    return -1;
}*/


void ludo_player_train::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
    int decision = make_decision();
    emit select_piece(decision);
}

void ludo_player_train::post_game_analysis(std::vector<int> relative_pos){
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    emit turn_complete(game_complete);
}

