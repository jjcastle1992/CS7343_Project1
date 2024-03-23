/*********************
Name: James Castle
CS 7343 Project 1
Purpose: Complete example of threading application and process scheduling algorithms
**********************/
#include <iostream> // to read in files
#include <vector> // For 2d vectors
#include <thread> // for threading in fibCalc
#include <fstream> // For file writing/reading
#include <sstream> // for file parsing
#include <algorithm> // for sort()

std::vector<int> fibArray;
std::vector<std::vector<int>> schedAlgos;
int sortColNum = 0;

void fibCalc(){
    // job for child thread that completes before parent continues
    int numbers = 0;
    std::cout << "How many Fibonacci numbers should I provide? (Positive ints only): ";
    std::cin >> numbers;
    int num1 = 0;
    int num2 = 1;
    if (numbers > 0 && numbers < 2) {
        fibArray.push_back(num1);
    }
    else if(numbers >= 2){
        // Wont validate types here
        fibArray.push_back(num1);
        fibArray.push_back(num2);
        for (int i = 0; i < numbers - 1; i++){
            if (i > 0){
                // After first run
                num1 = fibArray[i - 1];
                num2 = fibArray[i];
                int nextNum = num1 + num2;
                fibArray.push_back(nextNum);
            }
        }
    }
    std::cout << "done" << std::endl;
}

void fibPrint(){
    // Job for parent thread to be done synchronously (child finishes first)
    std::cout << "Fibonacci Numbers for input of size: " << fibArray.size() << std::endl;
    for (int i = 0; i < fibArray.size(); i++){
        std::cout << fibArray[i] << ", ";
    }
    // Clear FibArray after printing so new cycle can be done;
    std::cout << "\nPrint completed" << std::endl;
    fibArray.clear();
}

void readFromFile(const std::string &filename) {
    std::ifstream infile;
    infile.open(filename);
    if(!infile.is_open()){ // File could not be opened
        std::cerr << "ERROR: File could not be opened" << std::endl;
        std::exit(1);
    }
    // Read the file line by line
    std::string line;
    while(std::getline(infile, line)) {
        // define a stringstreeam to parse each line
        std::istringstream inputString(line);

        // Parse each line into a field
        std::string field;
        int colNum = 0;
        int taskName;
        int priority;
        int cpuBurst;


        while (std::getline(inputString, field, ',')) { // parse string into fields
            if((colNum == 1) || (colNum == 2)){
                int item = std::stoi(field);
                if (colNum == 1) {priority = item;}
                else if (colNum == 2) {cpuBurst = item;}
            }
            else if (colNum == 0) {
                field.erase(0, 1);  //Remove leading T from string, then convert to number;
                int item = std::stoi(field);
                taskName = item;
            }
            else {
                std::cout << "ERROR: Illegal column [" << colNum << "] - (not 0, 1, or 2)." << std::endl;
            }
            colNum++;
        }
        // Add items to schedAlgos global vector (bad practice, simple solution)
        std::vector<int> tempVector;
        tempVector.push_back(taskName);
        tempVector.push_back(priority);
        tempVector.push_back(cpuBurst);
        schedAlgos.push_back(tempVector);
    }
}

void fcfs (){
    // bring in the process array and make a copy
    std::vector<std::vector<int>> fcfsScheduler;
    fcfsScheduler = schedAlgos;
    std::vector<std::vector<int>>::iterator it; // Prep iterator for FIFO queue style ops
    int currentTime = -1;

    std::cout << "\n****************FCFS Algorithm Demonstration****************" << std::endl;
    //Iterate through all processes in a FCFS fashion
    while (it != fcfsScheduler.end()) {
        it = fcfsScheduler.begin();
        int burstStart = fcfsScheduler[0][2]; // Set Burst time of current process
        int burstLeft = burstStart;

        //Print Queue
        std::cout << "Processes in Queue: ";
        for (int j = 0; j < fcfsScheduler.size(); j++){
            std::cout << 'T' << fcfsScheduler[j][0];
            if(j < (fcfsScheduler.size()-1)) {
                std::cout << " ,";
            }
        }
        //Print Process to be scheduled
        std::cout << "\nProcess to be scheduled: T" << fcfsScheduler[0][0];
        while(burstLeft > 0){
            burstLeft--;
            fcfsScheduler[0][2] = burstLeft;
            currentTime++;
        }
        // Give process stats on finish (TRT, Wait, Finish Time (1 + last scheduled T)
        int finishTime = 1 + currentTime;
        int trt = finishTime - 0; // since all tasks arrive at T = 0
        int waitTime  = trt - burstStart;
        std::cout << "\nProcess Scheduling Metrics for Process T" << fcfsScheduler[0][0] << ": " << std::endl;
        std::cout << "\tFinish Time: T = " << finishTime << " seconds" << std::endl;
        std::cout << "\tWait Time: " << waitTime << " seconds" << std::endl;
        std::cout << "\tTurnaround Time: " << trt << " seconds" << std::endl;
        fcfsScheduler.erase(it); // Erase first thing in vector (i.e. Pop the FIFO queue)
    }
    std::cout << "FCFS done" << std::endl;
}

