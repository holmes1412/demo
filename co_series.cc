#include <ucontext.h> 
#include <stdio.h>
#include <errno.h>

#include <stdlib.h>
#include <string.h>
#include <string>

#include "workflow/Workflow.h"
#include "workflow/WFTaskFactory.h"

#include <unistd.h>

using REQ = std::string;
using RESP = std::string;
using STATE = std::string;

struct tutorial_series_context
{
	REQ *req;
	RESP *resp;
	STATE *state;
	ucontext_t *user_ctx;
};

void start_task(REQ *req, RESP *resp, STATE *state, ucontext_t *user_ctx)
{
	printf("start_task()\n");

	auto *task = WFTaskFactory::create_timer_task(3 * 1000 * 1000, [](WFTimerTask *task) {
		printf("task callback\n");
		struct tutorial_series_context *series_ctx;
		series_ctx = (struct tutorial_series_context *)series_of(task)->get_context();

		if (task->get_state() == WFT_STATE_SUCCESS)
			*(series_ctx->state) = "success";
		else
			*(series_ctx->state) = "error";

		*(series_ctx->resp) = *(series_ctx->req); // simple for test
		printf("ready to setcontext()!!!\n");
//		setcontext(series_ctx->user_ctx);
		auto *go_task = WFTaskFactory::create_go_task("go", setcontext, series_ctx->user_ctx);
		go_task->start();
	});

	SeriesWork *series = Workflow::create_series_work(task, [](const SeriesWork *series) {
		printf("series callback\n");
		delete (struct tutorial_series_context *)series->get_context();
	});

	struct tutorial_series_context *series_ctx = new tutorial_series_context();
	series_ctx->req = req;
	series_ctx->resp = resp;
	series_ctx->state = state;
	series_ctx->user_ctx = user_ctx;
	series->set_context(series_ctx);
	series->start();

	pause(); // TODO: should not pause(), just to hold the stack context.
}

void sync_request(REQ *req, RESP *resp, STATE *state)
{
	char stack[10 * 1024]; // too small will segment fault

	ucontext_t cur_ctx;
	if (getcontext(&cur_ctx) < 0)
	{
		perror("getcontext ");
		*state = "getcontext() error";
		return;
	}

	ucontext_t new_ctx = cur_ctx;
	new_ctx.uc_stack.ss_sp = stack;
	new_ctx.uc_stack.ss_size = sizeof(stack);
	new_ctx.uc_stack.ss_flags = 0;

	makecontext(&new_ctx, (void (*)(void))start_task, 4, req, resp, state, &cur_ctx);

	perror("makecontext ");
//	if (!errno)
		swapcontext(&cur_ctx, &new_ctx);

	printf("continue\n");
}

int main()
{
	REQ http_req = "http";
	RESP http_resp;
	STATE sync_state;

	sync_request(&http_req, &http_resp, &sync_state);
	printf("finish HTTP sync_request() state=%s resp=%s\n",
		   sync_state.c_str(), http_resp.c_str());

	REQ redis_req = "redis";
	RESP redis_resp;
	sync_request(&redis_req, &redis_resp, &sync_state);
	printf("finish REDIS sync_request() state=%s resp=%s\n",
		   sync_state.c_str(), redis_resp.c_str());

	return 0;
}
