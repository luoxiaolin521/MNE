#include<iostream>
#include<map>
#include<vector>
#include<cmath>
#include<fstream>
#include<cstring>
#include<sstream>
#include<stdlib.h>
#include<algorithm>
#include<assert.h>
#include<sstream>
#include<ctime>
    
    
using namespace std;



struct tuple
{
    int _id;
    int _rela;
    
    tuple(int id,int rela) {
        _id=id;
        _rela=rela;
    }
};

struct FindTuple
{
    const tuple t;
    FindTuple(const tuple _t) : t(_t) {}
    bool operator()(const tuple tu) const{
        return t._id==tu._id && t._rela==tu._rela;
    }
};



map<string,int> entity2id;
map<string,int> relation2id;
map<int,vector<tuple> > entity_out;
map<int,vector<tuple> > entity_in;

vector<vector<double> > entity_vec;
vector<vector<double> > entity_context;
vector<vector<double> > entity_recontext;
vector<vector<double> > relation_vec;

int entitynum=0;
int relationnum=0;
#define entitydim 100
double rho=0.025;
vector<double> sigmoid_table;
#define sigmoid_table_size 1000
#define SIGMOID_BOUND 6
char buf[100000];
#define pi 3.1415926535897932384626433832795

clock_t start,finish;

vector<string> split(string str,char deli)
{
    vector<string> strings;
    istringstream f(str.c_str());
    string s;
    while(getline(f,s,deli)) {
        strings.push_back(s);
    }   
    return strings;
}

double myrandom()
{
    
    
    double deci=(double)rand() / RAND_MAX;
    return (deci - 0.5) / entitydim;
}
double rand(double min, double max)//²úÉú£¨min,max£©Ö®¼äµÄËæ»úÊý
{
    return min+(max-min)*rand()/(RAND_MAX+1.0);
}
double norm(double x, double miu,double sigma)//¸ßË¹±ê×¼»¯
{
    return 1.0/sqrt(2*pi)/sigma*exp(-1*(x-miu)*(x-miu)/(2*sigma*sigma));
}
double randn(double miu,double sigma, double min ,double max)
{
    double x,y,dScope;
    do{
        x=rand(min,max);
        y=norm(x,miu,sigma);
        dScope=rand(0.0,norm(miu,miu,sigma));
    }while(dScope>y);
    return x;
}

double dot(const vector<double>& v1,const vector<double>& v2)
{
    assert(v1.size()==v2.size());
    double r=0;
    for(int i=0;i<v1.size();i++) {
        r+=v1[i]*v2[i];
    }
    return r;
}

double norm2(const vector<double>& vec)
{
    double r=dot(vec,vec);
    return sqrt(r);
}

void InitSigmoidTable()
{
    int k=0;
    while(k < sigmoid_table_size) {
        double x = 2. * SIGMOID_BOUND * k / sigmoid_table_size - SIGMOID_BOUND;
        sigmoid_table. push_back(1. / (1. + exp(-x)));
        k++; 
    }
}

double FastSigmoid(double x)
{
    if(x > SIGMOID_BOUND) {
        return 1;
    } else if(x < -SIGMOID_BOUND) {
        return 0;
    } else {
        double k=(x + SIGMOID_BOUND) * sigmoid_table_size / SIGMOID_BOUND / 2;
        return sigmoid_table[(int)k];
    }
}

void write(string filename,vector<vector<double> > dic) 
{
    ofstream  fout(filename.c_str());
    for(int i=0;i<dic.size();i++) {
        if(i!=0) {fout<<endl;}
        for(int j=0;j<dic[i].size();j++) {
            if(j!=0){fout<<"\t";}
            fout<<dic[i][j];
        }
    }
    fout.close(); 
}
void read_id()
{
    // ifstream fin("entity2id.txt",ios::in);
    // char line[256];
    // while(!fin.eof()) {
    //     fin.getline(line,256);
    //     vector<string> ss=split(line,'\t');
    //     if(entity2id.count(ss[0])==0) {
    //         entity2id[ss[0]]=atoi(ss[1].c_str());
    //         entitynum++;
    //     }
    // }
    // fin.close();
    FILE* f1 = fopen("entity2id.txt","r");
    int x;
    while (fscanf(f1,"%s%d",buf,&x)==2)
    {
        string st=buf;
        entity2id[st]=x;
        entitynum++;
    }
    fclose(f1);
}

