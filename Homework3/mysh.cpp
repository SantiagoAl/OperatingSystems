// Alex Santiago
// UCF ID: 3216767
// COP 4600 Spring 2021

// This is my final solution for Homework assignment 3

// NOTE: You should compile this code using at least C++11. For example
// g++ -std=c++11 mysh.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

// Our current directory
std::string currentDir = "";
// This map will hold the main commands and their index to assist in execution
std::unordered_map<std::string, int> mainCommands =
                    { {"movetodir", 1}, {"whereami", 2},
                      {"history", 3}, {"byebye", 4}, {"replay", 5},
                      {"start", 6}, {"background", 7}, {"dalek", 8},
                      {"repeat", 9}
                    };
// This will hold our recent command history. 'currentHistorySize' will help
// later on so that we do not overwrite our history in 'history.txt' everytime
// the program terminates
std::vector<std::string> history;
int currentHistorySize {0};

// This function will load up recent history from an external file ("history.txt")
void getHistory()
{
    // Create an 'ifstream' object and attempt to open the file
    std::ifstream inFile;
    inFile.open("history.txt");

    // If the files does not exist, then return immediately
    if (!inFile)
        return;

    std::string line{};

    // While there is still history to read, add to our recently typed commands
    while (std::getline(inFile, line))
    {
        history.emplace_back(line);
    }

    // Remember to close the file because you know... Memory Leaks!!
    inFile.close();
}

// This function will write our recent history to an external file ("history.txt")
void writeHistory()
{
    // Create an 'ofstream' object and attempt to create an output file
    std::ofstream outFile {"history.txt", std::ios::app};

    // If an error was encountered while creating the output file, emit an
    // error message and return immediately
    if (!outFile)
    {
        std::cout << "Error: Encountered a problem creating external file for"
                 << " history" << std::endl;
        return;
    }

    // Go through the 'history' of our recently typed commands and write them
    // to an external file. Starting from 'currentHistorySize' will help to not
    // overwrite history in 'history.txt' when the program terminates
    for (int i = currentHistorySize; i < history.size(); i++)
    {
        outFile << history[i] << std::endl;
    }

    // Remember to close the file because you know... Memory Leaks!!
    outFile.close();
}

// This function will take an input string from the user and tokenize the
// commands
std::vector<std::string> tokenizeInput(std::string& input, char delimiter)
{
    // Vector of strings to hold the tokens
    std::vector<std::string> tokens;
    std::string intermediate;

    // stringstream class check1
    std::stringstream check1(input);

    // Tokenize the string with a space (' ')
    while (std::getline(check1, intermediate, delimiter))
        tokens.emplace_back(intermediate);

    return tokens;
}

/*************** EXTRA CREDIT 1. THIS IS THE REPEAT COMMAND ***************/
// This function will repeat a command 'n' times as specified by the user,
// printing out the PIDs of the created processes as well and return without
// waiting for the processes to terminate
void repeat(std::vector<std::string> tokens)
{
    std::vector<pid_t> PIDs;
    int x;

    if (tokens.size() <= 2)
    {
        std::cout << "You did not provide the necessary amount of arguments for"
                    << " 'repeat'" << std::endl;
        return;
    }

    for (int i = 0; i < std::stoi(tokens[1]); i++)
    {
        pid_t pid = fork();
        PIDs.emplace_back(pid);

        // If the pid comes back less than zero, then we will kill it
        if (pid < 0)
        {
            kill(pid, SIGKILL);
            std::cout << "Error: Process has not been run" << std::endl;
        }
        else if (pid == 0)
        {
            char* argvList[tokens.size() - 1];

            // This will change the string into 'char *' arrays, so that we can use
            // the 'execv' system call
            for (int j = 2, x = 0; j < tokens.size(); j++, x++)
                argvList[x] = (char *)tokens[j].c_str();

            argvList[x] = NULL;

            // Here, command will be interpreted as the full path to the program
            // to execute
            if (execv(argvList[0], argvList) < 0)
            {
                perror("Fork failed");
                exit(1);
            }
        }
    }

    // Go ahead and print out all of the PIDs of the newly created processes
    std::cout << "PIDs: ";

    for (int i = 0; i < PIDs.size(); i++)
        if (i != PIDs.size() - 1)
            std::cout << PIDs[i] << ", ";
        else
            std::cout << PIDs[i] << std::endl;

    std::cout << std::endl;
}

// This is almost the same function as 'startProgram', but the parent will not
// wait for the child process. We will also return the PID of the process
void runInBackground(std::vector<std::string>& tokens)
{
    pid_t pid;

    if (tokens.size() <= 1)
    {
        std::cout << "You did not provide the necessary amount of "
                    << "arguments for 'background'" << std::endl;
        return;
    }

    // Fork the process
    pid = fork();

    // If the pid comes back less than zero, then we will kill it
    if (pid < 0)
    {
        kill(pid, SIGKILL);
        std::cout << "Error: Process has not been run" << std::endl;
    }
    // This indicates success and we will execute the process
    else if (pid == 0)
    {
        std::string path = tokens[1];
        char* argvList[tokens.size()];
        int i;

        // This will change the string into 'char *' arrays, so that we can use
        // the 'execvp' system call
        for (i = 1; i < tokens.size(); i++)
            argvList[i - 1] = (char *)tokens[i].c_str();

        argvList[i - 1] = NULL;

        // If our program starts with '/' or '.' then we will use 'execv', if
        // not we will use 'execvp'
        if (path[0] == '/' || path[0] == '.')
        {
            if (execv(argvList[0], argvList) < 0)
            {
                perror("Fork failed");
                exit(1);
            }
        }
        else
        {
            if (execvp(argvList[0], argvList) < 0)
            {
                perror("Fork failed");
                exit(1);
            }
        }
    }

    std::cout << "PID: " << pid << std::endl;
}

