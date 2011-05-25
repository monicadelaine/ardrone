#ifndef _IHM_STAGES_O_GTK_H
#define _IHM_STAGES_O_GTK_H

#include <config.h>
#include <VP_Api/vp_api_thread_helper.h>



typedef  struct _vp_stages_gtk_config_ {
  int max_width;
  int max_height;
  int rowstride;
  void * last_decoded_frame_info;
}  vp_stages_gtk_config_t;

PROTO_THREAD_ROUTINE(video_stage, data);



#endif // _IHM_STAGES_O_GTK_H