void read_relation()
{
    // ifstream fin("relation2id.txt",ios::in);
    // char line[256];
    // while(!fin.eof()) {
    //     fin.getline(line,256);
    //     vector<string> ss=split(line,'\t');
    //     if(relation2id.count(ss[0])==0) {
    //         relation2id[ss[0]]=atoi(ss[1].c_str());
    //         relationnum++;
    //     }
    // }
    // fin.close();
    FILE* f2 = fopen("relation2id.txt","r");
    int x;
    while (fscanf(f2,"%s%d",buf,&x)==2)
    {
        string st=buf;
        relation2id[st]=x;
        relationnum++;
    }
    fclose(f2);
}    


void read_train()
{
    // ifstream fin("train.txt",ios::in);
    // char line[256];
    // while(!fin.eof()) {
    //     fin.getline(line,256);
    //     vector<string> ss=split(line,'\t');
    //     int h=entity2id[ss[0]];
    //     int l=entity2id[ss[1]];
    //     int r=relation2id[ss[2]];
    //     if(find_if(entity_out[h].begin(),entity_out[h].end(),FindTuple(tuple(l,r)))==entity_out[h].end()) {
    //         entity_out[h].push_back(tuple(l,r));
    //     }
    //     if(find_if(entity_in[l].begin(),entity_in[l].end(),FindTuple(tuple(h,r)))==entity_in[l].end()) {
    //         entity_in[l].push_back(tuple(h,r));
    //     }
    // }
    // fin.close();

    FILE* f_kb = fopen("train.txt","r");
    while (fscanf(f_kb,"%s",buf)==1)
    {
        string s1=buf;
        fscanf(f_kb,"%s",buf);
        string s2=buf;
        fscanf(f_kb,"%s",buf);
        string s3=buf;

        int h=entity2id[s1];
        int l=entity2id[s2];
        int r=relation2id[s3];

        if(find_if(entity_out[h].begin(),entity_out[h].end(),FindTuple(tuple(l,r)))==entity_out[h].end()) {
            entity_out[h].push_back(tuple(l,r));
        }
        if(find_if(entity_in[l].begin(),entity_in[l].end(),FindTuple(tuple(h,r)))==entity_in[l].end()) {
            entity_in[l].push_back(tuple(h,r));
        }
    }
    fclose(f_kb);
}
   
void InitVector()
{
    double rand_num;
    int n = entitydim;
    srand(time(NULL));
    for(int i=0;i<entitynum;i++) {
        vector<double> v,v_context,v_recontext;
        for(int j=0;j<entitydim;j++) {
            rand_num = randn(0,1.0/n,-6/sqrt(n),6/sqrt(n));
            // v.push_back(myrandom());
            v.push_back(rand_num);
            v_context.push_back(0.0);
            v_recontext.push_back(0.0);
        }
        entity_vec.push_back(v);
        entity_context.push_back(v_context);
        entity_recontext.push_back(v_recontext);
    }
    for(int i=0;i<relationnum;i++) {
        vector<double> v;
        for(int j=0;j<entitydim;j++) {
            // v.push_back(myrandom());
            rand_num = randn(0,1.0/n,-6/sqrt(n),6/sqrt(n));
            v.push_back(rand_num);
        }
        relation_vec.push_back(v);
    } 
}

void normal(vector<double>& vec)
{
    double y=norm2(vec);
    if(y>0) {
        for(int i=0;i<vec.size();i++) {
            vec[i]/=y;
        }
    }
}

