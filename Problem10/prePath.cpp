#include <bits/stdc++.h>
using namespace std;

#define x first
#define y second

typedef unsigned long long ULL;
typedef pair<int, int> PII;
int n, m;    // n行m列的原图
int optNum;  //优化目标的数量

int g[100][100];         // 用于存储原始输入01矩阵
double f[100][100][10];  // fk[i][j][k] 用于记录点(i, j)的第k个指标
int keyGraph[200][200];  // 记录关键点的图，其中点(i, j)代表的是编号为i的关键点和编号为j的关键点之间是否一步可达
map<PII, int> hashc;   // 保存结点和结点编号的映射关系
vector<PII> keyPoint;  // 保存所有关键点的vector
PII start, goal;
int st_idx, goal_idx;  //起始节点的编号, 目标节点的编号

struct edge {
    PII st, ed;
    // int dist = 0;
    // double weight = 0;
    vector<double> w;  // w存储各个优化目标的值
    edge() { w.resize(10, 0); }
};
vector<vector<edge>> edges;

void readData() {
    freopen("Problem_10.txt", "r", stdin);
    cin >> n >> m;
    int num = 0;
    for (int i = 1; i <= n; i++)
        for (int j = 1; j <= m; j++) {
            cin >> g[i][j];
            if (g[i][j] == 0) num++;
        }
    cout << num << endl;
    cin.clear();
    freopen("F_10.txt", "r", stdin);
    int t;
    cin >> t >> optNum;
    cout << t << " " << optNum << endl;
    while (t--) {
        double x, y, weight;
        cin >> y >> x;
        f[int(x)][int(y)][0] = 1;
        for (int i = 1; i < optNum; i++) {
            cin >> weight;
            f[int(x)][int(y)][i] = weight;
        }
    }
    cin.clear();
    freopen("START_GOAL_10.txt", "r", stdin);
    cin >> start.y >> start.x;
    cin >> goal.y >> goal.x;
}
int check(int x, int y) {
    int dx[] = {0, 1};   // 竖直方向
    int dy[] = {-1, 0};  // 水平方向
    int factor[] = {1, -1};
    int res = 0;
    for (int k = 0; k < 2; ++k) {
        int part = 0;
        for (int t = 0; t < 2; ++t) {
            int nx = x + factor[t] * dx[k];
            int ny = y + factor[t] * dy[k];
            if (nx <= 0 || nx > n || ny <= 0 || ny > m) continue;
            part = part || (1 - g[nx][ny]);
        }
        res += part;
    }
    return res;
}
void findKeyPoint() {
    for (int i = 1; i <= n; ++i)
        for (int j = 1; j <= m; ++j) {
            if (g[i][j] == 0 &&
                check(i, j) ==
                    2)  // 一个可行结点在垂直和水平方向上都有相邻结点，该点才能是一个拐点
                keyPoint.push_back({i, j});
        }
}
bool cmp(pair<int, int> a, pair<int, int> b) {
    if (a.second != b.second) return a.second < b.second;
    return a.first < b.first;
}
bool checkAdj(PII a, PII b) {
    int dx = b.x - a.x;
    int dy = b.y - a.y;
    if (dx > 1) dx = 1;
    else if (dx < -1) dx = -1;
    if (dy > 1) dy = 1;
    else if (dy < -1) dy = -1;
    for (int x = a.x, y = a.y; (x != b.x || y != b.y); x += dx, y += dy) {
        if (g[x][y]!= 0) return false; 
    }
    return true;
}

