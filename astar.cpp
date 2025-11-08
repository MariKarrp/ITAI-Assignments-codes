#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <sstream>
using namespace std;

const int direction_x[4] = {0,1,0,-1};
const int direction_y[4] = {1,0,-1,0};

//These structures are from my A* code (please check Github)
struct Node {
    int x, y;
    int g, f;
    Node(int x, int y, int g, int f) : x(x), y(y), g(g), f(f) {}
};

struct Priority {
    bool operator()(const Node& a, const Node& b) {
        return a.f > b.f;
    }
};

//Frodo's state
struct State {
    int x, y;
    bool hasMithril = false;
    bool ringOn = false;
    bool foundGollum = false;
    int doomX = -1, doomY = -1;
};

class FrodoNavigator  {
private:
    int variant, gollumX, gollumY;
    vector<pair<int,int>> safeCells; //known as safe cells with perception
    vector<pair<int,int>> dangerCells; //to control dangerous cells with perception
    vector<pair<pair<int,int>, char>> enemies; //enemies and their positions
    State current;
public:
    //constructor
    FrodoNavigator (int v, int gx, int gy) : variant(v), gollumX(gx), gollumY(gy) {
        current.x = 0;
        current.y = 0;
        safeCells.push_back({0,0});
    }
    //initializing the dangerous radius of the enemies, considering the mithril and ring on/off
    int getDangerRadius(char type) const {
        if (type == 'O') {
            if (current.hasMithril || current.ringOn) {
                return 0;
            } else {
                return 1;
            }
        }
        if (type == 'U') {
            if (current.hasMithril || current.ringOn) {
                return 1;
            } else {
                return 2;
            }
        }
        if (type == 'N') {
            if (current.ringOn) {
                return 2;
            } else {
                return 1;
            }
        }
        if (type == 'W') {
            return 2;
        }
        return 0;
    }
    //updating the list of dangerous cells based on enemies and their radius
    void updateDangerZones() {
        dangerCells.clear();
        //going through the known enemies
        for (int i = 0; i < enemies.size(); i++) {
            int ex = enemies[i].first.first;
            int ey = enemies[i].first.second;
            char type = enemies[i].second;
            int radius = getDangerRadius(type);
            dangerCells.push_back({ex, ey});
            if (type == 'O' || type == 'U') {
                //adding cells within the danger radius
                for (int dx = -radius; dx <= radius; dx++) {
                    for (int dy = -radius; dy <= radius; dy++) {
                        if (abs(dx) + abs(dy) <= radius) {
                            int nx = ex + dx;
                            int ny = ey + dy;
                            if (nx >= 0 && nx < 13 && ny >= 0 && ny < 13) {
                                dangerCells.push_back({nx, ny});
                            }
                        }
                    }
                }
            } else { //since the enemies' perception zone can consider "ears", so here how to process this case
                for (int dx = -radius; dx <= radius; dx++) {
                    for (int dy = -radius; dy <= radius; dy++) {
                        int nx = ex + dx;
                        int ny = ey + dy;
                        if (nx >= 0 && nx < 13 && ny >= 0 && ny < 13) {
                            dangerCells.push_back({nx, ny});
                        }
                    }
                }
            }
        }
    }
    //cell is safe if it's known and it isn't dangerous
    bool isSafe(int x,int y) {
        return find(dangerCells.begin(), dangerCells.end(), make_pair(x, y)) == dangerCells.end();
    }
    //heuristic for the A* (please check the Github repository, that was provided below)
    int heuristic(int x, int y, int goalX, int goalY) {
        return abs(x - goalX) + abs(y - goalY);
    }
    //A*
    vector<pair<int,int>> aStar(int sx,int sy,int gx,int gy) {
        vector<vector<int>> g(13, vector<int>(13, INT_MAX));
        vector<vector<pair<int, int>>> parent(13, vector<pair<int, int>>(13, {-1, -1}));
        priority_queue<Node, vector<Node>, Priority> pq;

        g[sy][sx] = 0;
        int start_f = heuristic(sx, sy, gx, gy);
        pq.push(Node(sx, sy, 0, start_f));

        while (!pq.empty()) {
            Node current = pq.top();
            pq.pop();
            int x = current.x;
            int y = current.y;

            // if the goal is reached, restore the path
            if (x == gx && y == gy) {
                vector<pair<int, int>> path;
                pair<int, int> cur = {x, y};
                while (cur.first != -1) {
                    path.push_back(cur);
                    cur = parent[cur.second][cur.first];
                }
                //laying out the path from start to goal
                reverse(path.begin(), path.end());
                return path;
            }

            //checking all neighbors
            for (int i = 0; i < 4; i++) {
                int nx = x + direction_x[i], ny = y + direction_y[i];
                if (nx < 0 || nx >= 13 || ny < 0 || ny >= 13) {
                    continue;
                }
                if (!isSafe(nx, ny)) {
                    continue; //skipping unsafe cells
                }
                //if a shorter path to the neighbor is found
                int newG = g[y][x] + 1;
                if (newG < g[ny][nx]) {
                    g[ny][nx] = newG;
                    parent[ny][nx] = {x, y}; //remember the parent
                    int newF = newG + heuristic(nx, ny, gx, gy);
                    //adding the neighbor to the queue with a new score
                    pq.push(Node(nx, ny, newG, newF));
                }
            }
        }
        return {};//path was not found
    }
    //processing perception through given cells from input
    void processPerception(){
        int count;
        cin >> count;
        for (int i =0 ; i < count; i++){
            int x,y;
            char type;
            cin >> x >> y >> type;
            if (type == 'C') {
                current.hasMithril=true;
            }
            if (type == 'O' || type == 'U' || type == 'N' || type == 'W') {
                enemies.push_back({{x,y},type});
            }
            safeCells.push_back({x,y});
        }
        updateDangerZones();
    }

    void tryActivateRing() {
        if (!current.ringOn) {
            current.ringOn = true;
            updateDangerZones();
        }
    }
    //the main solver, it's our Frodo
    void Frodo () {
        vector<string> moves;
        //try to find the path to Gollum
        processPerception();
        auto pathG = aStar(0,0,gollumX,gollumY);
        if (pathG.empty()) {
            tryActivateRing();
            pathG = aStar(0,0,gollumX,gollumY);
        }
        if (pathG.empty()) {
            moves.clear();
            cout << "e -1\n";
            return;
        }
        for (size_t i = 1; i < pathG.size(); i++)
            moves.push_back("m " + to_string(pathG[i].first) + " " + to_string(pathG[i].second));

        current.foundGollum = true;
        string line;
        getline(cin,line);
        stringstream ss(line);
        string tmp;
        ss >> tmp >> tmp >> tmp >> tmp >> tmp;
        ss >> current.doomX >> current.doomY;
        auto pathD = aStar(gollumX,gollumY,current.doomX,current.doomY);
        if (pathD.empty()) {
            tryActivateRing();
            pathD = aStar(gollumX,gollumY,current.doomX,current.doomY);
        }
        if (pathD.empty()) {
            moves.clear();
            cout << "e -1\n";
            return;
        }
        for (size_t i = 1; i < pathD.size(); i++)
            moves.push_back("m " + to_string(pathD[i].first) + " " + to_string(pathD[i].second));
        for(auto &mv : moves)
            cout << mv << "\n";
        cout << "e " << (pathG.size()-1 + pathD.size()-1) << "\n";
    }
};
//main
int main(){
    int variant, gx, gy;
    cin >> variant >> gx >> gy;
    FrodoNavigator Journey(variant,gx,gy);
    Journey.Frodo ();
}
