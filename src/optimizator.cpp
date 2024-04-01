#include "../include/optimizator.h"

Optimizator::Optimizator(std::vector<rw_instruction> program_, bool delete_dumb_, bool delete_overlap_){
    program = program_;
    dumb_flag = delete_dumb_;
    overlap_flag = delete_overlap_;
}

void Optimizator::unrelate_reg(uint16_t reg){

    uint64_t mem_addr = regs_source[reg].first;

    regs_source[reg] = {-1,0};    

    related_regs[mem_addr].erase(reg);

}

void Optimizator::unrelate_mem(uint64_t mem_addr){


    if (mem_source.count(mem_addr) && mem_source[mem_addr].second!=-1){
        for(auto el: related_regs[mem_addr]){
            regs_source[el] = mem_source[mem_addr];
        }
    }else{
        for(auto el: related_regs[mem_addr]){
            regs_source[el] = {mem_addr,time};
        }
    }

    related_regs[mem_addr].clear();

}


void Optimizator::delete_dumb(){//deletes intructions, that dont change values in regs/memory

    time = 0;
    std::vector<rw_instruction>n_program;

    

    for(auto& elem : program){


        std::string read = elem.read;
        std::string write = elem.write;


        uint8_t n1 = read.size();

        uint8_t n2 = write.size();

        if (n1 == 0 || n2 == 0) continue;

        if (read == write) continue;


        if (read[0]=='r' && write[0]=='r'){

            uint16_t source = std::stoi(read.substr(1));
            uint16_t dest = std::stoi(write.substr(1));
            uint8_t count = regs_source.count(source);

            if (count==0 || (regs_source[source]==regs_source[dest] && regs_source[source].first!=-1) ) continue;
            if (regs_source.count(dest)) unrelate_reg(dest);


            regs_source[dest] = regs_source[source];
            if (regs_source[source].second==-1)  related_regs[regs_source[source].first].insert(dest);
            n_program.push_back(elem);




        }else if (read[0]=='r'){

            uint16_t source = std::stoi(read.substr(1));
            uint64_t dest = std::stoll(write, nullptr, 16);

            if (regs_source[source]==mem_source[dest]) continue;

            unrelate_mem(dest);



            mem_source[dest] = regs_source[source];

            related_regs[dest].insert(source);

            n_program.push_back(elem);

        }else if (write[0]=='r'){
            
            uint64_t source = std::stoll(read, nullptr, 16);
            uint16_t dest = std::stoi(write.substr(1));

            if (related_regs[source].count(dest)) continue;

            unrelate_reg(dest);


            if (!mem_source.count(source)){
                mem_source[source] = {source,-1};
            }

            regs_source[dest] = mem_source[source];


            related_regs[source].insert(dest);
            related_regs[mem_source[source].first].insert(dest);


            n_program.push_back(elem);


        }else{
            uint64_t source = std::stoll(read,nullptr, 16);
            uint64_t dest = std::stoll(write,nullptr, 16);
            if (mem_source[dest].first==source && mem_source[dest].second==-1) continue;

            unrelate_mem(dest);

            mem_source[dest]={source,-1};

            n_program.push_back(elem);

        }

        time++;
    }
    program = n_program;
}


void Optimizator::delete_overlap(){//deletes intructions, that dont affect to future regs/memory state and rearrange instructions  

    std::unordered_map<uint16_t, uint32_t>last_instr;

    std::vector<rw_instruction>n_program;

    std::unordered_map<uint64_t, bool> used;

    for(int i=0;i<program.size();++i){

        if (program[i].write[0]=='r'){
            uint16_t reg = std::stoi(program[i].write.substr(1));
            last_instr[reg] = i;
        }

        if (program[i].read[0]=='r'){

            uint16_t reg = std::stoi(program[i].read.substr(1));

            if (last_instr.count(reg)){  
                n_program.push_back(program[last_instr[reg]]);
                used[last_instr[reg]]=true;
            }

            if (program[i].write[0]!='r'){
                 n_program.push_back(program[i]);
                 used[i] = true;
            }

        }

    }

    for(auto& el: last_instr) if (!used[el.second]) n_program.push_back(program[el.second]);


    program = n_program;

}

void Optimizator::execute(){

    if (dumb_flag){
        delete_dumb();
    }


    if (overlap_flag){

        delete_overlap();
    }

    uint64_t instruction_count = program.size();

    uint64_t cycle_count = 0;

    std::unordered_map<uint16_t, uint64_t>reg_avail;

    std::unordered_map<uint64_t, uint64_t>mem_avail;

    boost::circular_buffer<std::pair<rw_instruction, uint64_t>> window(4);


    auto instr_ptr= program.begin();


    while (true){

        while(!window.full() && instr_ptr!=program.end()){

            uint64_t source;

            uint64_t dest;

            uint64_t avail_time=0;

            if (instr_ptr->read[0]=='r'){

                source = std::stoi(instr_ptr->read.substr(1));

                if (reg_avail.count(source))avail_time = reg_avail[source];

            } else{

               source = std::stoll(instr_ptr->read, nullptr, 16);
               if (mem_avail.count(source)) avail_time = mem_avail[source];
            }

            uint64_t instr_issue_time = std::max(avail_time, cycle_count+1);

            window.push_back({*instr_ptr,instr_issue_time});

            if (instr_ptr->write[0]=='r'){

                dest = std::stoi(instr_ptr->write.substr(1));

                reg_avail[dest] = instr_issue_time+1;

            } else if (instr_ptr->write.size()){

               dest = std::stoll(instr_ptr->write,nullptr, 16);

                mem_avail[dest] = instr_issue_time+1;
            }

            instr_ptr++;


        }

        cycle_count++;

        for(auto it = window.begin(); it!=window.end() && !window.empty();){
            if (it->second<cycle_count) it = window.erase(it);
            else it++;
        }

        if (window.empty() && instr_ptr == program.end()) break;
    }

    printf("clock sycle count = %ld\ninstruction count = %ld\n", cycle_count, instruction_count);

}