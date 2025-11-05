#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;
struct node {
    int g, h, f;
    int x,y;
    node* previous;
    node(int x, int y) : x(x), y(y), g(0), h(0), f(0), previous(nullptr) {}
};
struct priority {
    bool operator()(node* a, node* b) {
        return a->f > b->f;
    }
};
vector<pair<int, int>> AStar (vector<vector<int>> map, pair<int, int> start, pair<int, int> end) {
    int rows = map.size();
    int columns = map[0].size();
    node *startN = new node(start.first, start.second);
    vector<vector<node*>> nodes(rows, vector<node*>(columns, nullptr));
    priority_queue<node*, vector<node*>, priority> openTile;
    auto heuristic = [](int x1, int y1, int x2, int y2) {
        int dx = abs(x1 - x2);
        int dy = abs(y1 - y2);

        return min(dx, dy) + (max(dx, dy) - min(dx, dy));
    };
    startN->h = heuristic(start.first, start.second, end.first, end.second);
    startN->f = startN->h;
    openTile.push(startN);
    nodes[start.second][start.first] = startN;
    vector <pair<int, int>> direction = {{0,1}, {1,0}, {0, -1}, {-1, 0}, {-1,1}, {-1, -1}, {1, -1}, {1, 1}};
    while (!openTile.empty()) {
        //since the priority queue worked, the min f on the top
        node* current = openTile.top();
        openTile.pop();
        //backtrack
        if (current->x == end.first && current->y == end.second) {
            vector<pair<int, int>> path;
            node* temp = current;
            while (temp != nullptr) {
                path.push_back({temp->x, temp->y});
                temp = temp->previous;
            }
            reverse(path.begin(), path.end());
            return path;
        }
        //neighbors verification
        for (auto dir : direction) {
            int newX = current->x + dir.first;
            int newY = current->y + dir.second;
            //0 - ok, 1 - obstacle
            if (newX < 0 || newX >= columns || newY < 0 || newY >= rows || map[newY][newX] == 1) {
                continue;
            }

            int moveCost = (dir.first != 0 && dir.second != 0) ? 14 : 10;
            int newG = current->g + moveCost;
            //if node is not visited or found a shorter path
            if (nodes[newY][newX] == nullptr || newG < nodes[newY][newX]->g) {
                node* neighbor;
                if (nodes[newY][newX] == nullptr) {
                    neighbor = new node(newX, newY);
                    nodes[newY][newX] = neighbor;
                } else {
                    neighbor = nodes[newY][newX];
                }
                //update
                neighbor->g = newG;
                neighbor->h = heuristic(newX, newY, end.first, end.second);
                neighbor->f = neighbor->g + neighbor->h;
                neighbor->previous = current;

                openTile.push(neighbor);
            }
        }
    }
    //no path has found. bad
    return {};
}
//just an example
int main() {
    vector<vector<int>> map = {
            {0, 0, 0, 0, 0},
            {0, 1, 1, 0, 0},
            {0, 0, 0, 0, 0},
            {0, 1, 1, 1, 0},
            {0, 0, 0, 0, 0}
    };

    pair<int, int> start = {0, 0};
    pair<int, int> end = {4, 4};

    vector<pair<int, int>> path = AStar(map, start, end);

    if (path.empty()) {
        cout << "No path" << endl;
    } else {
        cout << "Path ";
        for (auto point : path) {
            cout << "(" << point.first << "," << point.second << ") ";
        }
        cout << endl;
    }
}