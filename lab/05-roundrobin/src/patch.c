/* --------------------------------------------------------------
 * File          : patch.c
 * Authors       : 西安电子科技大学:网络与信息安全国家级实验教学中心
 * Created on    : <2024-02-01>
 * Last modified : <2024-03-01>
 * LICENSE       : 由实验教学中心拥有，分发需申请并征得同意
 * -------------------------------------------------------------*/

// 以下是要修改的地方：共3处

// 1. kernel/sched/sched.h
struct rt_rq {

	struct sched_rt_entity *last_enq_se; 		// 新加字段
	int                     last_enq_prio;	// 新加字段

}

// 2. kernel/sched/rt.c
void init_rt_rq()
{
	/* ... */

	/* delimiter for bitsearch: */
	__set_bit(MAX_RT_PRIO, array->bitmap);
	// 该行以上保持不变

	/* Add for Lab 5 */
	rt_rq->last_enq_se = NULL;
	rt_rq->last_enq_prio = -1;
	/* Add for Lab 5 complete */
}



// 3. kernel/sched/rt.c
static void __enqueue_rt_entity(struct sched_rt_entity *rt_se, unsigned int flags)
{
	/* ... */

	/* Add for Lab 5 */
	struct sched_rt_entity *last_enq_se = rt_rq->last_enq_se;
	int last_enq_prio = rt_rq->last_enq_prio;
	/* Add for Lab 5 complete */

	if (group_rq && (rt_rq_throttled(group_rq) || !group_rq->rt_nr_running)) {
		if (rt_se->on_list)
			__delist_rt_entity(rt_se, array);
		return;
	}

	if (move_entity(flags)) {
		WARN_ON_ONCE(rt_se->on_list);
		if (flags & ENQUEUE_HEAD)
			list_add(&rt_se->run_list, queue);
		else {
			/* Change for Lab 5*/
			if (last_enq_se != NULL && last_enq_se->on_list == 1 && last_enq_prio == rt_se_prio(rt_se)) {
				list_add(&rt_se->run_list, &last_enq_se->run_list);
			}
			else {
				list_add_tail(&rt_se->run_list, queue);
			}
		} /* Change for Lab 5 complete */

		__set_bit(rt_se_prio(rt_se), array->bitmap);
		rt_se->on_list = 1;
	}
	rt_se->on_rq = 1;

	/* Add for Lab 5 */
	rt_rq->last_enq_se = rt_se;
    rt_rq->last_enq_prio = rt_se_prio(rt_se);
	/* Add for Lab 5 complete */

}
