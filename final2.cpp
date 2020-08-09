#include<iostream>
#include<fstream>
#include<string.h>
#include<stdio.h>
#include<windows.h>
#include<stdlib.h>
#include<conio.h>
#include<math.h>
#include<limits.h>
#include<ctime>
//#include<graphics.h>
#include<conio.h>
using namespace std;
COORD coord={0,0};
void gotoxy(int x,int y)        //function that moves the cursor to the specified x and y coordinates
{
    coord.X=x;
    coord.Y=y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
}
struct Driver       //structure used for writing and reading driver information
{
    char name[15];
    char gender;
    char veh_no[10];
    int x,y;
};
struct acc      //structure used for writing and reading user information
{
    char id[15];
    char password[6];
}a;
struct map_     //structure used for writing and reading every place's information
{
    int n;
    char place[50];
    int mx,my;
};
struct AdjListNode  // A structure to represent a node in adjacency list
{
    int dest;
    int distance;
    struct AdjListNode* next;
};
struct AdjList  // A structure to represent an adjacency liat
{
    struct AdjListNode *head; // pointer to head node of list
};
struct Graph    // A structure to represent a graph. A graph is an array of adjacency lists.
{
    int V;
    struct AdjList* array;
};
struct AdjListNode* newAdjListNode(int dest, int distance)    // A utility function to create a new adjacency list node
{
    struct AdjListNode* newNode =(struct AdjListNode*) malloc(sizeof(struct AdjListNode));
    newNode->dest = dest;
    newNode->distance = distance;
    newNode->next = NULL;
    return newNode;
}
struct Graph* createGraph(int V)    // A utility function that creates a graph of V vertices
{
    struct Graph* graph = (struct Graph*) malloc(sizeof(struct Graph));
    graph->V = V;
    graph->array = (struct AdjList*) malloc(V * sizeof(struct AdjList));
    for (int i = 0; i < V; ++i)
        graph->array[i].head = NULL;
    return graph;
}
void addEdge(struct Graph* graph, int src, int dest, int distance)    // Adds an edge to an undirected graph
{
    struct AdjListNode* newNode = newAdjListNode(dest, distance);
    newNode->next = graph->array[src].head;
    graph->array[src].head = newNode;
    newNode = newAdjListNode(src, distance);
    newNode->next = graph->array[dest].head;
    graph->array[dest].head = newNode;
}
struct MinHeapNode  // Structure to represent a min heap node
{
    int v;
    int dist;
};
struct MinHeap      // Structure to represent a min heap
{
    int size;     // Number of heap nodes present currently
    int capacity;   // Capacity of min heap
    int *pos;     // This is needed for decreaseKey()
    struct MinHeapNode **array;
};
struct MinHeapNode* newMinHeapNode(int v, int dist)     // A utility function to create a new Min Heap Node
{
    struct MinHeapNode* minHeapNode =(struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    minHeapNode->v = v;
    minHeapNode->dist = dist;
    return minHeapNode;
}
struct MinHeap* createMinHeap(int capacity)     // A utility function to create a Min Heap
{
    struct MinHeap* minHeap=(struct MinHeap*) malloc(sizeof(struct MinHeap));
    minHeap->pos = (int *)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array =(struct MinHeapNode**) malloc(capacity * sizeof(struct MinHeapNode*));
    return minHeap;
}
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b)    // A utility function to swap two nodes of min heap. Needed for min heapify
{
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}
void minHeapify(struct MinHeap* minHeap, int idx)
{
    int smallest, left, right;
    smallest = idx;
    left = 2 * idx + 1;
    right = 2 * idx + 2;
    if(left<minHeap->size && minHeap->array[left]->dist<minHeap->array[smallest]->dist)
        smallest = left;
    if(right<minHeap->size && minHeap->array[right]->dist<minHeap->array[smallest]->dist)
        smallest = right;
    if (smallest != idx)
    {
        MinHeapNode *smallestNode = minHeap->array[smallest];
        MinHeapNode *idxNode = minHeap->array[idx];
        minHeap->pos[smallestNode->v] = idx;
        minHeap->pos[idxNode->v] = smallest;
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}
int isEmpty(struct MinHeap* minHeap)
{
    return minHeap->size == 0;
}
struct MinHeapNode* extractMin(struct MinHeap* minHeap)
{
    if (isEmpty(minHeap))
        return NULL;
    struct MinHeapNode* root = minHeap->array[0];
    struct MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;
    minHeap->pos[root->v] = minHeap->size-1;
    minHeap->pos[lastNode->v] = 0;
    --minHeap->size;
    minHeapify(minHeap, 0);
    return root;
}
void decreaseKey(struct MinHeap* minHeap, int v, int dist)
{
    int i = minHeap->pos[v];
    minHeap->array[i]->dist = dist;
    while (i && minHeap->array[i]->dist < minHeap->array[(i - 1) / 2]->dist)
    {
        minHeap->pos[minHeap->array[i]->v] = (i-1)/2;
        minHeap->pos[minHeap->array[(i-1)/2]->v] = i;
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}
bool isInMinHeap(struct MinHeap *minHeap, int v)
{
    if (minHeap->pos[v] < minHeap->size)
        return true;
    return false;
}
void printPath(int arr[], int j)
{
    if (arr[j]==-1)
        return;
    printPath(arr,arr[j]);
    cout<<"->"<<j+1;
}
int shortest(struct Graph* graph, int src, int target)
{
    int V = graph->V;// Get the number of vertices in graph
    int dist[V]; // dist values used to pick minimum distance edge
    int arr[V];
    //int t=0;
    struct MinHeap* minHeap = createMinHeap(V);
    for (int v = 0; v < V; ++v)
    {
        dist[v] = INT_MAX;
        minHeap->array[v] = newMinHeapNode(v, dist[v]);
        minHeap->pos[v] = v;
    }
    arr[src]=-1;
    minHeap->array[src] = newMinHeapNode(src, dist[src]);
    minHeap->pos[src] = src;
    dist[src] = 0;
    decreaseKey(minHeap, src, dist[src]);
    minHeap->size = V;
    while (!isEmpty(minHeap))
    {
        struct MinHeapNode* minHeapNode = extractMin(minHeap);
        int u = minHeapNode->v; // Store the extracted vertex number
        struct AdjListNode* temp = graph->array[u].head;
        if(u==target)
        {
            gotoxy(85,34);cout<<"SHORTEST PATH:"<<src+1;printPath(arr,u);
            return dist[u];
        }
        while (temp != NULL)
        {
            int v = temp->dest;
            if (isInMinHeap(minHeap, v)&&dist[u]!=INT_MAX&&temp->distance + dist[u] < dist[v])
            {
                arr[v]=u;
                dist[v] = dist[u] + temp->distance;
                decreaseKey(minHeap, v, dist[v]);
            }
            temp = temp->next;
        }
    }
    return 0;
}
void sample_map()       //Function to generate the map on the screen and generates a legend
{
    gotoxy(70,5);cout<<"1";
    gotoxy(35,8);cout<<"3";
    gotoxy(57,8);cout<<"2";
    gotoxy(10,10);cout<<"4";
    gotoxy(65,11);cout<<"7";
    gotoxy(15,15);cout<<"5";
    gotoxy(49,15);cout<<"6";
    gotoxy(60,20);cout<<"8";
    gotoxy(35,20);cout<<"9";
    gotoxy(43,22);cout<<"12";
    gotoxy(65,25);cout<<"13";
    gotoxy(40,25);cout<<"11";
    gotoxy(25,27);cout<<"10";
    gotoxy(50,31);cout<<"14";
    gotoxy(65,35);cout<<"15";
    gotoxy(90,7);cout<<"LEGEND";
    gotoxy(85,9);cout<<"1.Le Meridian";
    gotoxy(85,10);cout<<"2.KMCH";
    gotoxy(85,11);cout<<"3.Canford International School";
    gotoxy(85,12);cout<<"4.Marudhamalai Temple";
    gotoxy(85,13);cout<<"5.Adhyayana Public School";
    gotoxy(85,14);cout<<"6.PSG College of Technology";
    gotoxy(85,15);cout<<"7.Airport";
    gotoxy(85,16);cout<<"8.Singanallur";
    gotoxy(85,17);cout<<"9.Brookesfield Mall";
    gotoxy(85,18);cout<<"10.Shirdi SaiBaba Temple";
    gotoxy(85,19);cout<<"11.Railway Station";
    gotoxy(85,20);cout<<"12.Nehru Stadium";
    gotoxy(85,21);cout<<"13.Police Station";
    gotoxy(85,22);cout<<"14.Arasan Cinemas";
    gotoxy(85,23);cout<<"15.Coimbatore Golf Club";
}
int shortest_dist_graph(int init,int fin)
{
    int V = 15;
    struct Graph* graph = createGraph(V);
    addEdge(graph,0,1,5);
    addEdge(graph,0,6,6);
    addEdge(graph,1,2,12);
    addEdge(graph,1,5,5);
    addEdge(graph,1,6,1);
    addEdge(graph,2,3,13);
    addEdge(graph,2,4,12);
    addEdge(graph,2,8,8);
    addEdge(graph,2,5,7);
    addEdge(graph,3,4,5);
    addEdge(graph,4,5,17);
    addEdge(graph,4,8,12);
    addEdge(graph,4,9,15);
    addEdge(graph,5,6,5);
    addEdge(graph,5,8,6);
    addEdge(graph,5,7,6);
    addEdge(graph,5,11,5);
    addEdge(graph,6,7,7);
    addEdge(graph,6,12,6);
    addEdge(graph,7,11,9);
    addEdge(graph,7,12,2);
    addEdge(graph,8,9,6);
    addEdge(graph,8,10,2);
    addEdge(graph,8,11,2);
    addEdge(graph,9,10,5);
    addEdge(graph,9,13,10);
    addEdge(graph,10,13,8);
    addEdge(graph,10,11,1);
    addEdge(graph,11,13,7);
    addEdge(graph,11,12,9);
    addEdge(graph,11,13,7);
    addEdge(graph,11,14,17);
    addEdge(graph,12,14,12);
    addEdge(graph,13,14,9);
    return (shortest(graph,init,fin));
}
void check_date(int& date,int& month,int& year)     //validates the entered date
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int d,m,y;
    d=ltm->tm_mday;
    m=1+ltm->tm_mon;
    //y=ltm->tm_year;
    y=1900 + ltm->tm_year;
    //cout<<"Date: "<<d<<" Month: "<<m<<" Year: "<<y;
    while(1)
    {
        if(year<y)
        {
            gotoxy(85,40);cout<<"WRONG ENTRY ..";
            gotoxy(85,41);cout<<"RE-ENTER DATE, MONTH, YEAR:";
            cin>>date>>month>>year;
        }
        else if(year>y)
        {
            gotoxy(85,40);cout<<"The year seems to be too far, its preferable to put a better, immediate date";
            gotoxy(85,41);cout<<"RE-ENTER DATE, MONTH, YEAR:";
            cin>>date>>month>>year;
        }
        else if(month<m)
        {
            gotoxy(85,40);cout<<"WRONG ENTRY ..";
            gotoxy(85,41);cout<<"RE-ENTER DATE, MONTH, YEAR:";
            cin>>date>>month>>year;
        }
        else if(month==m)
        {
            if(date<d)
            {
                gotoxy(85,40);cout<<"WRONG ENTRY ..";
                gotoxy(85,41);cout<<"RE-ENTER DATE, MONTH, YEAR:";
                cin>>date>>month>>year;
            }
            else
                break;
        }
        else
            break;
    }
}
void check_time(int& m_h,int& m_m,char *t)      //validates the entered time
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    int h,m;
    h=ltm->tm_hour;
    m=ltm->tm_min;
    while(1)
    {
    if(strcmp(t,"am")==0)
    {
            if(h>m_h)
            {
                gotoxy(85,42);cout<<"WRONG ENTRY ..";
                gotoxy(85,43);cout<<"RE-ENTER TIME:";
                cin>>m_h>>m_m;
                cin>>t;
            }
            if(m>m_m)
            {
                h=h-1;
                m=m+60;
                if(h<m_h)
                {
                    gotoxy(85,42);cout<<"WRONG ENTRY ..";
                    gotoxy(85,43);cout<<"RE-ENTER TIME:";
                    cin>>m_h>>m_m;
                    cin>>t;
                }
            }
            else
            {
                break;
            }
        }
    if(strcmp(t,"pm")==0)
    {
            m_h=m_h+12;
            if(h>m_h)
            {
                gotoxy(85,42);cout<<"WRONG ENTRY ..";
                gotoxy(85,43);cout<<"RE-ENTER TIME:";
                cin>>m_h>>m_m;
                cin>>t;
            }
            else if(h==m_h)
            {
                if(m>m_m)
                {
                    gotoxy(85,42);cout<<"WRONG ENTRY ..";
                    gotoxy(85,43);cout<<"RE-ENTER TIME:";
                    cin>>m_h>>m_m;
                    cin>>t;
                }
                else
                {
                    break;
                }
            }
            else if(h<m_h)
            {
                break;
            }
            else
            {
                break;
            }
        }
    }
    if(m_h>12)
        m_h=m_h-12;
}
void driver_gen()       //Generates the driver details and stores into file
{
    Driver d[10];
    ofstream f1,f2;
    f1.open("Male_driver.txt",ios::out);
    f2.open("Female_driver.txt",ios::out);
    strcpy(d[0].name,"Maanikam");
    d[0].gender='M';
    strcpy(d[0].veh_no,"TN38SS1119");
    d[0].x=65;
    d[0].y=6;
    strcpy(d[1].name,"Neelambari");
    d[1].gender='F';
    strcpy(d[1].veh_no,"TN38ED0621");
    d[1].x=68;
    d[1].y=9;
    strcpy(d[2].name,"Basha");
    d[2].gender='M';
    strcpy(d[2].veh_no,"TN38BU1009");
    d[2].x=55;
    d[2].y=13;
    strcpy(d[3].name,"Arunachalam");
    d[3].gender='M';
    strcpy(d[3].veh_no,"TN38MS1909");
    d[3].x=20;
    d[3].y=9;
    strcpy(d[4].name,"Sandhya");
    d[4].gender='F';
    strcpy(d[4].veh_no,"TN38SS1519");
    d[4].x=45;
    d[4].y=8;
    strcpy(d[5].name,"Chithra");
    d[5].gender='F';
    strcpy(d[5].veh_no,"TN38YZ1112");
    d[5].x=43;
    d[5].y=17;
    strcpy(d[6].name,"Bheem");
    d[6].gender='M';
    strcpy(d[6].veh_no,"TN38BU1009");
    d[6].x=25;
    d[6].y=18;
    strcpy(d[7].name,"Jerry");
    d[7].gender='M';
    strcpy(d[7].veh_no,"TN38TJ0410");
    d[7].x=57;
    d[7].y=33;
    strcpy(d[8].name,"Dora");
    d[8].gender='F';
    strcpy(d[8].veh_no,"TN38BD8005");
    d[8].x=52;
    d[8].y=21;
    strcpy(d[9].name,"Jessica");
    d[9].gender='F';
    strcpy(d[9].veh_no,"TN38JV1234");
    d[9].x=32;
    d[9].y=26;
    for(int i=0;i<10;i++)
    {
        if(d[i].gender=='M')
            f1.write(reinterpret_cast<char*>(&d[i]),sizeof(d[i]));
        else
            f2.write(reinterpret_cast<char*>(&d[i]),sizeof(d[i]));
    }
    f1.close();
    f2.close();
}
void map_file()     //Generates places information and stores into file
{
    ofstream fout;
    fout.open("map.txt",ios::out);
    map_ ptr[15];
    ptr[0].n=1;
    strcpy(ptr[0].place,"Le Meridian");
    ptr[0].mx=70;
    ptr[0].my=5;
    fout.write(reinterpret_cast<char *>(&ptr[0]),sizeof(ptr[0]));
    ptr[1].n=2;
    strcpy(ptr[1].place,"KMCH");
    ptr[1].mx=57;
    ptr[1].my=8;
    fout.write(reinterpret_cast<char *>(&ptr[1]),sizeof(ptr[1]));
    ptr[2].n=3;
    strcpy(ptr[2].place,"Canford International School");
    ptr[2].mx=35;
    ptr[2].my=8;
    fout.write(reinterpret_cast<char *>(&ptr[2]),sizeof(ptr[2]));
    ptr[3].n=4;
    strcpy(ptr[3].place,"Marudhamalai Temple");
    ptr[3].mx=10;
    ptr[3].my=10;
    fout.write(reinterpret_cast<char *>(&ptr[3]),sizeof(ptr[3]));
    ptr[4].n=5;
    strcpy(ptr[4].place,"Adhyayana Public school");
    ptr[4].mx=15;
    ptr[4].my=15;
    fout.write(reinterpret_cast<char *>(&ptr[4]),sizeof(ptr[4]));
    ptr[5].n=6;
    strcpy(ptr[5].place,"PSG College of Technology");
    ptr[5].mx=49;
    ptr[5].my=15;
    fout.write(reinterpret_cast<char *>(&ptr[5]),sizeof(ptr[5]));
    ptr[6].n=7;
    strcpy(ptr[6].place,"Airport");
    ptr[6].mx=65;
    ptr[6].my=11;
    fout.write(reinterpret_cast<char *>(&ptr[6]),sizeof(ptr[6]));
    ptr[7].n=8;
    strcpy(ptr[7].place,"Singanallur");
    ptr[7].mx=60;
    ptr[7].my=20;
    fout.write(reinterpret_cast<char *>(&ptr[7]),sizeof(ptr[7]));
    ptr[8].n=9;
    strcpy(ptr[8].place,"Brookesfield");
    ptr[8].mx=35;
    ptr[8].my=20;
    fout.write(reinterpret_cast<char *>(&ptr[8]),sizeof(ptr[8]));
    ptr[9].n=10;
    strcpy(ptr[9].place,"Shiridi Saibaba Temple");
    ptr[9].mx=25;
    ptr[9].my=27;
    fout.write(reinterpret_cast<char *>(&ptr[9]),sizeof(ptr[9]));
    ptr[10].n=11;
    strcpy(ptr[10].place,"Railway Station");
    ptr[10].mx=40;
    ptr[10].my=25;
    fout.write(reinterpret_cast<char *>(&ptr[10]),sizeof(ptr[10]));
    ptr[11].n=12;
    strcpy(ptr[11].place,"Nehru Stadium");
    ptr[11].mx=43;
    ptr[11].my=22;
    fout.write(reinterpret_cast<char *>(&ptr[11]),sizeof(ptr[11]));
    ptr[12].n=13;
    strcpy(ptr[12].place,"Police station");
    ptr[12].mx=65;
    ptr[12].my=25;
    fout.write(reinterpret_cast<char *>(&ptr[12]),sizeof(ptr[12]));
    ptr[13].n=14;
    strcpy(ptr[13].place,"Arasan Cinemas");
    ptr[13].mx=50;
    ptr[13].my=31;
    fout.write(reinterpret_cast<char *>(&ptr[13]),sizeof(ptr[13]));
    ptr[14].n=15;
    strcpy(ptr[14].place,"Coimbatore Golf Club");
    ptr[14].mx=65;
    ptr[14].my=35;
    fout.write(reinterpret_cast<char *>(&ptr[14]),sizeof(ptr[14]));
    fout<<"\n";
    fout.close();
}
/*void mainscreen()
{
    int gd = DETECT,gm,i;
    initgraph(&gd,&gm,"");
    setcolor(3);
    settextstyle(5,0,5);
    for(int i=0;i<55;i++)
    {
        outtextxy(i,50,"WELCOME TO SASY CABS");
        delay(50);
    }
    closegraph();
}
void show_car()
{
    int gd=DETECT,gm=0;
    int i,maxx,midy;
    initgraph(&gd,&gm,"");    //initialize graphic mode
    maxx=getmaxx();           //maximum pixel in horizontal axis
    midy=getmaxy()/2;         //maximum pixel in horizontal axis
    for(i=0;i<maxx-150;i=i+5)
    {
        cleardevice();         //clears screen
        setcolor(WHITE);       //draw a white road
        line(0,midy+37,maxx,midy+37);
        setcolor(YELLOW);
        setfillstyle(SOLID_FILL, RED);
        line(i, midy + 23, i, midy);
        line(i, midy, 40 + i, midy - 20);
        line(40 + i, midy - 20, 80 + i, midy - 20);
        line(80 + i, midy - 20, 100 + i, midy);
        line(100 + i, midy, 120 + i, midy);
        line(120 + i, midy, 120 + i, midy + 23);
        line(0 + i, midy + 23, 18 + i, midy + 23);
        arc(30 + i, midy + 23, 0, 180, 12);
        line(42 + i, midy + 23, 78 + i, midy + 23);
        arc(90 + i, midy + 23, 0, 180, 12);
        line(102 + i, midy + 23, 120 + i, midy + 23);
        line(28 + i, midy, 43 + i, midy - 15);
        line(43 + i, midy - 15, 57 + i, midy - 15);
        line(57 + i, midy - 15, 57 + i, midy);
        line(57 + i, midy, 28 + i, midy);
        line(62 + i, midy - 15, 77 + i, midy - 15);
        line(77 + i, midy - 15, 92 + i, midy);
        line(92 + i, midy, 62 + i, midy);
        line(62 + i, midy, 62 + i, midy - 15);
        floodfill(5 + i, midy + 22, YELLOW);
        setcolor(BLUE);
        setfillstyle(SOLID_FILL, DARKGRAY);
        circle(30 + i, midy + 25, 9);
        circle(90 + i, midy + 25, 9);
        floodfill(30 + i, midy + 25, BLUE);
        floodfill(90 + i, midy + 25, BLUE);
        delay(50);
    }
    closegraph();
}*/
char name[15],pwd[6];
int main()
{
    //mainscreen();
    ifstream fin1,fin;
    ofstream fout;
    int opt,day,month,year,h,m,n,short_dist,p=0,d=0,f1=0,cnt=0,ch,pick_x,pick_y;
    char gen,ch1,pick[50],drop[50],t_[3];
    cout<<"\n\n\n\t\t\t1.LOGIN USING EXISTING ACCOUNT";
    cout<<"\n\n\t\t\t2.CREATE A NEW ACCOUNT";
    cout<<"\n\n\n\tEnter your choice...";
    cin>>ch;
    system("cls");
    if(ch==1)
    {
        char u_id[15],pwd[7];
        int f=0;
        cout<<"\n\n\n\t\tUSER-ID:";
        getchar();
        cin.getline(u_id,15);
        cout<<"\n\n\t\tPASSWORD(length:6):";
        cin>>pwd;
        fin.open("login.txt",ios::in);
        while(!fin.eof())
        {
            fin>>a.id>>a.password;
            while(strcmp(a.id,u_id)==0)
            {
                if(strcmp(a.password,pwd)==0)
                {
                    cout<<"\n\n\n\t\tLogin Success!!";
                    f=1;
                    break;
                }
                else
                {
                    cnt++;
                    if(cnt>3)
                        exit(0);
                    cout<<"WRONG PASSWORD!!RE-ENTER....\n";
                    cin>>pwd;
                }
            }
            if(f)
                break;
        }
        if(f==0)
        {
            cout<<"\nUSER DOESNT EXIST \n \t\t\tCREATE A NEW ONE...";
            ch=2;
        }
        fin.close();
    }
    if(ch==2)
    {
        fout.open("login.txt",ios::out|ios::app);
        fin.open("login.txt",ios::in);
        int cnt=0;       //tracks to see if username is already used
        int my_c=0;      //counts the number of records in file
        cout<<"\n\n\n\n\t\t\tENTER USER-ID:";
        getchar();
        cin.getline(name,15);
        while(!fin.eof())
        {
            fin.read((char*)&a,sizeof(a));
            my_c++;
            if(strcmp(name,a.id)!=0)
                cnt++;
        }
        if(my_c==cnt)
            strcpy(a.id,name);
        else
        {
            cout<<"\n\n\n\t\t\tUSER NAME ALREADY TAKEN!";
            cout<<"\n\n\n\t\t\tRE-ENTER USER-ID:";
            getchar();
            cin.getline(name,15);
        }
        do
        {
            cout<<"\n\n\t\t\tENTER PASSWORD(length:6):";
            cin>>pwd;
        }while(strlen(pwd)!=6);
        strcpy(a.password,pwd);
        fout<<a.id<<" "<<a.password<<"\n";
        cout<<"\nACCOUNT CREATED SUCCESSFULLY!!";
        fout.close();
    }
    cout<<"\n\n\n\nPress any key to continue...";
    getch();
    system("cls");
    cout<<"\n\n\n\n\n\t\t\t\t\t\tMENU";
    cout<<"\n\n\n\t\t\t\t\t1.PRE BOOKING";
    cout<<"\n\n\t\t\t\t\t2.IMMEDIATE BOOKING";
    //cout<<"\n\n\t\t\t\t\t3.VIEW YOUR HISTORY";
    cout<<"\n\n\tEnter your option....";
    cin>>opt;
    system("cls");
    driver_gen();
    map_file();
    sample_map();
    ifstream f;
    switch(opt)
    {
        case 1:
                {gotoxy(85,28);cout<<"ENTER PICK UP LOCATION:";
                getchar();
                cin.getline(pick,50);
                ifstream fin2;
                map_ my;
                fin2.open("map.txt",ios::in);
                while(!fin2.eof())
                {
                    fin2.read((char*)&my,sizeof(my));
                    if(stricmp(my.place,pick)==0)
                    {
                        p=my.n;
                        fin2.close();
                    }
                }
                fin2.close();
                gotoxy(85,29);cout<<"ENTER DROP LOCATION:";
                cin.getline(drop,50);
                fin2.open("map.txt",ios::in);
                while(!fin2.eof())
                {
                    fin2.read((char*)&my,sizeof(my));
                    if(stricmp(my.place,drop)==0)
                    {
                        d=my.n;
                        fin2.close();
                    }
                }
                fin2.close();
                gotoxy(85,30);cout<<"ENTER DATE(dd mm yyyy):";
                cin>>day>>month>>year;
                check_date(day,month,year);
                gotoxy(85,31);cout<<"ENTER TIME(hh mm am/pm):";
                cin>>h>>m>>t_;
                check_time(h,m,t_);
                gotoxy(85,32);cout<<"ENTER THE NUMBER OF PASSENGERS:";
                cin>>n;
                gotoxy(85,33);cout<<"DRIVER GENDER PREFERENCE(M/F)?";
                cin>>gen;
                short_dist=shortest_dist_graph(p-1,d-1);
                gotoxy(85,35);cout<<"TOTAL APPROX. DISTANCE: "<<short_dist<<" Kms";
                gotoxy(85,36);cout<<"ESTIMATED AMOUNT:"<<short_dist*20;
                gotoxy(85,37);cout<<"CONFIRM BOOKING?(Y/N)";
                cin>>ch1;
                system("cls");
                if(ch1=='Y'||ch1=='y')
                {
                    Driver D;
                    f.open("map.txt",ios::in);
                    map_ ma;
                    while(!f.eof())
                    {
                        f.read(reinterpret_cast<char*>(&ma),sizeof(ma));
                            if(ma.n==p)
                        {
                            pick_x=ma.mx;
                            pick_y=ma.my;
                            break;
                        }
                    }
                    if(gen=='F' || gen=='f')
                    {
                        char f_driver[50];
                        char veh_num[10];
                        ifstream fin;
                        Driver d;
                        float dist[5];
                        int x_cord[5];
                        int f_d=0;
                        fin.open("Female_driver.txt",ios::in);
                        while(!fin.eof())
                        {
                            fin.read(reinterpret_cast<char*>(&d),sizeof(d));
                            x_cord[f_d]=d.x;
                            dist[f_d]=sqrt(pow((pick_x - d.x),2)+(pow((pick_y - d.y),2)));
                            f_d=f_d + 1;
                        }
                        f.close();
                        float min=dist[0];
                        int min_driver=x_cord[0];
                        for(int i=0;i<f_d;i++)
                        {
                            if(dist[i]<min)
                            {
                                min=dist[i];
                                min_driver=x_cord[i];
                            }
                        }
                        ifstream fin2;
                        fin2.open("Female_driver.txt",ios::in);
                        while(!fin2.eof())
                        {
                            fin2.read((char*)&d,sizeof(d));
                            if(d.x==min_driver)
                            {
                                strcpy(f_driver,d.name);
                                strcpy(veh_num,d.veh_no);
                                break;
                            }
                        }
                        fin2.close();
                        cout<<"\n\n\n\n\n\t\t\tDRIVER AVAILABLE..."<<"\n";
                        cout<<"\n\n\n\t\t\tDRIVER NAME: "<<f_driver;
                        cout<<"\n\n\t\t\tCAB NUMBER: "<<veh_num;
                        cout<<"\n\n\t\tBOOKING CONFIRMED:)";
                        cout<<"\n\n\tDRIVER WOULD BE THERE TO PICK YOU UP...";
                        //show_car();
                    }
                    else if(gen=='M' || gen=='m')
                    {
                        char driver[50];
                        char veh_num[10];
                        ifstream fin;
                        Driver d;
                        float dist[5];
                        int x_cord[5];
                        int f_d=0;
                        fin.open("Male_driver.txt",ios::in);
                        while(!fin.eof())
                        {
                            fin.read(reinterpret_cast<char*>(&d),sizeof(d));
                            x_cord[f_d]=d.x;
                            dist[f_d]=sqrt(pow((pick_x - d.x),2)+(pow((pick_y - d.y),2)));
                            f_d=f_d + 1;
                        }
                        f.close();
                        int min=dist[0];
                        int min_driver=x_cord[0];
                        for(int i=0;i<f_d;i++)
                        {
                            if(dist[i]<min)
                            {
                                min=dist[i];
                                min_driver=x_cord[i];
                            }
                        }
                        ifstream fin2;
                        fin2.open("Male_driver.txt",ios::in);
                        while(!fin2.eof())
                        {
                            fin2.read(reinterpret_cast<char*>(&d),sizeof(d));
                            if(d.x==min_driver)
                            {
                                strcpy(driver,d.name);
                                strcpy(veh_num,d.veh_no);
                                break;
                            }
                        }
                        fin2.close();
                        cout<<"\n\n\n\n\n\t\t\tDRIVER AVAILABLE..."<<"\n";
                        cout<<"\n\n\n\t\t\tDRIVER NAME: "<<driver;
                        cout<<"\n\n\t\t\tCAB NUMBER: "<<veh_num;
                        cout<<"\n\n\t\tBOOKING CONFIRMED:)";
                        cout<<"\n\n\tDRIVER WOULD BE THERE TO PICK YOU UP...";
                        //show_car();
                    }
                }
                }
                break;
        case 2:
                {gotoxy(85,28);cout<<"ENTER PICK UP LOCATION:";
                getchar();
                cin.getline(pick,50);
                ifstream fin2;
                map_ my;
                //int pick_=0;
                fin2.open("map.txt",ios::in);
                while(!fin2.eof())
                {
                    fin2.read((char*)&my,sizeof(my));
                    if(stricmp(my.place,pick)==0)
                    {
                        p=my.n;
                        fin2.close();
                    }
                }
                fin2.close();
                gotoxy(85,29);cout<<"ENTER DROP LOCATION:";
                cin.getline(drop,50);
                fin2.open("map.txt",ios::in);
                while(!fin2.eof())
                {
                    fin2.read((char*)&my,sizeof(my));
                    if(stricmp(my.place,drop)==0)
                    {
                        d=my.n;
                        fin2.close();
                    }
                }
                fin2.close();
                gotoxy(85,30);cout<<"ENTER TIME(hh mm am/pm):";
                cin>>h>>m>>t_;
                check_time(h,m,t_);
                gotoxy(85,31);cout<<"ENTER THE NUMBER OF PASSENGERS:";
                cin>>n;
                gotoxy(85,32);cout<<"DRIVER GENDER PREFERENCE(M/F)?";
                cin>>gen;
                short_dist=shortest_dist_graph(p-1,d-1);
                float arr_time;
                arr_time=float(short_dist/50.0);
                gotoxy(85,33);cout<<"TOTAL APPROX. DISTANCE: "<<short_dist<<" Kms";
                gotoxy(85,34);cout<<"TIME TAKEN TO REACH DESTINATION:"<<int(arr_time*60)<<" mins";
                gotoxy(85,35);cout<<"ESTIMATED AMOUNT:"<<short_dist*20;
                gotoxy(85,36);cout<<"CONFIRM BOOKING?(Y/N)";
                cin>>ch1;
                system("cls");
                if(ch1=='Y'||ch1=='y')
                {
                    Driver D;
                    f.open("map.txt",ios::in);
                    map_ ma;
                    int pick_x;
                    int pick_y;
                    while(!f.eof())
                    {
                        f.read(reinterpret_cast<char*>(&ma),sizeof(ma));
                        if(ma.n==p)
                        {
                            pick_x=ma.mx;
                            pick_y=ma.my;
                            break;
                        }
                    }
                    if(gen=='f' || gen=='F')
                    {
                        Driver d;
                        char f_driver[50];
                        char veh_num[10];
                        float dist[5];
                        int x_cord[5];
                        int f_d=0;
                        fin.open("Female_driver.txt",ios::in);
                        while(!fin.eof())
                        {
                            fin.read((char*)&d,sizeof(d));
                            x_cord[f_d]=d.x;
                            dist[f_d]=sqrt(pow((pick_x - d.x),2)+(pow((pick_y - d.y),2)));
                            f_d=f_d + 1;
                        }
                        f.close();
                        float min=dist[0];
                        int min_driver=x_cord[0];
                        for(int i=0;i<f_d;i++)
                        {
                            if(dist[i]<min)
                            {
                                min=dist[i];
                                min_driver=x_cord[i];
                            }
                        }
                        ifstream fin2;
                        fin2.open("Female_driver.txt",ios::in);
                        while(!fin2.eof())
                        {
                            fin2.read((char*)&d,sizeof(d));
                            if(d.x==min_driver)
                            {
                                strcpy(f_driver,d.name);
                                strcpy(veh_num,d.veh_no);
                                break;
                            }
                        }
                        fin2.close();
                        cout<<"\n\n\n\n\n\t\t\tDRIVER AVAILABLE..."<<"\n";
                        cout<<"\n\n\n\t\t\tDRIVER NAME: "<<f_driver;
                        cout<<"\n\n\t\t\tCAB NUMBER: "<<veh_num;
                        cout<<"\n\n\t\tBOOKING CONFIRMED:)";
                        cout<<"\n\n\tDRIVER ON THE WAY TO PICK YOU UP...";
                        //show_car();
                        break;
                    }
                    else if(gen=='M' || gen=='m')
                    {
                        char driver[50];
                        char veh_num[10];
                        ifstream fin;
                        Driver d;
                        float dist[5];
                        int x_cord[5];
                        int m_d=0;
                        fin.open("Male_driver.txt",ios::in);
                        while(!fin.eof())
                        {
                            fin.read(reinterpret_cast<char*>(&d),sizeof(d));
                            x_cord[m_d]=d.x;
                            dist[m_d]=sqrt(pow((pick_x - d.x),2)+(pow((pick_y - d.y),2)));
                            m_d=m_d + 1;
                        }
                        f.close();
                        int min=dist[0];
                        int min_driver=x_cord[0];
                        for(int i=0;i<m_d;i++)
                        {
                            if(dist[i]<min)
                            {
                                min=dist[i];
                                min_driver=x_cord[i];
                            }
                        }
                        ifstream fin2;
                        fin2.open("Male_driver.txt",ios::in);
                        while(!fin2.eof())
                        {
                            fin2.read(reinterpret_cast<char*>(&d),sizeof(d));
                            if(d.x==min_driver)
                            {
                                strcpy(driver,d.name);
                                strcpy(veh_num,d.veh_no);
                                break;
                            }
                        }
                        fin2.close();
                        cout<<"\n\n\n\n\n\t\t\tDRIVER AVAILABLE..."<<"\n";
                        cout<<"\n\n\n\t\t\tDRIVER NAME: "<<driver;
                        cout<<"\n\n\t\t\tCAB NUMBER: "<<veh_num;
                        cout<<"\n\n\t\tBOOKING CONFIRMED:)";
                        cout<<"\n\n\tDRIVER ON THE WAY TO PICK YOU UP...";
                        //show_car();
                        break;
                    }
                }
                }
    }
}
