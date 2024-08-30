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

#include "CinderAubioPitch.h"
#include <cinder/app/App.h>


CinderAubioPitch::CinderAubioPitch()
    : CinderAubioPitch( kDefaultMethod, kDefaultPitchBufSize, kDefaultHopSize, kDefaultSampleRate )
{

}

CinderAubioPitch::CinderAubioPitch( const std::string &method, int bufSize, int hopSize, int sampleRate )
    : CinderAubioBlock( method, bufSize, hopSize, sampleRate )
{
    mPitch = AubioPitchPtr( new_aubio_pitch( method.c_str(), bufSize, hopSize, sampleRate ),
        [] ( aubio_pitch_t *p ) { del_aubio_pitch( p ); } );
    aubio_pitch_set_unit( mPitch.get(), "midi" );
    //aubio_pitch_set_tolerance(pitch, 0.7);
    if( mPitch ) {
        ci::app::console() << "created CinderAubioPitch(" << method
            << ", " << mBufSize
            << ", " << mHopSize
            << ", " << sampleRate
            << ")";
    }
}

CinderAubioPitch::CinderAubioPitch( CinderAubioPitch &&other ) noexcept
    : CinderAubioBlock( std::move( other ) )
{
    mPitch = std::move( other.mPitch );
}

CinderAubioPitch &CinderAubioPitch::operator=( CinderAubioPitch &&rhs ) noexcept
{
    if( this != &rhs )
    {
        CinderAubioBlock::operator=( std::move( rhs ) );
        mPitch = std::move( rhs.mPitch );
    }
    return *this;
}

void CinderAubioPitch::blockAudioIn()
{
    aubio_pitch_do( mPitch.get(), mAubioInput.get(), mAubioOutput.get());
    //ci::app::console() << "found pitch: " << aubio_output->data[0];
    mPitchConfidence = aubio_pitch_get_confidence( mPitch.get() );
    mLatestPitch = mAubioOutput->data[0];
}
