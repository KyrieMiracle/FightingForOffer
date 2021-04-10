#define MAX_VERTEX_NUM 20
const int infinty = INT_MAX;
struct ArcCell
{
	int adj;         //对无权图有1、0表示是否相邻，对带权图，则为权值类型
	char *info;      //该弧的相关信息
};

struct MGraph
{
	string vexs[MAX_VERTEX_NUM];                  //顶点表
	ArcCell arcs[MAX_VERTEX_NUM][MAX_VERTEX_NUM]; //邻接矩阵，即边表
	int vexnum;   //顶点数
	int arcnum;   //边数
	int kind;     //邻接矩阵存储的图的种类
};

class Graph
{
	MGraph m_Graph;
	bool visited[MAX_VERTEX_NUM];
public:
	Graph();
	~Graph();
	int LocateVex(string u);
	bool CreateUDG();
	void DFSTraversal();
	void BFSTraversal();
};

int Graph::LocateVex(string u)
{
	for(int i = 0; i < MAX_VERTEX_NUM; i++){
		if(u == m_Graph.vexs[i])  return i;
	}
	return -1;
}

bool Graph::CreateUDG()
{
	int i,j;
	string v1, v2;
	cout<<"请输入无向图的顶点个数，边的个数： "<<endl;
	cin>>m_Graph.vexnum>>m_Graph.arcnum;
	cout<<"请输入各个顶点："
	for(i = 0; i < m_Graph.vexnum; i++){  //构造顶点向量
		cin>>m_Graph.vexs[i];
	}
	for(i = 0; i < m_Graph.vexnum; i++){
		for(j = 0; j < m_Graph.vexnum; j++){
			m_Graph.arcs[i][j].adj = 0;
			m_Graph.arcs[i][j].info = false;
		}
	}
	for(i = 0; i < m_Graph.arcnum; i++){  //构造邻接矩阵
		cout<<"请输入一条边依附的两个顶点"<<endl;
		cin>>v1>>v2;
		int m = LocateVex(v1);
		int n = LocateVex(v2);
		m_Graph.arcs[m][n].adj = 1;
		m_Graph.arcs[n][m].adj = 1;
	}
	m_Graph.kind = 2;
	return true;
}

void Graph::DFSTraversal(int v)
{
	cout<<m_Graph.vexs[v]<<endl;
	visited[v] = 1;
	for(int i = 0; i < m_Graph.vexnum; i++){
		if(m_Graph.arcs[v][i] == 1 && visited[i] == 0){
			DFSTraversal(i);
		}
	}
}




struct ArcNode  //定义边结点
{
	int adjvex;
	ArcNode* next;
};

struct VertexNode //定义表结点
{
	int vertex;
	ArcNode* firstedge;
};