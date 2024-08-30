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

#include <string>
#include <aubio.h>
#include "CinderAubioBlock.h"

class CinderAubioPitch : public CinderAubioBlock {

public:
    CinderAubioPitch();
    CinderAubioPitch( const std::string &method, int bufSize, int hopSize, int samplerate );
    ~CinderAubioPitch() override = default;
    CinderAubioPitch( const CinderAubioPitch &other ) = delete;
    CinderAubioPitch &operator=( const CinderAubioPitch &rhs ) = delete;
    CinderAubioPitch( CinderAubioPitch &&other ) noexcept;
    CinderAubioPitch &operator=( CinderAubioPitch &&other ) noexcept;

    float getLatestPitch() const;
    float getPitchConfidence() const;

protected:
    void blockAudioIn() override;

    using AubioPitchPtr = std::unique_ptr<aubio_pitch_t, std::function<void( aubio_pitch_t * )>>;
    AubioPitchPtr mPitch;
    float mLatestPitch{ 0.0f };
    float mPitchConfidence{ 0.0f };
    static constexpr int kDefaultPitchBufSize{ 2048 };
};

inline float CinderAubioPitch::getLatestPitch() const { return mLatestPitch; }
inline float CinderAubioPitch::getPitchConfidence() const { return mPitchConfidence; }
