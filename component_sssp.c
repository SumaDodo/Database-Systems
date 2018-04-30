#include "graph.h"
#include "graph.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "config.h"
#include "tuple.h"
#include <fcntl.h>
#define IN 99
#define N 6

int dijkstra(int cost[][N], int source, int target);
void
component_neighbors(char *grdbdir, int gidx, int cidx, vertexid_t id)
{
	

	struct component c;
	int fd;
	char s[BUFSIZE];
	char gname[15];
	char cname[15];
	FILE *out;
	char *buf;
	int readlen;
	ssize_t len, size;
	off_t off;
	int with_tuples;


	/* Load enums */
	c.efd = enum_file_open(grdbdir, gidx, cidx);
	if (c.efd >= 0) {
		enum_list_init(&(c.el));
		enum_list_read(&(c.el), c.efd);
	}
	/* Load vertex schema */
	memset(s, 0, BUFSIZE);
	sprintf(s, "%s/%d/%d/sv", grdbdir, gidx, cidx);
	printf("Vertex Schema - %s/%d/%d/sv\n", grdbdir, gidx, cidx);
	fd = open(s, O_RDONLY);
	if (fd >= 0) {
		c.sv = schema_read(fd, c.el);
		close(fd);
	}
	/* Load edge schema */
	memset(s, 0, BUFSIZE);
	sprintf(s, "%s/%d/%d/se", grdbdir, gidx, cidx);
	fd = open(s, O_RDONLY);
	if (fd >= 0) {
		c.se = schema_read(fd, c.el);
		close(fd);
	}
	
	printf("Reading edge scheme, vertex schemas \n");

	sprintf(gname,"%d",gidx);
	sprintf(cname,"%d",cidx);

	printf("Gname - %s\n",gname);
	printf("Cname - %s\n",cname);

	/* Open vertex file */
	memset(s, 0, BUFSIZE);
	sprintf(s, "%s/%s/%s/v", grdbdir, gname, cname);
	c.vfd = open(s, O_RDWR);

	/* Open edge file */
	memset(s, 0, BUFSIZE);
	sprintf(s, "%s/%s/%s/e", grdbdir, gname, cname);
	c.efd = open(s, O_RDWR);

	with_tuples = 0;
//	component_print(out, &c, with_tuples);
	read_vertices_schema(&c, with_tuples);

	close(c.efd);
	close(c.vfd);
	return;
}

int
component_sssp(
        char *grdbdir,
        int gidx,
        int cidx,
        vertexid_t id1,
        vertexid_t id2,
        char *weight_field,
		component_t c, int with_tuples)
{
	off_t off;
	ssize_t len, size;
	vertexid_t id, id1, id2;
	struct tuple tuple;
	char *buf;
	int readlen;
	assert (c != NULL);
	int vertex_array[MAX_SIZE];
	int array_size = 0;

	printf("({");

	/* Vertices */
	if (c->sv == NULL)
		size = 0;
	else
		size = schema_size(c->sv);

	readlen = sizeof(vertexid_t) + size;
	buf = malloc(readlen);
	assert (buf != NULL);
	memset(buf, 0, readlen);

	for (off = 0;; off += readlen) {
		lseek(c->vfd, off, SEEK_SET);
		len = read(c->vfd, buf, readlen);
		if (len <= 0)
			break;

		id = *((vertexid_t *) buf);
		vertex_array[array_size] = id;
		array_size +=1;
		//printf("%llu", id);			
	}

	printf("Vertex Array : ");
	for (int i=0; i < array_size ; i++){
		//printf("Vertex : %d\t",vertex_array[i]);
		printf("Neighbors of vertex %d :\t",vertex_array[i]);
		/* Edges */
		if (c->se == NULL)
			size = 0;
		else
			size = schema_size(c->se);
	
		readlen = (sizeof(vertexid_t) << 1) + size;
		#if 0
			free(buf);
		#endif
		buf = malloc(readlen);
		assert (buf != NULL);
		memset(buf, 0, readlen);

		for (off = 0;; off += readlen) {
			lseek(c->efd, off, SEEK_SET);
			len = read(c->efd, buf, readlen);
			if (len <= 0)
				break;
		
			id1 = *((vertexid_t *) buf);
			id2 = *((vertexid_t *) (buf + sizeof(vertexid_t)));
			/*if (vertex_array[i] == id1){
				printf("%llu \t",id2);
			}*/
			if (c->se != NULL && with_tuples) {
			memset(&tuple, 0, sizeof(struct tuple));
			tuple.s = c->se;
			tuple.len = size;
			tuple.buf = buf + (sizeof(vertexid_t) << 1);
			i = tuple_get_int(tuple.buf);
			//add_edge(g, id1 , id2 , i);
			//printf("i - %d\t",i);			
			//tuple_print(out, &tuple, c->el);
		
			//printf("(%llu,%llu)", id1, id2);
			 int cost[N][N],i,j,w,ch,co;
    int source, target,x,y;
    printf("\t The Shortest Path Algorithm ( DIJKSTRA'S ALGORITHM in C \n\n");
    for(i=1;i< N;i++)
    for(j=1;j< N;j++)
    cost[i][j] = IN;
    for(id1=1;x< N;x++)
    {
        for(id2=x+1;y< N;y++)
        {
            //printf("Enter the weight of the path between nodes %d and %d: ",x,y);
            //scanf("%d",i);
            cost [id1][id2] = cost[id2][id1] = i;
        }
        printf("\n");
    }
	source = id1;
	target = id2;

    co = dijsktra(cost,source,target);
    printf("\nThe Shortest Path: %d",co);
		}
		}
		printf("\n");

	}
}

int dijsktra(int cost[][N],int source,int target)
{
    int dist[N],prev[N],selected[N]={0},i,m,min,start,d,j;
    char path[N];
    for(i=1;i< N;i++)
    {
        dist[i] = IN;
        prev[i] = -1;
    }
    start = source;
    selected[start]=1;
    dist[start] = 0;
    while(selected[target] ==0)
    {
        min = IN;
        m = 0;
        for(i=1;i< N;i++)
        {
            d = dist[start] +cost[start][i];
            if(d< dist[i]&&selected[i]==0)
            {
                dist[i] = d;
                prev[i] = start;
            }
            if(min>dist[i] && selected[i]==0)
            {
                min = dist[i];
                m = i;
            }
        }
        start = m;
        selected[start] = 1;
    }
    start = target;
    j = 0;
    while(start != -1)
    {
        path[j++] = start+65;
        start = prev[start];
    }
    path[j]='\0';
    strrev(path);
    printf("%s", path);
    return dist[target];
}