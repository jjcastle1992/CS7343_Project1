/*********************
Name: James Castle
CS 7343 Project 1
Purpose: Complete example of threading application and process scheduling algorithms
**********************/
#include <iostream>
#include <vector>
#include <thread>
#include <fstream> // For file writing/reading
#include <sstream> // for file parsing

std::vector<int> fibArray;
std::vector<std::vector<int>> schedAlgos;

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
    // Illustrate SJF
    // Illustrate non-Pre-emptive Priority Scheduling (all arive at 0) where highest priority is largest int
    // Illustrate RR with a time quantum of 10;

    return 0;
}