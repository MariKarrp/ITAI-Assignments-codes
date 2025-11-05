#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <unordered_set>
using namespace std;
//The "skeleton" for the Backtracking code was taken from my A* code, but changed to DFS logic for backtracking
//initializing the directions
int direction_x[4] = {0,1,0,-1};
int direction_y[4] = {1,0,-1,0};

//Frodo's state
struct State {
    int x, y;
    bool hasMithril = false;
    bool ringOn = false;
    bool foundGollum = false;
    int doomX = -1, doomY = -1;
};

class FrodoNavigator {
private:
    int variant, gollumX, gollumY;
    vector<pair<int,int>> dangerCells; //to control dangerous cells with perception
    vector<pair<pair<int,int>, char>> enemies; //enemies and their positions
    State current;
    vector<vector<bool>> knownCells; //known cells of the map
    vector<vector<string>> color; //the main idea, that I took from the DSA course, exactly from the CLRS
    vector<pair<int, int>> currentPath; //the current path in DFS
    vector<pair<int, int>> foundPath;
public:
    //constructor
    FrodoNavigator(int v, int gx, int gy) : variant(v), gollumX(gx), gollumY(gy) {
        current.x = 0;
        current.y = 0;
        knownCells = vector<vector<bool>>(13, vector<bool>(13, false));
        knownCells[0][0] = true; //Frodo's initial position always (0,0)
    }
    //initializing the dangerous radius of the enemies, considering the mithril and ring on/off
    int getDangerRadius(char type) const {
        if (type == 'O') {
            if (current.hasMithril || current.ringOn) {
                return 0;
            }
            else {
                return 1;
            }
        }
        if (type == 'U') {
            if (current.hasMithril || current.ringOn) {
                return 1;
            }
            else {
                return 2;
            }
        }
        if (type == 'N') {
            if (current.ringOn) {
                return 2;
            }
            else {
                return 1;
            }
        }
        if (type == 'W') {
            return 2;
        }
        return 0;
    }
    //since the enemies' perception zone can consider "ears", so here how to process this
    string getNeighborhoodType(char type) const {
        if (type == 'O' || type == 'U') {
            return "no_ears";
        }
        return "ears";
    }
    //updating the list of dangerous cells based on enemies and their radius
    void updateDangerZones() {
        dangerCells.clear();
        unordered_set<string> dangerSet; //to avoid duplicates

        //going through the known enemies
        for (const auto& enemy : enemies) {
            int ex = enemy.first.first;
            int ey = enemy.first.second;
            char type = enemy.second;
            int radius = getDangerRadius(type);
            string neighborhood = getNeighborhoodType(type);
            //cell with the enemy always dangerous
            dangerSet.insert(to_string(ex) + "," + to_string(ey));
            //adding cells within the danger radius
            if (radius > 0) {
                for (int direction_x = -radius; direction_x <= radius; direction_x++) {
                    for (int direction_y = -radius; direction_y <= radius; direction_y++) {
                        if (direction_x == 0 && direction_y == 0) {
                            continue; //miss the center
                        }

                        int nx = ex + direction_x;
                        int ny = ey + direction_y;

                        if (nx >= 0 && nx < 13 && ny >= 0 && ny < 13) {
                            if (neighborhood == "no_ears") {
                                if (abs(direction_x) + abs(direction_y) <= radius) {
                                    dangerSet.insert(to_string(nx) + "," + to_string(ny));
                                }
                            } else {
                                if (max(abs(direction_x), abs(direction_y)) <= radius) {
                                    dangerSet.insert(to_string(nx) + "," + to_string(ny));
                                }
                            }
                        }
                    }
                }
            }
        }
        //here I changed the set to vector since I work with them from the beginning
        for (const auto& coord : dangerSet) {
            size_t pos = coord.find(',');
            int x = stoi(coord.substr(0, pos));
            int y = stoi(coord.substr(pos + 1));
            dangerCells.push_back({x, y});
        }
    }
    //cell is safe if it's known and it isn't dangerous
    bool isSafe(int x, int y) {
        if (!knownCells[y][x]) {
            return false;
        }
        for (const auto& danger : dangerCells) {
            if (danger.first == x && danger.second == y) {
                return false;
            }
        }
        return true;
    }
    //DFS
    int findShortestPathDFS(int sx, int sy, int gx, int gy) {
        //initializing the colors for the DFS, at the beginning all WHITE
        color = vector<vector<string>>(13, vector<string>(13, "WHITE"));
        currentPath.clear();
        foundPath.clear();

        if (dfsVisit(sx, sy, gx, gy)) {
            return foundPath.size() - 1; //path length = the amount of steps
        }
        return -1;
    }
    //please check the code from my Github repository to get more understanding of what is happening and the report paper to understand the theoretical part of this solution
    bool dfsVisit(int x, int y, int gx, int gy) {
        color[y][x] = "GRAY"; //the attended ones are GRAY
        currentPath.push_back({x, y});
        //if the goal is reached, save the path and highlight it as BLACK
        if (x == gx && y == gy) {
            foundPath = currentPath;
            color[y][x] = "BLACK";
            return true;
        }

        for (int i = 0; i < 4; i++) {
            int nx = x + direction_x[i];
            int ny = y + direction_y[i];

            if (nx < 0 || nx >= 13 || ny < 0 || ny >= 13) {
                continue;
            }
            if (!isSafe(nx, ny)) {
                continue;
            }

            if (color[ny][nx] == "WHITE") {
                if (dfsVisit(nx, ny, gx, gy)) {
                    return true;
                }
            }
            else if (color[ny][nx] == "GRAY") {
            }
        }
        //backtrack: remove the cell from the path and mark it as processed
        currentPath.pop_back();
        color[y][x] = "BLACK";
        return false;
    }

