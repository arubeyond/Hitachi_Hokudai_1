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

const int T = 10000;
const int MAX_V = 400;
const int MIN_V = 200;
const int MAX_Degree = 5;

void make(path){
    int V, E;
    int  R = 0;
    V = MIN_V + randxor() % (MAX_V - MIN_V);
    vector<vector<int>> edge;
    vector<vector<int>> edge_use(V,vector<int>(V,1));
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
    MAKE_TREE(V, edge, coo, col);
    
}

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
    const string path = "../Hitachi_Hokkaido_1/test_A/test";
    int n = 0;
    rep(i,n)
    {
        path_in = path + (char)i + "_in.in";
        make(path_in);
    }
    ofstream outputfile("test.out");
    outputfile << "test\n";
    outputfile.close();
    /*
    "https://atcoder.jp/contests/hokudai-hitachi2019-1"
    Hitachi Hokudai Labo & Hokkaido University Contest 2019-1
    での入力ファイル作成のプログラム（途中）と考察

    １．入力ファイル作成について
    ・make(path)で1ケース作るとする
    ・頂点数については記述がなかった（多分）ため、制約の中で一様分布とした
    ・V：頂点数、E：辺数（多分これもランダム）
    ・R、rの決定、座標の決定、ランダム化->coo(vi)=(x,y)
    これから
    ・MAKE_TREE：辺の構築。ユークリッド距離で最小全域木、残りのE-(V-1)本はまだ読んでない
    ・注文頻度の決定
    ・注文内容の決定
    ・書き込み

    ２．問題の考察
    まずスコアの計算について
    入力：移動クエリT=10^4
    店にいる場合：注文を受け取り、顧客スタックに注文時間を保存（注文スタック回）
    顧客にいる場合：顧客スタックだけ配達が完了し効用を計算する（顧客スタック回）
    厳密には顧客スタックと注文スタックは同じ配列で行える
    注文数に対して最大で2回の参照のため、計算量は注文数*2+T

    少なくとも店舗からすべての顧客への最短距離と最短ルートを計算する必要がある


    ３．問題A
    移動ルートを乱択し、改善する焼きなまし法を考える。
    移動ルートを事前に決定し変更するが、辺がない場合に注意
    ・あらかじめ最短ルートを計算しておいて
    向かうルートと通るルートを分ける。
    ・通るルート通りにクエリを生成して、Tmaxで終わる。

    店以外でstayする必要はないため、店ならば移動orstay
    顧客なら移動というように乱択する
    変更する場合は後ろすべてを乱択にする（前から決定する）

    30secあるのでかなりループできる

    ４．問題B
    Bでは全体への山登り法は使えず貪欲になる
    
    次の行動をしたとき、全体の利益が最大になるように移動する
    行動を決定する要素は
    ・注文スタックの数：多いほど店舗へ戻ろうとする
    ・移動先の顧客スタックの数：多いほど向かおうとする
    ・残り時間：いらないかも
    ・移動先の距離：遠いほど移動したがらない

    ？：辺の上で引き返す条件：とりあえず引き返さないとしてみる
    ？：店舗でstayする：注文数＝（顧客スタックの合計）が少ない場合。はじめはナシでもいいかも

    */
}