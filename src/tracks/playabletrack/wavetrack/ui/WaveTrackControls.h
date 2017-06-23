/**********************************************************************

Audacity: A Digital Audio Editor

WavelTrackControls.h

Paul Licameli split from TrackPanel.cpp

**********************************************************************/

#ifndef __AUDACITY_WAVE_TRACK_CONTROLS__
#define __AUDACITY_WAVE_TRACK_CONTROLS__

#include "../../../ui/TrackControls.h"

class WaveTrackControls final : public TrackControls
{
   WaveTrackControls(const WaveTrackControls&) = delete;
   WaveTrackControls &operator=(const WaveTrackControls&) = delete;

public:
   explicit
   WaveTrackControls( std::shared_ptr<Track> pTrack )
      : TrackControls( pTrack ) {}
   ~WaveTrackControls();

   HitTestResult HitTest
      (const TrackPanelMouseEvent &event,
       const AudacityProject *pProject) override;

   PopupMenuTable *GetMenuExtension(Track *pTrack) override;
};

#endif
