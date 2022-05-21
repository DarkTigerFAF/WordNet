#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <string.h>
#include <fstream>
#include <stdio.h>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include <bits/stdc++.h>

using namespace std;

const int N = 90000;
int dp[N][2];
vector<pair<int, int>> Edges;
vector<pair<string, string>> Relations;
unordered_map<string, vector<int>> NounToIDs;
vector<string> IDToNouns[N], OutCast[N];
vector<int> adj[N], revadj[N];
bool vis[N][3];
int depth[N][2], root, n;

void Input()
{
    ifstream input;
    //-----------------------------------------------Synsets Input--------------------------------------------------------
    input.open("1synsets.txt");
    string line;
    int idx = 0; // idx is the number of lines finally
    if (input.is_open())
    {
        bool start_synset = false;
        while (getline(input, line, '\n'))
        {
            string x;
            for (int i = 0; i < line.size(); i++)
            {
                if (line[i] == ',' && start_synset == false)
                {
                    start_synset = true;
                    continue;
                }
                if (start_synset == true)
                {
                    if (line[i] == ',')
                    {
                        NounToIDs[x].push_back(idx);
                        IDToNouns[idx].push_back(x);
                        idx++;
                        start_synset = false;
                        x.clear();
                        break;
                    }
                    if (line[i] == ' ')
                    {
                        NounToIDs[x].push_back(idx);
                        IDToNouns[idx].push_back(x);
                        x.clear();
                    }
                    else
                        x.push_back(line[i]);
                }
            }
        }
        n = idx;
        input.close();
    }
    else
    {
        cout << "Invalid File" << endl;
        input.close();
    }
    //-----------------------------------------------Hypernyms Input------------------------------------------------------
    input.close();
    input.open("2hypernyms.txt");
    if (input.is_open())
    {
        while (getline(input, line))
        {
            string h, firsth;
            int i = 0;
            while (i < line.size() && line[i] != ',')
            {
                firsth += line[i];
                i++;
            }
            i++;
            while (i < line.size())
            {
                if (line[i] == ',')
                {
                    Edges.push_back({stoi(firsth), stoi(h)});
                    h.clear();
                }
                else
                {
                    h += line[i];
                }
                i++;
            }
            if (!h.empty())
                Edges.push_back({stoi(firsth), stoi(h)});
        }
        input.close();
    }
    else
    {
        cout << "Invalid File" << endl;
        input.close();
    }
    //-----------------------------------------------Relation Input------------------------------------------------------
    input.open("3RelationsQueries.txt");
    if (input.is_open())
    {
        int n;
        input >> n;
        for (int i = 0; i < n; i++)
        {
            string s;
            input >> s;
            string a, b;
            bool done = false;

            for (auto u : s)
            {
                if (u == ',')
                {
                    done = true;
                    continue;
                }
                if (done)
                    b.push_back(u);
                else
                    a.push_back(u);
            }

            Relations.push_back({a, b});
        }
        input.close();
    }
    else
    {
        cout << "Invalid File" << endl;
        input.close();
    }
    //-----------------------------------------------OutCast Input------------------------------------------------------
    input.open("4OutcastQueries.txt");
    if (input.is_open())
    {
        int n;
        input >> n;
        idx = 0;
        for (int i = 0; i < n; i++)
        {
            input >> line;
            string s;
            for (auto u : line)
            {
                if (u == ',')
                {
                    OutCast[idx].push_back(s);
                    s.clear();
                    continue;
                }
                else
                    s.push_back(u);
            }
            OutCast[idx].push_back(s);
            idx++;
        }
        input.close();
    }
    else
        input.close();
}

int solve(int node, int turn)
{
    int &ret = dp[node][turn];
    if (~ret)
        return ret;
    ret = 1e9;

    for (auto u : revadj[node])
        ret = min(ret, solve(u, turn) + 1);

    return ret;
}

pair<int, int> solveDP(string &x, string &y)
{
    memset(dp, -1, sizeof dp);
    vector<int> a = NounToIDs[x];
    vector<int> b = NounToIDs[y];

    for (auto u : a)
        dp[u][0] = 0;
    for (auto u : b)
        dp[u][1] = 0;

    solve(root, 0);
    solve(root, 1);

    int mnm = INT_MAX, idx, idxx = INT_MAX;
    for (int i = 0; i < n; i++)
        if (dp[i][0] < 1e9 && dp[i][1] < 1e9 && dp[i][0] >= 0 && dp[i][1] >= 0)
            if (mnm > dp[i][0] + dp[i][1])
                mnm = dp[i][0] + dp[i][1], idx = i, idxx = IDToNouns[i].size();
            else if (mnm == dp[i][0] + dp[i][1] && IDToNouns[i].size() < idxx)
                idx = i, idxx = IDToNouns[i].size();

    return {mnm, idx};
}

int main()
{
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

    for (auto u : Relations)
    {
        pair<int, int> ans = solveDP(u.first, u.second);
        cout << ans.first << ',';
        for (auto u : IDToNouns[ans.second])
            cout << u << ' ';
        cout << '\n';
    }

    for (auto u : OutCast)
    {
        if (u.empty())
            break;
        vector<int> dist(u.size());
        for (int i = 0; i < u.size(); i++)
        {
            for (int j = i + 1; j < u.size(); j++)
            {
                pair<int, int> ans = solveDP(u[i], u[j]);
                dist[i] += ans.first, dist[j] += ans.first;
            }
        }

        int mxm = INT_MIN, idx;
        for (int i = 0; i < u.size(); i++)
            if (dist[i] > mxm)
                mxm = dist[i], idx = i;

        cout << u[idx] << '\n';
    }
    return 0;
}