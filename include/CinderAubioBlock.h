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

#include <functional>
#include <memory>
#include <string>
#include <aubio.h>

class CinderAubioBlock 
{
public:
    CinderAubioBlock( const std::string &method, int bufSize, int hopSize, int sampleRate );
    virtual ~CinderAubioBlock() = default;
    CinderAubioBlock( const CinderAubioBlock &other ) = delete;
    CinderAubioBlock &operator=( const CinderAubioBlock &rhs ) = delete;
    CinderAubioBlock( CinderAubioBlock &&other )  noexcept;
    CinderAubioBlock &operator=( CinderAubioBlock &&other )  noexcept;

    void audioIn( float *input, int bufferSize, int nChannels );

protected:
    virtual void blockAudioIn();

    uint_t mBufSize{ 0 };
    uint_t mHopSize{ 0 };
    uint_t mCurPos{ 0 };
    using AubioFvecPtr = std::unique_ptr<fvec_t, std::function<void( fvec_t * )>>;
    AubioFvecPtr mAubioInput;
    AubioFvecPtr mAubioOutput;

    static constexpr const char * const kDefaultMethod{ "default" };
    static constexpr int kDefaultBufSize{ 512 };
    static constexpr int kDefaultHopSize{ 256 };
    static constexpr int kDefaultSampleRate{ 44100 };
};