    //processing perception through given cells from input
    void processPerception(){
        int count;
        cin >> count;
        knownCells[current.y][current.x] = true;
        for (int i = 0; i < count; i++){
            int x, y;
            char type;
            cin >> x >> y >> type;

            knownCells[y][x] = true;

            if (type == 'C') {
                current.hasMithril = true;
            }
            if (type == 'O' || type == 'U' || type == 'N' || type == 'W') {
                enemies.push_back({{x,y}, type});
            }
        }
        updateDangerZones();
    }
    //the main solver, it's our Frodo
    void Frodo() {
        //try to find the path to Gollum
        processPerception();
        int pathToGollum = -1;
        //try 4 variants: with ring and without, with mithril and without
        for (int ringState = 0; ringState <= 1; ringState++) {
            for (int mithrilState = 0; mithrilState <= 1; mithrilState++) {
                if (mithrilState == 1 && !current.hasMithril) {
                    continue;
                }
            }
            current.ringOn = (ringState == 1);
            updateDangerZones();

            int path = findShortestPathDFS(0, 0, gollumX, gollumY);
            if (path != -1) {
                pathToGollum = path;
                break; //found path then get out
            }

            if (ringState == 1) {
                current.ringOn = false;
                updateDangerZones();
            }
        }

        if (pathToGollum == -1) {
            cout << "e -1" << endl;
            return;
        }

        current.foundGollum = true;

        string line;
        getline(cin, line);
        if (line.empty()) getline(cin, line);
        stringstream ss(line);
        ss >> current.doomX >> current.doomY;

        int pathToDoom = -1;

        for (int ringState = 0; ringState <= 1; ringState++) {
            if (ringState == 1 && !current.ringOn) {
                current.ringOn = true;
                updateDangerZones();
            }

            int path = findShortestPathDFS(gollumX, gollumY, current.doomX, current.doomY);
            if (path != -1) {
                pathToDoom = path;
                break;
            }

            if (ringState == 1) {
                current.ringOn = false;
                updateDangerZones();
            }
        }
        //if the path to the Doom Mountain is not found
        if (pathToDoom == -1) {
            cout << "e -1" << endl;
            return;
        }
        //otherwise return the path
        cout << "e " << (pathToGollum + pathToDoom) << endl;
    }
};
//main
int main() {
    int variant, gx, gy;
    cin >> variant >> gx >> gy;
    FrodoNavigator journey(variant, gx, gy);
    journey.Frodo();
}