void update_0(int a1,int b0,int c0,int rel_b,int rel_c,int label)
{
    // vector<double> vec_error_1(entitydim);
    double *vec_error_1 = (double *)calloc(entitydim, sizeof(double));
    for (int c = 0; c != entitydim; c++) vec_error_1[c] = 0;

    double y=0,g=0;
    for(int c=0;c<entitydim;c++) {
        double a1_c,b0_c,c0_c,rel_b_c,rel_c_c;
        a1_c=entity_vec[a1][c];
        b0_c=entity_recontext[b0][c];
        c0_c=entity_recontext[c0][c];
        rel_b_c=relation_vec[rel_b][c];
        rel_c_c=relation_vec[rel_c][c];
        y+=a1_c * (b0_c + c0_c + rel_b_c + rel_c_c);
    }
    y=FastSigmoid(y);
    g=(label - y) * rho;
    
    for(int i=0;i<entitydim;i++) {
        vec_error_1[i] += g * (entity_recontext[b0][i] + entity_recontext[c0][i] + relation_vec[rel_b][i] + relation_vec[rel_c][i]);
        entity_recontext[b0][i] += g * entity_vec[a1][i];
        entity_recontext[c0][i] += g * entity_vec[a1][i];
        relation_vec[rel_b][i] += g * entity_vec[a1][i];
        relation_vec[rel_c][i] += g * entity_vec[a1][i];
    }
    for(int c=0;c<entitydim;c++) {
        entity_context[a1][c] += vec_error_1[c];
    }
    free(vec_error_1);
}

void update_1(int a0,int b1,int c0,int rel_b,int rel_c,int label)
{
    // vector<double> vec_error_0(entitydim);
    // vector<double> vec_error_1(entitydim);
    double *vec_error_0 = (double *)calloc(entitydim, sizeof(double));
    for (int c = 0; c != entitydim; c++) vec_error_0[c] = 0;
    double *vec_error_1 = (double *)calloc(entitydim, sizeof(double));
    for (int c = 0; c != entitydim; c++) vec_error_1[c] = 0;

    double y=0,g=0;
    for(int i=0;i<entitydim;i++) {
        y+=entity_context[b1][i] * (entity_vec[a0][i] + relation_vec[rel_b][i]) + entity_vec[a0][i] * (entity_recontext[c0][i] + relation_vec[rel_c][i]);
    }
    y=FastSigmoid(y);
    g=(label - y) * rho;
    for(int i=0;i<entitydim;i++) {
        vec_error_0[i]+=g*(entity_context[b1][i]+entity_recontext[c0][i] + relation_vec[rel_c][i]);//entity_vec[a0][i]
        //vec_error_1[i]+=g*(entity_vec[c0][i]+relation_vec[rel_c][i]);
        vec_error_1[i]+=g*(entity_vec[a0][i] + relation_vec[rel_b][i]);//entity_context[b1][i]
        entity_recontext[c0][i]+=g*entity_vec[a0][i];
        relation_vec[rel_b][i]+=g*entity_context[b1][i];
        relation_vec[rel_c][i]+=g*entity_vec[a0][i];
        //b1=k,c0=j
    }
    for(int c=0;c<entitydim;c++) {
        entity_vec[a0][c]+=vec_error_0[c];
	entity_context[b1][c]+=vec_error_1[c];
        //entity_context[a0][c]+=vec_error_1[c];
    }
    free(vec_error_0);
    free(vec_error_1);
}

void update_2(int a0,int b1,int c1,int rel_b,int rel_c,int label)
{
    //vector<double> vec_error_0(entitydim);
    double *vec_error_0 = (double *)calloc(entitydim, sizeof(double));
    for (int c = 0; c != entitydim; c++) vec_error_0[c] = 0;
    double *vec_error_1 = (double *)calloc(entitydim, sizeof(double));
    for (int c = 0; c != entitydim; c++) vec_error_1[c] = 0;
    double *vec_error_2 = (double *)calloc(entitydim, sizeof(double));
    for (int c = 0; c != entitydim; c++) vec_error_2[c] = 0;
    double y=0,g=0;
    for(int i=0;i<entitydim;i++) {
        y+=entity_context[b1][i]*(entity_vec[a0][i]+relation_vec[rel_b][i])+entity_context[c1][i]*(entity_vec[a0][i]+relation_vec[rel_c][i]);
    }
    y=FastSigmoid(y);
    g=(label-y)*rho;
    for(int i=0;i<entitydim;i++) {
        vec_error_0[i]+=g*(entity_context[b1][i]+entity_context[c1][i]);
        vec_error_1[i]+=g*(entity_vec[a0][i]+relation_vec[rel_b][i]);//entity_context[b1][i]
        vec_error_2[i]+=g*(entity_vec[a0][i]+relation_vec[rel_c][i]);//entity_context[c1][i]
        relation_vec[rel_b][i]+=g*entity_context[b1][i];
        relation_vec[rel_c][i]+=g*entity_context[c1][i];
    }
    for(int c=0;c<entitydim;c++) {
        entity_vec[a0][c]+=vec_error_0[c];
	entity_context[b1][c]+=vec_error_1[c];
	entity_context[c1][c]+=vec_error_2[c];
    }
    free(vec_error_0);
    free(vec_error_1);
    free(vec_error_2);
}

