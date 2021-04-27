// Alex Santiago
// UCF ID: 3216767
// COP 4600 Spring 2021

// This is my final solution for Homework assignment 5
// You can compile this file normally using the command: g++ mysh.cpp

// NOTE: If you want to execute a program like HelloWorld.cpp, you should first
// create its executable file. So, for example:
// g++ HelloWorld.cpp -o HelloWorld

#include <iostream>
#include <limits.h>
#include <fstream>
#include <string>
#include <sstream>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <map>
#include <vector>

// Our current directory
std::string currentDir = "";
// This map will hold the main commands and their index to assist in execution
std::map<std::string, int> mainCommands;

// This will hold our recent command history. 'currentHistorySize' will help
// later on so that we do not overwrite our history in 'history.txt' everytime
// the program terminates
std::vector<std::string> history;
int currentHistorySize = 0;

// This function will insert the commands of our shell into mainCommands
// so that we can enable quick lookup
void insertMainCommands(void)
{
    mainCommands["movetodir"] = 1;
    mainCommands["whereami"] = 2;
    mainCommands["history"] = 3;
    mainCommands["byebye"] = 4;
    mainCommands["replay"] = 5;
    mainCommands["start"] = 6;
    mainCommands["background"] = 7;
    mainCommands["dalek"] = 8;
    mainCommands["repeat"] = 9;
    mainCommands["dwelt"] = 10;
    mainCommands["maik"] = 11;
    mainCommands["coppy"] = 12;
}

// A function that will set our current working directory
std::string getCurrentWorkingDir(void)
{
    // Create a temporary char array that will hold the path of our current
    // working directory on start up
    char buffer[PATH_MAX];
    getcwd(buffer, PATH_MAX);

    // Create a string from the char array returned by 'getcwd'
    // and return the result
    std::string currentWorkingDir(buffer);
    return currentWorkingDir;
}


// This function will load up recent history from an external file ("history.txt")
void getHistory(void)
{
    // Create an 'ifstream' object and attempt to open the file
    std::ifstream inFile;
    inFile.open("history.txt");

    // If the files does not exist, then return immediately
    if (!inFile)
        return;

    std::string line = "";

    // While there is still history to read, add to our recently typed commands
    while (std::getline(inFile, line))
    {
        history.push_back(line);
    }

    // Remember to close the file because you know... Memory Leaks!!
    inFile.close();
}

// This function will write our recent history to an external file ("history.txt")
void writeHistory(void)
{
    // Create an 'ofstream' object and attempt to create an output file
    std::ofstream outFile ("history.txt", std::ios::app);

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
        tokens.push_back(intermediate);

    return tokens;
}

// This function will parse 'tokens' into a 'char *' array so that it can
// be used in the 'exec' family of system calls
void parse(std::vector<std::string>& tokens, char** argvList)
{
    int i;

    // Go through the input that has been tokenized and we will insert them
    // into our array of c-style strings
    for (i = 1; i < tokens.size(); i++)
            argvList[i - 1] = (char *)tokens[i].c_str();

    // The argument list for the 'exec' family of functions hs to be
    // NULL terminated
    argvList[i - 1] = NULL;
}

// This function will determine if the parameter is a directory or a file.
// It is used by both the 'changeDirectory' function to implement the 'movetodir'
// command and the 'checkDwelt' function to implement the 'dwelt' function
int directoryOrFile(std::string& searchFor)
{
    struct stat buffer;

    // If the parameter is a directory, return zero
    if (stat(searchFor.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode))
        return 0;

    // If the parameter is a file, return 1
    if (stat(searchFor.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode))
        return 1;

    return -1;
}

