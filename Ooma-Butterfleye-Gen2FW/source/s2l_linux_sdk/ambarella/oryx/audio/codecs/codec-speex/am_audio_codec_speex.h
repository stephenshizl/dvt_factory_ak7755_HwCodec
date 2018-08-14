/*******************************************************************************
 * am_audio_codec_speex.h
 *
 * History:
 *   Jul 23, 2015 - [ypchang] created file
 *
 * Copyright (c) 2016 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella, Inc. and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/
#ifndef AM_AUDIO_CODEC_SPEEX_H_
#define AM_AUDIO_CODEC_SPEEX_H_

#include <speex.h>
#include <speex_header.h>
#include <speex_callbacks.h>
#include <speex_stereo.h>

class AMAudioCodecSpeexConfig;
struct AudioCodecSpeexConfig;

class AMAudioCodecSpeex: public AMAudioCodec
{
    typedef AMAudioCodec inherited;

  public:
    static AMIAudioCodec* create(const char *config);

  public:
    virtual void destroy();
    virtual bool initialize(AM_AUDIO_INFO *srcAudioInfo,
                            AM_AUDIO_CODEC_MODE mode);
    virtual bool finalize();
    virtual AM_AUDIO_INFO* get_codec_audio_info();
    virtual uint32_t get_codec_output_size();
    virtual bool check_encode_src_parameter(AM_AUDIO_INFO &info);
    virtual uint32_t get_encode_required_chunk_size(AM_AUDIO_INFO &info);
    /* encode: return generated bytes,
     * generated data size is also stored in out_data_size
     */
    virtual uint32_t encode(uint8_t *input,
                            uint32_t in_data_size,
                            uint8_t *output,
                            uint32_t *out_data_size);
    /* decode: return consumed bytes,
     * generated data size is stored in out_data_size.
     */
    virtual uint32_t decode(uint8_t *input,
                            uint32_t in_data_size,
                            uint8_t *output,
                            uint32_t *out_data_size);

  protected:
    AMAudioCodecSpeex();
    virtual ~AMAudioCodecSpeex();
    bool init(const char *config);

  private:
    AudioCodecSpeexConfig   *m_speex_config; /* no need to delete here */
    AMAudioCodecSpeexConfig *m_config;
    const SpeexMode         *m_speex_mode;
    void                    *m_speex_state;
    uint32_t                 m_speex_frame_size;
    uint32_t                 m_speex_frame_bytes;
    int32_t                  m_mode_id;      /* NB, WB, UWB */
    int32_t                  m_lookahead;
    int32_t                  m_speex_remain_frames;
    bool                     m_is_packet_start;
    SpeexHeader              m_speex_header;
    SpeexCallback            m_speex_callback;
    SpeexStereoState         m_speex_stereo_state;
    SpeexBits                m_speex_bits;
};


#endif /* AM_AUDIO_CODEC_SPEEX_H_ */