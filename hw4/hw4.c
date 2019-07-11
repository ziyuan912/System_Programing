#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
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
#include <pthread.h>

#define set 100

double data[25151][35];
struct node *root[1000];
unsigned int seed[1000];

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

struct node *maketree(int start,int num,int label1,double tree[set][34]){
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
	Node->left = maketree(start,thredpos,label,tree);
	Node->right = maketree(start + thredpos,num - thredpos,label1 - label,tree);
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

void* thr_tree(void *ptr){
	int t = (int)ptr;
	int label1 = 0;
	double tree[set][34];
	for(int i = 0;i < set;i ++){
		int rand;
		rand = random()%25150;
		//printf("%d\n",rand );
		for(int j = 0;j < 34;j ++){
			tree[i][j] = data[rand][j+1];
		}
		if(tree[i][33] == 1)label1 ++;
	}
	root[t] = maketree(0,set,label1,tree);

}

int main(int argc, char const *argv[])
{
	char trainingdata[1000],testingdata[1000],submission[1000];
	int treenum,threadnum,i = 0,fd;
	srand(time(NULL));
	double START,END;
	START = clock();
	sscanf(argv[2],"%s",trainingdata);
	strcpy(trainingdata + strlen(trainingdata)*sizeof(char),"/training_data");
	sscanf(argv[2],"%s",testingdata);
	strcpy(testingdata + strlen(testingdata)*sizeof(char),"/testing_data");
	sscanf(argv[4],"%s",submission);
	printf("trainingdata:%s\ntestingdata:%s\nsubmission:%s\n",trainingdata,testingdata,submission );
	FILE *trainfile = fopen(trainingdata,"r");
	fd = open(submission,O_WRONLY);
	sscanf(argv[6],"%d",&treenum);
	pthread_t tid[treenum];

	sscanf(argv[8],"%d",&threadnum);
	/*for(int i = 0;i < threadnum;i ++){
		seed[i] = rand();
	}*/
	//printf("treenum = %d threadnum = %d\n",treenum,threadnum );
	//printf("%s %s %d!!!!\n",argv[1],argv[0],argc );
	while(!feof(trainfile)){
		int j = 0;
		while(j < 35){
			fscanf(trainfile,"%lf",&data[i][j]);
			j ++;
		}
		i ++;
	}
	int tre[10000],t;
	//printf("treenum = %d threadnum = %d\n",treenum,threadnum );
	for(t = 0;t < treenum;t ++){
		//printf("treenum = %d threadnum = %d %d\n",treenum,threadnum,t );
		if(t < threadnum){
			tre[t] = t;
			pthread_create(&tid[t],NULL,thr_tree,(void*) tre[t]);
			//printf("finish\n");
		}
		else break;
	}
	//printf("%d %d %d\n",t,treenum,threadnum );
	while(t < treenum){
		int ret = 1,i;
		for(i = 0;i < threadnum;i ++){
			//printf("pending %d\n",i );
			ret = pthread_tryjoin_np(tid[i],NULL);
			if(ret == 0){
				//printf("ready %d\n",t);
				break;
			}
		}
		if(ret == 0){
			tre[t] = t;
			pthread_create(&tid[i],NULL,thr_tree,(void*)tre[t]);
			t ++;
		}
	}
	if(threadnum > treenum)threadnum = treenum;
	for(int i = 0;i < threadnum;i ++){
		pthread_join(tid[i],NULL);
	}
	//END = clock();
	//printf("%lf\n",(double)(END-START)/CLOCKS_PER_SEC );
	FILE *testfile = fopen(testingdata,"r");
	FILE *submitfile = fopen(submission,"w+");
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