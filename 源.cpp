#include <iostream>
#include <vector>
#include <algorithm> 
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
using namespace std;
class Process {
public:
	string pname;
	int arrTime;
	int operTime;
	int priority_num;
	int responseTime;
	int inventory;
	Process(string pname, int arrTime, int operTime, int priority_num) {
		this->pname = pname;
		this->arrTime = arrTime;
		this->operTime = operTime;
		this->priority_num = priority_num;
	}
	Process(vector<string> processInfo) {
		pname = processInfo[0];
		arrTime = stoi(processInfo[1]);
		operTime = stoi(processInfo[2]);
		priority_num = stoi(processInfo[3]);
		responseTime = 0;
		inventory = 0;
	}
};



//sort parameter
bool SortByM1(const Process *v1, const Process *v2)//注意：本函数的参数的类型一定要与vector中元素的类型一致  
{
	return v1->operTime < v2->operTime;//升序排列  
}
bool SortByM2(const Process *v1, const Process *v2)//注意：本函数的参数的类型一定要与vector中元素的类型一致  
{
	return v1->priority_num < v2->priority_num;//升序排列  
}

bool operator==(Process const &p1, Process const & p2) {
	return p1.pname == p2.pname;
}
class processManage{
private:
	int timeNow;
public:
	vector<Process> processes;
	vector<Process> finishedProcesses;
	processManage(vector<Process> processes) {
		this->processes = processes;
		this->finishedProcesses = {};
		timeNow = 0;
	}

	vector<Process *> pushWait() {
		vector<Process *> waitProcesses;
		cout << "=====================" << endl;
		cout << "Now time is =" << timeNow << endl;
		for (vector<Process>::size_type iter = 0; iter != processes.size(); iter++) {
			if (processes[iter].arrTime <= this->timeNow) {
				waitProcesses.push_back(&processes[iter]);
				//remove(processes.begin(),processes.end(),processes[iter]);
			}
			if (processes[iter].arrTime == this->timeNow){
				cout << processes[iter].pname << " arrive!" << endl;
			}
		}
		return waitProcesses;
	}

	void excute(Process *process,int slice =1) {
		cout << process->pname << " started excuting." << endl;
		if (process->operTime <= slice) {
			cout<< process->pname << " finished excuting." << endl;
			finishedProcesses.push_back(*process);
			remove(processes.begin(), processes.end(), *process);
			processes.pop_back();

			for (int times = 0; times < process->operTime; times++) {
				this->timeNow++;
				pushWait();
			}
		}
		else {
			for (int times = 0; times < slice; times++) {
				this->timeNow++;
				pushWait();
			}
			if (slice != 1)
				cout << process->pname << " was interrupted!" << endl;
			process->operTime -= slice;
		}
	}
	void SJF() {
		vector<Process *> waitProcesses;
		cout << "===========进程调度开始============" << endl;
		while (processes.size() != 0) {
			waitProcesses = pushWait();
			if (waitProcesses.size() != 0) {
				sort(waitProcesses.begin(), waitProcesses.end(), SortByM1);
				excute(waitProcesses[0]);
			}
			this->timeNow++;
		}
		cout << "=============进程调度结束===========" << endl;
	}

	void roundRobin(int slice) {
		vector<Process *> waitProcesses;
		cout << "===========进程调度开始============" << endl;
		int timeslice = slice;
		while (processes.size() != 0){
			waitProcesses = pushWait();
			if (waitProcesses.size() != 0 && timeslice%slice==0) {
				sort(waitProcesses.begin(), waitProcesses.end(), SortByM1);
				excute(waitProcesses[0], slice);
			}
			timeslice++;
			this->timeNow++;
		}
		cout << "=============进程调度结束===========" << endl;
	} 
	void priority() {
		vector<Process *> waitProcesses;
		cout << "===========进程调度开始============" << endl;
		while (processes.size() != 0) {
			waitProcesses = pushWait();
			if (waitProcesses.size() != 0) {
				sort(waitProcesses.begin(), waitProcesses.end(), SortByM2);
				excute(waitProcesses[0]);
			}
			this->timeNow++;
		}
		cout << "=============进程调度结束===========" << endl;
	}
};
void splitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));

		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}
vector<Process> readFile(string filepath) {
	ifstream file;
	string line;
	vector<Process> processes = {};
	file.open(filepath);
	if (file.is_open()) {
		while (getline(file, line)) {
			vector<string> processInfo;
			splitString(line, processInfo, ",");
			Process process = Process(processInfo);
			processes.push_back(process);
		}
	}
	else {
		cout << "Open file failed" << endl;
		exit(EXIT_FAILURE);
	}
	file.close();
	return processes;
}



int main(int argc, const char * argv[]) {

	string filePahth;
	cout << "Please input file path:" << endl;
	cin >> filePahth;

	vector<Process> processes = readFile(filePahth);

	processManage manager0 = *new processManage(processes);
	manager0.SJF();

	processManage manager1 = *new processManage(processes);
	manager1.roundRobin(1);
	//manager1.roundRobin(2);
	//manager1.roundRobin(3);

	processManage manager2 = *new processManage(processes);
	manager2.priority();
	return 0;
}