bool sortcol(std::vector<int> &v1, const std::vector<int> &v2){
    // From https://www.geeksforgeeks.org/sorting-2d-vector-in-c-set-1-by-row-and-column/
    return v1[sortColNum] < v2[sortColNum];
}

bool sortcolAsc(std::vector<int> &v1, const std::vector<int> &v2){
    // From https://www.geeksforgeeks.org/sorting-2d-vector-in-c-set-1-by-row-and-column/
    return v1[sortColNum] > v2[sortColNum];
}

int findIndexByID(const std::vector<std::vector<int>> &vect, int targetID) {
    for (int i = 0; i < vect.size(); ++i) {
        if (vect[i][0] == targetID) {
            return i; // Return the index if ID is found in column 0
        }
    }
    return -1; // Return -1 if ID is not found
}


void sjf(){
    // bring in the process array and make a copy
    std::vector<std::vector<int>> sjfScheduler;
    sjfScheduler = schedAlgos;
    std::vector<std::vector<int>>::iterator it; // Prep iterator for FIFO queue style ops
    int currentTime = -1;

    std::cout << "\n****************SJF Algorithm Demonstration****************" << std::endl;

    //Print Queue (BEFORE SORT)
    std::cout << "Processes in Queue before Sorting: ";
    for (int j = 0; j < sjfScheduler.size(); j++){
        std::cout << 'T' << sjfScheduler[j][0];
        if(j < (sjfScheduler.size()-1)) {
            std::cout << " ,";
        }
    }
    std::cout << std::endl;
    // Sort Queue based on burst time
    sortColNum = 2; // Column to sort on (CPU Burst)
    sort(sjfScheduler.begin(), sjfScheduler.end(), sortcol);

    //Iterate through all processes in a SJF fashion
    while (it != sjfScheduler.end()) {
        it = sjfScheduler.begin();
        int burstStart = sjfScheduler[0][2]; // Set Burst time of current process
        int burstLeft = burstStart;

        //Print Queue
        std::cout << "Processes in Queue: ";
        for (int j = 0; j < sjfScheduler.size(); j++){
            std::cout << 'T' << sjfScheduler[j][0];
            if(j < (sjfScheduler.size()-1)) {
                std::cout << " ,";
            }
        }
        //Print Process to be scheduled
        std::cout << "\nProcess to be scheduled: T" << sjfScheduler[0][0];
        while(burstLeft > 0){
            burstLeft--;
            sjfScheduler[0][2] = burstLeft;
            currentTime++;
        }
        // Give process stats on finish (TRT, Wait, Finish Time (1 + last scheduled T)
        int finishTime = 1 + currentTime;
        int trt = finishTime - 0; // since all tasks arrive at T = 0
        int waitTime  = trt - burstStart;
        std::cout << "\nProcess Scheduling Metrics for Process T" << sjfScheduler[0][0] << ": " << std::endl;
        std::cout << "\tFinish Time: T = " << finishTime << " seconds" << std::endl;
        std::cout << "\tWait Time: " << waitTime << " seconds" << std::endl;
        std::cout << "\tTurnaround Time: " << trt << " seconds" << std::endl;
        sjfScheduler.erase(it); // Erase first thing in vector (i.e. Pop the FIFO queue)
    }
    std::cout << "SJF done" << std::endl;
}

