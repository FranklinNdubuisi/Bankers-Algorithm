#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip>
using namespace std;

// Function to dump core (all the vectors)
void dumpCore(const vector<vector<int>>& MAX, const vector<vector<int>>& ALLOCATION,
    const vector<vector<int>>& NEED, const vector<int>& AVAILABLE) {
    cout << "-----------------------------------------------\n";
    cout << " Resource Allocation State \n";
    cout << "-----------------------------------------------\n";

    cout << "MAX:\n";
    for (const auto& row : MAX) {
        for (int val : row) cout << setw(4) << val << " ";
        cout << endl;
    }
    cout << "-----------------------------------------------\n";

    cout << "ALLOCATION:\n";
    for (const auto& row : ALLOCATION) {
        for (int val : row) cout << setw(4) << val << " ";
        cout << endl;
    }
    cout << "-----------------------------------------------\n";

    cout << "NEED:\n";
    for (const auto& row : NEED) {
        for (int val : row) cout << setw(4) << val << " ";
        cout << endl;
    }
    cout << "-----------------------------------------------\n";

    cout << "AVAILABLE:\n";
    for (int val : AVAILABLE) cout << setw(4) << val << " ";
    cout << endl;
    cout << "-----------------------------------------------\n";
}

// Function to check if the system is in a safe state
bool isSafeState(const vector<vector<int>>& ALLOCATION, const vector<vector<int>>& NEED,
    const vector<int>& AVAILABLE) {
    int N = ALLOCATION.size(); // Number of processes
    int M = AVAILABLE.size();  // Number of resources
    vector<bool> finished(N, false); // Track completed processes
    vector<int> work = AVAILABLE;    // Work vector simulating available resources

    int completed = 0; // Number of completed processes
    while (completed < N) {
        bool progress = false;
        for (int i = 0; i < N; i++) {
            if (!finished[i]) {
                bool canAllocate = true;
                for (int j = 0; j < M; j++) {
                    if (NEED[i][j] > work[j]) {
                        canAllocate = false;
                        break;
                    }
                }
                if (canAllocate) {
                    // Reclaim resources from this process
                    for (int j = 0; j < M; j++) {
                        work[j] += ALLOCATION[i][j];
                    }
                    finished[i] = true;
                    progress = true;
                    completed++;
                }
            }
        }
        if (!progress) {
            // No progress means deadlock
            return false;
        }
    }
    return true; // All processes can finish
}


// Function to handle a request
bool handleRequest(int process, const vector<int>& request, vector<vector<int>>& ALLOCATION,
    vector<vector<int>>& NEED, vector<int>& AVAILABLE) {
    int M = AVAILABLE.size();

    // Validate the request
    for (int i = 0; i < M; i++) {
        if (request[i] > NEED[process][i]) {
            cout << "Request exceeds NEED for resource " << i << ". Request denied.\n";
            return false;
        }
        if (request[i] > AVAILABLE[i]) {
            cout << "Request exceeds AVAILABLE for resource " << i << ". Request denied.\n";
            return false;
        }
    }

    // Temporarily allocate resources
    for (int i = 0; i < M; i++) {
        AVAILABLE[i] -= request[i];
        ALLOCATION[process][i] += request[i];
        NEED[process][i] -= request[i];
    }

    // Check if the system is in a safe state
    if (isSafeState(ALLOCATION, NEED, AVAILABLE)) {
        cout << "Request is granted.\n";
        return true;
    } else {
        // Roll back the allocation if it leads to an unsafe state
        for (int i = 0; i < M; i++) {
            AVAILABLE[i] += request[i];
            ALLOCATION[process][i] -= request[i];
            NEED[process][i] += request[i];
        }
        cout << "Request leads to unsafe state. Request denied.\n";
        return false;
    }
}




int main() {
    int N, M;
    vector<vector<int>> MAX, ALLOCATION, NEED;
    vector<int> AVAILABLE;

    ifstream inputFile("input.txt");
    if (!inputFile) {
        cerr << "Error: Unable to open file.\n";
        return 1;
    }

    inputFile >> N >> M;
    MAX.resize(N, vector<int>(M));
    ALLOCATION.resize(N, vector<int>(M, 0));
    NEED.resize(N, vector<int>(M));
    AVAILABLE.resize(M);

    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            inputFile >> MAX[i][j]; //read the MAX (maximum priori need)

    for (int i = 0; i < M; i++)
        inputFile >> AVAILABLE[i]; // read the available (resources available)

    inputFile.close();

    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
            NEED[i][j] = MAX[i][j] - ALLOCATION[i][j];

    while (true) {
        dumpCore(MAX, ALLOCATION, NEED, AVAILABLE);

        cout << "\nEnter process number (-1 to quit): ";
        int process;
        cin >> process;
        if (process == -1) break;

        if (process < 0 || process >= N) {
            cout << "Invalid process number. Try again.\n";
            continue;
        }

        vector<int> request(M);
        cout << "Enter request vector:";
        for (int i = 0; i < M; i++) cin >> request[i];

        handleRequest(process, request, ALLOCATION, NEED, AVAILABLE);
    }

    dumpCore(MAX, ALLOCATION, NEED, AVAILABLE);
    cout << "Program terminated.\n";
    return 0;
}
