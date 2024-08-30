/*
  Copyright (C) 2015 Paul Brossier <piem@aubio.org>

  This file is ported from ofxAubio.

  ofxAubio is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  aubio is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with aubio.  If not, see <http://www.gnu.org/licenses/>.

*/

#pragma once

#include <cinder/Signals.h>
#include "CinderAubioBlock.h"

class CinderAubioBeat : public CinderAubioBlock {

public:
    CinderAubioBeat();
    CinderAubioBeat( const std::string &method, int bufSize, int hopSize, int sampleRate );
    ~CinderAubioBeat() override = default;
    CinderAubioBeat( const CinderAubioBeat &other ) = delete;
    CinderAubioBeat &operator=( const CinderAubioBeat &rhs ) = delete;
    CinderAubioBeat( CinderAubioBeat &&other )  noexcept;
    CinderAubioBeat &operator=( CinderAubioBeat &&other )  noexcept;
    cinder::signals::Signal<void( float )> &getGotBeatSignal();
    
private:
    void blockAudioIn() override;

    using AubioTempoPtr = std::unique_ptr<aubio_tempo_t, std::function<void( aubio_tempo_t * )>>;
    AubioTempoPtr mTempo;
    float mBpm{ 0.0f };
    cinder::signals::Signal<void( float )> mGotBeatSignal;
};

inline cinder::signals::Signal<void( float )> &CinderAubioBeat::getGotBeatSignal() { return mGotBeatSignal; }
