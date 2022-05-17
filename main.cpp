#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <stdio.h>
#include <string.h>
#include <set>
#include <unordered_set>

using namespace std;

const int N = 90000;
vector<pair<int, int>> Edges;
vector<pair<string, string>> Relations;
map<string, vector<int>> NounToIDs;
vector<string> IDToNouns[N];
unordered_set<string> st[N];
vector<int> adj[N], revadj[N];
bool vis[N][3];
int depth[N][2], root, n;

void Input() {
    ifstream input;
    //-----------------------------------------------Synsets Input--------------------------------------------------------
    input.open("1synsets.txt");
    string line;
    int idx = 0;//idx is the number of lines finally
    if (input.is_open()) {
        bool start_synset = false;
        while (getline(input, line, '\n')) {
            string x;
            for (int i = 0; i < line.size(); i++) {
                if (line[i] == ',' && start_synset == false) {
                    start_synset = true;
                    continue;
                }
                if (start_synset == true) {
                    if (line[i] == ',') {
                        NounToIDs[x].push_back(idx);
                        IDToNouns[idx].push_back(x);
                        st[idx].insert(x);
                        idx++;
                        start_synset = false;
                        x.clear();
                        break;
                    }
                    if (line[i] == ' ') {
                        NounToIDs[x].push_back(idx);
                        IDToNouns[idx].push_back(x);
                        st[idx].insert(x);
                        x.clear();
                    } else
                        x.push_back(line[i]);
                }

            }
        }
        n = idx;
        input.close();
    } else
        cout << "Invalid Filex" << endl;

    //-----------------------------------------------Hypernyms Input------------------------------------------------------
    input.close();
    input.open("2hypernyms.txt");
    if (input.is_open()) {
        while (getline(input, line)) {
            string h, firsth;
            int i = 0;
            while (i < line.size() && line[i] != ',') {
                firsth += line[i];
                i++;
            }
            i++;
            while (i < line.size()) {
                if (line[i] == ',') {
                    Edges.push_back({stoi(firsth), stoi(h)});
                    h.clear();
                } else {
                    h += line[i];
                }
                i++;
            }
            if (!h.empty())
                Edges.push_back({stoi(firsth), stoi(h)});
        }
        input.close();
    } else
        cout << "Invalid File" << endl;
    //-----------------------------------------------Relation Input------------------------------------------------------
    input.close();
    input.open("3RelationsQueries.txt");
    if (input.is_open()) {
        int n;
        input >> n;
        for (int i = 0; i < n; i++) {
            string s;
            input >> s;
            string a, b;
            bool done = false;

            for (auto u : s) {
                if (u == ',') {
                    done = true;
                    continue;
                }
                if (done)
                    b.push_back(u);
                else a.push_back(u);
            }

            Relations.push_back({a, b});
        }
    } else cout << "Invalid File" << endl;
    input.close();
}

vector<int> Noun_to_SynsetsIDs(string noun) {
    vector<int> result = NounToIDs[noun];
    return result;
}

vector<string> SynsetsID_to_Nouns(int id) {
    vector<string> result = IDToNouns[id];
    return result;
}

void dfs(int node, int turn, int dep) {
    vis[node][turn] = true;
    depth[node][turn] = dep;
    for (auto u : adj[node])
        if (!vis[u])
            dfs(u, turn, dep + 1);
}

int LCA(int a, int b) {
    memset(vis, false, sizeof vis);
    dfs(a, 0, 0);
    dfs(b, 1, 0);

    queue<int> q;
    q.push(b);

    while (!q.empty()) {
        int node = q.front();
        q.pop();
        if (vis[node][2]) continue;
        if (vis[node][0] && vis[node][1]) return depth[node][0] + depth[node][1];
        vis[node][2] = true;

        for (auto u : adj[node])
            q.push(u);
    }

    return 0;
}

int dp[N][2];
string X, Y;

int solve(int node, int turn) {
    int &ret = dp[node][turn];
    if (~ret) return ret;
    ret = 1e9;

    for (auto u : revadj[node])
        ret = min(ret, solve(u, turn) + 1);

    bool ok = st[node].count((turn ? X : Y));

    if (ok) return ret = 0;
    else {
        if (revadj[node].empty()) return ret = 1e9;
        else return ret;
    }
}

pair<int, int> solveDP(string &x, string &y) {
    X = x, Y = y;
    memset(dp, -1, sizeof dp);
    solve(root, 0);
    solve(root, 1);

    int mnm = INT_MAX;
    int idx;
    for (int i = 0; i < n; i++) {
        if (dp[i][0] < 1e9 && dp[i][1] < 1e9 && dp[i][0] >= 0 && dp[i][1] >= 0) {
            if (mnm > dp[i][0] + dp[i][1])
                mnm = dp[i][0] + dp[i][1], idx = i;
        }
    }

    return {mnm, idx};
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    Input();

    for (int i = 0; i < Edges.size(); i++)
        adj[Edges[i].first].push_back(Edges[i].second),
                revadj[Edges[i].second].push_back(Edges[i].first);

    for (int i = 0; i < n; i++)
        if (adj[i].empty())
            root = i;

    for (auto u : Relations) {
        pair<int , int> ans = solveDP(u.first, u.second);
        cout << ans.first << ',';
        for (auto u : IDToNouns[ans.second])
            cout << u << ' ';
        cout << '\n';
    }

    return 0;
}