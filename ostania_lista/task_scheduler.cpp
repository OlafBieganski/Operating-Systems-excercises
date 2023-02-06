#include <queue>
#include <iostream>
#include <functional>
#include <sstream>
#include <vector>
#include <array>

using std::cout;
using std::endl;

#define NO_ID -1

struct process {
    int id; // process's id
    int pi; // process's priority
    int ri; // process's full runtime
    uint arrT; // process's arrival time
    std::function<bool(process, process)> compareFunc = nullptr;

    public:
    process() : id(NO_ID) {}
    process(int _id, int _ri, int _pi, uint _arrT) : id(_id), ri(_ri) {}
    bool operator<(const process &p){
        return compareFunc(*this, p);
    }
    void assignCompFunc(std::function<bool(process, process)> _compareFunc){
        compareFunc = _compareFunc;
    }
};

int main(int argc, char** argv){

    if(argc < 2){
        std::cerr << "Too few arguments.\n";
        exit(-1);
    }
    // setting processor nr and p
    int algorithmType = atoi(argv[1]);
    uint procNr = 1, preempTime = 1;
    if(argc == 3) procNr = atoi(argv[2]);
    if(argc == 4) preempTime = atoi(argv[3]);
    // based on argument choose scheduling algorithm
    std::function<bool(process, process)> _compareFunc;
    switch(algorithmType){
        // FCFS
        case 0:
            _compareFunc = [](process a, process b) -> bool {
                if(a.arrT == b.arrT) return a.id < b.id;
                return a.arrT < b.arrT;
            };
            break;
        // SJF
        case 1:
            _compareFunc = [](process a, process b) -> bool {
                if(a.ri == b.ri) return a.id < b.id;
                return a.ri < b.ri;
            };
            break;
        // SRT preemptive
        case 2:
            _compareFunc = [](process a, process b) -> bool {
                if(a.ri == b.ri) return a.id < b.id;
                return a.ri < b.ri;
            };
            break;
        // priority preemptive FCFS
        case 4:
            _compareFunc = [](process a, process b) -> bool {
                if(a.pi != b.pi) return a.pi < b.pi;
                if(a.ri == b.ri) return a.id < b.id;
                return a.ri < b.ri;
            };
            break;
        // priority preemptive SRT
        case 5:
            _compareFunc = [](process a, process b) -> bool {
                if(a.pi != b.pi) return a.pi < b.pi;
                if(a.ri == b.ri) return a.id < b.id;
                return a.ri < b.ri;
            };
            break;
        // priority non-preemptive FCFS
        case 6:
            _compareFunc = [](process a, process b) -> bool {
                if(a.pi != b.pi) return a.pi < b.pi;
                if(a.ri == b.ri) return a.id < b.id;
                return a.ri < b.ri;
            };
            break;
    }
    // all except RR
    if(algorithmType != 3){
        // create a priority queue to store incoming processes
        std::priority_queue<process> procStorage;
        std::vector<process> CPU;
        process emptyProc;
        for(int i = 0; i < procNr; i++){
            CPU.push_back(emptyProc);
        }
        do{
            std::string input;
            uint time;
            int _pi, _ri, _id;
            // read input data about new processes
            std::getline(std::cin, input);
            std::istringstream stream(input);
            stream >> time;
            while(stream >> _id >> _pi >> _ri){
                process newProc(_id, _ri, _pi, time);
                procStorage.push(newProc);
            }
            // apply suitable algorithm
            switch(algorithmType){
                case 0:
                    
            }

        }while( !procStorage.empty() || (std::cin.peek() != EOF) );
    }

}