void graph_init() {
    readData();
    findKeyPoint();
    cout << keyPoint.size() << endl;
    // cout<<"--------按行坐标排序后的点--------"<<endl;
    for (int i = 0; i < keyPoint.size(); ++i) {
        hashc[keyPoint[i]] = i;  // 给每个KeyPoint结点编号, 因为后面要排序, 会使得顺序被打乱
                // cout<<keyPoint[i].first<<" "<<keyPoint[i].second<<endl;
    }
    for (int i = 0; i < keyPoint.size(); ++i) {
        int j = i - 1;
        if (j >= 0 && (keyPoint[i].first == keyPoint[j].first &&
                       keyPoint[i].second > keyPoint[j].second) && checkAdj(keyPoint[i], keyPoint[j])) {
            keyGraph[i][j] = 1;
            keyGraph[j][i] = 1;
        }
        j = i + 1;
        if (j < keyPoint.size() && (keyPoint[i].first == keyPoint[j].first &&
                                    keyPoint[i].second < keyPoint[j].second) && checkAdj(keyPoint[i], keyPoint[j])) {
            keyGraph[i][j] = 1;
            keyGraph[j][i] = 1;
        }
    }
    sort(keyPoint.begin(), keyPoint.end(),
         cmp);  // pair<int, int> 先按行坐标排序，再按列坐标排序,
                // 这样可以一步到达的两个拐点一定是相邻的
    for (int i = 0; i < keyPoint.size(); ++i) {
        int j = i - 1;
        if (j >= 0 && (keyPoint[i].second == keyPoint[j].second &&
                       keyPoint[i].first > keyPoint[j].first) && checkAdj(keyPoint[i], keyPoint[j])) {
            int index_i = hashc[keyPoint[i]];
            int index_j = hashc[keyPoint[j]];
            keyGraph[index_i][index_j] = 1;
            keyGraph[index_j][index_i] = 1;
        }
        j = i + 1;
        if (j < keyPoint.size() && (keyPoint[i].second == keyPoint[j].second &&
                                    keyPoint[i].first < keyPoint[j].first) && checkAdj(keyPoint[i], keyPoint[j])) {
            int index_i = hashc[keyPoint[i]];
            int index_j = hashc[keyPoint[j]];
            keyGraph[index_i][index_j] = 1;
            keyGraph[index_j][index_i] = 1;
        }
    }
    sort(keyPoint.begin(), keyPoint.end());
    edges.resize(keyPoint.size());  // edges[][] 是一个二维矩阵, 二维矩阵的大小:
                                    // 关键点的个数 * 关键点的个数
    for (int i = 0; i < keyPoint.size(); ++i) {
        edges[i].resize(keyPoint.size());
        for (int j = 0; j < i; ++j) {
            if (keyGraph[i][j]) {
                edge e;
                if (keyPoint[i].first ==
                    keyPoint[j].first) {  // 这条边是横着的边
                    int minn = min(keyPoint[i].second, keyPoint[j].second);
                    int maxx = max(keyPoint[i].second, keyPoint[j].second);
                    for (int k = minn; k <= maxx; ++k) {
                        for (int a = 0; a < optNum; a++)
                            e.w[a] += f[keyPoint[i].first][k][a];
                    }
                    e.st = keyPoint[i];  // 因为是无向边，因此st,ed是可以相互转换的
                    e.ed = keyPoint[j];
                } else if (keyPoint[i].second ==
                           keyPoint[j].second) {  // 这条边是垂直的边
                    int minn = min(keyPoint[i].first, keyPoint[j].first);
                    int maxx = max(keyPoint[i].first, keyPoint[j].first);
                    for (int k = minn; k <= maxx; ++k) {
                        for (int a = 0; a < optNum; a++)
                            e.w[a] += f[k][keyPoint[i].second][a];
                    }
                    e.st = keyPoint[i];
                    e.ed = keyPoint[j];
                }
                edge e1 = e, e2 = e;
                swap(e2.st, e2.ed);
                for (int i = 0; i < optNum; i++) {
                    e1.w[i] -= f[e1.st.x][e1.st.y][i];
                    e2.w[i] -= f[e2.st.x][e2.st.y][i];
                }
                edges[i][j] = e1;
                edges[j][i] = e2;
            }
        }
    }
    for (int i = 0; i < keyPoint.size(); i++) {
        if (keyPoint[i] == start)
            st_idx = i;
        else if (keyPoint[i] == goal)
            goal_idx = i;
    }
}

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
    void calcW() {
        w.resize(10, 0);
        for (int i = 1; i < path.size(); i++) {
            int u = path[i - 1], v = path[i];
            w_add(w, edges[u][v].w);
        }
    }
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
        int cnt1 = 0, cnt2 = 0;
        for (int i = 0; i < optNum; i++) {
            if (abs(w[i] - rhs.w[i]) < 1e-6) continue;
            if (w[i] < rhs.w[i]) cnt1++;
            else if (w[i] > rhs.w[i]) cnt2++;
        }
        if ((cnt1 == 0 && cnt2 == 0) || cnt1) return false;
        return true;
    }
};
vector<Node> path_set[200][200];  // path_set[i][j]存储节点编号i和j之间能通过的合法路径
void path_init() {
    queue<Node> q;
    for (int i = 0; i < keyPoint.size(); i++) {
        Node t;
        t.path.push_back(i);
        q.push(t);
    }
    //迭代7次, 找到所有7步以内可以直达的路径
    for (int i = 0; i < 7; i++) {
        cout << i << endl;
        int len = q.size();
        while (len--) {
            Node t = q.front();
            q.pop();
            auto path = t.path;
            auto w = t.w;
            int u = path.back();
            for (int v = 0; v < keyPoint.size(); v++) {
                // u和v可以达到, 并且v不在路径中
                if (keyGraph[u][v] && !count(path.begin(), path.end(), v)) {
                    edge &e = edges[u][v];
                    path.push_back(v);
                    vector<double> tw(w);
                    w_add(tw, e.w);
                    Node newNode = Node(path, tw);
                    path_set[path.front()][v].push_back(newNode);
                    q.push(newNode);
                    path.pop_back();
                }
            }
        }
    }
}

//生成初始种群, sz为所要生成种群的大小
vector<Node> pop_init(int sz) {
    vector<Node> pathToGoal;
    queue<Node> q;
    Node st;
    st.path.push_back(st_idx);
    q.push(st);
    while (q.size() && pathToGoal.size() < sz) {
        Node t = q.front();
        q.pop();
        auto path = t.path;
        int u = path.back();
        for (int v = 0; v < keyPoint.size(); v++) {
            // u和v可以达到, 并且v不在路径中
            if (keyGraph[u][v] && !count(path.begin(), path.end(), v)) {
                edge &e = edges[u][v];
                path.push_back(v);
                Node newNode = Node(path);
                newNode.calcW();
                if (v == goal_idx) pathToGoal.push_back(newNode);
                q.push(newNode);
                path.pop_back();
            }
        }
    }
    puts("endGEn");
    cout << pathToGoal.size() << endl;
    return pathToGoal;
}

//检查路径是否合法
bool checkPath(vector<int> &path) {
    set<int> s;
    s.insert(path[0]);
    for (int i = 1; i < path.size(); i++) {
        s.insert(path[i]);
        //路径中相邻两点不可达
        if (!keyGraph[path[i - 1]][path[i]]) return false;
    }
    //路径中经过重复的点
    if (s.size() != path.size()) return false;
    return true;
}