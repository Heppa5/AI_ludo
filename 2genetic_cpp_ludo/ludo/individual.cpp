#include "individual.h"

individual::individual()
{

}

individual::individual(double min_weig, double max_weig, int reso, int gene)
{
    ann.create_standard_array(num_layers,layers);
    ann.set_activation_function_hidden(SIGMOID_SYMMETRIC);
    ann.set_activation_function_output(SIGMOID_SYMMETRIC);

    min_weight_value=min_weig;
    max_weight_value=max_weig;
    range_weights=max_weig-min_weig;
    resolution=reso;
    connum=ann.get_total_connections();
    evaluation=0;
    generation=gene;
    wins=0;

    con=(connection*)calloc(connum, sizeof(*con));
    ann.get_connection_array(con);
}


/*vector<individual*> individual::make_two_children(individual *mother, int cur_gen){
    random_device gen;
    bool debug=false;
    std::uniform_int_distribution<> dis(0, pow(2,resolution)-1);
    unsigned int bit_mask = dis(gen);
    if(debug)
        cout << "Bit mask is: " <<  bit_mask <<endl;
    individual* children1 = new individual(min_weight_value,max_weight_value, resolution, cur_gen);
    individual* children2 = new individual(min_weight_value,max_weight_value, resolution, cur_gen);

    for(int i=0; i< genes.size() ; i++) // bit 1 -> take fathers gen to children1  / bit 0 -> take fathers gene to children2
    {
        int gene1=0;
        int gene2=0;
        for (int j=0 ; j<resolution ; j++)
        {
            if (debug)
                cout << "Bit mask is: " <<  bit_mask <<endl;
            int bit = (bit_mask & ( 1 << j ))>>j;
            int father_bit = (genes[i] & ( 1 << j ))>>j;
            int mother_bit = (mother->genes[i] & ( 1 << j ))>>j;
            if(debug)
            {
                cout << "We are at iteration " << j << " and bit from bit mask is: " << bit << " and bit from mother is: " << mother_bit << " and bit from father is: " << father_bit <<  endl;
                cout << "Father gene: " << genes[i] << " Mother gene: " << mother->genes[i] << endl;
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
        if(debug)
            cout << "GENE 1 is: " << gene1  << "\tGENE 2 is: " << gene2<< endl;
        children1->genes.push_back(gene1);
        children2->genes.push_back(gene2);
    }
    children1->generate_connections_from_genes();
    children2->generate_connections_from_genes();
    vector<individual*> two_children;
    two_children.push_back(children1);
    two_children.push_back(children2);
    return two_children;
}*/

void individual::adjust_weights_to_resolution(){

    //cout << connum << endl;
    //cout << min_weight_value << " " << max_weight_value << " " << range_weights << endl;
    for (uint i = 0; i < connum; ++i)
    {
        double cur_weight=(double)con[i].weight;
        double error=99;
        int min_error_index=0;

        for( int j=0 ; j< pow(2,resolution) ; j++)
        {
            if(abs(cur_weight-(min_weight_value+(j*1/(pow(2,resolution)))*range_weights))< error )
            {

                error=abs(cur_weight-(min_weight_value+(j*1/(pow(2,resolution)))*range_weights));
                min_error_index=j;
            }
        }
        con[i].weight=min_weight_value+(min_error_index*1/(pow(2,resolution)))*range_weights;
    }
    //cout << "Jeg kom ud" << endl;
    //print_connections();
    ann.set_weight_array(con,connum);
    //cout << "wow" << endl;
}

