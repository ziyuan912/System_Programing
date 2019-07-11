#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>

#define _GNU_SOURCE
#define set 300

double data[25151][35];

double tree[300][34];

struct node{
	int dimension;
	int vote;
	double threshold;
	struct node *left;
	struct node *right;
};

int compare(const void *ain,const void *bin,void *thunkin){
	
	int thunk = *((int *)thunkin);
	double a = *((double*)(ain + thunk*sizeof(double)));
	double b = *((double*)(bin + thunk*sizeof(double)));

	if(a < b)return -1;
	else if(a > b)return 1;
	else return 0;
}

struct node *maketree(int start,int num,int label1){
	int thredpos = 0,dimension = 0;
	double min = 100,threshold = 0;
	double onedown = label1,oneup = 0.0,total_impurity;
	struct node *Node = (struct node*)malloc(sizeof(struct node));
	if(label1 == num){
		//printf("yaaaaaaaa\n");
		Node->vote = 1;
		Node->left = NULL;
		Node->right = NULL;
		return Node;
	}
	else if(label1 == 0){
		//printf("ggggggggggggg\n");
		Node->vote = 0;
		Node->left = NULL;
		Node->right = NULL;
		return Node;
	}
	//printf("%d\n",label1 );
	for(int j = 0;j < 33;j ++){
		int thunk = j;
		onedown = label1,oneup = 0;
		qsort_r(tree[start],num,34*sizeof(double),compare,&thunk);
		/*for(int k = start;k < num;k ++){
			printf("%lf\n",tree[k][j]);
		}*/
		//printf("next\n");
		for(int i = 0;i < num-1;i ++){
			if(tree[start+i][33] == 1){
				oneup += 1.0;
				onedown -= 1.0;
			}
			double up,down;
			up = (double) (oneup/((double)i + 1.0))*(1.0 - oneup/((double)i + 1.0))*2.0;
			down = (double) (onedown/(num - i - 1.0))*(1.0 - onedown/(num - i - 1.0))*2.0;
			total_impurity = (double)up + down;
			//printf("%lf %lf\n",impurityup,impuritydown );
			if(total_impurity < min){
				min = total_impurity;
				threshold = (tree[start + i][j] + tree[start + i + 1][j])/2;
				dimension = j;
				thredpos = i + 1;
				//printf("%lf %lf\n",up,down );
			}
		}
	}
	//printf("%lf %lf %d %d num = %d, label1 = %d\n",min,threshold,thredpos,dimension,num,label1 );
	int thunk = dimension;
	qsort_r(&tree[start][0],num,34*sizeof(double),compare,&thunk);
	int label = 0;
	for(int i = 0;i < thredpos;i ++){
		if(tree[start + i][33] == 1)label ++;
	}
	Node->dimension = dimension;
	Node->threshold = threshold;
	//printf("%d %lf\n",dimension,threshold );
	Node->left = maketree(start,thredpos,label);
	Node->right = maketree(start + thredpos,num - thredpos,label1 - label);
	return Node;
}

int checktree(double *test ,struct node *tree){
	if(tree->left == NULL && tree->right == NULL){
		return tree->vote;
	}
	else {
		if(test[tree->dimension] < tree->threshold)
			return (checktree(test,tree->left));
		else
			return (checktree(test,tree->right));
	}
}

int main(int argc, char const *argv[])
{
	char data_dir[1000],submission[1000];
	struct node *root[1000];
	int treenum = 1,threadnum,i = 0,fd;
	srand(time(NULL));
	//sscanf(argv[2],"%s",data_dir);
	//sscanf(argv[4],"%s",submission);
	FILE *trainfile = fopen("training_data","r");
	fd = open(submission,O_WRONLY);
	//sscanf(argv[6],"%d",&treenum);
	sscanf(argv[8],"%d",&threadnum);
	while(!feof(trainfile)){
		int j = 0;
		while(j < 35){
			fscanf(trainfile,"%lf",&data[i][j]);
			j ++;
		}
		i ++;
	}
	for(int t = 0;t < treenum;t ++){
		int label1 = 0;
		for(int i = 0;i < set;i ++){
			int rand = random()%25150;
			//printf("%d\n",rand );
			for(int j = 0;j < 34;j ++){
				tree[i][j] = data[rand][j+1];
			}
			if(tree[i][33] == 1)label1 ++;
		}
		root[t] = maketree(0,set,label1);
		//printf("finish\n");
	}
	FILE *testfile = fopen("testing_data","r");
	FILE *submitfile = fopen("submission.csv","w+");
	fprintf(submitfile, "id,label\n" );
	while(!feof(testfile)){
		int j = 0,garbige;
		double test[34];
		fscanf(testfile,"%d",&garbige);
		while(j < 33){
			fscanf(testfile,"%lf",&test[j]);
			j ++;
		}
		int ans = 0;
		for(int t = 0;t < treenum;t ++){
			ans += checktree(test,root[t]);
			//printf("%d\n",ans[t]);
		}
		if(ans > treenum/2)test[33] = 1;
		else test[33] = 0;
		int answer = test[33];
		fprintf(submitfile, "%d,%d\n",garbige,answer);
	}
	return 0;
}