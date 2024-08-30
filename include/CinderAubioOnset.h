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

class CinderAubioOnset : public CinderAubioBlock {

public:
    CinderAubioOnset();
    CinderAubioOnset( const std::string &method, int bufSize, int hopSize, int sampleRate );
    ~CinderAubioOnset() override = default;
    CinderAubioOnset( const CinderAubioOnset &other ) = delete;
    CinderAubioOnset &operator=( const CinderAubioOnset &rhs ) = delete;
    CinderAubioOnset( CinderAubioOnset &&other )  noexcept;
    CinderAubioOnset &operator=( CinderAubioOnset &&other ) noexcept;

    void setThreshold( float threshold );
    float getNovelty() const;
    float getThresholdedNovelty() const;
    float getThreshold() const;
    cinder::signals::Signal<void( float )> &getGotOnsetSignal();

private:
    void blockAudioIn() override;

    using AubioOnsetPtr = std::unique_ptr<aubio_onset_t, std::function<void( aubio_onset_t * )>>;
    AubioOnsetPtr mOnset;
    float mNovelty{ 0.0f };
    float mThresholdedNovelty{ 0.0f };
    float mThreshold{ 0.0f };
    cinder::signals::Signal<void( float )> mGotOnsetSignal;
};

inline float CinderAubioOnset::getNovelty() const { return mNovelty; }
inline float CinderAubioOnset::getThresholdedNovelty() const { return mThresholdedNovelty; }
inline float CinderAubioOnset::getThreshold() const { return mThreshold; }
inline cinder::signals::Signal<void( float )> &CinderAubioOnset::getGotOnsetSignal() { return mGotOnsetSignal; }
