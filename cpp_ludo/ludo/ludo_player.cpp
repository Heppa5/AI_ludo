#include "ludo_player.h"
#include <random>

ludo_player::ludo_player():
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
}

/*int ludo_player::make_decision(){
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

int ludo_player::make_decision(){
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
    /*if(calc_out[0]>1 || calc_out[0]<(-1))
        cout << "###### Dont trust fann ###########" << endl;*/
    //bool debug=false;
    if (debug)
    {
        cout << "############################### MOVE GETTING DECIDED ###############################" << endl;
        cout << "Piece pos " << 0 << " " << input[0] << "\n";
        cout << "Piece pos " << 1 << " " << input[1] << "\n";
        cout << "Piece pos " << 2 << " " << input[2] << "\n";
        cout << "Piece pos " << 3 << " " << input[3] << "\n";
        cout << "Piece nearest opp " << 0 << " " << input[4] << "\n";
        cout << "Piece nearest opp " << 1 << " " << input[5] << "\n";
        cout << "Piece nearest opp " << 2 << " " << input[6] << "\n";
        cout << "Piece nearest opp " << 3 << " " << input[7] << "\n";
        cout << "Piece nearest star " << 0 << " " << input[8] << "\n";
        cout << "Piece nearest star " << 1 << " " << input[9] << "\n";
        cout << "Piece nearest star " << 2 << " " << input[10] << "\n";
        cout << "Piece nearest star " << 3 << " " << input[11] << "\n";
        cout << "Piece nearest globe " << 0 << " " << input[12] << "\n";
        cout << "Piece nearest globe " << 1 << " " << input[13] << "\n";
        cout << "Piece nearest globe " << 2 << " " << input[14] << "\n";
        cout << "Piece nearest globe " << 3 << " " << input[15] << "\n";
        cout << "Dice roll " << input[16] << "\n";
        cout << "\n Output is: " << calc_out[0] << endl;
    }
    if(calc_out[0] < -0.5 )
    {
        if(debug)
            std::cout << calc_out[0] << " and we choose 0" << std::endl;
        return 0;
    }
    else if(calc_out[0] > -0.5 && calc_out[0]<0)
    {
        if(debug)
            std::cout << calc_out[0] << " and we choose 1" << std::endl;
        return 1;
    }
    else if(calc_out[0]>0 && calc_out[0]<0.5)
    {
        if(debug)
            std::cout << calc_out[0] << " and we choose 2" << std::endl;
        return 2;
    }
    else if(calc_out[0]>0.5)
    {
        if(debug)
            std::cout << calc_out[0] << " and we choose 3" << std::endl;
        return 3;
    }
    return -1;
}


void ludo_player::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
    int decision = make_decision();
    emit select_piece(decision);
}

void ludo_player::post_game_analysis(std::vector<int> relative_pos){
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    emit turn_complete(game_complete);
}
