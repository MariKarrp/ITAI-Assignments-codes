#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
using namespace std;
//initializing the directions
int direction_x[4] = {0, 1, 0, -1};
int direction_y[4] = {1, 0, -1, 0};

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
    int variant;
    int gollumX, gollumY;
    vector<pair<int,int>> dangerCells; //to control dangerous cells with perception
    vector<pair<pair<int,int>, char>> enemies; //enemies and their positions
    State current;
    vector<vector<bool>> knownCells; //known cells of the map
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
        vector<vector<bool>> danger(13, vector<bool>(13, false));

        //going through the known enemies
        for (auto &enemy : enemies) {
            int ex = enemy.first.first;
            int ey = enemy.first.second;
            char type = enemy.second;
            int radius = getDangerRadius(type);
            string neighborhood = getNeighborhoodType(type);
            //adding cells within the danger radius
            for (int direction_x = -radius; direction_x <= radius; direction_x++) {
                for (int direction_y = -radius; direction_y <= radius; direction_y++) {
                    int nx = ex + direction_x;
                    int ny = ey + direction_y;
                    if (nx < 0 || nx >= 13 || ny < 0 || ny >= 13) {
                        continue;
                    }
                    if (neighborhood == "no_ears" && abs(direction_x) + abs(direction_y) > radius) {
                        continue;
                    }
                    if (neighborhood == "ears" && max(abs(direction_x), abs(direction_y)) > radius) {
                        continue;
                    }
                    danger[ny][nx] = true;
                }
            }
        }
        //converting the matrix fo danger into a list of cells
        for (int y = 0; y < 13; y++)
            for (int x = 0; x < 13; x++)
                if (danger[y][x])
                    dangerCells.push_back({x, y});
    }
    //cell is safe if it's known and it isn't dangerous
    bool isSafe(int x, int y) {
//        if (x < 0 || x >= 13 || y < 0 || y >= 13) {
//            return false;
//        }
        if (!knownCells[y][x]) {
            return false;
        }
        for (auto &danger : dangerCells) {
            if (danger.first == x && danger.second == y) {
                return false;
            }
        }
        return true;
    }
    //heuristic for the A* (please check the Github repository, that was provided below)
    int heuristic(int x, int y, int goalX, int goalY) {
        return abs(x - goalX) + abs(y - goalY);
    }
    //A*
    vector<pair<int,int>> aStarPath(int sx, int sy, int gx, int gy) {
        struct Node {
            int x, y;
            int g; //the cost of the path from the start
            int f; //g + heuristic
        };
        auto cmp = [](const Node &a, const Node &b) { return a.f > b.f; };
        priority_queue<Node, vector<Node>, decltype(cmp)> pq(cmp);

        vector<vector<int>> dist(13, vector<int>(13, 1e9)); //from the beginning all paths are infinity
        vector<vector<pair<int,int>>> parent(13, vector<pair<int,int>>(13, {-1, -1}));
        dist[sy][sx] = 0;
        pq.push({sx, sy, 0, heuristic(sx, sy, gx, gy)});
//        pq.push({sx, sy, 0, abs(sx - gx) + abs(sy - gy)});

        while (!pq.empty()) {
            //extracting the node with the lowest f score
            auto [x, y, g, f] = pq.top();
            pq.pop();
            //if the goal is reached, we restore the path
            if (x == gx && y == gy) {
                vector<pair<int,int>> path;

                for (int cx = x, cy = y; cx != -1;) {
                    path.push_back({cx, cy});
                    auto p = parent[cy][cx];
                    cx = p.first; cy = p.second;
                }
                //laying out the path from start to goal
                reverse(path.begin(), path.end());
                return path;
            }
            //checking all neighbors
            for (int i = 0; i < 4; i++) {
                int nx = x + direction_x[i], ny = y + direction_y[i];
                if (!isSafe(nx, ny)) {
                    continue; //skipping unsafe cells
                }
                //if a shorter path to the neighbor is found
                if (dist[ny][nx] > g + 1) {
                    dist[ny][nx] = g + 1;
                    parent[ny][nx] = {x, y}; //remember the parent
                    //adding the neighbor to the queue with a new score
                    pq.push({nx, ny, g + 1, g + 1 + heuristic(nx, ny, gx, gy)});
//                    pq.push({nx, ny, g + 1, g + 1 + abs(nx - gx) + abs(ny - gy)});
                }
            }
        }
        return {};//path was not found
    }
    //processing perception through given cells from input
    void processPerception() {
        int count;
        cin >> count;
        knownCells[current.y][current.x] = true;
        for (int i = 0; i < count; i++) {
            int x, y; char t;
            cin >> x >> y >> t;
            knownCells[y][x] = true;

            if (t == 'C') {
                current.hasMithril = true;
            }
            else if (t == 'O' || t == 'U' || t == 'N' || t == 'W') {
                enemies.push_back({{x, y}, t});
            }
        }
        updateDangerZones();
    }
    //the main solver, it's our Frodo
    void Frodo() {
        //try to find the path to Gollum
        processPerception();
        int pathToGollum = -1;
        for (int ringState = 0; ringState <= 1; ringState++) {
            for (int mithrilState = 0; mithrilState <= 1; mithrilState++) {
                if (mithrilState == 1 && !current.hasMithril) {
                    continue;
                }
            }
            current.ringOn = (ringState == 1);
            updateDangerZones();

            auto path = aStarPath(0, 0, gollumX, gollumY);
            if (!path.empty()) {
                pathToGollum = (int)path.size() - 1;
                break;
            }

            if (ringState == 1) {
                current.ringOn = false;
            }
        }

        if (pathToGollum == -1) {
            cout << "e -1" << endl;
            return;
        }

        current.foundGollum = true;

        int doomX, doomY;
        if (!(cin >> doomX >> doomY)) {
            cout << "e -1" << endl;
            return;
        }
        current.doomX = doomX;
        current.doomY = doomY;
        
        int bestDoom = -1;
        //again checking the mithril & ring
        for (int ringState = 0; ringState <= 1; ringState++) {
            for (int mithrilState = 0; mithrilState <= 1; mithrilState++) {
                if (mithrilState == 1 && !current.hasMithril) {
                    continue;
                }
            }
            current.ringOn = (ringState == 1);
            updateDangerZones();

            auto pathToDoom = aStarPath(gollumX, gollumY, doomX, doomY);
            if (!pathToDoom.empty()) {
                bestDoom = (int)pathToDoom.size() - 1;
                break;
            }

            if (ringState == 1) {
                current.ringOn = false;
            }
        }

        if (bestDoom == -1) {
            cout << "e -1" << endl;
            return;
        }

        cout << "e " << (pathToGollum + bestDoom) << endl;
    }
};
//main
int main() {
    int variant, gx, gy;
    cin >> variant >> gx >> gy;
    FrodoNavigator journey(variant, gx, gy);
    journey.Frodo();
}