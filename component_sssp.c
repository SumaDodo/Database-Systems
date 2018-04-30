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

#define MAX_SIZE 100


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
 
/*typedef struct {
    int vertex;
    int weight;
} dedge_t;
 
typedef struct {
    edge_t **edges;
    int edges_len;
    int edges_size;
    int dist;
    int prev;
    int visited;
} dvertex_t;
 
typedef struct {
    vertex_t **vertices;
    int vertices_len;
    int vertices_size;
} dgraph_t;
 
typedef struct {
    int *data;
    int *prio;
    int *index;
    int len;
    int size;
} dheap_t;*/

dgraph_t *g;

int
component_sssp(
        char *grdbdir,
        int gidx,
        int cidx,
        vertexid_t id1,
        vertexid_t id2,
        char *weight_field)
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
	
	sprintf(gname,"%d",gidx);
	sprintf(cname,"%d",cidx);

	//printf("Gname - %s\n",gname);
	//printf("Cname - %s\n",cname);

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
	sssp_read_edges_schema(&c, with_tuples);

	/*Dijikstra function call */
	dijkstra(g, id1, id2);
	print_path(g, id2); 
       

	close(c.efd);
	close(c.vfd);
	/* Change this as needed */
	return (-1);
}


void sssp_read_edges_schema(component_t c, int with_tuples){

	off_t off;
	ssize_t len, size;
	vertexid_t id, id1, id2;
	struct tuple tuple;
	char *buf;
	int readlen;
	assert (c != NULL);
	int vertex_array[MAX_SIZE];
	int array_size = 0;


	/*Initialize graph for heap*/
	g = calloc(1, sizeof (graph_t));

	/* Edges */
	int i;
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
				
		
		if (c->se != NULL && with_tuples) {
			memset(&tuple, 0, sizeof(struct tuple));
			tuple.s = c->se;
			tuple.len = size;
			tuple.buf = buf + (sizeof(vertexid_t) << 1);
			i = tuple_get_int(tuple.buf);
			add_edge(g, id1 , id2 , i);
			printf("i - %d\t",i);			
			//tuple_print(out, &tuple, c->el);
		}
	}

#if 0
	free(buf);
#endif
}

/*int main()
{
    int G[MAX][MAX],i,j,n,u;
    printf("Enter no. of vertices:");
    scanf("%d",&n);
    printf("\nEnter the adjacency matrix:\n");
    
    for(i=0;i<n;i++)
        for(j=0;j<n;j++)
            scanf("%d",&G[i][j]);
    
    printf("\nEnter the starting node:");
    scanf("%d",&u);
    dijkstra(G,n,u);
    
    return 0;
}*/
 