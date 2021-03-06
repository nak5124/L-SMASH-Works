/*****************************************************************************
 * libavsmash.h
 *****************************************************************************
 * Copyright (C) 2012-2013 L-SMASH Works project
 *
 * Authors: Yusuke Nakamura <muken.the.vfrmaniac@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *****************************************************************************/

/* This file is available under an ISC license. */

typedef struct
{
    int                 width;      /* the maximum visual presentation width */
    int                 height;     /* the maximum visual presentation height */
    uint64_t            channel_layout;
    enum AVSampleFormat sample_format;
    int                 sample_rate;
    int                 bits_per_sample;
    int                 frame_length;
    int                 upsampling;
} extended_summary_t;

typedef struct
{
    lsmash_summary_t  *summary;
    extended_summary_t extended;
} libavsmash_summary_t;

typedef struct
{
    int                   error;
    int                   update_pending;
    int                   dequeue_packet;
    uint32_t              count;
    uint32_t              index;    /* index of the current decoder configuration */
    uint32_t              delay_count;
    uint8_t              *input_buffer;
    AVCodecContext       *ctx;
    libavsmash_summary_t *entries;
    extended_summary_t    prefer;
    lw_log_handler_t      lh;
    int  (*get_buffer)( struct AVCodecContext *, AVFrame *, int );
    struct
    {
        uint32_t       index;       /* index of the queued decoder configuration */
        uint32_t       delay_count;
        uint32_t       sample_number;
        AVPacket       packet;
        enum AVCodecID codec_id;
        uint8_t       *extradata;
        int            extradata_size;
        /* Parameters stored in audio summary doesn't always tell appropriate info.
         * The followings are imported from CODEC specific extensions. */
        int sample_rate;
        int bits_per_sample;
        int channels;
    } queue;
} codec_configuration_t;

static inline uint32_t get_decoder_delay
(
    AVCodecContext *ctx
)
{
    return ctx->has_b_frames + ((ctx->active_thread_type & FF_THREAD_FRAME) ? ctx->thread_count - 1 : 0);
}

int get_summaries
(
    lsmash_root_t         *root,
    uint32_t               track_ID,
    codec_configuration_t *config
);

AVCodec *libavsmash_find_decoder
(
    codec_configuration_t *config
);

int initialize_decoder_configuration
(
    lsmash_root_t         *root,
    uint32_t               track_ID,
    codec_configuration_t *config
);

int get_sample
(
    lsmash_root_t         *root,
    uint32_t               track_ID,
    uint32_t               sample_number,
    codec_configuration_t *config,
    AVPacket              *pkt
);

void update_configuration
(
    lsmash_root_t         *root,
    uint32_t               track_ID,
    codec_configuration_t *config
);

void libavsmash_flush_buffers
(
    codec_configuration_t *config
);

void cleanup_configuration
(
    codec_configuration_t *config
);
