#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <iostream>
#include <stdio.h>
#include <map>
#define MAX 1000 
#define MAXLIST 100
using namespace std;
map<string, string> dict; //(key,value) -> by that order
map <string, string> :: iterator itr;

bool replace(string& str, const string& from, const string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}
void printDir()
{
    char cwd[1024];
    char* cc;
    cc=getcwd(cwd, sizeof(cwd));
    cc+=1;
    char *usr = getenv("HOME");
    string user=usr; 
    string cwd2=cwd;
    replace(cwd2, user, "~");
    cout << "OS SHell:" << cwd2 << "> ";
}
bool Input(char* str)
{
    cin.getline(str,1024);
    return true;
}

void execArgs(char** parsed)
{
    pid_t pid = fork(); 
    int status;
    string s;
    if (pid == -1) {
        cout << "Error-Failed forking child"<<endl;
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {          
            cout << "Error - Could not execute command." <<endl;
        }
        exit(0);
    } else {
        waitpid(pid,&status,0);
        s=to_string(status);
        dict["?"]=s;
        //wait(NULL); 
        return;
    }
}
int varDict(char* parsed )
{
    char* temp[2];
    for(int i=0;i<2;i++){
        temp[i]=strsep(&parsed, "=");
    }
    dict[string(temp[0])] = string(temp[1]);
    return 0;
}

int ownCmdHandler(char** parsed)
{
    char cwd[1024];
    char*c;
    c=getcwd(cwd, sizeof(cwd));
    c+=1;
    int i=0,flag=0;
    while(parsed[i]!=NULL){
        if(strstr(parsed[i], "=") != NULL){
            flag=1;
            varDict(parsed[i]);
            continue;     
        }
        i++;
    }
    if(flag>0)
        return 1;      
    else if(strcmp(parsed[0],"exit")==0){
        cout << "C ya"<<endl;
        exit(0);
    }
    else if(strcmp(parsed[0],"cd")==0){
        if(strstr(parsed[1], "~") != NULL){
            char *hm = getenv("HOME");
            string s = string(hm),p=parsed[1];
            replace(p,"~", s); 
            char * writable = new char[p.size() + 1];
            copy(p.begin(), p.end(), writable);
            writable[p.size()] = '\0';
            parsed[1]=writable;
        }
        if (chdir(parsed[1])<0)
            perror(parsed[1]);
        //chdir(parsed[1]);
        dict["?"]="1";
        return 1;
    }
    dict["?"]="0";
    return 0;
}

void parseSpace(char* str, char** parsed)
{
    int i;   
    for (i = 0; i < MAXLIST; i++) {
        parsed[i] = strsep(&str, " ");
 
        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
}

int processString(char* str, char** parsed)
{
    parseSpace(str, parsed);
    int i=0;
    string s;
    
    while(parsed[i]!=NULL){
        if(strstr(parsed[i], "$") != NULL){
            char* var[2];
            for(int j=0;j<2;j++)
                var[j]=strsep(&parsed[i], "$");
            for (itr = dict.begin(); itr != dict.end(); ++itr){
                if(itr->first == var[1]){
                    s=string(var[0])+string(itr->second);
                    char * writable = new char[s.size() + 1];
                    copy(s.begin(), s.end(), writable);
                    writable[s.size()] = '\0';
                    parsed[i]=writable;
                } 
            }
        }  
        i++;
    }
    if (ownCmdHandler(parsed))
        return 0;
    else    
        return 1;
}

int main(int argc, char *argv[])
{
    char inputParse[MAX],*parseArgs[MAXLIST];
    int execFlag = 0;
    dict["HOME"]=getenv("HOME");
    cout << "***********"<< endl << "Welcome to OS SHell!" << endl <<"***********" << endl;
    while(true){
        printDir();
        Input(inputParse);
        execFlag = processString(inputParse,parseArgs);
        if (execFlag == 1)
            execArgs(parseArgs);
        continue;
    }
    return 0;
}