/******************************************************************************
 * Copyright (C) 2014-2020 Zhifeng Gong <gozfree@163.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/
#include "transport_session.h"
#include "media_source.h"
#include "rtp.h"
#include <liblog.h>
#include <libtime.h>
#include <libdict.h>
#include <libgevent.h>
#include <libmedia-io.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>


void *transport_session_pool_create()
{
    return (void *)dict_new();
}

void transport_session_pool_destroy(void *pool)
{
    int rank = 0;
    char *key, *val;
    while (1) {
        rank = dict_enumerate((dict *)pool, rank, &key, &val);
        if (rank < 0) {
            break;
        }
        free(val);
    }
    dict_free((dict *)pool);
}

static uint32_t get_random_number()
{
    struct timeval now = {0};
    gettimeofday(&now, NULL);
    srand(now.tv_usec);
    return (rand() % ((uint32_t)-1));
}

struct transport_session *transport_session_create(void *pool, struct transport_header *t)
{
    char key[9];
    struct transport_session *s = calloc(1, sizeof(struct transport_session));
    s->session_id = get_random_number();
    snprintf(key, sizeof(key), "%08X", s->session_id);
    s->rtp = rtp_create(90000, 0);
    s->rtp->sock = rtp_socket_create(t->mode, 0, t->source, t->destination);
    s->rtp->sock->rtp_dst_port = t->rtp.u.client_port1;
    s->rtp->sock->rtcp_dst_port = t->rtp.u.client_port2;
    dict_add((dict *)pool, key, (char *)s);
    return s;
}

void transport_session_destroy(void *pool, char *key)
{
    struct transport_session *s = transport_session_lookup(pool, key);
    if (!s) {
        return;
    }
    rtp_socket_destroy(s->rtp->sock);
    dict_del((dict *)pool, key);
}

struct transport_session *transport_session_lookup(void *pool, char *key)
{
    return (struct transport_session *)dict_get((dict *)pool, key, NULL);
}

#define MILLISECOND_DEN 1000

static int32_t get_ms_time_v(struct video_packet *packet, int64_t val)
{
    return (int32_t)(val * MILLISECOND_DEN / packet->encoder.timebase.den);
}
static void *send_thread(struct thread *t, void *ptr)
{
    struct transport_session *ts = (struct transport_session *)ptr;
    struct media_source *ms = ts->media_source;
    void *data = NULL;
    size_t len = 0;
    if (-1 == ms->open(ms, "sample.264")) {
        loge("open failed!\n");
        return NULL;
    }
    ms->is_active = true;
    unsigned int ssrc = (unsigned int)rtp_ssrc();
    uint64_t pts = time_msec();
    unsigned int seq = ssrc;
    logd("rtp send thread %s created\n", t->name);
    while (t->run) {
        if (-1 == ms->read(ms, &data, &len) || data == NULL) {
            loge("read failed!\n");
            sleep(1);
            continue;
        }
        struct media_packet *pkt = data;
        switch (pkt->type) {
        case MEDIA_TYPE_AUDIO:
            logd("MEDIA_TYPE_AUDIO\n");
            break;
        case MEDIA_TYPE_VIDEO: {
            logd("MEDIA_TYPE_VIDEO\n");
            struct video_packet *vpkt = pkt->video;
            struct rtp_packet *pkt = rtp_packet_create(RTP_PT_H264, vpkt->size, seq, ssrc);
            if (!pkt) {
                loge("rtp_packet_create failed!\n");
                break;
            }
            pts = get_ms_time_v(vpkt, vpkt->dts);
            logd("rtp_packet_create video size=%d, pts=%d\n", vpkt->size, pts);
            rtp_payload_h264_encode(ts->rtp->sock, pkt, vpkt->data, vpkt->size, pts);
            seq = pkt->header.seq;
            rtp_packet_destroy(pkt);
        } break;
        default:
            loge("unsupport media type!\n");
            break;
        }
    }
    ms->is_active = false;
    ms->close(ms);
    return NULL;
}

static void on_recv(int fd, void *arg)
{
    char buf[2048];
    memset(buf, 0, sizeof(buf));
    int ret = sock_recv(fd, buf, 2048);
    if (ret > 0) {
        rtcp_parse(buf, ret);
    } else if (ret == 0) {
        loge("delete connection fd:%d\n", fd);
    } else if (ret < 0) {
        loge("recv failed!\n");
    }
}

static void on_error(int fd, void *arg)
{
    loge("error: %d\n", errno);
}

static void *event_thread(struct thread *t, void *ptr)
{
    struct transport_session *ts = (struct transport_session *)ptr;
    gevent_base_loop(ts->evbase);
    return NULL;
}

int transport_session_start(struct transport_session *ts, struct media_source *ms)
{
    ts->evbase = gevent_base_create();
    if (!ts->evbase) {
        return -1;
    }
    sock_set_noblk(ts->rtp->sock->rtcp_fd, true);
    struct gevent *e = gevent_create(ts->rtp->sock->rtcp_fd, on_recv, NULL, on_error, NULL);
    if (-1 == gevent_add(ts->evbase, e)) {
        loge("event_add failed!\n");
        gevent_destroy(e);
    }
    ts->media_source = ms;
    ts->ev_thread = thread_create(event_thread, ts);
    thread_set_name(ts->ev_thread, "event_thread");
    ts->thread = thread_create(send_thread, ts);
    thread_set_name(ts->thread, "send_thread");
    logi("session_id = %d, name=%s\n", ts->session_id, ts->media_source->name);
    return 0;
}