void prioritySched(){
    // bring in the process array and make a copy
    std::vector<std::vector<int>> priorityScheduler;
    priorityScheduler = schedAlgos;
    std::vector<std::vector<int>>::iterator it; // Prep iterator for FIFO queue style ops
    int currentTime = -1;

    std::cout << "\n****************Basic Priority Algorithm Demonstration****************" << std::endl;

    //Print Queue (BEFORE SORT)
    std::cout << "Processes in Queue before Sorting: ";
    for (int j = 0; j < priorityScheduler.size(); j++){
        std::cout << 'T' << priorityScheduler[j][0];
        if(j < (priorityScheduler.size()-1)) {
            std::cout << " ,";
        }
    }
    std::cout << std::endl;
    // Sort Queue based on PRIORITY
    sortColNum = 1; // Column to sort on (PRIORITY)
    sort(priorityScheduler.begin(), priorityScheduler.end(), sortcolAsc);

    //Iterate through all processes in a Priority fashion
    while (it != priorityScheduler.end()) {
        it = priorityScheduler.begin();
        int burstStart = priorityScheduler[0][2]; // Set Burst time of current process
        int burstLeft = burstStart;

        //Print Queue
        std::cout << "Processes in Queue: ";
        for (int j = 0; j < priorityScheduler.size(); j++){
            std::cout << 'T' << priorityScheduler[j][0];
            if(j < (priorityScheduler.size()-1)) {
                std::cout << " ,";
            }
        }
        //Print Process to be scheduled
        std::cout << "\nProcess to be scheduled: T" << priorityScheduler[0][0];
        while(burstLeft > 0){
            burstLeft--;
            priorityScheduler[0][2] = burstLeft;
            currentTime++;
        }
        // Give process stats on finish (TRT, Wait, Finish Time (1 + last scheduled T)
        int finishTime = 1 + currentTime;
        int trt = finishTime - 0; // since all tasks arrive at T = 0
        int waitTime  = trt - burstStart;
        std::cout << "\nProcess Scheduling Metrics for Process T" << priorityScheduler[0][0] << ": " << std::endl;
        std::cout << "\tFinish Time: T = " << finishTime << " seconds" << std::endl;
        std::cout << "\tWait Time: " << waitTime << " seconds" << std::endl;
        std::cout << "\tTurnaround Time: " << trt << " seconds" << std::endl;
        priorityScheduler.erase(it); // Erase first thing in vector (i.e. Pop the FIFO queue)
    }
    std::cout << "Basic Priority done" << std::endl;
}

void basic_rr() {
    int timeQuantum = 10;
    // bring in the process array and make a copy
    std::vector<std::vector<int>> rrScheduler;
    rrScheduler = schedAlgos;
    std::vector<std::vector<int>>::iterator it; // Prep iterator for FIFO queue style ops
    int currentTime = -1;

    std::cout << "\n****************Basic Round Robin Algorithm Demonstration****************" << std::endl;
    //Iterate through all processes in a RR fashion
    while (it != rrScheduler.end()) {
        it = rrScheduler.begin();
        int burstStart = rrScheduler[0][2]; // Set Burst time of current process
        int burstLeft = burstStart;

        //Print Queue
        std::cout << "\nProcesses in Queue: ";
        for (int j = 0; j < rrScheduler.size(); j++){
            std::cout << 'T' << rrScheduler[j][0];
            if(j < (rrScheduler.size()-1)) {
                std::cout << " ,";
            }
        }

        // Start RR loop
        int quantumLeft = timeQuantum;
        //Print Process to be scheduled
        std::cout << "\nProcess to be scheduled: T" << rrScheduler[0][0] << "\nBurst Remaining: " << rrScheduler[0][2];
        while((burstLeft > 0) && (quantumLeft > 0)){
            burstLeft--;
            rrScheduler[0][2] = burstLeft;
            currentTime++;
            quantumLeft--;
        }
        if ((burstLeft > 0) && (quantumLeft == 0)) {
            // Update the time left
            rrScheduler[0][2] = burstLeft;
            // Put the current item at the end of the list
            rrScheduler.push_back(rrScheduler[0]);
            // Pop it off the front
            it = rrScheduler.begin();
            rrScheduler.erase(it); // Erase first thing in vector (i.e. Pop the FIFO queue)
            std::cout << "\nQuantum Ended. Burst remaining: " << burstLeft << "\nNew Process Scheduled." <<
            "\nCurrent Time: "<< currentTime << std::endl;
        }

        // Swap processes as needed

        if (burstLeft == 0){
            // Give process stats on finish (TRT, Wait, Finish Time (1 + last scheduled T)
            int finishTime = 1 + currentTime;
            int trt = finishTime - 0; // since all tasks arrive at T = 0
            //Find burstStart of original process
            const int searchId = rrScheduler[0][0]; // Process ID
            int originalId = findIndexByID(schedAlgos, searchId); // returns ID of process in schedAlgos
            burstStart = schedAlgos[originalId][2];  // Sets burst start for process to original value
            int waitTime  = trt - burstStart;
            std::cout << "\nProcess Scheduling Metrics for Process T" << rrScheduler[0][0] << ": " << std::endl;
            std::cout << "\tFinish Time: T = " << finishTime << " seconds" << std::endl;
            std::cout << "\tWait Time: " << waitTime << " seconds" << std::endl;
            std::cout << "\tTurnaround Time: " << trt << " seconds" << std::endl;
            rrScheduler.erase(it); // Erase first thing in vector (i.e. Pop the FIFO queue)
        }
    }
    std::cout << "Basic Round Robin done" << std::endl;
}

