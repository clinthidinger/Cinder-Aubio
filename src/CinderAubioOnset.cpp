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

#include "CinderAubioOnset.h"
#include <cinder/app/App.h>

//ofEvent<float> CinderAubioOnset::gotGlobalOnset = ofEvent<float>();


CinderAubioOnset::CinderAubioOnset()
    : CinderAubioOnset( kDefaultMethod, kDefaultBufSize, kDefaultHopSize, kDefaultSampleRate )
{

}

CinderAubioOnset::CinderAubioOnset( const std::string &method, int bufSize, int hopSize, int sampleRate )
    : CinderAubioBlock( method, bufSize, hopSize, sampleRate )
{
    mOnset = AubioOnsetPtr( new_aubio_onset( method.c_str(), bufSize, hopSize, sampleRate ),
        [] ( aubio_onset_t *o ) {del_aubio_onset( o ); } );
    if( mOnset ) {
        mThreshold = aubio_onset_get_threshold( mOnset.get() );
        ci::app::console() << "created CinderAubioOnset(" << method
            << ", " << mBufSize
            << ", " << mHopSize
            << ", " << sampleRate
            << ")";
    }
}

CinderAubioOnset::CinderAubioOnset( CinderAubioOnset &&other ) noexcept
    : CinderAubioBlock( std::move( other ) )
{
    mOnset = std::move( other.mOnset );
}

CinderAubioOnset &CinderAubioOnset::operator=( CinderAubioOnset &&rhs ) noexcept
{
    if( this != &rhs )
    {
        CinderAubioBlock::operator=( std::move( rhs ) );
        mOnset = std::move( rhs.mOnset );
    }
    return *this;
}

void CinderAubioOnset::blockAudioIn()
{
    aubio_onset_do( mOnset.get(), mAubioInput.get(), mAubioOutput.get());
    if( mAubioOutput->data[0] ) {
        //ci::app::console() << "found onset";
        float lastOnset = aubio_onset_get_last_s( mOnset.get() );
        mGotOnsetSignal.emit( lastOnset );// , this );
        //ofNotifyEvent( gotGlobalOnset, last_onset );
    }
    mNovelty = aubio_onset_get_descriptor( mOnset.get() );
    mThresholdedNovelty = aubio_onset_get_thresholded_descriptor( mOnset.get() );
}

void CinderAubioOnset::setThreshold( float newThreshold )
{
    aubio_onset_set_threshold( mOnset.get(), newThreshold);
    mThreshold = newThreshold;
}
