#include <bits/stdc++.h>
using namespace std;

int optNum;
typedef unsigned long long ULL;

void w_add(vector<double> &w1, vector<double> &w2) {
    // if (w2.size() != 10) puts("w2");
    // if (w1.size() != 10) puts("s1");
    for (int i = 0; i < min(w1.size(), w2.size()); i++) w1[i] += w2[i];
}
struct Node {          //一个Node即为合法的一条路径
    vector<int> path;  //记录路径上经过的点
    vector<double> w;  //记录路径上各优化指标的值
    ULL hashCode;      //用来唯一标志一条路径
    Node() {
        hashCode = 0;
        w.resize(10, 0);
    }
    Node(vector<int> p) : path(p), hashCode(0) { w.resize(10, 0); }
    Node(vector<int> p, vector<double> w) : path(p), w(w), hashCode(0) {}
    ULL getHash() {
        if (hashCode != 0) return hashCode;
        hashCode = 1;
        for (int i = 0; i < path.size(); i++) {
            hashCode *= (path[i] + 1)*(i+1);
            hashCode += path[i] + 1;
        }
        return hashCode;
    }
    //重载<, 判断两条路径上的指标是否构成支配关系
    bool operator<(const Node &rhs) {
        int num = 0;
        for (int i = 0; i < optNum; i++) {
            // cout << w[i] << " " << rhs.w[i] << endl;
            if (abs(w[i] - rhs.w[i]) < 1e-6) num++;
        }
        if (num == optNum) return false;
        for (int i = 0; i < optNum; i++) {
            if (w[i] < rhs.w[i]) {
                return false;
            }
        }
        return true;
    }
};

bool cmp(const Node & a, const Node & b) {
    int cnt1 = 0, cnt2 = 0;
    for (int i = 0; i < optNum; i++) {
        if (abs(a.w[i] - b.w[i]) < 1e-6) continue;
        if (a.w[i] < b.w[i]) cnt1++;
        else if (a.w[i] > b.w[i]) cnt2++;
    }
    if ((cnt1 == 0 && cnt2 == 0) || cnt1) return false;
    return true;
}

int main() {
    optNum = 7;
    int n = 995;
    freopen("val.txt", "r", stdin);
    vector<Node> v;
    for (int i = 0; i < n; i++) {
        vector<double> w;
        vector<int> path;
        w.clear();
        for (int i = 0; i < n; i++) {
            double t;
            cin >> t;
            w.push_back(t);
        }
        v.push_back(Node(path, w));
    }
    // sort(v.begin(), v.end(), cmp);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (cmp(v[i], v[j])) {
                cout << i << " " << j << endl;
            }
        }
    }
    // freopen("test.txt", "w", stdout);
    // for (int i = 0; i < 1150; i++) {
    //     for (int j = 0; j < optNum; j++) {
    //         cout << v[i].w[j] << " ";
    //     }
    //     cout << endl;
    // }
    return 0;
}