// This function will takes a string as a parameter. It will decide if the the
// input should be treated as an absolute or realtive path. Then it will use
// 'directoryOrFile' to check for the validity of the path
int checkValidity(std::string& searchFor)
{
    // The 'searchFor' parameter will be treated as an absolute path
    if (searchFor[0] == '/')
    {
        // The parameter is a directory
        if (directoryOrFile(searchFor) == 0)
            return 0;
        // The parameter is a file
        else if (directoryOrFile(searchFor) == 1)
            return 1;
        // The parameter is neither a directory or a file
        else
            return -1;
    }
    // The 'searchFor' parameter will be treated as a relative path
    else
    {
        std::string temp = currentDir + '/' + searchFor;

        // The parameter is a directory
        if (directoryOrFile(temp) == 0)
            return 0;
        // The parameter is a file
        else if (directoryOrFile(temp) == 1)
            return 1;
        // The parameter is neither a directory or a file
        else
            return -1;
    }
}

// This function will check if a file or directory exists
void checkDwelt(std::string& searchFor)
{
    // The parameter is a directory
    if (checkValidity(searchFor) == 0)
        std::cout << "Abode is." << std::endl;
    // The parameter is a file
    else if (checkValidity(searchFor) == 1)
        std::cout << "Dwelt Indeed." << std::endl;
    // The parameter is neither a directory or a file
    else
        std::cout << "Dwelt not." << std::endl;
}

// This function will create an emtpy file and write the word 'Draft' into it
// If the file already exists it will print an error
void createFile(std::string& file)
{
    if (checkValidity(file) == 1)
    {
        std::cout << "Error: The file \"" << file << "\" already exists" << std::endl;
        return;
    }

    // If the file does not start with a '/', then we will treat it as a relative path
    // to our current directory. Otherwise, it is an absolute path
    std::string destination;
    if (file[0] != '/')
        destination = currentDir + '/' + file;
    else
        destination = file;

    // Create an 'ofstream' object, so that we can attemp to open the file
    // If the file does not exist this will create it
    std::ofstream outFile (destination, std::ios::app);

    // If an error was encountered while either creating the 'ofstream' object or
    // or the output file, then emit an error message and return immediately
    if (!outFile)
    {
        std::cout << "Error: Encountered a problem creating \"" << file << " \"" << std::endl;
        return;
    }

    // Now, write 'Draft' to the newly created file
    outFile << "Draft" << std::endl;

    // Remember to close the file because you know... Memory Leaks!!
    outFile.close();
}

// This function will copy the contents of 'sourceFile' to 'destinationFile'
void copyFile(std::string& sourceFile, std::string& destinationFile)
{
    // First, we will check the validity of the source and destination files
    int isSourceValid = checkValidity(sourceFile);
    int isDestinationValid = checkValidity(destinationFile);

    // If the source file is either or a directory or not a valid file,
    // then we print an error message and return immediately
    if (isSourceValid != 1)
    {
        std::cout << "Error: Source file is either a directory or not a valid file" << std::endl;
        return;
    }

    // If the destination file is a directory or is a file that already exists,
    // then we print an error message and return immediately
    if (isDestinationValid != -1)
    {
        std::cout << "Error: Destination file is either a directory or the file " << 
                    "already exists" << std::endl;
        return;
    }

    // Lets get the full path of the destination file. We must decide if it is an
    // absolute path or if it will be treated as a relative path
    if (destinationFile[0] == '/') {}
    else
        destinationFile = currentDir + '/' + destinationFile;

    // Loop through the full path to find the destination directory
    int i;
    for (i = destinationFile.length() - 1; i >= 0; i--)
    {
        if (destinationFile[i] == '/')
            break;
    }

    // Now, we want to verify that the destination directory is valid
    std::string destinationDirectory = destinationFile.substr(0, i);
    if (checkValidity(destinationDirectory) != 0)
    {
        std::cout << "Error: Destination directory is not valid" << std::endl;
        return;
    }

    // Here, we want to get the path of the source file. This checks if the source
    // file input is to be treated with a relative path
    if (sourceFile[0] != '/')
        sourceFile = currentDir + '/' + sourceFile;

    // // To read and write to a file we need to create both an 'ifstream' and 'ofstream'
    // // object. We will use this to copy the contents of one file to another
    std::ifstream inFile;
    inFile.open(sourceFile);

    std::ofstream outFile (destinationFile, std::ios::app);

    // If an error has occurred, print an error message and return
    if (!inFile || !outFile)
    {
        std::cout << "Error: Fatal error encountered while attempting IO, check your inputs" << std::endl;
        return;   
    }

    // String that will hold the contents of our source file
    std::string line;

    // Copy the contents of the source file over to the destination file
    while (getline(inFile, line))
    {
        outFile << line << std::endl;
    }

    // Remember to close the files because you know... Memory Leaks!!
    inFile.close();
    outFile.close();
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
        PIDs.push_back(pid);

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
    int statusPtr;

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

        // Here, we will create a 'char *' array from our tokens so that we
        // can use them with the 'exec' system calls
        parse(tokens, argvList);

        if (execvp(argvList[0], argvList) < 0)
        {
            perror("Fork failed");
            exit(1);
        }
    }

    // Here, the parent will wait for the child process to terminate
    if (pid > 0)
    {
        std::cout << "PID: " << pid << std::endl;
        waitpid(pid, &statusPtr, WNOHANG);
    }
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
        // int i;
    
        // If the path is not an absolute path, then it will be treated as
        // a relative path starting from the current directory
        if (path[0] != '/')
        {
            path = currentDir + '/' + path;
            tokens[1] = path;
        }

        // Here, we will create a 'char *' array from our tokens so that we
        // can use them with the 'exec' system calls
        parse(tokens, argvList);

        if (execvp(argvList[0], argvList) < 0)
        {
            perror("Fork failed");
            exit(1);
        }
    }

    // Here, the parent will wait for the child process to terminate
    if (pid > 0)
    {
        waitpid(pid, &statusPtr, WUNTRACED);
    }
}