void priorityRoundRobin() {
    int timeQuantum = 10;
    // bring in the process array and make a copy
    std::vector<std::vector<int>> priorityRrScheduler;
    priorityRrScheduler = schedAlgos;
    std::vector<std::vector<int>>::iterator it; // Prep iterator for FIFO queue style ops
    int currentTime = -1;

    std::cout << "\n****************Priority Round Robin Algorithm Demonstration****************" << std::endl;
    //Print Queue (BEFORE SORT)
    std::cout << "Processes in Queue before Sorting: ";
    for (int j = 0; j < priorityRrScheduler.size(); j++){
        std::cout << 'T' << priorityRrScheduler[j][0];
        if(j < (priorityRrScheduler.size()-1)) {
            std::cout << " ,";
        }
    }
    std::cout << std::endl;

    // Sort Queue based on PRIORITY
    sortColNum = 1; // Column to sort on (PRIORITY)
    sort(priorityRrScheduler.begin(), priorityRrScheduler.end(), sortcolAsc);

    // Use priority scheduling UNLESS priority is the same, then RR
    while (it != priorityRrScheduler.end()) {
        it = priorityRrScheduler.begin();
        int burstStart = priorityRrScheduler[0][2]; // Set Burst time of current process
        int burstLeft = burstStart;

        //Print Queue
        std::cout << "\nProcesses in Queue: ";
        for (int j = 0; j < priorityRrScheduler.size(); j++){
            std::cout << 'T' << priorityRrScheduler[j][0];
            if(j < (priorityRrScheduler.size()-1)) {
                std::cout << " ,";
            }
        }
        // IF QUEUE IS BIGGER THAN ONE, COMPARE PRIORITIES
        if(priorityRrScheduler.size() > 1) {
            // If size > 1, check to see if front 2 elements in queue are the same priority
            if(priorityRrScheduler[0][1] == priorityRrScheduler[1][1]){
                //use round robin until queue size is 1 or queue[0] and queue[1] have different priorities
                // Start RR loop
                int quantumLeft = timeQuantum;
                //Print Process to be scheduled
                std::cout << "\nProcess to be scheduled: T" << priorityRrScheduler[0][0] << " (Priority: " <<
                priorityRrScheduler[0][1] <<")\nBurst Remaining: " << priorityRrScheduler[0][2];
                while((burstLeft > 0) && (quantumLeft > 0)){
                    burstLeft--;
                    priorityRrScheduler[0][2] = burstLeft;
                    currentTime++;
                    quantumLeft--;
                }
                if ((burstLeft > 0) && (quantumLeft == 0)) {
                    // Update the time left
                    priorityRrScheduler[0][2] = burstLeft;
                    // Swap items at position 1 and 0
                    // Idea from https://stackoverflow.com/questions/6224830/c-trying-to-swap-values-in-a-vector
                    std::iter_swap(priorityRrScheduler.begin(), priorityRrScheduler.begin() + 1);

                    std::cout << "\nQuantum Ended. Burst remaining: " << burstLeft << "\nNew Process Scheduled." <<
                              "\nCurrent Time: "<< currentTime << std::endl;
                }

                if (burstLeft == 0){
                    // Give process stats on finish (TRT, Wait, Finish Time (1 + last scheduled T)
                    int finishTime = 1 + currentTime;
                    int trt = finishTime - 0; // since all tasks arrive at T = 0
                    //Find burstStart of original process
                    const int searchId = priorityRrScheduler[0][0]; // Process ID
                    int originalId = findIndexByID(schedAlgos, searchId); // returns ID of process in schedAlgos
                    burstStart = schedAlgos[originalId][2];  // Sets burst start for process to original value
                    int waitTime  = trt - burstStart;
                    std::cout << "\nProcess Scheduling Metrics for Process T" << priorityRrScheduler[0][0] << ": " <<
                    std::endl;
                    std::cout << "\tFinish Time: T = " << finishTime << " seconds" << std::endl;
                    std::cout << "\tWait Time: " << waitTime << " seconds" << std::endl;
                    std::cout << "\tTurnaround Time: " << trt << " seconds" << std::endl;
                    priorityRrScheduler.erase(it); // Erase first thing in vector (i.e. Pop the FIFO queue)
                }
            }

            // THEY'RE NOT THE SAME, SO USE REG PRIORITY
            else{
                // if not, proceed as normal (standard priority)
                //Print Process to be scheduled
                std::cout << "\nProcess to be scheduled: T" << priorityRrScheduler[0][0] << " (Priority: " <<
                priorityRrScheduler[0][1] <<")\nBurst Remaining: " << priorityRrScheduler[0][2];

                while(burstLeft > 0){
                    burstLeft--;
                    priorityRrScheduler[0][2] = burstLeft;
                    currentTime++;
                }
                // Give process stats on finish (TRT, Wait, Finish Time (1 + last scheduled T)
                int finishTime = 1 + currentTime;
                int trt = finishTime - 0; // since all tasks arrive at T = 0
                int waitTime  = trt - burstStart;
                std::cout << "\nProcess Scheduling Metrics for Process T" << priorityRrScheduler[0][0] << ": " << std::endl;
                std::cout << "\tFinish Time: T = " << finishTime << " seconds" << std::endl;
                std::cout << "\tWait Time: " << waitTime << " seconds" << std::endl;
                std::cout << "\tTurnaround Time: " << trt << " seconds" << std::endl;
                priorityRrScheduler.erase(it); // Erase first thing in vector (i.e. Pop the FIFO queue)
            }
        }

        // There is one element left, so use regular processing
        else{
            // if not, proceed as normal (standard priority)
            //Print Process to be scheduled
            std::cout << "\nProcess to be scheduled: T" << priorityRrScheduler[0][0] << " (Priority: " <<
            priorityRrScheduler[0][1] <<")\nBurst Remaining: " << priorityRrScheduler[0][2];
            while(burstLeft > 0){
                burstLeft--;
                priorityRrScheduler[0][2] = burstLeft;
                currentTime++;
            }
            // Give process stats on finish (TRT, Wait, Finish Time (1 + last scheduled T)
            int finishTime = 1 + currentTime;
            int trt = finishTime - 0; // since all tasks arrive at T = 0
            int waitTime  = trt - burstStart;
            std::cout << "\nProcess Scheduling Metrics for Process T" << priorityRrScheduler[0][0] << ": " << std::endl;
            std::cout << "\tFinish Time: T = " << finishTime << " seconds" << std::endl;
            std::cout << "\tWait Time: " << waitTime << " seconds" << std::endl;
            std::cout << "\tTurnaround Time: " << trt << " seconds" << std::endl;
            priorityRrScheduler.erase(it); // Erase first thing in vector (i.e. Pop the FIFO queue)
        }
    }
    std::cout << "Priority Round Robin done" << std::endl;
}

int main(){
    // ********** Q1 *****************
    // Used this as guidance:
    // https://www.geeksforgeeks.org/multithreading-in-cpp/

    // parent thread is main
    std::thread t1(fibCalc); // child thread that calcs the numbers
    t1.join();
    fibPrint();

    // ********** Q2 *****************
    //Provide filename path
    std::string filename = R"(C:\Users\Wolf\Dropbox\Grad School\Spring 2024\CS 7343 Operating Systems\Project 1\schedule.txt)";
    //Read the file
    readFromFile(filename);
    std::cout << "done" << std::endl;

    // Write out rep that shows timeline, process loaded, queue, pre-emption, and process completion

    // Illustrate FCFS
//    fcfs();
    // Illustrate SJF
//    sjf();
    // Illustrate non-Pre-emptive Priority Scheduling (all arive at 0) where highest priority is largest int
//    prioritySched();
    // Illustrate RR with a time quantum of 10;
//    basic_rr();
    // Illustrate Priority with Round Robin
    priorityRoundRobin();

    return 0;
}