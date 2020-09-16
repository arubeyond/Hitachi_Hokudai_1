#include <bits/stdc++.h>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <iostream>
#include <bitset>
#include <cassert>
#include <queue>
#include <stack>
#include <iomanip>
#include <math.h>
#include <time.h>
#include <chrono>
#include <random>
using namespace std;
#define rep(i, n) for (int i = 0; i < (int)n; i++)
#define repf(i, a, b) for (int i = (int)a; i < (int)b; i++)
#define repr(i, a, b) for (int i = (int)a; i > (int)b; i--)
#define all(v) (v).begin(), (v).end()
#define mp(a, b) make_pair(a, b)
#define eb(x) emplace_back(x)
#define F first
#define S second
typedef long long ll;
typedef long double ld;
typedef pair<int, int> pii;
typedef pair<ll, ll> pll;
typedef pair<ld, ld> pdd;
typedef vector<ll> vll;
typedef vector<vll> vvll;
typedef vector<int> vii;
typedef vector<vii> vvii;
const ll mod = 1e9 + 7;
const int infi = 1147483600;
const ll infl = 4e18 + 5;
const char ENDL = '\n';
//cout << fixed << setprecision(17) << res << endl;
const ll MOD = 998244353;

bool debug = true;

const ll T = 10000;
const ll MAX_V = 400;
const ll MIN_V = 200;
const int MAX_Degree = 5;
const ll MAX_DIST = 5000;

static uint32_t randxor()
{
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123;
    uint32_t t;
    t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}
random_device rnd;
mt19937 mt11(rnd());
static double randxor01()
{
    return (randxor() + 0.5) * (1.0 / UINT_MAX);
}

ld calc_dist(pdd a,pdd b){
    return sqrt(pow(a.F - b.F, 2) + pow(a.S - b.S, 2));
}

void MAKE_TREE(int V, int E, vvii &edge, vector<pdd> coo, vector<int> col){
    vector<vector<int>> used(V, vector<int>(V, 1));
    //最小全域木を作る
    vector<int> connect(V, 0);
    multimap<ld, pair<int, int>> near_edge;
    connect[0] = 1;
    repf(i,1,V)
        near_edge.insert(mp(calc_dist(coo[0], coo[i]), mp(0, i)));
    rep(i,V-1){
        ld w = (*near_edge.begin()).F;
        int u = (*near_edge.begin()).S.F;
        int v = (*near_edge.begin()).S.S;
        int uv = u;
        if (used[u])
            uv = v;
        edge[i][0] = u;
        edge[i][1] = v;
        edge[i][2] = (int)ceil(2 * w + 1);
        used[u][v] = 0;
        used[v][u] = 0;
        connect[u] = 1;
        connect[v] = 1;
        rep(j, uv) if (connect[j] ^ 1) near_edge.insert(mp(calc_dist(coo[uv], coo[j]), mp(j, uv)));
        repf(j, uv + 1, V) if (connect[j] ^ 1) near_edge.insert(mp(calc_dist(coo[uv], coo[j]), mp(uv, j)));
        auto itr = near_edge.end();
        while(itr!=near_edge.begin()){
            itr--;
            if (connect[(*itr).S.F]&connect[(*itr).S.S])
                near_edge.erase(itr);
        }
    }

    //残りの辺を作る
    vector<int> degree(V, 0);
    rep(i,V-1){
        degree[edge[i][0]]++;
        degree[edge[i][1]]++;
    }
    repf(i,V-1,E){
        multimap<ld, pair<int, int>> cost;
        rep(u,V-1){repf(v,u+1,V){
            if(used[u][v]){
                int f = 1 + 4 * (col[u] == col[v]);
                int g = (1 + MAX_DIST * (degree[u] >= MAX_Degree)) * (1 + MAX_DIST * (degree[v] >= MAX_Degree));
                cost.insert(mp(calc_dist(coo[u], coo[v]) * degree[u] * degree[v] * f * g));
            }}}

        ld w = (*cost.begin()).F;
        int u = (*cost.begin()).S.F;
        int v = (*cost.begin()).S.S;
        edge[i][0] = u;
        edge[i][1] = v;
        edge[i][2] = (int)ceil(4 * w + 1);
        used[u][v] = 0;
        used[v][u] = 0;
        degree[u]++;
        degree[v]++;
    }

}

