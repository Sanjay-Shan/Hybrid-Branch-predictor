#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>  //for string related operations
#include <typeinfo> //to check the type of the variable in c++
#include <sstream>  
#include <bitset>
#include <bits/stdc++.h>
#include <math.h>
using namespace std;

//utility functions
int to_decimal(string index) {
  int ind= std::stoi(index,nullptr,2); // index in decimal format
  return ind;
}

//convert string to hex to binary
string Hex2Bin(string hexadecimal,int bits){ 
string bin;
stringstream ss(hexadecimal); //convert string to stringstream
ss<< hex << hexadecimal; //stream to hex
unsigned n;
ss >> n;
if (bits==24){
bitset<24> b(n);
bin= b.to_string();}
return bin; // 24 bit binary
} 

string Ind2Slice(int m,string bin ){
string temp=bin.substr(bin.length()-1-(m+1),bin.length()); //index
string index=temp.substr(0,temp.length()-2);
return index;
} 

string stringxor(string GHR, string n_bits){
  string Xor="";
  for(int i=0;i<GHR.length();i++){
     if (GHR[i]==n_bits[i]){
        Xor+="0";
     }
     else{
        Xor+="1";
     }
  }
  return Xor;
}

//function for bimodal prediction
string bimodal_pred(int M2,string hexadecimal,int *bimodal){
     string Pred;
     string binary=Hex2Bin(hexadecimal,24);
     string index=Ind2Slice(M2,binary);
     int dec_ind  =to_decimal(index);
    
     if (bimodal[dec_ind]>=(int)(pow(2, 3))/2){
        Pred="t";
     }
     else{
        Pred="n";
     }
    return Pred+std::to_string(dec_ind); //return prediction + the index
}

//function for gshare prediction
string gshare_pred(int M1,int N, string hexadecimal,int *gshare,string GHR){
  string Pred;
  string binary=Hex2Bin(hexadecimal,24);
  string m_bits=Ind2Slice(M1,binary); //gets the m bits just as in the bimodal i.e. excluding the last 2 bits of the PC
  string m_n_bits= m_bits.substr(0,M1-N); //gets the m-n bits from the m bits of the PC
  string n_bits=Ind2Slice(N,binary); //gets the n bits from the PC
  string Xor=stringxor(GHR,n_bits);//xor between n bit global history register and n_bits
  int index = to_decimal(m_n_bits+Xor);
  
  if (gshare[index]>=(int)(pow(2, 3))/2){
    Pred="t";
  }
  else{
    Pred="n";
  }
return Pred+std::to_string(index); //return prediction + the index
}

//function for bimodal and gshare updation
void bimodal_update(string branch ,string Pred,int index ,int *bimodal){
    if ((Pred=="t" && branch=="t")||(Pred=="n" && branch=="t")){
         bimodal[index]+=1;
         if (bimodal[index]> (int)(pow(2, 3))-1){ //setting m to 3 , as the counter is 3 bit wide ,in case of bimodal 
         bimodal[index]-=1;
       }
       else if (bimodal[index]<0){
         bimodal[index]=0;
       }
       }
       else if ((Pred=="n" && branch=="n")||(Pred=="t" && branch=="n")){
         bimodal[index]-=1;
         if (bimodal[index]> (int)(pow(2, 3))-1){ //setting m to 3 , as the counter is 3 bit wide ,in case of bimodal 
            bimodal[index]-=1;
          }
       else if (bimodal[index]<0){
         bimodal[index]=0;
       }
       }
  return;
}

void gshare_update(string branch ,string Pred,int index,int *gshare){
    if ((Pred=="t" && branch=="t")||(Pred=="n" && branch=="t")){
         gshare[index]+=1;
         if (gshare[index]> (int)(pow(2, 3))-1){ //setting m to 3 , as the counter is 3 bit wide ,in case of bimodal 
         gshare[index]-=1;
       }
       else if (gshare[index]<0){
         gshare[index]=0;
       }
       }
       else if ((Pred=="n" && branch=="n")||(Pred=="t" && branch=="n")){
         gshare[index]-=1;
         if (gshare[index]> (int)(pow(2, 3))-1){ //setting m to 3 , as the counter is 3 bit wide ,in case of bimodal 
            gshare[index]-=1;
          }
       else if (gshare[index]<0){
         gshare[index]=0;
       }
       }
  return;
}


