#include "../include/optimizator.h"


int main(int argc, char* argv[]){

    Optimizator opt1({{"ffffb396be70","r3"},{"r3","r3"}, {"r3", "ffffb396be70"},
                      {"fffff23b2fd0", "r2"}, {"r2",""}, {"ffffb396be80", "r3"},
                      {"r3","r3"}, {"r3", "ffffb396be80"}, {"fffff23b2fd8", "r2"},
                      {"r2",""}, {"ffffb396bef0", "r3"}, {"r3","r3"},{"r3", "ffffb396bef0"},
                      {"fffff23b3058", "r2"}, {"r2",""}, {"ffffb396bee0", "r3"},{"r3","r3"},
                      {"r3", "ffffb396bee0"}, {"fffff23b3160", "r2"}, {"r2",""}}, argv[1][0]-'0', argv[2][0]-'0');


    opt1.execute();

}