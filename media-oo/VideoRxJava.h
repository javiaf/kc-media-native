/*
 * (C) Copyright 2013 Kurento (http://kurento.org/)
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the GNU Lesser General Public License
 * (LGPL) version 2.1 which accompanies this distribution, and is available at
 * http://www.gnu.org/licenses/lgpl-2.1.html
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 */

#ifndef __VIDEO_RX_JAVA_H__
#define __VIDEO_RX_JAVA_H__

#include "MediaRx.h"
#include "VideoRx.h"
extern "C" {
#include <jni.h>
}

namespace media {
	class VideoRxJava : public MediaRx {
	private:
		FrameManager *_frame_manager;
		AVFrame *_pFrame;
		JNIEnv *_env;
		jobject _jDecObj;
		uint8_t *_frame_out;
		int _width,_height,_npacket;
		jintArray _frameRGB;
		jmethodID _mPutVideo;
		jclass _clDecoder;


	public:
		VideoRxJava(JNIEnv *env, MediaPort* mediaPort, const char* sdp, int max_delay,
				FrameManager *frame_manager);
		~VideoRxJava();

	private:
		jstring getJDecoderName();
		int decodeFrameJava(AVPacket *avpk);
		void initDecoderJava();
		void processFirstPacket(AVPacket avpkt, int64_t rx_time);
		void processPacketJava(AVPacket avpkt, int64_t rx_time);
		void processPacket(AVPacket avpkt, int64_t rx_time);
	};
}

#endif /* __VIDEO_RX_H__ */