//function for updating the global history table
string shift_register(string branch,string ghr){
  string b;
  if (branch=="t"){b="1";}
  else {b="0";}
  string Ghr=b+ghr; //updating GHR
  Ghr=Ghr.substr(0,Ghr.length()-1);
  return Ghr;
}


int main(int argc, char** argv) // setting up the command line arguments for the cache simulator
{
  // arguments order <branch_predictor_type> <K> <M1> <N> <M2> <trace_file>
  // M1 and N belongs to Gshare
  // M2 belongs to bimodal
  
  string Branch_Pred,Trace,GHR="0";
  // cout<<"im here"<<endl;
  int bimodal[(int)(pow(2, std::stoi(argv[5])))];
  int gshare[(int)(pow(2, std::stoi(argv[3])))];
  int chooser[(int)(pow(2, std::stoi(argv[2])))];
  int K,M1,N,M2;
  Branch_Pred=argv[1];
  K=std::stoi(argv[2]);
  M1=std::stoi(argv[3]);
  N=std::stoi(argv[4]);
  M2=std::stoi(argv[5]);
  Trace=argv[6];
  
//one time initialization of various tables bimodal and ghshare branch prediction table, chooser table, Global history table

for(int i=0;i<((int)(pow(2, M2)));i++){ //initializing the bimodal branch prediction table 
  bimodal[i]=(int)(pow(2, 3))/2;
}
    
if (GHR.length() < N){
  GHR.insert (0,N-GHR.length(), '0');}
      //intializing the global history table
for(int i=0;i<((int)(pow(2, M1)));i++){ //initializing the gshare branch prediction table 
    gshare[i]=(int)(pow(2, 3))/2;
}

for(int i=0;i<((int)(pow(2, K)));i++){ //initializing the chooser table 
    chooser[i]=1;                      //with each entry set to 1
}



  ifstream MyReadFile(Trace); // reading the trace file 

  string T,branch,Hex,Pred1,Pred2,Line;
  vector<string> data;
  int predictions=0,mispredictions=0;

while (getline (MyReadFile, Line)) { //read a line in a file 
    stringstream X(Line);              //convert it to a stringstream
    while (getline(X, T, ' ')) {        // split the string into 2 based on the delimiter
       
       data.push_back(T);} 
    Hex=data[0];
    branch=data[1];
    data.clear(); //clear the vector
    predictions+=1;
 
    //get the predictions of both gshare and bimodal branch predictor
    Pred1 = bimodal_pred(M2,Hex,bimodal);
    Pred2 = gshare_pred(M1,N,Hex,gshare,GHR);

    
   //extracting K from the hexadecimal address
   string binary=Hex2Bin(Hex,24);
   int K_bits=to_decimal(Ind2Slice(K,binary));
   

  //get the index and the check the counter value at the index to select the correct branch predictor

  if (chooser[K_bits]<=1){
    //get the relevant index and the prediction
    int index= std::stoi(Pred1.substr(1,Pred1.length()));
    // cout<<index<<endl;
    string Pred= Pred1.substr(0,1);
    // cout<<Pred1<<"\t"<<index<<"\t"<<Pred<<endl;
    //update the prediction table
    if (Pred!=branch){
         mispredictions+=1;
      }
    if ((Pred=="t" && branch=="t")||(Pred=="n" && branch=="t")){
         bimodal[index]+=1;
         if (bimodal[index]> (int)(pow(2, 3))-1){ //setting m to 3 , as the counter is 3 bit wide ,in case of bimodal 
         bimodal[index]-=1;
       }
       else if (bimodal[index]<0){
         bimodal[index]=0;
       }
       }
       else if ((Pred=="n" && branch=="n")||(Pred=="t" && branch=="n")){
         bimodal[index]-=1;
         if (bimodal[index]> (int)(pow(2, 3))-1){ //setting m to 3 , as the counter is 3 bit wide ,in case of bimodal 
            bimodal[index]-=1;
          }
       else if (bimodal[index]<0){
         bimodal[index]=0;
       }
       }
    // cout<<index<<"\t"<<bimodal[index]<<endl;
  }

  else if (chooser[K_bits]>=2){
    //get the relevant index and the prediction
    int index= std::stoi(Pred2.substr(1,Pred2.length()));
    string Pred= Pred2.substr(0,1);

    // cout<<Pred2<<"\t"<<index<<"\t"<<Pred<<endl;
    if (Pred!=branch){
         mispredictions+=1;
      }
    //update the prediction table
    if ((Pred=="t" && branch=="t")||(Pred=="n" && branch=="t")){
         gshare[index]+=1;
         if (gshare[index]> (int)(pow(2, 3))-1){ //setting m to 3 , as the counter is 3 bit wide ,in case of bimodal 
         gshare[index]-=1;
       }
       else if (gshare[index]<0){
         gshare[index]=0;
       }
       }
       else if ((Pred=="n" && branch=="n")||(Pred=="t" && branch=="n")){
         gshare[index]-=1;
         if (gshare[index]> (int)(pow(2, 3))-1){ //setting m to 3 , as the counter is 3 bit wide ,in case of bimodal 
            gshare[index]-=1;
          }
       else if (gshare[index]<0){
         gshare[index]=0;
       }
       }
  }
  //update the GHR irrespective of the chosen predictor
  GHR=shift_register(branch,GHR);
  
  // cout<<Pred1.substr(0,1)<<"\t"<<Pred2.substr(0,1)<<"\t"<<branch<<"\t"<<K_bits<<"\t"<<chooser[K_bits]<<endl;

  //update the chooser table
  if ((Pred1.substr(0,1)==branch) && (Pred2.substr(0,1)!=branch)){
    chooser[K_bits]-=1;
    if (chooser[K_bits]<0){
      chooser[K_bits]=0;
    }
  }
  else if ((Pred2.substr(0,1)==branch) && (Pred1.substr(0,1)!=branch)){
    chooser[K_bits]+=1;
    if (chooser[K_bits]>3){
      chooser[K_bits]=3;
    }
  }
}
 
  // printing the output as per the given format
    cout<<"COMMAND"<<endl;
    for(int i=0;i<argc;i++){
      cout<<argv[i]<<"\t";
    }
    cout<<endl;
    cout<<"OUTPUT"<<endl;
    cout<<"number of predictions:"<<"\t"<<predictions<<endl;
    cout<<"number of mispredictions:"<<"\t"<<mispredictions<<endl;
    float misprediction_rate= (mispredictions/float(predictions))*100;
    cout<<"misprediction rate:"<<"\t"<<fixed<<setprecision(2)<<misprediction_rate<<"%"<<endl;

    cout<<"FINAL CHOOSER CONTENTS"<<endl;
    for(int i=0;i<((int)(pow(2, K)));i++){ //print the final values of the chooser table 
        cout<<i<<"\t"<<chooser[i]<<endl;
      }

    cout<<"FINAL GSHARE CONTENTS"<<endl;
    for(int i=0;i<((int)(pow(2, M1)));i++){ //print the final values of the gshare table 
        cout<<i<<"\t"<<gshare[i]<<endl;
      }

    cout<<"FINAL BIMODAL CONTENTS"<<endl;
    for(int i=0;i<((int)(pow(2, M2)));i++){ //print the final values of the bimodal table 
        cout<<i<<"\t"<<bimodal[i]<<endl;
      }

    

return 0;
}