// This function will terminate a specific process with it's PID sent as input
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
void changeDirectory(std::string& dir)
{
    // The parameter will be treated as an absolute path
    if (dir[0] == '/')
    {
        if (directoryOrFile(dir) == 0)
            currentDir = dir;
        else
            std::cout << "No such directory exists" << std::endl;
    }
    // The parameter will be treated as a relative path
    else
    {
        std::string temp = currentDir + '/' + dir;

        if (directoryOrFile(temp) == 0)
            currentDir = temp;
        else
            std::cout << "No such directory exists" << std::endl;
    }
}

// This will print the history of the users commands
void printHistory(std::vector<std::string>& tokens)
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

    //Making it here means that we should print out the contents of the history
    for (int i = history.size() - 1, j = 0; i >= 0; i--, j++)
        std::cout << j << ": " << history[i] << std::endl;
}

int main(void)
{
    // We first want to prepare the commands of the shell
    insertMainCommands();

    // Variable for the input command, and boolean variables to help with
    // running the loop
    std::string input;
    bool breakLoop = true;
    bool inReplay = false;

    // Get our current working directory
    currentDir = getCurrentWorkingDir();

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
        std::map<std::string, int>::iterator command = mainCommands.find(tokens[0]);

        // Add the input to our history
        history.push_back(input);

        // If the command does not exist, emit an error message, and move on
        // to the next command
        if (command == mainCommands.end())
        {
            std::cout << "Command not recognized" << std::endl;
            continue;
        }

        // Find which command to execute
        switch (command->second) 
        {
            case 1: // movetodir
                changeDirectory(tokens[1]);
                break;
            case 2: // whereami
                std::cout << "Your current directory is: " << currentDir
                            << std::endl;
                break;
            case 3: // history
                printHistory(tokens);
                break;
            case 4: // byebye
                writeHistory();
                breakLoop = false;
                break;
            case 5: // Replay
                input = history[std::stoi(tokens[1])];
                inReplay = true;
                break;
            case 6: // Start
                startProgram(tokens);
                break;
            case 7: // background
                runInBackground(tokens);
                break;
            case 8: // dalek
                dalek(tokens);
                break;
            case 9: // repeat
                repeat(tokens);
                break;
            case 10: // dwelt
                checkDwelt(tokens[1]);
                break;
            case 11: // maik
                createFile(tokens[1]);
                break;
            case 12: // coppy
                copyFile(tokens[1], tokens[2]);
                break;
            default: // There was an error
                break;
        }
    }

    return 0;
}
