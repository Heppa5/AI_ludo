#include "individual.h"

individual::individual()
{

}

individual::individual(int min_weig, int max_weig, int reso, int gene)
{
    ann.create_standard_array(num_layers,layers);
    ann.set_activation_function_hidden(SIGMOID_SYMMETRIC);
    ann.set_activation_function_output(LINEAR_PIECE_SYMMETRIC);

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


vector<individual*> individual::make_two_children(individual *mother, int cur_gen){
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
        children1->genes.push_back(gene1);
        children2->genes.push_back(gene2);
    }
    children1->generate_connections_from_genes();
    children2->generate_connections_from_genes();
    vector<individual*> two_children;
    two_children.push_back(children1);
    two_children.push_back(children2);
    return two_children;
}

void individual::adjust_weights_to_resolution(){

    cout << connum << endl;
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
    for (uint i=0 ; i < connum ; i++)
    {
        for( int j=0 ; j< pow(2,resolution) ; j++)
        {
            if((double)con[i].weight == (double)(min_weight_value+(j*1/(pow(2,resolution)))*range_weights))
            {
                genes.push_back(j);
            }
        }
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

void individual::true_random_weights()
{
    random_device gen;
    std::uniform_int_distribution<> dis(0, pow(2,resolution)-1);

    for (int i=0; i< connum ; i++)
    {
        int random_number = dis(gen);
        con[i].weight = (min_weight_value+range_weights*random_number*1/pow(2,resolution));
    }
    ann.set_weight_array(con,connum);
}
