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

#include "CinderAubioBeat.h"
#include <cinder/app/App.h>

CinderAubioBeat::CinderAubioBeat()
    : CinderAubioBeat( kDefaultMethod, kDefaultBufSize, kDefaultHopSize, kDefaultSampleRate )
{
    
}

CinderAubioBeat::CinderAubioBeat( const std::string &method, int bufSize, int hopSize, int sampleRate )
    : CinderAubioBlock( method, bufSize, hopSize, sampleRate )
{
    mTempo = AubioTempoPtr( new_aubio_tempo( method.c_str(), mBufSize, mHopSize, sampleRate ),
        [] ( aubio_tempo_t *t ) { del_aubio_tempo( t ); } );
    constexpr smpl_t kSilence = -90;//-40;
    aubio_tempo_set_silence( mTempo.get(), kSilence );
    if( mTempo ) {
        ci::app::console() << "created CinderAubioBeat(" << method
            << ", " << mBufSize
            << ", " << mHopSize
            << ", " << sampleRate
            << ")";
    }
}

CinderAubioBeat::CinderAubioBeat( CinderAubioBeat &&other ) noexcept
    : CinderAubioBlock( std::move( other ) )
{
    mTempo = std::move( other.mTempo );
}

CinderAubioBeat &CinderAubioBeat::operator=( CinderAubioBeat &&rhs ) noexcept
{
    if( this != &rhs )
    {
        CinderAubioBlock::operator=( std::move( rhs ) );
        mTempo = std::move( rhs.mTempo );
    }
    return *this;
}

void CinderAubioBeat::blockAudioIn()
{
    aubio_tempo_do( mTempo.get(), mAubioInput.get(), mAubioOutput.get() );
    if( mAubioOutput->data[0] ) {
        //ci::app::console() << "found beat: " << aubio_output->data[0];
        mBpm = aubio_tempo_get_bpm( mTempo.get() );
        float lastBeat = aubio_tempo_get_last_s( mTempo.get() );
        mGotBeatSignal.emit( lastBeat );// , this );
        //ofNotifyEvent( gotGlobalBeat, last_beat );
    }
}