// This function will run a program along with any specfied parameters, and wait
// until the program terminates
void startProgram(std::vector<std::string>& tokens)
{
    pid_t pid;
    int statusPtr;

    if (tokens.size() <= 1)
    {
        std::cout << "You did not provide the necessary amount of "
                    << "arguments for 'start'" << std::endl;
        return;
    }

    // For the process
    pid = fork();

    // If the pid comes back less than zero, then we will kill it
    if (pid < 0)
    {
        kill(pid, SIGKILL);
        std::cout << "Error: Process has not been run" << std::endl;
    }
    // This indicates success and we will execute the process
    else if (pid == 0)
    {
        std::string path = tokens[1];
        char* argvList[tokens.size()];
        int i;

        // This will change the string into 'char *' arrays, so that we can use
        // the 'execv' and 'execvp' system calls
        for (i = 1; i < tokens.size(); i++)
            argvList[i - 1] = (char *)tokens[i].c_str();

        argvList[i - 1] = NULL;

        // If our program starts with '/' or '.' then we will use 'execv', if
        // not we will use 'execvp'
        if (path[0] == '/' || path[0] == '.')
        {
            if (execv(argvList[0], argvList) < 0)
            {
                perror("Fork failed");
                exit(1);
            }
        }
        else
        {
            if (execvp(argvList[0], argvList) < 0)
            {
                perror("Fork failed");
                exit(1);
            }
        }
    }

    // Here, the parent will wait for the child process to terminate
    if (pid > 0)
    {
        waitpid(pid, &statusPtr, WUNTRACED);
    }
}

// This funciton will terminate a specific process with it's PID sent as input
void dalek(std::vector<std::string>& tokens)
{
    // Here, we just want to make sure that a PID was actually sent to the
    // function
    if (tokens.size() <= 1)
    {
        std::cout << "An error has occured. Re-check your input command"
                << std::endl;
        return;
    }

    // If the 'kill' was sucessful, print a success message. If not, print an
    // error message
    if (!(kill(std::stoi(tokens[1]), SIGQUIT)))
        std::cout << "PID: " << tokens[1] << " terminated" << std::endl;
    else
        std::cout << "Error: Could not terminate process: " << tokens[1] << std::endl;
}

// This function will take a path to a directory as a parameter, check if that
// directory exists and if so change our current directory variable ('currentDir')
// to the new directory
void changeDirectory(std::string& dir, std::string& input)
{
    // Place the input command in our history
    history.emplace_back(input);

    // A system call to determine if the directory exists
    struct stat buffer;

    // If this returns zero, we will separate the path at the '/' characters to
    // get the final destination, and that is our current directory
    if (stat(dir.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode))
    {
        std::vector<std::string> tokens = tokenizeInput(dir, '/');
        currentDir = tokens[tokens.size() - 1];
    }
    else
        std::cout << "No such file or directory" << std::endl;
}

// This will print the history of the users commands
void printHistory(std::vector<std::string>& tokens, std::string& input)
{
    // If the size of tokens is greater than 1, then there was more input after
    // 'history'. Check if it is '[-c]', if not, invalid command
    if (tokens.size() > 1)
    {
        if (tokens[tokens.size() - 1] == "-c" || tokens[tokens.size() - 1] == "[-c]")
        {
            history.clear();
            return;
        }
        else
        {
            std::cout << "No such command exists" << std::endl;
            return;
        }
    }

    history.emplace_back(input);

    //Making it here means that we should print out the contents of the history
    for (int i = history.size() - 1, j = 0; i >= 0; i--, j++)
        std::cout << j << ": " << history[i] << std::endl;
}

int main(void)
{
    // Variable for the input command, and boolean variables to help with
    // running the loop
    std::string input;
    bool breakLoop = true;
    bool inReplay = false;

    // Now that the shell has started, we will load history from the external
    // file
    getHistory();
    currentHistorySize = history.size();

    // This is the main loop for the shell
    while (breakLoop)
    {
        // A vector that will hold the tokenized input
        std::vector<std::string> tokens;

        // If we are not in replay then we want to get the next input command.
        // If not, then we want to execute on of the previous commands specified
        // by the user
        if (!inReplay)
        {
            std::cout << "# ";
            std::getline(std::cin, input);
        }
        else
            inReplay = false;

        // Here we want to tokenize the input and search for it in our
        // mainCommands
        tokens = tokenizeInput(input, ' ');
        auto command = mainCommands.find(tokens[0]);

        // If the command does not exist, emit an error message, and move on
        // to the next command
        if (command == mainCommands.end())
        {
            std::cout << "Command not recognized" << std::endl;
            continue;
        }

        // Find which command to execute
        switch (command->second) {
            case 1: // movetodir
                changeDirectory(tokens[1], input);
                break;
            case 2: // whereami
                history.emplace_back(input);
                std::cout << "Your current directory is: " << currentDir
                            << std::endl;
                break;
            case 3: // history
                printHistory(tokens, input);
                break;
            case 4: // byebye
                history.emplace_back(input);
                writeHistory();
                breakLoop = false;
                break;
            case 5: // Replay
                history.emplace_back(input);
                input = history[std::stoi(tokens[1])];
                inReplay = true;
                break;
            case 6: // Start
                history.emplace_back(input);
                startProgram(tokens);
                break;
            case 7: // background
                history.emplace_back(input);
                runInBackground(tokens);
                break;
            case 8: // dalek
                history.emplace_back(input);
                dalek(tokens);
                break;
            case 9: // repeat
                history.emplace_back(input);
                repeat(tokens);
                break;
            default: // There was an error
                break;
        }
    }

    return 0;
}
