#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <unordered_map>
#include <stdint.h>
#include <set>
#include <limits>
#include <boost/circular_buffer.hpp>



struct rw_instruction
{
    std::string read;
    std::string write;
};

class Optimizator{
private:
    std::vector<rw_instruction>program;
    uint32_t time = 0;
    std::unordered_map<uint16_t, std::pair<uint64_t, int32_t>>regs_source;  //source of registers data
    std::unordered_map<uint64_t, std::pair<uint64_t, int32_t>>mem_source;  //source of memory data
    std::unordered_map<uint64_t, std::set<uint16_t>>related_regs; //related registers to memory adresses

    bool dumb_flag=0;

    bool overlap_flag=0;

    void unrelate_reg(uint16_t reg);
    void unrelate_mem(uint64_t mem);
    void delete_dumb();
    void delete_overlap();
public:

    Optimizator(std::vector<rw_instruction> program_, bool delete_dumb_ = 0, bool delete_overlap_=0);


    void execute();


};
