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

#include "VideoRxJava.h"


extern "C" {
#include <jni.h>
#include "libswscale/swscale.h"
}

static int SWS_FLAGS = SWS_BICUBIC;

using namespace media;
VideoRxJava::VideoRxJava(JNIEnv *env, MediaPort* mediaPort, const char* sdp, int max_delay, FrameManager *frame_manager)
: MediaRx(mediaPort, sdp, max_delay, CODEC_TYPE_VIDEO)
{
	_frame_manager = frame_manager;
	_pFrame = avcodec_alloc_frame();
	_env = env;
	_npacket = 0;
	_clDecoder = _env->FindClass("com/kurento/kas/media/rx/Decoder");
	_mPutVideo = _env->GetMethodID(_clDecoder, "putVideoFrame", "([B)[I");

	LOG_TAG = "media-video-rx";
}

VideoRxJava::~VideoRxJava()
{
	av_free(_pFrame);
	_frame_manager->release_decoded_frame();
}

int
VideoRxJava::decodeFrameJava(AVPacket *avpk){
	jmethodID mPutVideo;
	jbyteArray frameIn, globalIn;
	jintArray frameRGB;
	int outSize;

	frameIn = _env->NewByteArray(avpk->size);
	_env->SetByteArrayRegion(frameIn, 0, avpk->size, (jbyte*) avpk->data);
	globalIn = (jbyteArray)_env->NewGlobalRef(frameIn);
	_env->DeleteLocalRef(frameIn);
	frameRGB = (jintArray) _env->CallObjectMethod(_jDecObj, _mPutVideo, globalIn);
	outSize = _env->GetArrayLength(frameRGB);
	_frameRGB = (jintArray)_env->NewGlobalRef(frameRGB);
	_env->DeleteLocalRef(frameRGB);
	/*_env->ReleaseByteArrayElements(frameIn, (jbyte*) avpk->data, JNI_ABORT);
	_env->DeleteLocalRef(frameIn);
*/
	return outSize;

}

jstring
VideoRxJava::getJDecoderName(){
	jstring jcodec;
	char *codec;
	switch(_pDecodecCtx->codec_id){
	case CODEC_ID_H264:
		codec = "video/avc";
		break;
	case CODEC_ID_H263:
		codec = "video/3gpp";
		break;
	case CODEC_ID_MPEG4:
		codec = "video/mp4v-es";
		break;
	default:
		codec = "";
		break;

	}

	jcodec = _env->NewStringUTF(codec);
	return jcodec;
}

void
VideoRxJava::initDecoderJava(){
	jobject localObj;
	jmethodID constructor;
	jstring mimetype;

	mimetype = getJDecoderName();
	constructor = _env->GetMethodID(_clDecoder, "<init>", "(Ljava/lang/String;II)V");
	localObj = _env->NewObject(_clDecoder, constructor, mimetype, _width, _height);
	_jDecObj = _env->NewGlobalRef(localObj);
	_env->DeleteLocalRef(localObj);
}



void
VideoRxJava::processPacketJava(AVPacket avpkt, int64_t rx_time)
{
	int len, got_picture;
	DecodedFrame *decoded_frame;
	struct SwsContext *img_convert_ctx;

	//Is this a avpkt from the video stream?
	if (avpkt.stream_index != _stream)
		return;

	while (avpkt.size > 0) {
		len = decodeFrameJava(&avpkt);
		if (len < 0) {
			media_log(MEDIA_LOG_ERROR, LOG_TAG, "Error in video decoding");
			break;
		}

		//Did we get a video frame?
		if(len > 1){
			decoded_frame = _frame_manager->get_decoded_frame(
						_width, _height);
			if (!decoded_frame)
				break;


			decoded_frame->priv_data = (void*)_frameRGB;
			decoded_frame->width = _width;
			decoded_frame->height = _height;
			decoded_frame->time_base = _pFormatCtx->streams[_stream]->time_base;
			decoded_frame->pts = avpkt.pts;
			decoded_frame->start_time = _pFormatCtx->streams[_stream]->start_time;
			decoded_frame->rx_time = rx_time;
			decoded_frame->encoded_size = len;

			_frame_manager->put_video_frame_rx(decoded_frame);


			}
/*		jint *pointer = (_env->GetIntArrayElements(_frameRGB, JNI_FALSE));
		_env->ReleaseIntArrayElements(_frameRGB, pointer, JNI_ABORT);
		_env->DeleteLocalRef(_frameRGB);*/
		avpkt.size -= len;
		avpkt.data += len;
		}


}


void
VideoRxJava::processPacket(AVPacket avpkt, int64_t rx_time)
{
	media_log(MEDIA_LOG_DEBUG, LOG_TAG, "Decode Frame");
	int width, height;

	_width = _pDecodecCtx->width;
	_height = _pDecodecCtx->height;
	if(_npacket == 0){
		initDecoderJava();
		_npacket++;
	}
	processPacketJava(avpkt,rx_time);

}