void make(str path,str problem){
    int V, E;
    int  R = 0;
    //V,E,Rの決定
    V = MIN_V + randxor() % (MAX_V - MIN_V);
    E = 3 * V / 2 + randxor() % (V / 2);
    vector<vector<int>> edge(E,vector<int>(3,0));
    while(true){
        R++;
        if(R*R>V){
            R--;
            break;
        }
    }
    int r = V-(R*R);
    
    multiset<int> leave;
    rep(i, V) leave.insert(i);
    vector<pair<ld,ld>> coo(V);
    vector<int> col(V);
    //座標を作る（色も）
    rep(x,R){
        rep(y,R){
            auto itr = leave.begin();
            rep(i, randxor() % (V - x * R + y)) itr++;
            coo[(*itr)] = mp(x + randxor01(), y + randxor01());
            col[(*itr)] = (x + y) % 2;
            leave.erase(itr);
        }
    }
    rep(i,r){
        auto itr = leave.begin();
        rep(j, randxor() % (V - R * R - i)) itr++;
        coo[(*itr)] = mp(randxor01() * R, randxor01() * R);
        col[(*itr)] = (randxor01() > 0.5);
        leave.erase(itr);
    }
    MAKE_TREE(V, E, edge, coo, col);
    
    //注文頻度、注文を作る
    vector<int> freq(V, 1);
    freq[0] = 0;
    pair<ld, ld> cent;
    cent = mp((ld)R / 4.0 + randxor01() * ((ld)R / 2.0), (ld)R / 4.0 + randxor01() * ((ld)R / 2.0));
    repf(i,1,V){
        if (calc_dist(cent, coo[i]) <= ((ld)R / 8.0) * (1.0 + randxor01()))freq[i] = 2;
        freq[i] += freq[i-1]
    }
    vector<int> Ord(T, -1);
    int T_last = 9500;
    ld T_peak = randxor01() * T_last;
    rep(t,T_last){
        if (t<T_peak){
            if (randxor01() < ((ld)t/T_peak))
                Ord[t] = (upper_bound(all(freq), randxor() % ((ll)freq[V - 1])) - freq.begin());
        }   
        else{
            if (randxor01() < ((ld)(T_last-t)/(ld)(Tlast-T_peak)))
                Ord[t] = (upper_bound(all(freq), randxor() % ((ll)freq[V - 1])) - freq.begin());
        }
    }

    //csvへの書き込み

    ofstream ofs(path);
    if (problem=="A"){
        ofs << V << "," << E << endl;
        rep(i, E) ofs << edge[i][0] - 1 << "," << edge[i][1] - 1 << "," << edge[i][2] << endl;
        ofs << T << endl;
        int ord_id = 1;
        rep(t,T){
            ofs << (Ord[t] >= 0) << endl;
            if (Ord[t]>=0){
                ofs << ord_id << "," << Ord[t] + 1;
                ord_id++;
            }
        }
    }
    else if (problem=="B"){}
    ofs.close();
}


/*auto startClock = system_clock::now();

const double time = duration_cast<microseconds>
		(system_clock::now() - startClock).count() * 1e-6

system_clock::now() が現在のクロック
差.count()　でクロック数
duration_cast<microseconds>　がマイクロ秒で変換
*/

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    const string path = "../HH_1/test_A/input";
    int n = 1;
    problem = "A";
    rep(i,n)
    {
        path_in = path + (char)i + ".csv";
        make(path_in, problem);
    }
    
}