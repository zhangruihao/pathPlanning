#include <bits/stdc++.h>

#include "prePath.cpp"
using namespace std;
#pragma GCC optimize(3, "Ofast", "inline")

#define random(x) rand() % (x)

typedef pair<double, int> PDI;

double randInOne() {
    return rand() / (double)RAND_MAX;
}

//遗传算法
class GA {
  private:
    int pop_size;          //种群数量
    double cross_rate;     //交叉概率
    double mutation_rate;  //变异概率
    unordered_set<ULL> s;  //种群个体的hashCode的集合,用于去除重复个体
  public:
    vector<Node> pop;    //族群
    vector<Node> child;  //本次种群的后代
    GA(vector<Node> p, int pop_size, double cross_rate, double mutation_rate) {
        this->pop_size = pop_size;
        this->pop = p;
        this->cross_rate = cross_rate;
        this->mutation_rate = mutation_rate;
    }
    //非支配排序, 返回应保留个体在pop中的下标
    vector<int> NSGA_II() {
        int lower[pop.size()];  // lower[i]表示第i个个体劣于其他个体的数量
        int rank[pop.size()];   // rank[i], 种群第i个个体所在支配集层数
        set<int> dominate[pop.size()];// dominata[i]表示pop[i]支配的个体
        for (int i = 0; i < pop.size(); i++) {
            dominate[i].clear();
            lower[i] = 0;
            for (int j = 0; j < pop.size(); j++)
                if (pop[i] < pop[j]) lower[i]++;
                else if (pop[j] < pop[i]) dominate[i].insert(j);
        }
        int rk = 1;
        queue<int> q;
        for (int i = 0; i < pop.size(); i++) {
            if (lower[i] == 0) {
                q.push(i);
                rank[i] = rk;
            }
        }
        while (q.size()) {
            queue<int> p;
            while (q.size()) {
                int u = q.front();
                q.pop();
                for (auto &x : dominate[u]) {
                    lower[x]--;
                    if (lower[x] == 0) {
                        p.push(x);
                        rank[x] = rk + 1;
                    }
                }
            }
            q = p;
            rk++;
        }
        map<int, vector<int>> mp;  //保存每一层支配集的个体
        for (int i = 0; i < pop.size(); i++) mp[rank[i]].push_back(i);
        vector<int> survival;  //被保留的个体在pop中的下标
        vector<int> lastSeq; //最后一层非支配集合
        for (auto& it : mp) {
            if (survival.size() + it.second.size() <= pop_size)
                survival.insert(survival.end(), it.second.begin(),
                                it.second.end());
            else {
                lastSeq = it.second;
                break;
            }
        }
        //对最后一层非支配集合, 根据拥塞度选取个体
        if (lastSeq.size()) {
            PDI dist[lastSeq.size()];
            for (int i = 0; i < lastSeq.size(); i++)
                dist[i].x = 0, dist[i].y = lastSeq[i];
            for (int i = 0; i < optNum; i++) {
                PDI d[lastSeq.size()];
                for (int j = 0; j < lastSeq.size(); j++)
                    d[i].x = pop[lastSeq[i]].w[i], d[i].y = i;
                sort(d, d + lastSeq.size());
                //保留第一个和最后一个
                dist[d[0].y].x += 1e9;
                dist[d[lastSeq.size() - 1].y].x += 1e9;
                double minv = d[0].x, maxv = d[lastSeq.size() - 1].x;
                for (int i = 1; i < lastSeq.size() - 1; i++) {
                    dist[d[i].y].x += (d[i+1].x - d[i-1].x)/(maxv - minv);
                }
            }
            sort(dist, dist + lastSeq.size());
            reverse(dist, dist + lastSeq.size());
            int numAdd = pop_size - survival.size();
            for (int i = 0; i < numAdd; i++)
                survival.push_back(dist[i].second);
        }
        return survival;
    }
    //选择下一代种群
    void select() {
        pop.insert(pop.end(), child.begin(),
                   child.end());  //合并当前种群和所产生的子代
        vector<Node> new_pop;     //新种群
        vector<int> survival = NSGA_II();
        for (int i = 0; i < survival.size(); i++)
            new_pop.push_back(pop[survival[i]]);
        pop = new_pop;
    }
    //交叉配对
    void crossOver() {
        int len = pop.size();
        for (int i = 0; i < pop_size; i++) {  //种群内交叉配对的次数
            //从种群中随机选出进行交叉配对的双亲
            int idx_fa = random(len);
            int idx_ma = random(len);
            if (idx_fa == idx_ma)
                continue;  //双亲为同一个个体, 则放弃此次交叉配对
            Node fa = pop[idx_fa], ma = pop[idx_ma];

            //获取双亲可以进行交叉配对的位置
            int cnt[keyPoint.size()];
            memset(cnt, 0, sizeof(cnt));
            PII record[keyPoint.size()];
            for (int i = 0; i < fa.path.size(); i++) {
                cnt[fa.path[i]]++;
                record[fa.path[i]].first = i;
            }
            for (int i = 0; i < ma.path.size(); i++) {
                cnt[ma.path[i]]++;
                record[ma.path[i]].second = i;
            }

            for (int i = 0; i < keyPoint.size(); i++) {
                if (i == st_idx || i == goal_idx)
                    continue;  //不对路径的起点和终点位置进行交叉配对
                if (cnt[i] != 2)
                    continue;  //双亲的路径中必须经过当前点才能进行交叉配对
                vector<int> newPath;
                newPath.insert(newPath.end(), fa.path.begin(),
                               fa.path.begin() + record[i].first);
                newPath.insert(newPath.end(),
                               ma.path.begin() + record[i].second,
                               ma.path.end());
                if (checkPath(newPath)) {
                    if (randInOne() > cross_rate) continue;
                    Node newNode = Node(newPath);
                    ULL hashCode = newNode.getHash();
                    if (s.count(hashCode)) continue;
                    s.insert(hashCode);
                    newNode.calcW();
                    child.push_back(newNode);
                }
            }
        }
    }
    //对个体进行变异, 向下一代child中添加变异个体
    void mutate(Node node) {
        int len = node.path.size();
        int idx_l = random(len), idx_r = random(len); //变异区间len[idx_l~idx_r];
        if (idx_l == idx_r) return; //变异区间长度为0
        if (idx_l > idx_r) swap(idx_l, idx_r);
        int u = node.path[idx_l], v = node.path[idx_r];
        auto& p_set = path_set[u][v];
        if (p_set.size() == 0) return;
        int sz = p_set.size();
        for (int i = 0; i < sz; i++) {
            int idx = random(sz);
            vector<int> path;
            //组成新的路径
            path.insert(path.end(), node.path.begin(),
                        node.path.begin() + idx_l);
            path.insert(path.end(), p_set[idx].path.begin(),
                        p_set[idx].path.end());
            path.insert(path.end(), node.path.begin() + idx_r + 1,
                        node.path.end());
            //路径是否合法
            if (checkPath(path)) {
                if (randInOne() > mutation_rate) continue;
                Node newNode = Node(path);
                ULL hashCode = newNode.getHash();
                if (s.count(hashCode)) continue;//此个体已经在当前种群和子代中出现
                s.insert(hashCode);
                newNode.calcW();
                child.push_back(newNode);
                break;
            }
        }
    }
    //族群进化
    void evolve() {
        child.clear();
        s.clear();
        int len = pop.size();
        for (int i = 0; i < len; i++) s.insert(pop[i].getHash());
        for (int i = 0; i < len; i++) {
            mutate(pop[i]);  //对个体进行变异
        }
        crossOver();
        select();
    }
    void outputResult(int mode = 0) {
        ofstream fs1, fs2;
        if (mode) {
            fs1.open("path.txt", ios::app);
            fs2.open("val.txt", ios::app);
        } else {
            fs1.open("path.txt");
            fs2.open("val.txt");
        }
        int lower[pop_size];
        memset(lower, 0, sizeof(lower));
        for (int i = 0; i < pop.size(); i++) {
            for (int j = 0; j < pop.size(); j++) {
                if (pop[i] < pop[j]) lower[i]++;
            }
        }
        int cnt = 0;
        for (int i = 0; i < pop_size; i++) {
            if (lower[i] == 0) cnt++;
        }
        cout << cnt << endl;
        fs1 << cnt << endl;

        for (int i = 0; i < pop.size(); i++) {
            if (lower[i]) continue;
            pop[i].calcW();
            pop[i].calcW();
            vector<PII> p;
            if(!checkPath(pop[i].path)) {
                puts("Error");
            }
            for (auto & it : pop[i].path)
                p.push_back({keyPoint[it].second, keyPoint[it].first});
            vector<PII> path;
            for (int i = 1; i < p.size(); i++) {
                int dx = p[i].x - p[i - 1].x;
                if (dx > 1) dx = 1;
                else if (dx < -1) dx = -1;
                int dy = p[i].y - p[i - 1].y;
                if (dy > 1) dy = 1;
                else if (dy < - 1) dy = -1;
                
                bool isFirst = true;
                for (int x = p[i - 1].x, y = p[i - 1].y; (x != p[i].x || y != p[i].y) ;x +=dx, y += dy) {
                    path.push_back({x, y});
                }
            }
            path.push_back({keyPoint[goal_idx].y, keyPoint[goal_idx].x});
            for (int i = 1; i < path.size(); i++) {
                if (abs(path[i].x - path[i - 1].x) + abs(path[i].y - path[i - 1].y) != 1 || g[path[i].y][path[i].x] != 0) {
                    puts("Error");
                    break;
                }
            }
            fs1 << path.size() << endl;
            for (auto & it:path) {
                fs1 << it.x << " " << it.y << endl;
            }
            for (int j = 0; j < optNum; j++)
                fs2 << pop[i].w[j] + f[start.x][start.y][j] << " ";
            fs2 << endl;
        }
        fs1.close();
        fs2.close();
    }
};

void init() {
    srand((int)time(0));  // 产生随机种子
    graph_init();
    path_init();
}

int main() {
    clock_t st;
    st = clock();
    init();
    vector<Node> pop = pop_init(1000);  //生成个体数量为100的种群
    GA ga = GA(pop, 1000, 1.0, 1.0);
    int generation = 100;  //迭代次数
    for (int i = 0; i <= generation; i++) {
        cout << i << endl;
        ga.evolve();
    }
    ga.outputResult();
    puts("end");
    cout << double(clock() - st)/CLOCKS_PER_SEC << endl; 
    return 0;
}