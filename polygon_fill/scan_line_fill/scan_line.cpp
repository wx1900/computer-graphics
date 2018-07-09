/* wangxu, 2018/03/19 */
#include <iostream>
#include <vector>
using namespace std;

struct Edge {
    float x = -1.;
    float d = -1.;
    int y = -1;
};

int main() {
    vector<vector<Edge>> NET; // vector<Edge> NET [100];
    int ymin = 100;
    int ymax = 0;
    float vtx[12] = {
        2,2,
        5,1,
        11,3,
        11,8,
        5,5,
        2,7
    };
    int graph[12][12] = {
        0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0
    };
    
    for (int i = 1; i < 12; i+=2) {
        if (ymin > vtx[i]) ymin = vtx[i];
        else if (ymax < vtx[i]) ymax = vtx[i];
    } 
    // printf("%d, %d\n", ymin, ymax);
    // initialize NET
    for (int i = ymin; i <= ymax; i++) {
        vector<Edge> edges;
        NET.push_back(edges);
    }
   
    for (int i = 0; i < 6; i++) {
        float x0 = vtx[(2*i)%12];
        int y0 = vtx[(2*i+1)%12];
        float x1 = vtx[(2*i+2)%12];
        float y1 = vtx[(2*i+3)%12];
        if (y1 < y0) {
            float tmp = y1;
            y1 = y0;
            y0 = tmp;
            tmp = x0;
            x0 = x1;
            x1 = tmp;
        }
        float d;
        if (y0 != y1) {
            if (x0 == x1) d = 0.;
            else d = (x0-x1)/(y0-y1);
            // printf("x=%f, d=%f, y=%f\n", x0, d, y1);
            Edge edge;
            edge.x = x0;
            edge.d = d;
            edge.y = y1;
            NET.at(y0).push_back(edge);
        }
    }
   
    vector<Edge> AET;
    for (int i = ymin; i < ymax; i++) {
        // printf("edge i=%d aet.size=%d\n", i, AET.size());
        // printf("net.size %d\n", NET.at(i).size());
        if (NET.at(i).size() > 0) {
            // printf("net.size=%d\n", NET.at(i).size());
            for (int j = 0; j < NET.at(i).size(); j++) {
                Edge edge;
                edge.x = NET.at(i).at(j).x;
                edge.d = NET.at(i).at(j).d;
                edge.y = NET.at(i).at(j).y;
                // printf("net: %f, %f, %d\n", edge.x, edge.d, edge.y);
                if (AET.size() == 0) {
                    AET.push_back(edge);
                    // printf("aet:%f, edge:%f\n", AET.at(0).x, edge.x);
                } else {
                    // printf("AET SIZE=%d\n", AET.size());
                    int k = AET.size()-1;
                    int insert = 0;
                    vector<Edge>::iterator pE = AET.end();
                    for (; k >= 0; k--) {
                        if (AET.at(k).x < edge.x) {
                            AET.insert(pE, edge);
                            // printf("insert: aet:%f,%f\n", AET.at(k+1).d, edge.d);
                            insert = 1;
                            break;
                            // printf("insert: aet:%f,%f\n", AET.at(k+1).d, edge.d);
                        } else if (AET.at(k).x == edge.x) {
                            // x相同时, 把 d<0 的边放在前面
                            if (AET.at(k).d > edge.d) {
                                AET.insert(pE-1, edge);
                            } else {
                                AET.insert(pE,edge);
                            }
                            insert = 1;
                            break;
                        } 
                        pE--;
                    }
                    if (insert == 0) {
                        AET.insert(pE, edge);
                    }
                }
            }
        }
        
        if (AET.size() > 0) {
            // for (int j = 0; j < AET.size(); j++)
                // printf("AET: %f, %f, %d\n",AET.at(j).x, AET.at(j).d, AET.at(j).y);
            float x0;
            float x1;
            for (int j = 0; j < AET.size()/2; j++) {
                x0 = AET.at(2*j).x;
                x1 = AET.at(2*j+1).x;
                for (int k = x0; k <= x1; k++) {
                    graph[i][k] = 1;
                }
                // printf("**** x0:%f, y0:%d, x1:%f, y1:%d\n",x0, i, x1, i);
            }
            if (AET.size()%2 == 1) {
                x0 = AET.at(AET.size()-1).x;
                for (int k = x1; k <= x0; k++) {
                    graph[i][k] = 1;
                }
                // printf("**** x0:%f, y0:%d, x1:%f, y1:%d\n",x1, i, x0, i);
            }
        }
       
        for (int j = 0; j < AET.size(); j++) {
            if (AET.at(j).y == i) {
                // printf("AET.erase(%d) %d\n", AET.begin()+j, AET.at(j).y);
                AET.erase(AET.begin()+j);
            } else {
                AET.at(j).x = AET.at(j).x + AET.at(j).d;
                // 不能一边修改一边排序,会改变vector顺序,影响修改的代码;
            }
        }
    }

    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            printf("%d", graph[i][j]);
        }
        printf("\n");
    }


    
}