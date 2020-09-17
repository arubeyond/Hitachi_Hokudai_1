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
using namespace chrono;
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

inline int string_to_int(string s)
{
    int ans = 0;
    rep(i, s.size())
    {
        ans *= 10;
        ans += (int)((char)s[i] - '0');
    }
    return ans;
}

string int_to_string(int i)
{
    if (i == 0)
        return "00";
    vector<char> moji;
    while (i > 0)
    {
        moji.push_back((char)(i % 10 + (int)'0'));
        i /= 10;
    }
    string ans = "";
    repr(s, moji.size() - 1, -1) ans += moji[s];
    if (ans.size() < 2)
        return "0" + ans;
    return ans;
}

void read_csv(string path, vector<int> &output){
    ifstream ifs(path);
    string str_buf;
    string str_conma_buf;
    int idx = 0;
    while (getline(ifs, str_buf))
    {
        istringstream i_stream(str_buf);
        while (getline(i_stream, str_conma_buf, ','))
        {
            output[idx] = (string_to_int(str_conma_buf));
            idx++;
        }
    }
    ifs.close();
}

ld calc_value(int t, vector<ld> hav){
    ld cons = T * T - t * t;
    return (hav[0] * 2 * (ld)t - hav[1] + cons * hav[2]);
}

ll CALC_MAIN(string path, ld time_d_rate, ld store_custo_rate, ld stay_rate,ld zero_value_rate)
{

    auto startClock = system_clock::now();
    if (debug){
        cout << "CALC_MAIN start at : "
             << duration_cast<microseconds>(startClock - startClock).count() * 1e-6
             << ENDL;
    }

    ll score = 0;

    //input
    vector<int> input_data(32500);
    if(debug){
        read_csv(path, input_data);
    }
    else
    {
        int idx;
        cin >> input_data[0] >> input_data[1];
        rep(i, input_data[1]) cin >> input_data[3 * i + 2] >> input_data[3 * i + 3] >> input_data[3 * i + 4];
        cin >> input_data[(3 * input_data[1] + 2)];
        idx = 3 * input_data[1] + 3;
        rep(t,T){
            cin >> input_data[idx];
            idx++;
            if (input_data[idx]){
                cin >> input_data[idx] >> input_data[idx+1];
                idx += 2;
            }
        }
    }

    if (debug)
    {
        cout << "input end at : "
             << duration_cast<microseconds>(system_clock::now() - startClock).count() * 1e-6
             << ENDL;
    }

    //initialize
    int V, E;
    V = input_data[0];
    E = input_data[1];
    vector<multimap<int, int>> edge(V);
    rep(i,E){
        int u, v, d;
        u = input_data[3 * i + 2] - 1;
        v = input_data[3 * i + 3] - 1;
        d = input_data[3 * i + 4];
        edge[u].insert(mp(v, d));
        edge[v].insert(mp(u, d));
    }
    //距離の計算
    vector<vector<int>> dist(V, vector<int>(V, infi));
    rep(i,V){
        //頂点iからのダイクストラ
        multiset<pair<int, int>> search;
        dist[i][i] = 0;
        search.insert(mp(0, i));
        while (true){
            if (search.empty())
                break;
            int now = (*search.begin()).S;
            search.erase(search.begin());
            for (auto itr = edge[now].begin(); itr != edge[now].end();itr++){
                int nex = (*itr).F;
                int d = (*itr).S;
                if (dist[i][now]+d<dist[i][nex]){
                    dist[i][nex] = dist[i][now] + d;
                    search.insert(mp(dist[i][nex], nex));
                }
            }
        }
    }

    if (debug)
    {
        cout << "init and calc dist end at : "
             << duration_cast<microseconds>(system_clock::now() - startClock).count() * 1e-6
             << ENDL;
    }

    //main_treatment
    vector<int> now(3, 0);  //いま頂点[0]と[1]の間で、[0]から[2]進んだところにいる
    int bef = 0; //最後に店舗0を訪れた時刻
    vector<int> order_id(T, -1);  //時刻[t]に出現した注文はorder_id[t]というIDを持つ。なければ-1
    vector<set<int>> order(V);  //頂点iにある注文の注文時刻
    vector<vector<ld>> have(V,vector<ld>(3,0));  
    //荷物を既に積んでいる注文の、注文時刻の和と平方和と個数の距離減衰したもの
    //（各頂点）（店舗はすべての和と平方和）
    vector<vector<ld>> nhave(V,vector<ld>(3,0)); 
    //荷物をまだ詰んでいない注文の同上
    vector<int> ans(T, -1);
    int idx = 3 * E + 3;
    //main_loop
    rep(i, V) dist[i][i] = zero_value_rate;
    rep(t,T){
        //時刻tからt+1の処理をおこなう
        //注文を受け取る
        int ord_num = input_data[idx];
        idx++;
        if (ord_num){
            order_id[t] = input_data[idx];
            idx++;
            int target = input_data[idx];
            idx++;
            order[target].insert(t);
            rep(i,V)
            {
                nhave[i][0] += (ld)t / (ld)dist[target][i];
                nhave[i][1] += (ld)(t * t) / (ld)dist[target][i];
                nhave[i][2] += (ld)1.0 / (ld)dist[target][i];
                //距離について反比例にしたけど改良の余地あり
            }
        }

        //商品を積む
        if(now[0]==now[1] && now[0]==0){
            rep(i,V){
                have[i][0] += nhave[i][0];
                have[i][1] += nhave[i][1];
                have[i][2] += nhave[i][2];
                nhave[i][0] = 0;
                nhave[i][1] = 0;
                nhave[i][2] = 0;
            }
            bef = t;
        }

        //行動の決定
        int u, v, d;
        u = now[0];
        v = now[1];
        d = now[2];
        if (d){
            //uからvに向かっている辺の上にいる
            //uまたはvが店舗の場合の計算が必要
            int dd =(*(edge[u].find(v))).S - d;
            ld vu = calc_value(t, nhave[0]) * (store_custo_rate) / (ld)(d + dist[u][0]);
            ld vv = calc_value(t, nhave[0]) * (store_custo_rate) / (ld)(dd + dist[v][0]);
            vv += calc_value(t, have[v]) / (ld)dd;
            if (vu > vv){
                //uに引き返す
                ans[t] = u;
                now[0] = v;
                now[1] = u;
                now[2] = dd + 1;
            }
            else{
                //予定通りvに向かう
                ans[t] = v;
                now[2]++;
            }

        }
        else if (u == 0 && calc_value(t,have[0])<=stay_rate)
        {
                ans[t] = -1;
            //店舗にstayするかどうかを決定する
            //have[0]の大きさと、時間1待った場合得られる注文の期待値で決まる
        }
        else{
            //stayせずに移動する
            //いまいるuから1つ移動した場合の価値（have[nex])
            //次の頂点から店舗への距離と積んでいない商品の数(nhave[0]*dist[nex][0])
            //この2つで向かう頂点を決める
            ld mx = -infi;
            ld v0 = calc_value(t, nhave[0]) * (store_custo_rate);
            for (auto itr = edge[u].begin(); itr != edge[u].end();itr++){
                int nex = (*itr).F;
                ld vnex = v0 / (ld)(dist[nex][u] + dist[nex][0]) + calc_value(t, have[nex]) / (ld)(dist[nex][u]);
                if(vnex>mx){
                    ans[t] = nex;
                    mx = vnex;
                }
            }
            now[0] = u;
            now[1] = ans[t];
            now[2] = 1;
        }

        //移動結果の処理
        //配達完了処理とスコアの集計
        u = now[0];
        v = now[1];
        d = now[2];
        if ((*(edge[u].find(v))).S == d)
        {
            //頂点ｖに到着しているかの判定と処理
            now[0] = v;
            now[2] = 0;
        }
        int z = 0;
        if(now[0] != 0 && now[2] == 0){
            //頂点u=vにちょうどいるとき、配達完了処理をする
            ld x = 0;
            ld y = 0;
            //スコアの集計と各頂点に与えている影響を計算し、have[i]から/dist[i][v]をかけて引く
            //<=befの注文を処理
            auto itr = order[v].end();
            while(true){
                itr--;
                if((*itr)>bef)
                    continue;

                x += (*itr);
                y += (*itr) * (*itr);
                z += 1;
                score += pow(T, 2) - pow(t - (*itr), 2);
                order[v].erase(itr);

                if(itr==order[v].begin())
                    break;
            }
            rep(i,V)
            {
                have[i][0] += (ld)x / (ld)dist[v][i];
                have[i][1] += (ld)y / (ld)dist[v][i];
                have[i][2] += (ld)z / (ld)dist[v][i];
            }
        }
    
        //z : 時刻t+1に完了した注文の数
    }
    return 1;
}

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    const string path = "./HH_1/test_A/input_";
    int n = 1;
    string problem = "A";
    rep(i, n)
    {
        ld s_c_rate = 1;
        ld stay_rate = 0;
        ld z_v_rate = 0.25;
        //s_c_rate：顧客に対して店舗を重視する割合。1より大きいと店舗重視
        //1よりちいさい値推奨[0,1]。1より小さいと、受注がある程度貯まるまでは配達を行う
        //stay_rate : 注文価値がこれ未満のときstayする。受注した注文があまりにも少ないとstayする
        string path_in = path + int_to_string(i) + ".csv";
        cout << CALC_MAIN(path_in, 1, s_c_rate,stay_rate, z_v_rate) << ENDL;
    }

}