void train()
{
    for(int iter2=0;iter2<1000;iter2++) {
        cout<<"iter2="<<iter2<<endl;
        cout<<"rho="<<rho<<endl;
        for(map<int,vector<tuple> >::iterator it = entity_in.begin();it!=entity_in.end();it++) {
            int x=it->second.size();
            if(x >= 2) {
                int i=0;
                while(i < x - 1) {
                    // int j=i+1;
                    int j = int(rand(i+1,x));
                    int vertex_b,vertex_c,relation_b,relation_c;
                    int index = 1;
                    int flag = 0;
                    while(it->second[j]._id == it->second[i]._id) {
                        j = int(rand(i+1,x));
                        index++;
                        if (index > x-i-1)
                        {
                            flag = 1;
                            break;
                        }
                    }
                    vertex_b=it->second[i]._id;
                    vertex_c=it->second[j]._id;
                    relation_b=it->second[i]._rela;
                    relation_c=it->second[j]._rela;
                    if (flag == 0) {
                        update_0(it->first,vertex_b,vertex_c,relation_b,relation_c,1);
                        
                        normal(entity_vec[it->first]);
                        normal(entity_recontext[vertex_b]);
                        normal(entity_recontext[vertex_c]);
                        normal(relation_vec[relation_b]);
                        normal(relation_vec[relation_c]);

                    }
                    int sample=-1;
                    int relation=-1;
                    do {
                        sample=rand()%entitynum;
                    } while(sample == it->first || sample == vertex_b);
                    do {
                        relation=rand()%relationnum;
                    }while(find_if(it->second.begin(),it->second.end(),FindTuple(tuple(sample,relation))) != it->second.end());
                    update_0(it->first,vertex_b,sample,relation_b,relation,0);

                    normal(entity_vec[it->first]);
                    normal(entity_recontext[vertex_b]);
                    normal(entity_recontext[sample]);
                    normal(relation_vec[relation_b]);
                    normal(relation_vec[relation]);

                    i++;
                }
            }
        }
        cout<<"update_0 successful"<<endl;
        
        for(map<int,vector<tuple> >::iterator it=entity_in.begin();it!=entity_in.end();it++) {
            int key=it->first;
            if(entity_out.count(key) > 0) {
                int x=entity_in[key].size();
                int y=entity_out[key].size();
                int i=0;
                int vertex_c,relation_c,vertex_b,relation_b;
                while(i < x) {
                    // int j=0;
                    int j = int(rand(0,y));
                    vertex_b=entity_out[key][j]._id;
                    relation_b=entity_out[key][j]._rela;

                    vertex_c=entity_in[key][i]._id;
                    relation_c=entity_in[key][i]._rela;
                    int index = 1;
                    int flag = 0;
                    while(vertex_c == vertex_b) {
                        j = int(rand(0,y));
                        vertex_b=entity_out[key][j]._id;
                        relation_b=entity_out[key][j]._rela;
                        index++;
                        if (index > y) {
                            flag = 1;
                            break;
                        }
                    }
                    if (flag == 0) {
                        update_1(key,vertex_b,vertex_c,relation_b,relation_c,1);

                        normal(entity_vec[key]);
                        //normal(entity_context[key]);
                        normal(entity_context[vertex_b]);
                        normal(entity_recontext[vertex_c]);
                        normal(relation_vec[relation_b]);
                        normal(relation_vec[relation_c]);

                    }
                    int sample=-1,relation=-1;
                    do{
                        sample=rand()%entitynum;
                    } while(sample == key || sample == vertex_c);
                    do{
                        relation=rand()%relationnum;
                    } while(find_if(it->second.begin(),it->second.end(),FindTuple(tuple(sample,relation))) != it->second.end());
                    
                    update_1(key,sample,vertex_c,relation,relation_c,0);

                    normal(entity_vec[key]);
                    //normal(entity_context[key]);
                    normal(entity_context[sample]);
                    normal(entity_recontext[vertex_c]);
                    normal(relation_vec[relation]);
                    normal(relation_vec[relation_c]);

                    i++;
                }
            }
        }
        cout<<"update_1 successful"<<endl;
        
        for(map<int,vector<tuple> >::iterator it=entity_out.begin();it!=entity_out.end();it++) {
            int key=it->first;
            int x=it->second.size();
            
            if(x >= 2) {
                int i=0;
                
                while(i < x - 1) {
                    // int j=i+1;
                    int j = int(rand(i+1,x));
                    int index = 1;
                    int flag = 0;
                    int vertex_c,relation_c,vertex_b,relation_b;
                    
                    while(it->second[i]._id == it->second[j]._id) {
                        j = int(rand(i+1,x));
                        index++;
                        if (index > x-i-1) {
                            flag = 1;
                            break;
                        }
                    }
                    vertex_b=it->second[i]._id;
                    relation_b=it->second[i]._rela;
                    vertex_c=it->second[j]._id;
                    relation_c=it->second[j]._rela;
                    if (flag == 0) {
                        update_2(key,vertex_b,vertex_c,relation_b,relation_c,1);

                        normal(entity_vec[key]);
                        normal(entity_context[vertex_b]);
                        normal(entity_context[vertex_c]);
                        normal(relation_vec[relation_b]);
                        normal(relation_vec[relation_c]);

                    }
                    int sample=-1,relation=-1;
                    do{
                        sample=rand()%entitynum;
                    } while(sample == key || sample == vertex_b);
                    do{
                        relation=rand()%relationnum;
                    } while(find_if(it->second.begin(),it->second.end(),FindTuple(tuple(sample,relation))) != it->second.end());
                    update_2(key,vertex_b,sample,relation_b,relation,0);

                    normal(entity_vec[key]);
                    normal(entity_context[vertex_b]);
                    normal(entity_context[sample]);
                    normal(relation_vec[relation_b]);
                    normal(relation_vec[relation]);

                    i++;
                }
            }
        }
        cout<<"update_2 sucessful"<<endl;
        if(iter2%200 == 0 && iter2 != 0) 
        {
            rho*=0.5;
        }
        if(iter2%50 == 0 && iter2 != 0) 
        {
            finish = clock();
            int duration = (int)(finish-start)/CLOCKS_PER_SEC;
            cout <<"iter = "<<iter2<<";"<<"duration = "<<duration<<endl;
            /*FILE *fo;
            char output_file[100];
            output_file = "result/time_res.txt".c_str();
            fo = fopen(output_file, "a");
            fprintf(fo, "iter = %d ; duration = %d\n", iter2, duration);
            fclose(fo);*/

            stringstream ss;
            ss << iter2;
            string striter2 = ss.str();
            write("result1/"+striter2+"entity_vec.txt",entity_vec);
            write("result1/"+striter2+"entity_context",entity_context);
            write("result1/"+striter2+"relation_vec.txt",relation_vec);
	    write("result1/"+striter2+"entity_recontext.txt",relation_vec);
        }
    }
}



int main()
{
    start = clock();
    InitSigmoidTable();
    read_id();
    read_relation();
    read_train();
    InitVector();
    train();
    write("result1/entity_vec.txt",entity_vec);
    write("result1/entity_context",entity_context);
    write("result1/relation_vec.txt",relation_vec);
    write("result1/entity_recontext.txt",relation_vec);
    return 0;
} 

