/***

Copyright (C) 2015, 2016, 2017 Teclib'

This file is part of Armadito core.

Armadito core is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Armadito core is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Armadito core.  If not, see <http://www.gnu.org/licenses/>.

***/

#include "armadito-config.h"

#include <libarmadito/armadito.h>

#include "core/event.h"

#include <time.h>
#include <stdlib.h>
#include <string.h>

struct callback_entry {
	enum a6o_event_type mask;
	a6o_event_cb_t cb;
	void *data;
	struct callback_entry *next;
};

struct a6o_event_source {
	struct callback_entry *callbacks;
};

static void detection_event_clone(struct a6o_detection_event *dst, const struct a6o_detection_event *src)
{
	dst->context = src->context;
	dst->scan_id = src->scan_id;
	dst->path = strdup(src->path);
	dst->scan_status = src->scan_status;
	dst->scan_action = src->scan_action;
	dst->module_name = strdup(src->module_name);
	dst->module_report = strdup(src->module_report);
}

static void on_demand_start_event_clone(struct a6o_on_demand_start_event *dst, const struct a6o_on_demand_start_event *src)
{
	dst->root_path = strdup(src->root_path);
	dst->scan_id = src->scan_id;
}

static void on_demand_completed_event_clone(struct a6o_on_demand_completed_event *dst, const struct a6o_on_demand_completed_event *src)
{
	dst->scan_id = src->scan_id;
	dst->cancelled = src->cancelled;
	dst->total_malware_count = src->total_malware_count;
	dst->total_suspicious_count = src->total_suspicious_count;
	dst->total_scanned_count = src->total_scanned_count;
	dst->duration = src->duration;
}

static void on_demand_progress_event_clone(struct a6o_on_demand_progress_event *dst, const struct a6o_on_demand_progress_event *src)
{
	dst->scan_id = src->scan_id;
	dst->progress = src->progress;
	dst->path = strdup(src->path);
	dst->malware_count = src->malware_count;
	dst->suspicious_count = src->suspicious_count;
	dst->scanned_count = src->scanned_count;
}

static void quarantine_event_clone(struct a6o_quarantine_event *dst, const struct a6o_quarantine_event *src)
{
	dst->quarantine_action = src->quarantine_action;
	dst->orig_path = strdup(src->orig_path);
	dst->quarantine_path = strdup(src->quarantine_path);
}

static void real_time_prot_event_clone(struct a6o_real_time_prot_event *dst, const struct a6o_real_time_prot_event *src)
{
	dst->rt_prot_new_state = src->rt_prot_new_state;
}

struct a6o_event *a6o_event_new(enum a6o_event_type ev_type, void *ev)
{
	struct a6o_event *e = malloc(sizeof(struct a6o_event));

	e->timestamp = time(NULL);
	e->type = ev_type;

	switch(e->type) {
	case EVENT_DETECTION:
		detection_event_clone(&e->u.ev_detection, (struct a6o_detection_event *)ev);
		break;
	case EVENT_ON_DEMAND_START:
		on_demand_start_event_clone(&e->u.ev_on_demand_start, (struct a6o_on_demand_start_event *)ev);
		break;
	case EVENT_ON_DEMAND_COMPLETED:
		on_demand_completed_event_clone(&e->u.ev_on_demand_completed, (struct a6o_on_demand_completed_event *)ev);
		break;
	case EVENT_ON_DEMAND_PROGRESS:
		on_demand_progress_event_clone(&e->u.ev_on_demand_progress, (struct a6o_on_demand_progress_event *)ev);
		break;
	case EVENT_QUARANTINE:
		quarantine_event_clone(&e->u.ev_quarantine, (struct a6o_quarantine_event *)ev);
		break;
	case EVENT_REAL_TIME_PROT:
		real_time_prot_event_clone(&e->u.ev_real_time_prot, (struct a6o_real_time_prot_event *)ev);
		break;
	case EVENT_AV_UPDATE:
		break;
	}

	return e;
}

static void detection_event_free(struct a6o_detection_event *e)
{
	free((void *)e->path);
	free((void *)e->module_name);
	free((void *)e->module_report);
}

static void on_demand_start_event_free(struct a6o_on_demand_start_event *e)
{
	free((void *)e->root_path);
}

static void on_demand_progress_event_free(struct a6o_on_demand_progress_event *e)
{
	free((void *)e->path);
}

static void quarantine_event_free(struct a6o_quarantine_event *e)
{
	free((void *)e->orig_path);
	free((void *)e->quarantine_path);
}

void a6o_event_free(struct a6o_event *ev)
{
	switch(ev->type) {
	case EVENT_DETECTION:
		detection_event_free(&ev->u.ev_detection);
		break;
	case EVENT_ON_DEMAND_START:
		on_demand_start_event_free(&ev->u.ev_on_demand_start);
		break;
	case EVENT_ON_DEMAND_COMPLETED:
		break;
	case EVENT_ON_DEMAND_PROGRESS:
		on_demand_progress_event_free(&ev->u.ev_on_demand_progress);
		break;
	case EVENT_QUARANTINE:
		quarantine_event_free(&ev->u.ev_quarantine);
		break;
	case EVENT_REAL_TIME_PROT:
		break;
	case EVENT_AV_UPDATE:
		break;
	}

	free(ev);
}

struct a6o_event_source *a6o_event_source_new(void)
{
	struct a6o_event_source *s = malloc(sizeof(struct a6o_event_source));

	s->callbacks = NULL;

	return s;
}

void a6o_event_source_free(struct a6o_event_source *s)
{
	struct callback_entry *p;
	struct callback_entry *next;

	for (p = s->callbacks; p != NULL; p = next) {
		next = p->next;
		free((void *)p);
	}

	free((void *)s);
}

void a6o_event_source_add_cb(struct a6o_event_source *s, enum a6o_event_type ev_mask, a6o_event_cb_t cb, void *data)
{
	struct callback_entry *p = malloc(sizeof(struct callback_entry));

	p->mask = ev_mask;
 	p->cb = cb;
	p->data = data;
	p->next = s->callbacks;

	s->callbacks = p;
}

void a6o_event_source_remove_cb(struct a6o_event_source *s, enum a6o_event_type ev_mask, a6o_event_cb_t cb, void *data)
{
	struct callback_entry **p = &s->callbacks;

	while (*p != NULL) {
		if ((*p)->mask == ev_mask && (*p)->cb == cb && (*p)->data == data) {
			struct callback_entry *to_free = *p;
			struct callback_entry *next = (*p)->next;

			free(to_free);
			*p = next;
		}
		else
			p = &(*p)->next;
	}
}

/*
  must add a threaded event firing:
  listeners list can be updated while events are fired
  => must lock the listener list
  1) locking from the event source thread: may delay scan threads
  2) locking from another thread dedicated to event firing: needs
  an async queue between the source thread and the event thread
 */
void a6o_event_source_fire_event(struct a6o_event_source *s, struct a6o_event *ev)
{
	struct callback_entry *p;

	for (p = s->callbacks; p != NULL; p = p->next)
		if (p->mask & ev->type)
			(*p->cb)(ev, p->data);
}