void individual::convert_connections_to_genes(){
    genes.clear();
    bool debug=false;
    if(debug)
        cout << min_weight_value << "\t"<< max_weight_value << "\t" << resolution << "\t" << range_weights << endl;
    for (uint i=0 ; i < connum ; i++)
    {
        /*for( int j=0 ; j< pow(2,resolution) ; j++)
        {
            cout << j << "\t" <<((double)con[i].weight) << "\t" << (double)(min_weight_value+(j*1/(pow(2,resolution)))*range_weights) << endl;
            if((double)con[i].weight == (double)(min_weight_value+(j*1/(pow(2,resolution)))*range_weights))
            {
                if(debug)
                    cout << "MATCH" << endl;
                genes.push_back(j);
            }
        }*/
        double nom=(double)con[i].weight-min_weight_value;
        double dem=1/(pow(2,resolution))*range_weights;
        double res=nom/dem;
        if(debug)
        {
            double test_con=(min_weight_value+range_weights*res*1/pow(2,resolution));
            cout << res << "\t" << con[i].weight << "\t" << test_con << endl;
        }
        genes.push_back((int)res);
    }
}

void individual::generate_connections_from_genes()
{
    for (int i=0; i< connum ; i++)
    {
        //con[i].weight=(-1+genes[i]*2/pow(2,resolution));
        con[i].weight = (min_weight_value+range_weights*genes[i]*1/pow(2,resolution));
        //cout << con[i].weight << "\t" << genes[i] << endl;
    }
    ann.set_weight_array(con,connum);
}

/*void individual::true_random_weights()
{
    random_device gen;
    std::uniform_int_distribution<> dis(0, pow(2,resolution)-1);

    for (int i=0; i< connum ; i++)
    {
        int random_number = dis(gen);
        con[i].weight = (min_weight_value+range_weights*random_number*1/pow(2,resolution));
    }
    ann.set_weight_array(con,connum);
}*/

void individual::true_random_weights()
{
    random_device gen;
    std::uniform_real_distribution<> dis(min_weight_value, max_weight_value);

    for (int i=0; i< connum ; i++)
    {
        double random_number = dis(gen);
        con[i].weight = random_number;
    }
    ann.set_weight_array(con,connum);
}

vector<individual*> individual::make_two_children(individual *mother, int cur_gen){
    random_device gen;
    bool debug=false;
    std::uniform_int_distribution<> dis(0, 1);

    individual* children1 = new individual(min_weight_value,max_weight_value, resolution, cur_gen);
    individual* children2 = new individual(min_weight_value,max_weight_value, resolution, cur_gen);

    for(int i=0; i< connum ; i++) // bit 1 -> take fathers gen to children1  / bit 0 -> take fathers gene to children2
    {
        double noise_father=abs(con[i].weight*percent_noise);
        double noise_mother=abs(mother->con[i].weight*percent_noise);
        if(noise_mother<minimum_noise)
        {
            noise_mother=minimum_noise;
        }
        if(noise_father<minimum_noise)
        {
            noise_father=minimum_noise;
        }
        std::uniform_real_distribution<> dis_father(con[i].weight-noise_father, con[i].weight+noise_father);
        std::uniform_real_distribution<> dis_mother(mother->con[i].weight-noise_mother, mother->con[i].weight+noise_mother);
        unsigned int bit= dis(gen);
        if(bit==1)
        {
            children1->con[i].weight = dis_father(gen);
            children2->con[i].weight = dis_mother(gen);
        }
        else
        {
            children1->con[i].weight = dis_mother(gen);
            children2->con[i].weight = dis_father(gen);
        }
    }
    children1->ann.set_weight_array(children1->con,connum);
    children2->ann.set_weight_array(children2->con,connum);
    vector<individual*> two_children;
    two_children.push_back(children1);
    two_children.push_back(children2);
    return two_children;
}

/*void individual::true_random_weights()
{


    for (i; i < (layers[0]+1)*4 ; i++)
    {
        int random_number = dis(gen);

        if( ( con[i].from_neuron%4 == (con[i].to_neuron-2)%4 || con[i].from_neuron==(layers[0]-1) ) && con[i].from_neuron!=layers[0])
        {
           con[i].weight = (min_weight_value+range_weights*random_number*1/pow(2,resolution));
        }
        else
        {
            con[i].weight = 0;
        }

    }
    for (i; i< connum ; i++)
    {
        int random_number = dis(gen);
        con[i].weight = (min_weight_value+range_weights*random_number*1/pow(2,resolution));
    }
    ann.set_weight_array(con,connum);
}*/
