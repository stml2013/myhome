#include <stdio.h>
#include <stdlib.h>
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_conf_file.h>
#include <nginx.h>
#include <ngx_queue.h>

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//这两个东东必须写，不为有编译错误
volatile ngx_cycle_t  *ngx_cycle;

void ngx_log_error_core(ngx_uint_t level, ngx_log_t *log, ngx_err_t err,
            const char *fmt, ...)
{
}
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

typedef struct
{
	ngx_int_t num;
	ngx_str_t str;
	ngx_queue_t queue;
}TestNode;

ngx_int_t compare_node(const ngx_queue_t *left, const ngx_queue_t *right)
{
	TestNode* left_node  = ngx_queue_data(left, TestNode, queue);
	TestNode* right_node = ngx_queue_data(right, TestNode, queue);

	return left_node->num > right_node->num;
}


int main()
{
    ngx_queue_t QueHead;
	ngx_queue_init(&QueHead);

	TestNode Node[10];
	ngx_int_t i;
	for (i=0; i<10; ++i)
	{
		Node[i].num = rand()%100;
	}

    ngx_queue_insert_head(&QueHead, &Node[0].queue);
	ngx_queue_insert_tail(&QueHead, &Node[1].queue);
	ngx_queue_insert_after(&QueHead, &Node[2].queue);
    ngx_queue_insert_head(&QueHead, &Node[4].queue);
	ngx_queue_insert_tail(&QueHead, &Node[3].queue);
    ngx_queue_insert_head(&QueHead, &Node[5].queue);
	ngx_queue_insert_tail(&QueHead, &Node[6].queue);
	ngx_queue_insert_after(&QueHead, &Node[7].queue);
    ngx_queue_insert_head(&QueHead, &Node[8].queue);
	ngx_queue_insert_tail(&QueHead, &Node[9].queue);

	ngx_queue_t *q;
	for (q = ngx_queue_head(&QueHead); q != ngx_queue_sentinel(&QueHead); q = ngx_queue_next(q))
	{
		TestNode* Node = ngx_queue_data(q, TestNode, queue);
		printf("Num=%d\n", Node->num);
	}

    ngx_queue_sort(&QueHead, compare_node);

    printf("\n");
	for (q = ngx_queue_head(&QueHead); q != ngx_queue_sentinel(&QueHead); q = ngx_queue_next(q))
	{
		TestNode* Node = ngx_queue_data(q, TestNode, queue);
		printf("Num=%d\n", Node->num);
	}

	return 0;
}
